/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuGirlLowBetaTagTool
//  AlgTool performing in-out reconstruction using the MuGirl package.
//  A MuGirlLowBetaTag is added to the InDetCandidate object.
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuGirlLowBetaTagTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuGirlLowBetaTagTool::MuGirlLowBetaTagTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent)
  {
    declareInterface<IMuonCombinedInDetExtensionTool>(this);

  }

  MuGirlLowBetaTagTool::~MuGirlLowBetaTagTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuGirlLowBetaTagTool::initialize() {
    ATH_MSG_INFO( "Initializing MuGirlLowBetaTagTool - package version " << PACKAGE_VERSION );

    return StatusCode::SUCCESS;
  }

  StatusCode MuGirlLowBetaTagTool::finalize() {
    return StatusCode::SUCCESS;
  }

}	// end of namespace
