/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetDenseEnvAmbiTrackSelectionTool.cxx, (c) ATLAS Detector software
// Anthony Morley based on the orginal tool by Markus Elsing
// Oct 2020 - Clean-up for rel22 - Gabriel Facini
///////////////////////////////////////////////////////////////////

#include "InDetAmbiTrackSelectionTool/InDetDenseEnvAmbiTrackSelectionTool.h"

#include "AthContainers/DataVector.h"
#include "CxxUtils/checker_macros.h"
#include "InDetIdentifier/SiliconID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventUtils/ClusterSplitProbabilityContainer.h"

#include "TString.h"

//================ Constructor =================================================

InDet::InDetDenseEnvAmbiTrackSelectionTool::InDetDenseEnvAmbiTrackSelectionTool(const std::string& t,
                                                                                const std::string& n,
                                                                                const IInterface*  p )
:
base_class(t,n,p)
{
}

//================ Initialisation =================================================

StatusCode InDet::InDetDenseEnvAmbiTrackSelectionTool::initialize()
{
  ATH_CHECK(AlgTool::initialize());

  if (m_IBLParameterSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
  } else {
    m_IBLParameterSvc->setBoolParameters(m_doPixelClusterSplitting.value(), "doPixelClusterSplitting");
  }

  // Get segment selector tool
  if (m_parameterization) {
    ATH_CHECK(m_selectortool.retrieve());
  } else {
    m_selectortool.disable();
  }

  ATH_CHECK(detStore()->retrieve(m_detID, "SiliconID"));
 
  ATH_CHECK(m_assoTool.retrieve());

  ATH_CHECK(m_inputHadClusterContainerName.initialize(m_useHClusSeed));
  ATH_CHECK(m_inputEmClusterContainerName.initialize(m_useEmClusSeed));

  return StatusCode::SUCCESS;
}


// @TODO move cluster "map" creation to separate algorithm
void InDet::InDetDenseEnvAmbiTrackSelectionTool::newEvent(CacheEntry* ent) const
{
  // Reload ROI's
  if (m_useHClusSeed) {
    ent->m_hadF.clear();
    ent->m_hadE.clear();
    ent->m_hadR.clear();
    ent->m_hadZ.clear();

    SG::ReadHandle<CaloClusterROI_Collection> calo(m_inputHadClusterContainerName);
    for ( const auto ccROI : *calo) {
      ent->m_hadF.push_back( ccROI->globalPosition().phi() );
      ent->m_hadE.push_back( ccROI->globalPosition().eta() );
      ent->m_hadR.push_back( ccROI->globalPosition().perp() );
      ent->m_hadZ.push_back( ccROI->globalPosition().z() );
    }
  }

  // Reload ROI's
  if (m_useEmClusSeed) {
    ent->m_emF.clear();
    ent->m_emE.clear();
    ent->m_emR.clear();
    ent->m_emZ.clear();

    SG::ReadHandle<CaloClusterROI_Collection> calo(m_inputEmClusterContainerName);
    for ( const Trk::CaloClusterROI* ccROI : *calo) {
      if ( ccROI->energy() * sin(ccROI->globalPosition().theta()) < m_minPtEm){
        continue;
      }
      ent->m_emF.push_back( ccROI->globalPosition().phi() );
      ent->m_emE.push_back( ccROI->globalPosition().eta() );
      ent->m_emR.push_back( ccROI->globalPosition().perp() );
      ent->m_emZ.push_back( ccROI->globalPosition().z() );
    }
  }
}


//============================================================================================
std::tuple<Trk::Track*,bool> InDet::InDetDenseEnvAmbiTrackSelectionTool::getCleanedOutTrack(const Trk::Track *ptrTrack,
                                                                                            const Trk::TrackScore score,
                                                                                            Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                                                            Trk::PRDtoTrackMap &prd_to_track_map) const
{

  // Test to see if we have a new event
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt!=ctx.evt()) { // New event in this slot
    //Fill ROI's
    newEvent(ent);

    ent->m_evt = ctx.evt();
  }
  
  // compute the number of shared hits from the number of max shared modules
  // reset every track as could be changed for tracks within an ROI
  // ROI matching is done within decideWhichHitsToKeep. Note mulitple ROI types
  ent->m_maxSharedModules = 2*m_maxSharedModules+1; // see header for meaning
  ent->m_minNotShared = m_minNotSharedHits;
  ent->m_minSiHits = m_minSiHitsToAllowSplitting;

  // cut on TRT hits, might use eta dependent cuts here
  int  nCutTRT = m_minTRT_Hits;
  if (m_parameterization) {
    const DataVector<const Trk::TrackParameters>* vpar = ptrTrack->trackParameters();
    const Trk::TrackParameters*                    par = (*vpar)[0];
    // ME bugfix TODO ... how old is this comment??
    int expected = m_selectortool->minNumberDCs(par);
    if (expected > nCutTRT) nCutTRT = expected; 
  }

  // get all TSOS the track
  const DataVector<const Trk::TrackStateOnSurface>* tsos = ptrTrack->trackStateOnSurfaces();
  ATH_MSG_DEBUG ("Study new Track "<< ptrTrack<<"\t , it has "<<tsos->size()<<"\t track states");
  
  //Create structs to hold track information
  //Values in here are used to decide if a track is kept or not
  TrackHitDetails trackHitDetails;

  ATH_MSG_DEBUG ("Filling in TrackDetails");
  TSoS_Details    tsosDetails(tsos->size());
  
  // Fill structs with information
  fillTrackDetails( ptrTrack, splitProbContainer, prd_to_track_map, trackHitDetails, tsosDetails);
  
  //Decide which hits to keep
  //This is a major function which checks the usage of each hit on the candidate track
  ATH_MSG_DEBUG ("decideWhichHitsToKeep");
  decideWhichHitsToKeep( ptrTrack,  score,  splitProbContainer, prd_to_track_map, trackHitDetails, tsosDetails, ent );
  
  ATH_MSG_DEBUG ("decideWhichHitsToKeep" << trackHitDetails.m_trkCouldBeAccepted );
  
  //
  // now see what to do with the track
  //
 
  ATH_MSG_DEBUG ("totalSiHits " << trackHitDetails.totalSiHits());
  ATH_MSG_DEBUG ("totalUniqueSiHits " << trackHitDetails.totalUniqueSiHits());
  ATH_MSG_DEBUG ("score " << score );


  //------------------------------------------------------------------------------------//
  //
  // All tracks must pass a minimum set of criteria 
  //
  // 1. Needs a minimum number of hits on track
  // This cut is also implmeneted in the scoring tool and usually the same values are set. 
  // A track which fails to have the minimum number of hits, gets a score of 0 
  // and will not make it this far. Might not be true for all versions of scoring tools.
  // The cut here maintains an explicit threshold
  //  
  // 2. Needs enough TRT hits. This is an important cut for back-tracking
  // 
  // 3. Needs to have enough unique hits (not shared with an accepted track)
  //
  //------------------------------------------------------------------------------------//
  bool passBasicCuts(true);
  if( trackHitDetails.totalSiHits() < m_minHits )                 { passBasicCuts = false; }
  if( trackHitDetails.m_numTRT_Unused < nCutTRT )                   { passBasicCuts = false; }
  if( trackHitDetails.totalUniqueSiHits() < ent->m_minNotShared ) { passBasicCuts = false; }
  if( !passBasicCuts ) {
    ATH_MSG_DEBUG ("reject track; failed basic cuts");
    return std::make_tuple(static_cast<Trk::Track *>(nullptr),false); // reject input track
  }
  //------------------------------------------------------------------------------------//


  //------------------------------------------------------------------------------------//
  //
  // Decide to keep track or not based on more complex criteria
  //  1. Check if track passes some criteria, including decision from decideWhichHitsToKeep
  //     If it passes, put the track into the output collection
  //  2. Those which fail are tested on a second set of criteria
  //     If it passes, strip off "RejectedHits" (decided in decideWhichHitsToKeep)
  //     and build new subtrack from all non-shared hits and the
  //     set of shared hits which pass some cuts.
  //       If 1. new track would have same hit content, save the original track
  //          2. if the new subtrack has less hits, and more than the min, created
  //             subtrack and reject the original
  //          3. if the new subtrack is below min hit cuts, junk it
  //  3. Those which fail are rejected with no chance for recovery
  //
  //------------------------------------------------------------------------------------//
  
  //------------------------------------------------------------------------------------//
  // Set some booleans for the up coming if/else statement

  // no shared hit at all
  // if shared hits, have additional requirements to check (3)
  bool noSharedHits( trackHitDetails.m_numShared == 0 );

  // set 2 bools for criteria placed on tracks with shared hits each criteria
  // 1/2 - too many shared modules?
  bool passSharedModulesCut( trackHitDetails.m_numWeightedShared < ent->m_maxSharedModules );

  // 2/2 - good quality?
  // Note, all tracks with a score of 0 are already removed
  bool passScoreCut( score > m_minScoreShareTracks );
  //------------------------------------------------------------------------------------//

  
  //------------------------------------------------------------------------------------//
  // Execute logic outlined above
  //
  // BEST CASE, we like this track and it passes this if statement
  // if have shared hits, not too many shared, and score ("quality") (note unique hits checked above)
  if( trackHitDetails.m_trkCouldBeAccepted && 
      ( noSharedHits || (passSharedModulesCut && passScoreCut) ) ) {
    ATH_MSG_DEBUG ("=> Suggest to keep track with "<<trackHitDetails.m_numShared<<" shared hits !");


    //  Change pixel hits property for shared hits as this track will be accepted into the final track colection
    if (!trackHitDetails.m_isPatternTrack){
      setPixelClusterSplitInformation( tsosDetails, splitProbContainer );
    }

    return std::make_tuple(static_cast<Trk::Track *>(nullptr),true); // keep input track

  }
  // Track failed to be accepted as is. 
  // Can we recover the track?
  else if ( passScoreCut ) {

    // catch, if this is cosmics, accept the incoming track
    if (m_cosmics) {
      ATH_MSG_DEBUG ("=> Cosmics, accept input track even with shared hits");
      return std::make_tuple(static_cast<Trk::Track *>(nullptr),true); // keep input track
    }

    //
    // Track is potentially ok, create a stripped down version 
    // from all hits not rejected
    // 

    ATH_MSG_VERBOSE ("Trying to recover track...gathering hits.");

    // decision on hits are already made
    // check the cleaning cuts and see if any of the rejected hits can be added back in
    // otherwise, go with what we have
    //
    // A candidate below the total hit cut will never make it this far. 
    // We are only extend tracks here if ever decide to override rejected hit decisions
     
    // loop over all TSOS (and types) and copy the good 
    // ones over to new TSOS vector
    std::vector<const Trk::TrackStateOnSurface*> newTSOS;
      
    // iterators for looping
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 

    for (int index = 0 ; iTsos != iTsosEnd ; ++iTsos,++index ) {

      // remove rejected hits
      if( tsosDetails.m_type[index] == RejectedHitOverUse || 
          tsosDetails.m_type[index] == RejectedHitInvalid || 
          tsosDetails.m_type[index] == RejectedHit ) {
        ATH_MSG_VERBOSE ("-> Dropping rejected hit");
        continue;
      }

      ATH_MSG_VERBOSE ("-> Copy good TSOS");
      newTSOS.push_back(*iTsos);

    } // loop over TSOS


    // newTSOS size would be hits on new track...how can the limit be 3? Is this backtracking?
    // Missing something? What configuration is relying on this cut? TODO
    if ( newTSOS.size() <= 3 ) {
      ATH_MSG_VERBOSE ("newTSOS.size(): "<<newTSOS.size() );
      ATH_MSG_DEBUG ("reject track; Too few hits, reject track with shared hits");
      return std::make_tuple(static_cast<Trk::Track *>(nullptr),false); // reject input track
    }

    // check that this is not the input track
    if ( newTSOS.size() == tsos->size() ) {
      ATH_MSG_DEBUG ("=> Recovered input track, accept it !");

      //  Change pixel hits property for shared hits as this is track will be accepeted into the final track colection
      if (!trackHitDetails.m_isPatternTrack){
        setPixelClusterSplitInformation( tsosDetails, splitProbContainer ); 
      }
      ATH_MSG_DEBUG ("reject track; maybe track was mark as rejected, but we recoverd it so no rejection");
      return std::make_tuple(static_cast<Trk::Track *>(nullptr),true); // keep input track
    } else {
      // ok, done, create subtrack
      Trk::Track* newTrack = createSubTrack(newTSOS,ptrTrack);
      if (!newTrack) {
        ATH_MSG_DEBUG ("=> Failed to create subtrack");
        ATH_MSG_DEBUG ("reject track; Failed to create subtrack");
        return std::make_tuple(static_cast<Trk::Track *>(nullptr),false); // reject input track
      }

      Trk::TrackInfo info;
      info.addPatternRecoAndProperties(ptrTrack->info());
      Trk::TrackInfo newInfo;
      newInfo.setPatternRecognitionInfo(Trk::TrackInfo::InDetAmbiTrackSelectionTool);
      info.addPatternReco(newInfo);
      newTrack->info().addPatternReco(ptrTrack->info()); 

      ATH_MSG_DEBUG ("=> Successfully created subtrack with shared hits recovered !");
      return std::make_tuple(newTrack,false); // create new, cleaned track and reject input track
    }
  }

  // if made it this far, junk the track
  ATH_MSG_DEBUG ("=> Track is recommended to be dropped");
  ATH_MSG_DEBUG ("reject track; other");
  return std::make_tuple(static_cast<Trk::Track *>(nullptr),false); // reject input track

}


