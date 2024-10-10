/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 North Carolina State University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Scott E. Carpenter <scarpen@ncsu.edu>
 *
 */

#include "ns3/bsm-application.h"
#include "ns3/log.h"
#include "ns3/wave-net-device.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wave-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/mobility-helper.h"

#include "bsm-timetag.h"
#include "ns3/AperiodicTag.h"


NS_LOG_COMPONENT_DEFINE ("BsmApplication");

namespace ns3 {

// (Arbitrary) port for establishing socket to transmit WAVE BSMs
int BsmApplication::wavePort = 9080;

NS_OBJECT_ENSURE_REGISTERED (BsmApplication);

TypeId
BsmApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BsmApplication")
    .SetParent<Application> ()
    .SetGroupName ("Wave")
    .AddConstructor<BsmApplication> ()
    ;
  return tid;
}

BsmApplication::BsmApplication ()
  : m_waveBsmStats (0),
    m_txSafetyRangesSq (),
    m_TotalSimTime (Seconds (10)),
    m_wavePacketSize (200),
    m_numWavePackets (1),
    m_waveInterval (MilliSeconds (100)),
    m_gpsAccuracyNs (10000),
    m_adhocTxInterfaces (0),
    m_nodesMoving (0),
    m_unirv (0),
    m_nodeId (0),
    m_chAccessMode (0),
    m_txMaxDelay (MilliSeconds (10)),
    m_prevTxDelay (MilliSeconds (0))
{
  NS_LOG_FUNCTION (this);
}

BsmApplication::~BsmApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
BsmApplication::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  // chain up
  Application::DoDispose ();
}

