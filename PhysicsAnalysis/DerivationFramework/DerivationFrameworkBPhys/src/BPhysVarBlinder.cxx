/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysVarBlinder.cxx
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// 
//============================================================================
//
#include "BPhysTools/BPhysBlindingTool.h"
#include "DerivationFrameworkBPhys/BPhysVarBlinder.h"

namespace DerivationFramework {

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  BPhysVarBlinder::BPhysVarBlinder(const std::string& t,
                                   const std::string& n,
                                   const IInterface* p) : 
    CfAthAlgTool(t,n,p),
    m_blindingTool("xAOD::BPhysBlindingTool") {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    // Declare tools    
    declareProperty("BlindingTool", m_blindingTool);

    // Declare user-defined properties
    declareProperty("EnableBlinding", m_enableBlinding = true);

  }
  //---------------------------------------------------------------------------
  // Initialization
  //---------------------------------------------------------------------------
  StatusCode BPhysVarBlinder::initialize() {
  
    ATH_MSG_DEBUG("in initialize()");
    
    // retrieve blinding tool
    CHECK( m_blindingTool.retrieve() );
    
    ATH_MSG_INFO("initialize(): EnableBlinding: " << m_enableBlinding);
    
    return StatusCode::SUCCESS;
  }
  //---------------------------------------------------------------------------
  // Finalization
  //---------------------------------------------------------------------------
  StatusCode BPhysVarBlinder::finalize() {

    // everything all right
    return StatusCode::SUCCESS;
  }
  //---------------------------------------------------------------------------
  // Perform blinding
  //---------------------------------------------------------------------------
  StatusCode BPhysVarBlinder::addBranches() const {
    
    if ( m_enableBlinding ) {
      CHECK( m_blindingTool->doBlind() );
    }
    
    return StatusCode::SUCCESS;
  }
  //---------------------------------------------------------------------------

} // namespace DerivationFramework