//
// Check if a shared hit invalidates any accepted track, if so, return false
// otherwise return true
// ONLY Shared hits are expected. We assume the decision to split has already been made
bool InDet::InDetDenseEnvAmbiTrackSelectionTool::checkOtherTracksValidity(TSoS_Details& tsosDetails,
    int index,
    Trk::ClusterSplitProbabilityContainer &splitProbContainer,
    Trk::PRDtoTrackMap &prd_to_track_map,
    int& maxiShared, 
    int& maxOtherNPixel, 
    bool& maxOtherHasIBL, 
    CacheEntry* ent) const
{

  // Hits here are only shared hits. 
  // Assumption: the decision to split has been done outside this function
  if( tsosDetails.m_type[index] != SharedHit ) {
    ATH_MSG_WARNING("Calling checkOtherTracksValidity with a hit that is not shared!");
    return true;
  }

  // needed?
  const Trk::RIO_OnTrack* rot = tsosDetails.m_RIO[index];

  // get list of tracks that use this hit already
  int numberOfTracksWithThisPrd = tsosDetails.m_hitIsShared[index];
  ATH_MSG_VERBOSE ("---> number of tracks with this shared Prd: " << numberOfTracksWithThisPrd << " maxtracks: " << m_maxTracksPerPRD);

  // check if adding one more shared hit would push any accepted track 
  //  -over the limit of shared hits
  //  -under the limit of unique hits
  maxiShared = -1;
  bool otherwillFailMinHits = false;
  bool otherwillFailSCTuniqueHits = false;

  //Setup counters
  int  iSharedMod     = 0;  //Number of Shared modules on the accepted track (will include this hit)
  int  iNotShared     = 0;  //Number of unshared hits on the accepted track
  int  iSctUnique     = 0;  //Number of unique SCT hits on the accepted track 
  int  iPixel         = 0;  //Number of pixel hits on the accepted track
  int  iSCT           = 0;  //Number of SCT hits on the accepted track
  bool iHasBlayer     = false; // Does the accepted track have a b-layer hit

  // loop over all tracks that have this hit and make sure none fail
  //  - requirement on unique hits
  //  - requirement on shared modules
  //  - requirement on unique sct hits
  //  send back some information about the most overlapping track
  auto currentHitsSharedTracks = tsosDetails.m_tracksSharingHit.equal_range(index);
  for ( auto track = currentHitsSharedTracks.first; track != currentHitsSharedTracks.second; ++track ){

    //Count up the hit properties of the track you are inspecting 
    iSharedMod     = 0;
    iNotShared     = 0;             
    iSctUnique     = 0;
    iPixel         = 0;
    iHasBlayer     = false;
    iSCT           = 0; 

    std::vector< const Trk::PrepRawData* > prdsToCheck = m_assoTool->getPrdsOnTrack(prd_to_track_map, *(track->second));
    for (const Trk::PrepRawData* prdToCheck : prdsToCheck) {
      bool isPixel(false);
      bool isSplitPixel(false);
      if (m_detID->is_pixel(prdToCheck->identify())) {
        isPixel = true;
        const InDet::PixelCluster* constPixelCluster = dynamic_cast<const InDet::PixelCluster*> ( prdToCheck  );    
        if (constPixelCluster){    
          const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = splitProbContainer.splitProbability(constPixelCluster);
          if ( splitProb.isSplit() )  {
            isSplitPixel = true;
          }
        }

        ++iPixel;
        if (m_detID->is_blayer(prdToCheck->identify()) ) iHasBlayer=true;
      }

      if ( m_detID->is_sct(prdToCheck->identify()) ) {
        ++iSCT;
      }

      //
      // count for general cases considering current flags (i.e. if split can be shared)
      // hit which are overused, are rejected and should not be in this part of the code
      //  if hit is new one to be shared, count it as shared
      //    - the new cluster to be shared is not a split cluster (that is why we are here)
      //    - this will count correctly if a split hit can also be shared
      if ( rot->prepRawData() == prdToCheck ) {
        iSharedMod += isPixel ? 2 : 1;
      //  else if hit is not the new hit to be shared, then see if it is already shared
      //    - if so, count considering if it is split or not
      } else if ( prd_to_track_map.isShared( *prdToCheck ) ) {
        iSharedMod += isSplitPixel ? 0 : isPixel ? 2 : 1;
      //  else 
      //    - not a shared cluster
      } else {
        ++iNotShared;
        if (m_detID->is_sct(prdToCheck->identify())){
          ++iSctUnique;
        }
      }
    } // loop over PRDs

    ATH_MSG_VERBOSE( "Track " <<  track->second << " will has " << iNotShared << " unique hits and  " <<  iSctUnique << " unique SCT hits.");
    // You are sharing too many hits reject the new track
    // TODO does not include dead modules! Can we get that from the track summary?
    if ( iNotShared  < ent->m_minNotShared ) otherwillFailMinHits = true;
    // You are sharing SCT hits and don't have enough to share -reject hit.
    if ( iSCT > iSctUnique && iSctUnique < m_minUniqueSCTHits ) otherwillFailSCTuniqueHits = true;    
  } // loop over tracks
    
  // Update details for the track with the most shared hits          
  if ( iSharedMod > maxiShared){
    maxiShared        = iSharedMod;
    maxOtherNPixel    = iPixel;
    maxOtherHasIBL    = iHasBlayer;
  }
   
  return !( otherwillFailMinHits || otherwillFailSCTuniqueHits );
}



