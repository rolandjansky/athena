/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoAllowCellTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoAllowCellTool::Muon_CDoAllowCellTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoAllowCellTool>(this);
    ATH_MSG_DEBUG("Muon_CDoAllowCellTool constructor");
    
  }

  Muon_CDoAllowCellTool::~Muon_CDoAllowCellTool()
  {
  }

  StatusCode Muon_CDoAllowCellTool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoAllowCellTool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoAllowCellTool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoAllowCellTool::execute");
  }

  StatusCode Muon_CDoAllowCellTool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoAllowCellTool::finalize");
    return StatusCode::SUCCESS; 
  }
}
