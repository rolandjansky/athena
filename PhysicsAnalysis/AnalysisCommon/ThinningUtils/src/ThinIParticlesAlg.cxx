///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ThinIParticlesAlg.cxx
// Implementation file for class ThinIParticlesAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinIParticlesAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinIParticlesAlg::ThinIParticlesAlg( const std::string& name,
																		  ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_thinTool("ThinIParticlesTool/ThinIParticlesTool", this),
	m_setIPartKey(false),
	m_setInCollKey(false),
	m_setSelection(false),
	m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",        m_jos, "The JobOptionService instance.");

  declareProperty("ThinTool",             m_thinTool, "The private ThinningTool" );

  declareProperty("IParticlesToThin",     m_ipartKey = "",
                  "The xAOD::IParticleContainer to be thinned" );
	m_ipartKey.declareUpdateHandler( &ThinIParticlesAlg::setupIParticlesToThin, this );

  declareProperty("InputContainerList",   m_inCollKeyList,
                  "Containers from which to extract the information which xAOD::IParticles should be kept" );
	m_inCollKeyList.declareUpdateHandler( &ThinIParticlesAlg::setupInputContainerList, this );

  declareProperty("Selection",            m_selection="",
								  "The selection string that defines which xAOD::IParticles to select from the container" );
	m_selection.declareUpdateHandler( &ThinIParticlesAlg::setupSelection, this );
}



// Destructor
///////////////
ThinIParticlesAlg::~ThinIParticlesAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinIParticlesAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_thinTool );
  ATH_MSG_DEBUG ( " using = " << m_streamName );
  ATH_MSG_DEBUG ( " using = " << m_ipartKey );
  ATH_MSG_DEBUG ( " using = " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using = " << m_selection );

  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;

  // Get the JobOptionService
  // We will use this to set the properties of our private skimming tool
  // from the properties of this algorithm.
  ATH_MSG_VERBOSE( "Getting the JobOptionService");
  ATH_CHECK( m_jos.retrieve() );

  // Get the full name of the private skimTool
  ATH_MSG_VERBOSE( "Getting the full name of the tool");
  const std::string& fullToolName = this->name() + "." + m_thinTool.name();
  ATH_MSG_DEBUG( "Got the full name of the tool: " << fullToolName );

  // Now, set all properties of the private skimTool that were acutally configured
	ATH_MSG_DEBUG( "Setting property" << m_streamName
                 << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + ".StreamName", m_streamName.value());

  if (m_setIPartKey) {
    ATH_MSG_DEBUG( "Setting property" << m_ipartKey
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_ipartKey.name(), m_ipartKey.value());
  }
  if (m_setInCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_inCollKeyList
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_inCollKeyList.name(), m_inCollKeyList.toString());
  }
  if (m_setSelection) {
    ATH_MSG_DEBUG( "Setting property" << m_selection
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_selection.name(), m_selection.value());
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_thinTool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode ThinIParticlesAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_thinTool.release() );

  return StatusCode::SUCCESS;
}



StatusCode ThinIParticlesAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the thinning tool
	ATH_CHECK( m_thinTool->doThinning() );

  return StatusCode::SUCCESS;
}