void InDet::InDetDenseEnvAmbiTrackSelectionTool::fillTrackDetails(const Trk::Track* ptrTrack,
                                                                  Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                                  const Trk::PRDtoTrackMap &prd_to_track_map,
                                                                  TrackHitDetails& trackHitDetails, 
                                                                  TSoS_Details& tsosDetails) const
{ 

  // get all TSOS the track
  const DataVector<const Trk::TrackStateOnSurface>* tsos = ptrTrack->trackStateOnSurfaces();
  ATH_MSG_DEBUG ("Study new Track "<< ptrTrack<<"\t , it has "<<tsos->size()<<"\t track states");


  // is this a track from the pattern or a fitted track ?
  trackHitDetails.m_isPatternTrack = (ptrTrack->info().trackFitter()==Trk::TrackInfo::Unknown);
  if (trackHitDetails.m_isPatternTrack) {
    ATH_MSG_DEBUG ("-> this is a pattern track, outliers are good hits (reintegration) ! " << ptrTrack->perigeeParameters() );
  } else {
    ATH_MSG_DEBUG ("-> this is a refitted track, so we can use the chi2 ! " << ptrTrack->perigeeParameters());
  }

  // some pre-processing of the summary information, if available, needed for special cuts
  const Trk::TrackSummary* trkSummary=ptrTrack->trackSummary();
  if (trkSummary) {
    ATH_MSG_VERBOSE ("--> Found summary information");
    trackHitDetails.m_numPixelDeadSensor = trkSummary->get(Trk::numberOfPixelDeadSensors);
    trackHitDetails.m_numSCTDeadSensor   = trkSummary->get(Trk::numberOfSCTDeadSensors);
    trackHitDetails.m_numPixelHits       = trkSummary->get(Trk::numberOfPixelHits);
    trackHitDetails.m_numPixelHoles      = trkSummary->get(Trk::numberOfPixelHoles);
    trackHitDetails.m_numSCTHoles        = trkSummary->get(Trk::numberOfSCTHoles);
    trackHitDetails.m_numSCTHits         = trkSummary->get(Trk::numberOfSCTHits);
  } else {
    ATH_MSG_WARNING("Did not find track summary. Some cuts will be less efficienct!");
  }


  // set nDeadSensors to 0 in case trkSummary wasn't called with HoleSearch
  // (i.e. number of deadSensors not available)
  if (trackHitDetails.m_numPixelDeadSensor == -1) trackHitDetails.m_numPixelDeadSensor = 0;
  if (trackHitDetails.m_numSCTDeadSensor   == -1) trackHitDetails.m_numSCTDeadSensor   = 0;
  ATH_MSG_VERBOSE ("--- Number of dead si sensors: " << trackHitDetails.m_numPixelDeadSensor + trackHitDetails.m_numSCTDeadSensor);


  //
  // loop over TSOS and save information about each
  // label each TSOS as Outlier, UnusedHit, SharedHit or RejectedHit (only for errors here)
  // labels are used refined in decideWhichHitsToKeep 
  ATH_MSG_VERBOSE ("--> Looping over TSOS's");
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();
  for (int index = 0 ; iTsos != iTsosEnd ; ++iTsos, ++index) {

    // get measurment from TSOS
    const Trk::MeasurementBase* meas = (*iTsos)->measurementOnTrack();

    // if we do not have a measurement, we should just mark it
    if (!meas) {
      ATH_MSG_VERBOSE ( Form("---> No measurement on TSOS, it is another type, %2d",index) );
      tsosDetails.m_type[index] = OtherTsos;
      continue;
    }

    // ok, let try to get the ROT then
    const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (meas);
    if (!rot) {
      // could be a Pseudo-Measurement ?
      const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast <const Trk::PseudoMeasurementOnTrack*> (meas);
      if (pseudo){
        ATH_MSG_VERBOSE ( Form("---> Pseudo measurement, %2d",index) );
        ++trackHitDetails.m_numPseudo; // increase counter
      } else {
        ATH_MSG_WARNING ( Form("---> Measurement is not a pseudo measurment, not yet supported! %2d",index));
      }
      tsosDetails.m_type[index] = OtherTsos;
      continue;
    }
    
    //Store RIO into vector for later use
    tsosDetails.m_RIO[index] = rot;

    //
    // we have a TSOS with a measurement, keep analysing it
    //

    // let's get some information about the measurement
    const Identifier& id = rot->identify();
    bool isTRT           = m_detID->is_trt(id);
    bool isPixel         = m_detID->is_pixel(id);
    bool isSCT           = m_detID->is_sct(id);
    bool isIBL           = isPixel ? m_detID->is_blayer(id) : false; // checks layer 0
    bool isoutlier       = (*iTsos)->type(Trk::TrackStateOnSurface::Outlier);              

    tsosDetails.m_detType[index] = isTRT * 3 + isSCT * 2 + isPixel * 1 + isIBL * 10;

    // Do we have a IBL hit on the track?
    if ( isIBL && (!isoutlier || trackHitDetails.m_isPatternTrack ) ) {
      trackHitDetails.m_thisHasIBLHit = true;  // we may reintegrate outliers from pattern 
    }

    // Add splitting information about the cluster
    if (isPixel) {
      // get pixel cluster
      const InDet::PixelCluster* clus = dynamic_cast <const InDet::PixelCluster*> (rot->prepRawData());
      if ( !clus ) {
        ATH_MSG_WARNING ("---> Cast to Pixel cluster failed, should not happen !");
        tsosDetails.m_type[index] = RejectedHit;
        continue; 
      } else {
        const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = splitProbContainer.splitProbability(clus);
        if ( !splitProb.isTooBigToBeSplit() )  {
          tsosDetails.m_splitProb1[index] = splitProb.splitProbability1();
          tsosDetails.m_splitProb2[index] = splitProb.splitProbability2();
        } else { 
          // cluster too big to split are default to 3 particle cluster
          // rigorously checked?
          tsosDetails.m_splitProb1[index] = m_sharedProbCut - 0.01;
          tsosDetails.m_splitProb2[index] = m_sharedProbCut2 + 0.01;
        }
      }
    } // isPixel


    //
    // define each hit
    // continue once definition set
    //

    // is this cluster used on an accepted track?
    bool isUsed = prd_to_track_map.isUsed(*(rot->prepRawData()));

    // assign outlier label using isoutlier bool above 
    // or if not inDet (does this even happen? Historical criteria)
    // further criteria on keeping a hit or not is done in decideWhichHitsToKeep
    // NOTE outliers can be shared but cannot save both in tsosDetails
    if( isoutlier || !(m_detID->is_indet(id))) {
      ATH_MSG_VERBOSE ( Form("---> Prd is outlier, %d", index) );
      tsosDetails.m_type[index] = Outlier;
      // if it is used, we need to send that information to the rest of the code
      // in decideWhichHitsToKeep we reject any outlier used on another track
      // if that logic was to change, and the number of tracks was needed, this would
      // need to change
      if(isUsed) { tsosDetails.m_hitIsShared[index] = 1; }
      continue;
    }

    
    if (!isUsed) {
      ATH_MSG_VERBOSE ( Form("---> Prd is unused, %d", index) );
      tsosDetails.m_type[index] = UnusedHit;
      continue;
    }

    //
    // we have a shared hit
    //


    // check if it is a shared TRT hit, this is not allowed
    if (isTRT) {
      ATH_MSG_VERBOSE (  Form("---> Share TRT hit, drop it !, %d", index) );
      tsosDetails.m_type[index] = RejectedHit;
      continue;
    }

    /* Allow a hit to be a shared one, if
       - not too many tracks share this hit already
       - the score of the track is high enough to allow for shared hits
       store information here but make decisions in decideWhichHitsToKeep
    */
    Trk::PRDtoTrackMap::ConstPrepRawDataTrackMapRange range = prd_to_track_map.onTracks(*(rot->prepRawData()));
    int numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    ATH_MSG_VERBOSE ( Form("---> Number of tracks with this share Prd %d: %2d maxtracks: %2d",index, numberOfTracksWithThisPrd, m_maxTracksPerPRD.value()) );
    tsosDetails.m_hitIsShared[index] = numberOfTracksWithThisPrd;


    // get iterators for range
    Trk::PRDtoTrackMap::PrepRawDataTrackMap::const_iterator  mapIt    = range.first;
    Trk::PRDtoTrackMap::PrepRawDataTrackMap::const_iterator mapItEnd = range.second;
    // simple for loop instead of fancier remove_if above
    for ( ;mapIt!=mapItEnd; ++mapIt) {
      tsosDetails.m_overlappingTracks.insert( std::pair<const Trk::Track*, int >(mapIt->second, index) );
      tsosDetails.m_tracksSharingHit.insert(std::pair< int, const Trk::Track* >(index, mapIt->second ) ); 
    }
 
    ATH_MSG_VERBOSE ("-----> Mark this hits as shared  -- Try and recover later!");
    ATH_MSG_VERBOSE ("------ Index: "<< index << " Type: " << tsosDetails.m_detType[index]  << " splitprob1  " << tsosDetails.m_splitProb1[index]);
    tsosDetails.m_type[index] = SharedHit;
  } // End loop over TSOS's
  
  return;
}

