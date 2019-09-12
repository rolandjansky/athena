/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQEVENTFLAGFILTERTOOL_H
#define DQEVENTFLAGFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"

// This filter tool only accepts events which do not fail DP event cleaning cuts
// @author Peter Onyisi <ponyisi@cern.ch>

class DQEventFlagFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQEventFlagFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQEventFlagFilterTool () override;
        
  virtual StatusCode initialize() override;

  virtual bool accept() const override;

 private:
  Gaudi::Property<bool> m_alwaysReturnTrue{this, "alwaysReturnTrue", false};
  Gaudi::Property<bool> m_invert{this, "invert", false};
  Gaudi::Property<bool> m_doLAr{this, "doLAr", true};
  Gaudi::Property<bool> m_doTile{this, "doTile", true};
  Gaudi::Property<bool> m_doSCT{this, "doSCT", true};
  Gaudi::Property<bool> m_doCore{this, "doCore", true};
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};
};

#endif //DQEVENTFLAGFILTERTOOL_H
