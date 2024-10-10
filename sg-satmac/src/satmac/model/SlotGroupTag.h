#ifndef SLOT_GROUP_TAG_H
#define SLOT_GROUP_TAG_H

#include "ns3/tag.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/packet.h"
#include <string>
#include "satmac-common.h"

namespace ns3 {

class SlotGroupTag : public Tag
{
public:
    SlotGroupTag();
    virtual ~SlotGroupTag();

    // 必须的 NS-3 方法
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
    virtual void Print (std::ostream &os) const;

    // 设置和获取 slot_group_info
    void SetSlotGroupInfo(slot_group_info* sg_info);
    slot_group_info* GetSlotGroupInfo() const;

    // 设置和获取节点ID
    void SetSti(int nodeId);
    int GetSti() const;

    // 设置和获取帧长度
    void SetFrameLen(int frameLen);
    int GetFrameLen() const;

private:
    slot_group_info *m_sg_info;  // 指向 slot_group_info 数组的指针
    int m_sti;
    int m_frameLen;
};

} // namespace ns3

#endif /* SLOT_GROUP_TAG_H */
