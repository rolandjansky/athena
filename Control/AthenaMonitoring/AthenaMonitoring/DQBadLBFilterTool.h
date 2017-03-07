/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQBadLBFILTERTOOL_H
#define DQBadLBFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

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

  virtual StatusCode updateCache();

  StatusCode callback( IOVSVC_CALLBACK_ARGS );

 private:
  bool m_fallbackValue;
  bool m_ignoreRecoverable;
  bool m_alwaysReturnTrue;
  bool m_valueCache;
  std::vector<int> m_listofdefects;
  std::vector<std::string> m_listofdefects_str;
};

#endif //DQATLASREADYFILTERTOOL_H
