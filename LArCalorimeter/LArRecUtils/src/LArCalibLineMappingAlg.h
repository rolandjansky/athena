//Dear emacs, this is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARCALIBLINEMAPPINGALG_H
#define LARRECCONDITIONS_LARCALIBLINEMAPPINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArCalibLineMapping.h"

class LArCalibLineMappingAlg: public AthAlgorithm {

public:

  LArCalibLineMappingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArCalibLineMappingAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<AthenaAttributeList>   m_readKey;
  SG::WriteCondHandleKey<LArCalibLineMapping>  m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  //bool m_isSuperCell;

};



#endif
