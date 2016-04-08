///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSelectionAlg.cxx
// Implementation file for class ParticleSelectionAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ParticleSelectionAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ParticleSelectionAlg::ParticleSelectionAlg( const std::string& name,
                                            ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_tool("ParticleSelectionTool/ParticleSelectionTool", this),
  m_inCollKey(""),
  m_setInCollKey(false),
  m_outCollKey(""),
  m_setOutCollKey(false),
  m_outCollType(""),
  m_setOutCollType(false),
  m_writeSplitAux(false),
  m_setWriteSplitAux(false),
  m_outOwnPolicyName("OWN_ELEMENTS"),
  m_setOwnPolicy(false),
  m_outLinkCollKey(""),
  m_setOutLinkCollKey(false),
  m_selection(""),
  m_setSelection(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",       m_jos, "The JobOptionService instance.");

  declareProperty("SelectionTool",       m_tool, "The private tool that will select the particles within a container" );

  declareProperty("InputContainer",      m_inCollKey="",   "Input container name" );
  m_inCollKey.declareUpdateHandler( &ParticleSelectionAlg::setupInputContainer, this );

  declareProperty("OutputContainer",     m_outCollKey="",
                  "The name of the output container with the deep copy of selected xAOD::IParticles" );
  m_outCollKey.declareUpdateHandler( &ParticleSelectionAlg::setupOutputContainer, this );

  declareProperty("OutputContainerType", m_outCollType="",
                  "The type of the output container, e.g., 'xAOD::JetContainer'" );
  m_outCollType.declareUpdateHandler( &ParticleSelectionAlg::setupOutputContainerType, this );

  declareProperty("WriteSplitOutputContainer", m_writeSplitAux=false,
                  "Decide if we want to write a fully-split AuxContainer such that we can remove any variables" );
  m_writeSplitAux.declareUpdateHandler( &ParticleSelectionAlg::setupWriteSplitOutputContainer, this );

  declareProperty("OutputContainerOwnershipPolicy", m_outOwnPolicyName="OWN_ELEMENTS",
                  "Defines the ownership policy of the output container (default: 'OWN_ELEMENTS'; also allowed: 'VIEW_ELEMENTS')" );
  m_outOwnPolicyName.declareUpdateHandler( &ParticleSelectionAlg::setupOutputContainerOwnPolicy, this );

  declareProperty("OutputLinkContainer", m_outLinkCollKey="",
                  "The name of the output container of ElementLinks to the selected xAOD::IParticles" );
  m_outLinkCollKey.declareUpdateHandler( &ParticleSelectionAlg::setupOutputLinkContainer, this );

  declareProperty("Selection",           m_selection="",
                  "The selection string that defines which xAOD::IParticles to select from the container" );
  m_selection.declareUpdateHandler( &ParticleSelectionAlg::setupSelection, this );
}



// Destructor
///////////////
ParticleSelectionAlg::~ParticleSelectionAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode ParticleSelectionAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_tool );
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollType );
  ATH_MSG_DEBUG ( " using = " << m_writeSplitAux );
  ATH_MSG_DEBUG ( " using = " << m_outOwnPolicyName );
  ATH_MSG_DEBUG ( " using = " << m_outLinkCollKey );
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
  const std::string& fullToolName = this->name() + "." + m_tool.name();
  ATH_MSG_DEBUG( "Got the full name of the tool: " << fullToolName );

  // Now, set all properties of the private skimTool that were acutally configured
  if (m_setInCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_inCollKey
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_inCollKey) );
  }
  if (m_setOutCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_outCollKey
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_outCollKey) );
  }
  if (m_setOutCollType) {
    ATH_MSG_DEBUG( "Setting property" << m_outCollType
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_outCollType) );
  }
  if (m_setWriteSplitAux) {
    ATH_MSG_DEBUG( "Setting property" << m_writeSplitAux
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_writeSplitAux) );
  }
  if (m_setOwnPolicy) {
    ATH_MSG_DEBUG( "Setting property" << m_outOwnPolicyName
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_outOwnPolicyName) );
  }
  if (m_setOutLinkCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_outLinkCollKey
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_outLinkCollKey) );
  }
  if (m_setSelection) {
    ATH_MSG_DEBUG( "Setting property" << m_selection
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_selection) );
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_tool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode ParticleSelectionAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_tool.release() );

  return StatusCode::SUCCESS;
}



StatusCode ParticleSelectionAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the tool and record resulting containers into StoreGate
  ATH_CHECK( m_tool->addBranches() );

  return StatusCode::SUCCESS;
}
