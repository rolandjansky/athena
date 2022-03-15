//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARMCSYMCONDGALG_H
#define LARRECCONDITIONS_LARMCSYMCONDGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRawConditions/LArMCSym.h"

class LArMCSymCondAlg: public AthAlgorithm {

public:
  using AthAlgorithm::AthAlgorithm;

  ~LArMCSymCondAlg()=default;

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_readKey  {this,"ReadKey","LArOnOffIdMap"};
  SG::WriteCondHandleKey<LArMCSym>         m_writeKey {this,"WriteKey","LArMCSym"};
};



#endif
