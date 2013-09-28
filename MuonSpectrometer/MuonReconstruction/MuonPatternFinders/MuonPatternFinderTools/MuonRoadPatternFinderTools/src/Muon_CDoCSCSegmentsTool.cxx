/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoCSCSegmentsTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoCSCSegmentsTool::Muon_CDoCSCSegmentsTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoCSCSegmentsTool>(this);
    ATH_MSG_DEBUG("Muon_CDoCSCSegmentsTool constructor");
    
  }

  Muon_CDoCSCSegmentsTool::~Muon_CDoCSCSegmentsTool()
  {
  }

  StatusCode Muon_CDoCSCSegmentsTool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoCSCSegmentsTool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoCSCSegmentsTool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoCSCSegmentsTool::execute");
  }

  StatusCode Muon_CDoCSCSegmentsTool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoCSCSegmentsTool::finalize");
    return StatusCode::SUCCESS; 
  }
}
