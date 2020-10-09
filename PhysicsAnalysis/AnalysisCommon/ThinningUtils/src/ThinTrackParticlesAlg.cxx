///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ThinTrackParticlesAlg.cxx
// Implementation file for class ThinTrackParticlesAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinTrackParticlesAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinTrackParticlesAlg::ThinTrackParticlesAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator ) :
  ::AthAlgorithm( name, pSvcLocator ),
  m_jos("JobOptionsSvc", name),
  m_thinTool("ThinTrackParticlesTool/ThinTrackParticlesTool", this),
  m_setTrackPartKey(false),
  m_setInCollKey(false),
  m_setSelection(false),
  m_setTauConv(false),
  m_setTauWide(false),
  m_setTauOther(false),
  m_nElectronPTMax(-1),
  m_setNEleTPMax(false),
  m_nEventsProcessed(0)
{
  declareProperty("JobOptionsSvc",        m_jos, "The JobOptionService instance.");

  declareProperty("ThinTool",             m_thinTool, "The private ThinningTool" );

  declareProperty("TrackParticlesToThin", m_trackParticleKey = "InDetTrackParticles",
                  "The xAOD::TrackParticleContainer to be thinned" );
  m_trackParticleKey.declareUpdateHandler( &ThinTrackParticlesAlg::setupTrackPartsToThin, this );

  declareProperty("InputContainerList",   m_inCollKeyList,
                  "Containers from which to extract the information which xAOD::TrackParticles should be kept" );
  m_inCollKeyList.declareUpdateHandler( &ThinTrackParticlesAlg::setupInputContainerList, this );

  declareProperty("Selection",            m_selection="",
                  "The selection string that defines which xAOD::TrackParticles to select from the container" );
  m_selection.declareUpdateHandler( &ThinTrackParticlesAlg::setupSelection, this );

  declareProperty("KeepTauConversions",   m_tauConversion=false,
                  "Flag to steer if one should also keep conversion track particles from taus" );
  m_tauConversion.declareUpdateHandler( &ThinTrackParticlesAlg::setupTauConv, this );

  declareProperty("KeepTauWide",          m_tauWide=false,
                  "Flag to steer if one should also keep 'wide track particles' from taus" );
  m_tauWide.declareUpdateHandler( &ThinTrackParticlesAlg::setupTauWide, this );

  declareProperty("KeepTauOther",         m_tauOther=false,
                  "Flag to steer if one should also keep 'other' track particles from taus" );
  m_tauOther.declareUpdateHandler( &ThinTrackParticlesAlg::setupTauOther, this );

  declareProperty("NMaxElectronTrackParticles", m_nElectronPTMax,
                  "Set the maximum number of TrackParticles from each electron to keep (default: -1 means all are kept");
  m_nElectronPTMax.declareUpdateHandler( &ThinTrackParticlesAlg::setupNEleTPMax, this );
}



// Destructor
///////////////
ThinTrackParticlesAlg::~ThinTrackParticlesAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinTrackParticlesAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_jos );
  ATH_MSG_DEBUG ( " using = " << m_thinTool );
  ATH_MSG_DEBUG ( " using = " << m_streamName );
  ATH_MSG_DEBUG ( " using = " << m_trackParticleKey );
  ATH_MSG_DEBUG ( " using = " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using = " << m_selection );
  ATH_MSG_DEBUG ( " using = " << m_tauConversion );
  ATH_MSG_DEBUG ( " using = " << m_tauWide );
  ATH_MSG_DEBUG ( " using = " << m_tauOther );
  ATH_MSG_DEBUG ( " using = " << m_nElectronPTMax );

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

  if (m_setTrackPartKey) {
    ATH_MSG_DEBUG( "Setting property" << m_trackParticleKey
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_trackParticleKey.name(), m_trackParticleKey.value());
  }
  if (m_setInCollKey) {
    ATH_MSG_DEBUG( "Setting property" << m_inCollKeyList
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_inCollKeyList.name(), m_inCollKeyList.toString());
  }
  if (m_setSelection) {
    ATH_MSG_DEBUG( "Setting property" << m_selection
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_selection.name(), m_selection.value() );
  }
  if (m_setTauConv) {
    ATH_MSG_DEBUG( "Setting property" << m_tauConversion
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_tauConversion.name(), m_tauConversion.toString());
  }
  if (m_setTauWide) {
    ATH_MSG_DEBUG( "Setting property" << m_tauWide
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_tauWide.name(), m_tauWide.toString());
  }
  if (m_setTauOther) {
    ATH_MSG_DEBUG( "Setting property" << m_tauOther
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_tauOther.name(), m_tauOther.toString());
  }
  if (m_setNEleTPMax) {
    ATH_MSG_DEBUG( "Setting property" << m_nElectronPTMax
                   << " of private tool with name: '" << fullToolName << "'" );
    m_jos->set (fullToolName + "." + m_nElectronPTMax.name(), m_nElectronPTMax.toString());
  }
  ATH_MSG_DEBUG( "Done setting properties of the tool");

  // Get the skimming tool
  ATH_CHECK( m_thinTool.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode ThinTrackParticlesAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_jos.release() );
  ATH_CHECK( m_thinTool.release() );

  return StatusCode::SUCCESS;
}



StatusCode ThinTrackParticlesAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );

  // Call the thinning tool
  ATH_CHECK( m_thinTool->doThinning() );

  return StatusCode::SUCCESS;
}