//==========================================================================================
//
// look at cluster properties and how often they are used to determine if can stay on track
//
void InDet::InDetDenseEnvAmbiTrackSelectionTool::decideWhichHitsToKeep(const Trk::Track* ptrTrack,
    const Trk::TrackScore score,
    Trk::ClusterSplitProbabilityContainer &splitProbContainer,
    Trk::PRDtoTrackMap &prd_to_track_map,
    TrackHitDetails& trackHitDetails,
    TSoS_Details& tsosDetails,
    CacheEntry* ent) const 
{

  // Can the track can automatically be accpeted without further checks
  trackHitDetails.m_trkCouldBeAccepted = true;

  // Does this track fall into an hadronic ROI?
  trackHitDetails.m_passHadronicROI = false;
  if( m_useHClusSeed && inHadronicROI(ptrTrack, ent) ) {
    trackHitDetails.m_passHadronicROI = true;
  }
 
 
  //------------------------------------------------------------------//
  // Should a shared hit be called shared or is it compatible with multiple clusters?
  // this flag controls if the additional information is conisdered and shared hits on this track
  // are "recovered" from the shared state to the split state
  // 
  // First criteria are checked which would NOT allow recovery
  // Then override criteria are checked
  bool recoverSharedHits(true);
  // reasons NOT to consider the information
  // not above pT for splitting
  if( ptrTrack->trackParameters()->front()->pT() < m_minPtSplit ) { recoverSharedHits = false; }
  // score too low
  if(score <= m_minScoreShareTracks) { recoverSharedHits = false; }
  // the following 2 cuts use information from the track summary stored in the details struct
  // not enough SiHits 
  if(trackHitDetails.m_numPixelHits+trackHitDetails.m_numSCTHits < m_minSiHitsToAllowSplitting) {
    recoverSharedHits = false;
  }
  // too many holes
  if(trackHitDetails.m_numPixelHoles>1) { recoverSharedHits = false; }



  //------------------------------------------------------------------//
  //
  // loop over all measurements, make decisions, increment counters
  //
  for (unsigned int index(0);  index  < tsosDetails.m_nTSoS; ++index ){
    //Skip all non-measurement TSOS's 
    if (tsosDetails.m_type[index] == OtherTsos) { continue; }

    // in fillTrackDetails, if an error, cluster could be rejected
    // also shared TRT hits are rejected there
    if (tsosDetails.m_type[index] == RejectedHit) {  // only generic rejects at that point
      trackHitDetails.m_trkCouldBeAccepted = false; // we have to remove at least one PRD
      continue;
    }

    // Keep all unused hits
    if (tsosDetails.m_type[index] == UnusedHit ){
      if ( tsosDetails.m_detType[index] == 3 ) {
        ++trackHitDetails.m_numTRT_Unused;
      } else {
        ++trackHitDetails.m_numUnused;
        if ( tsosDetails.m_detType[index] == 2 ) { ++trackHitDetails.m_numSCT_Unused; }
      }
      continue;
    }

    //===================================================================//
    //
    // OUTLIERS - not type Outlier is given to both Shared and UnShared hits. Sort it out here
    // Note in fillTrackDetails, for outliers we saved "1" in m_hitIsShared for any used hit, 
    // even if the true value is higher. As we will reject them here, it does not matter 
    // how many it is shared with
    // if a pattern track 
    //    - used   : reject the hit
    //    - unused : call it unused and move on (reintegration of pattern outliers)
    // if fitted track
    //    - used   : reject the hit
    //    - unused : keep it but do not call it an unused hit (not counted towards total SiHit count)
    //
    if( tsosDetails.m_type[index] == Outlier ) {

      // if on another track, get it off of this one
      // for an outlier, we need to check m_hitIsShared to see if it used
      if( tsosDetails.m_hitIsShared[index]>0 ) { // hit is used on another track
        ATH_MSG_VERBOSE( Form("---> Prd is outlier and shared, reject it. %d", index) );
        rejectHit(trackHitDetails, tsosDetails, index);
        continue;
      }

      if(trackHitDetails.m_isPatternTrack) {

        // For pattern tracks:
        // if used   : reject it  --> see above
        // if unused : reintegrate it and send to track fit
        ATH_MSG_VERBOSE ( Form("---> Prd is outlier on a pattern track and is unused, %d", index) );
        // change to unused hit, increment counter
        tsosDetails.m_type[index] = UnusedHit;  // change name so m_numUnused matches types on track
        ++trackHitDetails.m_numUnused;
        if( tsosDetails.m_detType[index]%10 == 2 ) { ++trackHitDetails.m_numSCT_Unused; }
        continue;
      }
      else { // is outlier on fitted track and ...
        // used  : reject the hit --> see above
        // unused: send it through, but do not count it towards the hit content
        ATH_MSG_DEBUG( Form("---> Prd is outlier on a fitted track and is unused, %d", index) );
        continue;
      } // m_isPatternTrack or not
    } // isOutlier or not
    //===================================================================//

    // cleaning cut: fitted track with 1 pixel hit without an IBL hit
    /* hurts conversion performance, slighly better perf in min-bias without it
     * but high pT jets can use this in the core
    if(!trackHitDetails.m_isPatternTrack  &&  // fitted track
        trackHitDetails.m_numPixelHits==1 &&  // 1 pixel hit
        !trackHitDetails.m_thisHasIBLHit  &&  // no IBL hits
        tsosDetails.m_detType[index]%10 == 1  && // index is the pixel hit
        !(tsosDetails.m_type[index]==Outlier)) { // and it is not an outlier) 
       ATH_MSG_VERBOSE ("-> Special case, problematic single pixel hit on fitted track, reject it !");
       TrkCouldBeAccepted = false; // we have to remove at least one PRD
       tsosDetails.m_type[index]    = RejectedHit;
       continue;             
    }
    */

    // At this point, only hits labeled shared from fillTrackDetails
    if( tsosDetails.m_type[index] != SharedHit ) {
      ATH_MSG_ERROR("A NON-Shared hit is being treated as one!");
    }

    //===================================================================//
    // Try and recover shared hits if
    //   - pixel cluster splitting flag is turned on
    //   - track passes criteria for cluster splitting
    //   - is a pixel hit
    if ( m_doPixelClusterSplitting && recoverSharedHits && tsosDetails.m_detType[index]%10 == 1 ) {

      //Find the index of the previous measurement on track
      int indexPreviousMeasurement = tsosDetails.findIndexOfPreviousMeasurement(index);
      if (indexPreviousMeasurement > 0 ) {
        ATH_MSG_VERBOSE ("--> Previous Measurement was at : " << indexPreviousMeasurement 
            << " and was a: "  << tsosDetails.m_type[indexPreviousMeasurement] 
            << " with splitprob1 " << tsosDetails.m_splitProb1[indexPreviousMeasurement] 
            << " and splitprob2 "  << tsosDetails.m_splitProb2[indexPreviousMeasurement]);
      }

      // If the previous measurement is a shared pixel and the current pixel 
      //  - wants to be shared 
      //  - and is compatible with being from multiple clusters
      // allow the previous measurement to be shared with up to m_maxTracksPerPRD tracks without penalty 
      if ( indexPreviousMeasurement >= 0 && 
          tsosDetails.m_type[indexPreviousMeasurement] == SharedHit &&
          tsosDetails.m_hitIsShared[indexPreviousMeasurement] < m_maxTracksPerPRD &&
          clusCanBeSplit(tsosDetails.m_splitProb1[index],tsosDetails.m_splitProb2[index]) ) {

        // Check if the previous shared hit shared on the same track as this one 
        bool sharedbetweenthesametracks = false;
        auto previousHitsSharedTracks = tsosDetails.m_tracksSharingHit.equal_range(indexPreviousMeasurement);
        auto currentHitsSharedTracks  = tsosDetails.m_tracksSharingHit.equal_range(index);
        for (auto iteratorP = previousHitsSharedTracks.first; 
            iteratorP != previousHitsSharedTracks.second; ++iteratorP) {
          for (auto iteratorC = currentHitsSharedTracks.first; 
              iteratorC != currentHitsSharedTracks.second; ++iteratorC) {
            if ( iteratorC->second == iteratorP->second){
              sharedbetweenthesametracks = true;
              break;                
            }
          }
          if (sharedbetweenthesametracks) break;
        }
        if (sharedbetweenthesametracks){
          sharedToSplitPix(trackHitDetails, tsosDetails, indexPreviousMeasurement);
        }          
      } 

      //
      //  Check the number network. 
      //   - check if the particle is compatible with 2 particles
      //   - else check if particle is compatible with >= 3 particles
      //   - else it is a 1 particle cluster
      //   Is a 2 particle cluster?
      if ( isTwoPartClus(tsosDetails.m_splitProb1[index],tsosDetails.m_splitProb2[index]) ) {

        // can be split, but how many accepted tracks use the hit?
        // okay to share
        if (tsosDetails.m_hitIsShared[index] < m_maxPixTwoPartCluster) {
          ATH_MSG_VERBOSE ("---> Pixel cluster is to be split shared with another track: Split Prob1 " << tsosDetails.m_splitProb1[index] );
          tsosDetails.m_type[index] = SplitSharedHit;
          ++trackHitDetails.m_numSplitSharedPix;
          continue;
        } else if(m_shareSplitHits && tsosDetails.m_hitIsShared[index] == m_maxPixTwoPartCluster) { 
          // if we allow split hits to also be shared
          //  - the first m_maxPixTwoPartCluster cluster get to use it without penalty
          //  - the m_maxPixTwoPartCluster+1 cluster gets called shared
          ATH_MSG_VERBOSE ("---> Pixel cluster is split, but must be called shared to stay on this track!");
          addSharedHit(trackHitDetails, tsosDetails, index);
          continue;
        } else {
          ATH_MSG_VERBOSE ("---> Pixel split but shared between too many tracks -- will be removed from the track!!!");
          rejectHitOverUse(trackHitDetails, tsosDetails, index);
          continue;
        }

        //  Is a 3+ particle cluster?
      } else if ( isMultiPartClus(tsosDetails.m_splitProb2[index]) ) {

        // can be split, but how many accepted tracks use the hit?
        // okay to share
        if (tsosDetails.m_hitIsShared[index] < m_maxPixMultiCluster ){
          ATH_MSG_VERBOSE ("---> Pixel cluster is to be split shared with another track: Split Prob2 " <<  tsosDetails.m_splitProb2[index] );
          tsosDetails.m_type[index] = SplitSharedHit;
          ++trackHitDetails.m_numSplitSharedPix;
          continue;             
        } else if(m_shareSplitHits && tsosDetails.m_hitIsShared[index] == m_maxPixMultiCluster) { 
          // if we allow split hits to also be shared
          //  - the first m_maxPixMultiCluster cluster get to use it without penalty
          //  - the m_maxPixMultiCluster+1 cluster gets called shared
          ATH_MSG_VERBOSE ("---> Pixel cluster is split, but must be called shared to stay on this track!");
          addSharedHit(trackHitDetails, tsosDetails, index);
          continue;
        } else {
          ATH_MSG_VERBOSE ("---> Pixel split but shared between too many tracks -- will be removed from the track!!!");
          rejectHitOverUse(trackHitDetails, tsosDetails, index);
          continue;
        }

      }  // end of checking number network output

      ATH_MSG_VERBOSE("---> Pixel cluster is not compatible with being shared (splitProb1 = " 
          << tsosDetails.m_splitProb1[index] << "), (splitProb2 = " 
          << tsosDetails.m_splitProb2[index] << ") , reject shared hit !!!");
      if ( tsosDetails.m_hitIsShared[index] < m_maxPixOnePartCluster ) {
        // this is a shared cluster
        addSharedHit(trackHitDetails, tsosDetails, index);
        continue;
      } else {
        ATH_MSG_DEBUG ("reject track; Too many hits shared - we have to remove at least one PRD");    
        rejectHitOverUse(trackHitDetails, tsosDetails, index);
        continue; 
      }
    }// End Attempt to recover shared hits
    //===================================================================//

    if( tsosDetails.m_hitIsShared[index] == 0 ) {
      ATH_MSG_ERROR("A hit is not shared but is where only shared hits should be");
    }

    //For all other shared hits 
    if ( tsosDetails.m_hitIsShared[index] < m_maxTracksPerPRD ){  // we do not allow to share with to many tracks
      ATH_MSG_VERBOSE ("---> Shared hit, but good track, let's enter hit in the list and try to keep it !");
      ATH_MSG_VERBOSE ("----- Index: "<< index << " Type: " << tsosDetails.m_detType[index]  << " splitprob1  " << tsosDetails.m_splitProb1[index]);
      addSharedHit(trackHitDetails, tsosDetails, index);
      continue;
    } else {
      ATH_MSG_DEBUG ("reject track; Too many hits shared - we have to remove at least one PRD");
      rejectHitOverUse(trackHitDetails, tsosDetails, index);
      continue;     
    }
    ATH_MSG_ERROR("Reached end of TSOS loop without a decision." );
  } // loop over TSOS 
  //------------------------------------------------------------------//

  //------------------------------------------------------------------//
  //
  // COUNTERS have been set above. From here on, if change the state
  // of any hit, must also adjust counters
  //
  //------------------------------------------------------------------//

  //------------------------------------------------------------------//
  // Check if the pair is compatible with being a light particle decay 
  // This focuses on conversions
  // The track in question must 
  //  - have an overlapping hit with an accepted track
  //  - those two track have a small separation 
  //
  trackHitDetails.m_passConversionSel = false; // make sure off to start
  if (m_doPairSelection && tsosDetails.m_overlappingTracks.size() > 0) {
    trackHitDetails.m_passConversionSel = performConversionCheck(ptrTrack, 
        prd_to_track_map, trackHitDetails, tsosDetails, ent);
  }
  //------------------------------------------------------------------//

  //------------------------------------------------------------------//
  // Also check if the pair is compatible with being a boosted hadronic topology. 
  // This branch focuses on high-pt taus and b-jets in particular  
  // Same conditions as above.
  // Do not process any track already modified by the conversion check
  //
  if (m_useHClusSeed && !trackHitDetails.m_passConversionSel && trackHitDetails.m_passHadronicROI && 
    m_doPairSelection && tsosDetails.m_overlappingTracks.size() > 0) {
    trackHitDetails.m_passConversionSel = performHadDecayCheck(ptrTrack, 
        prd_to_track_map, trackHitDetails, tsosDetails, ent);
  }
  
  //------------------------------------------------------------------//
  //
  // Three cuts which can pull shared hits off of the track (no order)
  //   1. min unique SCT hits to share hits
  //   2. min chi2/ndf to share hits
  //
  // We shared SCT hits when we don't really have enough to share
  // Reject SCT shared hits on the track 
  if ( trackHitDetails.m_numSCT_Unused != trackHitDetails.m_numSCTHits && // have shared SCT
       trackHitDetails.m_numSCT_Unused <  m_minUniqueSCTHits ){
    for (unsigned int index( 0 );  index > tsosDetails.m_nTSoS; ++index ){    
      if ( tsosDetails.m_detType[index]==2 && tsosDetails.m_type[index] == SharedHit){
        rejectSharedHit(trackHitDetails, tsosDetails, index);
      }      
    }
  }
  
  // Cut on chi2/NDF if shared hits on track
  // if fail cuts, remove all shared hits and reject track
  if ( !trackHitDetails.m_isPatternTrack && trackHitDetails.m_numShared > 0) {
    double trackchi2 = 0;
    if  (ptrTrack->fitQuality() && ptrTrack->fitQuality()->numberDoF()>0 ) {
      trackchi2 = ptrTrack->fitQuality()->chiSquared()/ptrTrack->fitQuality()->numberDoF();  
    }

    // if track fails cut for shared hits, remove the shared hits
    if ( trackchi2 > m_minTrackChi2ForSharedHits ) {
      ATH_MSG_DEBUG ("Shared hits, we have a bad chi2 track, mark it as bad !");
      // remove shared hits and see if track survives
      for (unsigned int index( 0 );  index > tsosDetails.m_nTSoS; ++index ){    
        if ( tsosDetails.m_type[index] != SharedHit ) { continue; }
        rejectSharedHit(trackHitDetails, tsosDetails, index);
      }
    } // fails cut
  } // is not a pattern track and has shared hits

  //------------------------------------------------------------------//

  
  //------------------------------------------------------------------//
  // Check accepted tracks with the surviving list of shared hits
  // Remove shared hits which push track over shared module cut
  // Set flags
  //
  // add up weighted count of shared hits on track, start removing
  // hits after counter is above cut. This preferences inner hits
  int newNumWeightedShared = 0; // tmp counter
  bool firstMeasurement(true);
  trackHitDetails.m_firstPixIsShared = false;
  // was already set in fillTrackDetails, but set again depending if blayer was rejected or not
  trackHitDetails.m_thisHasIBLHit = false;
  for (unsigned int index(0);  index  <tsosDetails.m_nTSoS; ++index ){

    // don't count rejected hits when setting flags
    if (tsosDetails.m_type[index] == RejectedHitOverUse ||
        tsosDetails.m_type[index] == RejectedHitInvalid ||
        tsosDetails.m_type[index] == RejectedHit ){ continue; }

    // Any track which is still labeled as SharedHit has to be checked 
    // against the accepted tracks. Accepted track should not be pushed
    // above cuts imposed here, some historic cuts remain
    if (tsosDetails.m_type[index] == SharedHit) {

      int  maxiShared     = -1;
      int  maxOtherNPixel = 0;
      bool maxOtherHasIBL = false;
      bool otherPassMinUniqueHits = checkOtherTracksValidity( tsosDetails, index, 
          splitProbContainer, prd_to_track_map, maxiShared, maxOtherNPixel, 
          maxOtherHasIBL, ent);

      if (!otherPassMinUniqueHits) {
        ATH_MSG_DEBUG ("reject track; Tracks shared hits does not leave enough unique hits on accepted track");
        rejectSharedHitInvalid(trackHitDetails, tsosDetails, index);
        continue;
      }

      // 
      // do not allow other accepted track to exceed the shared limit...
      // ...IF first pixel hit is shared  (historic cut)
      // NOTE: here "pixel" includes IBL
      // tightening cut by dropping shared hit clause impacts performance in high pT jet cores
      if( maxiShared >= ent->m_maxSharedModules ){

        // if this is the first hit (it is shared), and a pixel, reject
        if( (tsosDetails.m_detType[index] % 10 == 1) && firstMeasurement ) {
          ATH_MSG_DEBUG ("reject track; Tracks shared hits pushes accepted track above shared module limit");
          rejectSharedHitInvalid(trackHitDetails, tsosDetails, index);
          continue;
        }
        // if first pixel was shared (and this is not that hit)
        if( trackHitDetails.m_firstPixIsShared ) {
          ATH_MSG_DEBUG ("reject track; Tracks shared hits pushes accepted track above shared module limit");
          rejectSharedHitInvalid(trackHitDetails, tsosDetails, index);
          continue;
        }

        // unlinked (no truth particle) tracks tend to have low hit counts
        // don't let track that look like this touch accepted tracks (new)
        if( trackHitDetails.totalPixelHits() < m_minPixHitAccepted ) { 
          rejectSharedHitInvalid(trackHitDetails, tsosDetails, index);
          continue;
        }

      } // accepted track exceeds shared module cuts


      // 
      // only allow shared pixel if both have IBL or both not (historic cut)
      // if this is IBL hit, and other does not have IBL hit, remove shared IBL hit
      if( tsosDetails.m_detType[index] == 11 && !maxOtherHasIBL ) {
        ATH_MSG_VERBOSE ("---> Remove shared IBL as MaxShared accepted does not have an IBL hit");
        rejectSharedHit(trackHitDetails, tsosDetails, index);
        continue;
      }
      // if this is pixel hit, and candidate does not match IBL content of MaxShared accepted, remove shared hit
      if(tsosDetails.m_detType[index] == 1 && (trackHitDetails.m_thisHasIBLHit != maxOtherHasIBL) ) {
        ATH_MSG_VERBOSE ("---> Only allow shared pixel if candidate and accepted have same IBL hit content");
        rejectSharedHit(trackHitDetails, tsosDetails, index);
        continue;
      }

      // number of shared modules to be added to new sub-track
      // add shared hit to temporary counter
      newNumWeightedShared += (tsosDetails.m_detType[index]%10== 1 ? 2 : 1); // increase counter
      // should remain beow the threshold 
      if (newNumWeightedShared >= ent->m_maxSharedModules) {
        ATH_MSG_VERBOSE ("-> Too many share hits, dropping outer hit(s) " 
            << newNumWeightedShared << "\t" << ent->m_maxSharedModules);  
        newNumWeightedShared -= (tsosDetails.m_detType[index]%10== 1 ? 2 : 1); // decrease counter
        rejectSharedHit(trackHitDetails, tsosDetails, index);
        continue;
      }

      //------------------------------------------//
      // if still shared and not rejected by above, set flags
      if( tsosDetails.m_detType[index] % 10 == 1 ) {
        if (firstMeasurement) {
          trackHitDetails.m_firstPixIsShared = true;
        }
        if (tsosDetails.m_detType[index] == 11 ){ // Blayer
          trackHitDetails.m_hasSharedIBLHit = true;
        } else if ( tsosDetails.m_detType[index] == 1 ){
          trackHitDetails.m_hasSharedPixel  = true; // historic definition
        }
      } // pixel hit

    } // if shared

    if (tsosDetails.m_detType[index] == 11 ){ // Blayer
      trackHitDetails.m_thisHasIBLHit = true;
    }
    if (firstMeasurement && tsosDetails.m_type[index] != OtherTsos){ firstMeasurement = false; }

  } // loop over TSOS
  //------------------------------------------------------------------//

  if (msgLvl(MSG::VERBOSE)){
    trackHitDetails.dumpInfo();
  }
  
  return;
} // decideWhichHitsToKeep 

