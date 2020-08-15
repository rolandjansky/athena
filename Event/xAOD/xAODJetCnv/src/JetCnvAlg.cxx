///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnvAlg.cxx 
// Implementation file for class JetCnvAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetEvent/JetCollection.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

// #include "xAOD"
// #include "Particle/TrackParticleContainer.h"

// xAODJetCnv includes
#include "JetCnvAlg.h"

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
JetCnvAlg::JetCnvAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
  , m_inputJetCollectionName("AntiKt4LCTopoJets")
  , m_xaodJetCollectionName("")
  , m_constitContainerName("")
  , m_cnvTool("xAODMaker::JetCnvTool/JetCnvTool")
{
  //
  // Property declaration
  // 
  declareProperty( "InputJetCollection", m_inputJetCollectionName );
  declareProperty( "xAODJetContainer", m_xaodJetCollectionName = "" );
  declareProperty( "ConstituentContainer", m_constitContainerName = "");
  declareProperty( "JetCnvTool", m_cnvTool);
  declareProperty( "ConstitSearch", m_constitSearch = false);
}

// Destructor
///////////////
JetCnvAlg::~JetCnvAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode JetCnvAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if( m_xaodJetCollectionName == "" ) m_xaodJetCollectionName = m_inputJetCollectionName;

  CHECK( m_cnvTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode JetCnvAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");    
  return StatusCode::SUCCESS;
}



StatusCode JetCnvAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  ATH_MSG_DEBUG( "Converting " << m_inputJetCollectionName << "  into "<< m_xaodJetCollectionName );

  const JetCollection* inputJets=evtStore()->tryConstRetrieve<JetCollection>(m_inputJetCollectionName);
  if (!inputJets) {
    ATH_MSG_WARNING("No JetCollection with key " << m_inputJetCollectionName << " found. Do nothing.");
    return StatusCode::SUCCESS;
  }

  // Create the xAOD container and its auxiliary store:
  xAOD::JetContainer* xaodJets = new xAOD::JetContainer();
  CHECK( evtStore()->record( xaodJets, m_xaodJetCollectionName ) );
  xAOD::JetAuxContainer* auxJets = new xAOD::JetAuxContainer();
  CHECK( evtStore()->record( auxJets, m_xaodJetCollectionName + "Aux." ) );
  xaodJets->setStore( auxJets );
  ATH_MSG_DEBUG( "Recorded xAOD jets with key: "
                     << m_xaodJetCollectionName );


  DataLink<xAOD::IParticleContainer> constitContainer (m_constitContainerName);
  if( !constitContainer ) ATH_MSG_WARNING("User given jet constituent xAOD container : "<< m_constitContainerName <<" not in SG !");

  return m_cnvTool->convert(inputJets, xaodJets, constitContainer );
  
  
}


}
