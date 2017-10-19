//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARONOFFMAPPINGALG_H
#define LARRECCONDITIONS_LARONOFFMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArOnOffIdMapping.h"

class LArOnOffMappingAlg: public AthAlgorithm {

public:

  LArOnOffMappingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LArOnOffMappingAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;


 private:
  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey;
  SG::WriteCondHandleKey<LArOnOffIdMapping>  m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  bool m_isSuperCell;

};



#endif
