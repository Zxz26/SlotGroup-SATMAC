#ifndef MAC_LAYER_CONTROLLER_H
#define MAC_LAYER_CONTROLLER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4.h"


using namespace ns3;


class MacLayerController : public Object
{
public:
	static TypeId GetTypeId (void);
    MacLayerController();
    void Initialize(Ptr<WifiNetDevice> tdmaDevice, Ptr<WifiNetDevice> csmaDevice, Ptr<Node> node);

    // 切换到指定的网络设备
    void SwitchToDevice(Ptr<WifiNetDevice> device);

    // 根据 MAC 层条件检查并进行设备切换
    void CheckAndSwitch();

    // 定期调度检查并切换设备
    void ScheduleDeviceCheck(Time interval);

    Ptr<WifiNetDevice> GetTdmaDevice(){
    	return m_tdmaDevice;
    }
    Ptr<WifiNetDevice> GetCsmaDevice(){
    	return m_csmaDevice;
    }
    Ptr<Node> GetNodePtr(){
    	return m_node;
    }
    Ptr<WifiNetDevice> GetCurrentDevice();


    // 启用指定设备
    void EnableDevice(Ptr<WifiNetDevice> device);

    // 禁用指定设备
    void DisableDevice(Ptr<WifiNetDevice> device);

    //
    void TransBsmPacket(Ptr<const Packet> pkt, WifiMacHeader hdr);
private:
    Ptr<WifiNetDevice> m_tdmaDevice;
    Ptr<WifiNetDevice> m_csmaDevice;
    Ptr<WifiNetDevice> m_currentDevice;
    Ptr<Node> m_node;

    // 从一个设备的队列中取出数据包并将其转移到另一个设备
    void TransferPackets(Ptr<WifiNetDevice> fromDevice, Ptr<WifiNetDevice> toDevice);

    // 判断是否满足切换设备的条件（自定义条件）
    bool SomeMacLayerCondition();

    Time GetRandomTimeDelay();

};

#endif // MAC_LAYER_CONTROLLER_H
