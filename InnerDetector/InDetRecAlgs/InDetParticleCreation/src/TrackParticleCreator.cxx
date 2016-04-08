/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                      TrackParticleCreator.cxx  -  Description
                             -------------------
    begin   : 27-02-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes : Markus Elsing
 
 ***************************************************************************/

#include "InDetParticleCreation/TrackParticleCreator.h"
// forward includes
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
// other includes
#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackLink/ITrackLink.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "Particle/TrackParticleContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

namespace InDet
{
TrackParticleCreator::TrackParticleCreator(const std::string &name, ISvcLocator *pSvcLocator)
        : AthAlgorithm (name, pSvcLocator),
	  m_particleCreatorTool("Trk::TrackParticleCreatorTool"),
	  m_assoTool("Trk::PRD_AssociationTool"),
	  m_tracksName("Tracks"),
	  m_vxCandidatesPrimaryName("VxPrimaryCandidate"),
	  m_trackParticleOutputName("TrackParticleCandidate"),
	  m_doSharedHits(false),
	  m_numEvents(0),
	  m_nTracksProcessed(0),
	  m_nTrackParticlesCreated(0),

    m_numberOfBLayerHits(0),             
    m_numberOfBLayerSharedHits(0),       
    m_numberOfBLayerOutliers(0),
    
    m_numberOfContribPixelLayers(0),
    m_numberOfPixelHits(0),              
    m_numberOfPixelSharedHits(0),        
    m_numberOfPixelHoles(0),             
    m_numberOfGangedPixels(0),
    m_numberOfGangedFlaggedFakes(0),      
                                      
    m_numberOfSCTHits(0),                
    m_numberOfSCTSharedHits(0),          
    m_numberOfSCTHoles(0),               
    m_numberOfSCTDoubleHoles(0),         
    m_numberOfTRTHits(0),                
    m_numberOfTRTXenonHits(0),                
    m_numberOfTRTHighThresholdHits(0),   
    m_numberOfTRTOutliers(0),            
    m_numberOfTRTHighThresholdOutliers(0),
    m_numberOfOutliersOnTrack(0),
    
    m_numberOfPixelOutliers(0),
    m_numberOfPixelDeadSensors(0),
    m_numberOfPixelSpoiltHits(0), 
    m_numberOfBlayersMissed(0),

