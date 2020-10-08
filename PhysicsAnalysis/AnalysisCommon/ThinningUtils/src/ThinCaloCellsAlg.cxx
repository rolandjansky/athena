///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ThinCaloCellsAlg.cxx
// Implementation file for class ThinCaloCellsAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinCaloCellsAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinCaloCellsAlg::ThinCaloCellsAlg( const std::string& name,
																	  ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_thinTool("ThinCaloCellsTool/ThinCaloCellsTool", this),
  m_setCaloCellKey(false),
  m_setInCollKey(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",        m_jos, "The JobOptionService instance.");

  declareProperty("ThinTool",             m_thinTool, "The private ThinningTool" );

  declareProperty("CaloCellsToThin",      m_caloCellKey = "AllCells",
                  "The xAOD::CaloCellContainer to be thinned" );
	m_caloCellKey.declareUpdateHandler( &ThinCaloCellsAlg::setupCaloCellsToThin, this );

  declareProperty("InputContainerList",   m_inCollKeyList,
                  "Containers from which to extract the information which CaloCells should be kept" );
	m_inCollKeyList.declareUpdateHandler( &ThinCaloCellsAlg::setupInputContainerList, this );
}



// Destructor
///////////////
ThinCaloCellsAlg::~ThinCaloCellsAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinCaloCellsAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_thinTool );
  ATH_MSG_DEBUG ( " using = " << m_streamName );
  ATH_MSG_DEBUG ( " using = " << m_caloCellKey );
  ATH_MSG_DEBUG ( " using = " << m_inCollKeyList );

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

  if (m_setCaloCellKey) {
    ATH_MSG_DEBUG( "Setting property" << m_caloCellKey
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_caloCellKey.name(), m_caloCellKey.value() );
  }
  if (m_setInCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_inCollKeyList
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_inCollKeyList.name(), m_inCollKeyList.toString());
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_thinTool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode ThinCaloCellsAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_thinTool.release() );

  return StatusCode::SUCCESS;
}



StatusCode ThinCaloCellsAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the thinning tool
	ATH_CHECK( m_thinTool->doThinning() );

  return StatusCode::SUCCESS;
}
