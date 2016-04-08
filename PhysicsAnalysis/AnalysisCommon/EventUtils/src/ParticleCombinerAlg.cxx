/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: ParticleCombinerAlg.cxx
//
/**
   @class ParticleCombinerAlg

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date July 2014

   @brief Combine particles to composite particles

   Class to combine two or more particles from a given list of
   input containers. All combinations will be tried and only the
   ones passing the used selections will be written to StoreGate.
*/
//=============================================================================

// This classes header
#include "ParticleCombinerAlg.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

// Framework includes
#include "GaudiKernel/IJobOptionsSvc.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"



//=============================================================================
// Constructor
//=============================================================================
ParticleCombinerAlg::ParticleCombinerAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_tool("ParticleCombinerTool/ParticleCombinerTool", this),
  m_inCollKeyList(),
  m_setInCollKeyList(false),
  m_metName(""),
  m_setMetName(false),
  m_outCollKey(""),
  m_setOutCollKey(false),
  m_writeSplitAux(false),
  m_setWriteSplitAux(false),
  m_pdgId(0),
  m_setPdgId(false),
  m_setSort(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc", m_jos, "The JobOptionService instance.");

  declareProperty("ParticleCombinerTool",  m_tool, "The private tool that will do the particle combinatorics" );

  declareProperty("InputContainerList", m_inCollKeyList,   "List of input collection keys" );
  m_inCollKeyList.declareUpdateHandler( &ParticleCombinerAlg::setupInputContainerList, this );

  declareProperty("MissingETObject",    m_metName="Final", "The name of the xAOD::MissingET object (default: 'Final'" );
  m_metName.declareUpdateHandler( &ParticleCombinerAlg::setupMissingETObject, this );

  declareProperty("OutputContainer",    m_outCollKey="",   "The name of the output container" );
  m_outCollKey.declareUpdateHandler( &ParticleCombinerAlg::setupOutputContainer, this );

  declareProperty("WriteSplitOutputContainer", m_writeSplitAux=false,
                  "Decide if we want to write a fully-split AuxContainer such that we can remove any variables" );
  m_writeSplitAux.declareUpdateHandler( &ParticleCombinerAlg::setupWriteSplitOutputContainer, this );

  declareProperty("SetPdgId",           m_pdgId=0,         "PDG ID of the new output xAOD::CompositeParticle" );
  m_pdgId.declareUpdateHandler( &ParticleCombinerAlg::setupSetPdgId, this );

  declareProperty("SortConstituents",   m_sortConstit=false, "If true: sort the constituents in decending pt order" );
  m_sortConstit.declareUpdateHandler( &ParticleCombinerAlg::setupSort, this );
}







//=============================================================================
// Destructor
//=============================================================================
ParticleCombinerAlg::~ParticleCombinerAlg()
{
}



//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode ParticleCombinerAlg::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_tool );
  ATH_MSG_DEBUG ( " using = " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using = " << m_metName );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );
  ATH_MSG_DEBUG ( " using = " << m_writeSplitAux );
  ATH_MSG_DEBUG ( " using = " << m_pdgId );
  ATH_MSG_DEBUG ( " using = " << m_sortConstit );


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;


  // Get the JobOptionService
  // We will use this to set the properties of our private tool
  // from the properties of this algorithm.
  ATH_MSG_VERBOSE( "Getting the JobOptionService");
  ATH_CHECK( m_jos.retrieve() );

  // Get the full name of the private skimTool
  ATH_MSG_VERBOSE( "Getting the full name of the tool");
  const std::string& fullToolName = this->name() + "." + m_tool.name();
  ATH_MSG_DEBUG( "Got the full name of the tool: " << fullToolName );

  // Now, set all properties of the private skimTool that were acutally configured
  if (m_setInCollKeyList) {
    ATH_MSG_DEBUG( "Setting property" << m_inCollKeyList
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_inCollKeyList) );
  }
  if (m_setMetName) {
    ATH_MSG_DEBUG( "Setting property" << m_metName
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_metName) );
  }
  if (m_setOutCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_outCollKey
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_outCollKey) );
  }
  if (m_setWriteSplitAux) {
    ATH_MSG_DEBUG( "Setting property" << m_writeSplitAux
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_writeSplitAux) );
  }
  if (m_setPdgId) {
    ATH_MSG_DEBUG( "Setting property" << m_pdgId
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_pdgId) );
  }
  if (m_setSort) {
    ATH_MSG_DEBUG( "Setting property" << m_sortConstit
                   << " of private tool with name: '" << fullToolName << "'" );
    ATH_CHECK( m_jos->addPropertyToCatalogue (fullToolName,m_sortConstit) );
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the tool
  ATH_CHECK( m_tool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode ParticleCombinerAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the tool to build the particle combinatorics and record the result to StoreGate
  ATH_CHECK( m_tool->addBranches() );

  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ParticleCombinerAlg::finalize()
{
  ATH_MSG_DEBUG ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED. "
                  << "Saw " << m_nEventsProcessed << " events." );

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_tool.release() );


  return StatusCode::SUCCESS;
}