//==========================================================================================

// GOAL: Do not kill conversions in the ambi
bool InDet::InDetDenseEnvAmbiTrackSelectionTool::performConversionCheck(const Trk::Track* ptrTrack,
    Trk::PRDtoTrackMap &prd_to_track_map,
    TrackHitDetails& trackHitDetails,
    TSoS_Details& tsosDetails,
    CacheEntry* ent) const
{
  ATH_MSG_DEBUG(" Conversion Check ");

  // We need to have a good number of unshared SCT hits
  if ( trackHitDetails.m_numSCT_Unused < m_minUniqueSCTHits )              { return false; }
  if ( trackHitDetails.m_numPixelHoles + trackHitDetails.m_numSCTHoles >= 2) { return false; }

  //Find the accepted track that shares the most hits our proposed track 
  const Trk::Track*  mostOverlappingTrack(0);
  int mostOverlappingNumberOfHits(0);
  int indexOfFirstOverlappingHit(0);
  for ( std::multimap<const Trk::Track*,int>::iterator it = tsosDetails.m_overlappingTracks.begin(), 
      end = tsosDetails.m_overlappingTracks.end(); it != end; 
      it = tsosDetails.m_overlappingTracks.upper_bound(it->first) ) {
    int numberOfHitsSharedWithThisTrack  = std::distance( it, tsosDetails.m_overlappingTracks.upper_bound(it->first));
    ATH_MSG_DEBUG(it->first <<" shares " << numberOfHitsSharedWithThisTrack << " hits with this track " );
    if (mostOverlappingNumberOfHits < numberOfHitsSharedWithThisTrack){
      mostOverlappingNumberOfHits = numberOfHitsSharedWithThisTrack;
      mostOverlappingTrack        = it->first;
      indexOfFirstOverlappingHit  = it->second;
    }
  } // loop over overlapping tracks

  if(!mostOverlappingTrack) { return false; }

  // criteria applied for fitted and pattern tracks
  if(mostOverlappingNumberOfHits < 2)                         { return false; }
  if(mostOverlappingNumberOfHits < trackHitDetails.m_numShared) { return false; }


  //If an overlapping track if found get the track parameters on the first shared surface
  auto tpPair = getOverlapTrackParameters(indexOfFirstOverlappingHit, ptrTrack, mostOverlappingTrack, prd_to_track_map, trackHitDetails.m_numSplitSharedPix );

  // If found track parameters at first overlapping track, check separation
  if (tpPair.first && tpPair.second) {
    // Check a series of criteria to see if track is a compatible with a photon conversion
    // Check if both tracks are above threshold
    if(tpPair.first->pT() <= m_minPairTrackPt || tpPair.second->pT() <= m_minPairTrackPt) {
      return false;
    }
    //Check if it is in a ROI, if requested
    if(m_useEmClusSeed) {
      if(!isEmCaloCompatible( *tpPair.first, ent )) { return false; }
    }
    ATH_MSG_DEBUG ("Possible photon conversion");
  }
  // for pattern tracks, cannot get the track parameters at a hit position
  // Need an alternate way to find conversion, try to use the accepted track
  // Main point is to not kill the pattern track
  else if ( trackHitDetails.m_isPatternTrack ) {
    if(m_useEmClusSeed && tpPair.second ) {
      if(!isEmCaloCompatible( *tpPair.second, ent )) { return false; }
    }
    ATH_MSG_DEBUG ("Possible photon conversion - for pattern track");
  }
  // if cannot find track parameters, and not a pattern track, then failed
  else {
    return false;
  }

  ATH_MSG_DEBUG ("Number of unused SCT hits:   " <<  trackHitDetails.m_numSCT_Unused);  
  if (msgLvl(MSG::DEBUG)){
    trackHitDetails.dumpInfo();
  }
  ATH_MSG_DEBUG ("Track "<< mostOverlappingTrack << " shares " << mostOverlappingNumberOfHits ); 

  updateSharedForCollimated(trackHitDetails, tsosDetails);

  return true;
} // performConversionCheck


