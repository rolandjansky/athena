///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSortingAlg.cxx
// Implementation file for class ParticleSortingAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ParticleSortingAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ParticleSortingAlg::ParticleSortingAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_tool("ParticleSortingTool/ParticleSortingTool", this),
  m_inCollKey(""),
  m_setInCollKey(false),
  m_outCollKey(""),
  m_setOutCollKey(false),
  m_sortVar("pt"),
  m_setSortVar(false),
  m_sortDescending(true),
  m_setSortDescending(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",   m_jos, "The JobOptionService instance.");

  declareProperty("SortingTool",        m_tool, "The private ParticleSortingTool" );

  declareProperty("InputContainer",  m_inCollKey="",   "Input container name" );
  m_inCollKey.declareUpdateHandler( &ParticleSortingAlg::setupInputContainer, this );

  declareProperty("OutputContainer", m_outCollKey="",
                  "The name of the output container (with SG::VIEW_ELEMENTS) with the sorted copy of input objects" );
  m_outCollKey.declareUpdateHandler( &ParticleSortingAlg::setupOutputContainer, this );

  declareProperty("SortVariable",    m_sortVar="pt",
                  "Define by what parameter to sort (default: 'pt'; allowed: 'pt', 'eta', 'phi', 'm', 'e', 'rapidity')" );
  m_sortVar.declareUpdateHandler( &ParticleSortingAlg::setupSortVar, this );

  declareProperty("SortDescending",   m_sortDescending=true,
                  "Define if the container should be sorted in a descending order (default=true)" );
  m_sortDescending.declareUpdateHandler( &ParticleSortingAlg::setupSortDescending, this );
}



// Destructor
///////////////
ParticleSortingAlg::~ParticleSortingAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode ParticleSortingAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_tool );
  ATH_MSG_DEBUG ( " using = " << m_inCollKey );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );
  ATH_MSG_DEBUG ( " using = " << m_sortVar );
  ATH_MSG_DEBUG ( " using = " << m_sortDescending );


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
  if (m_setSortVar) {
    ATH_MSG_DEBUG( "Setting property" << m_sortVar
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_sortVar) );
  }
  if (m_setSortDescending) {
    ATH_MSG_DEBUG( "Setting property" << m_sortDescending
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_sortDescending) );
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_tool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode ParticleSortingAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_tool.release() );

  return StatusCode::SUCCESS;
}



StatusCode ParticleSortingAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the tool
  ATH_CHECK( m_tool->addBranches() );

  return StatusCode::SUCCESS;
}