// Application Methods
void BsmApplication::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  // setup generation of WAVE BSM messages
  Time waveInterPacketInterval = m_waveInterval;

  // BSMs are not transmitted for the first second
  Time startTime = Seconds (1.0);
  // total length of time transmitting WAVE packets
  Time totalTxTime = m_TotalSimTime - startTime;
  // total WAVE packets needing to be sent
  m_numWavePackets = (uint32_t) (totalTxTime.GetDouble () / m_waveInterval.GetDouble ());

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

  // Create two sockets for TDMA and CSMA devices
    tdmaDevice = DynamicCast<WifiNetDevice>(m_node->GetDevice(0)); // TDMA设备
    csmaDevice = DynamicCast<WifiNetDevice>(m_node->GetDevice(1)); // CSMA设备

    tdmaSocket = Socket::CreateSocket(m_node, tid);
    csmaSocket = Socket::CreateSocket(m_node, tid);

    // Bind TDMA and CSMA sockets to their respective devices
    tdmaSocket->BindToNetDevice(tdmaDevice);
    csmaSocket->BindToNetDevice(csmaDevice);

    // Set up receive callbacks for both sockets
    tdmaSocket->SetRecvCallback(MakeCallback(&BsmApplication::ReceiveWavePacket, this));
    csmaSocket->SetRecvCallback(MakeCallback(&BsmApplication::ReceiveWavePacket, this));

    // Bind the sockets to addresses and allow broadcast
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), wavePort);
    tdmaSocket->Bind(local);
    tdmaSocket->SetAllowBroadcast(true);

    csmaSocket->Bind(local);
    csmaSocket->SetAllowBroadcast(true);

    // Set up the destination address (broadcast) for each socket
    InetSocketAddress remote = InetSocketAddress(Ipv4Address("255.255.255.255"), wavePort);
    tdmaSocket->Connect(remote);
    csmaSocket->Connect(remote);


  // Transmission start time for each BSM:
  // We assume that the start transmission time
  // for the first packet will be on a ns-3 time
  // "Second" boundary - e.g., 1.0 s.
  // However, the actual transmit time must reflect
  // additional effects of 1) clock drift and
  // 2) transmit delay requirements.
  // 1) Clock drift - clocks are not perfectly
  // synchronized across all nodes.  In a VANET
  // we assume all nodes sync to GPS time, which
  // itself is assumed  accurate to, say, 40-100 ns.
  // Thus, the start transmission time must be adjusted
  // by some value, t_drift.
  // 2) Transmit delay requirements - The US
  // minimum performance requirements for V2V
  // BSM transmission expect a random delay of
  // +/- 5 ms, to avoid simultaneous transmissions
  // by all vehicles congesting the channel.  Thus,
  // we need to adjust the start transmission time by
  // some value, t_tx_delay.
  // Therefore, the actual transmit time should be:
  // t_start = t_time + t_drift + t_tx_delay
  // t_drift is always added to t_time.
  // t_tx_delay is supposed to be +/- 5ms, but if we
  // allow negative numbers the time could drift to a value
  // BEFORE the interval start time (i.e., at 100 ms
  // boundaries, we do not want to drift into the
  // previous interval, such as at 95 ms.  Instead,
  // we always want to be at the 100 ms interval boundary,
  // plus [0..10] ms tx delay.
  // Thus, the average t_tx_delay will be
  // within the desired range of [0..10] ms of
  // (t_time + t_drift)

  // WAVE devices sync to GPS time
  // and all devices would like to begin broadcasting
  // their safety messages immediately at the start of
  // the CCH interval.  However, if all do so, then
  // significant collisions occur.  Thus, we assume there
  // is some GPS sync accuracy on GPS devices,
  // typically 40-100 ns.
  // Get a uniformly random number for GPS sync accuracy, in ns.
  Time tDrift = NanoSeconds (m_unirv->GetInteger (0, m_gpsAccuracyNs));

  // When transmitting at a default rate of 10 Hz,
  // the subsystem shall transmit every 100 ms +/-
  // a random value between 0 and 5 ms. [MPR-BSMTX-TXTIM-002]
  // Source: CAMP Vehicle Safety Communications 4 Consortium
  // On-board Minimum Performance Requirements
  // for V2V Safety Systems Version 1.0, December 17, 2014
  // max transmit delay (default 10ms)
  // get value for transmit delay, as number of ns
  uint32_t d_ns = static_cast<uint32_t> (m_txMaxDelay.GetInteger ());
  // convert random tx delay to ns-3 time
  // see note above regarding centering tx delay
  // offset by 5ms + a random value.
  Time txDelay = NanoSeconds (m_unirv->GetInteger (0, d_ns));
  m_prevTxDelay = txDelay;

  Time txTime = startTime + tDrift + txDelay;

  // 非周期性数据包的初始生成，使用 GetAperiodicRandomInterval
  Time firstAperiodicTime = startTime + GetAperiodicRandomInterval();


  // 根据当前设备使用哪个socket来发送
  //if(m_nodeId == 0)
  if(m_macLayerController->GetCurrentDevice() == tdmaDevice)
  {
	  m_macLayerController->DisableDevice(csmaDevice);
	  m_macLayerController->EnableDevice(tdmaDevice);
      // 使用TDMA设备和socket发送数据包
      Simulator::ScheduleWithContext(tdmaSocket->GetNode()->GetId(),
                                     txTime, &BsmApplication::GenerateWaveTraffic, this,
                                     tdmaSocket, m_wavePacketSize, m_numWavePackets, waveInterPacketInterval, m_nodeId);
      Simulator::ScheduleWithContext(tdmaSocket->GetNode()->GetId(),
    		  	  	  	  	  	  	 firstAperiodicTime, &BsmApplication::GenerateAperiodicTraffic, this,
									 tdmaSocket, m_wavePacketSize, m_nodeId);
  }
  else
  {
	  m_macLayerController->DisableDevice(tdmaDevice);
	  m_macLayerController->EnableDevice(csmaDevice);
      // 使用CSMA设备和socket发送数据包
      Simulator::ScheduleWithContext(csmaSocket->GetNode()->GetId(),
                                     txTime, &BsmApplication::GenerateWaveTraffic, this,
                                     csmaSocket, m_wavePacketSize, m_numWavePackets, waveInterPacketInterval, m_nodeId);

      Simulator::ScheduleWithContext(csmaSocket->GetNode()->GetId(),
    		  	  	  	  	  	  	 firstAperiodicTime, &BsmApplication::GenerateAperiodicTraffic, this,
									 csmaSocket, m_wavePacketSize, m_nodeId);
  }

}

