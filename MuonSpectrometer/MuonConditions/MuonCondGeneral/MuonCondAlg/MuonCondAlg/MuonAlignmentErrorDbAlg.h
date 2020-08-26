/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
   MuonAlignmentErrorDbAlg reads raw condition data and writes derived condition data (MuonAlignmentErrorData) to the condition store
*/

#ifndef MUONCONDSVC_MUONALIGNMENTERRORDBALG_H
#define MUONCONDSVC_MUONALIGNMENTERRORDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
//#include "Gaudi/Property.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonCondAlg/MuonAlignmentErrorData.h"

class MuonAlignmentErrorDbAlg: public AthAlgorithm {

 public:

  MuonAlignmentErrorDbAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonAlignmentErrorDbAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private:
  
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/MUONALIGN/ERRS", "Key of input muon alignment error condition data"};
  SG::WriteCondHandleKey<MuonAlignmentErrorData> m_writeKey{this, "WriteKey", "MuonAlignmentErrorData", "Key of output muon alignment error condition data"};    
  ServiceHandle<ICondSvc> m_condSvc;
  
};

#endif
