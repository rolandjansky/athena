/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/Timeout.h" 

#include "MooTrackBuilder.h"
#include "MooTrackFitter.h"
#include "SortMuPatHits.h"
#include "MooCandidateMatchingTool.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonRecToolInterfaces/IMuonSeededSegmentFinder.h"
#include "MuPatCandidateTool.h"
#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "MuonTrackMakerUtils/SortTracksByHitNumber.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"

#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IAdjustableT0Tool.h"

#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "MuonTrackMakerUtils/MuonGetClosestParameters.h"

#include <set>

namespace Muon {

  
  MooTrackBuilder::MooTrackBuilder(const std::string& t,const std::string& n,const IInterface* p)  : 
    AthAlgTool(t,n,p),
    m_fitter("Muon::MooTrackFitter/MooTrackFitter"),
    m_slFitter("Muon::MooTrackFitter/MooSLTrackFitter"),
    m_candidateHandler("Muon::MuPatCandidateTool/MuPatCandidateTool"),
    m_candidateMatchingTool("Muon::MooCandidateMatchingTool/MooCandidateMatchingTool"),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_seededSegmentFinder("Muon::MuonSeededSegmentFinder/MuonSeededSegmentFinder"),
    m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_compRotCreator("Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"),
    m_propagator("Trk::STEP_Propagator/MuonPropagator"),
    m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
    m_hitRecoverTool("Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool"),
    m_muonChamberHoleRecoverTool("Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool"),
    m_trackExtrapolationTool("Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"),
    m_errorOptimisationTool(""),
    m_tofTool(""),
    m_magFieldSvc("AtlasFieldSvc",n),
    m_magFieldProperties(Trk::FullField),
    m_ncalls(0),
    m_nTimedOut(0)
  {

    declareInterface<IMuonSegmentTrackBuilder>(this);
    declareInterface<MooTrackBuilder>(this);
    declareInterface<IMuonTrackRefiner>(this);
    declareInterface<IMuonTrackBuilder>(this);
    
 
    declareProperty("Fitter",m_fitter,"Tool to fit segments to tracks");
    declareProperty("SLFitter",m_slFitter,"Tool to fit segments to tracks");
    declareProperty("SeededSegmentFinder",m_seededSegmentFinder,"Second stage segment finding tool");
    declareProperty("MdtRotCreator",m_mdtRotCreator,"Tool to recalibrate MdtDriftCircleOnTrack objects");
    declareProperty("CompetingClustersCreator",m_compRotCreator,"Tool to create CompetingMuonClustersOnTrack objects");
    declareProperty("Propagator",m_propagator,"propagator");
    declareProperty("TofTool",     m_tofTool);
    declareProperty("MagFieldSvc",    m_magFieldSvc );
    declareProperty("IdHelper",m_idHelper);
    declareProperty("HitRecoveryTool",m_hitRecoverTool);
    declareProperty("Helper",m_helper);
    declareProperty("Printer",m_printer);
    declareProperty("CandidateTool",m_candidateHandler);
    declareProperty("CandidateMatchingTool",m_candidateMatchingTool);
    declareProperty("TrackToSegmentTool",m_trackToSegmentTool);
    declareProperty("ErrorOptimisationTool",m_errorOptimisationTool ); 

    declareProperty("UseTimeOutGuard",m_doTimeOutChecks = true );  
    declareProperty("UseExclusionList",m_useExclusionList = true ); 
    declareProperty("UseTrackingHistory", m_useTrackingHistory = true );
    declareProperty("RecalibrateMDTHitsOnTrack", m_recalibrateMDTHits = true);
  }
    
  MooTrackBuilder::~MooTrackBuilder() {
      
  }
  
  StatusCode MooTrackBuilder::initialize() {

    ATH_CHECK( m_fitter.retrieve() );
    ATH_CHECK( m_slFitter.retrieve() );
    if( !m_tofTool.empty() ) ATH_CHECK( m_tofTool.retrieve() );
    ATH_CHECK(m_magFieldSvc.retrieve() );
    if( !m_errorOptimisationTool.empty() ) ATH_CHECK( m_errorOptimisationTool.retrieve() );
    ATH_CHECK( m_candidateHandler.retrieve() );
    ATH_CHECK( m_candidateMatchingTool.retrieve() );
    if( !m_hitRecoverTool.empty() ) ATH_CHECK( m_hitRecoverTool.retrieve() );
    ATH_CHECK( m_muonChamberHoleRecoverTool.retrieve() );
    ATH_CHECK( m_trackExtrapolationTool.retrieve() );
    ATH_CHECK( m_idHelper.retrieve() );
    ATH_CHECK( m_helper.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_trackToSegmentTool.retrieve() );
    ATH_CHECK( m_seededSegmentFinder.retrieve() );
    ATH_CHECK( m_mdtRotCreator.retrieve() );
    ATH_CHECK( m_compRotCreator.retrieve() );
    ATH_CHECK( m_propagator.retrieve() );
    ATH_CHECK( m_pullCalculator.retrieve() );

    return StatusCode::SUCCESS;
  }
  

  StatusCode MooTrackBuilder::finalize(){
    if(  m_nTimedOut > 0 && m_ncalls > 0 ) {     
      double scale = 1./m_ncalls;
      ATH_MSG_INFO(" Number of calls that timed out " << m_nTimedOut << " fraction of total calls " << scale*m_nTimedOut );
    }
    return StatusCode::SUCCESS;
  }

  Trk::Track* MooTrackBuilder::refit( const Trk::Track& track ) const {

    if( m_helper->isSLTrack(track) || !m_magFieldSvc->toroidOn() ) return m_slFitter->refit(track);
    
    // if not refit tool specified do a pure refit 
    if( m_errorOptimisationTool.empty() ) return m_fitter->refit(track);
    return m_errorOptimisationTool->optimiseErrors(track);
  }

  Trk::Track* MooTrackBuilder::refit( const MuPatTrack& trkCan ) const {

    if( !trkCan.hasMomentum() || !m_magFieldSvc->toroidOn() ) return m_slFitter->refit(trkCan);
 
    // if configured to use t0s check whether there are any segments with fitted T0
    if( !m_tofTool.empty() ){
      m_tofTool->ResetSetTShift( 0. );
      std::vector<MuPatSegment*>::const_iterator sit = trkCan.segments().begin();
      std::vector<MuPatSegment*>::const_iterator sit_end = trkCan.segments().end();
      for( ;sit!=sit_end;++sit ){
        
        // sanity checks
        if( !*sit || !(*sit)->segment ) continue;
        
        // check whether segment has T0
        if( !(*sit)->segment->hasFittedT0() ) continue;
        std::set<Identifier> chIds = m_helper->chamberIds(*(*sit)->segment);
        for( std::set<Identifier>::iterator chit = chIds.begin();chit!=chIds.end();++chit ){
          const Identifier& id = *chit;
          int ieta = m_idHelper->mdtIdHelper().stationEta(id);
          int iphi = m_idHelper->mdtIdHelper().stationPhi(id);
          m_tofTool->SetStatTShift(m_idHelper->chamberNameString(id),ieta,iphi,-(*sit)->segment->time());
          ATH_MSG_DEBUG(" Adding t0 swift: " << m_idHelper->toStringChamber(id) << "  --- " << (*sit)->segment->time() );
        }
      }
    }

    return refit(trkCan.track());
  }

