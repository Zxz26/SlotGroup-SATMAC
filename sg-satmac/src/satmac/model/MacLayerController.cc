#include "MacLayerController.h"
#include "ns3/log.h"
#include "ns3/wifi-net-device.h"
#include "ns3/ocb-wifi-mac.h"
#include "AperiodicTag.h"

NS_LOG_COMPONENT_DEFINE("MacLayerController");

TypeId
MacLayerController::GetTypeId (void)
{
    static TypeId tid = TypeId("ns3::MacLayerController")
        .SetParent<Object> ()
        .AddConstructor<MacLayerController> ()
        .AddAttribute("TDMADevice",
                      "The TDMA device associated with this controller.",
                      PointerValue(),
                      MakePointerAccessor(&MacLayerController::m_tdmaDevice),
                      MakePointerChecker<WifiNetDevice> ())
        .AddAttribute("CSMADevice",
                      "The CSMA device associated with this controller.",
                      PointerValue(),
                      MakePointerAccessor(&MacLayerController::m_csmaDevice),
                      MakePointerChecker<WifiNetDevice> ())
        .AddAttribute("CurrentDevice",
                      "The currently active device (TDMA or CSMA).",
                      PointerValue(),
                      MakePointerAccessor(&MacLayerController::m_currentDevice),
                      MakePointerChecker<WifiNetDevice> ());
    return tid;
}

MacLayerController::MacLayerController() : m_tdmaDevice(0), m_csmaDevice(0), m_currentDevice(0) {}

void
MacLayerController::Initialize(Ptr<WifiNetDevice> tdmaDevice, Ptr<WifiNetDevice> csmaDevice, Ptr<Node> node) {
    m_tdmaDevice = tdmaDevice;
    m_csmaDevice = csmaDevice;
    m_currentDevice = tdmaDevice;  // 默认使用 TDMA 设备
    m_node = node;
//    DisableDevice(csmaDevice);
//    EnableDevice(tdmaDevice);
}

void MacLayerController::SwitchToDevice(Ptr<WifiNetDevice> device)
{
    if (m_currentDevice != device) {
        // 先转移数据包到新设备
        TransferPackets(m_currentDevice, device);
    	// 禁用当前设备
        DisableDevice(m_currentDevice);
        // 启用新设备
        EnableDevice(device);
        m_currentDevice = device;
        NS_LOG_INFO("Switched to device: " << device);
    }
}


void MacLayerController::CheckAndSwitch()
{
    bool shouldUseCsma = SomeMacLayerCondition();
    if (shouldUseCsma) {
        SwitchToDevice(m_csmaDevice);
    } else {
        SwitchToDevice(m_tdmaDevice);
    }
}

void MacLayerController::ScheduleDeviceCheck(Time interval)
{
    Simulator::Schedule(interval, &MacLayerController::CheckAndSwitch, this);
}