void BsmApplication::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);
}

void
BsmApplication::Setup (Ipv4InterfaceContainer & i,
                       int nodeId,
                       Time totalTime,
                       uint32_t wavePacketSize, // bytes
                       Time waveInterval,
                       double gpsAccuracyNs,
                       std::vector <double> rangesSq,           // m ^2
                       Ptr<WaveBsmStats> waveBsmStats,
                       std::vector<int> * nodesMoving,
                       int chAccessMode,
                       Time txMaxDelay)
{
  NS_LOG_FUNCTION (this);

  m_unirv = CreateObject<UniformRandomVariable> ();

  m_TotalSimTime = totalTime;
  m_wavePacketSize = wavePacketSize;
  m_waveInterval = waveInterval;
  m_gpsAccuracyNs = gpsAccuracyNs;
  int size = rangesSq.size ();
  m_waveBsmStats = waveBsmStats;
  m_nodesMoving = nodesMoving;
  m_chAccessMode = chAccessMode;
  m_txSafetyRangesSq.clear ();
  m_txSafetyRangesSq.resize (size, 0);

  for (int index = 0; index < size; index++)
    {
      // stored as square of value, for optimization
      m_txSafetyRangesSq[index] = rangesSq[index];
    }

  m_adhocTxInterfaces = &i;
  m_nodeId = nodeId;
  m_txMaxDelay = txMaxDelay;
  m_macLayerController = m_node->GetObject<MacLayerController>();
}

