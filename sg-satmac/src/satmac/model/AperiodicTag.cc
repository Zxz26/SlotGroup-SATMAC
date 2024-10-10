#include "AperiodicTag.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AperiodicTag");

NS_OBJECT_ENSURE_REGISTERED (AperiodicTag);

AperiodicTag::AperiodicTag ()
{
}

AperiodicTag::~AperiodicTag ()
{
}

TypeId
AperiodicTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AperiodicTag")
    .SetParent<Tag> ()
    .AddConstructor<AperiodicTag> ();
  return tid;
}

TypeId
AperiodicTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
AperiodicTag::GetSerializedSize (void) const
{
  return 0; // No data to serialize
}

void
AperiodicTag::Serialize (TagBuffer i) const
{
  // No data to serialize
}

void
AperiodicTag::Deserialize (TagBuffer i)
{
  // No data to deserialize
}

void
AperiodicTag::Print (std::ostream &os) const
{
  os << "AperiodicTag (no data)";
}

} // namespace ns3
