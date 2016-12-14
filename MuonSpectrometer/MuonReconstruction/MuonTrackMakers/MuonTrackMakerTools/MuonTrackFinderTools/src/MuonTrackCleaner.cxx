/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonTrackCleaner.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkToolInterfaces/IUpdator.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"

#include <iostream>

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"

#include "TrkTrack/Track.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "StoreGate/StoreGateSvc.h"

namespace Muon {

  MuonTrackCleaner::MuonTrackCleaner(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_trackFitter("Trk::GlobalChi2Fitter/MCTBFitterMaterialFromTrack"),			      
      m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitterMaterialFromTrack"),
      m_fitter(0),
      m_measurementUpdator("Trk::KalmanUpdator/MuonMeasUpdator"),
      m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
      m_compRotCreator("Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"),
      m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
      m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_magFieldSvc("AtlasFieldSvc",na),
      m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"), 
      m_slFit(false) {
    declareInterface<IMuonTrackCleaner>(this);

    declareProperty("IdHelper",m_idHelper);
    declareProperty("Helper",m_helper);
    declareProperty("Printer",m_printer);
    declareProperty("MdtRotCreator",  m_mdtRotCreator );
    declareProperty("CompRotCreator", m_compRotCreator );
    declareProperty("PullCalculator", m_pullCalculator );
    declareProperty("Extrapolator",   m_extrapolator );
    declareProperty("MagFieldSvc",    m_magFieldSvc );
    declareProperty("MeasurementUpdator",   m_measurementUpdator );
    declareProperty("Chi2Cut",        m_chi2Cut = 100);
    declareProperty("PullCutPhi",     m_pullCutPhi = 10);
    declareProperty("PullCut",        m_pullCut = 5);
    declareProperty("MdtResiCut",     m_mdtResiCut = 1.);
    declareProperty("AssociationScaleFactor", m_associationScaleFactor = 0.7 );
    declareProperty("UseMdtResiCut",  m_useMdtResiCut = false );
    declareProperty("Fitter",         m_trackFitter);
    declareProperty("SLFitter",       m_slTrackFitter);
    declareProperty("CleaningCycles", m_ncycles = 5);
    declareProperty("MaxAvePullSumPerChamber", m_avePullSumPerChamberCut = 3.5);
    declareProperty("RecoverOutliers", m_recoverOutliers = true );
    declareProperty("FlipMdtDriftRadii",m_flipMdtDriftRadii = true );
    declareProperty("CleanCompROTs",  m_cleanCompROTs=true);
    declareProperty("OnlyUseHitErrorInRecovery", m_onlyUseHitErrorInRecovery = true );
    declareProperty("AdcCut", m_adcCut = 50 );
    declareProperty("Iterate", m_iterate = true );
  }


  MuonTrackCleaner::~MuonTrackCleaner(){}


  StatusCode MuonTrackCleaner::initialize()
  {
    
    if ( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;
    
    if (m_trackFitter.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not find refit tool "<<m_trackFitter<<". Exiting." );
      return StatusCode::FAILURE;
    }
    if (m_slTrackFitter.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not find refit tool "<<m_slTrackFitter<<". Exiting.");
      return StatusCode::FAILURE;
    }
    
    if (m_idHelper.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_idHelper); 
      return StatusCode::FAILURE;
    }

    if (m_helper.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_helper); 
      return StatusCode::FAILURE;
    }

    if (m_printer.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_printer); 
      return StatusCode::FAILURE;
    }
    
    if (m_extrapolator.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not find "<< m_extrapolator);
      return StatusCode::FAILURE;
    }
    
