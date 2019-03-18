/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECUTILS_LARFEBCONFIGCONDALG
#define LARRECUTILS_LARFEBCONFIGCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "LArRecConditions/LArFebConfig.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <map>

class LArOnlineID;

class LArFEBConfigCondAlg: public AthReentrantAlgorithm {

 public:
  LArFEBConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~LArFEBConfigCondAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  
 private:

  const LArOnlineID* m_onlineID;
  SG::ReadCondHandleKeyArray<CondAttrListCollection> m_listOfFolders;
  SG::WriteCondHandleKey<LArFebConfig> m_configKey{this, "keyOutput", "LArFebConfig", "Output key for LArFebConfig"};   
};

#endif
