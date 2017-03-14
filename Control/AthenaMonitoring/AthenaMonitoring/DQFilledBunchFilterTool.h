/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQFILLEDBUNCHFILTERTOOL_H
#define DQFILLEDBUNCHFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// This filter tool only accepts events which are filled (or only those not filled, if m_invert is set)
// @author Peter Onyisi <ponyisi@cern.ch>

class DQFilledBunchFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQFilledBunchFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQFilledBunchFilterTool () override;
        
  virtual StatusCode initialize() override;

  virtual bool accept() const override;

 private:
  bool m_alwaysReturnTrue;
  bool m_invert;
  ToolHandle< Trig::IBunchCrossingTool > m_bunchtool;
};

#endif //DQFILLEDBUNCHFILTERTOOL_H
