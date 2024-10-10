#ifndef APERIODIC_TAG_H
#define APERIODIC_TAG_H

#include "ns3/tag.h"
#include "ns3/uinteger.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"

namespace ns3 {

/**
 * \brief AperiodicTag is used to differentiate aperiodic packets from regular BSM packets.
 * Currently, it does not store any information but can be extended if needed.
 */
class AperiodicTag : public Tag
{
public:
  // Constructor
  AperiodicTag ();

  // Destructor
  virtual ~AperiodicTag ();

  // Get the type ID for this class
  static TypeId GetTypeId (void);

  // Get the instance type ID for this tag
  virtual TypeId GetInstanceTypeId (void) const;

  // Serialize the tag (empty for now)
  virtual void Serialize (TagBuffer i) const;

  // Deserialize the tag (empty for now)
  virtual void Deserialize (TagBuffer i);

  // Get the serialized size (always 0 for now)
  virtual uint32_t GetSerializedSize (void) const;

  // Print the tag information (if needed for debugging)
  virtual void Print (std::ostream &os) const;

};

} // namespace ns3

#endif /* APERIODIC_TAG_H */
