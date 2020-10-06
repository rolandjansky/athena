/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetDenseEnvAmbiTrackSelectionTool.cxx, (c) ATLAS Detector software
// Anthony Morley based on the orginal tool by Markus Elsing
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

  ATH_MSG_INFO( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::InDetDenseEnvAmbiTrackSelectionTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
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
    for ( const auto& ccROI : *calo) {
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
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt!=ctx.evt()) { // New event in this slot
    // compute the number of shared hits from the number of max shared modules
    ent->m_maxShared = 2*m_maxSharedModules+1;
    ent->m_minNotShared = m_minNotSharedModules;
    ent->m_minSiHits = m_minSiHitsToAllowSplitting;

    //Fill ROI's
    newEvent(ent);

    ent->m_evt = ctx.evt();
  }
  
  ATH_MSG_DEBUG ("getcleanedouttrack just got called "); //WPM

  // compute the number of shared hits from the number of max shared modules
  ent->m_maxShared = 2*m_maxSharedModules+1;
  ent->m_minNotShared = m_minNotSharedModules;
  ent->m_minSiHits = m_minSiHitsToAllowSplitting;

  // cut on TRT hits, might use eta dependent cuts here
  int  nCutTRT = m_minTRT_Hits;
  if (m_parameterization) {
    const DataVector<const Trk::TrackParameters>* vpar = ptrTrack->trackParameters();
    const Trk::TrackParameters*                    par = (*vpar)[0];
    // ME bugfix
    int expected = m_selectortool->minNumberDCs(par);
    if (expected > nCutTRT) nCutTRT = expected; 
  }

  // get all TSOS the track
  const DataVector<const Trk::TrackStateOnSurface>* tsos = ptrTrack->trackStateOnSurfaces();
  ATH_MSG_DEBUG ("Study new Track "<< ptrTrack<<"\t , it has "<<tsos->size()<<"\t track states");
  
  //Create structs to hold track information
  TrackHitDetails trackHitDetails;

  ATH_MSG_DEBUG ("Filling in TrackDetails");
  TSoS_Details    tsosDetails(tsos->size());
  
  // Fill structs will information
  fillTrackDetails( ptrTrack, splitProbContainer, prd_to_track_map, trackHitDetails, tsosDetails);
  
  //Decide which hits to keep
  ATH_MSG_DEBUG ("DecideWhichHitsToKeep");
  bool TrkCouldBeAccepted = decideWhichHitsToKeep( ptrTrack,  score,  splitProbContainer, prd_to_track_map, trackHitDetails, tsosDetails, nCutTRT, ent );
  
  ATH_MSG_DEBUG ("DecidedWhichHitsToKeep " << TrkCouldBeAccepted );
    
  //WPM add in a loop to check how many splitable SCT clusters there are here!!!!
  ATH_MSG_DEBUG ("How many SCT clusters can be split? ");
  
  
  // counter for the weighted number of added shared hits 
  int merged_sct = 0;
  
  if (m_doSCTSplitting){
    
    // new TSOS vector
    std::vector<const Trk::TrackStateOnSurface*> newTSOS_sct;
  
    // loop over all TSOS (and types) and copy the good ones over
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos_sct    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd_sct = tsos->end(); 
  
    for (int index_sct = 0 ; iTsos_sct != iTsosEnd_sct ; ++iTsos_sct,++index_sct ) {

      const Trk::RIO_OnTrack*  rot_sct =  tsosDetails.RIO[index_sct];
    
      if (!rot_sct) {
        ATH_MSG_WARNING ("No ROTs failed, should never happen !");
        continue;
      }  
  
      if (m_detID->is_sct(rot_sct->identify())){  //WPM
      
        bool isSplitable = tsosDetails.splitProb1[index_sct] >= m_sharedProbCut || tsosDetails.splitProb2[index_sct] >= m_sharedProbCut2;
      
        if (isSplitable){
          merged_sct++;
          ATH_MSG_DEBUG ("+1 Merged SCT ");
  
        }
      }
    }
    
    ATH_MSG_DEBUG ("There are "<<merged_sct<<" merged SCT clusters.");
  }


  //
  // now see what to do with the track
  //
 
  int  totalSiHits = trackHitDetails.totalSiHits();
  ATH_MSG_DEBUG ("totalSiHits " << totalSiHits );
  ATH_MSG_DEBUG ("score " << score );


  // best case, we like this track and it passes this complex if statement
  if ( TrkCouldBeAccepted                          && // we did not mark the track as bad before
       ( !trackHitDetails.hassharedblayer || trackHitDetails.numPixelHoles<=1 )    && // if blayer, at most 1 pixel hole
       !trackHitDetails.hassharedpixel                                           && // no shared pixel hits
       ( ( totalSiHits >= m_minHits      && trackHitDetails.numShared == 0 )       || // no shared and enough hits OR
         ( totalSiHits >= ent->m_minNotShared && trackHitDetails.numWeightedShared < ent->m_maxShared && // shared hits and enough unique hits and shared hits with good quality
           ( totalSiHits + std::min(trackHitDetails.numShared, ent->m_maxShared) ) >= m_minHits && score > m_minScoreShareTracks ) ) ) {
    ATH_MSG_DEBUG ("=> Suggest to keep track with "<<trackHitDetails.numShared<<" shared hits !");


    //  Change pixel hits property for shared hits as this track will be accepted into the final track colection
    if (!trackHitDetails.isPatternTrack){
      setPixelClusterSplitInformation( tsosDetails, splitProbContainer );
    }

    return std::make_tuple(static_cast<Trk::Track *>(nullptr),true); // keep input track

    // ok, failed that one, can we recover the track ?
    /*
      } else if ( trackHitDetails.numTRT_Unused >= nCutTRT           && // TRT track or no TRT at all (back tracking)
      ( totalSiHits >= m_minHits                           || // we have enough hits OR
      ( totalSiHits >= ent->m_minNotShared                      && // shared hits and enough unique hits and shared hits with good quality
      totalSiHits+std::min(trackHitDetails.numShared, ent->m_maxShared) >= m_minHits && score > m_minScoreShareTracks ) ) ) {
    */
  }  else if ( trackHitDetails.numTRT_Unused >= nCutTRT           && // TRT track or no TRT at all (back tracking)
               (
                ( totalSiHits >= m_minHits                           || // we have enough hits OR
                  ( totalSiHits >= ent->m_minNotShared                      && // shared hits and enough unique hits and shared hits with good quality
                    totalSiHits+std::min(trackHitDetails.numShared, ent->m_maxShared) >= m_minHits && score > m_minScoreShareTracks ) ) //That is the traditional statement
                ||  //here comes WPM's addendum, which should allow tracks with many merged sct hits to pass:
                ( m_doSCTSplitting && totalSiHits+merged_sct >= m_minHits && score > m_minScoreShareTracks )
                )
               ) {
    //WPM
    ATH_MSG_DEBUG ("Passed track hits if? "); //WPM
    // catch, if this is cosmics, accept the incoming track
    if (m_cosmics) {
      ATH_MSG_DEBUG ("=> Cosmics, accept input track even with shared hits");
      return std::make_tuple(static_cast<Trk::Track *>(nullptr),true); // keep input track
    }

    // Track is potentially ok, create a stripped down version from the unused hits and the allowed shared hits
    // join shared to unused hits, if requirements are met

    ATH_MSG_VERBOSE ("Trying to recover track, allow for some shared hits is possible.");
      
    // new TSOS vector
    std::vector<const Trk::TrackStateOnSurface*> newTSOS;
      
    // counter for the weighted number of added shared hits 
    int cntIns = 0;
    
    // loop over all TSOS (and types) and copy the good ones over
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
      
    for (int index = 0 ; iTsos != iTsosEnd ; ++iTsos,++index ) {
      
      // remove rejected hits
      if (tsosDetails.type[index] == RejectedHit) {
        ATH_MSG_VERBOSE ("-> Dropping rejected hit");
      } else if (tsosDetails.type[index] != SharedHit ) {
        ATH_MSG_VERBOSE ("-> Copy good TSOS");
        newTSOS.push_back(*iTsos);
      } else if (cntIns + 1 >= ent->m_maxShared) {
        ATH_MSG_VERBOSE ("-> Too many share hits, dropping outer hit(s)");  
      } else {
        ATH_MSG_VERBOSE ("-> Try to recover a shared hit");

        // get ROT from this TSOS
        const Trk::RIO_OnTrack*  rot =  tsosDetails.RIO[index];
  
        if (!rot) {
          ATH_MSG_WARNING ("No ROTs failed, should never happen !");
          continue;
        }

        // is it a pixel cluster ?
        bool isPixel = m_detID->is_pixel(rot->identify());

        int  maxiShared        = -1;
        int  maxothernpixel    = 0;
        bool maxotherhasblayer = false;
        bool otherfailsMinUniqueHits = false; 
        
        bool isSplitable = tsosDetails.splitProb1[index] >= m_sharedProbCut || tsosDetails.splitProb2[index] >= m_sharedProbCut2;
        
        int  numberOfTracksWithThisPrd = checkOtherTracksValidity( rot, isSplitable, splitProbContainer, prd_to_track_map, maxiShared, maxothernpixel, maxotherhasblayer, otherfailsMinUniqueHits, ent);


        // now decide what to do, can we keep the shared hit
        /*
          if ( numberOfTracksWithThisPrd < m_maxTracksPerPRD            && // number of tracks sharing hit 
          score > m_minScoreShareTracks                            && // score cut
          (!isPixel || !trackHitDetails.hassharedblayer || trackHitDetails.numPixelHoles <= 0)         && // shared b-layer only if no pixel holes
          !otherfailsMinUniqueHits                                 && //It will not invalidate another track
          ( maxiShared < ent->m_maxShared || (isPixel && !trackHitDetails.firstisshared) ) && // do not allow other accepted track to exceed the shared limit, if first pixel hit is shared
          (!isPixel || trackHitDetails.thishasblayer == maxotherhasblayer )           && // only allow shared pixel if both have blayer or both not
          (!isPixel || trackHitDetails.numPixelHits >= maxothernpixel )                    ) { // only allow shared pixel if new track as at least as many pixel hits  
        */
        if ( (  numberOfTracksWithThisPrd < m_maxTracksPerPRD            && // number of tracks sharing hit 
                score > m_minScoreShareTracks                            && // score cut
                (!isPixel || !trackHitDetails.hassharedblayer || trackHitDetails.numPixelHoles <= 0)         && // shared b-layer only if no pixel holes
                !otherfailsMinUniqueHits                                 && //It will not invalidate another track
                ( maxiShared < ent->m_maxShared || (isPixel && !trackHitDetails.firstisshared) ) && // do not allow other accepted track to exceed the shared limit, if first pixel hit is shared
                (!isPixel || trackHitDetails.thishasblayer == maxotherhasblayer )           && // only allow shared pixel if both have blayer or both not
                (!isPixel || trackHitDetails.numPixelHits >= maxothernpixel )  )  ||  ( m_doSCTSplitting && m_detID->is_sct(rot->identify()) && isSplitable )                  ) { //WPM only allow shared pixel if new track as at least as many pixel hits 

          ATH_MSG_VERBOSE ("---> Accepted hit shared with " << numberOfTracksWithThisPrd << " tracks !");

          newTSOS.push_back(*iTsos);
          ++trackHitDetails.numUnused; // update counter 
      
          // update shared hit counter
          //cntIns += isPixel ? 2 : 1;
          if (isPixel) {cntIns += 2;}
          else if (  m_detID->is_sct(rot->identify()) && isSplitable ){ cntIns += 0;}
          else{cntIns += 1;}


        } else {
          ATH_MSG_VERBOSE ("---> Reject hit shared with " << numberOfTracksWithThisPrd << " tracks !");
        }
      }
    }

    // this still may happen per (**) see above.
    //if ( ( trackHitDetails.numUnused+trackHitDetails.numPixelDeadSensor+trackHitDetails.numSCTDeadSensor+trackHitDetails.numSplitSharedPix  < m_minHits || newTSOS.size() <= 3 ) || ( m_doSCTSplitting && trackHitDetails.numUnused+trackHitDetails.numPixelDeadSensor+trackHitDetails.numSCTDeadSensor+trackHitDetails.numSplitSharedPix+merged_sct  < m_minHits || newTSOS.size() <= 3 ) ) {
    if ( trackHitDetails.numUnused+trackHitDetails.numPixelDeadSensor+trackHitDetails.numSCTDeadSensor+trackHitDetails.numSplitSharedPix+merged_sct  < m_minHits || newTSOS.size() <= 3 ) {
      //WPM another problematic if statment
      ATH_MSG_DEBUG ("=> Too few hits, reject track with shared hits");
      ATH_MSG_VERBOSE ("newTSOS.size(): "<<newTSOS.size() );
      ATH_MSG_DEBUG ("reject track; Too few hits, reject track with shared hits");
      return std::make_tuple(static_cast<Trk::Track *>(nullptr),false); // reject input track
    }

    // check that this is not the input track
    if ( newTSOS.size() == tsos->size() ) {
      ATH_MSG_DEBUG ("=> Recovered input track, accept it !");

      //  Change pixel hits property for shared hits as this is track will be accepeted into the final track colection
      if (!trackHitDetails.isPatternTrack){
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
  } else {
    ATH_MSG_DEBUG ("=> Track is recommended to be dropped");
    ATH_MSG_DEBUG ("reject track; other");
  }
  return std::make_tuple(static_cast<Trk::Track *>(nullptr),false); // reject input track
}

int  InDet::InDetDenseEnvAmbiTrackSelectionTool::checkOtherTracksValidity(const Trk::RIO_OnTrack* rot,
                                                                          const bool isSplitable,
                                                                          Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                                          Trk::PRDtoTrackMap &prd_to_track_map,
                                                                          int& maxiShared, 
                                                                          int& maxothernpixel, 
                                                                          bool& maxotherhasblayer, 
                                                                          bool& failMinHits,
                                                                          CacheEntry* ent) const
{

  // find out how many tracks use this hit already
  Trk::PRDtoTrackMap::ConstPrepRawDataTrackMapRange range = prd_to_track_map.onTracks(*(rot->prepRawData()));
  int                             numberOfTracksWithThisPrd = std::distance(range.first,range.second);
  ATH_MSG_VERBOSE ("---> number of tracks with this shared Prd: " << numberOfTracksWithThisPrd << " maxtracks: " << m_maxTracksPerPRD);

  // check if this newly shared hit would exceed the shared hits limit of the already accepted track (**) 

  maxiShared = -1;
  bool otherwillFailMinHits = false;
  bool otherwillFailSCTuniqueHits = false;

  if ( numberOfTracksWithThisPrd > 0 ) {
    //Determine of the hit we are checking is a SCT or Pixel hit
    bool isSCT = m_detID->is_sct( rot->prepRawData()->identify() );
    bool isPix = m_detID->is_pixel( rot->prepRawData()->identify() );

    //Setup counters
    int  iShared        = 0;  //Number of Shared hits on the test track if this new hit is going to be shared
    int  iNotShared     = 0;  //Number of unshared hits on the test track
    int  iSctUnique     = 0;  //Number of unique SCT hits on the test track 
    int  iPixel         = 0;  //Number of pixel hits on the test track
    int  iSCT           = 0;  //Number of SCT hits on the test track
    bool iHasBlayer     = false; // Does the test track have a b-layer hit
    
    
    // Use the track with most shared hits as the one to compare to --  
    // This will probably be the worst track compete
    for ( auto track = range.first; track != range.second; ++track ){
      //Count up the hit properties of the track you are inspecting 
      iShared        = isSplitable ? 0 : isPix ?  2 : 1; //You share this new hit now so.
      iNotShared     = isSplitable ? 0 : -1;             //You will now be sharing this hit so start off at -1
      iSctUnique     = isSCT ? -1 : 0;                   //Reduce the number unique SCT hits by one if PRD in question is shared
      iPixel         = 0;
      iHasBlayer     = false;
      iSCT           = 0; 
      
      auto prdsToCheck = m_assoTool->getPrdsOnTrack(prd_to_track_map, *(track->second));
      for ( const auto&  prdToCheck: prdsToCheck) {
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
            
        if (prd_to_track_map.isShared( *prdToCheck )){
          iShared += isSplitPixel ? 0 : isPixel ? 2 : 1;
        } else {
          ++iNotShared;
          if (m_detID->is_sct(prdToCheck->identify())){
            ++iSctUnique;
          }
        }
      }
    
      ATH_MSG_VERBOSE( "Track " <<  track->second << " will has " << iNotShared << " unique hits and  " <<  iSctUnique << " unique SCT hits.");
      // You are sharing too many hits reject the new track
      if ( iNotShared  < ent->m_minNotShared ) otherwillFailMinHits = true;
      // You are sharing too many shared SCT hits and you don't have enough to share -reject hit.
      if ( iSCT <= m_minUniqueSCTHits && iSCT > iSctUnique   ) otherwillFailSCTuniqueHits = true;    
    }
    
    // Update details for the track with the most shared hits          
    if ( iShared > maxiShared){
      maxiShared        = iShared;
      maxothernpixel    = iPixel;
      maxotherhasblayer = iHasBlayer;
    }
  }
   
   
  failMinHits = otherwillFailMinHits || otherwillFailSCTuniqueHits;      
  
  return numberOfTracksWithThisPrd;

}



void InDet::InDetDenseEnvAmbiTrackSelectionTool::fillTrackDetails(const Trk::Track* ptrTrack,
                                                                  Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                                  const Trk::PRDtoTrackMap &prd_to_track_map,
                                                                  TrackHitDetails& trackHitDetails, 
                                                                  TSoS_Details& tsosDetails) const
{ 

  ATH_MSG_DEBUG ("filltrackdetails just got called "); //WPM

  // get all TSOS the track
  const DataVector<const Trk::TrackStateOnSurface>* tsos = ptrTrack->trackStateOnSurfaces();
  ATH_MSG_DEBUG ("Study new Track "<< ptrTrack<<"\t , it has "<<tsos->size()<<"\t track states");


  // is this a track from the pattern or a fitted track ?
  trackHitDetails.isPatternTrack = (ptrTrack->info().trackFitter()==Trk::TrackInfo::Unknown);
  if (trackHitDetails.isPatternTrack) {
    ATH_MSG_DEBUG ("-> this is a pattern track, outliers are good hits (reintegration) ! " << ptrTrack->perigeeParameters() );
  } else {
    ATH_MSG_DEBUG ("-> this is a refitted track, so we can use the chi2 ! " << ptrTrack->perigeeParameters());
  }

  // some pre-processing of the summary information, if available, needed for special cuts
  const Trk::TrackSummary* trkSummary=ptrTrack->trackSummary();
  if (trkSummary) {
    ATH_MSG_VERBOSE ("--> Found summary information");
    trackHitDetails.numPixelDeadSensor = trkSummary->get(Trk::numberOfPixelDeadSensors);
    trackHitDetails.numSCTDeadSensor   = trkSummary->get(Trk::numberOfSCTDeadSensors);
    trackHitDetails.numPixelHits       = trkSummary->get(Trk::numberOfPixelHits);
    trackHitDetails.numPixelHoles      = trkSummary->get(Trk::numberOfPixelHoles);
    trackHitDetails.numSCTHoles        = trkSummary->get(Trk::numberOfSCTHoles);
    trackHitDetails.numSCTHits         = trkSummary->get(Trk::numberOfSCTHits);
  }


  // set nDeadSensors to 0 in case trkSummary wasn't called with HoleSearch
  // (i.e. number of deadSensors not available)
  if (trackHitDetails.numPixelDeadSensor == -1) trackHitDetails.numPixelDeadSensor = 0;
  if (trackHitDetails.numSCTDeadSensor   == -1) trackHitDetails.numSCTDeadSensor   = 0;
  ATH_MSG_VERBOSE ("--- Number of dead si sensors: " << trackHitDetails.numPixelDeadSensor + trackHitDetails.numSCTDeadSensor);


  // loop over TSOS
  ATH_MSG_VERBOSE ("--> Looping over TSOS's");
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();
  //int numissplit = 0; //WPM
  for (int index = 0 ; iTsos != iTsosEnd ; ++iTsos, ++index) {

    // get measurment from TSOS
    const Trk::MeasurementBase* meas = (*iTsos)->measurementOnTrack();

    // if we do not have a measurement, we should just mark it
    if (!meas) {
      ATH_MSG_VERBOSE ( Form("---%3d No measurement on TSOS, it is another type, to be copied over.",index) );
      tsosDetails.type[index] = OtherTsos;
      continue;
    }

    // ok, let try to get the ROT then
    const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (meas);
    if (!rot) {
      // could be a Pseudo-Measurement ?
      const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast <const Trk::PseudoMeasurementOnTrack*> (meas);
      if (pseudo){
        ATH_MSG_VERBOSE ( Form("---%3d Copy pseudo measurement.",index) );
        ++trackHitDetails.numPseudo; // increase counter
      } else {
        ATH_MSG_WARNING ( Form("---%3d Measurement is not a pseudo measurment, not yet supported, try to copy !",index));
      }
      tsosDetails.type[index] = OtherTsos;
      continue;
    }
    
    //Store RIO into vector for later use
    tsosDetails.RIO[index] = rot;

    //
    // we have a TSOS with a measurement, keep analysing it
    //

    // let's get some information about the measurement
    const Identifier& id = rot->identify();
    bool isTRT           = m_detID->is_trt(id);
    bool isPixel         = m_detID->is_pixel(id);
    bool isSCT           = m_detID->is_sct(id);
    bool isBlayer        = isPixel ? m_detID->is_blayer(id) : false;
    bool isoutlier       = (*iTsos)->type(Trk::TrackStateOnSurface::Outlier);              

    tsosDetails.detType[index] = isTRT * 3 + isSCT * 2 + isPixel * 1 + isBlayer * 10;


    // Add splitting information about the cluster
    if (isPixel) {
      // get pixel cluster
      const InDet::PixelCluster* clus = dynamic_cast <const InDet::PixelCluster*> (rot->prepRawData());
      if ( !clus ) {
        ATH_MSG_WARNING ("---> Cast to Pixel cluster failed, should not happen !");
        tsosDetails.type[index]    = RejectedHit;
        continue; 
      } else {
        const Trk::ClusterSplitProbabilityContainer::ProbabilityInfo &splitProb = splitProbContainer.splitProbability(clus);
        if ( !splitProb.isTooBigToBeSplit() )  {
          tsosDetails.splitProb1[index] = splitProb.splitProbability1();
          tsosDetails.splitProb2[index] = splitProb.splitProbability2();
        } else {
          tsosDetails.splitProb1[index] = 0.51;
          tsosDetails.splitProb2[index] = 0.51;
        }
      }
    }


    if (isSCT) {
      if (m_doSCTSplitting){
        ATH_MSG_DEBUG ("Entered doSCTSplitting.  Getting cluster parameters.");
        const InDet::SCT_Cluster* clussct = dynamic_cast <const InDet::SCT_Cluster*> (rot->prepRawData());
        if (!clussct){
          ATH_MSG_WARNING ("---> Cast to SCT cluster failed, should not happen !");
          tsosDetails.type[index]    = RejectedHit;
          continue;
        }
        if ( std::abs((*iTsos)->surface().normal()(2)) < 0.1 ){
          float px = (*iTsos)->trackParameters()->momentum()(0);
          float py = (*iTsos)->trackParameters()->momentum()(1);
          float nx = (*iTsos)->surface().normal()(0);
          float ny = (*iTsos)->surface().normal()(1);

          //lphi will be the momentum's angle in the x-y plane minus the surface's normal angle
          double lphi = std::atan2( py, px ) - std::atan2( ny, nx );

          double diff = ((285*std::abs( std::tan(lphi)-std::tan(-0.07)) ) -  80*clussct->width().colRow()(0));
          ATH_MSG_DEBUG ("Calculated width - observed width? "<< diff );

          //track pT would be sqrt( ( px * px ) + ( py * py ) )
          //isStereo?: (*iTsos)->surface().isStereo()
          //Number of split pixel clusters: numissplit <- would have to uncomment stuff out from above

          if (std::abs(diff) > 80){
            tsosDetails.splitProb1[index] = .99;
            tsosDetails.splitProb2[index] = .99;
          } else {
            tsosDetails.splitProb1[index] = 0;
            tsosDetails.splitProb2[index] = 0;
          }
          //This ends the "isBarrel" if statement; no endcap clusters are split
        } else {
          tsosDetails.splitProb1[index] = 0;
          tsosDetails.splitProb2[index] = 0;
        }
      } else { //Always set to 0 if splitting not allowed
        tsosDetails.splitProb1[index] = 0;
        tsosDetails.splitProb2[index] = 0;
      }
    } 

    bool isUsed = prd_to_track_map.isUsed(*(rot->prepRawData()));

    // Do we have an outlier (and not a pattern track) ?
    if ( (isoutlier && !isUsed && !trackHitDetails.isPatternTrack) || !(m_detID->is_indet(id)) ) {
      ATH_MSG_VERBOSE ( Form("---%3d Prd is outlier on a fitter track (or not InDet), copy it over.", index) );
      tsosDetails.type[index] = Outlier;
      continue;
    }

    // let's check if this is a shared hit (even if it is an outlier on a pattern track) ?
    if (!isUsed) {
      if ( !isoutlier ) {
        ATH_MSG_VERBOSE ( Form("---%3d Prd is unused, copy it over", index) );
      } else {
        ATH_MSG_VERBOSE ( Form("---%3d Prd is outlier on a pattern track and is unused, copy it over", index) );
      }
      tsosDetails.type[index] = UnusedHit;
      continue;
    }

    //
    // ok, we have a shared hit
    //

    // do we have an outlier and a pattern track, but the hit is shared, so reject it (we reintegrate it otherwise)
    if ( isoutlier && trackHitDetails.isPatternTrack ) {
      ATH_MSG_VERBOSE ( Form("---%3d Prd is outlier on a pattern track, we do not want to reintegrate it, so reject it.", index) );
      tsosDetails.type[index]    = RejectedHit;
      continue;
    }

    // first check if it is a shared TRT hit, this is not allowed
    if (isTRT) {
      ATH_MSG_VERBOSE (  Form("---%3d Share TRT hit, drop it !", index) );
      tsosDetails.type[index]    = RejectedHit;
      continue;
    }

    /* Allow a hit to be a shared one, if
       - not too many tracks share this hit already
       - the score of the track is high enough to allow for shared hits
    */
    Trk::PRDtoTrackMap::ConstPrepRawDataTrackMapRange range = prd_to_track_map.onTracks(*(rot->prepRawData()));
    int numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    ATH_MSG_VERBOSE ( Form("---%3d Number of tracks with this share Prd: %2d maxtracks: %2d",index, numberOfTracksWithThisPrd, m_maxTracksPerPRD.value()) );
    tsosDetails.hitIsShared[index] = numberOfTracksWithThisPrd;


    // get iterators for range
    Trk::PRDtoTrackMap::PrepRawDataTrackMap::const_iterator  mapIt    = range.first;
    Trk::PRDtoTrackMap::PrepRawDataTrackMap::const_iterator mapItEnd = range.second;
    // simple for loop instead of fancier remove_if above
    for ( ;mapIt!=mapItEnd; ++mapIt) {
      tsosDetails.overlappingTracks.insert( std::pair<const Trk::Track*, int >(mapIt->second, index) );
      tsosDetails.tracksSharingHit.insert(std::pair< int, const Trk::Track* >(index, mapIt->second ) ); 
    }
 
    ATH_MSG_VERBOSE ("-----> Mark this hits as shared  -- Try and recover later!");
    ATH_MSG_VERBOSE ("------ Index: "<< index << " Type: " << tsosDetails.detType[index]  << " splitprob1  " << tsosDetails.splitProb1[index]);
    tsosDetails.type[index]    = SharedHit;
  } // End loop over TSOS's
  
  return;
}

bool InDet::InDetDenseEnvAmbiTrackSelectionTool::decideWhichHitsToKeep(const Trk::Track* ptrTrack,
                                                                       const Trk::TrackScore score,
                                                                       Trk::ClusterSplitProbabilityContainer &splitProbContainer,
                                                                       Trk::PRDtoTrackMap &prd_to_track_map,
                                                                       TrackHitDetails& trackHitDetails,
                                                                       TSoS_Details& tsosDetails,
                                                                       int nCutTRT,
                                                                       CacheEntry* ent)const
{

  // Can the track can automatically be accpeted without further checks
  bool TrkCouldBeAccepted(true);

  // let's remember the last 2 ROTs on the track
  const Trk::RIO_OnTrack* lastrot       = 0;
  const Trk::RIO_OnTrack* lastbutonerot = 0;  
  int                     lastrotindex  = 0;


  // Are we in a ROI?
  bool inROIandPTok(true);
  bool inROI(true);
  if (  ptrTrack->trackParameters()->front() ){
    if (  ptrTrack->trackParameters()->front()->pT() < m_minPtSplit ) {
      inROIandPTok = false;
    }
    
    if (  ptrTrack->trackParameters()->front()->pT() < m_minPtBjetROI ) {
      inROI = false;
    }

    if (inROI){ 
      inROI  = m_useHClusSeed && isHadCaloCompatible(*ptrTrack->trackParameters()->front(), ent);
    
      // If we are in a ROI change the shared hit cut;
      if (inROI){
        ent->m_maxShared = 2*m_maxSharedModulesInROI+1;
        ent->m_minNotShared = m_minNotSharedModulesInROI;
        ent->m_minSiHits = m_minSiHitsToAllowSplittingInROI;
      }
    }
  } 
  
  
  
  // Try and recover shared hits on this track if.. 
  bool recoverSharedHits = inROIandPTok &&                           // Track is in  ROI and has enough PT       
    score > m_minScoreShareTracks &&          // score needs to be good
    trackHitDetails.numPixelHits+trackHitDetails.numSCTHits >= m_minSiHitsToAllowSplitting &&
    trackHitDetails.numPixelHoles<=1 ;        // we do not share pixels if there are too many  holes in pixels


  // loop over all measurements
  for (unsigned int index(0);  index  < tsosDetails.nTSoS; ++index ){
    //Skip all non-measurement TSOS's 
    if (tsosDetails.type[index] == OtherTsos) continue;
    
    
    // Do we have some b-layer hit in the track?
    if (tsosDetails.detType[index] == 11 && (!(tsosDetails.type[index]== Outlier) || trackHitDetails.isPatternTrack)) {
      trackHitDetails.thishasblayer = true;  // we may reintegrate outliers from pattern
    }

    // Keep all unused hits
    if (tsosDetails.type[index] == UnusedHit ){
      if (tsosDetails.detType[index] == 3) {
        ++trackHitDetails.numTRT_Unused;
      } else {
        ++trackHitDetails.numUnused;
      }
      
      // remember the last  2 ROTs
      lastbutonerot = lastrot;
      lastrot       = tsosDetails.RIO[index];
      lastrotindex  = index;    
      continue;
    }

    // special cut to remove problematic combinations in the pixels
    // So far left this in need to check if it make a difference    
    if ( tsosDetails.detType[index]%10 == 1 && !trackHitDetails.thishasblayer && trackHitDetails.numPixelHoles>0               // a pixel hit, no b-layer, we do have pixel holes
         && ( ( trackHitDetails.numPixelHits==1 && !(tsosDetails.type[index]==Outlier) )                                         // one pixel on track, it is not an additional outlier     
              || ( trackHitDetails.isPatternTrack && trackHitDetails.numPixelHits==0 && (tsosDetails.type[index]==Outlier) ) )   ) {  // pattern track, no pixels, but an outlier (we may reintegrate it)
      ATH_MSG_VERBOSE ("-> Special case, problematic single pixel hit on track, reject it !");
      tsosDetails.type[index]    = RejectedHit;
      // Mark track as bad !
      TrkCouldBeAccepted = false;
      ATH_MSG_DEBUG ("reject track; Special case, problematic single pixel hit on track, reject it !");
      continue;             
    }

    // Try and recover  shared hits if.. 
    if ( recoverSharedHits ) { 

      //Find the index of the previous measurement on track
      int indexPreviousMeasurement = tsosDetails.findIndexOfPreviousMeasurement(index);
      if (indexPreviousMeasurement > 0 ) {
        ATH_MSG_VERBOSE ("--> Previous Measurement was at : " << indexPreviousMeasurement 
                         << " and was a: "  << tsosDetails.type[indexPreviousMeasurement] 
                         << " with splitprob1 " << tsosDetails.splitProb1[indexPreviousMeasurement] 
                         << " and splitprob2 "  << tsosDetails.splitProb2[indexPreviousMeasurement]);
      }
      // Special treatment of share split pixel clusters...
      if (m_doPixelClusterSplitting && tsosDetails.detType[index]%10 == 1) {
        
        //  If there is an earlier hit on the track and it is a pixel hit check to see if it's shared or split.
        //  This effectively means that singular shared hits in the pixel detector are only allowed in the first layer.
        //  Otherwise  we make the assumption that a shared in nth layer means that 
        //  there should be a shared in n-1 th layer. i.e. collimated tracks 
        //  This breaks down in overlap regions hence it is disabled
        if ( false &&  indexPreviousMeasurement > 0 &&  
             !( tsosDetails.type[indexPreviousMeasurement]  == SplitSharedHit || tsosDetails.type[indexPreviousMeasurement]  == SharedHit ||
                tsosDetails.splitProb1[indexPreviousMeasurement] >= m_sharedProbCut || tsosDetails.splitProb2[indexPreviousMeasurement] >= m_sharedProbCut2) ) {
          ATH_MSG_VERBOSE ("--->  Previous hit was not shared or compatible with a split hit!!!");
          TrkCouldBeAccepted = false; // we have to remove at least one PRD
          tsosDetails.type[index]    = RejectedHit;            
          continue;
        }


        // If the previous measurement is a shared pixel and the current pixel wants to be shared 
        // and is compatible with being so  allow the previous measurement to be shared with up to m_maxTracksPerPRD tracks without penalty 
        if ( indexPreviousMeasurement >= 0 && 
             tsosDetails.type[indexPreviousMeasurement] == SharedHit &&
             tsosDetails.hitIsShared[indexPreviousMeasurement] < m_maxTracksPerPRD 
             && (tsosDetails.splitProb1[index] >= m_sharedProbCut || tsosDetails.splitProb2[index] >= m_sharedProbCut2)){
          
          // Check if the previous shared hit shared on the same track as this one 
          bool sharedbetweenthesametracks = false;
          auto previousHitsSharedTracks = tsosDetails.tracksSharingHit.equal_range(indexPreviousMeasurement);
          auto currentHitsSharedTracks = tsosDetails.tracksSharingHit.equal_range(index);
          for (auto iteratorP = previousHitsSharedTracks.first; iteratorP != previousHitsSharedTracks.second; ++iteratorP) {
            for (auto iteratorC = currentHitsSharedTracks.first; iteratorC != currentHitsSharedTracks.second; ++iteratorC) {
              if ( iteratorC->second == iteratorP->second){
                sharedbetweenthesametracks = true;
                break;                
              }
            }
            if (sharedbetweenthesametracks) break;
          }
          if (sharedbetweenthesametracks){
            tsosDetails.type[indexPreviousMeasurement]   = SplitSharedHit;
            trackHitDetails.numShared--;                             // decrease counter 
            trackHitDetails.numWeightedShared -= 2;                  // decrease counter
            trackHitDetails.numSplitSharedPix++;
          }          
        }

        //  If cluster can only be shared by two tracks
        if ( tsosDetails.splitProb1[index] >= m_sharedProbCut && tsosDetails.splitProb2[index] < m_sharedProbCut2){
          if (tsosDetails.hitIsShared[index] >= 2){
            ATH_MSG_VERBOSE ("---> Pixel cluster is split, and sheared  between too many tracks  -- hit will be removed from the track!!!");
            TrkCouldBeAccepted = false;        // Flag that we need to remove the at least one hit
            tsosDetails.type[index] = RejectedHit;
            continue;
          } else {
            ATH_MSG_VERBOSE ("---> Pixel cluster is to be split shared with another track: Split Prob1 " << tsosDetails.splitProb1[index] );
            // Check if this first hit --  if it is not check if the pervious hit has been shared 
            // For now just print some debug output
            if (  msg().level() <=MSG::VERBOSE  && indexPreviousMeasurement>0){
              if (tsosDetails.type[indexPreviousMeasurement] == SplitSharedHit){
                ATH_MSG_VERBOSE("----- Previous Hit ("<< indexPreviousMeasurement <<") Shared and Split");
              } else if (tsosDetails.type[indexPreviousMeasurement] == SharedHit) {
                ATH_MSG_VERBOSE("----- Previous Hit ("<< indexPreviousMeasurement<<") Shared");
              } else if (tsosDetails.type[indexPreviousMeasurement] == UnusedHit) {
                ATH_MSG_VERBOSE("----- Previous Hit ("<< indexPreviousMeasurement <<") Unused hit");        
              } else if (tsosDetails.type[indexPreviousMeasurement] == Outlier) {
                ATH_MSG_VERBOSE("----- Previous Hit ("<< indexPreviousMeasurement <<") Outlier");       
              } 
            }
            tsosDetails.type[index] = SplitSharedHit;
            ++trackHitDetails.numSplitSharedPix;
            // remember the last  2 ROTs
            lastbutonerot = lastrot;
            lastrot       = tsosDetails.RIO[index];
            lastrotindex  = index;   
            continue;              
          }
          //  If cluster can shared by more than 2 tracks   
        }  else if ( tsosDetails.splitProb2[index] >= m_sharedProbCut2){
          if (tsosDetails.hitIsShared[index] >= m_maxPixMultiCluster ){
            ATH_MSG_VERBOSE ("---> Pixel cluster is split, and sheared  between too many tracks -- hit will be removed from the track!!!");
            TrkCouldBeAccepted = false;      // Flag that we need to remove the at least one hit
            tsosDetails.type[index]    = RejectedHit;
            continue;
          } else {
            ATH_MSG_VERBOSE ("---> Pixel cluster is to be split shared with another track: Split Prob2 " <<  tsosDetails.splitProb2[index] );
            // Check if this is the first hit --  if it is not check if the pervious hit has been shared 
            // For now just print some debug output
            if (msg().level() <=MSG::VERBOSE &&  indexPreviousMeasurement>0){
              if (tsosDetails.type[indexPreviousMeasurement] == SplitSharedHit){
                ATH_MSG_VERBOSE("---- Previous Hit ("<< indexPreviousMeasurement <<") Shared and Split");
              } else if (tsosDetails.type[indexPreviousMeasurement] == SharedHit) {
                ATH_MSG_VERBOSE("---- Previous Hit ("<< indexPreviousMeasurement <<") Shared");
              } else if (tsosDetails.type[indexPreviousMeasurement] == UnusedHit) {
                ATH_MSG_VERBOSE("---- Previous Hit ("<< indexPreviousMeasurement <<") Unused hit");       
              } else if (tsosDetails.type[indexPreviousMeasurement] == Outlier) {
                ATH_MSG_VERBOSE("---- Previous Hit ("<< indexPreviousMeasurement <<") Outlier");        
              }  
            }
            tsosDetails.type[index] = SplitSharedHit;
            ++trackHitDetails.numSplitSharedPix;
            
            // remember the last  2 ROTs
            lastbutonerot = lastrot;
            lastrot       = tsosDetails.RIO[index];
            lastrotindex  = index;   
            continue;             
          }
        }  // only split pixel clusters if there are few hole and the tracks have a good score
  
        ATH_MSG_VERBOSE("---> Pixel cluster is not compatible with being shared (splitProb1 = " 
                        << tsosDetails.splitProb1[index] << "), (splitProb2 = " 
                        << tsosDetails.splitProb2[index] << ") , reject shared hit !!!");
        if ( tsosDetails.hitIsShared[index] >= 2 ) {
          TrkCouldBeAccepted = false; // we have to remove at least one PRD   
          ATH_MSG_DEBUG ("reject track; Too many hits shared - we have to remove at least one PRD 105");    
          tsosDetails.type[index]    = RejectedHit;
          continue; 
        } 
      }
    }// End Attempt to recover shared hits
    
    //For all other shared hits 
    if ( tsosDetails.hitIsShared[index] > 0 && tsosDetails.hitIsShared[index] < m_maxTracksPerPRD ){  // we do not allow to share with to many tracks
      ATH_MSG_VERBOSE ("---> Shared hit, but good track, let's enter hit in the list and try to keep it !");
      ATH_MSG_VERBOSE ("----- Index: "<< index << " Type: " << tsosDetails.detType[index]  << " splitprob1  " << tsosDetails.splitProb1[index]);
      tsosDetails.type[index]   = SharedHit;
      ++trackHitDetails.numShared;                             // increase counter 
      trackHitDetails.numWeightedShared += (tsosDetails.detType[index]%10== 1 ? 2 : 1);  // increase counter
      // remember the last  2 ROTs
      lastbutonerot = lastrot;
      lastrot       = tsosDetails.RIO[index];
      lastrotindex  = index;   
      continue;
    } else if (tsosDetails.type[index] == Outlier &&  tsosDetails.hitIsShared[index] <= 0){
      continue;
    } else {
      ATH_MSG_DEBUG ("reject track; Too many hits shared - we have to remove at least one PRD 106");
      TrkCouldBeAccepted         = false; // we have to remove at least one PRD       
      tsosDetails.type[index]    = RejectedHit;
      continue;     
    }
  } 
 
  // If there are no hits on track compatible with be being split and are shared
  // We will make use of the NN information to determine if we should allow that hit to be shared
  /* Don't do this for now
     if (  m_doPixelClusterSplitting && trackHitDetails.numSplitSharedPix < 1){
     for (unsigned int index(0);  index  < tsosDetails.nTSoS; ++index ){
     if ( tsosDetails.type[index]  == SharedHit &&  
     tsosDetails.detType[index] % 10 == 1 &&  
     tsosDetails.splitProb1[index] < m_minsharedProbCut &&
     tsosDetails.splitProb2[index] < m_minsharedProbCut){
     tsosDetails.type[index] = RejectedHit;
     trackHitDetails.numShared--;                             // decrease counter 
     trackHitDetails.numWeightedShared -= 2;                  // decrease counter
     }
     }
     }
  */  
   
  // special cut, do not allow the last hit to be to far away or after to many holes
  if ( trackHitDetails.isPatternTrack                                                && // pattern track and
       trackHitDetails.totalSiHits() > m_minHits                                     && // we have enough hits on the track
       ( trackHitDetails.numSCTHoles > 3 || !lastrot || !lastbutonerot 
         ||(lastrot->globalPosition()-lastbutonerot->globalPosition()).mag()>1000*CLHEP::mm)) { // to many holes or distance cut
    ATH_MSG_DEBUG ("Special cut on distance or too many holes, reject last hit on track !");
    tsosDetails.type[lastrotindex] = RejectedHit;
    if (tsosDetails.type[lastrotindex] == UnusedHit) {
      trackHitDetails.numUnused -= 1; // update counter
    }
    // mark track as bad !
    TrkCouldBeAccepted     = false;
    ATH_MSG_DEBUG ("reject track; Special cut on distance or too many holes, reject last hit on track");    
  }


  // Add up how many hits of each type there are 
  bool firstMeasurement(true);
  for (unsigned int index(0);  index  <tsosDetails.nTSoS; ++index ){
    // remember if first hit is shared, we need that later
    if (firstMeasurement && tsosDetails.type[index] != OtherTsos){
      firstMeasurement = false;     
      if (tsosDetails.type[index] != SharedHit) trackHitDetails.firstisshared = false;
    }
    
    if (tsosDetails.type[index] == SharedHit ){
      if (tsosDetails.detType[index] == 11 ){ // Blayer
        trackHitDetails.hassharedblayer = true;
      } else if ( tsosDetails.detType[index] == 1 ){
        trackHitDetails.hassharedpixel  = true;
      }
    }
     
    if (tsosDetails.type[index] == SharedHit ||  tsosDetails.type[index] == SplitSharedHit){
      ++trackHitDetails.numSharedOrSplit;
      if (tsosDetails.detType[index]%10==1){
        ++trackHitDetails.numSharedOrSplitPixels;
        trackHitDetails.averageSplit1 += tsosDetails.splitProb1[index];
        trackHitDetails.averageSplit2 += tsosDetails.splitProb2[index]; 
      }
    }  
     

    if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] == UnusedHit ){
      ++trackHitDetails.numSCT_Unused;
    }
  }
  
  // Calculate average pixel split prob for all of those pixels that are either shared or want to be shared
  if (trackHitDetails.numSharedOrSplitPixels > 0 ){
    trackHitDetails.averageSplit1/=trackHitDetails.numSharedOrSplitPixels;
    trackHitDetails.averageSplit2/=trackHitDetails.numSharedOrSplitPixels;
  }

  
  // Check if the pair is compatible with being a light particle decay 
  bool passPairSelection =false;
  if (m_doPairSelection){

    //Find the accepted track that shares the most hits our proposed track 
    const Trk::Track*  mostOverlappingTrack(0);
    int  mostOverlappingNumberOfHits(0);
    int  indexOfFirstOverlappingHit(0);
    for ( std::multimap<const Trk::Track*,int>::iterator it = tsosDetails.overlappingTracks.begin(), end = tsosDetails.overlappingTracks.end();
          it != end; it = tsosDetails.overlappingTracks.upper_bound(it->first)) {
      int numberOfHitsSharedWithThisTrack  = std::distance( it, tsosDetails.overlappingTracks.upper_bound(it->first));
      ATH_MSG_DEBUG(it->first <<" shares " << numberOfHitsSharedWithThisTrack << " hits with this track " );
      if (mostOverlappingNumberOfHits < numberOfHitsSharedWithThisTrack){
        mostOverlappingNumberOfHits = numberOfHitsSharedWithThisTrack;
        mostOverlappingTrack = it->first;
        indexOfFirstOverlappingHit = it->second;
      }
    }

    //If an overlapping track if found get the track parameters on the first shared surface
    if ( mostOverlappingTrack ){ 
      auto tpPair = getOverlapTrackParameters(indexOfFirstOverlappingHit, ptrTrack, mostOverlappingTrack, prd_to_track_map, trackHitDetails.numSplitSharedPix );
      if (tpPair.first && tpPair.second){
        // Check if both tracks are above threshold       
        bool passPt = tpPair.first->pT() > m_minPairTrackPt && tpPair.second->pT() > m_minPairTrackPt;
        //Check if it is in a ROI
        trackHitDetails.inROI  = m_useEmClusSeed && isEmCaloCompatible( *tpPair.first, ent );
        //If it is then See if it passes the pair selection
        if ( passPt && (!m_useEmClusSeed || trackHitDetails.inROI) ){
          passPairSelection = isNearbyTrackCandidate(tpPair.first, tpPair.second );    
          if ( mostOverlappingNumberOfHits >= 2 && mostOverlappingNumberOfHits >= trackHitDetails.numShared){
            passPairSelection = true;
          }
        }
        if (passPairSelection) ATH_MSG_DEBUG ("Possible photon conversion");
      } else if ( trackHitDetails.isPatternTrack
                  && mostOverlappingNumberOfHits >= 2  
                  && mostOverlappingNumberOfHits >= trackHitDetails.numShared){
        ATH_MSG_DEBUG ("Possible photon conversion - for pattern track");
        trackHitDetails.inROI = true;
        passPairSelection = true;
      }
    }


    ATH_MSG_DEBUG ("Number of unused SCT hits:   " <<  trackHitDetails.numSCT_Unused);  
    ATH_MSG_DEBUG ("The track has "<< trackHitDetails.numSharedOrSplit       << " shared hits. Track "<< mostOverlappingTrack << " shares " << mostOverlappingNumberOfHits ); 
    ATH_MSG_DEBUG ("The track has "<< trackHitDetails.numSharedOrSplitPixels << " shared pixel hits. Average splitProb1 "<< trackHitDetails.averageSplit1 << " splitProb2 " << trackHitDetails.averageSplit2 ); 


    //Keep photon conversions
    if ( passPairSelection && // Track pair must be neutral and pass preselection
         trackHitDetails.numSCT_Unused >= m_minUniqueSCTHits  &&  trackHitDetails.numPixelHoles + trackHitDetails.numSCTHoles < 2) { //  We need to have a good number of unshared SCT hits

      //Change all shared SCT and Pixel hits to SplitSharedHit
      //Need to double check consequence of having split shared SCT hits
      ATH_MSG_VERBOSE ("Updating SCT hit information");
      trackHitDetails.numSplitSharedSCT = 0;          // reset counter
      trackHitDetails.numShared         = 0;          // reset counter 
      trackHitDetails.numWeightedShared = 0;          // reset counter

      trackHitDetails.hassharedblayer = false;
      trackHitDetails.hassharedpixel  = false;

      //Update counts but only allow 1 pixel hit to be updated free of charge 
      int noUpdatedPixels = 0;

      for (unsigned int index(0);  index  < tsosDetails.nTSoS; ++index ){    
        // Dont change blayer only pixel hits --  all other shared hits become splitshared
        if ( noUpdatedPixels < 1 && tsosDetails.detType[index]==1 && tsosDetails.type[index] == SharedHit ){
          ++noUpdatedPixels;
          tsosDetails.type[index] = SplitSharedHit;
        } 

        if ( tsosDetails.detType[index]==1 && tsosDetails.type[index] == SplitSharedHit ){
          ++trackHitDetails.numSplitSharedPix;
        } 

        if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] == SharedHit ){
          tsosDetails.type[index] = SplitSharedHit;
        }

        if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] == SplitSharedHit ){
          ++trackHitDetails.numSplitSharedSCT;
        }

        if (tsosDetails.type[index] == SharedHit){
          trackHitDetails.numShared++;                                                   // increase counter 
          trackHitDetails.numWeightedShared += (tsosDetails.detType[index] % 10 == 1  ? 2 : 1);  // increase counter
          if (tsosDetails.detType[index]  == 11){
            trackHitDetails.hassharedblayer = true;
            trackHitDetails.hassharedpixel  = true;
          }
        }
      }
    }
  }
    
  if (trackHitDetails.numSCT_Unused < m_minUniqueSCTHits 
      && trackHitDetails.numSplitSharedSCT > 0){
    ATH_MSG_VERBOSE ("Track has too few unique SCT hits - changing split shared back to shared in SCT");
    for (unsigned int index( tsosDetails.nTSoS  - 1 );  index != 0 ; --index ){    
      //if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] ==  SplitSharedHit){
      if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] == SplitSharedHit && tsosDetails.splitProb1[index] < 0.1 ){ //WPM
        tsosDetails.type[index] =SharedHit;
        --trackHitDetails.numSplitSharedSCT;
        ++trackHitDetails.numShared;         // increase counter 
        ++trackHitDetails.numWeightedShared;  // increase counter
      }
      
      if ( trackHitDetails.numSplitSharedSCT == 0){
        break;
      }
    }
  }
  
  // We shared SCT hits when we don't really have enough to share
  // Reject SCT shared hits
  if ( trackHitDetails.numSCT_Unused != trackHitDetails.numSCTHits &&
       trackHitDetails.numSCTHits <=  m_minUniqueSCTHits ){

    for (unsigned int index( 0 );  index > tsosDetails.nTSoS; ++index ){    
      //if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] == SharedHit){
      if ( tsosDetails.detType[index]==2 && tsosDetails.type[index] == SharedHit && tsosDetails.splitProb1[index] < 0.1 ){ //WPM
        tsosDetails.type[index] = RejectedHit;
        trackHitDetails.numShared--;         // decrease counter 
        trackHitDetails.numWeightedShared--; // decrease counter
      }      
    }
  }
  
  // get chi2/NDF, if track is fitted
  if ( !trackHitDetails.isPatternTrack ) {
    double trackchi2 = 0;
    if  (ptrTrack->fitQuality() && ptrTrack->fitQuality()->numberDoF()>0 ) {
      trackchi2 = ptrTrack->fitQuality()->chiSquared()/ptrTrack->fitQuality()->numberDoF();  
    }

    // if we have share hits and this is a bad track, we reject it 
    if ( trackHitDetails.numShared > 0 && trackchi2 > m_minTrackChi2ForSharedHits ) {
      ATH_MSG_DEBUG ("Shared hits, we have a bad chi2 track, mark it as bad !");
      // mark track as bad !
      TrkCouldBeAccepted = false;
      ATH_MSG_DEBUG ("reject track; Shared hits, we have a bad chi2 track");
    }
  }


  // numTRT cut, special for back tracking
  if ( trackHitDetails.numTRT_Unused < nCutTRT) {
    ATH_MSG_DEBUG ("Track fails TRT hit cut, mark it as bad !");
    // mark track as bad !
    TrkCouldBeAccepted = false;
    ATH_MSG_DEBUG ("reject track; Track fails TRT hit cut, mark it as bad");
  }
  
  // Check to see if we should reject it on the basis it will mess up another track  
  if (TrkCouldBeAccepted && trackHitDetails.numWeightedShared > 0){
    ATH_MSG_DEBUG ("Track has shared hits  check to if sharing this hit will mess up an accepted track");
    for (unsigned int index(0);  index  < tsosDetails.nTSoS; ++index ){
      if (tsosDetails.type[index] == SharedHit){

        int  maxiShared        = -1;
        int  maxothernpixel    = 0;
        bool maxotherhasblayer = false;
        bool otherfailsMinUniqueHits = false;
        bool isSplitable = tsosDetails.splitProb1[index] >= m_sharedProbCut || tsosDetails.splitProb2[index] >= m_sharedProbCut2;
        int numberOfTracksWithThisPrd = checkOtherTracksValidity( tsosDetails.RIO[index],
                                                                  isSplitable,
                                                                  splitProbContainer,
                                                                  prd_to_track_map,
                                                                  maxiShared,
                                                                  maxothernpixel,
                                                                  maxotherhasblayer,
                                                                  otherfailsMinUniqueHits,
                                                                  ent);
 
        if (numberOfTracksWithThisPrd > 0 && ( otherfailsMinUniqueHits || maxiShared >= ent->m_maxShared )){
          TrkCouldBeAccepted = false;
          ATH_MSG_DEBUG ("reject track; Tracks shared hits will mess up an accepted track");
          tsosDetails.type[index] = RejectedHit;
          trackHitDetails.numShared--;                             // decrease counter 
          trackHitDetails.numWeightedShared -= (tsosDetails.detType[index]%10== 1 ? 2 : 1);  // increase counter
        }
      }
    }
  }

  if (msgLvl(MSG::VERBOSE)){
    trackHitDetails.dumpInfo();
  }
  
  return TrkCouldBeAccepted;
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
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos.begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos.end(); 
  for ( ; iTsos != iTsosEnd ; ++iTsos) {
    const Trk::TrackStateOnSurface* newTsos = new Trk::TrackStateOnSurface(**iTsos);
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

  for (unsigned int index(0);  index  < tsosDetails.nTSoS; ++index ){
    //Only consider split shared hits 
    if (tsosDetails.type[index] != SplitSharedHit)
      continue;

    // And the hit is a pixel hit
    if (tsosDetails.detType[index]%10 == 1){
      const InDet::PixelCluster* pixelCluster = dynamic_cast<const InDet::PixelCluster*> ( tsosDetails.RIO[index]->prepRawData()  );
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
                                                                      int SplitSharedPix ) const
{
  
  auto returnPair = std::make_pair<const Trk::TrackParameters*,const Trk::TrackParameters*>(0,0);

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

  if (!(*firstTsos)->trackParameters()){
    ATH_MSG_DEBUG("There are no TrackParameters on this TSOS");
    return returnPair;  
  }

  returnPair.first = (*firstTsos)->trackParameters();

  const DataVector<const Trk::TrackStateOnSurface>* track2tsos = track2->trackStateOnSurfaces();

  auto iTsos    = track2tsos->begin(); 
  auto iTsosEnd = track2tsos->end();
  int measurementsBeforeShared = 0; 
  for (; iTsos != iTsosEnd ; ++iTsos) {
    
    if (measurementsBeforeShared > 1 + SplitSharedPix ){
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
    ATH_MSG_DEBUG("Success! got both track parameters "<< returnPair.first<< " " << returnPair.second );
    return returnPair;
  }
  
  
  ATH_MSG_DEBUG("Failed! got both track parameters");
  return returnPair;
  
}


bool  InDet::InDetDenseEnvAmbiTrackSelectionTool::isNearbyTrackCandidate(const Trk::TrackParameters* paraA, const Trk::TrackParameters* paraB ) const
{
  bool pass = true;
   
  //Only require opposite charge for is using EM clusters  -- more photon like 
  //if (m_useEmClusSeed && paraA->charge() * paraB->charge() > 0 ) 
  //  pass = false; 

  double dlocalX = paraA->parameters()[Trk::locX] - paraB->parameters()[Trk::locX];
  double dlocalY = paraA->parameters()[Trk::locY] - paraB->parameters()[Trk::locY];
 


  double dPhi = paraA->parameters()[Trk::phi] - paraB->parameters()[Trk::phi];
  if (dPhi >  M_PI) dPhi -= 2* M_PI;
  if (dPhi < -M_PI) dPhi += 2* M_PI;

  double dEta = std::fabs(1./std::tan(paraA->parameters()[Trk::theta]) - 1./std::tan(paraB->parameters()[Trk::theta]));

  if (std::fabs(dlocalX) > m_pairDeltaX || std::fabs(dlocalY) > m_pairDeltaY || std::fabs(dPhi) > m_pairDeltaPhi || std::fabs(dEta) > m_pairDeltaEta)
    pass = false;


  //double  d_beta = (paraA->momentum().dot(paraB->momentum()))/(paraA->momentum().mag()*paraB->momentum().mag());
  
  ATH_MSG_DEBUG("Conv?  Charge: "<<paraA->charge() * paraB->charge()
                <<" dX:   "   <<   dlocalX 
                <<" dy:   "   <<   dlocalY
                <<" dphi: "   <<   dPhi
                <<" dEta: "   <<   dEta                 
                <<" pass: "   << pass);
  
  return pass;

}
