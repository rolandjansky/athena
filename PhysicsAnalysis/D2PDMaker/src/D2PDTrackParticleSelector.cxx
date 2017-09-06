/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDTrackParticleSelector.cxx
//
/**
   @class D2PDTrackParticleSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select TrackParticle and write out good ones as ElementLinks

   Class to select good TrackParticle from an input container (which can either
   be an jet container or a container of ElementLinks poining to an
   TrackParticleContainer) and write out the good ones as a new container of
   ElementLinks pointing to the good TrackParticle in the original input container.
   The TrackParticle is the base class to (as far as I know) every offline object.
   That means that every electron, jet, cluster, track, muon,... inherits from this.
*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDTrackParticleSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// TrackParticle includes
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"




//=============================================================================
// Constructor
//=============================================================================
D2PDTrackParticleSelector::D2PDTrackParticleSelector( const std::string& name,
                                                      ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< Rec::TrackParticle, Rec::TrackParticleContainer >( name, pSvcLocator ),
  m_doNumberBLayerHits(false),
  m_idxNumberBLayerHits(0),
  m_doNumberPixelHits(false),
  m_idxNumberPixelHits(0),
  m_doNumberSCTHits(false),
  m_idxNumberSCTHits(0),
  m_doNumberSiliconHits(false),
  m_idxNumberSiliconHits(0),
  m_doNumberTRTHits(false),
  m_idxNumberTRTHits(0)
{
  declareProperty("numberBLayerHitsMin",   m_nBLayerHitsMin=-INT_MAX,  "Minimum number of b-layer hits" );
  m_nBLayerHitsMin.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberBLayerHits, this );
  declareProperty("numberBLayerHitsMax",   m_nBLayerHitsMax=INT_MAX,   "Maximum number of b-layer hits" );
  m_nBLayerHitsMax.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberBLayerHits, this );

  declareProperty("numberPixelHitsMin",    m_nPixelHitsMin=-INT_MAX,  "Minimum number of pixel hits" );
  m_nPixelHitsMin.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberPixelHits, this );
  declareProperty("numberPixelHitsMax",    m_nPixelHitsMax=INT_MAX,   "Maximum number of pixel hits" );
  m_nPixelHitsMax.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberPixelHits, this );

  declareProperty("numberSCTHitsMin",      m_nSCTHitsMin=-INT_MAX,  "Minimum number of SCT hits" );
  m_nSCTHitsMin.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberSCTHits, this );
  declareProperty("numberSCTHitsMax",      m_nSCTHitsMax=INT_MAX,   "Maximum number of SCT hits" );
  m_nSCTHitsMax.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberSCTHits, this );

  declareProperty("numberSiliconHitsMin",  m_nSiliconHitsMin=-INT_MAX,  "Minimum number of silicon hits" );
  m_nSiliconHitsMin.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberSiliconHits, this );
  declareProperty("numberSiliconHitsMax",  m_nSiliconHitsMax=INT_MAX,   "Maximum number of silicon hits" );
  m_nSiliconHitsMax.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberSiliconHits, this );

  declareProperty("numberTRTHitsMin",      m_nTRTHitsMin=-INT_MAX,  "Minimum number of TRT hits" );
  m_nTRTHitsMin.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberTRTHits, this );
  declareProperty("numberTRTHitsMax",      m_nTRTHitsMax=INT_MAX,   "Maximum number of TRT hits" );
  m_nTRTHitsMax.declareUpdateHandler( &D2PDTrackParticleSelector::setupNumberTRTHits, this );
}



//=============================================================================
// Destructor
//=============================================================================
D2PDTrackParticleSelector::~D2PDTrackParticleSelector()
{
}



//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDTrackParticleSelector::initializeMore()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using numberBLayerHitsMin   = " << m_nBLayerHitsMin );
      ATH_MSG_INFO ( " using numberBLayerHitsMax   = " << m_nBLayerHitsMax );
      
      ATH_MSG_INFO ( " using numberPixelHitsMin    = " << m_nPixelHitsMin );
      ATH_MSG_INFO ( " using numberPixelHitsMax    = " << m_nPixelHitsMax );
      
      ATH_MSG_INFO ( " using numberSCTHitsMin      = " << m_nSCTHitsMin );
      ATH_MSG_INFO ( " using numberSCTHitsMax      = " << m_nSCTHitsMax );
      
      ATH_MSG_INFO ( " using numberSiliconHitsMin  = " << m_nSiliconHitsMin );
      ATH_MSG_INFO ( " using numberSiliconHitsMax  = " << m_nSiliconHitsMax );
      
      ATH_MSG_INFO ( " using numberTRTHitsMin      = " << m_nTRTHitsMin );
      ATH_MSG_INFO ( " using numberTRTHitsMax      = " << m_nTRTHitsMax );
    }


  // Setup the internal cutflow stuff
  if ( m_printInternalCutflow )
    {
      if ( m_doNumberBLayerHits )
        {
          m_idxNumberBLayerHits = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < numberBLayerHits < %d",
                                                          m_nBLayerHitsMin.value(), m_nBLayerHitsMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doNumberPixelHits )
        {
          m_idxNumberPixelHits = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < numberPixelHits < %d",
                                                          m_nPixelHitsMin.value(), m_nPixelHitsMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doNumberSCTHits )
        {
          m_idxNumberSCTHits = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < numberSCTHits < %d",
                                                          m_nSCTHitsMin.value(), m_nSCTHitsMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doNumberSiliconHits )
        {
          m_idxNumberSiliconHits = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < numberSiliconHits < %d",
                                                          m_nSiliconHitsMin.value(), m_nSiliconHitsMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doNumberTRTHits )
        {
          m_idxNumberTRTHits = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%d < numberTRTHits < %d",
                                                          m_nTRTHitsMin.value(), m_nTRTHitsMax.value() ), std::make_pair(0,0) ) );
        }
    } // End: if ( m_printInternalCutflow )


  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDTrackParticleSelector::processObject( const Rec::TrackParticle* trackPart,
                                                     bool &isPassed )
{
  // Print a verbose message
  ATH_MSG_VERBOSE ( "Starting TrackParticle processObject." );
  

  // Make sure we have a TrackParticle
  if ( trackPart )
    {
      ATH_MSG_VERBOSE ( "Having a TrackParticle." );

      //--------------------------
      // Do the selections
      //--------------------------



      // Check how many hits the TrackParticle has in the silicon
      const Trk::TrackSummary* trackSum = trackPart->trackSummary();
      if ( trackSum )
        {
          ATH_MSG_VERBOSE ( "Having the TrackSummary." );

          int nBLayHits(0);
          int nPixelHits(0);
          int nSctHits(0);
          int nSiliconHits(0);
          int nTrtHits(0);
          nBLayHits    = trackSum->get(Trk::numberOfInnermostPixelLayerHits);
          nPixelHits   = trackSum->get(Trk::numberOfPixelHits);
          nSctHits     = trackSum->get(Trk::numberOfSCTHits);
          nSiliconHits = nPixelHits + nSctHits + nBLayHits;
          nTrtHits     = trackSum->get(Trk::numberOfTRTHits);

          // Number of b-layer hits selection
          if ( isPassed && m_doNumberBLayerHits )
            {
              isPassed = ( nBLayHits >= m_nBLayerHitsMin && nBLayHits <= m_nBLayerHitsMax );
              ATH_MSG_VERBOSE ( "TrackParticle number of b-layer hits cut is " << isPassed );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxNumberBLayerHits] += 1; }
            }

          // Number of pixel hits selection
          if ( isPassed && m_doNumberPixelHits )
            {
              isPassed = ( nPixelHits >= m_nPixelHitsMin && nPixelHits <= m_nPixelHitsMax );
              ATH_MSG_VERBOSE ( "TrackParticle number of pixel hits cut is " << isPassed );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxNumberPixelHits] += 1; }
            }

          // Number of SCT hits selection
          if ( isPassed && m_doNumberSCTHits )
            {
              isPassed = ( nSctHits >= m_nSCTHitsMin && nSctHits <= m_nSCTHitsMax );
              ATH_MSG_VERBOSE ( "TrackParticle number of SCT hits cut is " << isPassed );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxNumberSCTHits] += 1; }
            }

          // Number of silicon hits selection
          if ( isPassed && m_doNumberSiliconHits )
            {
              isPassed = ( nSiliconHits >= m_nSiliconHitsMin && nSiliconHits <= m_nSiliconHitsMax );
              ATH_MSG_VERBOSE ( "TrackParticle number of silicon hits cut is " << isPassed );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxNumberSiliconHits] += 1; }
            }

          // Number of TRT hits selection
          if ( isPassed && m_doNumberTRTHits )
            {
              isPassed = ( nTrtHits >= m_nTRTHitsMin && nTrtHits <= m_nTRTHitsMax );
              ATH_MSG_VERBOSE ( "TrackParticle number of TRT hits cut is " << isPassed );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxNumberTRTHits] += 1; }
            }

        } // End: if ( trackSum )

    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to a TrackParticle! Failing this TrackParticle!" );
    } // End: if/else have inav4mom


  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDTrackParticleSelector::finalizeMore()
{
  return StatusCode::SUCCESS;
}