// GOAL: Do not kill light particle decays in dense jets in the ambi.
// Based on conversion logic, but tuned towards later decays 
// Heavily borrows from performConversionCheck, but 
// separate implementation to for allow future fine-tuning
// distinct from the conversion version. 
bool InDet::InDetDenseEnvAmbiTrackSelectionTool::performHadDecayCheck(const Trk::Track* ptrTrack,
    Trk::PRDtoTrackMap &prd_to_track_map,
    TrackHitDetails& trackHitDetails,
    TSoS_Details& tsosDetails,
    CacheEntry* ent) const
{
  ATH_MSG_DEBUG(" Hadron decay Check ");

  // We need to have a good number of unshared SCT hits
  if ( trackHitDetails.m_numSCT_Unused < m_minUniqueSCTHits )              { return false; }
  if ( trackHitDetails.m_numPixelHoles + trackHitDetails.m_numSCTHoles >= 2) { return false; }

  //Find the accepted track that shares the most hits our proposed track 
  const Trk::Track*  mostOverlappingTrack(0);
  int mostOverlappingNumberOfHits(0);
  int indexOfFirstOverlappingHit(0);
  for ( std::multimap<const Trk::Track*,int>::iterator it = tsosDetails.m_overlappingTracks.begin(), 
      end = tsosDetails.m_overlappingTracks.end(); it != end; 
      it = tsosDetails.m_overlappingTracks.upper_bound(it->first) ) {
    int numberOfHitsSharedWithThisTrack  = std::distance( it, tsosDetails.m_overlappingTracks.upper_bound(it->first));
    ATH_MSG_DEBUG(it->first <<" shares " << numberOfHitsSharedWithThisTrack << " hits with this track " );
    if (mostOverlappingNumberOfHits < numberOfHitsSharedWithThisTrack){
      mostOverlappingNumberOfHits = numberOfHitsSharedWithThisTrack;
      mostOverlappingTrack        = it->first;
      indexOfFirstOverlappingHit  = it->second;
    }
  } // loop over overlapping tracks

  if(!mostOverlappingTrack) { return false; }

  // criteria applied for fitted and pattern tracks
  if(mostOverlappingNumberOfHits < 2)                         { return false; }
  if(mostOverlappingNumberOfHits < trackHitDetails.m_numShared) { return false; }


  //If an overlapping track if found get the track parameters on the first shared surface
  auto tpPair = getOverlapTrackParameters(indexOfFirstOverlappingHit, ptrTrack, mostOverlappingTrack, prd_to_track_map, trackHitDetails.m_numSplitSharedPix );

  // If found track parameters at first overlapping track, check separation
  if (tpPair.first && tpPair.second) {
    // Check a series of criteria to see if track is a compatible with a boosted decay
    // Check if both tracks are above threshold
    if(tpPair.first->pT() <= m_minPairTrackPt || tpPair.second->pT() <= m_minPairTrackPt) {
      return false;
    }
    //Check if it is in a ROI, if requested
    if(m_useHClusSeed) {
      if(!isHadCaloCompatible( *tpPair.first, ent )) { return false; }
    }
    ATH_MSG_DEBUG ("Possible boosted decay");
  }
  // for pattern tracks, cannot get the track parameters at a hit position
  else if ( trackHitDetails.m_isPatternTrack ) {
    if(m_useHClusSeed && tpPair.second ) {
      if(!isHadCaloCompatible( *tpPair.second, ent )) { return false; }
    }
    ATH_MSG_DEBUG ("Possible boosted decay - for pattern track");
  }
  // if cannot find track parameters, and not a pattern track, then failed
  else {
    return false;
  }

  ATH_MSG_DEBUG ("Number of unused SCT hits:   " <<  trackHitDetails.m_numSCT_Unused);  
  if (msgLvl(MSG::DEBUG)){
    trackHitDetails.dumpInfo();
  }
  ATH_MSG_DEBUG ("Track "<< mostOverlappingTrack << " shares " << mostOverlappingNumberOfHits ); 

  updateSharedForCollimated(trackHitDetails, tsosDetails);

  return true; 
}

