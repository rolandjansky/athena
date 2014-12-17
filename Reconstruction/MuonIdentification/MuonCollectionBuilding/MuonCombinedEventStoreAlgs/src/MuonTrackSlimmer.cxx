/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************************
Name    : MuonTrackSlimmer.cxx
Author  : Ketevi A. Assamagan
Created : July 2007

DESCRIPTION:

This class builds Slim Track objects for muons in the AOD; see MuonTrackSlimmer.h.

*****************************************************************************/

// Include files

#include "AthenaKernel/IThinningSvc.h"

#include "MuonTrackSlimmer.h"
#include <algorithm>
#include <math.h>

using namespace Analysis;
using namespace Trk;

/////////////////////////////////////////////////////////////////
// Constructor

Rec::MuonTrackSlimmer::MuonTrackSlimmer(const std::string& name, 
			           ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),  
  m_muonTrackSlimmerTool("Trk::TrackSlimmingTool/MuonAODTrackSlimmingTool", this), 
  m_thinningSvc( "ThinningSvc",  name ),
  m_muonContainerNames(0),
  m_tempTrackCollection(0)
{

  declareProperty("ThinService", m_thinningSvc);
  declareProperty("MuonTrackSlimmerTool", m_muonTrackSlimmerTool );
  declareProperty("MuonContainerNames",   m_muonContainerNames );
  declareProperty("SlimTrackContainer", m_muonSlimTrackContainerName = "MuonSlimmedTrackCollection");

  declareProperty("CaloLikelihoodCut", m_caloLikelihood = 0.5);

  declareProperty("InDetSlimTrackContainer", m_inDetSlimTrackContainerName="Tracks");

  declareProperty("TakeAllMuGirlRefittedSlimmedTracks", m_takeAllMuGirlRefittedTrack = false);
 
  declareProperty("doMuonTrackSlimming", m_doMuonTrackSlimming = true);
  
  declareProperty("doAppendToExisting", m_doAppendToExisting = false);

}

/////////////////////////////////////////////////////////////////
// Destructor

Rec::MuonTrackSlimmer::~MuonTrackSlimmer() {}

/////////////////////////////////////////////////////////////////
// Initialize

