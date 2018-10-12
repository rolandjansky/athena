//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADCHANNELCONDALG_H
#define LARBADCHANNELCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"

class LArBadChannelCondAlg: public AthAlgorithm {
 public:

  LArBadChannelCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArBadChannelCondAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize() {return StatusCode::SUCCESS;}

 private:
  SG::ReadCondHandleKey<CondAttrListCollection>   m_BCInputKey; 
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey;   
  SG::WriteCondHandleKey<LArBadChannelCont>      m_BCOutputKey;
  ServiceHandle<ICondSvc> m_condSvc;
  std::string m_inputFileName;
};



#endif
