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


#include "ns3/wave-bsm-stats.h"
#include "ns3/integer.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <numeric>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("WaveBsmStats");

WaveBsmStats::WaveBsmStats ()
  : m_wavePktSendCount (0),
    m_waveByteSendCount (0),
    m_wavePktReceiveCount (0),
    m_log (0)
{
  m_wavePktExpectedReceiveCounts.resize (10, 0);
  m_wavePktInCoverageReceiveCounts.resize (10, 0);
  m_waveTotalPktExpectedReceiveCounts.resize (10, 0);
  m_waveTotalPktInCoverageReceiveCounts.resize (10, 0);
}

/* static */
TypeId
WaveBsmStats::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WaveBsmStats")
    .SetParent<Object> ()
    .SetGroupName ("Stats")
    .AddConstructor<WaveBsmStats> ()
    ;
  return tid;
}
  
void
WaveBsmStats::IncTxPktCount ()
{
  m_wavePktSendCount++;
}

int
WaveBsmStats::GetTxPktCount ()
{
  return m_wavePktSendCount;
}

void
WaveBsmStats::IncExpectedRxPktCount (int index)
{
  m_wavePktExpectedReceiveCounts[index - 1]++;
  m_waveTotalPktExpectedReceiveCounts[index - 1]++;
}

void
WaveBsmStats::IncRxPktCount ()
{
  m_wavePktReceiveCount++;
}

void
WaveBsmStats::IncRxPktInRangeCount (int index)
{
  m_wavePktInCoverageReceiveCounts[index - 1]++;
  m_waveTotalPktInCoverageReceiveCounts[index - 1]++;
}

int
WaveBsmStats::GetRxPktCount ()
{
  return m_wavePktReceiveCount;
}

int
WaveBsmStats::GetExpectedRxPktCount (int index)
{
  return m_wavePktExpectedReceiveCounts[index - 1];
}

int
WaveBsmStats::GetRxPktInRangeCount (int index)
{
  return m_wavePktInCoverageReceiveCounts[index - 1];
}

void
WaveBsmStats::SetTxPktCount (int count)
{
  m_wavePktSendCount = count;
}

void
WaveBsmStats::SetRxPktCount (int count)
{
  m_wavePktReceiveCount = count;
}

void
WaveBsmStats::IncTxByteCount (int bytes)
{
  m_waveByteSendCount += bytes;
}

int
WaveBsmStats::GetTxByteCount ()
{
  return m_waveByteSendCount;
}

double
WaveBsmStats::GetBsmPdr (int index)
{
  double pdr = 0.0;

  if (m_wavePktExpectedReceiveCounts[index - 1] > 0)
    {
      pdr = (double) m_wavePktInCoverageReceiveCounts[index - 1] / (double) m_wavePktExpectedReceiveCounts[index - 1];
      // due to node movement, it is
      // possible to receive a packet that is not slightly "within range" that was
      // transmitted at the time when the nodes were slightly "out of range"
      // thus, prevent overflow of PDR > 100%
      if (pdr > 1.0)
        {
          pdr = 1.0;
        }
    }

  return pdr;
}

double
WaveBsmStats::GetCumulativeBsmPdr (int index)
{
  double pdr = 0.0;

  if (m_waveTotalPktExpectedReceiveCounts[index - 1] > 0)
    {
      pdr = (double) m_waveTotalPktInCoverageReceiveCounts[index - 1] / (double) m_waveTotalPktExpectedReceiveCounts[index - 1];
      // due to node movement, it is
      // possible to receive a packet that is not slightly "within range" that was
      // transmitted at the time when the nodes were slightly "out of range"
      // thus, prevent overflow of PDR > 100%
      if (pdr > 1.0)
        {
          pdr = 1.0;
        }
    }

  return pdr;
}

void
WaveBsmStats::SetLogging (int log)
{
  m_log = log;
}

int
WaveBsmStats::GetLogging ()
{
  return m_log;
}

void
WaveBsmStats::SetExpectedRxPktCount (int index, int count)
{
  m_wavePktExpectedReceiveCounts[index - 1] = count;
}

void
WaveBsmStats::SetRxPktInRangeCount (int index, int count)
{
  m_wavePktInCoverageReceiveCounts[index - 1] = count;
}

void
WaveBsmStats::ResetTotalRxPktCounts (int index)
{
  m_waveTotalPktInCoverageReceiveCounts[index - 1] = 0;
  m_waveTotalPktExpectedReceiveCounts[index - 1] = 0;
}

