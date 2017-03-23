#include "LArRecConditions/LArADC2MeV.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <cassert>

LArADC2MeV::LArADC2MeV(const LArOnlineID_Base* onlineID) :
  m_onlineID(onlineID) {

  assert(m_onlineID); 

  for(auto& perGain : m_adc2MeV) {
    perGain.resize(onlineID->channelHashMax());
  }
}


bool LArADC2MeV::set(const IdentifierHash& hid, const int gain, std::vector<float>& adc2mev) {
  if (gain>=CaloGain::LARNGAIN || m_adc2MeV[gain].size() >= hid) return false;
  //assert (gain<3);
  //assert (hid< m_adc2MeV[gain].size());

  m_adc2MeV[gain][hid].swap(adc2mev);
  return true;
}

