//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARMCSYMCONDGALG_H
#define LARRECCONDITIONS_LARMCSYMCONDGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "LArRecConditions/LArOnOffIdMapping.h"
#include "LArRawConditions/LArMCSym.h"

class LArMCSymCondAlg: public AthAlgorithm {

public:

  LArMCSymCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArMCSymCondAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_readKey;
  SG::WriteCondHandleKey<LArMCSym>  m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;

};



#endif
