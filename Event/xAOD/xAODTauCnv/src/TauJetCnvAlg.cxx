///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TauJetCnvAlg.cxx 
// Implementation file for class TauJetCnvAlg
// Author: Michel Janus janus@cern.ch
/////////////////////////////////////////////////////////////////// 


// xAODTauJetCnv includes
#include "TauJetCnvAlg.h"

// Old tau Includes:
#include "tauEvent/TauJetContainer.h"

//New tau
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "AthenaKernel/errorcheck.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

namespace xAODMaker {
// Constructors
////////////////
TauJetCnvAlg::TauJetCnvAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
  , m_inputTauJetContainerName("TauRecContainer")
  , m_xaodTauJetContainerName("TauRecContainer")
{
  //
  // Property declaration
  // 
  declareProperty( "InputTauJetContainer", m_inputTauJetContainerName );
  declareProperty( "xAODTauJetContainer", m_xaodTauJetContainerName );
  declareProperty("CnvTool", m_cnvTool, "The converter tool for TauJets");
}

// Destructor
///////////////
TauJetCnvAlg::~TauJetCnvAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode TauJetCnvAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if( m_xaodTauJetContainerName == "" ) m_xaodTauJetContainerName = m_inputTauJetContainerName;

  CHECK(m_cnvTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TauJetCnvAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  
  
  return StatusCode::SUCCESS;
}

StatusCode TauJetCnvAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  ATH_MSG_DEBUG( "Converting " << m_inputTauJetContainerName << "  into "<< m_xaodTauJetContainerName );
  
  const Analysis::TauJetContainer * inputTaus=evtStore()->tryConstRetrieve< Analysis::TauJetContainer>(m_inputTauJetContainerName);
  if (!inputTaus) {
    ATH_MSG_WARNING("No TauJetContainer with key " << m_inputTauJetContainerName << " found. Do nothing.");
    return StatusCode::SUCCESS;
  }
  
  // Create the xAOD container and its auxiliary store:
  xAOD::TauJetContainer* xaodTauJets = new xAOD::TauJetContainer();
  CHECK( evtStore()->record( xaodTauJets, m_xaodTauJetContainerName ) );
  xAOD::TauJetAuxContainer* auxTauJets = new xAOD::TauJetAuxContainer();
  CHECK( evtStore()->record( auxTauJets, m_xaodTauJetContainerName + "Aux." ) );
  xaodTauJets->setStore( auxTauJets );
  ATH_MSG_DEBUG( "Recorded xAOD tau jets with key: "
		 << m_xaodTauJetContainerName );
 
  CHECK( m_cnvTool->convert(inputTaus, xaodTauJets) );


  return StatusCode::SUCCESS;
}

}


//  LocalWords:  tempfloat
