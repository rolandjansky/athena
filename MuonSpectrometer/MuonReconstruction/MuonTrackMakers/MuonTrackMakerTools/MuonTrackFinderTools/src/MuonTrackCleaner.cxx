/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonTrackCleaner.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"

#include <iostream>

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

#include "TrkTrack/Track.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon {

  MuonTrackCleaner::MuonTrackCleaner(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa)
  {
    declareInterface<IMuonTrackCleaner>(this);
  }

  StatusCode MuonTrackCleaner::initialize()
  {
    
    ATH_CHECK( m_trackFitter.retrieve() );
    ATH_CHECK( m_slTrackFitter.retrieve() );
    ATH_CHECK( m_idHelperSvc.retrieve() );
    ATH_CHECK( m_edmHelperSvc.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_slextrapolator.retrieve() );
    ATH_CHECK( m_pullCalculator.retrieve() );
    ATH_CHECK( m_mdtRotCreator.retrieve() );
    ATH_CHECK( m_compRotCreator.retrieve() );
    ATH_CHECK( m_measurementUpdator.retrieve() );
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode MuonTrackCleaner::finalize()
  {
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<Trk::Track> MuonTrackCleaner::clean( Trk::Track& track, const std::set<Identifier>& chamberRemovalExclusionList ) const {

    CleaningState state;
    state.chamberRemovalExclusionList=chamberRemovalExclusionList;

    if( !state.chamberRemovalExclusionList.empty() ) {
      ATH_MSG_DEBUG(" Cleaning with exclusion list " << state.chamberRemovalExclusionList.size() );
    }

    std::unique_ptr<Trk::Track> cleanedTrack=cleanTrack(&track,state);

    for_each( state.parsToBeDeleted.begin(), state.parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );
    state.parsToBeDeleted.clear();

    return cleanedTrack;
  }
  

  std::unique_ptr<Trk::Track> MuonTrackCleaner::clean( Trk::Track& track ) const {
    
    CleaningState state;
    std::unique_ptr<Trk::Track> cleanedTrack=cleanTrack(&track,state);
    
    for_each( state.parsToBeDeleted.begin(), state.parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );
    state.parsToBeDeleted.clear();

    return cleanedTrack;
  }

  std::unique_ptr<Trk::Track> MuonTrackCleaner::cleanTrack( Trk::Track* track, CleaningState& state ) const {
    
    ATH_MSG_DEBUG(" Perform cleaning for track ");
    ATH_MSG_DEBUG(m_printer->print( *track ));
    
    init(*track, state);

    ATH_MSG_DEBUG("after init, track is "<<m_printer->print(*track));
    ATH_MSG_DEBUG("  start cleaning " );

    unsigned int nstationsInitial = state.stations.size();

    // first clean up chambers
    std::unique_ptr<Trk::Track> chamberTrack=chamberCleaning(std::make_unique<Trk::Track>(*track),state);
    if(!chamberTrack) {
      ATH_MSG_DEBUG(" chamber removal failed " );
      return nullptr;
    }
    ATH_MSG_DEBUG("after chamber cleaning, track is "<<m_printer->print(*chamberTrack));
    
    //fail if only station remains on the track
    unsigned int nstationsChamberCleaning = state.stations.size();
    if( nstationsInitial != nstationsChamberCleaning ){
      if(!checkStations(state)) return nullptr;
    }

    // if performed curved fit and we did not have a vertex constraint or ID hits, reject the track if inner was removed
    if( !state.slFit && !(state.hasVertexConstraint || state.nIdHits > 0 ) ) {
      if(!checkInnerConstraint(state)) return nullptr;
    }

    // if performing a single station layer cleaning without ID hits, reject track if there are insufficient phi constraints
    if(!checkPhiConstraint(state)) return nullptr;

    // clean competing ROTs
    std::unique_ptr<Trk::Track> cleanCompTrack=cleanCompROTs(std::move(chamberTrack),state);
    if(!cleanCompTrack) {
      ATH_MSG_DEBUG(" CompROT cleaning failed " );
      return nullptr;
    }
    ATH_MSG_DEBUG("after comp rot cleaning, track is "<<m_printer->print(*cleanCompTrack));

    // recover MDTs with flipped signs
    std::unique_ptr<Trk::Track> flippedTrack=recoverFlippedMdt(std::move(cleanCompTrack),state);
    if(!flippedTrack) {
      ATH_MSG_DEBUG(" MDT sign flipping failed " );
      return nullptr;
    }
    ATH_MSG_DEBUG("after flipped mdt recovery, track is "<<m_printer->print(*flippedTrack));

    std::unique_ptr<Trk::Track> hitTrack=hitCleaning(std::move(flippedTrack),state);
    if( !hitTrack ){
      ATH_MSG_DEBUG(" track lost after outlier removal " );
      return nullptr;
    }
    ATH_MSG_DEBUG("after hit cleaning, track is "<<m_printer->print(*hitTrack));

    //keep this clone in case outlier recovery fails but the rest of the cleaning was succesful.
    std::unique_ptr<Trk::Track> hitTrackClone(new Trk::Track(*(hitTrack.get())));

    //fail if only one station remains
    unsigned int nstationsHitCleaning = state.stations.size();
    if( nstationsInitial != nstationsHitCleaning ){
      if(!checkStations(state)) return nullptr;
    }

    // if performed curved fit and we did not have a vertex constraint or ID hits, reject the track if inner was removed
    if( !state.slFit && !(state.hasVertexConstraint || state.nIdHits > 0 ) ) {
      if(!checkInnerConstraint(state)) return nullptr;
    }
  
    // if performing a single station layer cleaning without ID hits, reject track if there are insufficient phi constraints
    if(!checkPhiConstraint(state)) return nullptr;

    std::unique_ptr<Trk::Track> cleanedTrack = outlierRecovery(std::move(hitTrack),state);
    // do not discard tracks that fail outlierRecovery, check that the track is ok
    //note that this also performs a useful check on the quality of the cleaning in general
    if( !cleanedTrack || !state.chambersToBeRemoved.empty() || !state.largePullMeasurements.empty() ) {
      init(*hitTrackClone,state);
      if(!state.chambersToBeRemoved.empty() || !state.largePullMeasurements.empty()){
        ATH_MSG_DEBUG("Outlier recovery failure unrecoverable, reject track");
        return nullptr;
      }
      else{
        ATH_MSG_DEBUG("Outlier recovery failed but initial track is recoverable");
        cleanedTrack = std::move(hitTrackClone);
      }
    }
    
    if( state.nhits < state.noutliers ){
      ATH_MSG_DEBUG(" track rejected due to high outlier ratio: hits " 
		    << state.nhits << "   outliers " << state.noutliers);
      return nullptr;
    }

    unsigned int nstationsFinal = state.stations.size();
    if( nstationsInitial != nstationsFinal ){
      if(!checkStations(state)) return nullptr;
    }

    ATH_MSG_VERBOSE(" final track " << m_printer->print( *cleanedTrack ));
    ATH_MSG_VERBOSE(m_printer->printStations( *cleanedTrack ));

    return cleanedTrack;
  }

  std::unique_ptr<Trk::Track> MuonTrackCleaner::cleanCompROTs( std::unique_ptr<Trk::Track> track, CleaningState& state ) const {
    
    if( !m_cleanCompROTs || state.numberOfCleanedCompROTs == 0 ) return track;

    const Trk::Perigee* perigee = track->perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return nullptr;
    }

    ATH_MSG_DEBUG(" Clean comp rots " << state.numberOfCleanedCompROTs );
    
    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( state.measInfo.size() );

    unsigned int nmeas = 0;
    // loop over hits
    InfoIt hit = state.measInfo.begin();
    InfoIt hit_end = state.measInfo.end();
    for( ;hit!=hit_end;++hit){

      // hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
      if( !hit->useInFit ){
	ATH_MSG_DEBUG("   removing hit " << m_idHelperSvc->toString(hit->id) 
		      << " pull " << hit->resPull->pull().front() );
	if( hit->inBounds ) tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState, 
										  *hit->meas,
										  *hit->pars, 
										  Trk::TrackStateOnSurface::Outlier) );

	continue;
      }
      else{
	if( hit->meas ) ++nmeas;
	  

	if( hit->cleanedCompROT ) {
	  tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								*hit->cleanedCompROT,
								*hit->pars, 
								Trk::TrackStateOnSurface::Measurement) );
	  ATH_MSG_DEBUG("   replacing CompROT " << m_idHelperSvc->toString(hit->id) 
			<< " pull " << hit->resPull->pull().front() );
	}
	else{
	  tsos->push_back( hit->originalState->clone() );
	}
      }
    }

    if( nmeas < 6 ){
      ATH_MSG_DEBUG(" too few hits, cannot recover CompROTS " );
      delete tsos;
      return nullptr;
    }

    // create new track
    std::unique_ptr<Trk::Track> cleanedTrack(new Trk::Track( track->info(), tsos, track->fitQuality() ? track->fitQuality()->clone():0 ));
    printStates(cleanedTrack.get());

    // fit new track
    std::unique_ptr<Trk::Track> newTrack=fitTrack(*cleanedTrack,track->info().particleHypothesis(),state.slFit);

    if( newTrack ) {
      init(*newTrack,state);
      return newTrack;
    }
    else return nullptr;
  }

  std::unique_ptr<Trk::Track> MuonTrackCleaner::recoverFlippedMdt( std::unique_ptr<Trk::Track> track, CleaningState& state ) const {
    
    if( !m_flipMdtDriftRadii || state.numberOfFlippedMdts == 0 ) return track;

    const Trk::Perigee* perigee = track->perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return nullptr;
    }

    ATH_MSG_DEBUG(" Trying to flip MDT signs: total number of hits with wrong sign " << state.numberOfFlippedMdts );
    
    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( state.measInfo.size() );

    unsigned int nmeas = 0;
    // loop over hits
    InfoIt hit = state.measInfo.begin();
    InfoIt hit_end = state.measInfo.end();
    for( ;hit!=hit_end;++hit){

      // hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
      if( !hit->useInFit ){
	ATH_MSG_DEBUG("   removing hit " << m_idHelperSvc->toString(hit->id) 
		      << " pull " << hit->resPull->pull().front() );
	if( hit->inBounds ) tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
										  *hit->meas,
										  *hit->pars,
										  Trk::TrackStateOnSurface::Outlier) );

	continue;
      }
      else{
	if( hit->meas ) ++nmeas;

	if( hit->flippedMdt ) {
	  tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								*hit->flippedMdt, 
								*hit->pars,
								Trk::TrackStateOnSurface::Measurement) );
	  ATH_MSG_DEBUG("   flipping sign hit " << m_idHelperSvc->toString(hit->id) 
			<< " pull " << hit->resPull->pull().front() );
	}
	else{
	  tsos->push_back( hit->originalState->clone() );
	}
      }
    }

    if( nmeas < 6 ){
      ATH_MSG_DEBUG(" too few hits, cannot flip MDT hit sign " );
      delete tsos;
      return nullptr;
    }

    // create new track
    std::unique_ptr<Trk::Track> cleanedTrack(new Trk::Track( track->info(), tsos, track->fitQuality() ? track->fitQuality()->clone():0 ));
    printStates(cleanedTrack.get());

    // fit new track
    std::unique_ptr<Trk::Track> newTrack=fitTrack(*cleanedTrack,track->info().particleHypothesis(),state.slFit);

    if( newTrack ) {
      init(*newTrack,state);
      return newTrack;
    }
    else return nullptr;
  }

  std::unique_ptr<Trk::Track> MuonTrackCleaner::hitCleaning( std::unique_ptr<Trk::Track> track, CleaningState& state ) const {

    if( state.largePullMeasurements.empty() ) return track;
    ATH_MSG_DEBUG(" trying outlier removal " );

    const Trk::Perigee* perigee = track->perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("input track without perigee " );
      return nullptr;
    }

    std::unique_ptr<Trk::Track> newTrack;

    for( unsigned int n=0;n<m_ncycles;++n ){

      // sanity check, should not remove too many hits
      if( state.largePullMeasurements.size() > 10 ) {
	ATH_MSG_DEBUG(" Too many outliers, cannot perform cleaning " );
	return nullptr;
      }

      ATH_MSG_VERBOSE(" outlier removal cycle " << n );

      DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
      tsos->reserve( state.measInfo.size() );
      ATH_MSG_VERBOSE("cleaning track with "<<state.measInfo.size()<<" hits");

      unsigned int nmeas = 0;
      unsigned int nremovedPhi = 0;
      bool hasSmall = false;
      bool hasLarge = false;
      MCTBCleaningInfo* firstPhi = 0;
      MCTBCleaningInfo* lastPhi = 0;
      std::map<MuonStationIndex::StIndex, std::pair<bool,bool> > slCountsPerStationLayer;
      // loop over hits
      InfoIt hit = state.measInfo.begin();
      InfoIt hit_end = state.measInfo.end();
      for( ;hit!=hit_end;++hit){
	
	bool remove = state.largePullMeasurements.count(hit->meas) || !hit->inBounds || hit->isNoise;
	// hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
	if( !hit->useInFit || remove ){

	  hit->useInFit = 0;

	  // count number of phi outliers
	  if( !hit->id.is_valid() || m_idHelperSvc->measuresPhi(hit->id) ) ++nremovedPhi;

	  if( remove ) {
	    std::string inb=hit->inBounds ? " inBounds" : " outBounds";
	    ATH_MSG_DEBUG("   removing hit " << m_idHelperSvc->toString(hit->id) 
			  << " pull " << hit->resPull->pull().front()<<inb);
	  }
	  if( hit->inBounds ) {
	    if( hit->cleanedCompROT ) {
	      tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								    *hit->cleanedCompROT,
								    *hit->pars, 
								    Trk::TrackStateOnSurface::Outlier) );
	    } else {
	      tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								    *hit->meas,
								    *hit->pars,
								    Trk::TrackStateOnSurface::Outlier) );
	    }
	  }
	  continue;
	}
	else{

	  if( hit->resPull ) ATH_MSG_DEBUG("   keeping hit " << m_idHelperSvc->toString(hit->id) 
					   << " pull " << hit->resPull->pull().front());
	  
	  if( hit->meas ) {
	    ++nmeas;
	    if( !hit->id.is_valid() || m_idHelperSvc->measuresPhi(hit->id)){
	      if( !firstPhi )     firstPhi = &(*hit);
	      else                lastPhi  = &(*hit);
	    }

	    if( hit->id.is_valid() && m_idHelperSvc->isMdt(hit->id) ){
	      MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex(hit->chIndex);
	      bool isSmall = m_idHelperSvc->isSmallChamber(hit->id);
	      bool isLarge = !isSmall;
	      // look for layer
	      std::map<MuonStationIndex::StIndex, std::pair<bool,bool> >::iterator pos = slCountsPerStationLayer.find(stIndex);
	      
	      // if not found add current
	      if( pos == slCountsPerStationLayer.end() ) slCountsPerStationLayer[stIndex] = std::make_pair(isSmall,isLarge);
	      else{
		// update 
		if( isSmall ) slCountsPerStationLayer[stIndex].first  = true;
		if( isLarge ) slCountsPerStationLayer[stIndex].second = true;
	      }
	    }
	  }
	  if(!hit->originalState) ATH_MSG_DEBUG("no original state!");
	  tsos->push_back( hit->originalState->clone() );
	}
      }
      // loop over sl map and count the overlaps
      unsigned int noverlaps = 0;
      for(auto [index, slCounts] : slCountsPerStationLayer){
	if( !hasSmall && slCounts.first )  hasSmall = true;
	if( !hasLarge && slCounts.second ) hasLarge = true;
	if( slCounts.first && slCounts.second ) ++noverlaps;
      }
      // now update for the case there are small and large chambers but not in the same layer
      if( noverlaps == 0 && hasSmall && hasLarge ) ++noverlaps;

      if( nmeas < 6 ){
	ATH_MSG_DEBUG(" too few hits, cannot perform hitCleaning " );
	delete tsos;
	return nullptr;
      }

      ATH_MSG_DEBUG( " nremovedPhi " << nremovedPhi << " noverlaps " << noverlaps << " nid " << state.nIdHits);
      if( firstPhi ) ATH_MSG_DEBUG( " hasFirstPhi: " << m_idHelperSvc->toString(firstPhi->id));
      if( lastPhi )  ATH_MSG_DEBUG(" hasLastPhi: " << m_idHelperSvc->toString(lastPhi->id));

      // only perform check on phi constraints if any phi hits were removed
      if( nremovedPhi > 0 ){
	bool hasPhiConstraint = false;
	
	if( state.nIdHits > 0 )                  hasPhiConstraint = true; // ok if ID hits
	else if( firstPhi && noverlaps > 0 )     hasPhiConstraint = true; // ok if one phi hit + one overlap
	else if( noverlaps > 1 )                 hasPhiConstraint = true; // ok if two overlaps
	else if( firstPhi && lastPhi && firstPhi->pars && lastPhi->pars ){
	  double distPhi = fabs((firstPhi->pars->position() - lastPhi->pars->position()).dot(firstPhi->pars->momentum().unit()));
	  ATH_MSG_DEBUG(" Distance between phi hits " << distPhi );
	  if( distPhi > 450. ) hasPhiConstraint = true;
	}
	if( !hasPhiConstraint ){
	  ATH_MSG_DEBUG("Lost phi constraint during track cleaning, reject track");
	  delete tsos;
	  return nullptr;
	}
      }

      // create new track
      std::unique_ptr<Trk::Track> cleanedTrack(new Trk::Track( track->info(), tsos, track->fitQuality() ? track->fitQuality()->clone():0 ));
      
      // fit new track
      printStates(cleanedTrack.get());

      newTrack=fitTrack(*cleanedTrack,track->info().particleHypothesis(),state.slFit);

      if( !newTrack ) {
	return nullptr;
      }
      else{
	const Trk::Perigee* perigee = newTrack->perigeeParameters();
	if( !perigee ){
	  ATH_MSG_DEBUG("   track without perigee " );
	  return nullptr;
	}
	// reinitialize cleaner
	init(*newTrack,state);
      }

      if( state.largePullMeasurements.empty() ) {
	ATH_MSG_DEBUG("   cleaning ended successfully after cycle " << n );
	return newTrack;
      }
    }
    
    return nullptr;
  }



  std::unique_ptr<Trk::Track> MuonTrackCleaner::chamberCleaning( std::unique_ptr<Trk::Track> track, CleaningState& state ) const {

    ATH_MSG_DEBUG("run chamber cleaning on track "<<m_printer->print(*track));

    if( state.chambersToBeRemoved.empty() && state.chambersToBeRemovedPhi.empty() ) return track;

    if( state.pullSumPerChamber.size() == 2 ) {
      
      // loop over chambers that should be removed, if one of them is MDT or CSC don't clean
      // as this would result in a single chamber track
      PullChIt chit = state.chambersToBeRemoved.begin();
      PullChIt chit_end = state.chambersToBeRemoved.end();
      for( ;chit!=chit_end;++chit ){
	const Identifier& chid = chit->second;
	if( m_idHelperSvc->isMdt(chid) || m_idHelperSvc->isCsc(chid) ){
	  ATH_MSG_DEBUG(" only two precision chambers, cannot remove chamber.  " );
	  return nullptr;
	}
      }
    }

    if( !state.chamberRemovalExclusionList.empty() ){

      unsigned int foundChambers = 0;
      // loop over chambers that should be removed and check if they are in the exclusion list
      PullChIt chit = state.chambersToBeRemoved.begin();
      PullChIt chit_end = state.chambersToBeRemoved.end();
      for( ;chit!=chit_end;++chit ){
	const Identifier& chid = chit->second;
	if( m_idHelperSvc->isMdt(chid) && state.chamberRemovalExclusionList.count(chid) ){
	  ATH_MSG_DEBUG(" found excluded chamber " << m_idHelperSvc->toStringChamber(chid) );
	  ++foundChambers;
	}
      }
      unsigned int excludedChambers = state.chamberRemovalExclusionList.size();
      if( foundChambers > excludedChambers ) {
	ATH_MSG_WARNING(" Found more excluded chambers than in list, this should not happen " );
	return 0;
      }
      else if( foundChambers == excludedChambers ){
	ATH_MSG_DEBUG(" all excluded chambers in removal list, failing cleaning " );
	return 0;
      }      
    }

    std::vector<ChamberRemovalOutput> cleaningResults;
    cleaningResults.reserve(state.chambersToBeRemoved.size());


    // TODO first remove hits in both eta and phi

    ATH_MSG_DEBUG(" number of chambers short listed to be removed: " << state.chambersToBeRemoved.size() );
    std::stable_sort( state.chambersToBeRemoved.begin(), state.chambersToBeRemoved.end(), SortByAvePull() );

    // try all combinations of removing a chamber with eta hits
    unsigned int nchambers = state.chambersToBeRemoved.size() + state.chambersToBeRemovedPhi.size();
    PullChIt chit = state.chambersToBeRemoved.begin();
    PullChIt chit_end = state.chambersToBeRemoved.end();
    for( ;chit!=chit_end;++chit ){
    
      ChamberRemovalOutput result = removeChamber( track.get(), chit->second, false, true, state );
      if( !result.track ) {
	ATH_MSG_DEBUG(" Removed eta hits of " << m_idHelperSvc->toStringChamber( chit->second ) << ", track lost ");
	continue;
      }
      ATH_MSG_DEBUG(" Removed eta hits of " << m_idHelperSvc->toStringChamber( chit->second ));
      ATH_MSG_DEBUG(m_printer->print( *result.track ));

      // this is an optimization for the common case where there is only one chamber to be removed
      if( nchambers > 1 ) unremoveHits(result);

      cleaningResults.push_back(std::move(result));      
    } // for (chit)

    ATH_MSG_DEBUG(" number of phi chambers short listed to be removed: " << state.chambersToBeRemoved.size() );
    std::stable_sort( state.chambersToBeRemovedPhi.begin(), state.chambersToBeRemovedPhi.end(), SortByAvePull() );
    // try all combinations of removing a chamber with phi hits
    chit = state.chambersToBeRemovedPhi.begin();
    chit_end = state.chambersToBeRemovedPhi.end();
    for( ;chit!=chit_end;++chit ){
   
      ChamberRemovalOutput result = removeChamber( track.get(), chit->second, true, false, state );
      if( !result.track ) {
	ATH_MSG_DEBUG(" Removed phi hits of " << m_idHelperSvc->toStringChamber( chit->second ) << ", track lost ");
	continue;
      }
      ATH_MSG_DEBUG(" Removed phi hits of " << m_idHelperSvc->toStringChamber( chit->second ));
      ATH_MSG_DEBUG(m_printer->print( *result.track ));

      // this is an optimization for the common case where there is only one chamber to be removed
      if( nchambers > 1 ) unremoveHits(result);

      cleaningResults.push_back(std::move(result));
    }


    // if no cleaned tracks return 0
    if( cleaningResults.empty() ) return nullptr;
    std::stable_sort( cleaningResults.begin(),cleaningResults.end(),SortChamberRemovalResultByChi2Ndof());
    ATH_MSG_DEBUG(" chamberCleaning Results nr " << cleaningResults.size());
    for(auto& res : cleaningResults){
      ATH_MSG_DEBUG(" track " << m_printer->print(*res.track ));
    }

    ChamberRemovalOutput& finalResult = cleaningResults.front();
    if( nchambers > 1 ){
      // loop over removed hits and redo 'remove' 
      for(auto hit : finalResult.removedHits) hit->useInFit = 0;
    }
    
    ATH_MSG_DEBUG(" chamberCleaning:  track ");
    ATH_MSG_DEBUG(m_printer->print( *finalResult.track ));

    //make clone just in case outlier recovery fails
    std::unique_ptr<Trk::Track> finalResultTrackClone(new Trk::Track(*(finalResult.track.get())));

    init(*finalResultTrackClone,state);
    
    // now finally check whether the removed layer now is recoverable (happens sometimes if the segment has one or more bad hits)
    MuonStationIndex::ChIndex removedChamberIndex = m_idHelperSvc->chamberIndex(finalResult.chId);
    std::unique_ptr<Trk::Track> recoveredTrack = outlierRecovery(std::move(finalResult.track),state,&removedChamberIndex);
    if( !recoveredTrack  ) return finalResultTrackClone;
    init(*recoveredTrack,state);
    return recoveredTrack;
  }
  


  MuonTrackCleaner::ChamberRemovalOutput MuonTrackCleaner::removeChamber( Trk::Track* track, Identifier chId, bool removePhi, bool removeEta, CleaningState& state ) const {
    
    ATH_MSG_DEBUG(" removing chamber " << m_idHelperSvc->toStringChamber(chId) );

    // store result
    ChamberRemovalOutput result;
    result.chId = chId;
    const Trk::Perigee* perigee = track->perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return result;
    }
    
    
    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( state.measInfo.size() );

    unsigned int nmeas = 0;
    // loop over hits
    InfoIt hit = state.measInfo.begin();
    InfoIt hit_end = state.measInfo.end();
    for( ;hit!=hit_end;++hit){

      if( hit->id.is_valid() ) {
        bool measuresPhi = m_idHelperSvc->measuresPhi(hit->id);
        bool remove = hit->chId == chId && ( (removePhi && measuresPhi) || (removeEta && !measuresPhi) );
        // hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
        if( !hit->useInFit || remove ){
	  ATH_MSG_DEBUG("   removing hit " << m_idHelperSvc->toString(hit->id) 
			<< " pull " << hit->resPull->pull().front());
          // add as outlier
          if( hit->inBounds ) tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
										    *hit->meas,
										    *hit->pars,
										    Trk::TrackStateOnSurface::Outlier) );
          
          // if removed, add hit to vector of hits 
	  //but only if the hit was not already an outlier to be skipped!
          if( remove && hit->useInFit) result.removedHits.push_back(&*hit);
          hit->useInFit = 0;
          continue;
        }
      }
      if( hit->meas ) ++nmeas;
      tsos->push_back( hit->originalState->clone() );
    }

    if( nmeas < 6 ){
      ATH_MSG_DEBUG(" too few hits, cannot perform chamberCleaning " );
      delete tsos;
      return result;
    }

    // create new track
    std::unique_ptr<Trk::Track> cleanedTrack(new Trk::Track( track->info(), tsos, track->fitQuality() ? track->fitQuality()->clone():0 ));
    
    // fit new track
    printStates(cleanedTrack.get());

    if( !cleanedTrack->perigeeParameters() ){
      ATH_MSG_DEBUG("   track without perigee " );
    }

    std::unique_ptr<Trk::Track> newTrack=fitTrack(*cleanedTrack,track->info().particleHypothesis(),state.slFit);

    if( newTrack && !newTrack->perigeeParameters() ){
      ATH_MSG_DEBUG("   fitted track without perigee " << *newTrack << " " << newTrack->perigeeParameters() );
    }

    result.track = std::move(newTrack);
    return result;
  }


  std::unique_ptr<Trk::Track> MuonTrackCleaner::outlierRecovery( std::unique_ptr<Trk::Track> track, CleaningState& state, MuonStationIndex::ChIndex* currentIndex ) const {
    
    const Trk::Perigee* perigee = track->perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return nullptr;
    }

    ATH_MSG_VERBOSE("  outlierRecovery: ");
    if( currentIndex ) ATH_MSG_VERBOSE(" layer " << MuonStationIndex::chName(*currentIndex));
    ATH_MSG_VERBOSE("  printing chamber statistics ");

    for (auto [stationIndex, layer] : state.chamberLayerStatistics) {
      if( stationIndex == MuonStationIndex::ChUnknown ) continue;
      ATH_MSG_VERBOSE(print(layer));
    }

    std::set<MuonStationIndex::ChIndex> recoverableLayers;
    for (auto [stationIndex, layer] : state.chamberLayerStatistics) {
      if( stationIndex == MuonStationIndex::ChUnknown ) continue;
      
      // skip all chamber layers except the requested one
      if( currentIndex && *currentIndex != stationIndex ) continue;

      ChamberLayerStatistics& statistics = layer;
      unsigned int nhits = statistics.nhits;
      unsigned int noutliers = statistics.noutliers;
      //unsigned int ndeltas = statistics.ndeltas;
      unsigned int nrecoverableOutliers = statistics.nrecoverableOutliers;
      unsigned int noutBounds = statistics.noutBounds;

      if( nrecoverableOutliers > 0 ){
	if( nhits + nrecoverableOutliers > 2 && 
	    ((noutBounds == 0 && noutliers == 0) || (nrecoverableOutliers != 0 && noutliers < 2) ) ) {
	  recoverableLayers.insert(stationIndex);
	  ATH_MSG_DEBUG("   found recoverable layer " << MuonStationIndex::chName(statistics.chIndex) );
	}
      }
    }

    // if there is nothing to be done exit
    if( recoverableLayers.empty() && !state.hasOfBoundsOutliers ) return track;
    bool addedHits = false;
    unsigned int removedOutOfBoundsHits(0);

    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( state.measInfo.size() );

    // loop over hits
    for(auto& hit : state.measInfo){

      if( !hit.useInFit ){
	if( hit.inBounds ){
	  if( recoverableLayers.count(hit.chIndex) ) {
	    // check whether we can savely add hits in this chamber to the track
	    bool recover =  !isOutsideOnTrackCut( hit.id, hit.residual,hit.pull,m_associationScaleFactor ) ? true : false;
	    if( recover && m_onlyUseHitErrorInRecovery && hit.pars ){
	      const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( hit.meas, hit.pars, Trk::ResidualPull::HitOnly );
	      if( !resPull ) {
		ATH_MSG_DEBUG(" calculation of residual/pull failed !!!!! " );
		recover = false;
	      }else{
		recover = !isOutsideOnTrackCut( hit.id, resPull->residual().front(), fabs(resPull->pull().front()),m_associationScaleFactor ) ? true : false;
		delete resPull;
	      }
	    }
	    if(  recover ) {
	      ATH_MSG_DEBUG("   adding outlier " << m_idHelperSvc->toString(hit.id) 
			    << " pull " << std::setw(7) << hit.pull);
	      if( hit.flippedMdt ) {
		double rDrift = hit.meas->localParameters()[Trk::locR];
		double rDriftFlip = hit.flippedMdt->localParameters()[Trk::locR];
		double rTrack = hit.pars->parameters()[Trk::locR];
		ATH_MSG_DEBUG(" flipped MDT: r_orig " << rDrift << " flip " << rDriftFlip << " rTrack " << rTrack);
	      }
	      addedHits = true;
	      const Trk::MeasurementBase* newMeas = hit.flippedMdt ? hit.flippedMdt.get() : hit.meas;
	      
	      tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit.originalState,
								    *newMeas, 
								    *hit.pars,
								    Trk::TrackStateOnSurface::Measurement) );
	    }else{
	      tsos->push_back( hit.originalState->clone() );
	    }
	  }
	  //layer not recoverable, drop the outliers: but if RPC, TGC, or CSC, expect track to go through all layers, so add a hole instead
	  if(m_idHelperSvc->isRpc(hit.id) || m_idHelperSvc->isTgc(hit.id) || m_idHelperSvc->isCsc(hit.id)) tsos->push_back(MuonTSOSHelper::createHoleTSOS(hit.pars->clone()));
	}
	else{
	  ++removedOutOfBoundsHits;
	  //if RPC, TGC, or CSC, expect track to go through all layers: add a hole to replace lost outlier
	  if(m_idHelperSvc->isRpc(hit.id) || m_idHelperSvc->isTgc(hit.id) || m_idHelperSvc->isCsc(hit.id)) tsos->push_back(MuonTSOSHelper::createHoleTSOS(hit.pars->clone()));
	  ATH_MSG_DEBUG("   removing out of bounds outlier " << m_idHelperSvc->toString(hit.id) 
			<< " pull " << std::setw(7) << hit.pull);
	}
      }
      else{
	tsos->push_back( hit.originalState->clone() );
      }
    }

    if( !addedHits && removedOutOfBoundsHits == 0 ){
      ATH_MSG_DEBUG(" track unchanged ");
      delete tsos;
      return track;
    }

    if( tsos->size() < 6 ){
      ATH_MSG_WARNING(" too few hits, cannot add hits. This should not happen " );
      delete tsos;
      return nullptr;
    }

    // create new track
    std::unique_ptr<Trk::Track> cleanedTrack(new Trk::Track( track->info(), tsos, track->fitQuality() ? track->fitQuality()->clone():0 ));
    
    if( !addedHits ){
      ATH_MSG_DEBUG(" only removed out of bound hits, returning track without new fit " );
      return cleanedTrack;
    }

    // fit new track
    printStates(cleanedTrack.get());

    std::unique_ptr<Trk::Track> newTrack=fitTrack(*cleanedTrack,track->info().particleHypothesis(),state.slFit);

    if( newTrack ) {
      ATH_MSG_DEBUG("Outlier recovery successfull " );
      init(*newTrack,state);
    }else{
      ATH_MSG_DEBUG("refit after outlier recovery failed " );
    }

    return newTrack;
  }



  void MuonTrackCleaner::init( const Trk::Track& track, CleaningState& state ) const {

    //for_each( state.parsToBeDeleted.begin(), state.parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );
    //state.parsToBeDeleted.clear();
    state.nscatterers = 0;
    state.numberOfFlippedMdts = 0;
    state.numberOfCleanedCompROTs = 0;
    state.nhits = 0;
    state.noutliers = 0;
    state.pullSum = ChamberPullInfo(); 
    state.pullSumPhi = ChamberPullInfo();
    state.pullSumTrigEta = ChamberPullInfo();
    state.chambersToBeRemoved.clear();
    state.chambersToBeRemovedPhi.clear();
    state.pullSumPerChamber.clear();
    state.pullSumPerChamberPhi.clear();
    state.pullSumPerChamberEta.clear();
    state.hitsPerChamber.clear();
    state.outBoundsPerChamber.clear();
    state.measInfo.clear();
    state.largePullMeasurements.clear();
    state.chamberLayerStatistics.clear();
    state.stations.clear();
    state.phiLayers.clear();
    state.hasOfBoundsOutliers = false;
    state.hasVertexConstraint = false;
    state.hasSmall = false;
    state.hasLarge = false;
    state.nIdHits = 0;
    state.nPseudoMeasurements = 0;
    state.nPhiConstraints = 0;

    MagField::AtlasFieldCache    fieldCache;
    // Get field cache object
    EventContext ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
      return;
    }
    fieldCondObj->getInitializedCache (fieldCache);
    
    state.slFit =  !fieldCache.toroidOn() || m_edmHelperSvc->isSLTrack( track );
    if(m_use_slFit) state.slFit = true;

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, cannot perform cleaning " );
      return;
    }
    
    ATH_MSG_DEBUG(" init: " << m_printer->print(track));

    state.measInfo.reserve(states->size());

    std::set<int> rpcLayers;
    std::set<int> tgcLayers;
    const Trk::MeasurementBase* mdtmeas = 0;
    double largestmdtpull=-999;

    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      if((*tsit)->type(Trk::TrackStateOnSurface::Perigee)) ATH_MSG_DEBUG((*tsit)->dumpType()<<", parameters "<<*(*tsit)->trackParameters());
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ){
	state.measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }
      
      if( (*tsit)->type(Trk::TrackStateOnSurface::Scatterer) ) {
	const Trk::MaterialEffectsBase* matEff = (*tsit)->materialEffectsOnTrack();
	const Trk::MaterialEffectsOnTrack* matTrk = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(matEff);
	ATH_MSG_VERBOSE(" Scatterer: r  " << pars->position().perp()<< " z " << pars->position().z());
	if( matEff ) ATH_MSG_VERBOSE(" X0 " << matEff->thicknessInX0());
	if( matTrk && matTrk->scatteringAngles() ){
	  const Trk::ScatteringAngles* scatAngle = matTrk->scatteringAngles();
	  ATH_MSG_VERBOSE(" pull phi " << scatAngle->deltaPhi()/scatAngle->sigmaDeltaPhi()<< " pull theta " << scatAngle->deltaTheta()/scatAngle->sigmaDeltaTheta());
	}
	if( matTrk && matTrk->energyLoss() ){
	  const Trk::EnergyLoss* eloss = matTrk->energyLoss();
	  ATH_MSG_DEBUG( " eloss " << eloss->deltaE());
	}
	++state.nscatterers;
	state.measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ){	
	state.measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }

      Identifier id = m_edmHelperSvc->getIdentifier(*meas);
      bool pseudo = !id.is_valid();
      if( pseudo ) ++state.nPseudoMeasurements;

      if( !pseudo && !m_idHelperSvc->mdtIdHelper().is_muon(id) ){
	ATH_MSG_VERBOSE("      TSOS is not a muon hit, position: r  " << pars->position().perp()<< " z " << pars->position().z());

	// count ID hits on track
	if( (*tsit)->type(Trk::TrackStateOnSurface::Measurement) && m_idHelperSvc->mdtIdHelper().is_indet(id) ){
	  ++state.nIdHits;
	}
	state.measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }

      // check whether there is a vertex constraint
      if( pseudo && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) && pars->associatedSurface().center().perp() < 200. ){
	state.hasVertexConstraint = true;
      }

      Identifier chId = pseudo ? id : m_idHelperSvc->chamberId(id);
      bool measuresPhi = pseudo ? true : m_idHelperSvc->measuresPhi(id);
      
      // bound checks
      Amg::Vector2D locPos;
      if( !meas->associatedSurface().globalToLocal(pars->position(),pars->position(),locPos) ){
	ATH_MSG_DEBUG(" localToGlobal failed !!!!! " );
	continue;
      }
      bool inBounds = true;
      double tol1 = 100.;
      double tol2 = 2*tol1;
      if( !pseudo && m_idHelperSvc->isMdt(id) ) tol1 = 5.;
      
      // we need a special bound check for MDTs so we cast to SL surface
      const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>(&meas->associatedSurface());
      if( slSurf ) {
	// perform bound check only for second coordinate
	inBounds = slSurf->bounds().insideLoc2(locPos,tol2);
      }else{
	inBounds = meas->associatedSurface().insideBounds(locPos,tol1,tol2);
      }

      MuonStationIndex::ChIndex chIndex = !pseudo ? m_idHelperSvc->chamberIndex(id) : MuonStationIndex::ChUnknown;
      
      // pointer to resPull: workaround because a const pointer is returned
      const Trk::ResidualPull* tempPull=m_pullCalculator->residualPull( meas, pars, (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ? Trk::ResidualPull::Unbiased : Trk::ResidualPull::Biased );
      std::unique_ptr<Trk::ResidualPull> resPull=std::make_unique<Trk::ResidualPull>(*tempPull);
      delete tempPull;
      if( !resPull ) {
	ATH_MSG_DEBUG(" calculation of residual/pull failed !!!!! " );
	continue;
      }
      int pullSize=resPull->pull().size();
      double residual = resPull->residual().front(); 
      double pull = fabs(resPull->pull().front());
      
      // sanity check
      if( !pseudo && pullSize != 1 ){
	ATH_MSG_WARNING(" ResidualPull vector has size " << pullSize << " for channel " << m_idHelperSvc->toString(id) );
	continue;
      }
      
      bool isMDT = !pseudo ? m_idHelperSvc->isMdt(id) : false;
      double error = pull > 0.001 ? fabs(residual/pull) : 1000.;
      double rDrift = isMDT ? meas->localParameters()[Trk::locR] : 0.;
      double rTrack = isMDT ? pars->parameters()[Trk::locR] : 0.;
      double rTrackAbs = fabs(rTrack);
      double flippedResidual = isMDT ? rDrift + rTrack : 1e10;
      double flippedPull = isMDT ? fabs(flippedResidual/error) : 1e10;
      
      bool isNoise = false;
      bool isOutlier = isOutsideOnTrackCut(id,residual,pull, 1 );
      bool isRecoverable = m_recoverOutliers && !isOutlier && !isOutsideOnTrackCut(id,residual,pull, m_associationScaleFactor );
      bool flippedIsRecoverable = isMDT && flippedPull < pull - 0.1 && !isOutsideOnTrackCut(id,flippedResidual,flippedPull,m_associationScaleFactor);
      double innerRadius = 14.6;
      if( isMDT ) {
	const MdtDriftCircleOnTrack* mdtdc = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	if(mdtdc) innerRadius = mdtdc->detectorElement()->innerTubeRadius();
      }

      bool isDelta = isOutlier && isMDT && rTrackAbs < innerRadius && rTrackAbs > fabs(rDrift);

      // remove all outliers that are too far from the track
      if( isOutlier ){
	if( isMDT ){
	  if( rTrackAbs > innerRadius ) inBounds = false;
	}else if( pull > 10. ) {
	  inBounds = false;
	}
      }

      std::unique_ptr<MdtDriftCircleOnTrack> mdtRotFlipped;
      std::unique_ptr<CompetingMuonClustersOnTrack> updatedCompRot;
      bool flipSign = false;
      if( !pseudo ){
	const MdtDriftCircleOnTrack* mdtRot = isMDT ? dynamic_cast<const MdtDriftCircleOnTrack*>(meas) : 0;
	if( mdtRot && mdtRot->prepRawData() && mdtRot->prepRawData()->adc() < m_adcCut ) {
	  isNoise = true;
	  isOutlier = true;
	  isRecoverable = false;
	  flippedIsRecoverable = false;
	  isDelta = false;
	}
	if( flippedIsRecoverable ) {
	  residual = flippedResidual;
	  pull = flippedPull;
	  flipSign = true;
	}
	if( !isNoise && flipSign ){
	  if( mdtRot ){
	    mdtRotFlipped = std::make_unique<MdtDriftCircleOnTrack>(*mdtRot);
	    Trk::DriftCircleSide side = rDrift < 0. ? Trk::RIGHT : Trk::LEFT;
	    m_mdtRotCreator->updateSign(*mdtRotFlipped,side);
	    double rDriftFlip = mdtRotFlipped->localParameters()[Trk::locR];
	    int signRot = rDrift < 0 ? -1 : 1;
	    int signRotFlip = rDriftFlip < 0 ? -1 : 1;
	    if( rDrift != 0. && signRot == signRotFlip ){
	      ATH_MSG_WARNING(" failed to flip sign of MDT " << rDrift << "  flip " << rDriftFlip );
	    }
	  }
	  else{
	    ATH_MSG_WARNING(" failed to dynamic_cast measurement with MDT Identifier to a MdtDriftCircleOnTrack" );
	  }
	}
	else{
	  // if the outlier MDT is not recoverable by flipping the sign, add it as the worst outlier
	  if( isOutlier && isMDT && pull>largestmdtpull && !(*tsit)->type(Trk::TrackStateOnSurface::Outlier)){
	    largestmdtpull=pull;
	    mdtmeas=meas;
	  }
	}
	
	if( measuresPhi ){
	  bool isRpc = m_idHelperSvc->isRpc(id);
	  if( isRpc ){
	    int layer = 0;
	    MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
	    if( stIndex == Muon::MuonStationIndex::BM && m_idHelperSvc->rpcIdHelper().doubletR(id) == 1 ) layer = 1;
	    else if( stIndex == Muon::MuonStationIndex::BO ) layer = 2;
	    rpcLayers.insert(layer);
	  }
	  
	  bool isTgc = m_idHelperSvc->isTgc(id);
	  if( isTgc ){
	    int layer = 0;
	    MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
	    if( stIndex == Muon::MuonStationIndex::EM ){
	      std::string stName = m_idHelperSvc->chamberNameString(id);
	      if( stName[1] == '1' )      layer = 1;
	      else if( stName[1] == '2' ) layer = 2;
	      else if( stName[1] == '3' ) layer = 3;
	      else{
		ATH_MSG_WARNING("Unable to calculate TGC layer for " << m_idHelperSvc->toString(id) );
		layer = -1;
	      }
	    }	   
	    if( layer != -1 ) tgcLayers.insert(layer);
	  }
	  if( m_idHelperSvc->issTgc(id) ){
	    int layer = 4;
	    if( m_idHelperSvc->stgcIdHelper().multilayer(id) == 2 ) layer = 5;
	    tgcLayers.insert(layer);
	    ATH_MSG_VERBOSE("adding STGC phi hit " << layer  << " size " << tgcLayers.size() );
	  }
	}

	if( m_cleanCompROTs ){
	  const CompetingMuonClustersOnTrack* crot = (measuresPhi && !isMDT && m_idHelperSvc->isRpc(id)) ? 
	    dynamic_cast<const CompetingMuonClustersOnTrack*>(meas) : 0;
	  if( crot ){
	    ATH_MSG_DEBUG(" CompetingMuonClustersOnTrack with rots " << crot->numberOfContainedROTs());
	    double minpos = 0.;
	    double minres = 0.;
	    double maxres = 0.;
	    double absminres = 0.;
	    double absmaxres = 0.;
	    for( unsigned int i=0;i<crot->numberOfContainedROTs();++i){
	      const MuonClusterOnTrack* cluster = &crot->rioOnTrack(i);
	      if( !cluster ) continue;
	      double residual = cluster->localParameters()[Trk::locX] - pars->parameters()[Trk::locX];
	      double absres = residual < 0. ? -1.*residual : residual;
	      if( i == 0 ){
		minres = residual;
		maxres = residual;
		minpos = cluster->localParameters()[Trk::locX];
		absminres = absres;
		absmaxres = absres;
	      }else if( absres < absminres ){
		minres = residual;
		absminres = absres;
		minpos = cluster->localParameters()[Trk::locX];
	      }else if( absres > absmaxres ){
		maxres = residual;
		absmaxres = absres;
	      }
	      ATH_MSG_VERBOSE(" ROT " << m_idHelperSvc->toString(cluster->identify())<< " lpos " << cluster->localParameters()[Trk::locX] << 
			      " pars " << pars->parameters()[Trk::locX]<< " residual " << residual);
	    }
	    ATH_MSG_DEBUG(" residuals: min  " << minres << " max " << maxres << " diff " << maxres - minres);
	    bool splitCompRot = false;
	    if( fabs(maxres - minres) > 100 && absmaxres-absminres > 20 && crot->numberOfContainedROTs() < 20 ){
	      ATH_MSG_DEBUG( " recoverable double cluster ");
	      splitCompRot = true;
	    }
	    if( splitCompRot ){

	      std::list<const Trk::PrepRawData*> prdList;
	      ATH_MSG_DEBUG(" Splitting comp rot ");
	      for( unsigned int i=0;i<crot->numberOfContainedROTs();++i){
		const MuonClusterOnTrack* cluster = &crot->rioOnTrack(i);
		if( !cluster ) continue;
		double residual = cluster->localParameters()[Trk::locX] - minpos;
		double absres = residual < 0. ? -1.*residual : residual;
		if( absres < 40 ){
		  ATH_MSG_DEBUG("   NEW ROT " << m_idHelperSvc->toString(cluster->identify()) 
				<< " lpos " << cluster->localParameters()[Trk::locX] << " pars " << pars->parameters()[Trk::locX] 
				<< " residual " << residual);
		  prdList.push_back(cluster->prepRawData());
		}
	      }	
	      if( prdList.empty() ){
		ATH_MSG_WARNING("No clusters selected during comprot cleaning, keeping old cluster" );
	      }else{
		//TODO: createBroadCluster returns a const object so a workaround is needed to get a unique pointer, this should be fixed in some fashion 
		CompetingMuonClustersOnTrack tempCompRot=*m_compRotCreator->createBroadCluster(prdList,0.);
		updatedCompRot = std::make_unique<CompetingMuonClustersOnTrack>(tempCompRot);
		++state.numberOfCleanedCompROTs;
	      }
	    }
	  }
	}
      }

      state.measInfo.push_back( MCTBCleaningInfo(id,chId,chIndex,inBounds,residual,pull,*tsit,meas,pars,resPull,0) );
      MCTBCleaningInfo& info = state.measInfo.back();
      if( flipSign ) {
	info.flippedMdt = std::move(mdtRotFlipped);
      }
      info.isNoise = isNoise;
      if( updatedCompRot ){
	ATH_MSG_DEBUG("updated competing ROT");
	info.cleanedCompROT = std::move(updatedCompRot);
	if( info.cleanedCompROT->associatedSurface() != meas->associatedSurface() ){
	  const Trk::TrackParameters* exPars= state.slFit ? m_slextrapolator->extrapolate(*pars,info.cleanedCompROT->associatedSurface(),Trk::anyDirection,false,Trk::muon) :
                                                      m_extrapolator->extrapolate(*pars,info.cleanedCompROT->associatedSurface(),Trk::anyDirection,false,Trk::muon);
	  if( !exPars ){
	    ATH_MSG_WARNING("Update of comp rot parameters failed, keeping old ones" );
	    info.cleanedCompROT.reset();
	  }
	  else{
	    info.pars = exPars;
	    state.parsToBeDeleted.push_back(exPars);
	  }
	}
      }

      ChamberLayerStatistics& chamberStatistics = state.chamberLayerStatistics[chIndex];
      chamberStatistics.chIndex = chIndex;

      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) {
	ATH_MSG_DEBUG("  Outlier " << m_idHelperSvc->toString( id ) << m_printer->print( *(info.resPull) ));
	if( isMDT ) ATH_MSG_DEBUG(" r_drift " << rDrift << "  r_trk " << rTrack << " dr " << std::setw(7) << Amg::error(meas->localCovariance(),Trk::locR));
	if( isNoise ) ATH_MSG_DEBUG( " noise ");
	if( !inBounds ) ATH_MSG_DEBUG(" outBounds ");
	else{
	  ATH_MSG_DEBUG( " inBounds  ");
	  if( flippedIsRecoverable ) ATH_MSG_DEBUG(" flipped is recoverable, rFlipped " << info.flippedMdt->localParameters()[Trk::locR]);
	  else if( isRecoverable ) ATH_MSG_DEBUG( " recoverable ");
	  else if( isDelta ) ATH_MSG_DEBUG(" delta ");
	}

	if( !inBounds ){
	  state.hasOfBoundsOutliers = true;
	  ++chamberStatistics.noutBounds;
	}
	else if( flippedIsRecoverable || isRecoverable ) ++chamberStatistics.nrecoverableOutliers;
	else if( isDelta )                               ++chamberStatistics.ndeltas;
	else if( pull < 10 )                             ++chamberStatistics.noutliers;

	if(!pseudo) ++state.noutliers; //don't count pseudos here
	info.useInFit = 0;
	continue;
      }
      
      // if we get here could be as hit
      ++chamberStatistics.nhits;
      if(!pseudo) ++state.nhits; //pseudo's shouldn't count here

      if( flipSign ) ++state.numberOfFlippedMdts;

      std::string idString   = pseudo ? " Pseudomeasurement " :  m_idHelperSvc->toString( id ); 
      std::string boundCheck = inBounds ? "inBounds" : "outBounds"; 
      ATH_MSG_VERBOSE(m_printer->print( *(info.resPull) ) << " "  << idString << " " << boundCheck);
      if( isNoise ) ATH_MSG_VERBOSE(" noise");
      else if( isOutlier ) ATH_MSG_VERBOSE(" outlier");
      if( isMDT ){
	ATH_MSG_VERBOSE(" r_drift " << std::setw(8) << rDrift << "  r_trk " << std::setw(7) << rTrack 
			<< " dr " << std::setw(7) << Amg::error(meas->localCovariance(),Trk::locR));
	if( flipSign )  ATH_MSG_VERBOSE(" flipped rDrift " << info.flippedMdt->localParameters()[Trk::locR]
					 << "  pull " << flippedPull);
	else if( isDelta ) ATH_MSG_VERBOSE(" delta ");
      }
      else{
	const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(meas);
	if( !rpc ){
	  const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
	  if( crot ){
	    rpc = dynamic_cast<const RpcClusterOnTrack*>(crot->containedROTs().front());
	  }
	}
	if( rpc ) ATH_MSG_DEBUG( " time " << rpc->prepRawData()->time()-rpc->globalPosition().mag()/300.);
      }
  
      if( pseudo ){
	
	// pseudo measurements should not be included in chamber hit removal so stop here
	if( !isNoise && isOutlier ) {
	  state.largePullPseudoMeasurements.insert( meas );
	}
	continue;
	
      }
      else {
	// count hits
	EtaPhiHits& hits = state.hitsPerChamber[info.chId];
	if( measuresPhi ) ++hits.nphi;
	else              ++hits.neta;
	
	// bound checks
	if( !inBounds ) {
	  EtaPhiHits& outHits = state.outBoundsPerChamber[info.chId];
	  if( measuresPhi ) ++outHits.nphi;
	  else              ++outHits.neta;
	}
	
	// measurements with large pull
	if( !(*tsit)->type(Trk::TrackStateOnSurface::Outlier) && isOutlier && !isMDT ) {
	  state.largePullMeasurements.insert( meas );
	}
	
	// pulls per chamber
	if( !m_idHelperSvc->isTrigger(info.chId) ) {
	  
	  ChamberPullInfo& pullInfoHits = measuresPhi ? state.pullSumPhi : state.pullSum;
	  pullInfoHits.pullSum += pull;
	  ++pullInfoHits.nhits;
	  if( pull > pullInfoHits.maxPull ) pullInfoHits.maxPull = pull;
	  
	  ChamberPullInfo& pullInfoCh = measuresPhi ? state.pullSumPerChamberPhi[info.chId] : state.pullSumPerChamber[info.chId];
	  pullInfoCh.pullSum += pull;
	  ++pullInfoCh.nhits;
	  if( pull > pullInfoCh.maxPull ) pullInfoCh.maxPull = pull;
	}else{
	  ChamberPullInfo& pullInfoTrig = measuresPhi ? state.pullSumPhi : state.pullSumTrigEta;
          if(!measuresPhi) state.pullSumPerChamberEta[info.chId];
	  pullInfoTrig.pullSum += pull;
	  ++pullInfoTrig.nhits;
	  if( pull > pullInfoTrig.maxPull ) pullInfoTrig.maxPull = pull;
	}
      }
    }
    
    // if there was an MDT outlier add it to the list of hits to be removed
    if (mdtmeas) state.largePullMeasurements.insert( mdtmeas );

    // check whether we have sufficient layers to savely clean the RPC/TGC comp rots
    unsigned int nphiLayers = rpcLayers.size() + tgcLayers.size();
    if( state.hasVertexConstraint ) {
      nphiLayers += 1;
    }
    if( state.nIdHits > 0 ) {
      nphiLayers += 2;
    }

    if( ( nphiLayers > 2 ) && state.numberOfCleanedCompROTs > 0 ){
      ATH_MSG_DEBUG(" Sufficient constraints to clean competing ROTs: trigger layers " << nphiLayers );
    }else{
      // reset counter so no cleaning is performed
      state.numberOfCleanedCompROTs = 0;
    }

    // update sl fit configuration if track has ID hits or vertex constraint
    if( !m_use_slFit && state.slFit && (state.hasVertexConstraint || state.nIdHits > 0 ) && fieldCache.solenoidOn() ) {
      state.slFit = false;
    }


    if(state.hasVertexConstraint || state.nIdHits ) {
      if( state.hasVertexConstraint ) ATH_MSG_DEBUG(" Track has vertex contraint:");
      if( state.nIdHits > 0 )         ATH_MSG_DEBUG(" Track has  ID Hits: " << state.nIdHits);
      if( fieldCache.solenoidOn() ) ATH_MSG_DEBUG(" Solenoid On");
      else               ATH_MSG_DEBUG(" Solenoid Off");
      if( fieldCache.toroidOn() )   ATH_MSG_DEBUG(" Toroid On");
      else               ATH_MSG_DEBUG(" Toroid Off");
      if( state.slFit )  ATH_MSG_DEBUG(" Use SL fit");
      else               ATH_MSG_DEBUG(" Use curved fit");
    }

    std::set<Identifier> chambersToBeRemoved, chambersToBeRemovedPhi, goodEtaLayers, goodPhiLayers;

    // check whether there are any chambers in eta that should be removed
    extractChambersToBeRemoved(state,chambersToBeRemoved);

    // check whether there are any chambers in phi that should be removed
    extractChambersToBeRemoved(state,chambersToBeRemovedPhi,true); 

    ATH_MSG_DEBUG("Chambers on track summary:");

    EtaPhiPerChamberIt chit =  state.hitsPerChamber.begin();
    EtaPhiPerChamberIt chit_end =  state.hitsPerChamber.end();
    for( ;chit!=chit_end;++chit ){
      
      if( !chit->first.is_valid() ) continue;

      bool isPrec = m_idHelperSvc->isMdt(chit->first) || m_idHelperSvc->isCsc(chit->first) || m_idHelperSvc->isMM(chit->first) || m_idHelperSvc->issTgc(chit->first);
      if( isPrec ){
	MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chit->first);
	state.stations.insert(stIndex);
      }

      EtaPhiHits& nhits      = chit->second;
      EtaPhiHits& noutBounds = state.outBoundsPerChamber[chit->first];
      ATH_MSG_DEBUG(" chamber " << m_idHelperSvc->toStringChamber(chit->first) 
		    << "  eta hits " << nhits.neta << " outBounds " << noutBounds.neta
		    << "  phi hits " << nhits.nphi << " outBounds " << noutBounds.nphi);
      if( nhits.neta != 0 && nhits.neta == noutBounds.neta ){
	ATH_MSG_DEBUG("   --> All eta hits out of bounds ");
	// remove eta hits for this chamber
	double fakePull = 1000.*nhits.neta;
	if( !chambersToBeRemoved.count(chit->first) ) {
	  // just add it
	  state.chambersToBeRemoved.push_back( std::make_pair(fakePull,chit->first) );
	  chambersToBeRemoved.insert(chit->first);
	} else {
	  // replace existing entry if new one has larger Pull
	  PullChIt pIt     = state.chambersToBeRemoved.begin();
	  PullChIt pIt_end = state.chambersToBeRemoved.end();
	  for ( ; pIt != pIt_end; ++pIt ) {
	    if ( pIt->second == chit->first && pIt->first < fakePull ) {
	      pIt->first = fakePull;
	      break;
	    }
	  }
	}
      }
      else{
	if( noutBounds.neta != 0 ) ATH_MSG_DEBUG("   --> Some eta hits out of bounds ");

	if( isPrec && nhits.neta > 0 ){
	  if( m_idHelperSvc->isSmallChamber(chit->first) ) state.hasSmall = true;
	  else                                          state.hasLarge = true;
	}
      }
      if( nhits.nphi != 0 && nhits.nphi == noutBounds.nphi ){
	ATH_MSG_DEBUG("   --> All phi hits out of bounds ");
	// remove phi hits for this chamber
	double fakePull = 1000.*nhits.nphi;
	if( !chambersToBeRemovedPhi.count(chit->first) ) {
	  // just add it
	  state.chambersToBeRemovedPhi.push_back( std::make_pair(fakePull,chit->first) );
	  chambersToBeRemovedPhi.insert(chit->first);
	} else {
	  // replace existing entry if new one has larger pull
	  PullChIt pIt     = state.chambersToBeRemovedPhi.begin();
	  PullChIt pIt_end = state.chambersToBeRemovedPhi.end();
	  for ( ; pIt != pIt_end; ++pIt ) {
	    if ( pIt->second == chit->first && pIt->first < fakePull ) {
	      pIt->first = fakePull;
	      break;
	    }
	  }
	}
      }
      else {
	if( noutBounds.nphi != 0 ) ATH_MSG_DEBUG("   --> Some phi hits out of bounds ");
	if( nhits.nphi > 0 ){
	  MuonStationIndex::PhiIndex phiIndex = m_idHelperSvc->phiIndex(chit->first);
	  state.phiLayers.insert(phiIndex);
	}
      }
    }

    state.nPhiConstraints = state.phiLayers.size() + state.nPseudoMeasurements;
    if( state.hasLarge && state.hasSmall ) ++state.nPhiConstraints;
    if( state.stations.size() == 1 && nphiLayers > 1 ) ++state.nPhiConstraints;

    double pull_precisionhits = -1.;
    if (state.pullSum.nhits != 0) {
      pull_precisionhits = state.pullSum.pullSum/state.pullSum.nhits;
    }
    double pull_triggerhits = -1.;
    if (state.pullSumTrigEta.nhits != 0) {
      pull_triggerhits = state.pullSumTrigEta.pullSum/state.pullSumTrigEta.nhits;
    }
    double pull_phihits = -1.;
    if (state.pullSumPhi.nhits != 0) {
	pull_phihits = state.pullSumPhi.pullSum/state.pullSumPhi.nhits;
    }
    ATH_MSG_DEBUG("    pull sum: precision hits " << pull_precisionhits
		  << " trigger eta " << pull_triggerhits
		  << "  phi " << pull_phihits
		  << "  measurements " << state.measInfo.size() << std::endl
		  << "  precision chambers " << state.pullSumPerChamber.size() 
		  << "  hits with large pull " << state.largePullMeasurements.size() 
		  << "  pseudos with large pull " << state.largePullPseudoMeasurements.size() 
		  << "  chambers to be removed " << state.chambersToBeRemoved.size()
		  << "  phi " << state.chambersToBeRemovedPhi.size()
		  << "  phi lay " << state.phiLayers.size()
		  << "  phi constr " << state.nPhiConstraints);
      
    if (state.pullSumPhi.nhits != 0) {
      double phiPull = state.pullSumPhi.pullSum/state.pullSumPhi.nhits;
      if( state.stations.size() == 1 && phiPull > 5. ){
	ATH_MSG_DEBUG(" Single station track with large phi pulls!! ");
      }
    }
    else ATH_MSG_DEBUG(" Track with no phi hits!! ");
  }

  bool MuonTrackCleaner::extractChambersToBeRemoved( CleaningState& state, std::set<Identifier>& chambersToBeRemovedSet, bool usePhi ) const {

    bool doCleaning = false;
    PullChamberMap& pullSumPerChamber=usePhi ? state.pullSumPerChamberPhi : state.pullSumPerChamber;
    PullChVec& chambersToBeRemoved=usePhi ? state.chambersToBeRemovedPhi : state.chambersToBeRemoved;

    PullChamberCit cit = pullSumPerChamber.begin();
    PullChamberCit cit_end = pullSumPerChamber.end();
    if( msgLvl(MSG::DEBUG) ) {
      if ( pullSumPerChamber.size() ) msg() << MSG::DEBUG << "Chamber pulls " << pullSumPerChamber.size() << ":";
    }
    int ndof = 0;
    double pulltot = 0.;
    for( ;cit!=cit_end;++cit ){
      double avePull = cit->second.pullSum/cit->second.nhits;
      pulltot += cit->second.pullSum;
      ndof += cit->second.nhits;  
      double avePullReduced = cit->second.nhits > 1 ? (cit->second.pullSum-cit->second.maxPull)/(cit->second.nhits-1) : avePull;
      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << std::endl << " chamber " << m_idHelperSvc->toStringChamber(cit->first) 
				       << "  pull sum " << cit->second.pullSum << " max pull " << cit->second.maxPull << " nhits " << cit->second.nhits 
				       << " ave pull " << avePull << " reduced ave pull " << avePullReduced;
      if( avePull > m_avePullSumPerChamberCut  ) {
	doCleaning = true;
	if( cit->second.nhits > 3 && avePullReduced < m_avePullSumPerChamberCut ) {
	  if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "  large pull sum ignored ";
	  doCleaning = false;
	}else{
	  if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "  large pull sum => removing";
	  if( !chambersToBeRemovedSet.count(cit->first) ) {
	    chambersToBeRemoved.push_back( std::make_pair(avePull,cit->first) );
	    chambersToBeRemovedSet.insert(cit->first);
	  }
	}
      }
    }
    if( msgLvl(MSG::DEBUG) ) msg() << endmsg;
    bool dropMore = false;
    if(dropMore&&pulltot*pulltot>2.*ndof*ndof) {
      doCleaning = true;
      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "  large pull per dof " << pulltot << " ndof " << ndof << endmsg;
    }

    if(doCleaning && m_iterate) {
      cit = pullSumPerChamber.begin();
      for( ;cit!=cit_end;++cit ){
        double avePull = cit->second.pullSum/cit->second.nhits;
        if( !chambersToBeRemovedSet.count(cit->first) ) {
          chambersToBeRemoved.push_back( std::make_pair(avePull,cit->first) );
          chambersToBeRemovedSet.insert(cit->first);
        }
      }
    }

    if(dropMore && doCleaning && m_iterate) {
// add trigger chambers
      cit = state.pullSumPerChamberEta.begin();
      cit_end = state.pullSumPerChamberEta.end();
      for( ;cit!=cit_end;++cit ){
        double avePull = cit->second.pullSum/cit->second.nhits;
        if( !chambersToBeRemovedSet.count(cit->first) ) {
          chambersToBeRemoved.push_back( std::make_pair(avePull,cit->first) );
          chambersToBeRemovedSet.insert(cit->first);
        }
      }
    }

    return doCleaning;
  }

  bool MuonTrackCleaner::isOutsideOnTrackCut( const Identifier& id, double res, double pull, double cutScaleFactor ) const {


    
    bool isMdt = id.is_valid() ? m_idHelperSvc->isMdt(id) : false;
    bool measuresPhi = id.is_valid() ? m_idHelperSvc->measuresPhi(id) : false;

    // look at pulls and identify outliers
    double pullCut = measuresPhi ? m_pullCutPhi : m_pullCut; 
    
    if( isMdt ){
      // if mdt residual cut is activated check whether the residual is small that 80% of the cut of
      if( m_useMdtResiCut ){
	if( fabs(res) > cutScaleFactor*m_mdtResiCut ) return true;
	return false;
      }else{
	if( fabs(pull) > cutScaleFactor*pullCut ) return true;
	return false;
      }
    }else{
      if( fabs(pull) > cutScaleFactor*pullCut ) return true;
      return false;
    }      
  }

  std::string MuonTrackCleaner::print( MuonTrackCleaner::ChamberLayerStatistics& statistics ) const {
    std::ostringstream sout;

    unsigned int nhits = statistics.nhits;
    unsigned int noutliers = statistics.noutliers;
    unsigned int ndeltas = statistics.ndeltas;
    unsigned int nrecoverableOutliers = statistics.nrecoverableOutliers;
    unsigned int noutBounds = statistics.noutBounds;

    sout << MuonStationIndex::chName(statistics.chIndex)
	 << " hits " << std::setw(6) << nhits
	 << " outliers " << std::setw(6) << noutliers
	 << " deltas " << std::setw(6) << ndeltas
	 << " recoverableOutliers " << std::setw(6) << nrecoverableOutliers
	 << " outBounds " << std::setw(6) << noutBounds;

    return sout.str();
  }

  bool MuonTrackCleaner::checkStations(CleaningState& state ) const {
    unsigned int nstationsChamberCleaning = state.stations.size();
    ATH_MSG_DEBUG(" Cleaner removed full station from track: remaining layers " << nstationsChamberCleaning );

    if( nstationsChamberCleaning < 2 ){
      ATH_MSG_DEBUG(" Cleaner removed all but one station from track!!! " );
      return false;
    }
    return true;
  }

  bool MuonTrackCleaner::checkInnerConstraint(CleaningState& state ) const {
    unsigned int nstations = state.stations.size();
    if( nstations == 1 ||
	(  nstations == 2 && (state.stations.count(MuonStationIndex::EM) && state.stations.count(MuonStationIndex::EO) ) ) ){
      ATH_MSG_DEBUG(" Momentum measurement lost, cleaning given up ");
      return false;
    }
    return true;
  }

  bool MuonTrackCleaner::checkPhiConstraint(CleaningState& state ) const {
    if( state.stations.size() == 1 && !state.nIdHits && state.nPhiConstraints < 2 ){
      ATH_MSG_DEBUG(" Underconstraint fit " << state.nPhiConstraints);
      return false;
    }
    return true;
  }

  void MuonTrackCleaner::unremoveHits(ChamberRemovalOutput& result) const {
    // loop over removed hits and 'unremove' them so they are used in the next iteration                                                                                                          
    for(auto hit : result.removedHits) hit->useInFit = 1;
  }

  void MuonTrackCleaner::printStates(Trk::Track* track) const{
    const DataVector<const Trk::TrackStateOnSurface>* states = track->trackStateOnSurfaces();
    int nStates = 0;
    if ( states ) nStates = states->size();
    ATH_MSG_DEBUG("Calling fit with hits: " << nStates);
    ATH_MSG_VERBOSE(m_printer->printMeasurements( *track ));
  }

  std::unique_ptr<Trk::Track> MuonTrackCleaner::fitTrack(Trk::Track& track,Trk::ParticleHypothesis pHyp,bool slFit) const{
    Trk::Track* newTrack;
    if(slFit) newTrack=m_slTrackFitter->fit(track,false,pHyp);
    else newTrack=m_trackFitter->fit(track,false,pHyp);
    if(!newTrack){
      return nullptr;
    }
    else{
      return std::unique_ptr<Trk::Track>(newTrack);
    }
  }
}
