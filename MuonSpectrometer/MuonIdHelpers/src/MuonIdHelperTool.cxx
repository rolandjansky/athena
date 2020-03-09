/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonIdHelpers/MuonIdHelperTool.h"

namespace Muon {

  MuonIdHelperTool::MuonIdHelperTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa)
  {
    declareInterface<MuonIdHelperTool>(this);
  }

  MuonIdHelperTool::~MuonIdHelperTool()
  {
  }


  StatusCode MuonIdHelperTool::initialize() 
  {
    ATH_CHECK(m_muonIdHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode MuonIdHelperTool::finalize()
  {
    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
}
