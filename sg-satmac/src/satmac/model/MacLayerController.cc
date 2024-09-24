#include "MacLayerController.h"
#include "ns3/log.h"
#include "ns3/wifi-net-device.h"
//#include "ns3/ocb-wifi-mac.h"

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
    DisableDevice(csmaDevice);
    EnableDevice(tdmaDevice);
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
    Ptr<WifiMac> fromMac = fromDevice->GetMac();
    Ptr<WifiMac> toMac = toDevice->GetMac();

//    Ptr<OcbWifiMac> fromMac = DynamicCast<OcbWifiMac>(fromDevice->GetMac());
//    Ptr<OcbWifiMac> toMac = DynamicCast<OcbWifiMac>(toDevice->GetMac());



//    WifiMacHeader header;
//    while (!fromMac->IsQueueEmpty()) {
//        Ptr<const Packet> packet = fromMac->Dequeue(&header);  // 从当前设备出队
//        toMac->Enqueue(packet, header.GetAddr1());  // 入队到新设备
//    }

    NS_LOG_INFO("Transferred packets from current device to new device");
}

// 启用指定设备
void MacLayerController::EnableDevice(Ptr<WifiNetDevice> device) {
	device->SetSendEnabled(true);
}

// 禁用指定设备
void MacLayerController::DisableDevice(Ptr<WifiNetDevice> device) {
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




