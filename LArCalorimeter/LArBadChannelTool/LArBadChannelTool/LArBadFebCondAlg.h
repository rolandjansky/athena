//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADFEBCONDALG_H
#define LARBADFEBCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecConditions/LArBadChannelCont.h"

//class LArOnlineID;
//class LArOnline_SuperCellID;

class LArBadFebCondAlg: public AthAlgorithm {
 public:

  LArBadFebCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadFebCondAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<AthenaAttributeList>   m_BCInputKey; 
  SG::WriteCondHandleKey<LArBadFebCont>      m_BCOutputKey;
  ServiceHandle<ICondSvc> m_condSvc;
  std::string m_inputFileName;
};



#endif
