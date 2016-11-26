///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRefinerTool.cxx 
// Implementation file for class METRefinerTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METRefinerTool.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETComponentMap.h"

// For DeltaR
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METRefinerTool::METRefinerTool(const std::string& name) : 
    AsgTool(name)
  {
    declareProperty( "MissingETKey",    m_output_met_key  );
    declareProperty( "UseRapidity",     m_useRapidity = false );
  }

  // Destructor
  ///////////////
  METRefinerTool::~METRefinerTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METRefinerTool::initialize()
  {
    ATH_MSG_DEBUG ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METRefinerTool::execute(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    if( metTerm==0 ) {
      ATH_MSG_ERROR("Invalid input MissingET pointer provided!");
      return StatusCode::FAILURE;
    }
    if( metMap==0 ) {
      ATH_MSG_ERROR("Invalid input MissingETComponentMap pointer provided!");
      return StatusCode::FAILURE;
    }

    metTerm->setName(m_output_met_key);
    return this->executeTool(metTerm,metMap);
  }

  StatusCode METRefinerTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
