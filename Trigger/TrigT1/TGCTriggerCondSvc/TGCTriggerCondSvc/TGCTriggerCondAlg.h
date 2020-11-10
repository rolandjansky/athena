/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERCONDALG_H
#define TGCTRIGGERCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "TGCTriggerCondSvc/TGCTriggerLUTs.h"
#include "TGCTriggerCondSvc/TGCTriggerBWCWReader.h"

  class TGCTriggerCondAlg: public AthAlgorithm {

  public:

    TGCTriggerCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TGCTriggerCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;
 

  private:

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_bw;
    SG::WriteCondHandleKey<TGCTriggerLUTs> m_writeKey;    
    ServiceHandle<ICondSvc> m_condSvc;


    // LUT Readers 
    TGCTriggerBWCWReader m_bwCWReader;

  };


#endif


