/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoLooseSegmentsTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoLooseSegmentsTool::Muon_CDoLooseSegmentsTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoLooseSegmentsTool>(this);
    ATH_MSG_DEBUG("Muon_CDoLooseSegmentsTool constructor");
    
  }

  Muon_CDoLooseSegmentsTool::~Muon_CDoLooseSegmentsTool()
  {
  }

  StatusCode Muon_CDoLooseSegmentsTool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoLooseSegmentsTool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoLooseSegmentsTool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoLooseSegmentsTool::execute");
  }

  StatusCode Muon_CDoLooseSegmentsTool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoLooseSegmentsTool::finalize");
    return StatusCode::SUCCESS; 
  }
}
