#ifndef SLOT_GROUP_TAG_H
#define SLOT_GROUP_TAG_H

#include "ns3/tag.h"
#include "ns3/uinteger.h"
#include "ns3/nstime.h"
#include "ns3/address.h"
#include <string.h>

namespace ns3 {

class SlotGroupTag : public Tag
{
public:
  SlotGroupTag ();
  SlotGroupTag (const char* geohash, uint32_t countNode);

  // Inherited from Tag.
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  void SetGeohash (const char* geohash);
  const char* GetGeohash () const;

  void SetCountNode (uint32_t countNode);
  uint32_t GetCountNode () const;

private:
  char m_geohash[3]; // 3-byte geohash
  uint32_t m_countNode;
};

} // namespace ns3

#endif /* SLOT_GROUP_TAG_H */