void InDet::InDetDenseEnvAmbiTrackSelectionTool::updateSharedForCollimated(TrackHitDetails& trackHitDetails,
          TSoS_Details& tsosDetails) const{
  //Change all shared SCT to SplitSharedHit
  ATH_MSG_VERBOSE ("Updating SCT hit information");
  
  trackHitDetails.m_numSplitSharedPix = 0;          // reset counter
  trackHitDetails.m_numSplitSharedSCT = 0;          // reset counter
  trackHitDetails.m_numShared         = 0;          // reset counter 
  trackHitDetails.m_numSCT_Shared     = 0;          // reset counter 
  trackHitDetails.m_numWeightedShared = 0;          // reset counter

  trackHitDetails.m_hasSharedIBLHit = false;        // reset flag
  trackHitDetails.m_hasSharedPixel  = false;        // reset flag

  //Update counts but only allow 1 pixel hit to be updated free of charge 
  int noUpdatedPixels = 0;

  for (unsigned int index(0);  index  < tsosDetails.m_nTSoS; ++index ){    
    // Dont change blayer only pixel hits --  all other shared hits become splitshared
    if ( noUpdatedPixels < 1 && tsosDetails.m_detType[index]==1 && tsosDetails.m_type[index] == SharedHit ){
      ++noUpdatedPixels;
      tsosDetails.m_type[index] = SplitSharedHit;
    } 

    if ( tsosDetails.m_detType[index]==1 && tsosDetails.m_type[index] == SplitSharedHit ){
      ++trackHitDetails.m_numSplitSharedPix;
    } 

    if ( tsosDetails.m_detType[index]==2 && tsosDetails.m_type[index] == SharedHit ){
      tsosDetails.m_type[index] = SplitSharedHit;
    }

    if ( tsosDetails.m_detType[index]==2 && tsosDetails.m_type[index] == SplitSharedHit ){
      ++trackHitDetails.m_numSplitSharedSCT;
    }

    // only change shared hits
    // m_numUnused and m_numSCT_Unused are not changed from this
    if (tsosDetails.m_type[index] != SharedHit){ continue; }

    increaseSharedHitCounters( trackHitDetails, 
        (tsosDetails.m_detType[index]%10 == 1), 
        (tsosDetails.m_detType[index]%10 == 2) );

    // set flags
    if (tsosDetails.m_detType[index]  == 11){
      trackHitDetails.m_hasSharedIBLHit = true;
    } else if ( tsosDetails.m_detType[index] == 1 ){
      trackHitDetails.m_hasSharedPixel  = true;
    }
    
  } // loop over TSOS
} 

//==========================================================================================
Trk::Track* InDet::InDetDenseEnvAmbiTrackSelectionTool::createSubTrack( const std::vector<const Trk::TrackStateOnSurface*>& tsos, const Trk::Track* track ) const
{
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosit=tsos.begin();
  int nmeas=0;
  for (;tsosit!=tsos.end();++tsosit){
    if ((**tsosit).type(Trk::TrackStateOnSurface::Measurement)) ++nmeas;
  }
  if (nmeas<3) {
    ATH_MSG_DEBUG ("Less than 3 measurements, reject track !");
    return 0;
  }

  DataVector<const Trk::TrackStateOnSurface>* vecTsos = new DataVector<const Trk::TrackStateOnSurface>();

  // loop over TSOS, copy TSOS and push into vector
  for (const Trk::TrackStateOnSurface* iTsos : tsos) {
    const Trk::TrackStateOnSurface* newTsos = new Trk::TrackStateOnSurface(*iTsos);
    vecTsos->push_back(newTsos);
  }

  Trk::TrackInfo info;
  info.addPatternRecoAndProperties(track->info());
  Trk::TrackInfo newInfo;
  newInfo.setPatternRecognitionInfo(Trk::TrackInfo::InDetAmbiTrackSelectionTool);
  info.addPatternReco(newInfo);

  Trk::Track* newTrack = new Trk::Track(info, vecTsos,0);
  
  return newTrack;

}

//==========================================================================================
void InDet::InDetDenseEnvAmbiTrackSelectionTool::setPixelClusterSplitInformation(TSoS_Details& tsosDetails,
                                                                                 Trk::ClusterSplitProbabilityContainer &splitProbContainer) const
{

  for (unsigned int index(0);  index  < tsosDetails.m_nTSoS; ++index ){
    //Only consider split shared hits 
    if (tsosDetails.m_type[index] != SplitSharedHit)
      continue;

    // And the hit is a pixel hit
    if (tsosDetails.m_detType[index]%10 == 1){
      const InDet::PixelCluster* pixelCluster = dynamic_cast<const InDet::PixelCluster*> ( tsosDetails.m_RIO[index]->prepRawData()  );
      if (pixelCluster){
        Trk::ClusterSplitProbabilityContainer::ProbabilityInfo *splitProb = splitProbContainer.getSplitProbability(pixelCluster);
        if (!splitProb) {
           splitProb = &(splitProbContainer.setSplitInformation(pixelCluster,0.f,0.f));
        }
        splitProb->setSplit(true);
      } else {
        ATH_MSG_WARNING("Cast of a pixel cluster failed????");
      }
    }
  }
}


//==========================================================================================
bool InDet::InDetDenseEnvAmbiTrackSelectionTool::inHadronicROI(const Trk::Track* ptrTrack,
    CacheEntry* ent) const 
{

  if (  !ptrTrack->trackParameters()->front() ){ return false; }
  // above pT for ROI?
  if (  ptrTrack->trackParameters()->front()->pT() < m_minPtBjetROI ) { return false; }

  return isHadCaloCompatible(*ptrTrack->trackParameters()->front(), ent);
}
//==========================================================================================
bool InDet::InDetDenseEnvAmbiTrackSelectionTool::isHadCaloCompatible(const Trk::TrackParameters& Tp, CacheEntry* ent) const
{
  constexpr double pi = M_PI;
  constexpr double pi2 = 2.*M_PI;

  if (ent->m_hadF.empty()) return false;

  auto f = ent->m_hadF.begin(), fe = ent->m_hadF.end();
  auto e = ent->m_hadE.begin();
  auto r = ent->m_hadR.begin();
  auto z = ent->m_hadZ.begin();

  double F = Tp.momentum().phi();
  double E = Tp.eta();

  for (; f!=fe; ++f) {
    double df = std::fabs(F-(*f));
    if (df > pi        ) df = std::fabs(pi2-df);
    if (df < m_phiWidth) {
      //Correct eta of cluster to take into account the z postion of the track
      double newZ   = *z - Tp.position().z();
      double newEta =  std::atanh( newZ / std::sqrt( (*r) * (*r) + newZ*newZ ) );
 
      double de = std::fabs(E-newEta);
      if (de < m_etaWidth) return true;

    }
    ++e;
    ++r;
    ++z;
  }
  return false;
}


