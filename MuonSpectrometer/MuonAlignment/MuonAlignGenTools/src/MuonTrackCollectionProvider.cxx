/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackCollection.h"

#include "MuonAlignGenTools/MuonTrackCollectionProvider.h"
#include "MuonAlignGenTools/MuonAlignRefitTool.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"

namespace Muon {
  
  //________________________________________________________________________
  MuonTrackCollectionProvider::MuonTrackCollectionProvider(const std::string& type, 
						   const std::string& name,
						   const IInterface* parent)
    
    : AthAlgTool(type,name,parent)
    , m_tagTool("")
    , m_inputTrackType(MSStandaloneWithIDTrackMatch)
  {

    declareInterface<ITrackCollectionProvider>(this);

    declareProperty("TagTool",             m_tagTool);

    declareProperty("MuonAlignRefitTool",  m_muonAlignRefitTool);

    declareProperty("InputMuonCol",        m_inputMuonCol="MuidMuonCollection");
    declareProperty("InputTrackCol",       m_inputTrackCol="MooreTracks");
    declareProperty("InputIDTrackCol",     m_inputIDTrackCol="Tracks");
    
    declareProperty("UseStandaloneTracks", m_useStandaloneTracks=true);
    declareProperty("TrackType",           m_inputTrackType=MSStandaloneWithIDTrackMatch);

    declareProperty("MomentumCut",         m_momentumCut=4.);
    declareProperty("TrackMatchDeltaR",    m_trackMatchDeltaR=0.25);

    declareProperty("RemovePerigeeBeforeRefit", m_removePerigeeBeforeRefit = false);

  }

  //________________________________________________________________________
  MuonTrackCollectionProvider::~MuonTrackCollectionProvider()
  {

  }