  MuPatTrack* MooTrackBuilder::refine( MuPatTrack& track ) const {
    
    Trk::Track* finalTrack = m_hitRecoverTool->recover(track.track());
    if( !finalTrack ) {
      ATH_MSG_WARNING(" final track lost, this should not happen " );
      return 0;
    }
    ATH_MSG_VERBOSE("refine: after recovery " << std::endl
		    << m_printer->print(*finalTrack) << std::endl
		    << m_printer->printStations(*finalTrack) );
    

    bool recalibrateMDTHits = m_recalibrateMDTHits;
    bool recreateCompetingROTs = true;
    Trk::Track* recalibratedTrack = recalibrateHitsOnTrack(*finalTrack,recalibrateMDTHits,
							   recreateCompetingROTs);
    if( !recalibratedTrack ){
      ATH_MSG_WARNING(" failed to recalibrate hits on track " << std::endl
		      << m_printer->print(*finalTrack) );
    }else{
      if( finalTrack != &track.track() ) delete finalTrack;
      finalTrack = recalibratedTrack;
    }
          
    Trk::Track* refittedTrack = refit(*finalTrack);
    if( !refittedTrack ){
      ATH_MSG_VERBOSE(" failed to refit track " << std::endl
		      << m_printer->print(*finalTrack) << std::endl
		      << m_printer->printStations(*finalTrack) );
    }else{
      if( finalTrack != &track.track() ) delete finalTrack;
      finalTrack = refittedTrack;
    }

    // redo holes as they are dropped in the fitter
    Trk::Track* finalTrackWithHoles = m_muonChamberHoleRecoverTool->recover(*finalTrack);
    if( !finalTrackWithHoles ) {
      ATH_MSG_WARNING(" failed to add holes to final track, this should not happen " );
    }else{
      if( finalTrack != &track.track() ) delete finalTrack;
      finalTrack = finalTrackWithHoles;
    }
            
    Trk::Track* entryRecordTrack = m_trackExtrapolationTool->extrapolate( *finalTrack );
    if( entryRecordTrack ){
      if( finalTrack != &track.track() ) delete finalTrack;
      finalTrack = entryRecordTrack;
      ATH_MSG_VERBOSE(" track at muon entry record " << std::endl
		      << m_printer->print(*finalTrack) );
    }

    if( finalTrack != &track.track() ) m_candidateHandler->updateTrack( track, finalTrack );

    return &track;
  }

  MuonSegment* MooTrackBuilder::combineToSegment( const MuonSegment& seg1, const MuonSegment& seg2,
                                                  const PrepVec* externalPhiHits) const {
    
    // try to get track
    Trk::Track* track = combine( seg1, seg2, externalPhiHits );

    if( !track ) return 0;

    // create MuonSegment
    MuonSegment* seg = m_trackToSegmentTool->convert(*track);
    if( !seg ){
      ATH_MSG_WARNING( " conversion of track failed!! " );
    }
    
    // delete track
    delete track;

    return seg;
  }

  Trk::Track* MooTrackBuilder::combine( const MuonSegment& seg1, const MuonSegment& seg2, const PrepVec* externalPhiHits ) const {
    // convert segments 
    MuPatSegment* segInfo1 = m_candidateHandler->createSegInfo(seg1);
    if( !segInfo1 ) return 0;

    MuPatSegment* segInfo2 = m_candidateHandler->createSegInfo(seg2);
    if( !segInfo2 ) {
      delete segInfo1;
      return 0;
    }

    // call fit()
    Trk::Track* track = combine(*segInfo1,*segInfo2,externalPhiHits);
    delete segInfo1;
    delete segInfo2;
    
    // return result
    return track;
  }

  MuonSegment* MooTrackBuilder::combineToSegment( const MuPatCandidateBase& firstCandidate, const MuPatCandidateBase& secondCandidate, const PrepVec* externalPhiHits ) const {


    // try to get track
    Trk::Track* track = combine( firstCandidate, secondCandidate, externalPhiHits );

    if( !track ) return 0;

    // create MuonSegment
    MuonSegment* seg = m_trackToSegmentTool->convert(*track);
    if( !seg ){
      ATH_MSG_WARNING( " conversion of track failed!! " );
    }
    
    // delete track
    delete track;
    
    return seg;
  }

  Trk::Track* MooTrackBuilder::combine( const MuPatCandidateBase& firstCandidate, const MuPatCandidateBase& secondCandidate, 
                                        const PrepVec* externalPhiHits ) const {

    ++m_ncalls;

    if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) { 
      ATH_MSG_DEBUG("Timeout reached. Aborting sequence." ); 
      ++m_nTimedOut;
      return 0; 
    } 