void WaveBsmStats::LogPktRecvTime2Map(int txNodei, int rxNodei)
{// std::vector< std::map<int, std::vector<int>>>

	std::map<int, std::map<int, std::vector<int>>>::iterator ptxi;
	for(ptxi = pkt_revtime_per.begin(); ptxi != pkt_revtime_per.end(); ptxi++) {
		if (ptxi->first == txNodei) { //if txNodei existed

			std::map<int, std::vector<int>>::iterator prxi;
			for(prxi = ptxi->second.begin(); prxi != ptxi->second.end(); prxi++) {
				if (prxi->first == rxNodei) {// if rxNodei existed
					prxi->second.push_back(Simulator::Now().GetMicroSeconds());
					break;
				}
			}

			if (prxi == ptxi->second.end()) {
				std::vector<int> tv;
				tv.push_back(Simulator::Now().GetMicroSeconds());
				ptxi->second[rxNodei] = tv;
			}

			break;
		}
	}

	if (ptxi == pkt_revtime_per.end()) {
		std::vector<int> tv;
		tv.push_back(Simulator::Now().GetMicroSeconds());
		std::map<int, std::vector<int>> tm1;
		tm1[rxNodei] = tv;
		pkt_revtime_per[txNodei] = tm1;
	}

}

double WaveBsmStats::GetPIR(int *max, int *min)
{
	std::map<int, std::map<int, std::vector<int>>>::iterator ptxi;
	std::map<int, std::vector<int>>::iterator prxi;
	std::vector<int>::iterator pv;

	double tsum=0, tmean=0, tmax=0, tmin=99999;
	int totalgapcount=0;

	for(ptxi = pkt_revtime_per.begin(); ptxi != pkt_revtime_per.end(); ptxi++)
	{
		for(prxi= ptxi->second.begin(); prxi != ptxi->second.end(); prxi++)
		{
			if (prxi->second.size() > 1) {
				int lastpkttime = 0;
				for(pv = prxi->second.begin(); pv != prxi->second.end(); pv++)
				{//calculate gaps
					if (lastpkttime == 0) {
						lastpkttime = *pv;
					} else {
						int tmpgap = (*pv - lastpkttime);
						lastpkttime = *pv;
						tsum += tmpgap;
						tmin = tmin < tmpgap ? tmin : tmpgap;
						tmax = tmax > tmpgap ? tmax : tmpgap;
					}
				}
				totalgapcount += prxi->second.size() - 1;
			}
		}
	}


	if (tmax == 0) {
		*max = -1;
		*min = -1;
		tmean = -1;
	} else {
		*max = tmax;
		*min = tmin;
		tmean = round(tsum / (totalgapcount));
	}
	return tmean;
}

void WaveBsmStats::ResetRecvTimeMap ()
{
	std::map<int, std::map<int, std::vector<int>>>::iterator ptxi;
	std::map<int, std::vector<int>>::iterator prxi;
	std::vector<int>::iterator pv;

	for(ptxi = pkt_revtime_per.begin(); ptxi != pkt_revtime_per.end(); ptxi++)
	{
		for(prxi= ptxi->second.begin(); prxi != ptxi->second.end(); prxi++)
		{
			prxi->second.clear();
		}
		ptxi->second.clear();
	}
	pkt_revtime_per.clear();
}

void WaveBsmStats::LogPktRecvDeltaTimeUs(int delta)
{
	m_pktRecvDeltaTimeUs.push_back(delta);
	//m_pktRecvDeltaTimeUs_cumulative.push_back(delta);
}
void WaveBsmStats::ResetPktRecvDeltaTimeUs()
{
	m_pktRecvDeltaTimeUs.clear();
}
uint64_t WaveBsmStats::GetPktRecvDeltaTimeUs(int *max, int *min)
{
    uint64_t sum = 0;
    int size = m_pktRecvDeltaTimeUs.size();
    if (size > 0) {
		//sum = std::accumulate(m_pktRecvDeltaTimeUs.begin(), m_pktRecvDeltaTimeUs.end(), 0);
		for (int i=0; i<size; i++)
			sum += m_pktRecvDeltaTimeUs[i];
		uint64_t mean =  sum / m_pktRecvDeltaTimeUs.size();
		*max = *max_element(m_pktRecvDeltaTimeUs.begin(), m_pktRecvDeltaTimeUs.end());
		*min = *min_element(m_pktRecvDeltaTimeUs.begin(), m_pktRecvDeltaTimeUs.end());
		return mean;
    } else {
    	*max = -1;
    	*min = -1;
    	return -1;
    }
}

std::vector<int>* WaveBsmStats::GetPktRecvDeltaTimeUs_vec()
{
	return &m_pktRecvDeltaTimeUs;
}
} // namespace ns3
