///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METElectronAssociator.cxx
// Implementation file for class METElectronAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
///////////////////////////////////////////////////////////////////

// METReconstruction includes
#include "METReconstruction/METElectronAssociator.h"

// Egamma EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Photon.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METElectronAssociator::METElectronAssociator(const std::string& name) :
    AsgTool(name),
    METAssociator(name),
    METEgammaAssociator(name)
  {}

  // Destructor
  ///////////////
  METElectronAssociator::~METElectronAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METElectronAssociator::initialize()
  {
    ATH_CHECK( METEgammaAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METElectronAssociator::finalize()
  {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
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

  // executeTool
  ////////////////
  StatusCode METElectronAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const ElectronContainer* elCont(0);
    if( evtStore()->retrieve(elCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    if (fillAssocMap(metMap,elCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

}
