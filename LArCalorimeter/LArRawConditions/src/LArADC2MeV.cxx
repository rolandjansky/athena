#include "LArRawConditions/LArADC2MeV.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <cassert>

LArADC2MeV::LArADC2MeV(const LArOnlineID_Base* onlineID, const size_t nGains) :
  m_onlineID(onlineID),
  m_nGains(nGains) {

  assert(m_onlineID); 
  assert(nGains<3 && nGains>0);

  for (size_t i=0;i<nGains;++i) {
    m_adc2MeV[i].resize(onlineID->channelHashMax());
  }
  
}

LArADC2MeV::~LArADC2MeV() {}

bool LArADC2MeV::set(const IdentifierHash& hid, const int gain, std::vector<float>& adc2mev) {
  if (gain>=CaloGain::LARNGAIN || m_adc2MeV[gain].size() >= hid) return false;
  //assert (gain<3);
  //assert (hid< m_adc2MeV[gain].size());

  m_adc2MeV[gain][hid].swap(adc2mev);
  return true;
}

