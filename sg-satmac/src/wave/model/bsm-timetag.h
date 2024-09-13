#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"

namespace ns3 {
class BsmTimeTag : public Tag
{
public:
  BsmTimeTag(){
	  sendingTimeUs = 0;
  }
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  uint32_t getSendingTimeUs() {
		return sendingTimeUs;
	}

  void setSendingTimeUs(uint32_t t) {
	  sendingTimeUs = t;
}

private:
  uint32_t sendingTimeUs;
};

}
