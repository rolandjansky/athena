///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CutAlg.cxx
// Implementation file for class CutAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "CutAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
CutAlg::CutAlg( const std::string& name,
                ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_skimTool("CutTool/CutTool", this),
  m_cut(""),
  m_setCut(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",       m_jos, "The JobOptionService instance.");

  declareProperty("SkimTool",            m_skimTool, "The private SkimmingTool" );

  declareProperty("Cut",                 m_cut="",   "The name of the output container" );
  m_cut.declareUpdateHandler( &CutAlg::setupCut, this );
}



// Destructor
///////////////
CutAlg::~CutAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode CutAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using JobOptionsSvc = " << m_jos );
  ATH_MSG_DEBUG ( " using SkimTool      = " << m_skimTool );
  ATH_MSG_DEBUG ( " using Cut           = " << m_cut );


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;


  // Get the JobOptionService
  // We will use this to set the properties of our private skimming tool
  // from the properties of this algorithm.
  ATH_MSG_VERBOSE( "Getting the JobOptionService");
  ATH_CHECK( m_jos.retrieve() );

  // Get the full name of the private skimTool
  ATH_MSG_VERBOSE( "Getting the full name of the tool");
  const std::string& fullToolName = this->name() + "." + m_skimTool.name();
  ATH_MSG_DEBUG( "Got the full name of the tool: " << fullToolName );

  // Now, set all properties of the private skimTool that were acutally configured
  if (m_setCut) {
    ATH_MSG_DEBUG( "Setting property" << m_cut
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_cut) );
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_skimTool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode CutAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_skimTool.release() );

  return StatusCode::SUCCESS;
}



StatusCode CutAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the skimming tool and set its result
  bool eventPasses = m_skimTool->eventPassesFilter();
  this->setFilterPassed( eventPasses );
  ATH_MSG_DEBUG("Event passes/fails: " << eventPasses );

  return StatusCode::SUCCESS;
}