//==========================================================================================
bool InDet::InDetDenseEnvAmbiTrackSelectionTool::isEmCaloCompatible(const Trk::TrackParameters& Tp, CacheEntry* ent) const
{

  constexpr double pi = M_PI;
  constexpr double pi2 = 2.*M_PI;

  if (ent->m_emF.empty()) return false;

  auto f = ent->m_emF.begin(), fe = ent->m_emF.end();
  auto e = ent->m_emE.begin();
  auto r = ent->m_emR.begin();
  auto z = ent->m_emZ.begin();

  double F = Tp.momentum().phi();
  double E = Tp.momentum().eta();
  double R = Tp.position().perp();
  double Z = Tp.position().z();

  for (; f!=fe; ++f) {
    double df = std::fabs(F-(*f));
    if (df > pi        ) df = std::fabs(pi2-df);
    if (df < m_phiWidthEm) {
      //Correct eta of cluster to take into account the z postion of the track
      double newZ   = *z - Z;
      double newR   = *r - R;
      double newEta =  std::atanh( newZ / std::sqrt( newR*newR + newZ*newZ ) );
      double de = std::fabs(E-newEta);
       
      if (de < m_etaWidthEm) return true;
    }
    ++e;
    ++r;
    ++z;
  }
  return false;
}



std::pair<const Trk::TrackParameters*, const Trk::TrackParameters*> 
InDet::InDetDenseEnvAmbiTrackSelectionTool::getOverlapTrackParameters(int index, const Trk::Track* track1,
                                                                      const Trk::Track* track2,
                                                                      const Trk::PRDtoTrackMap &prd_to_track_map,
                                                                      int splitSharedPix ) const
{
  
  auto returnPair = std::make_pair<const Trk::TrackParameters*,const Trk::TrackParameters*>(0,0);

  // 
  // Get the TSOS in question from the candidate track since we know which one it is (index)
  // Do some basic checks
  //
  const DataVector<const Trk::TrackStateOnSurface>* track1tsos = track1->trackStateOnSurfaces();

  auto firstTsos    = track1tsos->begin();
  firstTsos += index;
  auto firstMeas = (*firstTsos)->measurementOnTrack();

  if (!firstMeas){
    ATH_MSG_ERROR("This is not a measurement!");
    return returnPair;
  }
  
  auto firstRot = dynamic_cast <const Trk::RIO_OnTrack*> (firstMeas);
  if (!firstRot) {
    ATH_MSG_DEBUG("This measurement is not a ROT");
    return returnPair;
  }

  if ( !prd_to_track_map.isUsed(*(firstRot->prepRawData()))){
    ATH_MSG_ERROR("This hist is not shared");
    return returnPair;
  }


  // 
  // now, get TSOS from 2nd track which is the one already accepted 
  // we know it was fitted so this should be possible
  const DataVector<const Trk::TrackStateOnSurface>* track2tsos = track2->trackStateOnSurfaces();

  auto iTsos    = track2tsos->begin(); 
  auto iTsosEnd = track2tsos->end();
  int measurementsBeforeShared = 0; 
  for (; iTsos != iTsosEnd ; ++iTsos) {
    
    // only compare to number of split hits? Why not shared also for NN inefficienicies
    if (measurementsBeforeShared > 1 + splitSharedPix ){
      ATH_MSG_DEBUG("Too many hits to before shared hit  -- unlikely they are from the same thing");
      return returnPair;  
    } 
     
    // get measurment from TSOS
    auto meas = (*iTsos)->measurementOnTrack();

    // if we do not have a measurement, we should just mark it
    if (!meas) {
      continue;
    }
    
    const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (meas);
    if (!rot) {
      continue;
    }
    
    if (rot->prepRawData() != firstRot->prepRawData()){
      ++measurementsBeforeShared;
      continue;
    }
    
    if (!(*iTsos)->trackParameters()){
      ATH_MSG_DEBUG("There are no TrackParameters on this TSOS");
      return returnPair;  
    }
    
    returnPair.second = (*iTsos)->trackParameters();
    ATH_MSG_DEBUG("Success! track parameters for accepted track " << returnPair.second );
    break;
  }


  //
  // Now let's see if we have TPs for the candidate track
  // 

  if (!(*firstTsos)->trackParameters()){
    ATH_MSG_DEBUG("There are no TrackParameters on this TSOS");
    return returnPair;  
  }

  returnPair.first = (*firstTsos)->trackParameters();

  
  ATH_MSG_DEBUG("Success! track parameters for both tracks " << returnPair.first<< " " << returnPair.second );
  
  return returnPair;
  
}


//============================================================================================
//
inline bool InDet::InDetDenseEnvAmbiTrackSelectionTool::clusCanBeSplit(float splitProb1, float splitProb2) const 
{
  return ( isTwoPartClus(splitProb1, splitProb2) || isMultiPartClus(splitProb2) );
}
inline bool InDet::InDetDenseEnvAmbiTrackSelectionTool::isTwoPartClus(float splitProb1, float splitProb2) const 
{
  return ( (splitProb1 >= m_sharedProbCut) && (splitProb2 < m_sharedProbCut2) );
}
inline bool InDet::InDetDenseEnvAmbiTrackSelectionTool::isMultiPartClus(float splitProb2) const 
{
  return ( splitProb2 >= m_sharedProbCut2 );
}
//============================================================================================
//
//============================================================================================
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::rejectHitOverUse(TrackHitDetails& trackHitDetails, 
    TSoS_Details& tsosDetails, int index) const {
  trackHitDetails.m_trkCouldBeAccepted = false; // we have to remove at least one PRD
  tsosDetails.m_type[index] = RejectedHitOverUse;
  return;
}
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::rejectHit(TrackHitDetails& trackHitDetails, 
    TSoS_Details& tsosDetails, int index) const {
  trackHitDetails.m_trkCouldBeAccepted = false; // we have to remove at least one PRD
  tsosDetails.m_type[index] = RejectedHit;
  return;
}
// used after counters have been set
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::rejectSharedHit(TrackHitDetails& trackHitDetails, 
    TSoS_Details& tsosDetails, int index) const {
  rejectHit(trackHitDetails, tsosDetails, index); // reject
  decreaseSharedHitCounters( trackHitDetails, 
      (tsosDetails.m_detType[index]%10 == 1), 
      (tsosDetails.m_detType[index]%10 == 2) );
  return;
}
// used after counters have been set
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::rejectSharedHitInvalid(TrackHitDetails& trackHitDetails, 
    TSoS_Details& tsosDetails, int index) const {
  trackHitDetails.m_trkCouldBeAccepted = false;
  tsosDetails.m_type[index] = RejectedHitInvalid; // do not use rejectHit function since use this flag
  decreaseSharedHitCounters( trackHitDetails, 
      (tsosDetails.m_detType[index]%10 == 1), 
      (tsosDetails.m_detType[index]%10 == 2) );
  return;
}
// used after counters have been set
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::sharedToSplitPix(TrackHitDetails& trackHitDetails, 
    TSoS_Details& tsosDetails, int index) const {
  // from shared
  decreaseSharedHitCounters( trackHitDetails, true, false ); // isPix=true
  // to split
  tsosDetails.m_type[index] = SplitSharedHit;
  trackHitDetails.m_numSplitSharedPix++;
  return;
}
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::addSharedHit(TrackHitDetails& trackHitDetails, 
    TSoS_Details& tsosDetails, int index) const {
  tsosDetails.m_type[index] = SharedHit;
  increaseSharedHitCounters( trackHitDetails, (tsosDetails.m_detType[index]%10 == 1), (tsosDetails.m_detType[index]%10 == 2) );
  return;
}
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::increaseSharedHitCounters(TrackHitDetails& trackHitDetails, bool isPix, bool isSCT) const {
  trackHitDetails.m_numShared++; // increase counter 
  trackHitDetails.m_numWeightedShared += (isPix ? 2 : 1); // increase counter
  // protect from TRT hits (needed?)
  if( isSCT ) { trackHitDetails.m_numSCT_Shared++; }
  return;
}
inline void InDet::InDetDenseEnvAmbiTrackSelectionTool::decreaseSharedHitCounters(TrackHitDetails& trackHitDetails, bool isPix, bool isSCT) const {
  trackHitDetails.m_numShared--; // decrease counter
  trackHitDetails.m_numWeightedShared -= (isPix ? 2 : 1); // increase counter
  // protect from TRT hits (needed?)
  if( isSCT ) { trackHitDetails.m_numSCT_Shared--; }
  return;
}
//============================================================================================
