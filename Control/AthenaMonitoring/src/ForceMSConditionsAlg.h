/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef FORCEMSCONDITIONSALG_H
#define FORCEMSCONDITIONSALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonAlignmentData/CorrContainer.h"

class ForceMSConditionsAlg : public AthReentrantAlgorithm {
 public:
  ForceMSConditionsAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {};
  virtual ~ForceMSConditionsAlg()=default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext&) const override { return StatusCode::SUCCESS; };

 protected:
  SG::ReadCondHandleKey<ALineMapContainer> m_ALineKey{this, "ALineKey", "ALineMapContainer", "Key of muon alignment ALine condition data"}; 
  SG::ReadCondHandleKey<BLineMapContainer> m_BLineKey{this, "BLineKey", "BLineMapContainer", "Key of muon alignment BLine condition data"};

};

#endif
