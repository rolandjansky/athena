/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQBadLBFILTERTOOL_H
#define DQBadLBFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <string>
#include <vector>

// This filter tool rejects events where the a bad LB block is flagged from a given defect
// @author Michele Petteni <mpetteni@sfu.ca>

class DQBadLBFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQBadLBFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQBadLBFilterTool () override;
  
  virtual StatusCode initialize() override;
        
  virtual bool accept() const override;

 private:
  bool m_fallbackValue;
  bool m_alwaysReturnTrue;
  SG::ReadCondHandleKey<AthenaAttributeList> m_key;
};

#endif //DQATLASREADYFILTERTOOL_H