    if (m_pullCalculator.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_pullCalculator); 
      return StatusCode::FAILURE;
    }

    if (m_mdtRotCreator.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not find refit tool "<<m_mdtRotCreator<<". Exiting.");
      return StatusCode::FAILURE;
    }

    if( m_compRotCreator.retrieve().isFailure() ){
      ATH_MSG_ERROR(" failed to retrieve " << m_compRotCreator );
      return StatusCode::FAILURE;
    }

    if (m_measurementUpdator.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_measurementUpdator); 
      return StatusCode::FAILURE;
    }

    if (m_magFieldSvc.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_magFieldSvc); 
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonTrackCleaner::finalize()
  {


    double norm = m_counters.nclean != 0 ? 1./m_counters.nclean : 1.;
    double normHit = m_counters.nhitCleaning != 0 ? 1./m_counters.nhitCleaning : 1.;


    msg() << MSG::INFO << " Summarizing statistics: ncleans " << m_counters.nclean << " successfull " << m_counters.nsuccess*norm << std::endl
	  << " of which " << m_counters.nfitsSL*norm << " SL, " << m_counters.nfitsMom*norm << " with momentum, " 
	  << m_counters.nvertexConstraints*norm << " with IP constraint and with ID hits " << m_counters.nIdTracks*norm << std::endl
	  << " Chamber cleaning: Total " << std::setw(8) << m_counters.nchCleaning*norm << " Failed " << m_counters.nfailedChamberCleaning*norm << std::endl
	  << "   Only Two Chambers     " << m_counters.nchTwoChambers*norm << std::endl
	  << "   All in exclusion list " << m_counters.nchExclusionList*norm << std::endl
	  << "   One station left      " << m_counters.nchOneStationLeft*norm << std::endl
	  << "   No result found       " << m_counters.nchAllLost*norm << std::endl
	  << "   Momentum lost         " << m_counters.nchMomLoss*norm << std::endl
	  << " MDT sign flipping       " << std::setw(8) << m_counters.nflipMdt*norm 
	  << " Failed " << m_counters.nflipMdtFailed*norm << std::endl
	  << " CompROT recovery        " << std::setw(8) << m_counters.ncleanComp*norm 
	  << " Failed " << m_counters.ncleanCompFailed*norm << std::endl
	  << " Hit cleaning            " << std::setw(8) << m_counters.nhitCleaning*norm << " Failed " << m_counters.nfailedHitCleaning*norm << std::endl
	  << "   One station left      " << m_counters.nhitOneChamberLeft*norm << std::endl
	  << "   Momentum lost         " << m_counters.nhitMomLoss*norm << std::endl
	  << "   Too many outliers     " << m_counters.nhitTooManyOutliers*norm << std::endl
	  << "   Too few hits left     " << m_counters.nhitTooFewHits*norm << std::endl
	  << "   Failed fit            " << m_counters.nhitFitFailed*norm << std::endl
	  << "   End of cycles         " << m_counters.nhitEndOffCycle*norm << std::endl
	  << "   Ave number of cycles  " << m_counters.nhitTotCycles*normHit << std::endl 
	  << " Outlier cut             " << m_counters.noutlierCut*norm << endmsg;

    cleanUp();
    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
  Trk::Track* MuonTrackCleaner::clean( Trk::Track& track, const std::set<Identifier>& chamberRemovalExclusionList ) const {
    m_chamberRemovalExclusionList = chamberRemovalExclusionList;
    
    if( !m_chamberRemovalExclusionList.empty() ) {
      ATH_MSG_DEBUG(" Cleaning with exclusion list " << m_chamberRemovalExclusionList.size() );
    }

    Trk::Track* cleanedTrack = clean(track);

    m_chamberRemovalExclusionList.clear();
    
    return cleanedTrack;
  }
  

  Trk::Track* MuonTrackCleaner::clean( Trk::Track& track ) const {
    
    // wrapper to simplify cleanup of memory
    Trk::Track* cleanedTrack = cleanTrack(track);
    
    // remove return track from list of to be deleted tracks
    if( cleanedTrack && cleanedTrack != &track ) {
      m_trackToBeDeleted.erase(cleanedTrack);
    }
    cleanUpTracks();
    return cleanedTrack;
  }

  Trk::Track* MuonTrackCleaner::cleanTrack( Trk::Track& track ) const {
    
    ++m_counters.nclean;

    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " Perform cleaning for track " << std::endl
				     << m_printer->print( track ) << std::endl;

    
    init(track);

    if( m_slFit ) ++m_counters.nfitsSL;
    else          ++m_counters.nfitsMom;

    ATH_MSG_DEBUG("  start cleaning " );

    //     // check whether this is an endcap with only EM EO but momentum, if so do not apply momentum loss cut
    //     bool endcapEM_EO_withMomentum = false;
    //     if( m_stations.size() == 2 && (m_stations.count(MuonStationIndex::EM) && m_stations.count(MuonStationIndex::EO) ) && !m_slFit ) {
    //       if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "  EM/EO track with momentum " );
    //       endcapEM_EO_withMomentum = true;
    //     }   

    unsigned int nstationsInitial = m_stations.size();

    // first clean up chambers
    Trk::Track* chamberTrack = chamberCleaning(track);
    if(!chamberTrack) {
      ATH_MSG_DEBUG(" chamber removal failed " );
      ++m_counters.nfailedChamberCleaning;
      return 0;
    }
    
    // reinitialize cleaner
    unsigned int nstationsChamberCleaning = m_stations.size();
    if( nstationsInitial != nstationsChamberCleaning ){
      ATH_MSG_DEBUG(" Cleaner removed full station from track: remaining layers " << nstationsChamberCleaning );
      
      if( nstationsChamberCleaning < 2 ){
	ATH_MSG_DEBUG(" Cleaner removed all but one station from track!!! " );
	++m_counters.nchOneStationLeft;
	return 0;
      }
    }

    // if performed curved fit and we did not have a vertex constraint or ID hits, reject the track if inner was removed
    if( !m_slFit && !(m_hasVertexConstraint || m_nIdHits > 0 ) ) {
      unsigned int nstations = m_stations.size();
      if( nstations == 1 || 
	  (  nstations == 2 && (m_stations.count(MuonStationIndex::EM) && m_stations.count(MuonStationIndex::EO) ) ) ){
	ATH_MSG_DEBUG(" Momentum measurement lost, cleaning given up ");
	++m_counters.nchMomLoss;
	return 0;
      } 
    }


    // if performing a single station layer cleaning without ID hits, reject track if there are insufficient phi constraints
    if( m_stations.size() == 1 && !m_nIdHits && m_nPhiConstraints < 2 ){
      ATH_MSG_DEBUG(" Underconstraint fit " << m_nPhiConstraints);
      
      return 0;
    }

    // recover MDTs with flipped signs
    Trk::Track* cleanCompTrack = cleanCompROTs(*chamberTrack);
    if(!cleanCompTrack) {
      ATH_MSG_DEBUG(" CompROT cleaning failed " );
      ++m_counters.ncleanCompFailed;
      return 0;
    }

    // recover MDTs with flipped signs
    Trk::Track* flippedTrack = recoverFlippedMdt(*cleanCompTrack);
    if(!flippedTrack) {
      ATH_MSG_DEBUG(" MDT sign flipping failed " );
      ++m_counters.nflipMdtFailed;
      return 0;
    }


    Trk::Track* hitTrack = hitCleaning(*flippedTrack);
    
    if( !hitTrack ){
      ATH_MSG_DEBUG(" track lost after outlier removal " );
      ++m_counters.nfailedHitCleaning;
      return 0;
    }

    // reinitialize cleaner
    if( hitTrack != chamberTrack ) {
      unsigned int nstationsHitCleaning = m_stations.size();
      if( nstationsInitial != nstationsHitCleaning ){
	ATH_MSG_DEBUG(" Cleaner removed full station from track: remaining layers " << nstationsHitCleaning );
	
	if( nstationsHitCleaning < 2 ){
	  ATH_MSG_DEBUG(" Cleaner removed all but one station from track!!! " );
	  ++m_counters.nhitOneChamberLeft;
	  return 0;
	}
      }
    }

    // if performed curved fit and we did not have a vertex constraint or ID hits, reject the track if inner was removed
    if( !m_slFit && !(m_hasVertexConstraint || m_nIdHits > 0 ) ) {
      unsigned int nstations = m_stations.size();
      if( nstations == 1 || 
	  (  nstations == 2 && (m_stations.count(MuonStationIndex::EM) && m_stations.count(MuonStationIndex::EO) ) ) ){
	ATH_MSG_DEBUG(" Momentum measurement lost, given up ");
	++m_counters.nhitMomLoss;
	return 0;
      } 
    }
  
    // if performing a single station layer cleaning without ID hits, reject track if there are insufficient phi constraints
    if( m_stations.size() == 1 && !m_nIdHits && m_nPhiConstraints < 2 ){
      ATH_MSG_DEBUG(" Underconstraint fit " << m_nPhiConstraints);
      
      return 0;
    }

    Trk::Track* cleanedTrack = outlierRecovery(*hitTrack);
    // do not discard tracks that fail outlierRecovery, check that the track is ok
    //note that this also performs a useful check on the quality of the cleaning in general
    if( !cleanedTrack || !m_chambersToBeRemoved.empty() || !m_largePullMeasurements.empty() ) {
      init(*hitTrack);
      if(!m_chambersToBeRemoved.empty() || !m_largePullMeasurements.empty()){
        ATH_MSG_DEBUG("Outlier recovery failure unrecoverable, reject track");
        return 0;
      }
      else{
        ATH_MSG_DEBUG("Outlier recovery failed but initial track is recoverable");
        cleanedTrack = hitTrack;
      }
    }
    
    if( m_nhits < m_noutliers ){
      ATH_MSG_DEBUG(" track rejected due to high outlier ratio: hits " 
		    << m_nhits << "   outliers " << m_noutliers);
      ++m_counters.noutlierCut;
      return 0;
    }

    unsigned int nstationsFinal = m_stations.size();
    if( nstationsInitial != nstationsFinal ){
      ATH_MSG_DEBUG(" Cleaner removed full station from track: remaining layers " << nstationsFinal );
	
      if( nstationsFinal < 2 ){
	ATH_MSG_DEBUG(" Cleaner removed all but one station from track!!! " );
	++m_counters.nhitOneChamberLeft;
	return 0;
      }
    }

    if( msgLvl(MSG::VERBOSE) ) {
      if( cleanedTrack == &track ){
	msg() << MSG::VERBOSE << " track unchanged " << endmsg;
      }else{
	msg() << MSG::VERBOSE << " new track " << m_printer->print( *cleanedTrack ) << std::endl
	      << m_printer->printStations( *cleanedTrack ) <<endmsg;
      }
    }


    ++m_counters.nsuccess;

    return cleanedTrack;
  }

  Trk::Track* MuonTrackCleaner::cleanCompROTs( Trk::Track& track ) const {
    
    if( !m_cleanCompROTs || m_numberOfCleanedCompROTs == 0 ) return &track;

    const Trk::Perigee* perigee = track.perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return 0;
    }

    ATH_MSG_DEBUG(" Trying to recover clean Comp rots " << m_numberOfCleanedCompROTs );
    
    ++m_counters.ncleanComp;
    
    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( m_measInfo.size() );

    unsigned int nmeas = 0;
    // loop over hits
    InfoIt hit = m_measInfo.begin();
    InfoIt hit_end = m_measInfo.end();
    for( ;hit!=hit_end;++hit){

      // hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
      if( !hit->useInFit ){
	ATH_MSG_DEBUG("   removing hit " << m_idHelper->toString(hit->id) 
		      << " pull " << hit->resPull->pull().front() );
	if( hit->inBounds ) tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState, 
										  *hit->meas,
										  *hit->pars, 
										  Trk::TrackStateOnSurface::Outlier) );

	continue;
      }else{
	if( hit->meas ) ++nmeas;
	  

	if( hit->cleanedCompROT ) {
	  tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								*hit->cleanedCompROT,
								*hit->pars, 
								Trk::TrackStateOnSurface::Measurement) );
	  ATH_MSG_DEBUG("   replacing CompROT " << m_idHelper->toString(hit->id) 
			<< " pull " << hit->resPull->pull().front() );
	}else{
	  tsos->push_back( hit->originalState->clone() );
	}
      }
    }

    if( nmeas < 6 ){
      ATH_MSG_DEBUG(" too few hits, cannot recover CompROTS " );
      delete tsos;
      return 0;
    }

    // create new track
    Trk::Track* cleanedTrack = new Trk::Track( track.info(), tsos, track.fitQuality() ? track.fitQuality()->clone():0 );

    if ( msgLvl(MSG::DEBUG) ) {
      const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
      int nStates = 0;
      if ( states ) nStates = states->size();
      msg() << MSG::DEBUG << "cleanCompROTs: calling fit with hits: " << nStates;
      if ( msgLvl(MSG::VERBOSE) ) {
	msg() << MSG::VERBOSE << std::endl << m_printer->printMeasurements( track );
      }
      msg() << endmsg;
    }

    // fit new track
    Trk::Track* newTrack = m_fitter->fit(*cleanedTrack,false,track.info().particleHypothesis());
    if( newTrack ) {
      m_trackToBeDeleted.insert( newTrack );
      init(*newTrack);
    }
    // delete input
    delete cleanedTrack;

    
    return newTrack;
  }

  Trk::Track* MuonTrackCleaner::recoverFlippedMdt( Trk::Track& track ) const {
    
    if( !m_flipMdtDriftRadii || m_numberOfFlippedMdts == 0 ) return &track;

    const Trk::Perigee* perigee = track.perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return 0;
    }

    ATH_MSG_DEBUG(" Trying to flip MDT signs: total number of hits with wrong sign " << m_numberOfFlippedMdts );
    
    ++m_counters.nflipMdt;
    
    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( m_measInfo.size() );

    unsigned int nmeas = 0;
    // loop over hits
    InfoIt hit = m_measInfo.begin();
    InfoIt hit_end = m_measInfo.end();
    for( ;hit!=hit_end;++hit){

      // hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
      if( !hit->useInFit ){
	ATH_MSG_DEBUG("   removing hit " << m_idHelper->toString(hit->id) 
		      << " pull " << hit->resPull->pull().front() );
	if( hit->inBounds ) tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
										  *hit->meas,
										  *hit->pars,
										  Trk::TrackStateOnSurface::Outlier) );

	continue;
      }else{
	if( hit->meas ) ++nmeas;
	  

	if( hit->flippedMdt ) {
	  tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								*hit->flippedMdt, 
								*hit->pars,
								Trk::TrackStateOnSurface::Measurement) );
	  ATH_MSG_DEBUG("   flipping sign hit " << m_idHelper->toString(hit->id) 
			<< " pull " << hit->resPull->pull().front() );
	}else{
	  tsos->push_back( hit->originalState->clone() );
	}
      }
    }

    if( nmeas < 6 ){
      ATH_MSG_DEBUG(" too few hits, cannot flip MDT hit sign " );
      delete tsos;
      return 0;
    }

    // create new track
    Trk::Track* cleanedTrack = new Trk::Track( track.info(), tsos, track.fitQuality() ? track.fitQuality()->clone():0 );

    if ( msgLvl(MSG::DEBUG) ) {
      const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
      int nStates = 0;
      if ( states ) nStates = states->size();
      msg() << MSG::DEBUG << "recoverFlippedMdt: calling fit with hits: " << nStates;
      if ( msgLvl(MSG::VERBOSE) ) {
	msg() << MSG::VERBOSE << std::endl << m_printer->printMeasurements( track );
      }
      msg() << endmsg;
    }

    // fit new track
    Trk::Track* newTrack = m_fitter->fit(*cleanedTrack,false,track.info().particleHypothesis());
    if( newTrack ) {
      m_trackToBeDeleted.insert( newTrack );
      init(*newTrack);
    }
    // delete input
    delete cleanedTrack;

    return newTrack;
  }

  Trk::Track* MuonTrackCleaner::hitCleaning( Trk::Track& track ) const {

    if( m_largePullMeasurements.empty() ) return &track;
    ATH_MSG_DEBUG(" trying outlier removal " );

    Trk::Track* currentTrack = &track;

    ++m_counters.nhitCleaning;
    
    for( unsigned int n=0;n<m_ncycles;++n ){
      
      ++m_counters.nhitTotCycles;

      // sanity check, should not remove too many hits
      if( m_largePullMeasurements.size() > 10 ) {
	ATH_MSG_DEBUG(" Too many outlier, cannot perform cleaning " );
	++m_counters.nhitTooManyOutliers;
	return 0;
      }

      ATH_MSG_VERBOSE(" outlier removal cycle " << n );

      const Trk::Perigee* perigee = currentTrack->perigeeParameters();
      if( !perigee ){
	ATH_MSG_DEBUG("   track without perigee " );
	return 0;
      }

      //    std::vector<const Trk::MeasurementBase*> hits;
      DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
      tsos->reserve( m_measInfo.size() );

      unsigned int nmeas = 0;
      unsigned int nremovedPhi = 0;
      bool hasSmall = false;
      bool hasLarge = false;
      MCTBCleaningInfo* firstPhi = 0;
      MCTBCleaningInfo* lastPhi = 0;
      std::map<MuonStationIndex::StIndex, std::pair<bool,bool> > slCountsPerStationLayer;
      // loop over hits
      InfoIt hit = m_measInfo.begin();
      InfoIt hit_end = m_measInfo.end();
      for( ;hit!=hit_end;++hit){
	
	bool remove = m_largePullMeasurements.count(hit->meas) || !hit->inBounds || hit->isNoise;
	// hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
	if( !hit->useInFit || remove ){
	  hit->useInFit = 0;

	  // count number of phi outliers
	  if( !hit->id.is_valid() || m_idHelper->measuresPhi(hit->id) ) ++nremovedPhi;

	  if( msgLvl(MSG::DEBUG) && remove ) {
	    msg() << MSG::DEBUG << "   removing hit " << m_idHelper->toString(hit->id) 
		  << " pull " << hit->resPull->pull().front();
	    if( hit->inBounds ) msg() << " inBounds" << endmsg;
	    else                msg() << " outBounds" << endmsg;
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
	}else{

	  if( msgLvl(MSG::DEBUG) && hit->resPull ) msg() << MSG::DEBUG << "   keeping hit " << m_idHelper->toString(hit->id) 
							   << " pull " << hit->resPull->pull().front() << endmsg;
	  
	  if( hit->meas ) {
	    ++nmeas;
	    if( !hit->id.is_valid() || m_idHelper->measuresPhi(hit->id)){
	      if( !firstPhi )     firstPhi = &(*hit);
	      else                lastPhi  = &(*hit);
	    }

	    if( hit->id.is_valid() && m_idHelper->isMdt(hit->id) ){
	      MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex(hit->chIndex);
	      bool isSmall = m_idHelper->isSmallChamber(hit->id);
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

	  tsos->push_back( hit->originalState->clone() );
	}
      }
      
      // loop over sl map and count the overlaps
      unsigned int noverlaps = 0;
      std::map<MuonStationIndex::StIndex, std::pair<bool,bool> >::iterator mapIt = slCountsPerStationLayer.begin();
      std::map<MuonStationIndex::StIndex, std::pair<bool,bool> >::iterator mapIt_end = slCountsPerStationLayer.end();
      for( ;mapIt!=mapIt_end;++mapIt ){
	if( !hasSmall && mapIt->second.first )  hasSmall = true;
	if( !hasLarge && mapIt->second.second ) hasLarge = true;
	if( mapIt->second.first && mapIt->second.second ) ++noverlaps;
      }
      // now update for the case there are small and large chambers but not in the same layer
      if( noverlaps == 0 && hasSmall && hasLarge ) ++noverlaps;

      if( nmeas < 6 ){
	ATH_MSG_DEBUG(" too few hits, cannot perform hitCleaning " );
	delete tsos;
	++m_counters.nhitTooFewHits;
	return 0;
      }

      if( msgLvl(MSG::DEBUG) ){
	msg() << MSG::DEBUG << " nremovedPhi " << nremovedPhi << " noverlaps " << noverlaps << " nid " << m_nIdHits; 
	if( firstPhi ) msg() << " hasFirstPhi: " << m_idHelper->toString(firstPhi->id);
	if( lastPhi )  msg() << " hasLastPhi: " << m_idHelper->toString(lastPhi->id);
	msg() << endmsg;
      }
      // only perform check on phi constraints if any phi hits were removed
      if( nremovedPhi > 0 ){
	bool hasPhiConstraint = false;
	
	if( m_nIdHits > 0 )                  hasPhiConstraint = true; // ok if ID hits
	else if( firstPhi && noverlaps > 0 ) hasPhiConstraint = true; // ok if one phi hit + one overlap
	else if( noverlaps > 1 )             hasPhiConstraint = true; // ok if two overlaps
	else if( firstPhi && lastPhi && firstPhi->pars && lastPhi->pars ){
	  double distPhi = fabs((firstPhi->pars->position() - lastPhi->pars->position()).dot(firstPhi->pars->momentum().unit()));
	  ATH_MSG_DEBUG(" Distance between phi hits " << distPhi );
	  if( distPhi > 450. ) hasPhiConstraint = true;
	}
	if( !hasPhiConstraint ){
	  msg() << MSG::DEBUG << "Lost phi constraint during track cleaning, rejection track" << endmsg;
	  delete tsos;
	  ++m_counters.nhitTooFewHits;
	  return 0;
	}
      }

      // create new track
      Trk::Track* cleanedTrack = new Trk::Track( track.info(), tsos, track.fitQuality() ? track.fitQuality()->clone():0 );
      
      // fit new track
      if ( msgLvl(MSG::DEBUG) ) {
	const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
	int nStates = 0;
	if ( states ) nStates = states->size();
	msg() << MSG::DEBUG << "hitCleaning: Calling fit with hits: " << nStates;
	if ( msgLvl(MSG::VERBOSE) ) {
	  msg() << MSG::VERBOSE << std::endl << m_printer->printMeasurements( *cleanedTrack );
	}
	msg() << endmsg;
      }
      Trk::Track* newTrack = m_fitter->fit(*cleanedTrack,false,track.info().particleHypothesis());

      // delete input
      delete cleanedTrack;
 
      if( !newTrack ) {
	++m_counters.nhitFitFailed;
	return 0;
      }else{
	m_trackToBeDeleted.insert(newTrack);
	// reinitialize cleaner
	init(*newTrack);
      }

      if( m_largePullMeasurements.empty() ) {
	ATH_MSG_DEBUG("   cleaning ended successfully after cycle " << n );
	return newTrack;
      }
      currentTrack = newTrack;
    }
    
    ++m_counters.nhitEndOffCycle;
    
    return 0;
  }



  Trk::Track* MuonTrackCleaner::chamberCleaning( Trk::Track& track ) const {
    

    if( m_chambersToBeRemoved.empty() && m_chambersToBeRemovedPhi.empty() ) return &track;

    ++m_counters.nchCleaning;

    if( m_pullSumPerChamber.size() == 2 ) {
      
      // loop over chambers that should be removed, if one of them is MDT or CSC don't clean
      // as this would result in a single chamber track
      PullChIt chit = m_chambersToBeRemoved.begin();
      PullChIt chit_end = m_chambersToBeRemoved.end();
      for( ;chit!=chit_end;++chit ){
	const Identifier& chid = chit->second;
	if( m_idHelper->isMdt(chid) || m_idHelper->isCsc(chid) ){
	  ATH_MSG_DEBUG(" only two precision chambers, cannot remove chamber.  " );
	  ++m_counters.nchTwoChambers;
	  return 0;
	}
      }
    }

    if( !m_chamberRemovalExclusionList.empty() ){

      unsigned int foundChambers = 0;
      // loop over chambers that should be removed and check if they are in the exclusion list
      PullChIt chit = m_chambersToBeRemoved.begin();
      PullChIt chit_end = m_chambersToBeRemoved.end();
      for( ;chit!=chit_end;++chit ){
	const Identifier& chid = chit->second;
	if( m_idHelper->isMdt(chid) && m_chamberRemovalExclusionList.count(chid) ){
	  ATH_MSG_DEBUG(" found excluded chamber " << m_idHelper->toStringChamber(chid) );
	  ++foundChambers;
	}
      }
      unsigned int excludedChambers = m_chamberRemovalExclusionList.size();
      if( foundChambers > excludedChambers ) {
	ATH_MSG_WARNING(" Found more excluded chambers than in list, this should not happen " );
	++m_counters.nchExclusionList;
	return 0;
      }else if( foundChambers == excludedChambers ){
	ATH_MSG_DEBUG(" all excluded chambers in removal list, failing cleaning " );
	++m_counters.nchExclusionList;
	return 0;
      }      
    }

    std::vector<ChamberRemovalOutput> cleaningResults;
    cleaningResults.reserve(m_chambersToBeRemoved.size());


    // TODO first remove hits in both eta and phi


    ATH_MSG_DEBUG(" number of chambers short listed to be removed: " << m_chambersToBeRemoved.size() );
    std::stable_sort( m_chambersToBeRemoved.begin(), m_chambersToBeRemoved.end(), SortByAvePull() );

    // try all combinations of removing a chamber with eta hits
    unsigned int nchambers = m_chambersToBeRemoved.size() + m_chambersToBeRemovedPhi.size();
    PullChIt chit = m_chambersToBeRemoved.begin();
    PullChIt chit_end = m_chambersToBeRemoved.end();
    for( ;chit!=chit_end;++chit ){
    
      ChamberRemovalOutput result = removeChamber( track, chit->second, false, true );
      Trk::Track* newtrack = result.track;
      if( !newtrack ) {
	if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " Removed eta hits of " << m_idHelper->toStringChamber( chit->second ) << ", track lost " << std::endl;
	continue;
      }
      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " Removed eta hits of " << m_idHelper->toStringChamber( chit->second ) << std::endl
				       << m_printer->print( *newtrack ) << std::endl;

      cleaningResults.push_back(result);
      
      // this is an optimization for the common case where there is only one chamber to be removed
      if( nchambers > 1 ){
	// loop over removed hits and 'unremove' them so they are used in the next iteration
	std::vector<MCTBCleaningInfo*>::iterator hit = result.removedHits.begin();
	std::vector<MCTBCleaningInfo*>::iterator hit_end = result.removedHits.end();
	for( ;hit!=hit_end;++hit ) (*hit)->useInFit = 1;
      }
    } // for (chit)

    // hack copy code instead of making function, to be cleaned up later
    ATH_MSG_DEBUG(" number of phi chambers short listed to be removed: " << m_chambersToBeRemoved.size() );
    std::stable_sort( m_chambersToBeRemovedPhi.begin(), m_chambersToBeRemovedPhi.end(), SortByAvePull() );
    // try all combinations of removing a chamber with phi hits
    chit = m_chambersToBeRemovedPhi.begin();
    chit_end = m_chambersToBeRemovedPhi.end();
    for( ;chit!=chit_end;++chit ){
   
      ChamberRemovalOutput result = removeChamber( track, chit->second, true, false );
      Trk::Track* newtrack = result.track;
      if( !newtrack ) {
	if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " Removed phi hits of " << m_idHelper->toStringChamber( chit->second ) << ", track lost " << std::endl;
	continue;
      }
      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " Removed phi hits of " << m_idHelper->toStringChamber( chit->second ) << std::endl
				       << m_printer->print( *newtrack ) << std::endl;

      cleaningResults.push_back(result);
      
      // this is an optimization for the common case where there is only one chamber to be removed
      if( nchambers > 1 ){
	// loop over removed hits and 'unremove' them so they are used in the next iteration
	std::vector<MCTBCleaningInfo*>::iterator hit = result.removedHits.begin();
	std::vector<MCTBCleaningInfo*>::iterator hit_end = result.removedHits.end();
	for( ;hit!=hit_end;++hit ) (*hit)->useInFit = 1;
      }
    }


    // if no cleaned tracks return 0
    if( cleaningResults.empty() ) {
      ++m_counters.nchAllLost;
      return 0;
    }
    std::stable_sort( cleaningResults.begin(),cleaningResults.end(),SortChamberRemovalResultByChi2Ndof());
    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " chamberCleaning Results nr " << cleaningResults.size() << endmsg;
    std::vector<ChamberRemovalOutput>::iterator itt= cleaningResults.begin();
    std::vector<ChamberRemovalOutput>::iterator itt_end= cleaningResults.end();
    for( ;itt!=itt_end;++itt ) {
       if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " track " << m_printer->print(*(*itt).track ) << endmsg;
    }

    ChamberRemovalOutput& finalResult = cleaningResults.front();
    Trk::Track* newtrack = finalResult.track;
    if( nchambers > 1 ){
      // loop over removed hits and redo 'remove' 
      std::vector<MCTBCleaningInfo*>::iterator hit = finalResult.removedHits.begin();
      std::vector<MCTBCleaningInfo*>::iterator hit_end = finalResult.removedHits.end();
      for( ;hit!=hit_end;++hit ) (*hit)->useInFit = 0;
    }
    
    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " chamberCleaning:  track " << std::endl << m_printer->print( *newtrack ) << endmsg;

    init(*newtrack);
    
    // now finally check whether the removed layer now is recoverable (happens sometimes if the segment has one or more bad hits)
    MuonStationIndex::ChIndex removedChamberIndex = m_idHelper->chamberIndex(finalResult.chId);
    Trk::Track* recoveredTrack = outlierRecovery(*newtrack,&removedChamberIndex);
    if( !recoveredTrack || recoveredTrack == newtrack ) return newtrack;
    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " outlierRecovery successfully " << endmsg;
    m_trackToBeDeleted.erase(newtrack);
    delete newtrack;
    init(*recoveredTrack);
    return recoveredTrack;
  }
  


  MuonTrackCleaner::ChamberRemovalOutput MuonTrackCleaner::removeChamber( Trk::Track& track, Identifier chId, bool removePhi, bool removeEta ) const {
    
    ATH_MSG_DEBUG(" removing chamber " << m_idHelper->toStringChamber(chId) );

    // store result
    ChamberRemovalOutput result;
    result.chId = chId;
    const Trk::Perigee* perigee = track.perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return result;
    }
    
    
    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( m_measInfo.size() );

    unsigned int nmeas = 0;
    // loop over hits
    InfoIt hit = m_measInfo.begin();
    InfoIt hit_end = m_measInfo.end();
    for( ;hit!=hit_end;++hit){

      if( hit->id.is_valid() ) {
        bool measuresPhi = m_idHelper->measuresPhi(hit->id);
        bool remove = hit->chId == chId && ( (removePhi && measuresPhi) || (removeEta && !measuresPhi) );
        // hits that are flagged as outlier or hits in the chamber to be removed are added as Outlier
        if( !hit->useInFit || remove ){
          hit->useInFit = 0;
          if( msgLvl(MSG::DEBUG) && remove ) msg() << MSG::DEBUG << "   removing hit " << m_idHelper->toString(hit->id) 
						   << " pull " << hit->resPull->pull().front() << endmsg;
          // add as outlier
          if( hit->inBounds ) tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
                                                                                    *hit->meas,
                                                                                    *hit->pars,
                                                                                    Trk::TrackStateOnSurface::Outlier) );
          
          // if removed, add hit to vector of hits 
          if( remove ) result.removedHits.push_back(&*hit);
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
    Trk::Track* cleanedTrack = new Trk::Track( track.info(), tsos, track.fitQuality() ? track.fitQuality()->clone():0 );
    
    // fit new track
    if ( msgLvl(MSG::DEBUG) ) {
      const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
      int nStates = 0;
      if ( states ) nStates = states->size();
      msg() << MSG::DEBUG << "removeChamber: Calling fit with hits: " << nStates;
      if ( msgLvl(MSG::VERBOSE) ) {
	msg() << MSG::VERBOSE << std::endl << m_printer->printMeasurements( track );
      }
      msg() << endmsg;
      if( !cleanedTrack->perigeeParameters() ){
        ATH_MSG_DEBUG("   track without perigee " );
      }
    }
    Trk::Track* newTrack = m_fitter->fit(*cleanedTrack,false,track.info().particleHypothesis());
    if( newTrack && !newTrack->perigeeParameters() ){
      ATH_MSG_DEBUG("   fitted track without perigee " << newTrack << " " << newTrack->perigeeParameters() );
    }

    result.track = newTrack;
    if( newTrack ) {
      m_trackToBeDeleted.insert( result.track );
    }
    // delete input
    delete cleanedTrack;

    // return result
    return result;
  }


    Trk::Track* MuonTrackCleaner::outlierRecovery( Trk::Track& track, MuonStationIndex::ChIndex* currentIndex ) const {
    

    const Trk::Perigee* perigee = track.perigeeParameters();
    if( !perigee ){
      ATH_MSG_DEBUG("   track without perigee " );
      return 0;
    }

    if( msgLvl(MSG::VERBOSE) ){
      msg() << MSG::VERBOSE << "  outlierRecovery: ";
      if( currentIndex ) msg() << MSG::VERBOSE << " layer " << MuonStationIndex::chName(*currentIndex);
      msg() << MSG::VERBOSE << "  printing chamber statistics "<< std::endl;
      std::map<MuonStationIndex::ChIndex,ChamberLayerStatistics>::iterator chit = m_chamberLayerStatistics.begin();
      std::map<MuonStationIndex::ChIndex,ChamberLayerStatistics>::iterator chit_end = m_chamberLayerStatistics.end();
      for( ;chit!=chit_end; ++chit ){
	if( chit->first == MuonStationIndex::ChUnknown ) continue;
	msg() << print(chit->second) << std::endl;
      }
      msg() << endmsg;
    }

    std::set<MuonStationIndex::ChIndex> recoverableLayers;
    std::map<MuonStationIndex::ChIndex,ChamberLayerStatistics>::iterator chit = m_chamberLayerStatistics.begin();
    std::map<MuonStationIndex::ChIndex,ChamberLayerStatistics>::iterator chit_end = m_chamberLayerStatistics.end();
    for( ;chit!=chit_end; ++chit ){
      if( chit->first == MuonStationIndex::ChUnknown ) continue;
      
      // skip all chamber layers except the requested one
      if( currentIndex && *currentIndex != chit->first ) continue;

      ChamberLayerStatistics& statistics = chit->second;
      unsigned int nhits = statistics.nhits;
      unsigned int noutliers = statistics.noutliers;
      //unsigned int ndeltas = statistics.ndeltas;
      unsigned int nrecoverableOutliers = statistics.nrecoverableOutliers;
      unsigned int noutBounds = statistics.noutBounds;
      
      if( nrecoverableOutliers > 0 ){
	if( nhits + nrecoverableOutliers > 2 && 
	    ((noutBounds == 0 && noutliers == 0) || (nrecoverableOutliers != 0 && noutliers < 2) ) ) {
	  recoverableLayers.insert(chit->first);
	  ATH_MSG_DEBUG("   found recoverable layer " << MuonStationIndex::chName(statistics.chIndex) );
	}
      }
    }

    // if there is nothing to be done exit
    if( recoverableLayers.empty() && !m_hasOfBoundsOutliers ) return &track;
    bool addedHits = false;
    unsigned int removedOutOfBoundsHits(0);

    DataVector<const Trk::TrackStateOnSurface>* tsos = new DataVector<const Trk::TrackStateOnSurface>();
    tsos->reserve( m_measInfo.size() );

    // loop over hits
    InfoIt hit = m_measInfo.begin();
    InfoIt hit_end = m_measInfo.end();
    for( ;hit!=hit_end;++hit){

      if( !hit->useInFit ){
	if( hit->inBounds ){
	  if( recoverableLayers.count(hit->chIndex) ) {

	    // check whether we can savely add hits in this chamber to the track
	    bool recover =  !isOutsideOnTrackCut( hit->id, hit->residual,hit->pull,m_associationScaleFactor ) ? true : false;
	    if( recover && m_onlyUseHitErrorInRecovery && hit->pars ){
	      const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( hit->meas, hit->pars, Trk::ResidualPull::HitOnly );
	      if( !resPull ) {
		ATH_MSG_DEBUG(" calculation of residual/pull failed !!!!! " );
		recover = false;
	      }else{
		recover = !isOutsideOnTrackCut( hit->id, resPull->residual().front(), fabs(resPull->pull().front()),m_associationScaleFactor ) ? true : false;
		delete resPull;
	      }
	    }
	    if(  recover ) {
	      if( msgLvl(MSG::DEBUG) ) {
		msg() << MSG::DEBUG << "   adding outlier " << m_idHelper->toString(hit->id) 
		      << " pull " << std::setw(7) << hit->pull;
		if( hit->flippedMdt ) {
		  double rDrift = hit->meas->localParameters()[Trk::locR];
		  double rDriftFlip = hit->flippedMdt->localParameters()[Trk::locR];
		  double rTrack = hit->pars->parameters()[Trk::locR];
		  msg() << " flipped MDT: r_orig " << rDrift << " flip " << rDriftFlip << " rTrack " << rTrack;
		}
		msg() << endmsg;
	      }
	      addedHits = true;
	      const Trk::MeasurementBase* newMeas = hit->flippedMdt ? hit->flippedMdt : hit->meas;
	      
	      tsos->push_back( MuonTSOSHelper::cloneTSOSWithUpdate( *hit->originalState,
								    *newMeas, 
								    *hit->pars,
								    Trk::TrackStateOnSurface::Measurement) );
	    }else{
	      tsos->push_back( hit->originalState->clone() );
	    }
	  }
	}else{
	  ++removedOutOfBoundsHits;
	  if( msgLvl(MSG::DEBUG) ) {
	    msg() << MSG::DEBUG << "   removing out of bounds outlier " << m_idHelper->toString(hit->id) 
		  << " pull " << std::setw(7) << hit->pull << endmsg;
	  }
	}
      }else{
	tsos->push_back( hit->originalState->clone() );
      }
    }

    if( !addedHits && removedOutOfBoundsHits == 0 ){
      if( msgLvl(MSG::DEBUG) ) msg() << " track unchanged " << endmsg;
      delete tsos;
      return &track;
    }

    if( tsos->size() < 6 ){
      if( msgLvl(MSG::DEBUG) ) ATH_MSG_WARNING(" too few hits, cannot add hits. This should not happen " );
      delete tsos;
      return 0;
    }


    // create new track
    Trk::Track* cleanedTrack = new Trk::Track( track.info(), tsos, track.fitQuality() ? track.fitQuality()->clone():0 );
    
    if( !addedHits ){
      ATH_MSG_DEBUG(" only removed out of bound hits, returning track without new fit " );
      m_trackToBeDeleted.insert( cleanedTrack );
      return cleanedTrack;
    }

    // fit new track
    if ( msgLvl(MSG::DEBUG) ) {
      const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
      int nStates = 0;
      if ( states ) nStates = states->size();
      msg() << MSG::DEBUG << "outlierRecovery: calling fit with hits: " << nStates;
      if ( msgLvl(MSG::VERBOSE) ) {
	msg() << MSG::VERBOSE << std::endl << m_printer->printMeasurements( track );
      }
      msg() << endmsg;
    }
    Trk::Track* newTrack = m_fitter->fit(*cleanedTrack,false,track.info().particleHypothesis());
    if( newTrack ) {
      ATH_MSG_DEBUG("Outlier recovery successfull " );
      m_trackToBeDeleted.insert( newTrack );
      init(*newTrack);
    }else{
      ATH_MSG_DEBUG("refit after outlier recovery failed " );
    }
    // delete input
    delete cleanedTrack;

    // return result
    return newTrack;
  }



  void MuonTrackCleaner::init( const Trk::Track& track ) const {

    cleanUp();
    
    m_slFit =  !m_magFieldSvc->toroidOn() || m_helper->isSLTrack( track );
    m_fitter = m_slFit ? &*m_slTrackFitter : &*m_trackFitter; 

    // init
    m_nscatterers = 0;
    m_numberOfFlippedMdts = 0;
    m_numberOfCleanedCompROTs = 0;
    m_nhits = 0;
    m_noutliers = 0;
    m_pullSum = ChamberPullInfo(); 
    m_pullSumPhi = ChamberPullInfo();
    m_pullSumTrigEta = ChamberPullInfo();
    m_chambersToBeRemoved.clear();
    m_chambersToBeRemovedPhi.clear();
    m_pullSumPerChamber.clear();
    m_pullSumPerChamberPhi.clear();
    m_pullSumPerChamberEta.clear();
    m_hitsPerChamber.clear();
    m_outBoundsPerChamber.clear();
    m_measInfo.clear();
    m_largePullMeasurements.clear();
    m_chamberLayerStatistics.clear();
    m_stations.clear();
    m_phiLayers.clear();
    m_hasOfBoundsOutliers = false;
    m_hasVertexConstraint = false;
    m_hasSmall = false;
    m_hasLarge = false;
    m_nIdHits = 0;
    m_nPseudoMeasurements = 0;
    m_nPhiConstraints = 0;

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, cannot perform cleaning " );
      return;
    }
    
    if( msgLvl(MSG::VERBOSE) ) msg() << " init: " << m_printer->print(track) << std::endl; 

    m_resPullsToBeDeleted.reserve(states->size());
    m_measInfo.reserve(states->size());

    std::set<int> rpcLayers;
    std::set<int> tgcLayers;
    const Trk::MeasurementBase* mdtmeas = 0;
    double largestmdtpull=-999;

    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ){
	m_measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }
      
      if( (*tsit)->type(Trk::TrackStateOnSurface::Scatterer) ) {
	const Trk::MaterialEffectsBase* matEff = (*tsit)->materialEffectsOnTrack();
	const Trk::MaterialEffectsOnTrack* matTrk = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(matEff);
	if( matEff ){

	}
	if( msgLvl(MSG::VERBOSE) ) {
	  msg() << MSG::VERBOSE << " Scatterer: r  " << pars->position().perp()
		<< " z " << pars->position().z();
	  if( matEff ) msg() << " X0 " << matEff->thicknessInX0();
	  if( matTrk && matTrk->scatteringAngles() ){
	    const Trk::ScatteringAngles* scatAngle = matTrk->scatteringAngles();
	    msg() << " pull phi " << scatAngle->deltaPhi()/scatAngle->sigmaDeltaPhi()
		  << " pull theta " << scatAngle->deltaTheta()/scatAngle->sigmaDeltaTheta();
	  }
	  if( matTrk && matTrk->energyLoss() ){
	    const Trk::EnergyLoss* eloss = matTrk->energyLoss();
	    msg() << " eloss " << eloss->deltaE();
	  }
	  msg() << std::endl;
	}
	++m_nscatterers;
	m_measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ){	
	m_measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }

      Identifier id = m_helper->getIdentifier(*meas);
      bool pseudo = !id.is_valid();
      if( pseudo ) ++m_nPseudoMeasurements;

      if( !pseudo && !m_idHelper->mdtIdHelper().is_muon(id) ){
	if( msgLvl(MSG::VERBOSE) ) msg() << MSG::VERBOSE << "      TSOS is not a muon hit, position: r  " << pars->position().perp()
					   << " z " << pars->position().z() << std::endl;

	// count ID hits on track
	if( (*tsit)->type(Trk::TrackStateOnSurface::Measurement) && m_idHelper->mdtIdHelper().is_indet(id) ){
	  ++m_nIdHits;
	}
	m_measInfo.push_back( MCTBCleaningInfo(*tsit) );
	continue;
      }

      // check whether there is a vertex constraint
      if( pseudo && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) && pars->associatedSurface().center().perp() < 200. ){
	m_hasVertexConstraint = true;
      }

      Identifier chId = pseudo ? id : m_idHelper->chamberId(id);
      bool measuresPhi = pseudo ? true : m_idHelper->measuresPhi(id);
      
      // bound checks
      Amg::Vector2D locPos;;
      if( !meas->associatedSurface().globalToLocal(pars->position(),pars->position(),locPos) ){
	ATH_MSG_DEBUG(" localToGlobal failed !!!!! " );
	continue;
      }
      bool inBounds = true;
      double tol1 = 100.;
      double tol2 = 2*tol1;
      if( !pseudo && m_idHelper->isMdt(id) ) tol1 = 5.;
      
      // we need a special bound check for MDTs so we cast to SL surface
      const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>(&meas->associatedSurface());
      if( slSurf ) {
	// perform bound check only for second coordinate
	inBounds = slSurf->bounds().insideLoc2(locPos,tol2);
      }else{
	inBounds = meas->associatedSurface().insideBounds(locPos,tol1,tol2);
      }

      MuonStationIndex::ChIndex chIndex = !pseudo ? m_idHelper->chamberIndex(id) : MuonStationIndex::ChUnknown;
      
      // pointer to resPull
      const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( meas, pars, (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ? 
									 Trk::ResidualPull::Unbiased : Trk::ResidualPull::Biased );
      if( !resPull ) {
	ATH_MSG_DEBUG(" calculation of residual/pull failed !!!!! " );
	continue;
      }
      m_resPullsToBeDeleted.push_back( resPull );
      
      // sanity check
      if( !pseudo && resPull->pull().size() != 1 ){
	ATH_MSG_WARNING(" ResidualPull vector has size " << resPull->pull().size() << " for channel " << m_idHelper->toString(id) );
	continue;
      }
      
      bool isMDT = !pseudo ? m_idHelper->isMdt(id) : false;
      double residual = resPull->residual().front(); 
      double pull = fabs(resPull->pull().front());
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
      bool isDelta = isOutlier && isMDT && rTrackAbs < 14.6 && rTrackAbs > fabs(rDrift);

      // remove all outliers that are too far from the track
      if( isOutlier ){
	if( isMDT ){
	  if( rTrackAbs > 14.6 ) inBounds = false;
	}else if( pull > 10 ) {
	  inBounds = false;
	}
      }

      MdtDriftCircleOnTrack* mdtRotFlipped = 0;
      const CompetingMuonClustersOnTrack* updatedCompRot = 0;
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
	    mdtRotFlipped = mdtRot->clone();
	    Trk::DriftCircleSide side = rDrift < 0. ? Trk::RIGHT : Trk::LEFT;
	    m_mdtRotCreator->updateSign(*mdtRotFlipped,side);
	    double rDriftFlip = mdtRotFlipped->localParameters()[Trk::locR];
	    int signRot = rDrift < 0 ? -1 : 1;
	    int signRotFlip = rDriftFlip < 0 ? -1 : 1;
	    if( rDrift != 0. && signRot == signRotFlip ){
	      ATH_MSG_WARNING(" failed to flip sign of MDT " << rDrift << "  flip " << rDriftFlip );
	    }
	    
	    m_measurementsToBeDeleted.push_back(mdtRotFlipped);
	  }else{
	    ATH_MSG_WARNING(" failed to dynamic_cast measurement with MDT Identifier to a MdtDriftCircleOnTrack" );
	  }
	}else{
	  // if the outlier MDT is not recoverable by flipping the sign, add it as the worst outlier
	  if( isOutlier && isMDT && pull>largestmdtpull && !(*tsit)->type(Trk::TrackStateOnSurface::Outlier)){
	    largestmdtpull=pull;
	    mdtmeas=meas;
	  }
	}
	
	if( measuresPhi ){
	  bool isRpc = m_idHelper->isRpc(id);
	  if( isRpc ){
	    int layer = 0;
	    MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
	    if( stIndex == Muon::MuonStationIndex::BM && m_idHelper->rpcIdHelper().doubletR(id) == 1 ) layer = 1;
	    else if( stIndex == Muon::MuonStationIndex::BO ) layer = 2;
	    rpcLayers.insert(layer);
	  }
	  
	  bool isTgc = m_idHelper->isTgc(id);
	  if( isTgc ){
	    int layer = 0;
	    MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
	    if( stIndex == Muon::MuonStationIndex::EM ){
	      std::string stName = m_idHelper->chamberNameString(id);
	      if( stName[1] == '1' )      layer = 1;
	      else if( stName[1] == '2' ) layer = 2;
	      else if( stName[1] == '3' ) layer = 3;
	      else{
		ATH_MSG_WARNING("Unable to calculate TGC layer for " << m_idHelper->toString(id) );
		layer = -1;
	      }
	    }	   
	    if( layer != -1 ) tgcLayers.insert(layer);
	  }
	  if( m_idHelper->issTgc(id) ){
	    int layer = 4;
	    if( m_idHelper->stgcIdHelper().multilayer(id) == 2 ) layer = 5;
	    tgcLayers.insert(layer);
	    ATH_MSG_VERBOSE("adding STGC phi hit " << layer  << " size " << tgcLayers.size() );
	  }
	}

	if( m_cleanCompROTs ){
	  const CompetingMuonClustersOnTrack* crot = (measuresPhi && !isMDT && m_idHelper->isRpc(id)) ? 
	    dynamic_cast<const CompetingMuonClustersOnTrack*>(meas) : 0;
	  if( crot ){
	    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " CompetingMuonClustersOnTrack with rots " << crot->numberOfContainedROTs() << std::endl; 
	    double minpos = 0.;
	    //double maxpos = 0.;
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
		//maxpos = cluster->localParameters()[Trk::locX];
		absminres = absres;
		absmaxres = absres;
	      }else if( absres < absminres ){
		minres = residual;
		absminres = absres;
		minpos = cluster->localParameters()[Trk::locX];
	      }else if( absres > absmaxres ){
		maxres = residual;
		absmaxres = absres;
		//maxpos = cluster->localParameters()[Trk::locX];
	      }
	      if( msgLvl(MSG::VERBOSE) ) msg() << MSG::VERBOSE << " ROT " << m_idHelper->toString(cluster->identify()) 
						 << " lpos " << cluster->localParameters()[Trk::locX] << " pars " << pars->parameters()[Trk::locX] 
						 << " residual " << residual << std::endl;
	    }	
	    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " residuals: min  " << minres << " max " << maxres << " diff " << maxres - minres;
	    bool splitCompRot = false;
	    if( fabs(maxres - minres) > 100 && absmaxres-absminres > 20 && crot->numberOfContainedROTs() < 20 ){
	      if( msgLvl(MSG::DEBUG) ) msg() << " recoverable double cluster ";
	      splitCompRot = true;
	    }
	    if( msgLvl(MSG::DEBUG) ) msg() << std::endl;
	    if( splitCompRot ){

	      std::list<const Trk::PrepRawData*> prdList;
	    
	      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << " Splitting comp rot " << std::endl;
	      for( unsigned int i=0;i<crot->numberOfContainedROTs();++i){
		const MuonClusterOnTrack* cluster = &crot->rioOnTrack(i);
		if( !cluster ) continue;
		double residual = cluster->localParameters()[Trk::locX] - minpos;
		double absres = residual < 0. ? -1.*residual : residual;
		if( absres < 40 ){
		  if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "   NEW ROT " << m_idHelper->toString(cluster->identify()) 
						   << " lpos " << cluster->localParameters()[Trk::locX] << " pars " << pars->parameters()[Trk::locX] 
						   << " residual " << residual << std::endl;
		
		  prdList.push_back(cluster->prepRawData());
		}
	      }	
	      if( prdList.empty() ){
		ATH_MSG_WARNING("No clusters selected during comprot cleaning, keeping old cluster" );
	      }else{
		updatedCompRot = m_compRotCreator->createBroadCluster(prdList,0.);
		m_measurementsToBeDeleted.push_back(updatedCompRot);
		++m_numberOfCleanedCompROTs;
	      }
	    }
	  }
	}
      }

      m_measInfo.push_back( MCTBCleaningInfo(id,chId,chIndex,inBounds,residual,pull,*tsit,meas,pars,resPull,0) );
      MCTBCleaningInfo& info = m_measInfo.back();
      if( flipSign ) {
	info.flippedMdt = mdtRotFlipped;
      }
      info.isNoise = isNoise;
      if( updatedCompRot ){
	info.cleanedCompROT = updatedCompRot;
	if( updatedCompRot->associatedSurface() != meas->associatedSurface() ){
	  const Trk::TrackParameters* exPars = m_extrapolator->extrapolate(*pars,updatedCompRot->associatedSurface(),Trk::anyDirection,false,Trk::muon);
	  if( !exPars ){
	    ATH_MSG_WARNING("Update of comp rot parameters failed, keeping old ones" );
	    info.cleanedCompROT = 0;
	  }else{
	    info.pars = exPars;
	    m_parsToBeDeleted.push_back(exPars);
	  }
	}
      }

      ChamberLayerStatistics& chamberStatistics = m_chamberLayerStatistics[chIndex];
      chamberStatistics.chIndex = chIndex;

      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) {
	if( msgLvl(MSG::VERBOSE) ) {
	  msg() << MSG::VERBOSE << "  Outlier " << m_idHelper->toString( id ) << m_printer->print( *resPull ); 
	  if( isMDT ) msg() << " r_drift " << rDrift << "  r_trk " << rTrack << " dr " << std::setw(7) << Amg::error(meas->localCovariance(),Trk::locR);
	  if( isNoise ) msg() << " noise ";
	  if( !inBounds ) msg() << " outBounds ";
	  else{
	    msg() << " inBounds  ";
	    if( flippedIsRecoverable ) msg() << MSG::VERBOSE << " flipped is recoverable, rFlipped " << mdtRotFlipped->localParameters()[Trk::locR];
	    else if( isRecoverable ) msg() << MSG::VERBOSE << " recoverable ";
	    else if( isDelta ) msg() << MSG::VERBOSE << " delta ";
	  }
	  msg() << MSG::VERBOSE << std::endl;
	}

	if( !inBounds ){
	  m_hasOfBoundsOutliers = true;
	  ++chamberStatistics.noutBounds;
	}else if( flippedIsRecoverable || isRecoverable ) ++chamberStatistics.nrecoverableOutliers;
	else if( isDelta )                                ++chamberStatistics.ndeltas;
	else if( pull < 10 )                              ++chamberStatistics.noutliers;

	if(!pseudo) ++m_noutliers; //don't count pseudos here
	info.useInFit = 0;
	continue;
      }
      
      // if we get here could as hit
      ++chamberStatistics.nhits;
      if(!pseudo) ++m_nhits; //pseudo's shouldn't count here

      if( flipSign ) ++m_numberOfFlippedMdts;
      
      if( msgLvl(MSG::VERBOSE) ) {
	std::string idString   = pseudo ? " Pseudomeasurement " :  m_idHelper->toString( id ); 
	std::string boundCheck = inBounds ? "inBounds" : "outBounds"; 
	msg() << MSG::VERBOSE << m_printer->print( *resPull ) << " "  << idString << " " << boundCheck;
	if( isNoise )   msg() << MSG::VERBOSE << " noise";
	else if( isOutlier ) msg() << MSG::VERBOSE << " outlier";
	if( isMDT ){
	  msg() << " r_drift " << std::setw(8) << rDrift << "  r_trk " << std::setw(7) << rTrack 
		<< " dr " << std::setw(7) << Amg::error(meas->localCovariance(),Trk::locR);
	  if( flipSign )  msg() << MSG::VERBOSE << " flipped rDrift " << mdtRotFlipped->localParameters()[Trk::locR]
				<< "  pull " << flippedPull;		
	  else if( isDelta ) msg() << MSG::VERBOSE << " delta ";
	}	  
	const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(meas);
	if( !rpc ){
	  const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
	  if( crot ){
	    rpc = dynamic_cast<const RpcClusterOnTrack*>(crot->containedROTs().front());
	  }
	}
	if( rpc ) msg() << " time " << rpc->prepRawData()->time()-rpc->globalPosition().mag()/300.;
	msg() << std::endl;
      }
  
      if( pseudo ){
	
	// pseudo measurements should be included in chamber hit removal so stop here
	if( !isNoise && isOutlier ) {
	  m_largePullPseudoMeasurements.insert( meas );
	}
	continue;
	
      }else {
	
	// count hits
	EtaPhiHits& hits = m_hitsPerChamber[info.chId];
	if( measuresPhi ) ++hits.nphi;
	else              ++hits.neta;
	
	// bound checks
	if( !inBounds ) {
	  EtaPhiHits& outHits = m_outBoundsPerChamber[info.chId];
	  if( measuresPhi ) ++outHits.nphi;
	  else              ++outHits.neta;
	}
	
	// measurements with large pull
	if( !(*tsit)->type(Trk::TrackStateOnSurface::Outlier) && isOutlier && !isMDT ) {
	  m_largePullMeasurements.insert( meas );
	}
	
	// pulls per chamber
	if( !m_idHelper->isTrigger(info.chId) ) {
	  
	  ChamberPullInfo& pullInfoHits = measuresPhi ? m_pullSumPhi : m_pullSum;
	  pullInfoHits.pullSum += pull;
	  ++pullInfoHits.nhits;
	  if( pull > pullInfoHits.maxPull ) pullInfoHits.maxPull = pull;
	  
	  ChamberPullInfo& pullInfoCh = measuresPhi ? m_pullSumPerChamberPhi[info.chId] : m_pullSumPerChamber[info.chId];
	  pullInfoCh.pullSum += pull;
	  ++pullInfoCh.nhits;
	  if( pull > pullInfoCh.maxPull ) pullInfoCh.maxPull = pull;
	}else{
	  ChamberPullInfo& pullInfoTrig = measuresPhi ? m_pullSumPhi : m_pullSumTrigEta;
          if(!measuresPhi) m_pullSumPerChamberEta[info.chId];
	  pullInfoTrig.pullSum += pull;
	  ++pullInfoTrig.nhits;
	  if( pull > pullInfoTrig.maxPull ) pullInfoTrig.maxPull = pull;
	}
      }
    }
    
    // if the was an MDT outlier add it to the list of hits to be removed
    if (mdtmeas) m_largePullMeasurements.insert( mdtmeas );

    // check whether we have sufficient layers to savely clean the RPC/TGC comp rots
    unsigned int nphiLayers = rpcLayers.size() + tgcLayers.size();
    if( m_hasVertexConstraint ) {
      nphiLayers += 1;
      ++m_counters.nvertexConstraints;
    }
    if( m_nIdHits > 0 ) {
      nphiLayers += 2;
      ++m_counters.nIdTracks;      
    }

    if( ( nphiLayers > 2 ) && m_numberOfCleanedCompROTs > 0 ){
      ATH_MSG_DEBUG(" Sufficient constraints to clean competing ROTs: trigger layers " << nphiLayers );
    }else{
      // reset counter so no cleaning is performed
      m_numberOfCleanedCompROTs = 0;
    }

    // update sl fit configuration if track has ID hits or vertex constraint
    if( m_slFit && (m_hasVertexConstraint || m_nIdHits > 0 ) && m_magFieldSvc->solenoidOn() ) {
      m_slFit = false;
      m_fitter = &*m_trackFitter; 
    }


    if( msgLvl(MSG::DEBUG) && (m_hasVertexConstraint || m_nIdHits ) ) {
      if( m_hasVertexConstraint ) msg() << MSG::DEBUG << " Track has vertex contraint:";
      if( m_nIdHits > 0 )         msg() << MSG::DEBUG << " Track has  ID Hits: " << m_nIdHits;
      if( m_magFieldSvc->solenoidOn() ) msg() << MSG::DEBUG << " Solenoid On";
      else               msg() << MSG::DEBUG << " Solenoid Off";
      if( m_magFieldSvc->toroidOn() )   msg() << MSG::DEBUG << " Toroid On";
      else               msg() << MSG::DEBUG << " Toroid Off";
      if( m_slFit )      msg() << MSG::DEBUG << " Use SL fit";
      else               msg() << MSG::DEBUG << " Use curved fit";
      msg() << endmsg;
    }

  
    std::set<Identifier> chambersToBeRemoved, chambersToBeRemovedPhi, goodEtaLayers, goodPhiLayers;


    // check whether there are any chambers in eta that should be removed
    extractChambersToBeRemoved(m_pullSumPerChamber,m_chambersToBeRemoved,chambersToBeRemoved);

    // check whether there are any chambers in phi that should be removed
    extractChambersToBeRemoved(m_pullSumPerChamberPhi,m_chambersToBeRemovedPhi,chambersToBeRemovedPhi); 

    if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << "Chambers on track summary:";

    EtaPhiPerChamberIt chit =  m_hitsPerChamber.begin();
    EtaPhiPerChamberIt chit_end =  m_hitsPerChamber.end();
    for( ;chit!=chit_end;++chit ){
      
      if( !chit->first.is_valid() ) continue;

      bool isPrec = m_idHelper->isMdt(chit->first) || m_idHelper->isCsc(chit->first) || m_idHelper->isMM(chit->first) || m_idHelper->issTgc(chit->first);
      if( isPrec ){
	MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(chit->first);
	m_stations.insert(stIndex);
      }

      EtaPhiHits& nhits      = chit->second;
      EtaPhiHits& noutBounds = m_outBoundsPerChamber[chit->first];
      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << std::endl << " chamber " << m_idHelper->toStringChamber(chit->first) 
				       << "  eta hits " << nhits.neta << " outBounds " << noutBounds.neta
				       << "  phi hits " << nhits.nphi << " outBounds " << noutBounds.nphi;
      if( nhits.neta != 0 && nhits.neta == noutBounds.neta ){
	if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << std::endl << "   --> All eta hits out of bounds ";
	// remove eta hits for this chamber
	double fakePull = 1000.*nhits.neta;
	if( !chambersToBeRemoved.count(chit->first) ) {
	  // just add it
	  m_chambersToBeRemoved.push_back( std::make_pair(fakePull,chit->first) );
	  chambersToBeRemoved.insert(chit->first);
	} else {
	  // replace existing entry if new one has larger Pull
	  PullChIt pIt     = m_chambersToBeRemoved.begin();
	  PullChIt pIt_end = m_chambersToBeRemoved.end();
	  for ( ; pIt != pIt_end; ++pIt ) {
	    if ( pIt->second == chit->first && pIt->first < fakePull ) {
	      pIt->first = fakePull;
	      break;
	    }
	  }
	}
      }else{
	if( msgLvl(MSG::DEBUG) && noutBounds.neta != 0 ) msg() << MSG::DEBUG << std::endl << "   --> Some eta hits out of bounds ";

	if( isPrec && nhits.neta > 0 ){
	  if( m_idHelper->isSmallChamber(chit->first) ) m_hasSmall = true;
	  else                                          m_hasLarge = true;
	}
      }
      if( nhits.nphi != 0 && nhits.nphi == noutBounds.nphi ){
	if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << std::endl << "   --> All phi hits out of bounds ";
	// remove phi hits for this chamber
	double fakePull = 1000.*nhits.nphi;
	if( !chambersToBeRemovedPhi.count(chit->first) ) {
	  // just add it
	  m_chambersToBeRemovedPhi.push_back( std::make_pair(fakePull,chit->first) );
	  chambersToBeRemovedPhi.insert(chit->first);
	} else {
	  // replace existing entry if new one has larger Pull
	  PullChIt pIt     = m_chambersToBeRemovedPhi.begin();
	  PullChIt pIt_end = m_chambersToBeRemovedPhi.end();
	  for ( ; pIt != pIt_end; ++pIt ) {
	    if ( pIt->second == chit->first && pIt->first < fakePull ) {
	      pIt->first = fakePull;
	      break;
	    }
	  }
	}
      }else {
	if( msgLvl(MSG::DEBUG) && noutBounds.nphi != 0 ) msg() << MSG::DEBUG << std::endl << "   --> Some phi hits out of bounds ";
	if( nhits.nphi > 0 ){
	  MuonStationIndex::PhiIndex phiIndex = m_idHelper->phiIndex(chit->first);
	  m_phiLayers.insert(phiIndex);
	}
      }
    }
    
    

    m_nPhiConstraints = m_phiLayers.size() + m_nPseudoMeasurements;
    if( m_hasLarge && m_hasSmall ) ++m_nPhiConstraints;
    if( m_stations.size() == 1 && nphiLayers > 1 ) ++m_nPhiConstraints;

    if( msgLvl(MSG::DEBUG) ) {

      double pull_precisionhits = -1.;
      if (m_pullSum.nhits != 0) {
	pull_precisionhits = m_pullSum.pullSum/m_pullSum.nhits;
      }
      double pull_triggerhits = -1.;
      if (m_pullSumTrigEta.nhits != 0) {
	pull_triggerhits = m_pullSumTrigEta.pullSum/m_pullSumTrigEta.nhits;
      }
      double pull_phihits = -1.;
      if (m_pullSumPhi.nhits != 0) {
	pull_phihits = m_pullSumPhi.pullSum/m_pullSumPhi.nhits;
      }
      msg() << MSG::DEBUG << std::endl << "    pull sum: precision hits " << pull_precisionhits
            << " trigger eta " << pull_triggerhits
            << "  phi " << pull_phihits
            << "  measurements " << m_measInfo.size() << std::endl
            << "  precision chambers " << m_pullSumPerChamber.size() 
            << "  hits with large pull " << m_largePullMeasurements.size() 
            << "  pseudos with large pull " << m_largePullPseudoMeasurements.size() 
            << "  chambers to be removed " << m_chambersToBeRemoved.size()
            << "  phi " << m_chambersToBeRemovedPhi.size()
            << "  phi lay " << m_phiLayers.size()
            << "  phi constr " << m_nPhiConstraints;
      
      if (m_pullSumPhi.nhits != 0) {
	double phiPull = m_pullSumPhi.pullSum/m_pullSumPhi.nhits;
	if( m_stations.size() == 1 && phiPull > 5. ){
	  msg() << MSG::DEBUG << std::endl << " Single station track with large phi pulls!! ";
	}
      }
      else {msg() << MSG::DEBUG << std::endl << " Track with no phi hits!! ";}
    }

    // finally end with a endmsg
    if ( msgLvl(MSG::DEBUG) ) msg() << endmsg;

  }

  bool MuonTrackCleaner::extractChambersToBeRemoved( const PullChamberMap& pullSumPerChamber, PullChVec& chambersToBeRemoved, 
						     std::set<Identifier>& chambersToBeRemovedSet   ) const {

    bool doCleaning = false;

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
      if( msgLvl(MSG::DEBUG) ) msg() << MSG::DEBUG << std::endl << " chamber " << m_idHelper->toStringChamber(cit->first) 
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

    if(doCleaning&&m_iterate) {
      cit = pullSumPerChamber.begin();
      for( ;cit!=cit_end;++cit ){
        double avePull = cit->second.pullSum/cit->second.nhits;
        if( !chambersToBeRemovedSet.count(cit->first) ) {
          chambersToBeRemoved.push_back( std::make_pair(avePull,cit->first) );
          chambersToBeRemovedSet.insert(cit->first);
        }
      }
    }

    if(dropMore&&doCleaning&&m_iterate) {
// add trigger chambers
      cit = m_pullSumPerChamberEta.begin();
      cit_end = m_pullSumPerChamberEta.end();
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


    
    bool isMdt = id.is_valid() ? m_idHelper->isMdt(id) : false;
    bool measuresPhi = id.is_valid() ? m_idHelper->measuresPhi(id) : false;

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

  
  void MuonTrackCleaner::cleanUp() const {
    
    for_each( m_resPullsToBeDeleted.begin(), m_resPullsToBeDeleted.end(), MuonDeleteObject<const Trk::ResidualPull>() );
    m_resPullsToBeDeleted.clear();

    for_each( m_parsToBeDeleted.begin(), m_parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );
    m_parsToBeDeleted.clear();

    for_each( m_fitQToBeDeleted.begin(), m_fitQToBeDeleted.end(), MuonDeleteObject<const Trk::FitQuality>() );
    m_fitQToBeDeleted.clear();

    for_each( m_measurementsToBeDeleted.begin(), m_measurementsToBeDeleted.end(), MuonDeleteObject<const Trk::MeasurementBase>() );
    m_measurementsToBeDeleted.clear();

  }
  
  void MuonTrackCleaner::cleanUpTracks() const {
    cleanUp();
    for_each( m_trackToBeDeleted.begin(), m_trackToBeDeleted.end(), MuonDeleteObject<Trk::Track>() );
    m_trackToBeDeleted.clear();
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

}
