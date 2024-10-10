#include "SlotGroupTag.h"
#include "ns3/log.h"
#include "ns3/nstime.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlotGroupTag");

SlotGroupTag::SlotGroupTag()
  : m_sti(0), m_frameLen(0)
{
    m_sg_info = new slot_group_info[128]; // 初始化 slot_group_info 数组
}

SlotGroupTag::~SlotGroupTag()
{
    delete[] m_sg_info; // 释放内存
}

TypeId
SlotGroupTag::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::SlotGroupTag")
        .SetParent<Tag> ()
        .AddConstructor<SlotGroupTag> ();
    return tid;
}

TypeId
SlotGroupTag::GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

uint32_t
SlotGroupTag::GetSerializedSize (void) const
{
    return 8 + 128 * (sizeof(uint32_t) + 2);  // nodeId + frameLen + slot_group_info的大小
}

void
SlotGroupTag::Serialize (TagBuffer i) const
{
    i.WriteU32(m_sti);
    i.WriteU32(m_frameLen);

    for (int j = 0; j < 128; ++j)
    {
        i.Write ((const uint8_t *) m_sg_info[j].geohash.c_str(), 2);
        i.WriteU32(m_sg_info[j].count_node);
    }
}

void
SlotGroupTag::Deserialize (TagBuffer i)
{
	m_sti = i.ReadU32();
    m_frameLen = i.ReadU32();

    for (int j = 0; j < 128; ++j)
    {
        char buffer[3] = {0}; // 用于存储geohash
        i.Read ((uint8_t *) buffer, 2);
        m_sg_info[j].geohash = std::string(buffer);
        m_sg_info[j].count_node = i.ReadU32();
    }
}

void
SlotGroupTag::Print (std::ostream &os) const
{
    os << "NodeSti=" << m_sti << ", FrameLen=" << m_frameLen << std::endl;
    for (int j = 0; j < 128; ++j)
    {
        os << "GeoHash=" << m_sg_info[j].geohash << ", CountNode=" << m_sg_info[j].count_node << std::endl;
    }
}

void
SlotGroupTag::SetSlotGroupInfo(slot_group_info* sg_info)
{
    for (int j = 0; j < 128; ++j)
    {
        m_sg_info[j].geohash = sg_info[j].geohash;
        m_sg_info[j].count_node = sg_info[j].count_node;
    }
}

slot_group_info*
SlotGroupTag::GetSlotGroupInfo() const
{
    return m_sg_info;
}

void
SlotGroupTag::SetSti(int sti)
{
	m_sti = sti;
}

int
SlotGroupTag::GetSti() const
{
    return m_sti;
}

void
SlotGroupTag::SetFrameLen(int frameLen)
{
    m_frameLen = frameLen;
}

int
SlotGroupTag::GetFrameLen() const
{
    return m_frameLen;
}

} // namespace ns3