void
BsmApplication::GenerateWaveTraffic (Ptr<Socket> socket, uint32_t pktSize,
                                     uint32_t pktCount, Time pktInterval,
                                     uint32_t sendingNodeId)
{
  NS_LOG_FUNCTION (this);
  // more packets to send?
  if (pktCount > 0)
    {
      // for now, we cannot tell if each node has
      // started mobility.  so, as an optimization
      // only send if  this node is moving
      // if not, then skip
	  //std::cout<<m_node->GetId()<<" Time:  "<<Simulator::Now().GetMicroSeconds()<<std::endl;
      int txNodeId = sendingNodeId;
      Ptr<Node> txNode = GetNode (txNodeId);
      Ptr<MobilityModel> txPosition = txNode->GetObject<MobilityModel> ();
      NS_ASSERT (txPosition != 0);

      int senderMoving = m_nodesMoving->at (txNodeId);
      if (senderMoving != 0)
        {

    	  BsmTimeTag tag;
    	  tag.setSendingTimeUs(Simulator::Now().GetMicroSeconds());
    	  Ptr<Packet> pkt = Create<Packet> (pktSize);
    	  pkt->AddPacketTag(tag);
    	  //std::cout<<Simulator::Now().GetMicroSeconds()<<std::endl;
	      // send it!
//    	  if(m_macLayerController->GetCurrentDevice() == tdmaDevice)
//    	  {
//    		  tdmaSocket->Send(pkt);
//    	  }
//    	  else
//    	  {
//    		  csmaSocket->Send(pkt);
//    	  }
    	  tdmaSocket->Send(pkt);
    	  //csmaSocket->Send(pkt);
          // count it
          m_waveBsmStats->IncTxPktCount ();
          m_waveBsmStats->IncTxByteCount (pktSize);
          int wavePktsSent = m_waveBsmStats->GetTxPktCount ();
          if ((m_waveBsmStats->GetLogging () != 0) && ((wavePktsSent % 1000) == 0))
            {
              NS_LOG_UNCOND ("Sending WAVE pkt # " << wavePktsSent );
            }

          // find other nodes within range that would be
          // expected to receive this broadbast
          int nRxNodes = m_adhocTxInterfaces->GetN ();
          for (int i = 0; i < nRxNodes; i++)
            {
              Ptr<Node> rxNode = GetNode (i);
              int rxNodeId = rxNode->GetId ();

              if (rxNodeId != txNodeId)
                {
                  Ptr<MobilityModel> rxPosition = rxNode->GetObject<MobilityModel> ();
                  NS_ASSERT (rxPosition != 0);
                  // confirm that the receiving node
                  // has also started moving in the scenario
                  // if it has not started moving, then
                  // it is not a candidate to receive a packet
                  int receiverMoving = m_nodesMoving->at (rxNodeId);
                  if (receiverMoving == 1)
                    {
                      double distSq = MobilityHelper::GetDistanceSquaredBetween (txNode, rxNode);
                      if (distSq > 0.0)
                        {
                          // dest node within range?
                          int rangeCount = m_txSafetyRangesSq.size ();
                          for (int index = 1; index <= rangeCount; index++)
                            {
                              if (distSq <= m_txSafetyRangesSq[index - 1])
                                {
                                  // we should expect dest node to receive broadcast pkt
                                  m_waveBsmStats->IncExpectedRxPktCount (index);
                                }
                            }
                        }
                    }
                }
            }
        }

      // every BSM must be scheduled with a tx time delay
      // of +/- (5) ms.  See comments in StartApplication().
      // we handle this as a tx delay of [0..10] ms
      // from the start of the pktInterval boundary
      uint32_t d_ns = static_cast<uint32_t> (m_txMaxDelay.GetInteger ());
      Time txDelay = NanoSeconds (m_unirv->GetInteger (0, d_ns));
      Time txTime = pktInterval - m_prevTxDelay + txDelay;
      m_prevTxDelay = txDelay;

      Simulator::ScheduleWithContext (tdmaSocket->GetNode ()->GetId (),
                                      txTime, &BsmApplication::GenerateWaveTraffic, this,
									  tdmaSocket, pktSize, pktCount - 1, pktInterval,  tdmaSocket->GetNode ()->GetId ());
    }
  else
    {
      socket->Close ();
    }
}

void BsmApplication::ReceiveWavePacket (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this);
  Ptr<Packet> packet;
  Address senderAddr;
  while ((packet = socket->RecvFrom (senderAddr)))
    {
	  //std::cout<<m_node->GetId()<<std::endl;
      Ptr<Node> rxNode = socket->GetNode ();

      if (InetSocketAddress::IsMatchingType (senderAddr))
        {
          InetSocketAddress addr = InetSocketAddress::ConvertFrom (senderAddr);
          int nodes = m_adhocTxInterfaces->GetN ();
          for (int i = 0; i < nodes; i++)
            {
              if (addr.GetIpv4 () == m_adhocTxInterfaces->GetAddress (i) )
                {
                  Ptr<Node> txNode = GetNode (i);
                  AperiodicTag apic_tag;
                  if(packet->PeekPacketTag(apic_tag))
                  {
                	  //std::cout<<"1"<<std::endl;
                	  HandleReceivedAperiodicPacket(txNode, rxNode);
                  }
                  else
                  {
                      HandleReceivedBsmPacket (txNode, rxNode);
                  }
                  BsmTimeTag tag;
                  packet->RemovePacketTag(tag);
                  uint32_t delta = Simulator::Now().GetMicroSeconds() - tag.getSendingTimeUs();
                  m_waveBsmStats->LogPktRecvDeltaTimeUs(delta);
                }
            }
        }
    }
}

