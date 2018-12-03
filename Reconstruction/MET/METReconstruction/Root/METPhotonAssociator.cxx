///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METPhotonAssociator.cxx
// Implementation file for class METPhotonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
///////////////////////////////////////////////////////////////////

// METReconstruction includes
#include "METReconstruction/METPhotonAssociator.h"

// Egamma EDM
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METPhotonAssociator::METPhotonAssociator(const std::string& name) :
    AsgTool(name),
    METAssociator(name),
    METEgammaAssociator(name)
  {}

  // Destructor
  ///////////////
  METPhotonAssociator::~METPhotonAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METPhotonAssociator::initialize()
  {
    ATH_CHECK( METEgammaAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METPhotonAssociator::finalize()
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
  StatusCode METPhotonAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const xAOD::PhotonContainer* phCont(0);
    if( evtStore()->retrieve(phCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input photon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved photon collection");

    if (fillAssocMap(metMap,phCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with photon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

}
