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

class LArADC2MeV {

 public:
  LArADC2MeV() = delete;
  LArADC2MeV(const LArOnlineID_Base* onlineID, 
	     const LArOnOffIdMapping* cabling,
	     const size_t nGains);

  ~LArADC2MeV();

  const std::vector<float>& ADC2MEV(const HWIdentifier& id, int gain) const {
    const IdentifierHash h=m_onlineID->channel_Hash(id);
    return m_adc2MeV[gain][h];
  }


  const std::vector<float>& ADC2MeV(const Identifier& offid, int gain) const {
    const Identifier hwid=m_cabling->createSignalChannelID(offid);
    return ADC2MeV(hwid,gain);
  }

  bool set(const IdentifierHash& hid, const int gain, std::vector<float>& adc2mev);

 private:
  std::array<std::vector<std::vector<float> >,CaloGain::LARNGAIN> m_adc2MeV; 
  
  //Possible optimizations: 
  //Make one flat vector and use something else (magic number?) to denote validity
  //In real life, the size of the inner vector (eg the degree of the ramp-polynom) is always 2

  const LArOnlineID_Base* m_onlineID;
  const LArOnOffIdMapping* m_cabling;
  const size_t m_nGains;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArADC2MeV, 87645297, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF (LArADC2MeV, 155872295);

#endif