void BsmApplication::HandleReceivedBsmPacket (Ptr<Node> txNode,
                                              Ptr<Node> rxNode)
{
  NS_LOG_FUNCTION (this);

  m_waveBsmStats->IncRxPktCount ();

  m_waveBsmStats->LogPktRecvTime2Map(txNode->GetId(), rxNode->GetId());



  Ptr<MobilityModel> rxPosition = rxNode->GetObject<MobilityModel> ();
  NS_ASSERT (rxPosition != 0);
  // confirm that the receiving node
  // has also started moving in the scenario
  // if it has not started moving, then
  // it is not a candidate to receive a packet
  int rxNodeId = rxNode->GetId ();
  int receiverMoving = m_nodesMoving->at (rxNodeId);
  if (receiverMoving == 1)
    {
      double rxDistSq = MobilityHelper::GetDistanceSquaredBetween (rxNode, txNode);
      if (rxDistSq > 0.0)
        {
          int rangeCount = m_txSafetyRangesSq.size ();
          for (int index = 1; index <= rangeCount; index++)
            {
              if (rxDistSq <= m_txSafetyRangesSq[index - 1])
                {
                  m_waveBsmStats->IncRxPktInRangeCount (index);
                }
            }
        }
    }
}

int64_t
BsmApplication::AssignStreams (int64_t streamIndex)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_unirv);  // should be set by Setup() prevoiusly
  m_unirv->SetStream (streamIndex);

  return 1;
}

Ptr<Node>
BsmApplication::GetNode (int id)
{
  NS_LOG_FUNCTION (this);

  std::pair<Ptr<Ipv4>, uint32_t> interface = m_adhocTxInterfaces->Get (id);
  Ptr<Ipv4> pp = interface.first;
  Ptr<Node> node = pp->GetObject<Node> ();

  return node;
}

Ptr<WifiNetDevice>
BsmApplication::GetNetDevice (int id)
{
  NS_LOG_FUNCTION (this);
//  std::pair<Ptr<Ipv4>, uint32_t> interface = m_adhocTxInterfaces->Get (id);
//  Ptr<Ipv4> pp = interface.first;
//  Ptr<NetDevice> device = pp->GetObject<NetDevice> ();

}

