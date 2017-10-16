//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARFEBRODMAPPINGALG_H
#define LARRECCONDITIONS_LARFEBRODMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArFebRodMapping.h"

class LArFebRodMappingAlg: public AthAlgorithm {

public:

  LArFebRodMappingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArFebRodMappingAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;


 private:
  SG::ReadCondHandleKey<AthenaAttributeList>   m_readKey;
  SG::WriteCondHandleKey<LArFebRodMapping>  m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;

};



#endif