  //________________________________________________________________________
  StatusCode MuonTrackCollectionProvider::initialize()
  {
    if (m_tagTool.name()!=""){
      if (m_tagTool.retrieve().isFailure()) {
	ATH_MSG_FATAL("Failed to retrieve tool " << m_tagTool);
	return StatusCode::FAILURE;
      }
      else {
	ATH_MSG_INFO("Retrieved tool " << m_tagTool);
      }       
    }

    if (m_muonAlignRefitTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_muonAlignRefitTool);
    else {
      ATH_MSG_WARNING("Failed to retrieve MuonAlignRefitTool");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonTrackCollectionProvider::finalize()
  {
    return StatusCode::SUCCESS;
  }

  
  //________________________________________________________________________
  StatusCode MuonTrackCollectionProvider::trackCollection(const TrackCollection*& originalTracks)
  {
    originalTracks=0;
    if (m_inputTrackType==Muon)
      return trackCollectionFromMuonContainer(originalTracks);
    else if (m_inputTrackType==MSStandaloneWithIDTrackMatch)
      return trackCollectionFromTrackContainers(originalTracks);
    else if (m_inputTrackType==CombinedMuonTrack)
      return trackCollectionFromCombinedTrack(originalTracks);
    else 
      return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  StatusCode MuonTrackCollectionProvider::trackCollectionFromMuonContainer(const TrackCollection*& originalTracks)
  {
    originalTracks=0;
    
    /** get the container of the original muons - */
    const Analysis::MuonContainer* muonTES=0;
    StatusCode sc = evtStore()->retrieve( muonTES, m_inputMuonCol);
    if ( sc.isFailure() || muonTES == 0 ) return sc;
    if ( muonTES->size() < 1 ) return sc;
    
    TrackCollection* trackCollection=new TrackCollection;

    Analysis::MuonContainer::const_iterator muonIt  = muonTES->begin();
    Analysis::MuonContainer::const_iterator muonItE = muonTES->end();
    
    // Loop over muon collection
    for ( ; muonIt!=muonItE; ++muonIt ) {
      
      if (muonIt == muonItE || !*muonIt) continue;
      
      // put fit chi2 cut=2.5/dof

      // Only keep combined muons with ID and MS 
      if (m_useStandaloneTracks) {
	if ( !(*muonIt)->isStandAloneMuon()) continue;
	trackCollection->push_back(const_cast<Trk::Track*>((**muonIt).muonSpectrometerTrkTrack()));
      }
      else {
	if ( !(*muonIt)->isCombinedMuon()) continue;
	trackCollection->push_back(const_cast<Trk::Track*>((**muonIt).combinedMuonTrkTrack()));
      }
    }      
    
    originalTracks=trackCollection;

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonTrackCollectionProvider::trackCollectionFromTrackContainers(const TrackCollection*& tracks)
  {

    // check if TagTool exists
    if (m_tagTool.empty()) {
      ATH_MSG_ERROR("need TagTool to match MS track to ID track!");
      return StatusCode::FAILURE;
    }
	        
    tracks=0;
    
    /** get the container of the original muon tracks */
    const TrackCollection* muonTracks=0;
    if( !evtStore()->contains<TrackCollection>(m_inputTrackCol) ||
	evtStore()->retrieve(muonTracks,m_inputTrackCol).isFailure() ) {      
      
      ATH_MSG_DEBUG("input muon track collection \'"<<m_inputTrackCol<<"\' not found for this event");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("have muon tracks, "<<muonTracks->size());

    /** get the container of the original ID tracks */
    const TrackCollection* inDetTracks=0;
    if( !evtStore()->contains<TrackCollection>(m_inputIDTrackCol) ||
	evtStore()->retrieve(inDetTracks,m_inputIDTrackCol).isFailure() ) {
      
      ATH_MSG_DEBUG("input ID track collection \'"<<m_inputIDTrackCol<<"\' not found for this event");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("have ID tracks, "<<inDetTracks->size());

    TrackCollection* trackCollection=new TrackCollection;

    int iMuTrack(0);
    TrackCollection::const_iterator muTrackIt  = muonTracks->begin();
    TrackCollection::const_iterator muTrackItE = muonTracks->end();
    for ( ; muTrackIt!=muTrackItE; ++muTrackIt,iMuTrack++ ) {
      
      ATH_MSG_DEBUG("mu track "<<iMuTrack<<"/"<<muonTracks->size());
      
      if (muTrackIt == muTrackItE || !*muTrackIt) continue;
      
      const Trk::Track* muTrack=*muTrackIt;
      
      // loop over ID tracks and see if there is a good match
      TrackCollection::const_iterator inDetTrackIt  = inDetTracks->begin();
      TrackCollection::const_iterator inDetTrackItE = inDetTracks->end();  
      const Trk::Track* bestTrack = 0;
      double bestMatchChi2=99999.;
      int iIdTrack=0;
      for ( ; inDetTrackIt!=inDetTrackItE; ++inDetTrackIt,iIdTrack++ ) {
	
	const Trk::Track* inDetTrack = *inDetTrackIt;
	
	// check momentum
	if (!inDetTrack->perigeeParameters()) continue;
	
	double matchChi2 = m_tagTool->chi2(*inDetTrack,*muTrack);
	if (matchChi2<bestMatchChi2) {
	  bestMatchChi2=matchChi2;
	  bestTrack=inDetTrack;
        } 
      }
      
      // get ID track momentum
      if (!bestTrack) {
	ATH_MSG_DEBUG("no ID track found to match MS track");
	continue;
      }
      else {
	ATH_MSG_DEBUG("found ID track");
      }

      double pT      =bestTrack->perigeeParameters()->pT();
      double inDetEta=bestTrack->perigeeParameters()->eta();
      double momentum=std::fabs(pT)*std::sqrt(1.+sinh(inDetEta)*sinh(inDetEta))/1000.;
      
      if (momentum<m_momentumCut) {
	ATH_MSG_DEBUG("no good ID track, continuing");
	continue;
      }
      
      double charge = (*(bestTrack->trackParameters()->begin()))->charge();
      double qOverP = charge/(momentum*1000.);
      ATH_MSG_DEBUG("calling pTCorrectTrack with p="<<.001/qOverP);
      Trk::Track* newTrack=
	m_muonAlignRefitTool->pTCorrectTrack(*muTrack,qOverP,m_removePerigeeBeforeRefit);
      trackCollection->push_back(newTrack);
    }
  
    if (StatusCode::SUCCESS!=evtStore()->record(trackCollection,"pTCorrectedTracks")) {
      ATH_MSG_WARNING("problem with recording tracks to StoreGate!");
    }

    tracks=trackCollection;
    
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode MuonTrackCollectionProvider::trackCollectionFromCombinedTrack(const TrackCollection*& tracks)
  {
    tracks=0;
    return StatusCode::FAILURE;
  }
  
} // end namespace
