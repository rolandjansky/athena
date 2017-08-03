/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQEVENTFLAGFILTERTOOL_H
#define DQEVENTFLAGFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

// This filter tool only accepts events which do not fail DP event cleaning cuts
// @author Peter Onyisi <ponyisi@cern.ch>

class DQEventFlagFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQEventFlagFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQEventFlagFilterTool () override;
        
  virtual StatusCode initialize() override;

  virtual bool accept() const override;

 private:
  bool m_alwaysReturnTrue;
  bool m_invert;
  bool m_doLAr;
  bool m_doTile;
  bool m_doSCT;
  bool m_doCore;
};

#endif //DQEVENTFLAGFILTERTOOL_H
