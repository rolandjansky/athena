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
    METEgammaAssociator(name),
    m_elContKey("")
  {
    declareProperty("ElectronContainer",m_elContKey);
    
  }

  // Destructor
  ///////////////
  METElectronAssociator::~METElectronAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METElectronAssociator::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    ATH_CHECK( m_elContKey.assign(m_input_data_key));
    ATH_CHECK( m_elContKey.initialize());
    ATH_CHECK( METEgammaAssociator::initialize() );

    if (m_usePFOLinks || m_usePFOElectronLinks) {
        if (m_electronNeutralPFOReadDecorKey.key()=="") {ATH_CHECK( m_electronNeutralPFOReadDecorKey.assign(m_input_data_key+"."+m_neutralPFOLinksKey));} 
        if (m_electronChargedPFOReadDecorKey.key()=="") {ATH_CHECK( m_electronChargedPFOReadDecorKey.assign(m_input_data_key+"."+m_chargedPFOLinksKey));} 
	ATH_CHECK(m_electronNeutralPFOReadDecorKey.initialize());
    	ATH_CHECK(m_electronChargedPFOReadDecorKey.initialize());
    }
    if (m_useFELinks || m_useFEElectronLinks) {
        if (m_electronNeutralFEReadDecorKey.key()=="")  {ATH_CHECK( m_electronNeutralFEReadDecorKey.assign(m_input_data_key+"."+m_neutralFELinksKey));} 
        if (m_electronChargedFEReadDecorKey.key()=="")  {ATH_CHECK( m_electronChargedFEReadDecorKey.assign(m_input_data_key+"."+m_chargedFELinksKey));} 
    	ATH_CHECK(m_electronNeutralFEReadDecorKey.initialize());
    	ATH_CHECK(m_electronChargedFEReadDecorKey.initialize());
    }

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

    SG::ReadHandle<xAOD::ElectronContainer> elCont(m_elContKey);
    if (!elCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    if (fillAssocMap(metMap,elCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with electron container " << m_input_data_key);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

}