void BsmApplication::GenerateAperiodicTraffic(Ptr<Socket> socket, uint32_t pktSize, uint32_t sendingNodeId)
{
    NS_LOG_FUNCTION(this);

    // 确定当前节点ID，获取节点位置信息
    int txNodeId = sendingNodeId;
    Ptr<Node> txNode = GetNode(txNodeId);
    Ptr<MobilityModel> txPosition = txNode->GetObject<MobilityModel>();
    NS_ASSERT(txPosition != 0);

    // 确认节点是否移动，如果移动，生成数据包
    int senderMoving = m_nodesMoving->at(txNodeId);
    if (senderMoving != 0)
    {
        // 生成数据包并添加时间戳标签
        BsmTimeTag tag;
        tag.setSendingTimeUs(Simulator::Now().GetMicroSeconds());
        AperiodicTag apic_tag;
        Ptr<Packet> pkt = Create<Packet>(pktSize);
        pkt->AddPacketTag(tag);
        pkt->AddPacketTag(apic_tag);

        // 根据 MAC 控制器的当前设备选择对应的 socket
        if (m_macLayerController->GetCurrentDevice() == tdmaDevice)
        {
            // 发送数据包使用 TDMA 设备
            tdmaSocket->Send(pkt);
        }
        else
        {
            // 发送数据包使用 CSMA 设备
            csmaSocket->Send(pkt);
        }

        // 统计发送数据包的数量和字节数
        m_waveBsmStats->IncTxPktCount();
        m_waveBsmStats->IncTxByteCount(pktSize);
        int wavePktsSent = m_waveBsmStats->GetTxPktCount();

        // 每发送1000个数据包，打印日志
        if ((m_waveBsmStats->GetLogging() != 0) && ((wavePktsSent % 1000) == 0))
        {
            NS_LOG_UNCOND("Sending aperiodic pkt # " << wavePktsSent);
        }

        // 确定所有在范围内的节点
        int nRxNodes = m_adhocTxInterfaces->GetN();
        for (int i = 0; i < nRxNodes; i++)
        {
            Ptr<Node> rxNode = GetNode(i);
            int rxNodeId = rxNode->GetId();

            if (rxNodeId != txNodeId)
            {
                Ptr<MobilityModel> rxPosition = rxNode->GetObject<MobilityModel>();
                NS_ASSERT(rxPosition != 0);

                // 确认接收节点是否也在移动
                int receiverMoving = m_nodesMoving->at(rxNodeId);
                if (receiverMoving == 1)
                {
                    double distSq = MobilityHelper::GetDistanceSquaredBetween(txNode, rxNode);
                    if (distSq > 0.0)
                    {
                        int rangeCount = m_txSafetyRangesSq.size();
                        for (int index = 1; index <= rangeCount; index++)
                        {
                            if (distSq <= m_txSafetyRangesSq[index - 1])
                            {
                                // 记录预期接收数据包的节点数
                                m_waveBsmStats->IncExpectedRxPktCount(index);
                            }
                        }
                    }
                }
            }
        }

        // 随机生成下一个数据包的间隔时间
        Time randomInterval = GetAperiodicRandomInterval();

        // 添加随机延迟以避免冲突
        uint32_t d_ns = static_cast<uint32_t>(m_txMaxDelay.GetInteger());
        Time txDelay = NanoSeconds(m_unirv->GetInteger(0, d_ns));
        Time txTime = randomInterval - m_prevTxDelay + txDelay;
        m_prevTxDelay = txDelay;

        // 调度下一次非周期性数据包发送
        if (m_macLayerController->GetCurrentDevice() == tdmaDevice)
        {
            Simulator::ScheduleWithContext(tdmaSocket->GetNode()->GetId(), txTime, &BsmApplication::GenerateAperiodicTraffic, this,
                                           tdmaSocket, pktSize, sendingNodeId);
        }
        else
        {
            Simulator::ScheduleWithContext(csmaSocket->GetNode()->GetId(), txTime, &BsmApplication::GenerateAperiodicTraffic, this,
                                           csmaSocket, pktSize, sendingNodeId);
        }
    }
}





void BsmApplication::HandleReceivedAperiodicPacket(Ptr<Node> txNode, Ptr<Node> rxNode)
{
    NS_LOG_FUNCTION(this);

    // 统计接收数据包
    m_waveBsmStats->IncRxPktCount();
    m_waveBsmStats->LogPktRecvTime2Map(txNode->GetId(), rxNode->GetId());

    Ptr<MobilityModel> rxPosition = rxNode->GetObject<MobilityModel>();
    NS_ASSERT(rxPosition != 0);

    int rxNodeId = rxNode->GetId();
    int receiverMoving = m_nodesMoving->at(rxNodeId);

    if (receiverMoving == 1)
    {
        double rxDistSq = MobilityHelper::GetDistanceSquaredBetween(rxNode, txNode);
        if (rxDistSq > 0.0)
        {
            int rangeCount = m_txSafetyRangesSq.size();
            for (int index = 1; index <= rangeCount; index++)
            {
                if (rxDistSq <= m_txSafetyRangesSq[index - 1])
                {
                    m_waveBsmStats->IncRxPktInRangeCount(index);
                }
            }
        }
    }
}



Time BsmApplication::GetAperiodicRandomInterval()
{
    // 创建一个均值为50毫秒的指数分布随机变量
    Ptr<ExponentialRandomVariable> expRandomVar = CreateObject<ExponentialRandomVariable>();
    expRandomVar->SetAttribute("Mean", DoubleValue(50.0));  // 均值50ms

    // 随机延迟 = 50ms 固定时间 + 指数分布随机时间
    Time randomInterval = MilliSeconds(50) + MilliSeconds(expRandomVar->GetValue());

    return randomInterval;
}



} // namespace ns3