    std::set<MuonStationIndex::StIndex> stations;
    stations.insert(firstCandidate.stations().begin(),firstCandidate.stations().end());
    stations.insert(secondCandidate.stations().begin(),secondCandidate.stations().end());
    unsigned int nstations = stations.size();
    bool slFit = nstations == 1 || ( nstations == 2 && 
                                     ( stations.count( MuonStationIndex::EM ) && 
				      ( stations.count( MuonStationIndex::BO ) || stations.count( MuonStationIndex::EO ) ) ) ); 
    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << MSG::DEBUG << " combining entries: nstations " << nstations << " types:";
      for( std::set<MuonStationIndex::StIndex>::iterator it=stations.begin(); it!=stations.end();++it ){
        msg(MSG::DEBUG) << MSG::DEBUG << "  " << MuonStationIndex::stName(*it);
      }
      if( slFit ) {
        msg(MSG::DEBUG) << " doing SL fit ";
      } else {
        msg(MSG::DEBUG) << " doing curved fit ";
      }
      msg(MSG::DEBUG) << endmsg;
    }

    const MuPatTrack* trkCan1 = dynamic_cast<const MuPatTrack*>(&firstCandidate);
    const MuPatTrack* trkCan2 = dynamic_cast<const MuPatTrack*>(&secondCandidate);

    const MuPatSegment* segCan1 = dynamic_cast<const MuPatSegment*>(&firstCandidate);
    const MuPatSegment* segCan2 = dynamic_cast<const MuPatSegment*>(&secondCandidate);

    const MuPatTrack* candidate = 0;
    const MuPatSegment*  segment = 0;
    if( trkCan1 && segCan2 ){
      candidate = trkCan1;
      segment   = segCan2;
    }else if(  trkCan2 && segCan1 ){
      candidate = trkCan2;
      segment   = segCan1;
    }

    // check whether this combination was already tried, if yes reject the combination
    if( candidate && segment ){
      ATH_MSG_DEBUG(" Track/segment combination" );
      for( std::vector<MuPatSegment*>::const_iterator esit = candidate->excludedSegments().begin();
           esit != candidate->excludedSegments().end(); ++esit ) {
        if(*esit == segment) {
          ATH_MSG_DEBUG(" Rejected segment based on exclusion list" );
          return 0;
        }
      }
    }
    
    // the following bit of code checks whether the current combination of segments was already tested
    if( m_useTrackingHistory ){
      // create a set of all segments of the would-be candidate
      std::set<const MuPatSegment*> segments;
      if( (segCan1 && segCan2) ){
	segments.insert(segCan1);
	segments.insert(segCan2);
      }
      if( candidate && segment ){
	segments.insert(segment);
	std::vector<MuPatSegment*>::const_iterator tsit = candidate->segments().begin();
	std::vector<MuPatSegment*>::const_iterator tsit_end = candidate->segments().end();
	for( ;tsit != tsit_end;++tsit ) segments.insert(*tsit);
      
      }
      //ATH_MSG_INFO(" Evaluating history, combination size " << segments.size() );
      // now loop over the segments and check if any of them is associated with a track that contains all of the segments
      std::set<const MuPatSegment*>::iterator sit = segments.begin();
      std::set<const MuPatSegment*>::iterator sit_end = segments.end();
      for( ;sit!=sit_end;++sit ){

	// loop over the tracks associated with the current segment
	std::set<MuPatTrack*>::const_iterator tit = (*sit)->tracks().begin();
	std::set<MuPatTrack*>::const_iterator tit_end = (*sit)->tracks().end();
	for( ;tit!=tit_end;++tit ){
	
	  // loop over the segments associated with the track
	  std::set<MuPatSegment*> foundSegments;
	  std::vector<MuPatSegment*>::const_iterator tsit = (*tit)->segments().begin();
	  std::vector<MuPatSegment*>::const_iterator tsit_end = (*tit)->segments().end();
	  for( ;tsit != tsit_end ; ++ tsit ) {
	    if( segments.count(*tsit) ) foundSegments.insert(*tsit);
	  }
	  //ATH_MSG_INFO(" Comparing segments with track: found segments " << foundSegments.size() );

	  // if all segments are already part of an existing track, don't perform the fit
	  if( foundSegments.size() == segments.size() ){
	    ATH_MSG_DEBUG("Combination already part of an existing track");
	    return 0;
	  }


	  // if all segments but one are already part of an existing track, check the exclusion list
	  if( candidate && !candidate->excludedSegments().empty() && foundSegments.size() == segments.size() - 1 ){
	    // create destination vector for segments that are not found
	    std::vector<const MuPatSegment*> unassociatedSegments(segments.size(),0); 
	    std::vector<const MuPatSegment*>::iterator it = std::set_difference( segments.begin(),segments.end(), 
										 foundSegments.begin(),foundSegments.end(), 
										 unassociatedSegments.begin() );
	    const MuPatSegment* zero = 0;
	    unassociatedSegments.erase(std::find(unassociatedSegments.begin(),unassociatedSegments.end(),zero),unassociatedSegments.end());

	    // check whether any pointers found
	    if( it != unassociatedSegments.begin() ){
	    
	      // this should always be one as we required the difference to be one!
	      if( unassociatedSegments.size() != 1 ) {
		ATH_MSG_DEBUG("Inconsistent result from set difference: size result " << unassociatedSegments.size() 
			      << " candidate " << segments.size() << " found " << foundSegments.size() );
		return 0;
	      }
	    
	      // check that the result is indeed part of the original set
	      if( !segments.count(unassociatedSegments.front()) ){
		ATH_MSG_DEBUG("Segment point not part of the original set, aborting!");
		return 0;
	      }
	    
	      // now check whether the segment is part of the excluded segments
	      std::vector<MuPatSegment*>::const_iterator pos =  std::find(candidate->excludedSegments().begin(), 
									   candidate->excludedSegments().end(),
									   unassociatedSegments.front() );
	      if( pos != candidate->excludedSegments().end() ){
		ATH_MSG_DEBUG("Segment found in exclusion list, not performing fit");
		return 0;
	      }
	    }
	  }
	}
      }
      //ATH_MSG_INFO(" Performing fit"  );
    }

    if( slFit || !m_magFieldSvc->toroidOn() ) return m_slFitter->fit(firstCandidate,secondCandidate,externalPhiHits);
    else                       return m_fitter->fit(firstCandidate,secondCandidate,externalPhiHits);
  }

   
  Trk::Track* MooTrackBuilder::combine( const Trk::Track& track, const MuonSegment& seg, 
                                        const PrepVec* externalPhiHits ) const {
    // convert segments 
    MuPatTrack* candidate = m_candidateHandler->createCandidate(new Trk::Track(track));
    if( !candidate ) return 0;

    MuPatSegment* segInfo = m_candidateHandler->createSegInfo(seg);
    if( !segInfo ) {
      delete candidate;
      return 0;
    }

    // call fit()
    Trk::Track* newTrack = combine(*candidate,*segInfo,externalPhiHits);
    delete candidate;
    delete segInfo;
    
    // return result
    return newTrack;
  }


  
  std::vector<Trk::Track*>* MooTrackBuilder::combineWithSegmentFinding( const Trk::Track& track, const MuonSegment& seg, 
                                                                        const PrepVec* externalPhiHits ) const {
    // convert segments 
    MuPatTrack* candidate = m_candidateHandler->createCandidate(new Trk::Track(track));
    if( !candidate ) return 0;

    MuPatSegment* segInfo = m_candidateHandler->createSegInfo(seg);
    if( !segInfo ) {
      delete candidate;
      return 0;
    }

    // call fit()
    std::vector<Trk::Track*>* tracks = combineWithSegmentFinding(*candidate,*segInfo,externalPhiHits);
    delete candidate;
    delete segInfo;
    
    // return result
    return tracks;
  }




  Trk::TrackParameters* MooTrackBuilder::findClosestParameters( const Trk::Track& track, const Amg::Vector3D& pos ) const {
    // are we in the endcap?
    bool isEndcap = m_helper->isEndcap(track);

    // position of segment
    double posSeg = isEndcap ? pos.z() : pos.perp();
    
    // position closest parameters
    double closest = 1e8;
    const Trk::TrackParameters* closestParameters = 0;
    bool closestIsMeasured = false;

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_DEBUG(" track without states! " );
      return 0;
    }
    
    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ){
        continue;
      }

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ){
        continue;
      }
      
      // check whether measured parameters
      bool isMeasured = pars->covariance();
      
      // skip all none measured TrackParameters as soon as we found one with a measurement
      if( closestIsMeasured && !isMeasured ) continue;

      // calculate position parameters and compare with position segment
      double posPars = isEndcap ? pars->position().z() : pars->position().perp();
      double diffPos = fabs(posPars - posSeg);

      // accept if measured parameters or the current accepted parameters are not yet measured
      if( (isMeasured && !closestIsMeasured) || diffPos < closest ){
        closest = diffPos;
        closestParameters = pars;
        closestIsMeasured = isMeasured;
        
        // if we are within 100 mm take current
        if( closest < 100.  ) {
          break;
        }
      }
    }
    
    // return clone of parameters
    if( closestParameters ) return closestParameters->clone();
    return 0;
  }

  Trk::TrackParameters* MooTrackBuilder::getClosestParameters( const MuPatCandidateBase& candidate, const Trk::Surface& surf ) const {
    
    // cast to segment, return segment parameters if cast success
    const MuPatSegment* segCandidate = dynamic_cast<const MuPatSegment*>(&candidate);
    if( segCandidate ) return segCandidate->entryPars().clone();

    // for a track candidate, return the closest parameter on the track
    const MuPatTrack& trkCandidate = dynamic_cast<const MuPatTrack&>(candidate);
    return getClosestParameters( trkCandidate.track(), surf );
  }

  Trk::TrackParameters* MooTrackBuilder::getClosestParameters( const Trk::Track& track, const Trk::Surface& surf ) const {
    return MuonGetClosestParameters::closestParameters(track,surf);
  }

  std::vector<Trk::Track*>* MooTrackBuilder::combineWithSegmentFinding( const Trk::Track& track,
                                                                        const Trk::TrackParameters& pars, 
                                                                        const std::set<Identifier>& chIds,
                                                                        const PrepVec* patternPhiHits ) const {
    // convert track
    MuPatTrack* can = m_candidateHandler->createCandidate(new Trk::Track(track));
    if( !can ) return 0;

    std::vector<Trk::Track*>* tracks = combineWithSegmentFinding(*can,pars,chIds,patternPhiHits);
    delete can;
    return tracks;
  }


  std::vector<Trk::Track*>* MooTrackBuilder::combineWithSegmentFinding( const MuPatTrack& candidate, 
                                                                        const MuPatSegment& segInfo, 
                                                                        const PrepVec* externalPhiHits ) const {
    

    /** second stage segment matching:
        - estimate segment parameters at segment position using fit of track + segment position
        - redo segment finding using the predicted parameters as seed
        - redo segment association
    */

    const MuonSegment& seg  = *segInfo.segment;

    // get chamber Id of segment
    std::set<Identifier> chIds = m_helper->chamberIds(seg);
    
    if( chIds.empty() ) return 0;


    // for now do not redo segment making for CSCs
    if( m_idHelper->isCsc( *chIds.begin() ) ){
      if( m_candidateMatchingTool->match(candidate,segInfo,true) ) {
	Trk::Track* newtrack = m_fitter->fit(candidate,segInfo,externalPhiHits);
	if( !newtrack ) return 0;
	std::vector<Trk::Track*>* newTracks = new std::vector<Trk::Track*>;
	newTracks->push_back(newtrack);
	return newTracks;
      }else{
	return 0;
      }
    }

    const Trk::Track& track = candidate.track();
    ATH_MSG_DEBUG(" in combineWithSegmentFinding " );
    ATH_MSG_VERBOSE(" segment " << m_printer->print(seg) );

    // find track parameters on segment surface
    Trk::TrackParameters* closestPars = findClosestParameters(track,seg.globalPosition());

    if( !closestPars ) {
      ATH_MSG_WARNING( " unable to find closest TrackParameters " );
      return 0;
    }

    ATH_MSG_VERBOSE(" closest parameter " << m_printer->print(*closestPars) );

    // propagate to segment surface
    const Trk::TrackParameters* exPars = m_propagator->propagate(*closestPars,seg.associatedSurface(),Trk::anyDirection,
                                                                 false,m_magFieldProperties);

    // free memory
    delete closestPars;

    if( !exPars ) {
      ATH_MSG_WARNING( " Propagation failed!! " );
      return 0;
    }
    
    ATH_MSG_VERBOSE(" extrapolated parameter " << m_printer->print(*exPars) );
    
    std::vector<Trk::Track*>* tracks = combineWithSegmentFinding(candidate,*exPars,chIds,externalPhiHits);
    delete exPars;

    return tracks;
  }

  std::vector< const MuonSegment*>
  MooTrackBuilder::removeDuplicateWithReference( const std::vector< const MuonSegment*>& segments,
                                                 const std::vector< const MuonSegment*>& referenceSegments ) const {

    if( segments.empty() || referenceSegments.empty() ) return std::vector< const MuonSegment*>();

    ATH_MSG_DEBUG(" Removing duplicates from segment vector of size " << segments.size() 
                  << " reference size " << referenceSegments.size() );

    CompareMuonSegmentKeys compareSegmentKeys;

    // create a vector with pairs of MuonSegmentKey and a pointer to the corresponding segment to resolve ambiguities
    std::vector< std::pair<MuonSegmentKey,const MuonSegment*> > segKeys;
    segKeys.reserve(segments.size());

    // loop over reference segments and make keys
    std::vector< const MuonSegment*>::const_iterator sit = segments.begin();
    std::vector< const MuonSegment*>::const_iterator sit_end = segments.end();
    for( ; sit!=sit_end;++sit) segKeys.push_back( std::make_pair(MuonSegmentKey(**sit),*sit) );

    // create a vector with pairs of MuonSegmentKey and a pointer to the corresponding segment to resolve ambiguities
    std::vector< std::pair<MuonSegmentKey,const MuonSegment*> > referenceSegKeys;
    referenceSegKeys.reserve(referenceSegments.size());

    // loop over reference segments and make keys
    sit = referenceSegments.begin();
    sit_end = referenceSegments.end();
    for( ; sit!=sit_end;++sit) referenceSegKeys.push_back( std::make_pair(MuonSegmentKey(**sit),*sit) );
    

    std::vector< const MuonSegment*> cleanedSegments;

      
    // loop over segments and compare the current segment with the reference ones
    std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator skit = segKeys.begin();
    std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator skit_end = segKeys.end();
    for( ;skit!=skit_end;++skit ){

      bool isDuplicate = false;

      std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator rskit = referenceSegKeys.begin();
      std::vector< std::pair<MuonSegmentKey,const MuonSegment*> >::iterator rskit_end = referenceSegKeys.end();
      for( ;rskit!=rskit_end;++rskit ){


        CompareMuonSegmentKeys::OverlapResult overlapResult = compareSegmentKeys( rskit->first, skit->first );
        if( overlapResult == CompareMuonSegmentKeys::Identical ) {
          ATH_MSG_DEBUG(" discarding identical segment" << m_printer->print(*skit->second) );
          isDuplicate = true;
          break;
        }else if( overlapResult == CompareMuonSegmentKeys::SuperSet ){
          // reference segment superset of current: discard
          ATH_MSG_DEBUG(" discarding (subset) " << m_printer->print(*skit->second) );
          isDuplicate = true;
          break;
        }
      }
      if( !isDuplicate ) cleanedSegments.push_back(skit->second);    }
    
    return cleanedSegments;
  }


  std::vector<Trk::Track*>* MooTrackBuilder::combineWithSegmentFinding( const MuPatTrack& candidate,
                                                                        const Trk::TrackParameters& pars, 
                                                                        const std::set<Identifier>& chIds,
                                                                        const PrepVec* externalPhiHits ) const {

    if( chIds.empty() )      return 0;

    if( !m_idHelper->isMdt(*chIds.begin()) ) {
      ATH_MSG_WARNING("combineWithSegmentFinding called with CSC hits!! retuning zero pointer");
      return 0;
    }

    // redo segment finding 
    std::vector<const MuonSegment*>* segments = m_seededSegmentFinder->find(pars,chIds);
 
    // check whether we got segments
    if( !segments ) {
      ATH_MSG_DEBUG(" failed to find new segments " );
      return 0;
    }    
    if( segments->empty() ){
      ATH_MSG_DEBUG(" got empty vector!! " );
      delete segments;
      return 0;      
    }


    std::vector< const MuonSegment*> cleanedSegments; 
    if( m_useExclusionList ){
      std::vector<const MuonSegment*> referenceSegments;
      referenceSegments.reserve(candidate.excludedSegments().size());
      for( std::vector<MuPatSegment*>::const_iterator esit = candidate.excludedSegments().begin();
           esit != candidate.excludedSegments().end(); ++esit ) {
        if( (*esit)->segment ) referenceSegments.push_back((*esit)->segment);
      }      
      cleanedSegments = removeDuplicateWithReference( *segments, referenceSegments ); 
    }else{ 
      cleanedSegments.insert( cleanedSegments.begin(),segments->begin(),segments->end()); 
    } 

    if( msgLvl(MSG::DEBUG) && cleanedSegments.size() != segments->size() ) {
      msg(MSG::DEBUG) << MSG::DEBUG << " Rejected segments based on exclusion list, number of removed segments: " 
                      << segments->size() - cleanedSegments.size() << " total " << segments->size() << endmsg;
    }
    
    std::vector<Trk::Track*>* newTracks = 0;
    if( !cleanedSegments.empty() ){
      
      newTracks = new std::vector<Trk::Track*>;
      
      // loop over segments
      std::vector<const MuonSegment*>::iterator sit = cleanedSegments.begin();
      std::vector<const MuonSegment*>::iterator sit_end = cleanedSegments.end();
      for( ;sit!=sit_end;++sit ){
        
        if( !*sit ) continue;
        
        if( msgLvl(MSG::DEBUG) ){
          msg(MSG::DEBUG) << MSG::DEBUG << " adding segment " << m_printer->print(**sit);
          if( msgLvl(MSG::VERBOSE) ) {
            msg(MSG::DEBUG)  << std::endl << m_printer->print((*sit)->containedMeasurements()) << endmsg;
            if( msgLvl(MSG::VERBOSE) && candidate.track().measurementsOnTrack() ) 
	      msg(MSG::DEBUG)  << " track " << m_printer->print(candidate.track()) << std::endl
			       << m_printer->print(candidate.track().measurementsOnTrack()->stdcont()) << endmsg;
          }else{
            msg(MSG::DEBUG)  << endmsg;
          }
        }
        MuPatSegment* segInfo = m_candidateHandler->createSegInfo(**sit);
        
	if( !m_candidateMatchingTool->match(candidate,*segInfo,true) ){
	  delete segInfo;
	  continue;
	}
	    
        Trk::Track* segTrack = m_fitter->fit(candidate,*segInfo,externalPhiHits);
        delete segInfo;
        
        if( !segTrack ) continue;
        
        ATH_MSG_DEBUG(" found new track " << m_printer->print(*segTrack) );
        newTracks->push_back(segTrack);
        
      }
    }

    // loop over segments
    std::vector<const MuonSegment*>::iterator sit = segments->begin();
    std::vector<const MuonSegment*>::iterator sit_end = segments->end();
    for( ;sit!=sit_end;++sit ) delete *sit;

    // free memory
    delete segments;

    if( !newTracks || newTracks->empty() ){
      delete newTracks;
      return 0;
    }
    
    ATH_MSG_DEBUG(" found new tracks for segment " << newTracks->size() );

    return newTracks;
  } 

  Trk::Track* MooTrackBuilder::recalibrateHitsOnTrack( const Trk::Track& track, bool doMdts, bool doCompetingClusters ) const {
    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_DEBUG(" track without states, discarding track " );
      return 0;
    }
    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << MSG::DEBUG << " recalibrating hits on track " << std::endl 
                      << m_printer->print(track );
      
      if( msgLvl(MSG::VERBOSE) ){
        if( track.measurementsOnTrack() ) msg(MSG::DEBUG) << std::endl << m_printer->print( track.measurementsOnTrack()->stdcont() ) << endmsg;
      }else{
        msg(MSG::DEBUG) << endmsg;
      }
      
    }
    // vector to store states, the boolean indicated whether the state was create in this routine (true) or belongs to the track (false)
    // If any new state is created, all states will be cloned and a new track will beformed from them.
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    newStates.reserve(states->size()+5);
     
    Identifier currentMdtChId;

    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // check whether state is a measurement
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) {
        newStates.push_back( std::make_pair(false,*tsit) );
        continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
        newStates.push_back( std::make_pair(false,*tsit) );
        continue;
      }

      Identifier id = m_helper->getIdentifier(*meas);
      
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelper->isMuon(id) ){
        newStates.push_back( std::make_pair(false,*tsit) );     
        continue;
      }
      
      ATH_MSG_VERBOSE(" new measurement " << m_idHelper->toString(id) );

      if( m_idHelper->isMdt(id) ) {

        if( doMdts ){
          const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
          if( !mdt ){
            ATH_MSG_WARNING( " Measurement with MDT identifier that is not a MdtDriftCircleOnTrack " );
            continue;
          }
          const Trk::RIO_OnTrack* newMdt = m_mdtRotCreator->correct( *mdt->prepRawData(), *pars );
          if( !newMdt ) {
            ATH_MSG_WARNING( " Failed to recalibrate MDT " );
            continue;
          }
          Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOSWithUpdate( **tsit,
                                                                                     newMdt, 
                                                                                     pars->clone(), 
                                                                                     (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ? 
                                                                                     Trk::TrackStateOnSurface::Outlier : Trk::TrackStateOnSurface::Measurement);
          newStates.push_back( std::make_pair(true,tsos) );     

        }else{
          newStates.push_back( std::make_pair(false,*tsit) );   
        }

      }else if( m_idHelper->isCsc(id) ) {

        newStates.push_back( std::make_pair(false,*tsit) );     
        
      }else if(  m_idHelper->isTrigger(id) ){
        
        if( doCompetingClusters ){
          tsit = insertClustersWithCompetingRotCreation( tsit, tsit_end, newStates );
        }else{
          newStates.push_back( std::make_pair(false,*tsit) );   
        }

      }else if( m_idHelper->isMM(id) || m_idHelper->issTgc(id)){
	newStates.push_back( std::make_pair(false,*tsit) );   
      }else{
        ATH_MSG_WARNING( " unknown Identifier " );
      }

    }

    ATH_MSG_DEBUG(" original track had " << states->size() 
                  << " TSOS, adding " << newStates.size() - states->size() << " new TSOS " );

    // states were added, create a new track
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve( newStates.size() );
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator nit = newStates.begin();
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator nit_end = newStates.end();
    for( ;nit!=nit_end;++nit ){
      // add states. If nit->first is true we have a new state. If it is false the state is from the old track and has to be cloned
      trackStateOnSurfaces->push_back( nit->first ? nit->second : nit->second->clone() );

      //       // check whether state is a measurement
      //       const Trk::MeasurementBase* meas = (nit)->second->measurementOnTrack();
      //       if( !meas ) {
      //        continue;
      //       }

      //       Identifier id = m_helper->getIdentifier(*meas);
    }
    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0 );
    
    return newTrack;
  }


  DataVector<const Trk::TrackStateOnSurface>::const_iterator 
  MooTrackBuilder::insertClustersWithCompetingRotCreation( DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit,
                                                           DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end,
                                                           std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states
                                                           ) const {
    // iterator should point to a valid element
    if( tsit == tsit_end ) {
      ATH_MSG_WARNING( " iterator pointing to end of vector, this should no happen " );
      return --tsit;
    }

    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if( !meas  || !pars ){
      ATH_MSG_WARNING( " iterator pointing to a TSOS without a measurement or TrackParameters " );
      if( tsit+1 == tsit_end ) --tsit;
      return tsit;
    }

    ATH_MSG_VERBOSE(" inserting with competing ROT creation " );

    // loop over states until we reached the last tgc hit in this detector element
    // keep trackof the identifiers and the states
    std::list<const Trk::PrepRawData*> etaPrds;
    std::list<const Trk::PrepRawData*> phiPrds;
    const Trk::TrkDetElementBase* currentDetEl = 0;
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    // keep track of outliers as we might have to drop them..
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > outlierStates;
    bool hasPhi = false;
    bool hasEta = false;

    for( ;tsit!=tsit_end;++tsit ){
      
      if( !*tsit ) continue;
      
      // check whether state is a measurement, keep if not
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
        newStates.push_back( std::make_pair(false,*tsit) );
        continue;
      }

      // get identifier, keep state if it has no identifier.
      Identifier id = m_helper->getIdentifier(*meas);
      if( !id.is_valid() ) {
        newStates.push_back( std::make_pair(false,*tsit) );
        continue;
      }


      // sanity check, this SHOULD be a RPC, TGC or CSC measurement
      if( !( m_idHelper->isTrigger(id) ) ){
        break;
      }

      bool measuresPhi = m_idHelper->measuresPhi(id);
      if( !hasPhi && measuresPhi )  hasPhi = true;
      if( !hasEta && !measuresPhi ) hasEta = true;

      // check whether state is a measurement
      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) {  
        outlierStates.push_back( std::make_pair(measuresPhi,*tsit) );
        continue;
      }

      
      //       // get chamber Id
      //       Identifier detElId =  m_idHelper->detElId(id);
      //       if( !detectorElId ) detectorElId = new Identifier( detElId );

      //       // check whether we are still in the same chamber, stop loop if not
      //       if( detElId != *detectorElId ) break;
      // check whether we are still in the same chamber, stop loop if not

      ATH_MSG_VERBOSE(" handling " << m_idHelper->toString(id) );

      std::list<const Trk::PrepRawData*>& prdList = measuresPhi ? phiPrds : etaPrds;
      const MuonClusterOnTrack* clus = dynamic_cast<const MuonClusterOnTrack*>(meas);
      if( clus ){
        const Trk::TrkDetElementBase* detEl = clus->detectorElement();
        if( !currentDetEl ) currentDetEl = detEl;
        if( detEl != currentDetEl ){
          ATH_MSG_VERBOSE(" new detector element stopping " );
          break;
        }
        prdList.push_back(clus->prepRawData());
      }else{
        // split competing ROTs into constituents
        const CompetingMuonClustersOnTrack* comp = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
        if( comp ) {

          const Trk::TrkDetElementBase* detEl = 0; 
          if( comp->containedROTs().empty() ) {
            ATH_MSG_WARNING( " CompetingROT without constituents "  );
            break;
          }
          detEl = comp->containedROTs().front()->detectorElement();
          if( !currentDetEl ) currentDetEl = detEl;
          if( detEl != currentDetEl ) {
            ATH_MSG_VERBOSE(" new detector element stopping " );
            break;
          }       
          std::vector<const MuonClusterOnTrack*>::const_iterator clit = comp->containedROTs().begin();
          std::vector<const MuonClusterOnTrack*>::const_iterator clit_end = comp->containedROTs().end();
          for( ;clit!=clit_end;++clit ) {
            prdList.push_back((*clit)->prepRawData());
          }
          
        }else{
          ATH_MSG_WARNING( " Unknown trigger hit type! "  );
          continue;
        }
      }
    }
  
    // now that we have the lists of prds we can create the competing rots
    if( !etaPrds.empty() ){
      const CompetingMuonClustersOnTrack* etaCompRot = m_compRotCreator->createBroadCluster(etaPrds,0.);
      if( !etaCompRot ){
        ATH_MSG_WARNING( " Failed to create CompetingMuonClustersOnTrack for eta hits! "  );
      }else{

        const Trk::TrackParameters* etaPars = 0;
        // check whether original parameters are on surface, if so clone original parameters
        if( etaCompRot->associatedSurface() == pars->associatedSurface() ){
          etaPars = pars->clone();
        }else{
          etaPars = m_propagator->propagate(*pars,etaCompRot->associatedSurface(),Trk::anyDirection,
                                            false,m_magFieldProperties);
        }
        if( !etaPars ){
          ATH_MSG_WARNING( " Failed to calculate TrackParameters for eta hits! "  );
          delete etaCompRot;
        }else{
          Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOS( etaCompRot, etaPars, Trk::TrackStateOnSurface::Measurement );
          newStates.push_back( std::make_pair(true,tsos) );
        }
      }
    }
    
    if( !phiPrds.empty() ){
      const CompetingMuonClustersOnTrack* phiCompRot = m_compRotCreator->createBroadCluster(phiPrds,0.);
      if( !phiCompRot ){
        ATH_MSG_WARNING( " Failed to create CompetingMuonClustersOnTrack for phi hits! "  );
      }else{
        
        const Trk::TrackParameters* phiPars = 0;
        // check whether original parameters are on surface, if so clone original parameters
        if(phiCompRot->associatedSurface() == pars->associatedSurface() ){
          phiPars = pars->clone();
        }else{
          phiPars = m_propagator->propagate(*pars,phiCompRot->associatedSurface(),Trk::anyDirection,
                                            false,m_magFieldProperties);
        }
        if( !phiPars ){
          ATH_MSG_WARNING( " Failed to calculate TrackParameters for phi hits! "  );
          delete phiCompRot;
        }else{
          Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOS( phiCompRot, phiPars, Trk::TrackStateOnSurface::Measurement );
          newStates.push_back( std::make_pair(true,tsos) );
        }
      }
    }

    // add outliers if there was no measurement on track in the same projection
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator outIt = outlierStates.begin();
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator outIt_end = outlierStates.end();
    for( ;outIt!=outIt_end;++outIt ){
      if( hasPhi && outIt->first )  newStates.push_back( std::make_pair(false,outIt->second) );
      else if( hasEta && !outIt->first ) newStates.push_back( std::make_pair(false,outIt->second));
      else if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " Dropping outlier " << endmsg;
    }

    
    // sort all states in this chamber
    std::stable_sort(newStates.begin(),newStates.end(),SortTSOSByDistanceToPars(pars) );

    // insert the states into 
    states.insert( states.end(),newStates.begin(),newStates.end() );

    // iterator should point to the last TGC in this chamber
    return --tsit;
  }

  std::pair<Trk::Track*,Trk::Track*> MooTrackBuilder::splitTrack( const Trk::Track& track ) const {
    return m_helper->isSLTrack(track) || !m_magFieldSvc->toroidOn() ?  m_slFitter->splitTrack(track) : m_fitter->splitTrack(track);    
  }

  std::vector<MuPatTrack*>* MooTrackBuilder::find( MuPatCandidateBase& candidate, const std::vector<MuPatSegment*>& segVec ) const {

    // check whether we have segments
    if( segVec.empty() ) return 0;

    std::vector<MuPatTrack*>* candidates = new std::vector<MuPatTrack*>();
    std::set<MuPatSegment*> usedSegments;
    std::map<MuPatSegment*,MuPatSegment*> slSegments;

    //int looseQualityLevel = 1; // Not used for the moment
    bool tightQualityCuts = false;
    ATH_MSG_DEBUG(" find: " << m_candidateHandler->print(candidate,0) 
                  << std::endl << m_candidateHandler->print(segVec,0) );

    // store whether segment was added to at least one candidates 
      
    // vector to store candidate extensions
    std::vector< std::pair<MuPatSegment*,Trk::Track*> > extensions;
    extensions.reserve( segVec.size() );
      
    // loop over segments
    std::vector<MuPatSegment*>::const_iterator sit = segVec.begin();
    std::vector<MuPatSegment*>::const_iterator sit_end = segVec.end();
    for( ;sit!=sit_end;++sit ){
        
      if( usedSegments.count(*sit) ) continue;

      // check whether chamber is already included in candidate
      if( candidate.shareChambers(**sit) ) {
        ATH_MSG_VERBOSE("addStationToSeed:: already on candidate " << std::endl
                        << m_printer->print(*(*sit)->segment) );
        continue;
      }

      if( !m_candidateMatchingTool->match(candidate,**sit,tightQualityCuts) ) {
        ATH_MSG_VERBOSE(" track/segment combination rejected based on angular matching " << std::endl
                        << m_printer->print(*(*sit)->segment) );          
        continue;
      }

      ATH_MSG_VERBOSE("combining: " << m_printer->print(*(*sit)->segment) );

      // try to combine track with segment
      Trk::Track* track = combine(candidate,**sit,0);
      
      // additional check in case the candidate is a MuPatTrack
      MuPatTrack* trkCan = dynamic_cast<MuPatTrack*>(&candidate);
      MuPatSegment* segCan = dynamic_cast<MuPatSegment*>(&candidate);
      if( trkCan ) {

        if( !track ) {
          trkCan->addExcludedSegment(*sit);
          continue;
        }

        // is the new track better
        SortTracksByHitNumber sortTracks;
        if( !sortTracks(*track,trkCan->track()) ){
          ATH_MSG_VERBOSE(" rejecting track as new segment results in worse fit" );
          delete track;
          continue;
        }

        // check whether the track cleaner didn't remove one of the already added chamber layers
        // loop over hits
        std::set<MuonStationIndex::StIndex> stationLayersOnTrack;
        std::vector<const Trk::MeasurementBase*>::const_iterator mit = track->measurementsOnTrack()->stdcont().begin();
        std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = track->measurementsOnTrack()->stdcont().end();
        for( ;mit!=mit_end;++mit ){
          
          const Trk::MeasurementBase* meas = *mit;
          
          Identifier id = m_helper->getIdentifier(*meas);
          if( !id.is_valid() || m_idHelper->isTrigger(id) ) {
            continue;
          }
          
          stationLayersOnTrack.insert(m_idHelper->stationIndex(id));
        }
        
        bool hasAllLayers = true;
        std::set<MuonStationIndex::StIndex>::iterator stIt = candidate.stations().begin();
        std::set<MuonStationIndex::StIndex>::iterator stIt_end = candidate.stations().end();
        for( ;stIt!=stIt_end;++stIt ) {
          if( !stationLayersOnTrack.count(*stIt) ){
            ATH_MSG_VERBOSE(" missing layer " << MuonStationIndex::stName(*stIt) );
            hasAllLayers = false;
          }
        }
        
        if( !hasAllLayers ) {
          ATH_MSG_VERBOSE(" rejecting track as one of the chamber layers of the candidate was removed " );
          delete track;
          continue;
        }
      }
    
      if( !track ) {
        continue;
      }

      
      usedSegments.insert(*sit);

      // now loop over segments once more and try to add SL overlap if missed
      // first check that segment is not an overlap segment
      if( !(*sit)->hasSLOverlap() ){
	
	MuPatTrack* newCandidate = 0;
	// loop over segments
	std::vector<MuPatSegment*>::const_iterator sit1 = segVec.begin();
	std::vector<MuPatSegment*>::const_iterator sit1_end = segVec.end();
	for( ;sit1!=sit1_end;++sit1 ){
	  
	  // select segments is different chamber
	  if( (*sit)->chIndex == (*sit1)->chIndex ) continue;
	  
	  if( !newCandidate ) {
	    if( trkCan ){
	      // copy candidate and add segment
	      newCandidate = new MuPatTrack(*trkCan);
	      m_candidateHandler->extendWithSegment(*newCandidate,**sit,track);
	    }else if( segCan ){
	      newCandidate = m_candidateHandler->createCandidate( *segCan,**sit,track );
	    }	    
	    if( !newCandidate ) break;
	  }
	  
	  if( !m_candidateMatchingTool->match(*newCandidate,**sit1,tightQualityCuts) ) {
	    ATH_MSG_VERBOSE("track/segment combination rejected based on angular matching " << std::endl
			    << m_printer->print(*(*sit)->segment) );          
	    continue;
	  }
	  ATH_MSG_VERBOSE("adding SL overlap " << m_printer->print(*(*sit1)->segment) );
	  Trk::Track* slOverlapTrack = combine( *track, *(*sit1)->segment );
	  if( !slOverlapTrack ) continue;
	  
	  // is the new track better
	  SortTracksByHitNumber sortTracks;
	  if( !sortTracks(*slOverlapTrack,*track) ){
	    ATH_MSG_VERBOSE(" rejecting track as new segment results in worse fit" );
	    delete slOverlapTrack;
	    continue;
	  }
	  ATH_MSG_VERBOSE("adding SL overlap ok, new track" << m_printer->print(*slOverlapTrack) << std::endl << m_printer->printStations(*slOverlapTrack) );
	  
	  // delete candidate, this will also delete 'track'
	  delete newCandidate;
	  newCandidate = 0;
	  track = slOverlapTrack;
	  usedSegments.insert(*sit1);
	  slSegments[*sit] = *sit1;
	  break;
	}
	if( newCandidate ){
	  newCandidate->releaseTrack();
	  delete newCandidate;
	}
      }

      ATH_MSG_VERBOSE(" Track found " << m_printer->print(*track) );

      // add new solution
      extensions.push_back( std::make_pair(*sit,track) );
        

    } // for (sit)
 
    // loop over solutions and add them
    if ( extensions.size() >= 1 ) {
 
      candidates->reserve(extensions.size());

      // additional check in case the candidate is a MuPatTrack
      MuPatTrack* trkCan = dynamic_cast<MuPatTrack*>(&candidate);
      MuPatSegment* segCan = dynamic_cast<MuPatSegment*>(&candidate);
      
      std::vector< std::pair<MuPatSegment*,Trk::Track*> >::iterator eit = extensions.begin();
      // if more than 1 extensions are found, first add the copies
      std::vector< std::pair<MuPatSegment*,Trk::Track*> >::iterator eit_end = extensions.end();
      // start from the second one to make copies based on the existing candidates
      for( ; eit!=eit_end; ++eit ){
        MuPatTrack* newCandidate = 0;
        if( trkCan ){
          // copy candidate and add segment
          newCandidate = new MuPatTrack(*trkCan);
          m_candidateHandler->extendWithSegment(*newCandidate,*eit->first,eit->second);
        }else if( segCan ){
          newCandidate = m_candidateHandler->createCandidate( *segCan,*eit->first,eit->second );
        }
        ATH_MSG_DEBUG(" " << m_printer->print(*eit->first->segment) );
        MuPatSegment* slOverlap = slSegments[eit->first];
	if( slOverlap ){
	  ATH_MSG_DEBUG("SLOverlap " << m_printer->print(*slOverlap->segment) );
	  // hack to allow me to add a second segment without copying the track
          // Coverity 13708 keep this 0 call with 0 pointer
	  newCandidate->addSegment(slOverlap,0);
	}
	candidates->push_back(newCandidate);
        if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " creating new candidate " << newCandidate
						 << std::endl << m_printer->print(newCandidate->track())
						 << std::endl << m_printer->printStations(newCandidate->track()) << endmsg;
      } // for( ; eit!=eit_end; ++eit )
    }else{
      //ATH_MSG_INFO("Failed extension" );
    }
    if( candidates->empty() ) {
      delete candidates;
      candidates = 0;
    }
    return candidates;
  }

  bool MooTrackBuilder::isSplitTrack( const Trk::Track& track1, const Trk::Track& track2 ) const {

    // some loose association cuts 
    const DataVector<const Trk::TrackParameters>* parsVec1 = track1.trackParameters();
    if( !parsVec1 || parsVec1->empty() ){
      ATH_MSG_WARNING(" isSplitTrack::Track without parameters! " );
      return false;
    }
    const Trk::TrackParameters* pars1 = parsVec1->front();
    if( !pars1 ){
      ATH_MSG_WARNING(" isSplitTrack::Track without NULL pointer in parameter vector! " );
      return false;
    }

    const DataVector<const Trk::TrackParameters>* parsVec2 = track2.trackParameters();
    if( !parsVec2 || parsVec2->empty() ){
      ATH_MSG_WARNING(" isSplitTrack::Track without parameters! " );
      return false;
    }
    const Trk::TrackParameters* pars2 = parsVec2->front();
    if( !pars2 ){
      ATH_MSG_WARNING(" isSplitTrack::Track without NULL pointer in parameter vector! " );
      return false;
    }
    
    if( !m_candidateMatchingTool->sameSide( pars1->momentum().unit(),pars1->position(),pars2->position(),true) ){
      ATH_MSG_DEBUG(" tracks in opposite hemispheres " );
      return false; 
    }
    
    double sinTheta1 = sin(pars1->momentum().theta());
    double sinTheta2 = sin(pars2->momentum().theta());
    double deltaSinTheta = sinTheta1 - sinTheta2;
    if( fabs(deltaSinTheta) > 1. ) {
      ATH_MSG_DEBUG(" too large opening angle in theta " << deltaSinTheta );
      //return false;
    }
    double sinPhi1 = sin(pars1->momentum().phi());
    double sinPhi2 = sin(pars2->momentum().phi());
    double deltaSinPhi = sinPhi1 - sinPhi2;
    if( fabs(deltaSinPhi) > 1. ){
      ATH_MSG_DEBUG(" too large opening angle in phi " << deltaSinPhi );
      //return false;
    }
    

    const Trk::Track* referenceTrack = 0;
    const Trk::Track* otherTrack = 0;

    // first check whether the tracks have a momentum measurement
    bool isSL1 = m_helper->isSLTrack(track1);
    bool isSL2 = m_helper->isSLTrack(track2);
    
    // now decide which track to use as reference
    if( isSL1 && !isSL2 ) {
      referenceTrack = &track2;
      otherTrack     = &track1;
    }else if( !isSL1 && isSL2 ){
      referenceTrack = &track1;
      otherTrack     = &track2;
    }else{
      SortTracksByHitNumber sortTracks;
      bool pickFirst = sortTracks(track1,track2);
      if( pickFirst ){
        referenceTrack = &track1;
        otherTrack     = &track2;
      }else{
        referenceTrack = &track2;
        otherTrack     = &track1;
      }
    }

    ATH_MSG_DEBUG(" close tracks " << std::endl
                  << m_printer->print(*referenceTrack) << std::endl
                  << m_printer->print(*otherTrack) );

    
    // get iterators to TSOSs
    const DataVector<const Trk::TrackStateOnSurface>* statesRef = referenceTrack->trackStateOnSurfaces();
    if( !statesRef ){
      ATH_MSG_WARNING(" track without states, cannot perform cleaning " );
      return false;
    }
    DataVector<const Trk::TrackStateOnSurface>::const_iterator refTSOS = statesRef->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator refTSOS_end = statesRef->end();

    const DataVector<const Trk::TrackStateOnSurface>* statesOther = otherTrack->trackStateOnSurfaces();
    if( !statesOther ){
      ATH_MSG_WARNING(" track without states, cannot perform cleaning " );
      return false;
    } 
    DataVector<const Trk::TrackStateOnSurface>::const_iterator otherTSOS = statesOther->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator otherTSOS_end = statesOther->end();

    DistanceAlongParameters distAlongPars;
    
    unsigned int nmatching(0);
    unsigned int noff(0);


    // keep track of previous distance and parameters as well
    double prevDist = 1e10;
    const Trk::TrackParameters* prevPars = 0;

    // now loop over the TSOSs of both tracks and compare hit by hit
    while( refTSOS != refTSOS_end && otherTSOS != otherTSOS_end ){


      const Trk::TrackParameters* parsRef = (*refTSOS)->trackParameters();
      if( !parsRef ){
        ++refTSOS;
        continue;
      }

      const Trk::TrackParameters* parsOther = (*otherTSOS)->trackParameters();
      if( !parsOther ){
        ++otherTSOS;
        continue;
      }

      double dist = distAlongPars( *parsRef, *parsOther );
      //std::cout << " distance between pars " << dist << std::endl;
      
      if( dist > 0. ){
        //std::cout << " moving reference " << m_printer->print(*(*refTSOS)->trackParameters()) << std::endl;
        prevDist = dist;
        prevPars = parsRef;
        ++refTSOS;
        continue;
      }else{
        
        const Trk::TrackParameters* closestPars = 0;
        if( prevPars && fabs(prevDist) < fabs(dist) ){
          //std::cout << " using previous parameters " << std::endl;
          closestPars = prevPars;
        }else{
          closestPars = parsRef;
        }

        // check whether state is a measurement
        const Trk::MeasurementBase* meas = (*otherTSOS)->measurementOnTrack();
        if( meas && (*otherTSOS)->type(Trk::TrackStateOnSurface::Measurement) ){        
          
          Identifier id = m_helper->getIdentifier(*meas);
          // skip pseudo measurements
          if( !id.is_valid() ) {
            prevDist = dist;
            prevPars = parsRef;
            ++otherTSOS;
            continue;
          }
          if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << m_idHelper->toString(id);

          const Trk::TrackParameters* impactPars = m_propagator->propagate(*closestPars,meas->associatedSurface(),Trk::anyDirection,
                                                                           false,m_magFieldProperties);
          if( impactPars ) {

            double residual = 1e10;
            double pull     = 1e10;
            // pointer to resPull
            const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( meas, impactPars, Trk::ResidualPull::Unbiased );
            if( resPull && resPull->pull().size() == 1 ) {
              if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "  residual " << m_printer->print(*resPull);
              residual = resPull->residual().front();
              pull     = resPull->pull().front();
              delete resPull;
            }else{
              ATH_MSG_WARNING("failed to calculate residual and pull" );
            }

            bool inBounds = false;
            Amg::Vector2D locPos;
	    bool ok = meas->associatedSurface().globalToLocal(impactPars->position(),impactPars->momentum(),locPos);
            delete impactPars;
            if( ok ){
              if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "  lpos (" << locPos[Trk::locX] << "," << locPos[Trk::locY] << ")";
              double tol1 = 50.;
              double tol2 = tol1;
              Identifier id = m_helper->getIdentifier(*meas);
              if( msgLvl(MSG::VERBOSE) && m_idHelper->isMdt(id) ) {
                const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
                if( mdt ){
                  int layer = m_idHelper->mdtIdHelper().tubeLayer(id);
                  int tube = m_idHelper->mdtIdHelper().tube(id);
                  double halfTubeLen = 0.5*mdt->detectorElement()->getActiveTubeLength(layer,tube);
                  if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "  range " << halfTubeLen;
                }
                //tol1 = 0;
              }
              inBounds = meas->associatedSurface().insideBounds(locPos,tol1,tol2);
              if( msgLvl(MSG::VERBOSE) ){
                if( inBounds ) msg(MSG::VERBOSE) << " inBounds ";
                else           msg(MSG::VERBOSE) << " outBounds ";
              }
            }else{
              ATH_MSG_WARNING("globalToLocal failed" );
            }
            
            if( inBounds && ( fabs(residual) < 20. || fabs(pull) < 10. ) ){
              ATH_MSG_VERBOSE(" --> matching " );
              ++nmatching;
            }else{
              ATH_MSG_VERBOSE(" --> off " );
              ++noff;
            }

          }else{
            ATH_MSG_DEBUG("failed to extrapolate parameters to surface" );
          }
          

        }

        //std::cout << " moving other forward " << std::endl;
        prevDist = dist;
        prevPars = parsRef;
        ++otherTSOS;
        continue;
      }
    }

    // if more hits are compatible with reference track than are not consider as split track
    if( nmatching > noff ) return true;

    return false;
  }
  
  TrackCollection* MooTrackBuilder::mergeSplitTracks( const TrackCollection& tracks ) const {

    // vector to store good track, boolean is used to identify whether the track was created in this routine or is from the collection
    std::vector< std::pair<bool,const Trk::Track*> > goodTracks;
    goodTracks.reserve(tracks.size());
    bool foundSplitTracks = false;
   
    ATH_MSG_DEBUG(" trying to merge split tracks, collection size " << tracks.size() );

    // loop over tracks
    TrackCollection::const_iterator tit = tracks.begin();
    TrackCollection::const_iterator tit_end = tracks.end();
    for( ;tit!=tit_end;++tit ){

      // pointer to merged track
      Trk::Track* mergedTrack = 0;

      // compare them to all good tracks and look for split tracks
      std::vector< std::pair<bool,const Trk::Track*> >::iterator git = goodTracks.begin();
      std::vector< std::pair<bool,const Trk::Track*> >::iterator git_end = goodTracks.end();
      for( ;git!=git_end;++git ){
        // check whether track is split
        bool isSplit = isSplitTrack( *git->second, **tit );
        if( isSplit ){

          // if we found a potential split track, try to combine them
          MuPatTrack* can1 = m_candidateHandler->createCandidate(new Trk::Track(*git->second));
          MuPatTrack* can2 = m_candidateHandler->createCandidate(new Trk::Track(**tit));
          
          mergedTrack = combine(*can1,*can2);
          delete can1;
          delete can2;

          // we have found a split track and have successfully merged it
          // replace the track in goodTracks with the new one
          if( mergedTrack ){
            ATH_MSG_DEBUG(" origninal tracks " << std::endl
                          << m_printer->print(*git->second) << std::endl << m_printer->printStations(*git->second) << std::endl
                          << m_printer->print(**tit) << std::endl << m_printer->printStations(**tit) << std::endl
                          << " merged track " << std::endl 
                          << m_printer->print(*mergedTrack) << std::endl << m_printer->printStations(*mergedTrack) );
            foundSplitTracks = true;
            // check whether this is a new track, if so delete the old one before overwriting it
            if( git->first ) {
              ATH_MSG_VERBOSE(" input track already result of a merge, deleting it" );
              delete git->second;
            }
            git->first = true;
            git->second = mergedTrack;
            break;
          }else{
            ATH_MSG_VERBOSE(" failed to merge tracks " << std::endl
                            << m_printer->print(*git->second) << std::endl << m_printer->printStations(*git->second) << std::endl
                            << m_printer->print(**tit) << std::endl << m_printer->printStations(**tit) );
          }
        }
      }
      
      // if this track was not merged with another track insert it into goodTracks
      if( !mergedTrack ){
        goodTracks.push_back( std::make_pair(false,*tit) );
      }
    }

    // did we find any?
    if( !foundSplitTracks ) return 0;
    // loop over the new track vector and create a new TrackCollection
    TrackCollection* newTracks = new TrackCollection();
    newTracks->reserve(goodTracks.size());
    std::vector< std::pair<bool,const Trk::Track*> >::iterator git = goodTracks.begin();
    std::vector< std::pair<bool,const Trk::Track*> >::iterator git_end = goodTracks.end();
    for( ;git!=git_end;++git ){
      // check whether we are dealing with a newly created track
      if( git->first ) { 
        // cast away constness before inserting track
        Trk::Track* theTrack = const_cast<Trk::Track*>(git->second);
        newTracks->push_back(theTrack);
      }else{
        newTracks->push_back( new Trk::Track(*git->second));
      }
    }
    return newTracks;
  }

}