    m_numberOfSCTOutliers(0),
    m_numberOfSCTDeadSensors(0),
    m_numberOfSCTSpoiltHits(0),   
    m_numberOfTRTHoles(0),        
    m_numberOfTRTDeadStraws(0),   
    m_numberOfTRTTubeHits(0)                                                
{
    declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
    declareProperty("PRDAssociationTool", m_assoTool);
    declareProperty("TracksName",m_tracksName);
    declareProperty("VxCandidatesPrimaryName",m_vxCandidatesPrimaryName);
    declareProperty("TrackParticlesOutputName",m_trackParticleOutputName);
    declareProperty("DoSharedHits", m_doSharedHits);  
    declareProperty("addAllTracksToDummyVtx", m_addAllTracksToDummyVtx = false);
}

TrackParticleCreator::~TrackParticleCreator()
{}


StatusCode TrackParticleCreator::initialize()
{
    msg(MSG::DEBUG) << "initialize() success" << endreq;

    if ( m_particleCreatorTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_particleCreatorTool << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_particleCreatorTool << endreq;
    }

    if (m_doSharedHits) {
      if ( m_assoTool.retrieve().isFailure() ) {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endreq;
        return StatusCode::FAILURE;
      } else {
        msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endreq;
      }
    }
    
    return StatusCode::SUCCESS;
}

StatusCode TrackParticleCreator::execute()
{
  ATH_MSG_DEBUG ("starting particle creation execute()");
    
  m_numEvents++;

  int dbgNumTPs(0),dbgNumTPsNotUsed(0);
  
  /**
     Retrieve all Tracks from StoreGate
  */
  const TrackCollection *allTracksFromStoreGate;
  StatusCode sc = evtStore()->retrieve( allTracksFromStoreGate, m_tracksName);
  if (sc.isFailure())
    {
      ATH_MSG_DEBUG ("Unable to retrieve track container: "<< m_tracksName << " from StoreGate. Return success");
      return StatusCode::SUCCESS;
    }
  m_nTracksProcessed += allTracksFromStoreGate->size();
  ATH_MSG_VERBOSE ("Number of Tracks: " << allTracksFromStoreGate->size());
  
  /**
     this is new !
  */
  if (m_doSharedHits) {
    // clear prdAssociationTool (this may be altered)
    m_assoTool->reset();
    // loop over tracks and to 
    TrackCollection::const_iterator trackIt    = allTracksFromStoreGate->begin();
    TrackCollection::const_iterator trackItEnd = allTracksFromStoreGate->end();
    for ( ; trackIt != trackItEnd ; ++trackIt) {
      if ( (m_assoTool->addPRDs(**trackIt)).isFailure() ) {
	ATH_MSG_ERROR ("could not add PRDs to association tool");
      }
    }
  }
  
  // Now prepare for particle creation
  Rec::TrackParticleContainer * trackPC = new Rec::TrackParticleContainer;
  trackPC->clear();
  Rec::TrackParticle* aParticle(0);
  
  // association map for getting the track to vertex association right
  std::map< Trk::VxTrackAtVertex*, Rec::TrackParticle* > testMap;
  
  // get vertex container
  const VxContainer* primcontainer(0);
  bool primVtxExists = evtStore()->contains<VxContainer>(m_vxCandidatesPrimaryName);
  
  // If no primary vertex is there, all TrackParticles will be constructed with m_reconstructedVertex(0).
  if (!primVtxExists) {
    ATH_MSG_DEBUG ("No primary vertex container, use vertex(0)");
    
    for (TrackCollection::const_iterator alltr  = (*allTracksFromStoreGate).begin() ;
	 alltr != (*allTracksFromStoreGate).end()   ; ++alltr) {
      ATH_MSG_DEBUG ("Create particle for track pointer: " << *alltr);
      aParticle = m_particleCreatorTool->createParticle(*alltr);
      ATH_MSG_DEBUG ("==> particle pointer: " << aParticle);
      if (aParticle!=0) {
	trackPC->push_back(aParticle);
	ATH_MSG_VERBOSE ("content: " << *aParticle);
      }
    }

  } else {

    if (evtStore()->retrieve(primcontainer, m_vxCandidatesPrimaryName).isFailure()) {
      ATH_MSG_WARNING ("No VxContainer with key " << m_vxCandidatesPrimaryName << " found, can not happen !");
    }
    
    ATH_MSG_DEBUG ("There are " << primcontainer->size() << " vertices and " << allTracksFromStoreGate->size() << " tracks.");
    
    /// loop over all vertices and create trackparticles from tracks used for those vertices
    /// (vertices DO NOT share tracks, so each track appears only once here!)
    /// (I do not need to exclude the dummy vertex at the end because it has no tracks anyway!)
    std::map<const Trk::Track*, bool> trackWasUsed;
    unsigned int nVtx(0);
    // position of TP which was already created needs to be stored
    // such that we can relink later successfully
    std::map<const Trk::Track*, unsigned int> trackWasUsedInt;
    //

    if  (!m_addAllTracksToDummyVtx) {

      for (VxContainer::const_iterator vtxItr=primcontainer->begin(); vtxItr!=primcontainer->end(); vtxItr++) {
	
	ATH_MSG_DEBUG ("Vertex " << nVtx+1 << " has " << (*vtxItr)->vxTrackAtVertex()->size() << " tracks.");
	
	// vertex type
	Trk::VertexType tpOrigin = Trk::PileUp;
	if (nVtx == 0) { /// the primary!
	  tpOrigin = Trk::PriVtx;
	}
	ATH_MSG_DEBUG ("Vertex type: " << tpOrigin); 
      
	for (unsigned int i = 0 ; i < (*vtxItr)->vxTrackAtVertex()->size() ; ++i) {
	  
	  nVtx++;

	  Trk::VxTrackAtVertex* tmpVTAV     = (*((*vtxItr)->vxTrackAtVertex()))[i];
	  Trk::ITrackLink*      trklink     = tmpVTAV->trackOrParticleLink();
	  Trk::LinkToTrack*     linkToTrack = dynamic_cast<Trk::LinkToTrack*>(trklink);
	
	  if (!linkToTrack) {
	    ATH_MSG_DEBUG ("Cast of element link failed, skip this track !!!!!");
	  } else {
	    const Trk::Track* tmpTrack(linkToTrack->cachedElement());
	    if (!tmpTrack) {
	      ATH_MSG_DEBUG ("Do not find back track pointer, skip track !!!!!");
	    } else {
	      
	      if (!trackWasUsed[tmpTrack]) {
	      
		ATH_MSG_DEBUG ("Create particle for track pointer: " << tmpTrack);
		aParticle = m_particleCreatorTool->createParticle(tmpTrack, *vtxItr, tpOrigin);
		ATH_MSG_DEBUG ("==> particle pointer: " << aParticle);
		if (aParticle!=0) {
		  trackPC->push_back(aParticle);
		  ATH_MSG_VERBOSE ("content: " << *aParticle);
		  // keep track of used tracks
		  testMap[tmpVTAV] = aParticle;
		  trackWasUsedInt[tmpTrack] = trackPC->size();
		  // debug output
		  dbgNumTPs++;
		}
		// mark track as used now
		trackWasUsed[tmpTrack]=true;
	      } else {
		// the already created particle still has to be associated
		// with the vxtrackatvertex object otherwise the relink
		// further down is not done for this vxtrackatvertex in the
		// vxcandidate
		testMap[tmpVTAV] = (*trackPC)[trackWasUsedInt[tmpTrack]-1];
		ATH_MSG_DEBUG ("Track already linked to previous vertex, relink it to this vertex as well");
	      }
	    }
	  }
	}
      }
    }
    /// the rest of the tracks which have not been used in any vertex fit are assigned to a dummy vertex
    if (trackWasUsed.size() != (*allTracksFromStoreGate).size()) {
      ATH_MSG_DEBUG ( "Remaining tracks will be assigned to dummy vertex!");
      for (TrackCollection::const_iterator
	     alltr  = (*allTracksFromStoreGate).begin() ;
	   alltr != (*allTracksFromStoreGate).end()   ; ++alltr) {
	
	if (!trackWasUsed[(*alltr)]) {
	  ATH_MSG_DEBUG ("Create particle for unassigned track pointer: " << *alltr);
	  aParticle = m_particleCreatorTool->createParticle((*alltr), primcontainer->back(), Trk::NoVtx);  
	  ATH_MSG_DEBUG ("==> particle pointer: " << aParticle);
	  if (aParticle!=0) {
	    trackPC->push_back(aParticle);
	    ATH_MSG_VERBOSE ("content: " << *aParticle);
	    // debug output
	    dbgNumTPs++;
	    dbgNumTPsNotUsed++;
	  }
	}
      }
    }
  
    // --- DEBUG
    ATH_MSG_DEBUG ("Number of TrackParticles associated to the dummy vertex: " << dbgNumTPsNotUsed);
    ATH_MSG_DEBUG ("The primary vertex is at: " << primcontainer->front()->recVertex() << " with " << primcontainer->front()->vxTrackAtVertex()->size() << " tracks.");
    ATH_MSG_DEBUG ("The  dummy  vertex is at: " << primcontainer->back()->recVertex() << " with " << primcontainer->back()->vxTrackAtVertex()->size() << " tracks.");
    ATH_MSG_DEBUG ("Number of TrackParticles have been created in total: " << dbgNumTPs);
  // --- DEBUG END
  }   
  /**
     These TrackParticles will be persistified!
     Therefore we have to assign a collection to the ElementLink<TrackCollection> datamember of it
    and to the VxContainer one
  */
  for (Rec::TrackParticleContainer::iterator itr = trackPC->begin();
       itr != trackPC->end(); ++itr) {
    ATH_MSG_DEBUG ("Origin type of TrackParticle: " << (*itr)->particleOriginType());
    // TrackParticle has a wrapped method setStorableObject which
    // calls the ElementLink::setStorableObject
    (*itr)->setStorableObject(allTracksFromStoreGate);
    if (primVtxExists) {
      (*itr)->setStorableObject(primcontainer);
    }
    // get information for final summary
    const Trk::TrackSummary* trackSummary = (*itr)->trackSummary();

    //
    // --------- statistics
    //
    if (trackSummary != 0) {

          // just sum up all shared hits (blayer is contained in pixel??)
//           m_nTotalNumberOfSharedHits += trackSummary->get(Trk::numberOfBLayerSharedHits);
//           unsigned int tmpSum(trackSummary->get(Trk::numberOfPixelSharedHits) + trackSummary->get(Trk::numberOfSCTSharedHits));
//           m_nTotalNumberOfSharedHits += tmpSum;
//           if (tmpSum > 0) m_nTrackParticlesWithSharedHits++;

          // integer entries in the summary can be -1 or -999, strange I know but that is the design ...
          if (trackSummary->get(Trk::numberOfBLayerHits)>=0)               m_numberOfBLayerHits              += trackSummary->get(Trk::numberOfBLayerHits);                           
          if (trackSummary->get(Trk::numberOfBLayerSharedHits)>=0)         m_numberOfBLayerSharedHits        += trackSummary->get(Trk::numberOfBLayerSharedHits);               
          if (trackSummary->get(Trk::numberOfBLayerOutliers)>=0)           m_numberOfBLayerOutliers          += trackSummary->get(Trk::numberOfBLayerOutliers);          
                                                                           
          if (trackSummary->get(Trk::numberOfContribPixelLayers)>=0)       m_numberOfContribPixelLayers      += trackSummary->get(Trk::numberOfContribPixelLayers);      
          if (trackSummary->get(Trk::numberOfPixelHits)>=0)                m_numberOfPixelHits               += trackSummary->get(Trk::numberOfPixelHits);                             
          if (trackSummary->get(Trk::numberOfPixelSharedHits)>=0)          m_numberOfPixelSharedHits         += trackSummary->get(Trk::numberOfPixelSharedHits);                 
          if (trackSummary->get(Trk::numberOfPixelHoles)>=0)               m_numberOfPixelHoles              += trackSummary->get(Trk::numberOfPixelHoles);                           
          if (trackSummary->get(Trk::numberOfGangedPixels)>=0)             m_numberOfGangedPixels            += trackSummary->get(Trk::numberOfGangedPixels);            
          if (trackSummary->get(Trk::numberOfGangedFlaggedFakes)>=0)       m_numberOfGangedFlaggedFakes      += trackSummary->get(Trk::numberOfGangedFlaggedFakes);            
                                                                                                                                                                       
          if (trackSummary->get(Trk::numberOfSCTHits)>=0)                  m_numberOfSCTHits                 += trackSummary->get(Trk::numberOfSCTHits);                                 
          if (trackSummary->get(Trk::numberOfSCTSharedHits)>=0)            m_numberOfSCTSharedHits           += trackSummary->get(Trk::numberOfSCTSharedHits);                     
          if (trackSummary->get(Trk::numberOfSCTHoles)>=0)                 m_numberOfSCTHoles                += trackSummary->get(Trk::numberOfSCTHoles);                               
          if (trackSummary->get(Trk::numberOfSCTDoubleHoles)>=0)           m_numberOfSCTDoubleHoles          += trackSummary->get(Trk::numberOfSCTDoubleHoles);                   
          if (trackSummary->get(Trk::numberOfTRTHits)>=0)                  m_numberOfTRTHits                 += trackSummary->get(Trk::numberOfTRTHits);                                 
          if (trackSummary->get(Trk::numberOfTRTXenonHits)>=0)             m_numberOfTRTXenonHits            += trackSummary->get(Trk::numberOfTRTXenonHits);                                 
          if (trackSummary->get(Trk::numberOfTRTHighThresholdHits)>=0)     m_numberOfTRTHighThresholdHits    += trackSummary->get(Trk::numberOfTRTHighThresholdHits);       
          if (trackSummary->get(Trk::numberOfTRTOutliers)>=0)              m_numberOfTRTOutliers             += trackSummary->get(Trk::numberOfTRTOutliers);                         
          if (trackSummary->get(Trk::numberOfTRTHighThresholdOutliers)>=0) m_numberOfTRTHighThresholdOutliers+= trackSummary->get(Trk::numberOfTRTHighThresholdOutliers);
          if (trackSummary->get(Trk::numberOfOutliersOnTrack)>=0)          m_numberOfOutliersOnTrack         += trackSummary->get(Trk::numberOfOutliersOnTrack);                 
        
          if (trackSummary->get(Trk::numberOfPixelOutliers)>=0)            m_numberOfPixelOutliers    += trackSummary->get(Trk::numberOfPixelOutliers);
          if (trackSummary->get(Trk::numberOfPixelDeadSensors)>=0)         m_numberOfPixelDeadSensors += trackSummary->get(Trk::numberOfPixelDeadSensors);
          if (trackSummary->get(Trk::numberOfPixelSpoiltHits)>=0)          m_numberOfPixelSpoiltHits  += trackSummary->get(Trk::numberOfPixelSpoiltHits); 

	  if (trackSummary->get(Trk::expectBLayerHit) > 0          &&
              trackSummary->get(Trk::numberOfBLayerHits)>=0        &&
	      trackSummary->get(Trk::numberOfBLayerOutliers)>=0    &&
	      trackSummary->get(Trk::numberOfContribPixelLayers)>=0) {
	    if (trackSummary->get(Trk::numberOfBLayerHits)+trackSummary->get(Trk::numberOfBLayerOutliers) == 0 &&
		trackSummary->get(Trk::numberOfContribPixelLayers) == 2) m_numberOfBlayersMissed++;
	  }

          if (trackSummary->get(Trk::numberOfSCTOutliers)>=0)              m_numberOfSCTOutliers      += trackSummary->get(Trk::numberOfSCTOutliers);     
          if (trackSummary->get(Trk::numberOfSCTDeadSensors)>=0)           m_numberOfSCTDeadSensors   += trackSummary->get(Trk::numberOfSCTDeadSensors);
          if (trackSummary->get(Trk::numberOfSCTSpoiltHits)>=0)            m_numberOfSCTSpoiltHits    += trackSummary->get(Trk::numberOfSCTSpoiltHits);   
          if (trackSummary->get(Trk::numberOfTRTHoles)>=0)                 m_numberOfTRTHoles         += trackSummary->get(Trk::numberOfTRTHoles);        
          if (trackSummary->get(Trk::numberOfTRTDeadStraws)>=0)            m_numberOfTRTDeadStraws    += trackSummary->get(Trk::numberOfTRTDeadStraws);   
          if (trackSummary->get(Trk::numberOfTRTTubeHits)>=0)              m_numberOfTRTTubeHits      += trackSummary->get(Trk::numberOfTRTTubeHits);     
        }
    }

    m_nTrackParticlesCreated += trackPC->size();
    /**
    Record TrackParticles to StoreGate
    */
    sc = evtStore()->record(trackPC, m_trackParticleOutputName, false);
    if (sc.isFailure())
    {
      ATH_MSG_ERROR ("Unable to record TrackParticleContainer "
		     << m_trackParticleOutputName << " to StoreGate.");
      return sc;
    }
    
    ATH_MSG_DEBUG ("Recorded TrackParticleContainer has size: " << trackPC->size());
    
    // in order to relink the track link in the vertex from track to trackparticle
    // we have to read in the symlinked TrackParticleBaseCollection
    const Trk::TrackParticleBaseCollection* theTPBC(0);
    sc = evtStore()->retrieve(theTPBC, m_trackParticleOutputName);
    if (sc.isFailure())
    {
      ATH_MSG_DEBUG ("Unable to retrieve TrackParticleBaseCollection " << m_trackParticleOutputName << " from StoreGate.");
      return sc;
    }
     
    ATH_MSG_DEBUG ("Retrieved TrackParticleContainer as symlinked TrackParticleBaseCollection with size: " << theTPBC->size());
    
    for (std::map< Trk::VxTrackAtVertex*, Rec::TrackParticle* >::iterator myItr  = testMap.begin();
                                                                          myItr != testMap.end(); ++myItr)
    {
      Trk::LinkToTrackParticleBase* link = new Trk::LinkToTrackParticleBase;
      link->setStorableObject( *theTPBC );
      link->setElement( myItr->second );
      ( myItr->first )->setOrigTrack ( link );
    }
    
    const INavigable4MomentumCollection* theNav4s(0);
    sc = evtStore()->symLink(trackPC,theNav4s);
    if (sc.isFailure())
    {
      ATH_MSG_WARNING ("Could not symLink TrackParticleContainer to INavigable4MomentumCollection");
    }
    
    return StatusCode::SUCCESS;
}

StatusCode TrackParticleCreator::finalize()
{
    double nTP = (double)m_nTrackParticlesCreated;
 
    msg(MSG::INFO) << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(9) << std::setprecision(3);
    msg(MSG::INFO) << " --- InDetParticleCreation Summary: " << endreq;
    
    msg(MSG::INFO) << " --- Input  TrackCollection        : \"" << m_tracksName              << "\" with " << m_nTracksProcessed << " Tracks (";
    msg(MSG::INFO) << ( m_numEvents ? (double)m_nTracksProcessed/(double)m_numEvents : 0 ) << " per event)." << endreq;
    
    
    msg(MSG::INFO) << " --- Output TrackParticleContainer : \"" << m_trackParticleOutputName << "\" with " << nTP << " TrackParticles (";
    msg(MSG::INFO) << (m_numEvents ? nTP/(double)m_numEvents : 0 ) << " per event)." << endreq;
    
    if (m_nTrackParticlesCreated>0){
        msg(MSG::INFO) << " ---  /Track in   -----> "
		       << " BLayer " 
		       << " Pixel " 
		       << "  SCT  "
		       << "  TRT  " 
		       << "  All (P+S+T)" << endreq;
        msg(MSG::INFO) << " ---  Hits                "
		       << (double)m_numberOfBLayerHits/nTP << "  "
		       << (double)m_numberOfPixelHits/nTP  << "  "
		       << (double)m_numberOfSCTHits/nTP    << "  "
		       << (double)m_numberOfTRTHits/nTP    << "  "
		       << (double)(m_numberOfPixelHits+m_numberOfSCTHits+m_numberOfTRTHits)/nTP << endreq;
        msg(MSG::INFO) << " ---  Shared Hits         "
		       << (double)m_numberOfBLayerSharedHits/nTP << "  " 
		       << (double)m_numberOfPixelSharedHits/nTP  << "  " 
		       << (double)m_numberOfSCTSharedHits/nTP    << "  " 
		       << " " << "       "
		       << " " << (double)(m_numberOfPixelSharedHits+m_numberOfSCTSharedHits)/nTP << endreq;
        msg(MSG::INFO) << " ---  Spoiled/Tube Hits    "
		       << "      "
		       << (double)m_numberOfPixelSpoiltHits/nTP << "  "
		       << (double)m_numberOfSCTSpoiltHits/nTP   << "  "
		       << " " << (double)m_numberOfTRTTubeHits/nTP     << "  "
		       << " " << (double)(m_numberOfPixelSpoiltHits+m_numberOfSCTSpoiltHits+m_numberOfTRTTubeHits)/nTP << endreq;
        msg(MSG::INFO) << " ---  Outliers            "
		       << (double)m_numberOfBLayerOutliers/nTP << "  "
		       << (double)m_numberOfPixelOutliers/nTP  << "  "
		       << (double)m_numberOfSCTOutliers/nTP    << "  "
		       << " " << (double)m_numberOfTRTOutliers/nTP    << "  "
		       << " " << (double)m_numberOfOutliersOnTrack/nTP << endreq;
        msg(MSG::INFO) << " ---  Holes               "
		       << "       "
		       << (double)m_numberOfPixelHoles/nTP   << "  " 
		       << (double)m_numberOfSCTHoles/nTP     << "  "
		       << " " << (double)m_numberOfTRTHoles/nTP     << "  "
		       << " " << (double)(m_numberOfPixelHoles+m_numberOfSCTHoles+m_numberOfTRTHoles)/nTP << endreq;
	msg(MSG::INFO) << "      missed (exp) blayer " << (double) m_numberOfBlayersMissed/nTP << endreq;         
        msg(MSG::INFO) << "      SCT Double Holes    "
		       << "       "
		       << "       "
		       << (double)m_numberOfSCTDoubleHoles/nTP
		       << endreq;
        msg(MSG::INFO) << " ---  Dead Sensors/Straws "
		       << "       "
		       << (double)m_numberOfPixelDeadSensors/nTP << "  "
		       << (double)m_numberOfSCTDeadSensors/nTP   << "  "
		       << " " << (double)m_numberOfTRTDeadStraws/nTP    << "  "
		       << " " << (double)(m_numberOfPixelDeadSensors+m_numberOfSCTDeadSensors+m_numberOfTRTDeadStraws)/nTP
		       << endreq;
        msg(MSG::INFO) << " ---  Add info Pixels " << endreq;
        msg(MSG::INFO) << "      contributing layers        " << (double)m_numberOfContribPixelLayers/nTP << endreq;
        msg(MSG::INFO) << "      ganged pixels              " << (double)m_numberOfGangedPixels/nTP << endreq;
        msg(MSG::INFO) << "      ganged flagged as fake     " << (double)m_numberOfGangedFlaggedFakes/nTP << endreq;
        msg(MSG::INFO) << " ---  Add info TRT " << endreq;
        msg(MSG::INFO) << "      High Threshold Hits "
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTHighThresholdHits/nTP
		       << endreq;
        msg(MSG::INFO) << "      High thre. outliers "
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTHighThresholdOutliers/nTP
		       << endreq;
        msg(MSG::INFO) << "      Xenon hits"
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTXenonHits/nTP
		       << endreq;

    
    } else 
        msg(MSG::INFO) << " No TrackParticles have been created ... skipping output." << endreq;
    
    msg(MSG::DEBUG) << "finalize() success" << endreq;
    return StatusCode::SUCCESS;
}

}
 // end namespace
