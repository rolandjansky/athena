/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoErrorSegmentTool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoErrorSegmentTool::Muon_CDoErrorSegmentTool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoErrorSegmentTool>(this);
    ATH_MSG_DEBUG("Muon_CDoErrorSegmentTool constructor");
    
  }

  Muon_CDoErrorSegmentTool::~Muon_CDoErrorSegmentTool()
  {
  }

  StatusCode Muon_CDoErrorSegmentTool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoErrorSegmentTool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoErrorSegmentTool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoErrorSegmentTool::execute");
  }

  StatusCode Muon_CDoErrorSegmentTool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoErrorSegmentTool::finalize");
    return StatusCode::SUCCESS; 
  }
}
