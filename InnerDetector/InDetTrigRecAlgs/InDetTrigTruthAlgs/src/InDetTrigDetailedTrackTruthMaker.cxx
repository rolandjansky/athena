/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// filename: InDetTrigDetailedTrackTruthMaker.cxx
//
// author: Iwona Grabowska-Bold, June 2006
//         Iwona.Grabowska@cern.ch
//
// Description:  Trigger version of the InDetDetailedTrackTruthMaker.cxx
//               (see original package documentation).
//
// **************************************************************************
// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "InDetTrigTruthAlgs/InDetTrigDetailedTrackTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/SubDetHitStatistics.h"
#include "TrkToolInterfaces/IDetailedTrackTruthBuilder.h"

#include <iterator>

namespace InDet {

//================================================================
InDetTrigDetailedTrackTruthMaker::InDetTrigDetailedTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) : HLT::FexAlgo(name,pSvcLocator),
	   m_truthTool("Trk::DetailedTrackTruthBuilder"),
	   m_subDetWeights(SubDetHitStatistics::NUM_SUBDETECTORS, 1.),
	   m_doTruth(false)  
{  
  declareProperty("WeightPixel",  m_subDetWeights[SubDetHitStatistics::Pixel]);
  declareProperty("WeightSCT",    m_subDetWeights[SubDetHitStatistics::SCT]);
  declareProperty("WeightTRT",    m_subDetWeights[SubDetHitStatistics::TRT]);
  declareProperty("WeightMDT",    m_subDetWeights[SubDetHitStatistics::MDT]);
  declareProperty("WeightRPC",    m_subDetWeights[SubDetHitStatistics::RPC]);
  declareProperty("WeightTGC",    m_subDetWeights[SubDetHitStatistics::TGC]);
  declareProperty("WeightCSC",    m_subDetWeights[SubDetHitStatistics::CSC]);
  declareProperty("doTruthAss",   m_doTruth);

  declareMonitoredVariable("numPRDs", m_numPRDs  );
  declareMonitoredVariable("numTruthTracks", m_numTruthTracks   );
  declareMonitoredStdContainer("numPRDClusters", m_numPRDClusters  );


}

