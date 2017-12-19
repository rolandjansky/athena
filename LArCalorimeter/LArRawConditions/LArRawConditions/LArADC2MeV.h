//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARADC2MEV
#define LARRAWCONDITIONS_LARADC2MEV

#include "CaloIdentifier/CaloGain.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

#include <vector>
#include <array>

class LArOnlineID_Base;

class LArADC2MeV {

 public:
  LArADC2MeV() = delete;
  LArADC2MeV(const LArOnlineID_Base* onlineID, const size_t nGains);
  ~LArADC2MeV();

  const std::vector<float>& ADC2MEV(const HWIdentifier& id, int gain) const;
  //TODO: Add accessor with offline id (requires cable map which is itself a conditions)

  bool set(const IdentifierHash& hid, const int gain, std::vector<float>& adc2mev);

 private:
  std::array<std::vector<std::vector<float> >,CaloGain::LARNGAIN> m_adc2MeV; 
  
  //Possible optimizations: 
  //Make one flat vector and use something else (magic number?) to denote validity
  //In real life, the size of the inner vector (eg the degree of the ramp-polynom) is always 2

  const LArOnlineID_Base* m_onlineID;
  const size_t m_nGains;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArADC2MeV, 87645297, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF (LArADC2MeV, 155872295);

#endif

