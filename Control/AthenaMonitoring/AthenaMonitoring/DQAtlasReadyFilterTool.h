/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQATLASREADYFILTERTOOL_H
#define DQATLASREADYFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

// This filter tool rejects events where the ATLAS READY flag is not set
// @author Peter Onyisi <ponyisi@cern.ch>

class DQAtlasReadyFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQAtlasReadyFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQAtlasReadyFilterTool () override;
  
  virtual StatusCode initialize() override;
        
  virtual bool accept() const override;

  virtual StatusCode updateCache();

  StatusCode callback( IOVSVC_CALLBACK_ARGS );

 private:
  bool m_fallbackValue;
  bool m_alwaysReturnTrue;
  bool m_valueCache;
};

#endif //DQATLASREADYFILTERTOOL_H
