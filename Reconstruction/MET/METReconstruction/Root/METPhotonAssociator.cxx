///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  {
  }

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METPhotonAssociator::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    ATH_CHECK( m_phContKey.initialize());
    ATH_CHECK( METEgammaAssociator::initialize() );


    if (m_usePFOLinks) {
        if (m_photonNeutralPFOReadDecorKey.empty()) {ATH_CHECK( m_photonNeutralPFOReadDecorKey.assign(m_phContKey.key()+"."+m_neutralPFOLinksKey));} 
        if (m_photonChargedPFOReadDecorKey.empty()) {ATH_CHECK( m_photonChargedPFOReadDecorKey.assign(m_phContKey.key()+"."+m_chargedPFOLinksKey));} 
	ATH_CHECK(m_photonNeutralPFOReadDecorKey.initialize());
    	ATH_CHECK(m_photonChargedPFOReadDecorKey.initialize());

    }
    if (m_useFELinks) {
        if (m_photonNeutralFEReadDecorKey.empty())  {ATH_CHECK( m_photonNeutralFEReadDecorKey.assign(m_phContKey.key()+"."+m_neutralFELinksKey));} 
        if (m_photonChargedFEReadDecorKey.empty())  {ATH_CHECK( m_photonChargedFEReadDecorKey.assign(m_phContKey.key()+"."+m_chargedFELinksKey));} 
    	ATH_CHECK(m_photonNeutralFEReadDecorKey.initialize());
    	ATH_CHECK(m_photonChargedFEReadDecorKey.initialize());
    }
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

    SG::ReadHandle<xAOD::PhotonContainer> phCont(m_phContKey);
    if (!phCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input photon container " << m_phContKey.key());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved photon collection");

    if (fillAssocMap(metMap,phCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with photon container " << m_phContKey.key());
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

}
