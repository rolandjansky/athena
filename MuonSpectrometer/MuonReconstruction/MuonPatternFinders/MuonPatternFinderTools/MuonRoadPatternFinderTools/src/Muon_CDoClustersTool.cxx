/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoClustersTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoClustersTool::Muon_CDoClustersTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoClustersTool>(this);
    ATH_MSG_DEBUG("Muon_CDoClustersTool constructor");
    
  }

  Muon_CDoClustersTool::~Muon_CDoClustersTool()
  {
  }

  StatusCode Muon_CDoClustersTool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoClustersTool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoClustersTool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoClustersTool::execute");
  }

  StatusCode Muon_CDoClustersTool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoClustersTool::finalize");
    return StatusCode::SUCCESS; 
  }
}
