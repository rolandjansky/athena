/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "LArRawConditions/LArADC2MeV.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <cassert>

LArADC2MeV::LArADC2MeV(const LArOnlineID_Base* onlineID, const LArOnOffIdMapping* cabling, const size_t nGains, const unsigned rampSize=2) :
  m_onlineID(onlineID),
  m_cabling(cabling),
  m_rampDegree(rampSize){
  assert(m_onlineID); 
  assert(nGains<=CaloGain::LARNGAIN && nGains>0);

  for (size_t i=0;i<nGains;++i) {
    m_adc2MeV[i].data.resize(onlineID->channelHashMax()*m_rampDegree,1.0);
  }
}

LArADC2MeV::~LArADC2MeV() {}

bool LArADC2MeV::set(const IdentifierHash& hid, const int gain, const std::vector<float>& adc2mev) {
  if (gain>=(int)m_adc2MeV.size() || hid*m_rampDegree >= m_adc2MeV[gain].data.size()) {
    //std::cout << "hid="<< hid << ",size overrun: " << gain << " ," << m_adc2MeV.size() << " ," << hid*m_rampDegree << " ," <<  m_adc2MeV[gain].data.size() << std::endl;
    return false;
  }
  if (adc2mev.size()!=m_rampDegree) {
    //std::cout << "hid="<< hid << ", Got deg " << adc2mev.size() << ", expected " << m_rampDegree << std::endl;
    return false;
  }
  m_adc2MeV[gain].valid.set(hid);
  std::vector<float>::iterator it=m_adc2MeV[gain].data.begin()+hid*m_rampDegree;
  std::copy(adc2mev.begin(),adc2mev.end(),it); 

  return true;
}

