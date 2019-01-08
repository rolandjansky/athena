/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQATLASREADYFILTERTOOL_H
#define DQATLASREADYFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// This filter tool rejects events where the ATLAS READY flag is not set
// @author Peter Onyisi <ponyisi@cern.ch>

class DQAtlasReadyFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQAtlasReadyFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQAtlasReadyFilterTool () override;
  
  virtual StatusCode initialize() override;
        
  virtual bool accept() const override;

 private:
  bool m_fallbackValue;
  bool m_alwaysReturnTrue;
  SG::ReadCondHandleKey<AthenaAttributeList> m_key;
};

#endif //DQATLASREADYFILTERTOOL_H