void MacLayerController::TransferPackets(Ptr<WifiNetDevice> fromDevice, Ptr<WifiNetDevice> toDevice)
{
    Ptr<OcbWifiMac> fromMac = DynamicCast<OcbWifiMac>(fromDevice->GetMac());
    Ptr<OcbWifiMac> toMac = DynamicCast<OcbWifiMac>(toDevice->GetMac());
	AperiodicTag tag;

    if(fromDevice == m_tdmaDevice)
    {
        //tdma to csma
    	Ptr<TdmaSatmac> satmac = fromMac->GetTdmaObject();
    	Ptr<TdmaMacQueue> tdma_q = satmac->GetTdmaQueue();
    	Ptr<Txop> txop = toMac->GetTxopObject();
    	std::vector<std::pair<Ptr<const Packet>, WifiMacHeader>> tempQueue;
    	while(!tdma_q->IsEmpty())
    	{
    		WifiMacHeader header;
    		Ptr<const Packet> packet = tdma_q->Dequeue(&header);
    		if(packet && packet->PeekPacketTag(tag))
    		{
    			//txop->Queue(packet, header);
//    			std::cout<<"node : "<<satmac->getNodePtr()->GetId()
//    					<<" transfer to csma at "<<Simulator::Now().GetMicroSeconds()<<std::endl;
    			Time ramdomDelay = GetRandomTimeDelay();
    			Simulator::Schedule(ramdomDelay, &OcbWifiMac::Enqueue, toMac, packet, header.GetAddr1());
    			//toMac->Enqueue(packet, header.GetAddr1());
    		}
    		else
    		{
    			tempQueue.push_back(std::make_pair(packet, header));
    		}
    	}
    	for(auto item : tempQueue)
    	{
    		fromMac->Enqueue(item.first, item.second.GetAddr1());
    		//satmac->Queue(item.first, item.second);
    	}
    }
    else
    {
    	//csma to tdma
    	Ptr<Txop> txop = fromMac->GetTxopObject();
    	Ptr<TdmaSatmac> satmac = toMac->GetTdmaObject();
    	Ptr<WifiMacQueue> txop_q = txop->GetWifiMacQueue();
    	std::vector<Ptr<WifiMacQueueItem>> tempQueue;
    	while(!txop_q->IsEmpty())
    	{
    		Ptr<WifiMacQueueItem> item = txop_q->Dequeue();
    		NS_ASSERT(item != 0);
    		Ptr<const Packet> packet = item->GetPacket();
    		WifiMacHeader header = item->GetHeader();
    		if(packet && packet->PeekPacketTag(tag))
    		{
        		//satmac->Queue(packet, header);
//    			std::cout<<"node : "<<satmac->getNodePtr()->GetId()
//    					<<" transfer to tdma at "<<Simulator::Now().GetMicroSeconds()<<std::endl;
    			toMac->Enqueue(packet, header.GetAddr1());
    		}
    		else
    		{
    			tempQueue.push_back(item);
    		}
    	}
    	for(auto item : tempQueue)
    	{
    		txop_q->Enqueue(item);
    	}
    }

    NS_LOG_INFO("Transferred packets from current device to new device");
}


// 启用指定设备
void MacLayerController::EnableDevice(Ptr<WifiNetDevice> device) {
	device->SetSendEnabled(true);
}

// 禁用指定设备
void MacLayerController::DisableDevice(Ptr<WifiNetDevice> device) {
	//std::cout<<"DisableDevice at "<<Simulator::Now()<<std::endl;
	device->SetSendEnabled(false);
}


Ptr<WifiNetDevice> MacLayerController::GetCurrentDevice()
{
	return m_currentDevice;
}

bool MacLayerController::SomeMacLayerCondition()
{
    // 自定义逻辑：基于 MAC 层的某些条件进行判断是否需要切换
    // 返回 true 切换到 CSMA，否则切换到 TDMA
    // 举个例子，这里可以使用信道状态、队列大小等条件
    return rand() % 2 == 0; // 随机切换作为示例
}

Time MacLayerController::GetRandomTimeDelay()
{
    Ptr<UniformRandomVariable> randomVar = CreateObject<UniformRandomVariable>();
    uint32_t maxDelayNs = 30000; // 最大延迟固定为1000纳秒
    uint32_t randomDelay = randomVar->GetInteger(0, maxDelayNs);
    return NanoSeconds(randomDelay);
}

void MacLayerController::TransBsmPacket(Ptr<const Packet> pkt, WifiMacHeader hdr)
{
	if(m_currentDevice == m_csmaDevice)
	{
		Ptr<OcbWifiMac> tdmaMac = DynamicCast<OcbWifiMac>(m_tdmaDevice->GetMac());
		Ptr<TdmaSatmac> satmac = tdmaMac->GetTdmaObject();
		satmac->GetTdmaQueue()->Enqueue(pkt, hdr);
		//satmac->Queue(pkt, hdr);
	}
}






