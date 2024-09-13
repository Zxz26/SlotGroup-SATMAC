#include "bsm-timetag.h"

using namespace ns3;

TypeId
BsmTimeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BsmTimeTag")
    .SetParent<Tag> ()
    .AddConstructor<BsmTimeTag> ()

  ;
  return tid;
}

TypeId
BsmTimeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
BsmTimeTag::GetSerializedSize (void) const
{
  return 7;
}
void
BsmTimeTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (sendingTimeUs);

}
void
BsmTimeTag::Deserialize (TagBuffer i)
{
	sendingTimeUs = i.ReadU32 ();

}
void
BsmTimeTag::Print (std::ostream &os) const
{
  os << "sendtime=" << (uint32_t)sendingTimeUs;
}

