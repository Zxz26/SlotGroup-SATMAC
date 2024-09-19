#include "SlotGroupTag.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlotGroupTag");
NS_OBJECT_ENSURE_REGISTERED (SlotGroupTag);

SlotGroupTag::SlotGroupTag () 
{
  strcpy(m_geohash, "00");
  m_countNode = 0;
}

SlotGroupTag::SlotGroupTag (const char* geohash, uint32_t countNode)
{
  strcpy(m_geohash, geohash);
  m_countNode = countNode;
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
  return 3 + sizeof(m_countNode);
}

void
SlotGroupTag::Serialize (TagBuffer i) const
{
  i.Write ((const uint8_t *) m_geohash, 3);
  i.WriteU32 (m_countNode);
}

void
SlotGroupTag::Deserialize (TagBuffer i)
{
  i.Read ((uint8_t *) m_geohash, 3);
  m_countNode = i.ReadU32 ();
}

void
SlotGroupTag::Print (std::ostream &os) const
{
  os << "Geohash=" << m_geohash << ", countNode=" << m_countNode;
}

void
SlotGroupTag::SetGeohash (const char* geohash)
{
  strcpy(m_geohash, geohash);
}

const char*
SlotGroupTag::GetGeohash () const
{
  return m_geohash;
}

void
SlotGroupTag::SetCountNode (uint32_t countNode)
{
  m_countNode = countNode;
}

uint32_t
SlotGroupTag::GetCountNode () const
{
  return m_countNode;
}

} // namespace ns3
