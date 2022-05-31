///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  { 
  }

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METElectronAssociator::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    ATH_CHECK( m_elContKey.initialize());
    ATH_CHECK( METEgammaAssociator::initialize() );

    if (m_usePFOLinks) {
        if (m_electronNeutralPFOReadDecorKey.empty()) {ATH_CHECK( m_electronNeutralPFOReadDecorKey.assign(m_elContKey.key()+"."+m_neutralPFOLinksKey));} 
        if (m_electronChargedPFOReadDecorKey.empty()) {ATH_CHECK( m_electronChargedPFOReadDecorKey.assign(m_elContKey.key()+"."+m_chargedPFOLinksKey));} 
	ATH_CHECK(m_electronNeutralPFOReadDecorKey.initialize());
    	ATH_CHECK(m_electronChargedPFOReadDecorKey.initialize());
    }
    if (m_useFELinks) {
        if (m_electronNeutralFEReadDecorKey.empty())  {ATH_CHECK( m_electronNeutralFEReadDecorKey.assign(m_elContKey.key()+"."+m_neutralFELinksKey));} 
        if (m_electronChargedFEReadDecorKey.empty())  {ATH_CHECK( m_electronChargedFEReadDecorKey.assign(m_elContKey.key()+"."+m_chargedFELinksKey));} 
    	ATH_CHECK(m_electronNeutralFEReadDecorKey.initialize());
    	ATH_CHECK(m_electronChargedFEReadDecorKey.initialize());
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
  StatusCode METElectronAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    SG::ReadHandle<xAOD::ElectronContainer> elCont(m_elContKey);
    if (!elCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_elContKey.key());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    if (fillAssocMap(metMap,elCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with electron container " << m_elContKey.key());
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

}
