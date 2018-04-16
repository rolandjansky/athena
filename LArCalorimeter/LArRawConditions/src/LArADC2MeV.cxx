#include "LArRawConditions/LArADC2MeV.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <cassert>

LArADC2MeV::LArADC2MeV(const LArOnlineID_Base* onlineID, const LArOnOffIdMapping* cabling, const size_t nGains) :
  m_onlineID(onlineID),
  m_cabling(cabling)
{

  assert(m_onlineID); 
  assert(nGains<=CaloGain::LARNGAIN && nGains>0);

  for (size_t i=0;i<nGains;++i) {
    m_adc2MeV[i].resize(onlineID->channelHashMax());
  }
  //std::cout << "Created a LArADC2MeV obj with " << CaloGain::LARNGAIN 
  //	    << " gains and " << onlineID->channelHashMax() << " hashes." << std::endl;
}

LArADC2MeV::~LArADC2MeV() {}

bool LArADC2MeV::set(const IdentifierHash& hid, const int gain, std::vector<float>& adc2mev) {
  if (gain>=CaloGain::LARNGAIN || hid >= m_adc2MeV[gain].size()) return false;
  //assert (gain<3);
  //assert (hid< m_adc2MeV[gain].size());

  m_adc2MeV[gain][hid].swap(adc2mev);
  return true;
}