StatusCode Rec::MuonTrackSlimmer::initialize() {

  // Get pointer to IThinningSvc and cache it :
  if ( !m_thinningSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Unable to retrieve pointer to IThinningSvc !");
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG ("Successfully retrieved IThinningSvc.");
   

  // get the track slimmer tool
  if (m_muonTrackSlimmerTool.retrieve().isFailure()) {
    ATH_MSG_ERROR ("Unable to create Muon TrackSlimmer tool !");
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG ("Successfully retrieved " << m_muonTrackSlimmerTool);

  // create an empty view TrackCollection for overlap checking
  m_tempTrackCollection = new TrackCollection(SG::VIEW_ELEMENTS);
  if (!m_tempTrackCollection) {
    ATH_MSG_FATAL ("Could not create a view TrackCollection !");
    return StatusCode::FAILURE;
  }

  if (m_muonContainerNames.size() == 0) {
    ATH_MSG_ERROR ("Muon Track slimmer invoked with empty list of muon collections - "
		   << "configuration mistake !");
    return StatusCode::FAILURE;
  } else {
    for (unsigned int i=0; i < m_muonContainerNames.size(); ++i)
      ATH_MSG_INFO(m_muonContainerNames[i]<<" ==> slim ==> "<< m_muonSlimTrackContainerName);
  }

  ATH_MSG_INFO ( "Done Initializing " << name() );
  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////
// Finalize

StatusCode Rec::MuonTrackSlimmer::finalize() {
  m_tempTrackCollection->clear();
  delete m_tempTrackCollection;
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Execute

StatusCode Rec::MuonTrackSlimmer::execute() {

  ATH_MSG_DEBUG ("Executing MuonTrackSlimmer." );
  StatusCode sc = StatusCode::SUCCESS;

  /** clear the view container for this event */
  m_tempTrackCollection->clear();

  /** now do the Inner Detector Track. This one goes through the Thinning Service 
      here select the Inner Detector Tracks associated to muons to be saved in AOD */
  std::vector<bool> selectedInDetTracks;
  const TrackCollection* trackTES=0;
  sc = evtStore()->retrieve( trackTES, m_inDetSlimTrackContainerName);
  if( sc.isFailure()  ||  !trackTES ) {
    ATH_MSG_WARNING ("No InDet container found in TDS : "<< m_inDetSlimTrackContainerName);
  }
  if ( trackTES ) selectedInDetTracks.resize( trackTES->size(), false );


  /** create an Track container  and register it */
  TrackCollection* slimmedTracks = 0;
  if ( m_doMuonTrackSlimming ) {
  
    if ( m_doAppendToExisting) {

      const TrackCollection* slimmedTracks_const(NULL);
      if(!evtStore()->retrieve(slimmedTracks_const,
			       m_muonSlimTrackContainerName).isSuccess()) {
	ATH_MSG_ERROR ("Cannot retrieve "<<m_muonSlimTrackContainerName <<
		       " which was explicitly configured to be appended.");
	return StatusCode::FAILURE;
      }
      slimmedTracks=const_cast<TrackCollection*>(slimmedTracks_const);

    } else {

      slimmedTracks = new TrackCollection;
      sc = evtStore()->record(slimmedTracks, m_muonSlimTrackContainerName);
      if (sc.isFailure() || slimmedTracks == NULL ) {
	ATH_MSG_WARNING ("Could not record MuonSlimmedTrackcontainer in TDS");
      }
    }
  }

  // === loop over input containers and call accumulative track extraction and slimming
  std::vector<std::string>::const_iterator muContIter = m_muonContainerNames.begin();
  for (; muContIter != m_muonContainerNames.end(); muContIter++) {

    const MuonContainer* muonTES=0;
    sc = evtStore()->retrieve( muonTES, *muContIter);
    if( sc.isFailure()  ||  !muonTES ) {
      ATH_MSG_WARNING ("No AOD muon container '"<<*muContIter<<"' found in event store !");
      continue;
    } else ATH_MSG_DEBUG ("Slimming tracks from '"<<*muContIter<<"' container.");
 
    // call the Track Slimmin Tool
    bool checkOverlap = true;
    this->doTrackSlimming( muonTES, slimmedTracks, selectedInDetTracks, checkOverlap );

  }
  if ( slimmedTracks )
    evtStore()->setConst(slimmedTracks).ignore();

  /** do the thinning - for the Inner Detector tracks 
      Use the Operator::Or - container may have been thinned already by EgammaRec or TauRec
      The Muon results should be merged with those, thus the "or" operator */  
  if ( trackTES && !m_doAppendToExisting) 
    sc = m_thinningSvc->filter( *trackTES, selectedInDetTracks, IThinningSvc::Operator::Or );
  if ( sc.isFailure() ) ATH_MSG_WARNING ("Failed to thin InDet Tracks associated to Muons !");
 
  ATH_MSG_DEBUG ("done execute() - success.");

  return StatusCode::SUCCESS;
}


// method extracting tracks from muons, slim them and fill the containers
void Rec::MuonTrackSlimmer::doTrackSlimming ( const Analysis::MuonContainer * muonTES, 
                                         TrackCollection* slimmedTracks, 
                                         std::vector<bool>& selectedInDetTracks,
                                         const bool checkOverlap, const bool doSelection ) {

  unsigned int max = selectedInDetTracks.size();
 
  /// iterators over the container 
  MuonContainer::const_iterator muonItr  = muonTES->begin();
  MuonContainer::const_iterator muonItrE = muonTES->end();
  for (; muonItr != muonItrE; ++muonItr) {

    /** this is here to select good muons from CaloTrkMuId */
    bool select = true;
    const CaloEnergy * caloEnergy = (*muonItr)->caloEnergyLoss();
    if ( caloEnergy && (*muonItr)->isCaloMuonId() ) {
       unsigned short tag = caloEnergy->caloMuonIdTag();
       double likelihood  = caloEnergy->caloLRLikelihood();
       select = tag > 0 || likelihood > m_caloLikelihood;
    }
    if ( !select && doSelection ) continue;

    /** do the Inner Detector Tracks associated to Muons first */
    const Rec::TrackParticle * trackParticle = (*muonItr)->inDetTrackParticle();
    if ( trackParticle ) {
       if ( (trackParticle->trackElementLink())->isValid() ) {
          unsigned int index = (trackParticle->trackElementLink())->index();
          if ( index<max && max>0 ) selectedInDetTracks[index] = true;
          else ATH_MSG_WARNING ("Track not in Container: index = " << index <<
				" container size = " << max <<
				" container name is " << m_inDetSlimTrackContainerName);
       } 
    }

    /** check for a valid point for the muon spectrometer slimmed track container */
    if ( !slimmedTracks ) continue;

    /** get all the Rec::TrackParticle and Trk::Tracks from this muon - some of the Track may not apply or be available
        that is OK, a null pointer should be returned. For example, a lowPt reconstructed muon
        will not have combined Track. No must check for valid returned Track pointer before
        doing anything with it */

    TrackParticle* muonSpectrometerTrackParticle = const_cast<Rec::TrackParticle*>( (*muonItr)->muonSpectrometerTrackParticle() );
    TrackParticle* muonTrackParticleToVertex     = const_cast<Rec::TrackParticle*>( (*muonItr)->muonExtrapolatedTrackParticle() );
    TrackParticle* muonTrackParticleFromVertex   = const_cast<Rec::TrackParticle*>( (*muonItr)->innerExtrapolatedTrackParticle() );
    TrackParticle* muonCombinedTrackParticle     = const_cast<Rec::TrackParticle*>( (*muonItr)->combinedMuonTrackParticle() );

    Track* muonSpectrometerTrack = const_cast<Trk::Track*>( (*muonItr)->muonSpectrometerTrkTrack() );
    Track* muonTrackToVertex     = const_cast<Trk::Track*>( (*muonItr)->muonExtrapolatedTrkTrack() );
    Track* muonTrackFromVertex   = const_cast<Trk::Track*>( (*muonItr)->innerExtrapolatedTrkTrack() );
    Track* muonCombinedTrack     = const_cast<Trk::Track*>( (*muonItr)->combinedMuonTrkTrack() );

    bool isOverlapping = false;

    /** check: if the Track Pointer is valid, create the slim Track and save the original Track
        for overlap checking */
    if ( (muonCombinedTrack && muonCombinedTrackParticle) || (muonTrackFromVertex && muonTrackParticleFromVertex) ) {
       if ( muonCombinedTrack && muonCombinedTrackParticle ) {
          if ( checkOverlap ) isOverlapping = this->checkForOverlap ( muonCombinedTrack );
          if ( !isOverlapping ) {
             Track *slimmed = m_muonTrackSlimmerTool->slim( *muonCombinedTrack );
             slimmedTracks->push_back( slimmed );
             m_tempTrackCollection->push_back( muonCombinedTrack );
             muonCombinedTrackParticle->setTrackLink( slimmedTracks, slimmed );
          }
       }
       if ( (muonTrackFromVertex && muonTrackParticleFromVertex) && m_takeAllMuGirlRefittedTrack) {
          if ( checkOverlap ) isOverlapping = this->checkForOverlap ( muonTrackFromVertex );
          if ( !isOverlapping ) {
             Track *slimmed = m_muonTrackSlimmerTool->slim( *muonTrackFromVertex );
             slimmedTracks->push_back( slimmed );
             m_tempTrackCollection->push_back( muonTrackFromVertex );
             muonTrackParticleFromVertex->setTrackLink( slimmedTracks, slimmed );
          }
       }
    }
//    else if ( muonTrackFromVertex && muonTrackParticleFromVertex && !m_takeAllMuGirlRefittedTrack) {
//       if ( checkOverlap ) isOverlapping = this->checkForOverlap ( muonTrackFromVertex );
//       if ( !isOverlapping ) {
//          Track *slimmed = m_muonTrackSlimmerTool->slim( *muonTrackFromVertex );
//          slimmedTracks->push_back( slimmed );
//          m_tempTrackCollection->push_back( muonTrackFromVertex );
//          muonTrackParticleFromVertex->setTrackLink( slimmedTracks, slimmed );
//       }
//    }
    else if ( muonTrackToVertex && muonTrackParticleToVertex ) {
       if ( checkOverlap ) isOverlapping = this->checkForOverlap ( muonTrackToVertex );
       if ( !isOverlapping ) { 
          Track *slimmed = m_muonTrackSlimmerTool->slim( *muonTrackToVertex );
          slimmedTracks->push_back( slimmed );
          m_tempTrackCollection->push_back( muonTrackToVertex );
	  muonTrackParticleToVertex->setTrackLink( slimmedTracks, slimmed );
       }
    }
/*
    else if ( muonTrackFromVertex && muonTrackParticleFromVertex && !m_takeAllMuGirlRefittedTrack) {
       if ( checkOverlap ) isOverlapping = this->checkForOverlap ( muonTrackFromVertex );
       if ( !isOverlapping ) {
          Track *slimmed = m_muonTrackSlimmerTool->slim( *muonTrackFromVertex );
          slimmedTracks->push_back( slimmed );
          m_tempTrackCollection->push_back( muonTrackFromVertex );
	  muonTrackParticleFromVertex->setTrackLink( slimmedTracks, slimmed );
       }
    }
*/
    else if ( muonSpectrometerTrack && muonSpectrometerTrackParticle ) {
       if ( checkOverlap ) isOverlapping = this->checkForOverlap ( muonSpectrometerTrack );
       if ( !isOverlapping ) {
          Track *slimmed = m_muonTrackSlimmerTool->slim( *muonSpectrometerTrack );
          slimmedTracks->push_back( slimmed );
          m_tempTrackCollection->push_back( muonSpectrometerTrack );
          muonSpectrometerTrackParticle->setTrackLink( slimmedTracks, slimmed );
      }
    }
  }
}

/** check for overlap and return "true" if there is an overlap */
bool Rec::MuonTrackSlimmer::checkForOverlap ( const Trk::Track * track ) {

  TrackCollection::const_iterator trackItr  = m_tempTrackCollection->begin();
  TrackCollection::const_iterator trackItrE = m_tempTrackCollection->end();
  for (; trackItr != trackItrE; ++trackItr) {
    if ( *trackItr == track ) return true;
  }
  return false;

}
