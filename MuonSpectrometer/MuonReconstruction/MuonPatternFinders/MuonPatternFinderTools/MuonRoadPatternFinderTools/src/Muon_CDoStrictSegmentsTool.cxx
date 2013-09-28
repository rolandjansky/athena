/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoStrictSegmentsTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoStrictSegmentsTool::Muon_CDoStrictSegmentsTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoStrictSegmentsTool>(this);
    ATH_MSG_DEBUG("Muon_CDoStrictSegmentsTool constructor");
    
  }

  Muon_CDoStrictSegmentsTool::~Muon_CDoStrictSegmentsTool()
  {
  }

  StatusCode Muon_CDoStrictSegmentsTool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoStrictSegmentsTool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoStrictSegmentsTool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoStrictSegmentsTool::execute");
  }

  StatusCode Muon_CDoStrictSegmentsTool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoStrictSegmentsTool::finalize");
    return StatusCode::SUCCESS; 
  }
}
