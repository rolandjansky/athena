/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQDUMMYFILTERTOOL_H
#define DQDUMMYFILTERTOOL_H

#include "AthenaMonitoring/IDQFilterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

// This filter tool rejects events where the ATLAS READY flag is not set
// @author Peter Onyisi <ponyisi@cern.ch>

class DQDummyFilterTool :  public AthAlgTool, virtual public IDQFilterTool   {
 public:
  DQDummyFilterTool(const std::string&,const std::string&,const IInterface*);
        
  virtual ~DQDummyFilterTool () override;
        
  virtual bool accept() const override;

};

#endif //DQDUMMYFILTERTOOL_H
