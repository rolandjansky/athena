/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Muon_CDoMDTROATool.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

namespace Muon {

  Muon_CDoMDTROATool::Muon_CDoMDTROATool(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p)
  {
    declareInterface<IMuon_CDoMDTROATool>(this);
    ATH_MSG_DEBUG("Muon_CDoMDTROATool constructor");
    
  }

  Muon_CDoMDTROATool::~Muon_CDoMDTROATool()
  {
  }

  StatusCode Muon_CDoMDTROATool::initialize()
  {
    ATH_MSG_DEBUG("Muon_CDoMDTROATool::initialize");
    return StatusCode::SUCCESS; 
  }

  void Muon_CDoMDTROATool::execute() const {
    ATH_MSG_DEBUG("Muon_CDoMDTROATool::execute");
  }

  StatusCode Muon_CDoMDTROATool::finalize()
  {
    ATH_MSG_DEBUG("Muon_CDoMDTROATool::finalize");
    return StatusCode::SUCCESS; 
  }
}
