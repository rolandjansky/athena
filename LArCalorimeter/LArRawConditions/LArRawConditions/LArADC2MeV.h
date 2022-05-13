//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARADC2MEV
#define LARRAWCONDITIONS_LARADC2MEV

#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID_Base.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

#include <vector>
#include <array>
#include "LArElecCalib/LArVectorProxy.h"

class LArADC2MeV {

 public:
  LArADC2MeV() = delete;
  LArADC2MeV(const LArOnlineID_Base* onlineID, 
	     const LArOnOffIdMapping* cabling,
	     const size_t nGains,
	     const unsigned rampDegree);

  ~LArADC2MeV();

  const LArVectorProxy ADC2MEV(const HWIdentifier& id, int gain) const {
    const IdentifierHash h=m_onlineID->channel_Hash(id);
    return ADC2MEV(h,gain);
  }

  const LArVectorProxy ADC2MEV(const IdentifierHash& hid, int gain) const {
    if (!m_adc2MeV[gain].valid.test(hid)) {
      return LArVectorProxy();
    }
    else {
      const float* ptr1=&(m_adc2MeV[gain].data[hid*m_rampDegree]);
      return LArVectorProxy(ptr1,ptr1+m_rampDegree);
    }
  };

  const LArVectorProxy ADC2MEV(const Identifier& offid, int gain) const {
    const HWIdentifier hwid=m_cabling->createSignalChannelID(offid);
    return ADC2MEV(hwid,gain);
  }

  bool set(const IdentifierHash& hid, const int gain, const std::vector<float>& adc2mev);

 private:
 
  struct validVec_t {
    std::vector<float> data;
    std::bitset<200000> valid;
  };

  std::array<validVec_t,CaloGain::LARNGAIN> m_adc2MeV; 
  
  const LArOnlineID_Base* m_onlineID;
  const LArOnOffIdMapping* m_cabling;
  const unsigned m_rampDegree;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArADC2MeV, 87645297, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_MIXED_DEF(LArADC2MeV, 155872295);
#endif