// Initialize method
// -----------------------------------------------------------------------------------------------------
HLT::ErrorCode InDetTrigDetailedTrackTruthMaker::hltInitialize() {

  if(!m_doTruth) {
    msg() << MSG::INFO << "InDetTrigDetailedTrackTruthMaker is skipped: doTruth = False" << endreq;
    return HLT::OK;
  }

  msg() << MSG::INFO << "InDetTrigDetailedTrackTruthMaker::initialize()" << endreq;
  
  //----------------
  if (m_truthTool.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Cannot retrieve tool " << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  
  msg() << MSG::INFO << "Initialization successful" << endreq;

  //----------------
  return HLT::OK;
}

// -----------------------------------------------------------------------------------------------------
HLT::ErrorCode InDetTrigDetailedTrackTruthMaker::hltFinalize() {

  if(!m_doTruth) {
    msg() << MSG::INFO << "InDetTrigDetailedTrackTruthMaker is skipped: doTruth = False" << endreq;
    return HLT::OK;
  }

  msg() << MSG::INFO << "InDetTrigDetailedTrackTruthMaker finalized" << endreq;
  return HLT::OK;
}
// -----------------------------------------------------------------------------------------------------
HLT::ErrorCode InDetTrigDetailedTrackTruthMaker::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
  
  //zero monitoring counters
  m_numPRDClusters.clear();
  m_numTruthTracks = 0;
  m_numPRDs = 0;

  int outputLevel = msgLvl();
  
  if(!m_doTruth) {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "InDetTrigDetailedTrackTruthMaker is skipped: doTruth = False" << endreq;
    return HLT::OK;
  }
  
  
  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "InDetTrigDetailedTrackTruthMaker::execHLTAlgorithm()" << endreq;
  
  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // input track collection.
  //----------------------------------------------------------------------
  const TrackCollection* tracks = 0;
  
  if ( HLT::OK != getFeature(outputTE, tracks) ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endreq;
    return HLT::NAV_ERROR;
  }
  
  if(!tracks){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input track collection was not attached. Algorithm not executed!" << endreq;
    
    return HLT::OK; 
  } else {
    if(outputLevel <= MSG::VERBOSE)
      msg() << MSG::VERBOSE << " Input track collection has size " << tracks->size() << endreq;
    if ( tracks->size() == 0 ) {
      if(outputLevel <= MSG::DEBUG)
        msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endreq;
      return HLT::OK; 
    }
  }

  m_numTruthTracks = tracks->size();
  if (outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST: Retrieved input track collection with "
          << m_numTruthTracks << " tracks. " << endreq;
  
  //----------------------------------------------------------------
  // Retrieve prep raw data truth
  std::vector<const PRD_MultiTruthCollection*> vectorPRDCollections;
  std::string key;
  if ( HLT::OK != getFeatures(outputTE, vectorPRDCollections) ) {
    msg() << MSG::ERROR << " Input PRDT collections could not be found " << endreq;
  }

  m_numPRDs = vectorPRDCollections.size();
  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Got " << vectorPRDCollections.size() << " PRD collections." << endreq;
  
  std::vector<const PRD_MultiTruthCollection*> prdCollectionVector(vectorPRDCollections.size());
  
  std::vector<const PRD_MultiTruthCollection*>::const_iterator 
    thePRDColl = vectorPRDCollections.begin(),
    endPRDColl = vectorPRDCollections.end();
  
  for ( int i = 0 ;thePRDColl != endPRDColl;  thePRDColl++, i++){
    
    prdCollectionVector[i] = (*thePRDColl);
    if(!prdCollectionVector[i] || prdCollectionVector[i]->size() <=0){
      if(outputLevel <= MSG::DEBUG)
	msg() <<  MSG::DEBUG
	      << "Failed to retrieve input PRD Collection from the TE" << endreq;
    }
    else{
      if (outputLevel <= MSG::DEBUG){
	msg() << MSG::DEBUG << "REGTEST: Retrieved "
	      << i+1 << " input PRD collection with "
	      << prdCollectionVector[i]->size() << " clusters." 
	      << endreq;
	m_numPRDClusters.push_back(prdCollectionVector[i]->size());

      }
    }
  }
  //----------------------------------------------------------------
  // Produce and store the output.
  
  DetailedTrackTruthCollection *dttc = new DetailedTrackTruthCollection(tracks);
  m_truthTool->buildDetailedTrackTruth(dttc, *tracks, prdCollectionVector);
  
  //  Attach resolved tracks to the trigger element.
  if ( HLT::OK !=  attachFeature(outputTE, dttc, name() ) ) {
    msg() << MSG::ERROR << "Could not attache feature to the TE" << endreq;
  }
  
  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "DetailedTrackTruthCollection is registered in StoreGate, size="
	  << dttc->size() << endreq;
  
  // Convert DetailedTruthCollection to TrackTruthCollection
  TrackTruthCollection *out = new TrackTruthCollection(dttc->trackCollectionLink());
  
  fillOutput(out, dttc);
  
  //  Attach resolved tracks to the trigger element.
  if ( HLT::OK !=  attachFeature(outputTE, out, name()) ) {
    msg() << MSG::ERROR << "Could not attache feature to the TE" << endreq;
  }
  
  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "TrackTruthCollection is registered in StoreGate" 
	  << " size=" << out->size() << endreq;
  
  
  return HLT::OK;
}
//================================================================
void InDetTrigDetailedTrackTruthMaker::fillOutput(TrackTruthCollection *out, 
				    const DetailedTrackTruthCollection *in){
  
  typedef DetailedTrackTruthCollection::const_iterator Iter;
  
  Iter itrackData=in->begin();
  
  while(itrackData!=in->end()) {
    std::pair<Iter,Iter> range = in->equal_range(itrackData->first);
 
     // We KNOW that the range is not empty - no need to check that.
    Iter selected = range.first;
    double bestProb = getProbability(selected->second);

    for(Iter imatch = ++range.first; imatch != range.second; imatch++) {
      double prob = getProbability(imatch->second);
      if(prob>bestProb) {
         bestProb = prob;
         selected = imatch;
      }
    }
    
    // trajectory[0] is the LAST particle on the trajectory. The first
    // is at trajectory.rbegin(), but different trajectories can have
    // the same first particle.
    //    const HepMcParticleLink& particleLink = selected->second.trajectory()[0];
    const HepMcParticleLink& particleLink = *(selected->second.trajectory().rbegin());
 
    out->insert(std::make_pair(selected->first, TrackTruth(particleLink, bestProb, 0) ));
     itrackData=range.second;
  }
}
//================================================================
double InDetTrigDetailedTrackTruthMaker::getProbability(const DetailedTrackTruth& dt) const{

  double prd_track=0, prd_common=0;

  for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
    prd_common += m_subDetWeights[i] * dt.statsCommon()[SubDetHitStatistics::SubDetType(i)];
    prd_track += m_subDetWeights[i] * dt.statsTrack()[SubDetHitStatistics::SubDetType(i)];
  }

  return (prd_track>0)? prd_common/prd_track : -1.;
 }

} //end of namespace
//================================================================
//EOF
