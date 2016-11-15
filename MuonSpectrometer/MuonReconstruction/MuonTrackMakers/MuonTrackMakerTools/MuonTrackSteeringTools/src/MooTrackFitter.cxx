/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MooTrackFitter.h"
#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuPatHitTool.h"
#include "MuPatCandidateTool.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "SortMuPatHits.h"

#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMomentumEstimator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonHitSelector.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentInOverlapResolvingTool.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/Layer.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkSurfaces/StraightLineSurface.h"

#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkDetDescrUtils/Intersection.h"

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventPrimitives/LocalDirection.h"

#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "TrkDriftCircleMath/MatchDCWithLine.h"
#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/LocPos.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Muon {

  
  MooTrackFitter::MooTrackFitter(const std::string& t,const std::string& n,const IInterface* p)  :  
    AthAlgTool(t,n,p),
    m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
    m_trackFitter("Trk::GlobalChi2Fitter/MCTBFitter"),                        
    m_trackFitterPrefit("Trk::GlobalChi2Fitter/MCTBFitter"),                          
    m_hitHandler("Muon::MuPatHitTool/MuPatHitTool"),
    m_momentumEstimator("MuonSegmentMomentum/MuonSegmentMomentum"),
    m_magFieldProperties(Trk::NoField),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_entryHandler("Muon::MuPatCandidateTool/MuPatCandidateTool"),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtTubeHitOnTrackCreator"),
    m_phiHitSelector("MuonPhiHitSelector/MuonPhiHitSelector"),
    m_cleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_overlapResolver("Muon::MuonSegmentInOverlapResolvingTool/MuonSegmentInOverlapResolvingTool"),
    m_trackSummaryTool("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"),
    m_patRecInfo(Trk::TrackInfo::Moore)
  {

    declareInterface<MooTrackFitter>(this);

    declareProperty("PreCleaningReducedChi2Cut",m_preCleanChi2Cut = 500.,"minimum reduced chi2 for a track to be cleaned");
    declareProperty("ReducedChi2Cut",m_chi2Cut = 100.,"minimum reduced chi2 for a track to be accepted");
    declareProperty("Propagator",          m_propagator);
    declareProperty("Fitter",              m_trackFitter);
    declareProperty("FitterPreFit",        m_trackFitterPrefit);
    declareProperty("SegmentMomentum",     m_momentumEstimator);
    declareProperty("HitTool",m_hitHandler);
    declareProperty("IdHelper",m_idHelperTool);
    declareProperty("MuonHelperTool",m_helperTool);
    declareProperty("MuonPrinterTool",m_printer);
    declareProperty("CandidateTool",m_entryHandler);
    declareProperty("TrackToSegmentTool",m_trackToSegmentTool);
    declareProperty("MdtRotCreator",m_mdtRotCreator);
    declareProperty("PhiHitSelector",m_phiHitSelector);
    declareProperty("TrackCleaner",m_cleaner);
    declareProperty("SegmentInOverlapTool",m_overlapResolver);

    declareProperty("SLFit",               m_slFit = true);
    declareProperty("RunOutlier",          m_runOutlier = false);
    declareProperty("MatEffects",          m_matEffects = 2);
    declareProperty("FitEtaStrips",        m_fitEtaStrips = false);
    declareProperty("SeedAtStartOfTrack",  m_seedAtStartOfTrack = true );
    declareProperty("SeedWithAvePhi",      m_seedWithAvePhi = true );
    declareProperty("SeedWithSegmentTheta",m_seedWithSegmentTheta = true );
    declareProperty("SeedPhiWithEtaHits",  m_seedPhiWithEtaHits = false );
    declareProperty("UsePreciseHits",      m_usePreciseHits = false );
    declareProperty("UsePrefit",           m_usePrefit = true );
    declareProperty("AllowFirstFitResult", m_allowFirstFit = false );
    declareProperty("PThreshold",          m_pThreshold = 500. );
    declareProperty("CleanPhiHits",        m_cleanPhiHits = true );
    declareProperty("MaxPatternPhiHits",   m_phiHitsMax = 40 );
    declareProperty("Cosmics",             m_cosmics = false );
    declareProperty("OpeningAngleCut",     m_openingAngleCut = 0.3 );
    declareProperty("UsePreciseHitsInFirstStation", m_preciseFirstStation = false );

  }
    
  MooTrackFitter::~MooTrackFitter() {
  }
  
  StatusCode MooTrackFitter::initialize() {

    if ( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;

    if( m_propagator.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_propagator); 
      return StatusCode::FAILURE;
    }


    if ( m_trackSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Unable to retrieve" << m_trackSummaryTool);
      return StatusCode::FAILURE;
    }

    if( m_cleaner.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_cleaner); 
      return StatusCode::FAILURE;
    }

    if( m_overlapResolver.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_overlapResolver); 
      return StatusCode::FAILURE;
    }

    if( m_trackFitter.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_trackFitter); 
      return StatusCode::FAILURE;
    }

    if( m_trackFitterPrefit.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_trackFitterPrefit); 
      return StatusCode::FAILURE;
    }

    if( m_momentumEstimator.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_momentumEstimator); 
      return StatusCode::FAILURE;
    }

    if( m_idHelperTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_idHelperTool); 
      return StatusCode::FAILURE;
    }

    if( m_helperTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_helperTool); 
      return StatusCode::FAILURE;
    }

    if( m_hitHandler.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_hitHandler); 
      return StatusCode::FAILURE;
    }

    if( m_entryHandler.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_entryHandler); 
      return StatusCode::FAILURE;
    }

    if( m_printer.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_printer); 
      return StatusCode::FAILURE;
    }

    // Configuration of the material effects
    Trk::ParticleSwitcher particleSwitch;
    m_ParticleHypothesis = particleSwitch.particle[m_matEffects];

    if( m_trackToSegmentTool.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_trackToSegmentTool); 
      return StatusCode::FAILURE;
    }

    if( m_mdtRotCreator.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_mdtRotCreator); 
      return StatusCode::FAILURE;
    }

    if( m_phiHitSelector.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not get " << m_phiHitSelector); 
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode MooTrackFitter::finalize(){
    
    double nfits = m_counters.nfits > 0 ? m_counters.nfits : 1.;
    double nfailedExtractInital = m_counters.nfailedExtractInital/nfits;
    double nfailedMinMaxPhi = m_counters.nfailedMinMaxPhi/nfits;
    double nfailedParsInital = m_counters.nfailedParsInital/nfits;
    double nfailedExtractCleaning = m_counters.nfailedExtractCleaning/nfits;
    double nfailedFakeInitial = m_counters.nfailedFakeInitial/nfits;
    double nfailedTubeFit = m_counters.nfailedTubeFit/nfits;
    double noPerigee = m_counters.noPerigee/nfits;
    double nlowMomentum = m_counters.nlowMomentum/nfits;
    double nfailedExtractPrecise = m_counters.nfailedExtractPrecise/nfits;
    double nfailedFakePrecise = m_counters.nfailedFakePrecise/nfits;
    double nfailedFitPrecise = m_counters.nfailedFitPrecise/nfits;
    double nsuccess = m_counters.nsuccess/nfits;
    ATH_MSG_INFO(" Summarizing statistics: nfits " << m_counters.nfits << std::endl
                 << "|  extract  | phi range | startPars | clean phi |  add fake |    fit    | no perigee |  low mom  |  extract  |  add fake | final fit |   passed  |" 
                 << std::endl << std::setprecision(2)
                 << std::setw(12) << nfailedExtractInital << std::setw(12) << nfailedMinMaxPhi << std::setw(12) << nfailedParsInital
                 << std::setw(12) << nfailedExtractCleaning << std::setw(12) << nfailedFakeInitial
                 << std::setw(12) << nfailedTubeFit << std::setw(13) << noPerigee
                 << std::setw(12) << nlowMomentum << std::setw(12) << nfailedExtractPrecise
                 << std::setw(12) << nfailedFakePrecise << std::setw(12) << nfailedFitPrecise
                 << std::setw(12) << nsuccess);
    
    
    return StatusCode::SUCCESS;
  }
  
  bool MooTrackFitter::corruptEntry( const MuPatCandidateBase& entry ) const {
    if( entry.hits().size() < entry.etaHits().size() ) {
      ATH_MSG_WARNING(" Corrupt Entry: more eta hits than hits! " );
      return true;
    }
    if( entry.etaHits().size() < 3 ) 
    	{
	return (entry.ncscHitsEta<2);
	}
    return false;
  }


  Trk::Track* MooTrackFitter::refit( const MuPatTrack& trkCan ) const {

    // internal representation of the track in fitter
    FitterData fitterData;
    
    // copy hit list
    fitterData.hitList = trkCan.hitList();

    // extract hits from hit list and add them to fitterData
    if( !extractData( fitterData, true ) ) return 0;

    // create start parameters
    const Trk::Perigee* pp = trkCan.track().perigeeParameters();
    if( !pp ) return 0;
    
    // fit track
    Trk::Track* track = fit(*pp,fitterData.measurements);

    if( track ){
      // clean and evaluate track
      std::set<Identifier> excludedChambers;
      Trk::Track* cleanTrack = cleanAndEvaluateTrack( *track, excludedChambers );
      if( cleanTrack && cleanTrack != track ){
        delete track;
        track = cleanTrack;
      }
    }else{
      ATH_MSG_DEBUG(" Fit failed " );
    }
 
    cleanUp();

    return track;
  }


  Trk::Track* MooTrackFitter::refit( const Trk::Track& track ) const {
    
    if ( msgLvl(MSG::DEBUG) ) {
      const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
      int nStates = 0;
      if ( states ) nStates = states->size();
      msg(MSG::DEBUG) << MSG::DEBUG << "refit: fitting track with hits: " << nStates;
      if ( msgLvl(MSG::VERBOSE) ) {
        msg(MSG::VERBOSE) << std::endl << m_printer->printMeasurements( track );
      }
      msg(MSG::DEBUG) << endmsg;
    }
    // fit track
    Trk::Track* newTrack = m_trackFitter->fit(track,false,m_ParticleHypothesis);

    if( newTrack ){
     
      // clean and evaluate track
      std::set<Identifier> excludedChambers;
      Trk::Track* cleanTrack = cleanAndEvaluateTrack( *newTrack, excludedChambers );

      if( cleanTrack ){
        // check whether cleaner returned same track, if not delete old track
        if( cleanTrack != newTrack ){
          delete newTrack;
          newTrack = cleanTrack;
        }
      }else{
        ATH_MSG_DEBUG(" Refit failed, rejected by cleaner " );
        delete newTrack;
        newTrack = 0;
      }
    }else{
      ATH_MSG_DEBUG(" Refit failed " );
    }
    cleanUp();
    return newTrack;
  }


  Trk::Track* MooTrackFitter::fit( const MuPatCandidateBase& entry1, const MuPatCandidateBase& entry2, const PrepVec* externalPhiHits ) const {

    ++m_counters.nfits;

    // internal representation of the track in fitter
    FitterData fitterData;

    // extract hits and geometrical information
    if( !extractData(entry1,entry2,fitterData) ) {
      ATH_MSG_DEBUG(" Failed to extract data for initial fit" );
      cleanUp();
      return 0;
    }
    ++m_counters.nfailedExtractInital;

    // get the minimum and maximum phi compatible with all eta hits, don't use for cosmics
    if( !m_cosmics && !getMinMaxPhi( fitterData )  ) {
      ATH_MSG_DEBUG(" Phi range check failed, candidate stations not pointing. Will not fit track" );
      cleanUp();      
      return 0;
    }

    ++m_counters.nfailedMinMaxPhi;


    // create start parameters
    Trk::Perigee* startPars = createStartParameters(fitterData);
    if( !startPars ) {
      ATH_MSG_DEBUG(" Creation of start parameters failed " );
      cleanUp();
      return 0;
    }
    ++m_counters.nfailedParsInital;

    // clean phi hits and reevaluate hits. Do not run for cosmics
    bool hasCleaned = m_cleanPhiHits ? cleanPhiHits( startPars->momentum().mag(), fitterData, externalPhiHits ) : true;
    if( hasCleaned ){
      ATH_MSG_DEBUG(" Cleaned phi hits, re-extracting hits" );
      bool usePrecise = m_usePreciseHits ? true : (fitterData.firstHasMomentum || fitterData.secondHasMomentum);
      if( !extractData( fitterData, usePrecise ) ) {
        ATH_MSG_DEBUG(" Failed to extract data after phi hit cleaning" );
        cleanUp();
        return 0;
      }
    }
    ++m_counters.nfailedExtractCleaning;

    // check whether there are enough phi constraints, if not add fake phi hits
    if( !addFakePhiHits(fitterData,startPars) ) {
      ATH_MSG_DEBUG(" Failed to add fake phi hits for precise fit" );
      cleanUp();
      return 0;
    }
    ++m_counters.nfailedFakeInitial;

    // fit track with broad errors, no material
    Trk::ParticleSwitcher particleSwitch;
    bool doPreFit = m_usePrefit;
    Trk::ParticleHypothesis particleType = particleSwitch.particle[0];
    if( fitterData.firstHasMomentum || fitterData.secondHasMomentum ) doPreFit = false;
    if( !doPreFit ) particleType = Trk::muon;
    
    Trk::Track* track = fit(*startPars,fitterData.measurements,particleType,doPreFit);
    
    
    if( !track ) {
      ATH_MSG_DEBUG(" Fit failed " );
      cleanUp();
      return 0;
    }
    ++m_counters.nfailedTubeFit;

    // create start parameters
    const Trk::Perigee* pp = track->perigeeParameters();
    if( !pp ) {
      ATH_MSG_DEBUG(" Track without perigee parameters, exit " );
      delete track;
      cleanUp();
      return 0;
    }
    ++m_counters.noPerigee;

    if( !m_slFit && !validMomentum( *pp ) ){
      ATH_MSG_DEBUG(" Low momentum, rejected " );
      delete track;
      cleanUp();
      return 0;
    }
    ++m_counters.nlowMomentum;

    if( !fitterData.firstHasMomentum && !fitterData.secondHasMomentum && doPreFit ){
      ATH_MSG_DEBUG(" Performing second fit " );

      // refit with precise errors
      FitterData fitterDataRefit;
      fitterDataRefit.startPars = pp->clone();
      m_parametersToBeDeleted.push_back(fitterDataRefit.startPars);
      fitterDataRefit.firstIsTrack = fitterData.firstIsTrack;
      fitterDataRefit.secondIsTrack = fitterData.secondIsTrack;
      fitterDataRefit.firstHasMomentum = fitterData.firstHasMomentum;
      fitterDataRefit.secondHasMomentum = fitterData.secondHasMomentum;
      fitterDataRefit.avePhi = fitterData.avePhi;
      fitterDataRefit.phiMin = fitterData.phiMin;
      fitterDataRefit.phiMax = fitterData.phiMax;
      fitterDataRefit.firstEntry = fitterData.firstEntry;
      fitterDataRefit.secondEntry = fitterData.secondEntry;
      fitterDataRefit.hitList = fitterData.hitList;
      
      // extract hits from hit list and add them to fitterData
      if( !extractData( fitterDataRefit, true ) ) {
        ATH_MSG_DEBUG(" Failed to extract data for precise fit" );
        delete track;
        cleanUp();
        return 0;
      }
      ++m_counters.nfailedExtractPrecise;
      
      // check whether there are enough phi constraints, if not add fake phi hits
      if( !addFakePhiHits(fitterDataRefit,startPars) ) {
        ATH_MSG_DEBUG(" Failed to add fake phi hits for precise fit" );
        cleanUp();
        delete track;
        return 0;
      }
      ++m_counters.nfailedFakePrecise;
      
      // fit track
      Trk::Track* newTrack = fit(*pp,fitterDataRefit.measurements);
      if( newTrack ){
        delete track;
        track = newTrack;
      }else if( !m_allowFirstFit ){
        ATH_MSG_DEBUG(" Precise fit failed " );
        delete track;
        cleanUp();
        return 0;
      }else{
        ATH_MSG_DEBUG(" Precise fit failed, keep fit with broad errors" );
      }
      ++m_counters.nfailedFitPrecise;
    }

    if( track ){
      // clean and evaluate track
      std::set<Identifier> excludedChambers;
      if( fitterData.firstIsTrack && !fitterData.secondIsTrack ) {
        excludedChambers = fitterData.secondEntry->chamberIds();
        ATH_MSG_VERBOSE(" Using exclusion list of second entry for cleaning" );
      }else if( !fitterData.firstIsTrack && fitterData.secondIsTrack ){
        excludedChambers = fitterData.firstEntry->chamberIds();
        ATH_MSG_VERBOSE(" Using exclusion list of first entry for cleaning" );
      }
      if( !excludedChambers.empty() ){
        ATH_MSG_DEBUG(" Using exclusion list for cleaning" );
      }
      Trk::Track* cleanTrack = cleanAndEvaluateTrack( *track, excludedChambers );
      if( cleanTrack ) {
        if( cleanTrack != track ){
          delete track;
          track = cleanTrack;
          
        }
      }else {
        delete track;
        track = 0;
      }
      
      
    }else{
//      ATH_MSG_DEBUG(" Refit failed " );
    }
    
    if( track ) ++m_counters.nsuccess;
    
    cleanUp();
    if( msgLvl(MSG::DEBUG) && track ) msg(MSG::DEBUG) << MSG::DEBUG << " Track found " << endmsg;
    return track;
  }

  bool MooTrackFitter::extractData( const MuPatCandidateBase& entry1, const MuPatCandidateBase& entry2 , MooTrackFitter::FitterData& fitterData ) const {
    // sanity checks on the entries
    if( corruptEntry( entry1 ) ) {
      ATH_MSG_DEBUG(" corrupt first entry,  cannot perform fit: eta hits " << entry1.etaHits().size() );
      return false;
    }
    if( corruptEntry( entry2 ) ) {
      ATH_MSG_DEBUG(" corrupt second entry, cannot perform fit: eta hits " << entry2.etaHits().size() );
      return false;
    }
    // are we in the endcap region?
    bool isEndcap = entry1.hasEndcap() || entry2.hasEndcap();
        
    // measurement sorting function
    SortMeasurementsByPosition sortMeasurements(isEndcap);
    
    bool entry1IsFirst = sortMeasurements(entry1.hits().front(),entry2.hits().front());
    if(m_cosmics){
      DistanceToPars distToPars(&entry1.entryPars());
      double distToSecond = distToPars(entry2.entryPars().position());
      if( distToSecond < 0 ) entry1IsFirst = false;
      ATH_MSG_DEBUG(" first entry dir " << entry1.entryPars().momentum() << " pos " << entry1.entryPars().position()
                           << " second " << entry2.entryPars().position() << "  dist " << distToSecond );
    }
    const MuPatCandidateBase& firstEntry  = entry1IsFirst ? entry1 : entry2;
    const MuPatCandidateBase& secondEntry = entry1IsFirst ? entry2 : entry1;

    fitterData.firstEntry = &firstEntry;
    fitterData.secondEntry = &secondEntry;

    // check whether we are dealing with a track or a segment
    if( dynamic_cast<const MuPatTrack*>(fitterData.firstEntry) ) {
      fitterData.firstIsTrack = true;
      fitterData.firstHasMomentum = fitterData.firstEntry->hasMomentum();
    }
    if( dynamic_cast<const MuPatTrack*>(fitterData.secondEntry) ) {
      fitterData.secondIsTrack = true;
      fitterData.secondHasMomentum = fitterData.secondEntry->hasMomentum();
    }
    // merge hitLists and add them to the fitterData
    MuPatHitList& hitList = fitterData.hitList;
    copyHitList(entry1.hitList(),fitterData.copyHitList1);
    copyHitList(entry2.hitList(),fitterData.copyHitList2);

    if( !m_hitHandler->merge(fitterData.copyHitList1,fitterData.copyHitList2,hitList) ) return false;

    bool usePrecise = m_usePreciseHits ? true : (fitterData.firstHasMomentum || fitterData.secondHasMomentum);
    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << MSG::DEBUG << " entering fitter: etaHits, first entry: ";
      if( fitterData.firstIsTrack ) msg(MSG::DEBUG) << " track ";
      else                          msg(MSG::DEBUG) << " segment ";
      msg(MSG::DEBUG) << fitterData.firstEntry->etaHits().size() << std::endl
             << m_hitHandler->print(fitterData.firstEntry->hitList()) << std::endl
             << "  second entry: ";
      if( fitterData.secondIsTrack ) msg(MSG::DEBUG) << " track ";
      else                           msg(MSG::DEBUG) << " segment ";
      msg(MSG::DEBUG) << fitterData.secondEntry->etaHits().size() << std::endl
             << m_hitHandler->print(fitterData.secondEntry->hitList()) << endmsg;
    }

    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << MSG::DEBUG << " merged hit lists, new list size:  " << hitList.size();
      if( usePrecise ) msg(MSG::DEBUG) << " using precise errors" << endmsg;
      if( msgLvl(MSG::VERBOSE) )  msg(MSG::DEBUG) << std::endl << m_hitHandler->print(hitList);
      msg(MSG::DEBUG) << endmsg;
    }

    return extractData( fitterData, usePrecise );
  }

  bool MooTrackFitter::extractData( MooTrackFitter::FitterData& fitterData, bool usePreciseHits ) const {

    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << MSG::DEBUG << " extracting hits from hit list, using ";
      if( usePreciseHits ) msg(MSG::DEBUG) << "precise measurements" << endmsg;
      else                 msg(MSG::DEBUG) << "broad measurements" << endmsg;
    }
    
    MuPatHitList& hitList = fitterData.hitList;
    // make sure the vector is sufficiently large
    unsigned int nhits = hitList.size();

    fitterData.measurements.clear();
    fitterData.firstLastMeasurements.clear();
    fitterData.etaHits.clear();
    fitterData.phiHits.clear();
    fitterData.measurements.reserve(nhits);
    fitterData.firstLastMeasurements.reserve(nhits);

    if( usePreciseHits && fitterData.startPars ) removeSegmentOutliers( fitterData );

    MuonStationIndex::StIndex firstStation   = MuonStationIndex::StUnknown;
    MuonStationIndex::ChIndex currentChIndex = MuonStationIndex::ChUnknown;
    bool                      currentMeasPhi = false;
    const MuPatHit*            previousHit = 0;

    // loop over hit list 
    MuPatHitCit lit = hitList.begin(), lit_end = hitList.end();
    for( ; lit!=lit_end; ++lit ){

      const MuPatHit& hit = **lit;
      const Identifier& id = hit.info().id;
      
      if( hit.info().status != MuPatHit::OnTrack || !id.is_valid() ) continue;

      MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(id);
      MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex( chIndex );
      fitterData.stations.insert(stIndex);
      
      if( firstStation == MuonStationIndex::StUnknown ) {
        if( msgLvl(MSG::VERBOSE) && !usePreciseHits && m_preciseFirstStation  ) msg(MSG::VERBOSE) << " Using precise errors for first station " << endmsg;
        firstStation = stIndex;
      }

      bool measuresPhi = m_idHelperTool->measuresPhi(id);

      if( !m_idHelperTool->isTgc(id) && !measuresPhi ){
        bool isSmall = m_idHelperTool->isSmallChamber(id);

        SLStationMap::iterator pos = fitterData.smallLargeChambersPerStation.find(stIndex);
        if( pos == fitterData.smallLargeChambersPerStation.end() ){
          int nSmall = isSmall ? 1 : 0;
          int nLarge = isSmall ? 0 : 1;
          fitterData.smallLargeChambersPerStation.insert( std::make_pair( stIndex, SmallLargeChambers(nSmall,nLarge) ) );
        }else{
          if( isSmall ) ++pos->second.first;
          else          ++pos->second.second;
        }
      }


      bool isEndcap = m_idHelperTool->isEndcap(id);
      if( isEndcap && !fitterData.hasEndcap ) fitterData.hasEndcap = true;
      if( !isEndcap && !fitterData.hasBarrel ) fitterData.hasBarrel = true;

      const Trk::MeasurementBase* meas = usePreciseHits ? &hit.preciseMeasurement() : &hit.broadMeasurement();

      // special treatment of hits in first stations on the track to stabalise the track fit
      if( !usePreciseHits && m_preciseFirstStation ) {
        meas = &hit.preciseMeasurement();
      }

      if( measuresPhi ) fitterData.phiHits.push_back(meas);
      else              fitterData.etaHits.push_back(meas);

      if( msgLvl(MSG::DEBUG) ){
        
        double rDrift = meas->localParameters()[Trk::locR];
        double rError = Amg::error(meas->localCovariance(),Trk::locR);
        if( usePreciseHits && m_idHelperTool->isMdt(id) && fabs(rDrift) < 0.01 && rError > 4. ) {
          ATH_MSG_WARNING(" MDT hit error broad but expected precise error " );
        }
      }
      
      fitterData.measurements.push_back(meas);

      if( !measuresPhi && m_idHelperTool->isTrigger(id) ) continue;

      if( lit == hitList.begin() ){
        currentChIndex = chIndex;
        currentMeasPhi = measuresPhi;
        fitterData.firstLastMeasurements.push_back(&hit.broadMeasurement());
        previousHit = 0;
      }else{
        // same chamber, store pointer to hit 
        if( currentChIndex == chIndex && currentMeasPhi == measuresPhi ) {
          previousHit = &hit;
        }else{
          // new chamber, add previous hit if set
          if( previousHit ) fitterData.firstLastMeasurements.push_back( &previousHit->broadMeasurement() );
          currentChIndex = chIndex;
          currentMeasPhi = measuresPhi;
          fitterData.firstLastMeasurements.push_back(&hit.broadMeasurement());
          previousHit = 0; // reset pointer
        }
      }
    }

    // add last hit if not already inserted
    if( previousHit && &previousHit->broadMeasurement() != fitterData.firstLastMeasurements.back() )  
      fitterData.firstLastMeasurements.push_back( &previousHit->broadMeasurement() );

    // require at least 6 measurements on a track
    if( fitterData.measurements.size() < 7 ){
      if( msgLvl(MSG::VERBOSE) )  msg(MSG::VERBOSE) << " Too few measurements, cannot perform fit  " << fitterData.measurements.size() << endmsg;
      return false;
    }

    // require at least 6 measurements on a track
    if( fitterData.etaHits.size() < 7 ){
      if( msgLvl(MSG::VERBOSE) )  msg(MSG::VERBOSE) << " Too few eta measurements, cannot perform fit  " << fitterData.etaHits.size() << endmsg;
      return false;
    }

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << MSG::DEBUG << " Extracted measurements: total " << fitterData.measurements.size() << "  eta " << fitterData.etaHits.size()
             << "  phi " << fitterData.phiHits.size();
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::DEBUG) << std::endl << m_printer->print(fitterData.measurements) << endmsg;
      else            msg(MSG::DEBUG) << endmsg;
    }


    return true;
  }
 
  bool MooTrackFitter::addFakePhiHits( MooTrackFitter::FitterData& fitterData, const Trk::TrackParameters* startpar ) const {
    
    // check whether we have enough phi constraints
    unsigned nphiConstraints = hasPhiConstrain(fitterData);
    
    // do we have enough constraints to fit the track
    if( nphiConstraints >= 2 ) {
      if( fitterData.firstEntry->stations().size() == 1 && fitterData.firstEntry->containsStation(MuonStationIndex::EI) && 
          ( fitterData.firstEntry->phiHits().empty() || 
            (fitterData.firstEntry->containsChamber(MuonStationIndex::CSS) || 
             fitterData.firstEntry->containsChamber(MuonStationIndex::CSL) ) ) ){
        if( msgLvl(MSG::VERBOSE) ) msg(MSG::DEBUG) << MSG::DEBUG << " Special treatment of the forward region: adding fake at ip " << endmsg;
      }
      return true;
    }
    
    // in case of a single station overlap fit, calculate a global position that is more or less located at 
    // the overlap. It is used to decide on which side of the tube the fake should be produced
    Amg::Vector3D* overlapPos = 0;
    const Amg::Vector3D* phiPos = 0;
    if( fitterData.numberOfSLOverlaps() > 0 || (fitterData.numberOfSmallChambers() > 0 && fitterData.numberOfLargeChambers() > 0 ) ){
      
      
      
      // in case of SL overlaps, pass average position of the two segments 
//       overlapPos = new Amg::Vector3D( 0.5*(fitterData.firstEntry->etaHits().front()->globalPosition() + 
//                                               fitterData.secondEntry->etaHits().front()->globalPosition()) );
      overlapPos = new Amg::Vector3D(1.,1.,1.);
      double phi = fitterData.avePhi;
      double theta = fitterData.secondEntry->etaHits().front()->globalPosition().theta();
      if( m_cosmics ){
        if( fitterData.firstEntry->hasSLOverlap() ){
	  theta = fitterData.firstEntry->entryPars().momentum().theta();
	  phi = fitterData.firstEntry->entryPars().momentum().phi();
        }else{
	  theta = fitterData.secondEntry->entryPars().momentum().theta();
	  phi = fitterData.secondEntry->entryPars().momentum().phi();
        }
      }
      Amg::setThetaPhi(*overlapPos,theta,phi);
      
      
      if( fitterData.startPars ) {
        phiPos = new Amg::Vector3D( fitterData.startPars->momentum() );
      }else{
        phiPos = new Amg::Vector3D( *overlapPos );
      }

      if( msgLvl(MSG::VERBOSE) ){
        if( fitterData.numberOfSLOverlaps() > 0 ){
          if( fitterData.stations.size() == 1 ) msg(MSG::VERBOSE) << " one station fit with SL overlap, using overlapPos " << *overlapPos << endmsg;
          else                                  msg(MSG::VERBOSE) << " multi station fit with SL overlap, using overlapPos " << *overlapPos << endmsg;
        }else if(fitterData.numberOfSmallChambers() > 0 && fitterData.numberOfLargeChambers() > 0 ) {
          msg(MSG::VERBOSE) << " multi station fit, SL overlap not in same station, using overlapPos " << *overlapPos << endmsg;
        }else ATH_MSG_WARNING(" Unknown overlap type " );
      }
    }

    //if( fitterData.numberOfSLOverlaps() > 1 ) ATH_MSG_WARNING(" unexpected number of overlaps " );
    
    // add fake phi hit on first and last measurement
    if( fitterData.phiHits.empty() ){
      const MuPatSegment* segInfo1 = dynamic_cast<const MuPatSegment*>(fitterData.firstEntry);
      const MuPatSegment* segInfo2 = dynamic_cast<const MuPatSegment*>(fitterData.secondEntry);
      if( segInfo1 && segInfo2 && fitterData.stations.size() == 1 && fitterData.numberOfSLOverlaps()==1 ){

        delete phiPos; phiPos=0;
        delete overlapPos;overlapPos=0;

        // only perform SL overlap fit for MDT segments
        Identifier chId = m_helperTool->chamberId(*segInfo1->segment);
        if( !m_idHelperTool->isMdt(chId) ) return false;

        ATH_MSG_VERBOSE(" Special treatment for tracks with one station, a SL overlap and no phi hits " );
        
        IMuonSegmentInOverlapResolvingTool::SegmentMatchResult result = m_overlapResolver->matchResult(*segInfo1->segment,*segInfo2->segment);

        if( !result.goodMatch() ) {
          ATH_MSG_VERBOSE(" Match failed " );
          return false;
        }
        // create a track parameter for the segment
        double locx1 = result.segmentResult1.positionInTube1;
        double locy1 = segInfo1->segment->localParameters().contains(Trk::locY) ? segInfo1->segment->localParameters()[Trk::locY] : 0.;
        Trk::AtaPlane segPars1(locx1,locy1,result.phiResult.segmentDirection1.phi(),result.phiResult.segmentDirection1.theta(),
                               0.,segInfo1->segment->associatedSurface());
        const Trk::TrackParameters* exPars1 = m_propagator->propagate(segPars1,fitterData.measurements.front()->associatedSurface(),
                                                                      Trk::anyDirection,false,m_magFieldProperties);
        if( exPars1 ){
          const Trk::MeasurementBase* fake = createFakePhiForMeasurement(*fitterData.measurements.front(),&exPars1->position(),0,10.);
          delete exPars1;
          if( fake ) {
            fitterData.phiHits.push_back(fake);
            fitterData.measurements.insert(fitterData.measurements.begin(),fake);
            fitterData.firstLastMeasurements.insert(fitterData.firstLastMeasurements.begin(),fake);
          }
        }else{
          ATH_MSG_WARNING(" failed to create fake for first segment " );
	  return false;
        }
        double locx2 = result.segmentResult2.positionInTube1;
        double locy2 = segInfo2->segment->localParameters().contains(Trk::locY) ? segInfo2->segment->localParameters()[Trk::locY] : 0.;
        Trk::AtaPlane segPars2(locx2,locy2,result.phiResult.segmentDirection2.phi(),result.phiResult.segmentDirection2.theta(),
                               0.,segInfo2->segment->associatedSurface());
        const Trk::TrackParameters* exPars2 = m_propagator->propagate(segPars2,fitterData.measurements.back()->associatedSurface(),
                                                                       Trk::anyDirection,false,m_magFieldProperties);
        if( exPars2 ){
          const Trk::MeasurementBase* fake = createFakePhiForMeasurement(*fitterData.measurements.back(),&exPars2->position(),0,10.);
          delete exPars2;
          if( fake ){
            fitterData.phiHits.push_back(fake);
            fitterData.measurements.push_back(fake);
            fitterData.firstLastMeasurements.push_back(fake);
          } 
        }else{
          ATH_MSG_WARNING(" failed to create fake for second segment " );
          return false;
        }
      }else if( nphiConstraints == 0 || 
          (fitterData.stations.size() == 1) || 
          (nphiConstraints==1 && fitterData.numberOfSLOverlaps()==0 && 
           fitterData.numberOfSmallChambers() > 0 && fitterData.numberOfLargeChambers() > 0) ){
        const Trk::MeasurementBase* fake = createFakePhiForMeasurement(*fitterData.measurements.front(),overlapPos,phiPos,100.);
        if( fake ) {
          fitterData.phiHits.push_back(fake);
          fitterData.measurements.insert(fitterData.measurements.begin(),fake);
          fitterData.firstLastMeasurements.insert(fitterData.firstLastMeasurements.begin(),fake);
        }
        fake = createFakePhiForMeasurement(*fitterData.measurements.back(),overlapPos,phiPos,100.);
        if( fake ){
          fitterData.phiHits.push_back(fake);
          fitterData.measurements.push_back(fake);
          fitterData.firstLastMeasurements.push_back(fake);
        }
      }else if( fitterData.numberOfSLOverlaps() == 1 ){
        // there is one overlap, add the fake in the station without the overlap
        ATH_MSG_VERBOSE(" Special treatment for tracks with one SL overlap and no phi hits " );

        MuonStationIndex::StIndex overlapStation = MuonStationIndex::StUnknown;  
        SLStationMap::iterator it = fitterData.smallLargeChambersPerStation.begin();
        SLStationMap::iterator it_end = fitterData.smallLargeChambersPerStation.end();
        for( ;it!=it_end;++it ){
          if( it->second.first && it->second.second ) {
            overlapStation = it->first;
            break;
          }
        }
        if( overlapStation == MuonStationIndex::StUnknown ){
         ATH_MSG_WARNING(" unexpected condition, unknown station type " );
	 delete phiPos; phiPos=0;
	 delete overlapPos;overlapPos=0;
         return false;
        }
        
        Identifier firstId = m_helperTool->getIdentifier(*fitterData.measurements.front());
        if( !firstId.is_valid() ) {
         ATH_MSG_WARNING(" unexpected condition, first measurement has no identifier " );
	 delete phiPos; phiPos=0;
	 delete overlapPos;overlapPos=0;
         return false;
        }
        MuonStationIndex::StIndex firstSt = m_idHelperTool->stationIndex(firstId);
        if( overlapStation == firstSt ){
          ATH_MSG_VERBOSE(" Adding fake in same station as overlap " );

          // create pseudo at end of track
          const Trk::MeasurementBase* fake = createFakePhiForMeasurement(*fitterData.measurements.back(),overlapPos,phiPos,100.);
          if( fake ){
            fitterData.phiHits.push_back(fake);
            fitterData.measurements.push_back(fake);
            fitterData.firstLastMeasurements.push_back(fake);
          }
        }else{
          ATH_MSG_VERBOSE(" Adding fake in other station as overlap " );
          // create pseudo at begin of track
          const Trk::MeasurementBase* fake = createFakePhiForMeasurement(*fitterData.measurements.front(),overlapPos,phiPos,100.);
          if( fake ) {
            fitterData.phiHits.push_back(fake);
            fitterData.measurements.insert(fitterData.measurements.begin(),fake);
            fitterData.firstLastMeasurements.insert(fitterData.firstLastMeasurements.begin(),fake);
          }
        } 

      }else{
	ATH_MSG_WARNING(" unexpected condition, cannot create fakes " );
        delete phiPos; phiPos=0;
        delete overlapPos;overlapPos=0;
	return false;
      }
    }else{
      // calculate distance between first phi/eta hit and last phi/eta hit
      double distFirstEtaPhi = (fitterData.measurements.front()->globalPosition() -
                                fitterData.phiHits.front()->globalPosition() ).mag();
      double distLastEtaPhi  = (fitterData.measurements.back()->globalPosition() -
                                fitterData.phiHits.back()->globalPosition() ).mag();


      // if there is one phi, use its phi + IP constraint to calculate position of fake 
      if( !overlapPos && m_seedWithAvePhi ){
	phiPos = new Amg::Vector3D(fitterData.phiHits.back()->globalPosition());
	ATH_MSG_VERBOSE(" using pointing constraint to calculate fake phi hit ");
      }

      // create a fake on the first and/or last MDT measurement if not near phi hits
      const Trk::MeasurementBase* fake = 0;
      const Trk::Surface *firstmdtsurf=0,*lastmdtsurf=0;
      const Trk::TrackParameters *lastmdtpar=0,*firstpar=0,*lastpar=0;
      int indexfirst=0,indexlast=(int)fitterData.measurements.size();
      MuPatHitCit hitit=fitterData.hitList.begin();
      for (;hitit!=fitterData.hitList.end();hitit++){
        if (!firstpar) firstpar=&(**hitit).parameters();

        if ((**hitit).info().measuresPhi) break;
	if ((**hitit).info().type==MuPatHit::MDT) {
	  firstmdtsurf=&(**hitit).measurement().associatedSurface();
	  break;
	}
	indexfirst++;
      }
      hitit=fitterData.hitList.end();
      hitit--;
      for (;hitit!=fitterData.hitList.begin();hitit--){
        if (!lastpar) lastpar=&(**hitit).parameters();

        if ((**hitit).info().measuresPhi) break;
	if ((**hitit).info().type==MuPatHit::MDT) {
	  lastmdtsurf=&(**hitit).measurement().associatedSurface();
          lastmdtpar=&(**hitit).parameters();
	  break;
	}
	indexlast--;
      }
      bool phifromextrapolation=false;
      if (!fitterData.secondEntry->phiHits().empty() || !fitterData.firstEntry->phiHits().empty() ) phifromextrapolation=true; 

      const Trk::Surface *firstphisurf=0,*lastphisurf=0;
      Amg::Vector3D firstphinormal,lastphinormal;
      if (!fitterData.phiHits.empty()) {
        firstphisurf=&fitterData.phiHits.front()->associatedSurface();
        lastphisurf=&fitterData.phiHits.back()->associatedSurface();
        firstphinormal=firstphisurf->normal();
        lastphinormal=lastphisurf->normal();
        if (firstphinormal.dot(startpar->momentum())<0) firstphinormal=-firstphinormal;
        if (lastphinormal.dot(startpar->momentum())<0) lastphinormal=-lastphinormal;
      }
      if (lastmdtsurf && (!firstphisurf || (lastmdtsurf->center()-lastphisurf->center()).dot(lastphinormal)>1000)){
	ATH_MSG_VERBOSE(" Adding fake at last hit: dist first phi/first eta " << distFirstEtaPhi
			<< " dist last phi/last eta " << distLastEtaPhi);
        if (fitterData.secondEntry->hasSLOverlap() || phifromextrapolation) {

          const Trk::TrackParameters *mdtpar=0;
          if (fitterData.secondEntry->hasSLOverlap()) mdtpar=lastmdtpar->clone();
          else mdtpar=m_propagator->propagateParameters(*startpar,*lastmdtsurf,Trk::alongMomentum,false,m_magFieldProperties);
          if (mdtpar){
            Amg::MatrixX cov(1,1);
	    cov(0,0) = 100.;
            fake = new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters(Trk::DefinedParameter(mdtpar->parameters()[Trk::locY],Trk::locY)),
						     cov,mdtpar->associatedSurface());
            delete mdtpar;
            m_measurementsToBeDeleted.push_back(fake);
          }
        }
        else fake = createFakePhiForMeasurement(*fitterData.measurements.back(),overlapPos,phiPos,10.);
        if ( fake ) {
          fitterData.phiHits.push_back(fake);
          fitterData.measurements.insert(fitterData.measurements.begin()+indexlast,fake);
          fitterData.firstLastMeasurements.push_back(fake);
        }
      }  
    
      if (firstmdtsurf && (!firstphisurf || (firstmdtsurf->center()-firstphisurf->center()).dot(firstphinormal)<-1000)) {

	ATH_MSG_VERBOSE(" Adding fake at first hit: dist first phi/first eta " << distFirstEtaPhi
			<< " dist last phi/last eta " << distLastEtaPhi);
        if (phifromextrapolation) {
          const Trk::TrackParameters *mdtpar=m_propagator->propagateParameters(*startpar,*firstmdtsurf,Trk::oppositeMomentum,false,m_magFieldProperties);
          if (mdtpar) {
            Amg::MatrixX cov(1,1);
	    cov(0,0) = 100.;
            fake = new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters(Trk::DefinedParameter(mdtpar->parameters()[Trk::locY],Trk::locY)),
						     cov,mdtpar->associatedSurface());
            m_measurementsToBeDeleted.push_back(fake);
            delete mdtpar;
          }
        }
        else fake = createFakePhiForMeasurement(*fitterData.measurements.front(),overlapPos,phiPos,100.);
        if ( fake ) {
          fitterData.phiHits.insert(fitterData.phiHits.begin(),fake);
          fitterData.measurements.insert(fitterData.measurements.begin()+indexfirst,fake);
          fitterData.firstLastMeasurements.insert(fitterData.firstLastMeasurements.begin(),fake);
        }
      }
    }

    delete phiPos;
    delete overlapPos;

    return true;
  } 

  const Trk::MeasurementBase* MooTrackFitter::createFakePhiForMeasurement( const Trk::MeasurementBase& meas, 
                                                                            const Amg::Vector3D* overlapPos, 
                                                                            const Amg::Vector3D* phiPos,
                                                                            double errPos ) const {
    
    
    // check whether measuring phi
    Identifier id = m_helperTool->getIdentifier( meas );
    if( id == Identifier () ) {
      ATH_MSG_WARNING(" Cannot create fake phi hit from a measurement without Identifier " );
      return 0;
    }
    // check whether phi measurement, exit if true
    if( m_idHelperTool->measuresPhi(id) ) {
      ATH_MSG_WARNING(" Measurement is a phi measurement! " << m_idHelperTool->toString(id) );
      return 0;
    }
    double length = 1e9; // initialize to large value
    
    // the MDT and CSC hits are ROTs
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
    if( !rot) {
      const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&meas);
      // if also to a crot we cannot create a fake phi hit
      if( !crot || crot->containedROTs().empty()) return 0;
      rot = &crot->rioOnTrack(0);
    }

    bool isMdt = false;

    // get channel length and detector element
    const Trk::TrkDetElementBase* detEl = 0;
    if( !rot ){
      ATH_MSG_WARNING(" Measurement not a ROT "<< m_idHelperTool->toString(id) );
      return 0;
    }

    if( m_idHelperTool->mdtIdHelper().is_mdt(id) ){
      isMdt = true;
      const MuonGM::MdtReadoutElement* mdtDetEl = dynamic_cast<const MuonGM::MdtReadoutElement*>(rot->detectorElement());
      if( !mdtDetEl ) {
        ATH_MSG_WARNING(" MDT without MdtReadoutElement! " );
        return 0;
      }
      detEl = mdtDetEl;
      int layer = m_idHelperTool->mdtIdHelper().tubeLayer(id);
      int tube  = m_idHelperTool->mdtIdHelper().tube(id);
      length    = mdtDetEl->getActiveTubeLength(layer,tube);
    }else if( m_idHelperTool->isCsc(id) ){
      const MuonGM::CscReadoutElement* cscDetEl = dynamic_cast<const MuonGM::CscReadoutElement*>(rot->detectorElement());
      if( !cscDetEl ) {
        ATH_MSG_WARNING(" CSC without CscReadoutElement! " );
        return 0;
      }
      detEl = cscDetEl;
      length = cscDetEl->stripLength(id);
    }else if( m_idHelperTool->isRpc(id) ){
      const MuonGM::RpcReadoutElement* rpcDetEl = dynamic_cast<const MuonGM::RpcReadoutElement*>(rot->detectorElement());
      if( !rpcDetEl ) {
        ATH_MSG_WARNING(" RPC without RpcReadoutElement! " );
        return 0;
      }
      detEl = rpcDetEl;
      length = rpcDetEl->StripLength(false); // eta-strip
    }else if( m_idHelperTool->isTgc(id) ){
      const MuonGM::TgcReadoutElement* tgcDetEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(rot->detectorElement());
      if( !tgcDetEl ) {
        ATH_MSG_WARNING(" TGC without TgcReadoutElement! " );
        return 0;
      }
      detEl = tgcDetEl;
      length = tgcDetEl->WireLength(m_idHelperTool->tgcIdHelper().gasGap(id),m_idHelperTool->tgcIdHelper().channel(id));
    }else if( m_idHelperTool->issTgc(id) ){
      const MuonGM::sTgcReadoutElement* stgcDetEl = dynamic_cast<const MuonGM::sTgcReadoutElement*>(rot->detectorElement());
      if( !stgcDetEl || !stgcDetEl->getDesign(id) ) {
        ATH_MSG_WARNING(" STGC without sTgcReadoutElement! " );
        return 0;
      }
      detEl = stgcDetEl;
      const MuonGM::MuonChannelDesign* design = stgcDetEl->getDesign(id);
      length = design->channelLength( m_idHelperTool->stgcIdHelper().channel(id) );
    }else if( m_idHelperTool->isMM(id) ){
      const MuonGM::MMReadoutElement* mmDetEl = dynamic_cast<const MuonGM::MMReadoutElement*>(rot->detectorElement());
      if( !mmDetEl || !mmDetEl->getDesign(id) ) {
        ATH_MSG_WARNING(" STGC without sTgcReadoutElement! " );
        return 0;
      }
      detEl = mmDetEl;
      const MuonGM::MuonChannelDesign* design = mmDetEl->getDesign(id);
      length = design->channelLength( m_idHelperTool->mmIdHelper().channel(id) );
    }

    // we need the detEl and channel length
    if( !detEl ) {
      ATH_MSG_WARNING(" no detector element found for measurement: "<< m_idHelperTool->toString(id) );
      return 0;
    }

    // error 
    const Amg::Vector2D* lpos = 0;
    //if( phiPos && !overlapPos ){
    if( phiPos ){

      Amg::Vector3D dir(1.,0.,0.);
      Amg::setThetaPhi(dir,rot->globalPosition().theta(),phiPos->phi());
      Amg::Vector3D ip(0.,0.,0.);
      Trk::Perigee perigee(ip,dir,0.,ip);
      
      // special treatment of MDTs, intersect with detector element surface instead of wire surface to avoid problem with 
      // shift of phi angle after projection into wire frame 
      const Trk::Surface& measSurf = isMdt ? detEl->surface() : meas.associatedSurface();

      Trk::Intersection intersect = measSurf.straightLineIntersection(ip,dir);
      if( !intersect.valid ){
	ATH_MSG_WARNING(" Intersect with surface failed for measurement "<< m_printer->print(meas)
			<< " position " << Amg::toString(ip) 
			<< " direction: phi " << dir.phi() << " theta " << dir.theta() 
			<< " seed: phi " << phiPos->phi() << " theta " << rot->globalPosition().theta() );
      }else{

	// now get central phi position on surface of segment
	lpos = meas.associatedSurface().globalToLocal(intersect.position,3000.);

        ATH_MSG_VERBOSE(" Used intersect with surface " << intersect.position.phi() 
			<< "  start phi " << phiPos->phi() );

      }
    }else{
      // now get central phi position on surface of segment
      lpos = meas.associatedSurface().globalToLocal(detEl->center(),3000.);

    }
      


    if( !lpos ){
      ATH_MSG_WARNING(" no local position, cannot make fake phi hit " );
      return 0;
    }
    double ly = (*lpos)[Trk::locY];
    delete lpos;

    bool shiftedPos = false;
    double halfLength = 0.5*length;
    if( fabs(ly) > halfLength ){
      double lyold = ly;
      ly = ly < 0 ? -halfLength : halfLength;
      ATH_MSG_DEBUG(" extrapolated position outside detector, shifting it back:  " << lyold << "  size " << halfLength << "  new pos " << ly);
      if( phiPos && fabs(lyold) - halfLength > 1000. ) {
        ATH_MSG_DEBUG(" rejecting track ");
        return 0;
      }
      shiftedPos = true;
    }

    ATH_MSG_VERBOSE("Creating fake measurement using measurement " << m_printer->print(meas) );

    const Trk::Surface& surf = meas.associatedSurface();

    // no overlap, put fake phi hit at center of the chamber
    if( overlapPos ) {

      // overlap, fake hit at 100. mm from the edge, error 100./sqrt(12.)
  
      // transform estimate of overlap position into surface frame
      const Amg::Vector2D* loverlapPos = surf.globalToLocal(*overlapPos, 3000.);
      if( !loverlapPos ){
        ATH_MSG_DEBUG(" globalToLocal failed for overlap position: " << surf.transform()*(*overlapPos) );
        // calculate position fake
        double lyfake = 0.5*length - 50.;
        
        Amg::Vector2D locpos_plus(0.,lyfake);
        const Amg::Vector3D* fakePos_plus = meas.associatedSurface().localToGlobal(locpos_plus);
        Amg::Vector2D locpos_min(0.,-lyfake);
        const Amg::Vector3D* fakePos_min = meas.associatedSurface().localToGlobal(locpos_min);
        
        if( !fakePos_min || !fakePos_plus ) {
          delete fakePos_plus;
          delete fakePos_min;
          ATH_MSG_WARNING(" Failed to calculate right and left phi position " );
          return 0;
        }
        
        double phi_min = fakePos_min->phi();
        double phi_plus = fakePos_plus->phi();
        delete fakePos_plus;
        delete fakePos_min;

        double phi_overlap = phiPos->phi();
        //       if( (*loverlapPos)[Trk::loc2] < 0. ) lyfake *= -1.; 
        if( ly < 0. ) lyfake *= -1.; 
        
        ly = lyfake;
        errPos = 50./sqrt(12);
        ATH_MSG_VERBOSE(" fake lpos " << lyfake << " ch half length " 
                               << 0.5*length << " phi+ " << phi_plus << " phi- " << phi_min << " phi overlap " << phi_overlap
                               << " err " << errPos );
      }else{
        ly = (*loverlapPos)[Trk::locY];

        halfLength = 0.5*length;
        if( fabs(ly) > halfLength ){
          ly = ly < 0 ? -halfLength : halfLength;
        }
        ATH_MSG_VERBOSE(" fake from overlap: lpos " << ly << " ch half length " 
                               << 0.5*length << " overlapPos " << *overlapPos );
        delete loverlapPos;
      }
    }

    Trk::LocalParameters locPars(Trk::DefinedParameter(ly,Trk::locY));
    // Error matrix
    Amg::MatrixX cov(1,1);
    cov(0,0) = errPos*errPos;
    Trk::PseudoMeasurementOnTrack* fake = new Trk::PseudoMeasurementOnTrack(locPars,cov,surf);
    
    // should be deleted before exiting, add to list 
    m_measurementsToBeDeleted.push_back(fake);

    if( msgLvl(MSG::DEBUG) ) {
    
      Amg::Vector2D locpos(0.,fake->localParameters().get(Trk::locY));
      const Amg::Vector3D* fakePos = meas.associatedSurface().localToGlobal(locpos);

      if( fakePos ){
        msg(MSG::DEBUG) << MSG::DEBUG << " createFakePhiForMeasurement for:  " << m_idHelperTool->toStringChamber( id ) 
               << "   locY " << ly
               << "  errpr " << errPos << " phi " << fakePos->phi() << endmsg;
        
        if( !shiftedPos && !overlapPos && phiPos && fakePos && fabs( phiPos->phi() - fakePos->phi() ) > 0.01 ){
	  Amg::Transform3D gToLocal = meas.associatedSurface().transform().inverse();
          Amg::Vector3D locMeas = gToLocal*fake->globalPosition();
          ATH_MSG_WARNING(" Problem calculating fake from IP seed: phi fake " 
			  << fakePos->phi() << "  IP phi " << phiPos->phi() << " local meas pos " << locMeas );
        }
        delete fakePos;
      }
    }

    return fake;
  }


  unsigned int MooTrackFitter::hasPhiConstrain( MooTrackFitter::FitterData& fitterData) const {
    // check distance between first and last hit to determine whether we need additional phi constrainst

    if ((fitterData.firstEntry->containsChamber(MuonStationIndex::CSS) ||
	 fitterData.firstEntry->containsChamber(MuonStationIndex::CSL)) && !fitterData.secondEntry->phiHits().empty()) return 2;

    unsigned int nphiConstraints = fitterData.phiHits.size();
    // count SL overlap as one phi constraint
//     if( fitterData.numberOfSLOverlaps() > 0 ) {
//       nphiConstraints += fitterData.numberOfSLOverlaps();
//       ATH_MSG_VERBOSE(" track has small/large overlaps " << fitterData.numberOfSLOverlaps() );
//     }else if( (fitterData.numberOfSmallChambers() > 0 && fitterData.numberOfLargeChambers() > 0 ) ){
//       nphiConstraints += 1;
//       ATH_MSG_VERBOSE(" track has small and large chambers " );
//     }

    double distanceMin = 400.;
    double distance = 0.;
    // we need at least two phis
    if( fitterData.phiHits.size() > 1 ) {

      // assume the hits to be sorted
      const Amg::Vector3D& gposFirstPhi = fitterData.phiHits.front()->globalPosition();
      const Amg::Vector3D& gposLastPhi  = fitterData.phiHits.back()->globalPosition();
      double distFirstEtaPhi = (fitterData.measurements.front()->globalPosition() -
				fitterData.phiHits.front()->globalPosition() ).mag();
      double distLastEtaPhi  = (fitterData.measurements.back()->globalPosition() -
				fitterData.phiHits.back()->globalPosition() ).mag();
      
      // calculate difference between hits
      Amg::Vector3D globalDistance = gposLastPhi - gposFirstPhi;

      // calculate 'projective' distance
      distance = fitterData.hasEndcap ? fabs(globalDistance.z()) : globalDistance.perp();
 
      // if the distance between the first and last phi hit is smaller than 1000. count as 1 phi hit
      if( distance < distanceMin || distFirstEtaPhi>1000 || distLastEtaPhi>1000) {
	nphiConstraints -= fitterData.phiHits.size(); // subtract phi hits as they should only count as one phi hit
	nphiConstraints += 1;                        // add one phi hit
	ATH_MSG_VERBOSE(" distance between phi hits too small, updating phi constraints ");
      }else{
	ATH_MSG_VERBOSE(" distance between phi hits sufficient, no fake hits needed ");
      }
    }
    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << MSG::DEBUG << " Check phi: " << std::endl
             << " | nphi hits | distance | SL station overlaps | small ch | large ch | nphiConstraints " << std::endl
             << std::setw(12) <<  fitterData.phiHits.size() << std::setw(11) << (int)distance 
             << std::setw(22) << fitterData.numberOfSLOverlaps() << std::setw(11) << fitterData.numberOfSmallChambers()
             << std::setw(11) << fitterData.numberOfLargeChambers() << std::setw(18) << nphiConstraints << endmsg;
    }

    return nphiConstraints;
  }

  unsigned int MooTrackFitter::hasPhiConstrain( const Trk::Track& track ) const {

    std::map<MuonStationIndex::StIndex,StationPhiData> stationDataMap;
    
    const Trk::TrackSummary* summary = track.trackSummary();
    Trk::MuonTrackSummary muonSummary;
    if( summary ){
      if( summary->muonTrackSummary() ) muonSummary = *summary->muonTrackSummary();
      else{
	Trk::TrackSummary* tmpSum = const_cast<Trk::TrackSummary*>(summary);
	if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(track,*tmpSum);
	if( tmpSum->muonTrackSummary() ) muonSummary = *tmpSum->muonTrackSummary();
      }
    }else{
      Trk::TrackSummary tmpSummary;
      m_trackSummaryTool->addDetailedTrackSummary(track,tmpSummary);
      if( tmpSummary.muonTrackSummary() ) muonSummary = *tmpSummary.muonTrackSummary();
    }

    std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit = muonSummary.chamberHitSummary().begin();
    std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit_end = muonSummary.chamberHitSummary().end();
    for( ;chit!=chit_end;++chit ){

      // get station data
      const Identifier& chId = chit->chamberId();
      MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(chId);
      StationPhiData& stData = stationDataMap[stIndex];
      if( chit->isMdt() ){
        if( m_idHelperTool->isSmallChamber(chId) ) ++stData.nSmallChambers;
        else                                       ++stData.nLargeChambers;
      }else{
        if( chit->phiProjection().nhits ) ++stData.nphiHits;
      }
    }
    
    unsigned int phiConstraints = 0;
    unsigned int stationsWithSmall = 0;
    unsigned int stationsWithLarge = 0;

    std::map<MuonStationIndex::StIndex,StationPhiData>::iterator sit = stationDataMap.begin();
    std::map<MuonStationIndex::StIndex,StationPhiData>::iterator sit_end = stationDataMap.end();
    for( ;sit!=sit_end;++sit ){
      StationPhiData& stData = sit->second;
      
      // count all stations with phi hits as phi constraint
      phiConstraints += stData.nphiHits;

      if( stData.nSmallChambers > 0 && stData.nLargeChambers > 0 ) ++phiConstraints;
      else if( stData.nSmallChambers > 0  ) ++stationsWithSmall;
      else if( stData.nLargeChambers > 0  ) ++stationsWithLarge;
    }

    if( stationsWithSmall > 0 && stationsWithLarge > 0 ) {
      ++phiConstraints;
    }

    return phiConstraints;
  }


  bool MooTrackFitter::getMinMaxPhi( FitterData& fitterData ) const {

    double phiStart = fitterData.etaHits.front()->globalPosition().phi();
    double phiOffset = 0.;
    double pi = M_PI;
    double phiRange = 0.75*pi;
    double phiRange2 = 0.25*pi;
    if( phiStart > phiRange || phiStart < -phiRange ) phiOffset = 2*pi;
    else if( phiStart > -phiRange2 && phiStart < phiRange2 ) phiOffset = pi;

    double phiMin = -999.;
    double phiMax =  999.;

    MeasCit hit = fitterData.etaHits.begin();
    MeasCit hit_end = fitterData.etaHits.end();
    for( ;hit!=hit_end; ++hit ) {
      
      const Trk::MeasurementBase& meas = **hit;
      
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
      if( !rot) {
        const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&meas);
        // if also to a crot we cannot create a fake phi hit
        if( !crot || crot->containedROTs().empty()) continue;
        rot = crot->containedROTs().front();
      }

      if( !rot ){
        ATH_MSG_WARNING(" Measurement not a ROT " );
        continue;
      }

      Identifier id = rot->identify();
      const Trk::Surface& surf = meas.associatedSurface();
      double x =  rot->localParameters()[Trk::locX];
      double halfLength = 0.;

      if( m_idHelperTool->isMdt(id) ){
	const MuonGM::MdtReadoutElement* mdtDetEl = dynamic_cast<const MuonGM::MdtReadoutElement*>(rot->detectorElement());
	if( mdtDetEl ) {
	  int layer = m_idHelperTool->mdtIdHelper().tubeLayer(id);
	  int tube  = m_idHelperTool->mdtIdHelper().tube(id);
	  halfLength = 0.5*mdtDetEl->getActiveTubeLength(layer,tube);
	}
      }else if( m_idHelperTool->isCsc(id) ) {
        const MuonGM::CscReadoutElement* cscDetEl = dynamic_cast<const MuonGM::CscReadoutElement*>(rot->detectorElement());
        if( cscDetEl ) {
          halfLength = 0.5*cscDetEl->stripLength(id);
        }
      }else if( m_idHelperTool->isRpc(id) ) {
	const MuonGM::RpcReadoutElement* rpcDetEl = dynamic_cast<const MuonGM::RpcReadoutElement*>(rot->detectorElement());
	if( rpcDetEl ) {
	  halfLength = 0.5*rpcDetEl->StripLength(false); // eta-strip
	}
      }else if( m_idHelperTool->isTgc(id) ){
	const MuonGM::TgcReadoutElement* tgcDetEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(rot->detectorElement());
	if( tgcDetEl ) {
	  halfLength = 0.5*tgcDetEl->WireLength(m_idHelperTool->tgcIdHelper().gasGap(id),m_idHelperTool->tgcIdHelper().channel(id));
	}
      }else if( m_idHelperTool->issTgc(id) ){
	const MuonGM::sTgcReadoutElement* stgcDetEl = dynamic_cast<const MuonGM::sTgcReadoutElement*>(rot->detectorElement());
	if( stgcDetEl ){
	  const MuonGM::MuonChannelDesign* design = stgcDetEl->getDesign(id);
	  if( design ){
	    halfLength = 0.5*design->channelLength( m_idHelperTool->stgcIdHelper().channel(id) );
	  }
	}
      }else if( m_idHelperTool->isMM(id) ){
	const MuonGM::MMReadoutElement* mmDetEl = dynamic_cast<const MuonGM::MMReadoutElement*>(rot->detectorElement());
	if( mmDetEl ) {
	  const MuonGM::MuonChannelDesign* design = mmDetEl->getDesign(id);
	  if( design ){
	    halfLength = 0.5*design->channelLength( m_idHelperTool->mmIdHelper().channel(id) );
	  }
	}
      }
      Amg::Vector2D lpLeft(x,-halfLength);
      const Amg::Vector3D* gposLeft = surf.localToGlobal(lpLeft);
      if( !gposLeft ){
        ATH_MSG_WARNING(" Failed calculation left phi for "<< m_idHelperTool->toString(id) );
        continue;
      }
      double phiLeft = gposLeft->phi();
      delete gposLeft;

      Amg::Vector2D lpRight(x,halfLength);
      const Amg::Vector3D* gposRight = surf.localToGlobal(lpRight);
      if( !gposRight ){
        ATH_MSG_WARNING(" Failed calculation right phi for "<< m_idHelperTool->toString(id) );
        continue;
      }
      double phiRight = gposRight->phi();
      delete gposRight;

      if( phiOffset > 1.5*pi ){
        if( phiLeft < 0 ) phiLeft = phiOffset + phiLeft;
        if( phiRight < 0 ) phiRight = phiOffset + phiRight;
      }else if( phiOffset > 0. ) {
        phiLeft = phiLeft + phiOffset;
        phiRight = phiRight + phiOffset;
      }
      
      bool leftSmaller = phiLeft < phiRight;

      double phiMinMeas   = leftSmaller ? phiLeft : phiRight;
      double phiMaxMeas   = leftSmaller ? phiRight : phiLeft;
      double orgPhiMin    = phiMinMeas;
      double orgPhiMax    = phiMaxMeas;

      if(  phiOffset > 1.5*pi){
        if( orgPhiMin > pi ) orgPhiMin = orgPhiMin-phiOffset;
        if( orgPhiMax > pi ) orgPhiMax = orgPhiMax-phiOffset;
      }else if( phiOffset > 0. ) {
        orgPhiMin = orgPhiMin-phiOffset;
        orgPhiMax = orgPhiMax-phiOffset;
      }
      
      if( phiMinMeas > phiMin ){
        phiMin = phiMinMeas;
      }
      if( phiMaxMeas < phiMax ){
        phiMax = phiMaxMeas;
      }
      
    }
    if( phiMin < -998 || phiMax > 998 ) {
      ATH_MSG_WARNING(" Could not determine minimum and maximum phi " );
      return false; 
    }

    double diffPhi = phiMax-phiMin;
    double avePhi  = phiMin + 0.5*diffPhi;

    ATH_MSG_VERBOSE("Phi ranges: min " << phiMin << "  max " << phiMax << " average phi " << avePhi );


    if( diffPhi < 0 ) {
      if( fabs(diffPhi) > m_openingAngleCut ) {
        ATH_MSG_VERBOSE(" Inconsistent min/max, rejecting track " );
        return false;
      }
    }

    if( !fitterData.phiHits.empty() ) {
      hit = fitterData.phiHits.begin();
      hit_end = fitterData.phiHits.end();
      for( ;hit!=hit_end; ++hit ) {
        
        double minPhi = phiMin;
        double maxPhi = phiMax;
        if( phiMin > phiMax  ){
          maxPhi = phiMin;
          minPhi = phiMax;
        }
        const Trk::MeasurementBase& meas = **hit;
        double phiMeas = meas.globalPosition().phi();
        if( phiOffset > 1.5*pi ) {
          if( phiMeas < 0. ) phiMeas = phiOffset + phiMeas;
        }else if( phiOffset > 0. ){
          phiMeas = phiMeas + phiOffset;
        }
        double diffMin = phiMeas - minPhi;
        double diffMax = phiMeas - maxPhi;
        if( diffMin < 0. || diffMax > 0. ) {
          if( diffMin < -m_openingAngleCut || diffMax > m_openingAngleCut ){
            ATH_MSG_VERBOSE(" Phi hits inconsistent with min/max, rejecting track: phi meas " 
                                   << meas.globalPosition().phi() );
            return false;
          }
        }

      }
    }

    if(  phiOffset > 1.5*pi){
      if( phiMax > pi ) phiMax = phiMax-phiOffset;
      if( phiMin > pi ) phiMin = phiMin-phiOffset;
      if( avePhi > pi ) avePhi = avePhi-phiOffset;
    }else if( phiOffset > 0. ) {
      phiMax = phiMax-phiOffset;
      phiMin = phiMin-phiOffset;
      avePhi = avePhi-phiOffset;
    }
    
    fitterData.avePhi = avePhi;
    fitterData.phiMin = phiMin;
    fitterData.phiMax = phiMax;
    
    return true;
  }

  
  const MuonSegment* MooTrackFitter::segmentFromEntry( const MuPatCandidateBase& entry ) const {

    // if track entry use first segment
    const MuPatTrack* trkEntry = dynamic_cast<const MuPatTrack*>(&entry);
    if( trkEntry ){
      const MuonSegment* seg = m_trackToSegmentTool->convert(trkEntry->track());
      m_measurementsToBeDeleted.push_back(seg);
      return seg;
    }

    // if segment entry use segment directly
    const MuPatSegment* segEntry = dynamic_cast<const MuPatSegment*>(&entry);
    if( segEntry ){
      return segEntry->segment;
    }
    
    return 0;
  }

  double MooTrackFitter::qOverPFromEntry( const MuPatCandidateBase& entry ) const {
    return entry.entryPars().charge()/entry.entryPars().momentum().mag();
  }

  double MooTrackFitter::qOverPFromEntries( const MuPatCandidateBase& firstEntry, const MuPatCandidateBase& secondEntry ) const {

    if( m_slFit ) return 0;

    double qOverP = 0;
    // if available take momentum from entries
    if( firstEntry.hasMomentum() )        return qOverPFromEntry(firstEntry);
    else if( secondEntry.hasMomentum() )  return qOverPFromEntry(secondEntry);
 
    // no momentum yet, estimate from segments
    const MuonSegment* segFirst = segmentFromEntry(firstEntry);
    if( !segFirst ){
      ATH_MSG_WARNING(" failed to get segment for first entry, this should not happen " );
      return qOverP;
    }

    const MuonSegment* segSecond = segmentFromEntry(secondEntry);
    if( !segSecond ){
      ATH_MSG_WARNING(" failed to get segment for second entry, this should not happen " );
      return qOverP;
    }

    std::vector<const MuonSegment*> segments;
    segments.push_back(segFirst);
    segments.push_back(segSecond);
    
    double momentum = 1.;
    m_momentumEstimator->fitMomentumVectorSegments(segments,momentum);
    //momentum = restrictedMomentum(momentum);
    
    if( momentum == 0. ) return 0.;

    qOverP = 1./momentum;
    return qOverP;
  }
  
  double MooTrackFitter::phiSeeding(  MooTrackFitter::FitterData& fitterData ) const {


    if( m_cosmics ){
      // check whether the first entry is a track, if not consider the second entry for phi seeding 
      if( !dynamic_cast<const MuPatTrack*>(fitterData.firstEntry) ) {
        // if second entry is a track, use its phi value
        if( dynamic_cast<const MuPatTrack*>(fitterData.secondEntry) ) {
          ATH_MSG_DEBUG("Using phi of second entry " << fitterData.secondEntry->entryPars().momentum().phi() );
          return fitterData.secondEntry->entryPars().momentum().phi();
        }
      }


      // add fake phi hit on first and last measurement
      const MuPatSegment* segInfo1 = dynamic_cast<const MuPatSegment*>(fitterData.firstEntry);
      const MuPatSegment* segInfo2 = dynamic_cast<const MuPatSegment*>(fitterData.secondEntry);
      if( segInfo1 && segInfo2 && fitterData.stations.size() == 1 && fitterData.numberOfSLOverlaps()==1 ){

        // only perform SL overlap fit for MDT segments
        Identifier chId = m_helperTool->chamberId(*segInfo1->segment);
        if( m_idHelperTool->isMdt(chId) ) {
        
          IMuonSegmentInOverlapResolvingTool::SegmentMatchResult result = m_overlapResolver->matchResult(*segInfo1->segment,*segInfo2->segment);
          
          if( !result.goodMatch() ) {
            ATH_MSG_VERBOSE(" Match failed " );
            return false;
          }
          double overlapPhi = result.phiResult.segmentDirection1.phi();
          ATH_MSG_DEBUG(" using overlap phi " << overlapPhi );
          return overlapPhi;
        }
      }   
      
      // calculate the difference between the segment positions to estimate the direction of the muon
      Amg::Vector3D difPos = fitterData.firstEntry->entryPars().position() - fitterData.secondEntry->entryPars().position();
      // check whether the new direction is pointing down
      if( difPos.y() > 0 ) difPos *= -1.;

      // calculate difference in angle between phi from segment and the phi of the two new segment positions
      double norm = difPos.perp()*fitterData.firstEntry->entryPars().momentum().perp();
      double cosdphi = (fitterData.firstEntry->entryPars().momentum().x()*difPos.x() + fitterData.firstEntry->entryPars().momentum().y()*difPos.y())/norm;
      double dphi = 0.; 
      if ( cosdphi >= +1.0 )      dphi = 0.0; 
      else if ( cosdphi <= -1.0 ) dphi = M_PI; 
      else                        dphi = std::acos(cosdphi); 
      if( fabs(dphi) > 0.2 ) {
        ATH_MSG_DEBUG("Large diff between phi of segment direction and of position "
                             << fitterData.firstEntry->entryPars().momentum().phi() 
                             << "  from pos " << difPos.phi() << " dphi " << dphi );
        return difPos.phi();
      }

      ATH_MSG_DEBUG("Using phi of first entry "
                           << fitterData.firstEntry->entryPars().momentum().phi() << "  phi from position " << dphi );


      return fitterData.firstEntry->entryPars().momentum().phi();
    }

    double phi(0.);
    // update parameters if seeding purely with positions
    if( m_seedPhiWithEtaHits ){
      // use eta of vector connecting first/last hit
      MooTrackFitter::MeasVec& etaHits = fitterData.etaHits;
      Amg::Vector3D difPos = etaHits.back()->globalPosition() - etaHits.front()->globalPosition();
      if( difPos.mag() > 3000 ){
        ATH_MSG_DEBUG("Seeding phi using average phi of eta hits " );
        phi = difPos.phi();
        return phi;
      }
    }


    // should have at least one phi hit
    MooTrackFitter::MeasVec& phiHits = fitterData.phiHits;

    if( phiHits.empty() ){
      ATH_MSG_DEBUG("No phi hits, using average phi " );
      // use average phi value to seed fit
      return fitterData.avePhi;
    }

    // in this case can't do much...
    if( phiHits.size() == 1 ) {
      return phiHits.front()->globalPosition().phi();
    }

    // in endcap, if first segment in EI use its direction
    const MuPatSegment* segInfo1 = dynamic_cast<const MuPatSegment*>(fitterData.firstEntry);
    if( segInfo1 && segInfo1->containsStation(MuonStationIndex::EI) && !segInfo1->phiHits().empty() ){
      return segInfo1->segment->globalDirection().phi();
    }

    // in endcap, if first segment in EM use its direction
    if( segInfo1 && segInfo1->containsStation(MuonStationIndex::EM) && !segInfo1->phiHits().empty() ){
      return segInfo1->segment->globalDirection().phi();
    }

    if( m_seedWithAvePhi ){
      // use average phi value to seed fit
      MeasCit hit = phiHits.begin();
      MeasCit hit_end = phiHits.end();
      Amg::Vector3D avePos(0.,0.,0.);
      for( ;hit!=hit_end; ++hit ) avePos += (*hit)->globalPosition();//phi += (*hit)->globalPosition().phi();
      //phi /= phiHits.size();
      avePos /= phiHits.size();
      phi = avePos.phi();
    }else{
      // use phi of vector connecting first/last hit
      Amg::Vector3D difPos = phiHits.back()->globalPosition() - phiHits.front()->globalPosition();
      phi = difPos.phi();
    }
    
    return phi;
  }

  double MooTrackFitter::thetaSeeding( const MuPatCandidateBase& entry, MooTrackFitter::MeasVec& etaHits ) const {
    // should have at least one eta hit
    if( etaHits.empty() ){
      ATH_MSG_WARNING(" cannot calculate eta seed from empty vector " );
      return 0;
    }

    double theta(0.);
    if( m_seedWithSegmentTheta ){
      theta = entry.entryPars().momentum().theta();
    }else{

      // in this case can't do much...
      if( etaHits.size() == 1 ) {
        theta = etaHits.front()->globalPosition().theta();
      }else{

        // use eta of vector connecting first/last hit
        Amg::Vector3D difPos = etaHits.back()->globalPosition() - etaHits.front()->globalPosition();
        theta = difPos.theta();
      }
    }
    
    return theta;

  }

  Trk::Perigee* MooTrackFitter::createStartParameters( MooTrackFitter::FitterData& fitterData ) const {

    // get momentum + charge from entry if available, else use MuonSegmentMomentum to estimate the momentum
    double qOverP = qOverPFromEntries(*fitterData.firstEntry,*fitterData.secondEntry);
    Trk::Perigee* perigee = 0,*startPars=0;
    const MuPatCandidateBase *entry1=fitterData.firstEntry,*entry2=fitterData.secondEntry;
     
    const MuPatTrack* trkEntry1 = dynamic_cast<const MuPatTrack*>(entry1);
    const MuPatTrack* trkEntry2 = dynamic_cast<const MuPatTrack*>(entry2);
    const MuPatSegment *seg1=dynamic_cast<const MuPatSegment*>(entry1);
    const MuPatSegment *seg2=dynamic_cast<const MuPatSegment*>(entry2);
    
    Amg::Vector3D dir1,dir2;
    Amg::Vector3D point1, point2;
    if (seg1) {
      dir1=seg1->segment->globalDirection(); 
      point1=seg1->segment->globalPosition();
    }
    else {
      if(trkEntry1->track().perigeeParameters()) {
	dir1=trkEntry1->track().perigeeParameters()->momentum().unit();
	point1=trkEntry1->track().perigeeParameters()->position();
      }else{
	return perigee;
      }
    }
    if (seg2) {
      dir2=seg2->segment->globalDirection();
      point2=seg2->segment->globalPosition();
    }
    else {
      if(trkEntry2->track().perigeeParameters()) {
	dir2=trkEntry2->track().perigeeParameters()->momentum().unit();
	point2=trkEntry2->track().perigeeParameters()->position();
      }else{
	return perigee;
      }
    }
    if (dir1.dot(point2-point1)<0){
      Amg::Vector3D tmp=point1;
      point1=point2;
      point2=tmp;
      Amg::Vector3D tmp2=dir1;
      dir1=dir2;
      dir2=tmp2;
      entry1=fitterData.secondEntry;
      entry2=fitterData.firstEntry;
      trkEntry1 = dynamic_cast<const MuPatTrack*>(entry1);
      trkEntry2 = dynamic_cast<const MuPatTrack*>(entry2);
      seg1=dynamic_cast<const MuPatSegment*>(entry1);
      seg2=dynamic_cast<const MuPatSegment*>(entry2);
    }
   
    const MuPatCandidateBase *bestentry=entry1;
    double dist1=-1,dist2=-1;
    const MuPatHit *firstphi1=0,*lastphi1=0,*firstphi2=0,*lastphi2=0;

    
    std::list<MuPatHit*>::const_iterator it=entry1->hitList().begin();
    for (;it!=entry1->hitList().end();it++){
      if ((**it).info().type!=MuPatHit::Pseudo && (**it).info().measuresPhi){
        if (!firstphi1) firstphi1=*it;
        lastphi1=*it;
      }   
    }
    it=entry2->hitList().begin();
    for (;it!=entry2->hitList().end();it++){
      if ((**it).info().type!=MuPatHit::Pseudo && (**it).info().measuresPhi){
        if (!firstphi2) firstphi2=*it;
        lastphi2=*it;
      }   
    }
     
    if (firstphi1) dist1=fabs((firstphi1->measurement().globalPosition()-lastphi1->measurement().globalPosition()).dot(dir1));
    if (firstphi2) dist2=fabs((firstphi2->measurement().globalPosition()-lastphi2->measurement().globalPosition()).dot(dir2));
    if (dist2>dist1) {
      bestentry=entry2;
    }
    const MuPatTrack* besttrkEntry = dynamic_cast<const MuPatTrack*>(bestentry);
    const MuPatSegment *bestseg=dynamic_cast<const MuPatSegment*>(bestentry);
    if (besttrkEntry){
      const Trk::TrackParameters *mdtpar=0;
      for (DataVector<const Trk::TrackParameters>::const_iterator parit=besttrkEntry->track().trackParameters()->begin();parit!=besttrkEntry->track().trackParameters()->end();parit++){
        mdtpar=*parit;
        if (mdtpar) break;
      }
      if( !mdtpar ) {
	ATH_MSG_WARNING("Failed to find valid Trackparameters on track " );
	return 0;
      }
      Amg::VectorX newpar=mdtpar->parameters(); //besttrkEntry->track().perigeeParameters()->parameters();
      newpar[Trk::qOverP]=qOverP;
      Trk::PerigeeSurface persurf(mdtpar->position());
      startPars=new Trk::Perigee(0,0,newpar[Trk::phi],newpar[Trk::theta],qOverP,persurf); //besttrkEntry->track().perigeeParameters()->cloneToNew(newpar);
    }
    
    else {
      
      
      Trk::PerigeeSurface persurf(bestseg->segment->globalPosition());
      double phi=bestseg->segment->globalDirection().phi();    
      if ((fitterData.firstEntry->containsChamber(MuonStationIndex::CSS) || fitterData.firstEntry->containsChamber(MuonStationIndex::CSL)) && fitterData.secondEntry->hasSLOverlap()) 
      phi = (fitterData.hitList.back()->parameters().position()-bestseg->segment->globalPosition()).phi();

      double theta=bestseg->segment->globalDirection().theta();    
      // create start parameter
      startPars = new Trk::Perigee(0,0,phi,theta,qOverP,persurf);
      //std::cout << "bestseg: " << *bestseg << " startpars: " << *startPars << std::endl; 

    }

    // create start parameter
    perigee = startPars;
    
    if( !perigee ){
      ATH_MSG_DEBUG(" failed to create perigee ");
      return 0;
    }

    m_parametersToBeDeleted.push_back(perigee);
    fitterData.startPars = perigee;

    return perigee;
  }

  Trk::Perigee* MooTrackFitter::createPerigee( const Trk::TrackParameters& firstPars, const Trk::MeasurementBase& firstMeas ) const {

    //const Amg::Vector3D& firstPos = firstMeas.globalPosition();

    Amg::Vector3D perpos(0.,0.,0.);
    // propagate segment parameters to first measurement
    const Trk::TrackParameters* exPars = &firstPars;
    const Trk::TrackParameters* garbage = 0;
    double shift = 1.;
    if( m_seedAtStartOfTrack ){

      exPars = m_propagator->propagate(firstPars,firstMeas.associatedSurface(),Trk::anyDirection,false,m_magFieldProperties);
      if( !exPars ) {
	ATH_MSG_DEBUG(" Propagation failed in createPerigee!! " );
	return 0;
      }
      garbage = exPars;
      shift = 100.; 
    }
      
    // small shift towards the ip
    double sign = exPars->position().dot(exPars->momentum()) > 0 ? 1. : -1.;
    perpos =  exPars->position() - shift*sign*exPars->momentum().unit();
 
    // create perigee
    double phi=exPars->momentum().phi();
    double theta=exPars->momentum().theta();
    double qoverp=exPars->charge()/exPars->momentum().mag();
    if( m_slFit ) qoverp = 0;
    else{
      if( !validMomentum( *exPars ) ) {
        delete garbage;
        return 0;
      }
    }
   

    Trk::PerigeeSurface persurf(perpos);
    Trk::Perigee* perigee = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);
    
    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << MSG::DEBUG << std::setprecision(5) << " creating perigee: phi " << phi << " theta " << theta 
             << " q*mom " << perigee->charge()*perigee->momentum().mag() << " r " << perigee->position().perp()
             << " z " << perigee->position().z() << " input q*mom " << firstPars.charge()*firstPars.momentum().mag() << endmsg;
    }
    delete garbage;
    

    return perigee;
  }

  double MooTrackFitter::restrictedMomentum( double momentum ) const {


    // restrict range of momenta to 2 GeV / 10 GeV
    if( momentum > 0 ) {
      if( momentum > 20000. ) momentum = 20000.;
      else if( momentum < 2000 ) momentum = 2000;
    }else{
      if( momentum < -20000. ) momentum = -20000.;
      else if( momentum > -2000. ) momentum = -2000.;
    }
    

    return momentum;
  }



  Trk::Track* MooTrackFitter::fit( const Trk::Perigee& startPars, MooTrackFitter::MeasVec& hits, 
                                    Trk::ParticleHypothesis partHypo, bool prefit ) const {

    if( hits.empty() ) return 0;
    

    if( msgLvl(MSG::VERBOSE) ) { 
      msg(MSG::VERBOSE) << std::setprecision(5) << " track start parameter: phi " << startPars.momentum().phi() 
             << " theta " << startPars.momentum().theta() 
             << " q*mom " << startPars.charge()*startPars.momentum().mag() << " r " << startPars.position().perp()
             << " z " << startPars.position().z() << std::endl
             << " start par is a perigee " << " partHypo " << partHypo;
      msg(MSG::VERBOSE) << std::endl << m_printer->print(hits) << endmsg;
    }


    const Trk::TrackParameters* pars = &startPars;
    if( m_seedAtStartOfTrack ){
      DistanceAlongParameters distAlongPars;
      double dist = distAlongPars(startPars,*hits.front());
      
      if( dist < 0. ){
	ATH_MSG_DEBUG(" start parameters after first hit, shifting them.... " );
	Trk::Perigee* perigee = createPerigee(startPars,*hits.front());
	if( perigee ){
	  m_parametersToBeDeleted.push_back(perigee);
	  pars = perigee;
	}else{
	  ATH_MSG_DEBUG(" failed to move start pars, failing fit " );
	  return 0;
	}
      }
    }
                                
    // fit track
    if ( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << MSG::DEBUG << "fit: " << (prefit ? "prefit" : "fit") << "track with hits: " << hits.size();
      if ( msgLvl(MSG::VERBOSE) ) {
        msg(MSG::DEBUG) << std::endl << m_printer->print(hits);
      }
      msg(MSG::DEBUG) << endmsg;
    }
    Trk::Track* track =  prefit ? 
      m_trackFitterPrefit->fit(hits,*pars,m_runOutlier,partHypo) :
      m_trackFitter->fit(hits,*pars,m_runOutlier,partHypo);     

    // 'sign' track
    if( track ) track->info().setPatternRecognitionInfo(m_patRecInfo);
    return track;
  }
    
  Trk::Track* MooTrackFitter::fitWithRefit( const Trk::Perigee& startPars, MooTrackFitter::MeasVec& hits ) const {
    
    Trk::Track* track = fit(startPars,hits);

    // exceptions that are not refitted
    if( m_slFit ) return track;

    // refit track to ensure correct handling of material effects
    if( track ) {
      const Trk::Perigee* pp = track->perigeeParameters();
      if( pp ){
        ATH_MSG_VERBOSE(" found track: " <<  m_printer->print(*track));
        
        // refit if absolute difference of the initial momentum and the final momentum is larger than 5 GeV
        double difMom = startPars.momentum().mag()-pp->momentum().mag();
        if( fabs(difMom) > 5000. ) {

          ATH_MSG_DEBUG(" absolute difference in momentum too large, refitting track. Dif momentum= " << difMom );
          if ( msgLvl(MSG::DEBUG) ) {
            msg(MSG::DEBUG) << MSG::DEBUG << "fitWithRefit: refitting track with hits: " << hits.size();
            if ( msgLvl(MSG::VERBOSE) ) {
              msg(MSG::DEBUG) << std::endl << m_printer->print(hits);
            }
            msg(MSG::DEBUG) << endmsg;
          }
          Trk::Track* refittedTrack = m_trackFitter->fit(hits,*pp,false,m_ParticleHypothesis);
          if( refittedTrack ){
            delete track;
            track = refittedTrack;
                    
            if( msgLvl(MSG::DEBUG) ) { 
              const Trk::Perigee* pp = track->perigeeParameters();
              if( pp ){
                ATH_MSG_DEBUG(" refitted track fit perigee: r " << pp->position().perp()
                                     << " z " << pp->position().z() );  
              }
            }   
          }
        }
      }
    }
    
    cleanUp();

    return track;
  } 

  void MooTrackFitter::cleanUp() const {
    std::for_each( m_measurementsToBeDeleted.begin(),m_measurementsToBeDeleted.end(),MuonDeleteObject<const Trk::MeasurementBase>());
    m_measurementsToBeDeleted.clear();

    std::for_each( m_parametersToBeDeleted.begin(),m_parametersToBeDeleted.end(),MuonDeleteObject<const Trk::TrackParameters>());
    m_parametersToBeDeleted.clear();

    std::for_each( m_mctbHitsToBeDeleted.begin(),m_mctbHitsToBeDeleted.end(),MuonDeleteObject<const MuPatHit>());
    m_mctbHitsToBeDeleted.clear();

  }


  bool MooTrackFitter::cleanPhiHits( double momentum, MooTrackFitter::FitterData& fitterData, 
                                      const std::vector<const Trk::PrepRawData*>* patternPhiHits ) const {

    ATH_MSG_VERBOSE(" cleaning phi hits " );

    MeasVec& phiHits = fitterData.phiHits;

    // copy phi ROTs into vector, split up competing ROTs
    std::vector<const Trk::RIO_OnTrack*> rots;
    std::vector<const Trk::RIO_OnTrack*> rotsNSW;  // hack as the phi hit cleaning does not work for NSW hits
    std::set<Identifier> ids;
    std::set<MuonStationIndex::StIndex> stations;
    rots.reserve(phiHits.size()+5);
    
    MeasVec::const_iterator hit = phiHits.begin();
    MeasVec::const_iterator hit_end = phiHits.end();
    for( ;hit!=hit_end;++hit ){
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*hit);
      if( rot ){
	if(  m_idHelperTool->issTgc( rot->identify() )  ){
	  rotsNSW.push_back(rot->clone());
	  continue;
	}
        rots.push_back(rot);
        ids.insert( rot->identify() );
        continue;
      }
      const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*hit);
      if( crot ){
        const std::vector<const MuonClusterOnTrack*>& mrots = crot->containedROTs();
        std::vector<const MuonClusterOnTrack*>::const_iterator mit = mrots.begin();
        std::vector<const MuonClusterOnTrack*>::const_iterator mit_end = mrots.end();
        for( ;mit!=mit_end; ++mit ) {
          rots.push_back( *mit );
          ids.insert( (*mit)->identify() );
          MuonStationIndex::StIndex stIndex =  m_idHelperTool->stationIndex( (*mit)->identify() );
          stations.insert(stIndex);
        }
      }else{
        ATH_MSG_WARNING(" phi hits should be ROTs or competing ROTs! Dropping hit " );
      }
    }
    
    if( rots.empty() ) return false;

    if( rots.size() > m_phiHitsMax ){
      ATH_MSG_DEBUG(" too many phi hits, not running cleaning " << rots.size() );
      return true;
    }

    if( msgLvl(MSG::VERBOSE) ) {
      msg(MSG::VERBOSE) << " contained phi hits ";
      std::vector<const Trk::RIO_OnTrack*>::iterator rit = rots.begin();
      std::vector<const Trk::RIO_OnTrack*>::iterator rit_end = rots.end();
      for( ;rit!=rit_end;++rit ){
        msg(MSG::DEBUG) << std::endl << "   " << m_printer->print( **rit );
      }
      msg(MSG::DEBUG) << endmsg;
    }

    // if available, extract additional phi hits from the road that were not on the entries
    std::vector<const Trk::PrepRawData*> roadPhiHits;
    if( patternPhiHits ){
      std::vector<const Trk::PrepRawData*>::const_iterator pit = patternPhiHits->begin();
      std::vector<const Trk::PrepRawData*>::const_iterator pit_end = patternPhiHits->end();
      for( ;pit!=pit_end; ++pit ){
        if( ids.count( (*pit)->identify() ) ) continue;
        
        // do not add CSCs as they really should be on a segment
        if( m_idHelperTool->isCsc( (*pit)->identify() ) || m_idHelperTool->issTgc( (*pit)->identify() ) ) continue;

        roadPhiHits.push_back( *pit );
      }
    }

    if( roadPhiHits.size() + rots.size() > m_phiHitsMax ){
      ATH_MSG_DEBUG(" too many pattern phi hits, not adding any " << roadPhiHits.size() );
      roadPhiHits.clear(); // cleaning road hits as we do not want to add them but we do want to clean
    }
    if( msgLvl(MSG::VERBOSE) ) {
      if( !roadPhiHits.empty() ) {
        msg(MSG::VERBOSE) << " additional pattern phi hits " << std::endl;
        std::vector<const Trk::PrepRawData*>::const_iterator pit = roadPhiHits.begin();
        std::vector<const Trk::PrepRawData*>::const_iterator pit_end = roadPhiHits.end();
        for( ;pit!=pit_end; ++pit ) {
          msg(MSG::DEBUG) << "   " << m_printer->print( **pit );
          if( pit+1 != pit_end ) msg(MSG::DEBUG) << std::endl;
          else                   msg(MSG::DEBUG) << endmsg;
        }
      }
    }

    
    std::vector<const Trk::MeasurementBase*>* newMeasurements = m_phiHitSelector->select_rio( momentum, rots, roadPhiHits );
    if( !newMeasurements ){
      ATH_MSG_WARNING(" no measurements returned by phi hit selector " );
      return false;
    }
    newMeasurements->insert(newMeasurements->end(),rotsNSW.begin(),rotsNSW.end());

    if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " selected phi hits " << newMeasurements->size() << std::endl;

    if( newMeasurements->empty() ) {
      ATH_MSG_DEBUG(" empty list of phi hits return from phi hit selector: input size " 
                           << phiHits.size() );
      delete newMeasurements;
      return false;
    }

    // require the new hits to be within a certain distance of the hits already on the track candidate
    DistanceAlongParameters distAlongPars;
    double maxDistCut = 800.;
    std::vector<const Trk::MeasurementBase*> measurementsToBeAdded;

    std::vector<const Trk::MeasurementBase*>::iterator mit = newMeasurements->begin();
    std::vector<const Trk::MeasurementBase*>::iterator mit_end = newMeasurements->end();
    for( ;mit!=mit_end;++mit ){

      m_measurementsToBeDeleted.push_back(*mit);

      const Identifier& id = m_helperTool->getIdentifier(**mit);
      
      if( !id.is_valid() ) {
        ATH_MSG_WARNING(" Phi measurement without valid Identifier! " );        
        continue;
      }
      if( msgLvl(MSG::VERBOSE) ) {
        msg(MSG::VERBOSE) << "  " << m_printer->print( **mit );
      }
      // only add hits if the hitList is not empty so we can calculate the distance of the new hit to the first and last hit on the track
      if( fitterData.hitList.empty() ) {
        if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << std::endl << "          discarding phi hit due to empty hitList " << std::endl;   
        continue;
      }
      
      // calculate the distance of the phi hit to the first hit. If the phi hit lies before the first hit, 
      // remove the hit if the distance is too large
      double dist = distAlongPars( fitterData.hitList.front()->parameters(), **mit );
      if( dist < -maxDistCut ) {
        if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << std::endl << "          discarded phi hit, distance to first hit too large " << dist << std::endl;
        continue;
      }
        
      // calculate the distance of the phi hit to the last hit. If the phi hit lies after the last hit, 
      // remove the hit if the distance is too large
      double distBack = distAlongPars( fitterData.hitList.back()->parameters(), **mit );
      if( distBack > maxDistCut ){
        if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << std::endl << "          discarded phi hit, distance to last hit too large " << distBack << std::endl;
        continue;
      }
      
      if( msgLvl(MSG::VERBOSE) )   msg(MSG::VERBOSE) << std::endl << "          new phi hit, distance from start pars " << dist << " distance to last pars " << distBack << std::endl;
      
      measurementsToBeAdded.push_back(*mit);
    }
     
    // now remove all previous phi hits and replace them with the new ones

    hit = phiHits.begin();
    for( ;hit!=hit_end;++hit ){
      if( !m_hitHandler->remove(**hit,fitterData.hitList) ) ATH_MSG_WARNING(" failed to remove measurement " 
                                                                   << m_printer->print(**hit) );
    }

    // add the new phi hits to the hit list
    if( !measurementsToBeAdded.empty() ){
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " adding measurements " << std::endl;
      MuPatHitList newHitList;
      m_hitHandler->create(fitterData.firstEntry->entryPars(),measurementsToBeAdded,newHitList);
      m_hitHandler->merge(newHitList,fitterData.hitList);
    }

    ATH_MSG_VERBOSE(" done cleaning " );

    delete newMeasurements;
    
    return true;
  }


  Trk::Track* MooTrackFitter::cleanAndEvaluateTrack( Trk::Track& track, const std::set<Identifier>& excludedChambers ) const {
    // preselection to get ride of really bad tracks
    if( !m_helperTool->goodTrack( track, m_preCleanChi2Cut ) ){
      ATH_MSG_DEBUG(" Track rejected due to large chi2" << std::endl
                           << m_printer->print(track) );
      return 0;
    }

    // perform cleaning of track
    Trk::Track* cleanTrack = 0;
    if( excludedChambers.empty() ) cleanTrack = m_cleaner->clean(track);
    else{
      ATH_MSG_DEBUG(" Cleaning with exclusion list " << excludedChambers.size() );

      cleanTrack = m_cleaner->clean(track,excludedChambers);
    }
    if( !cleanTrack ){
      ATH_MSG_DEBUG(" Cleaner returned a zero pointer, reject track " );
      return 0;
    }

    // selection to get ride of bad tracks after cleaning
    if( !m_helperTool->goodTrack( *cleanTrack, m_chi2Cut ) ){
      ATH_MSG_DEBUG(" Track rejected after cleaning " << std::endl
                           << m_printer->print(*cleanTrack) );
      if( cleanTrack != &track ) delete cleanTrack;
      return 0;
    }

    ATH_MSG_DEBUG(" Track passed selection after cleaning! " );
    return cleanTrack;
  }


  bool MooTrackFitter::validMomentum( const Trk::TrackParameters& pars ) const {
    double p = pars.momentum().mag();
    if( p < m_pThreshold ){
      ATH_MSG_DEBUG(" momentum below threshold: momentum  " << pars.momentum().mag()
                           << "  p " << pars.momentum().perp()  );
      return false;
    }
    return true;
  }

  void MooTrackFitter::cleanEntry( const MuPatCandidateBase& entry, std::set<Identifier>& removedIdentifiers  ) const {
    // if segment entry use segment directly
    const MuPatSegment* segEntry = dynamic_cast<const MuPatSegment*>(&entry);
    if( segEntry && segEntry->segment ){
      if( entry.hasSmallChamber() && entry.hasLargeChamber() ) {
        ATH_MSG_DEBUG(" Segment with SL overlap, cannot perform cleaning " );
      }else{
        cleanSegment( *segEntry->segment, removedIdentifiers );
      }
    }
  }


  void MooTrackFitter::cleanSegment( const MuonSegment& seg, std::set<Identifier>& removedIdentifiers  ) const {

    TrkDriftCircleMath::DCOnTrackVec dcs;
    /* ********  Mdt hits  ******** */

    const MuonGM::MdtReadoutElement* detEl = 0;

    Amg::Transform3D gToStation;

    // set to get Identifiers of chambers with hits
    std::vector< std::pair<Identifier,bool> > indexIdMap;
    indexIdMap.reserve( seg.containedMeasurements().size() );

    unsigned index = 0;
    float tubeRadius=14.6;
    std::vector<const Trk::MeasurementBase*>::const_iterator it = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator it_end = seg.containedMeasurements().end();
    ATH_MSG_DEBUG("loop through hits for segment");
    for(  ;it!=it_end;++it){

      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*it);

      if( !mdt ){
        continue;
      }
      Identifier id = mdt->identify();
      if( !detEl ) {
	detEl = mdt->prepRawData()->detectorElement();
	gToStation = detEl->GlobalToAmdbLRSTransform();
      }
      if( !detEl ){
        ATH_MSG_WARNING(" error aborting not detEl found ");
        break;
      }

      ATH_MSG_DEBUG("detector element of station type "<<detEl->getStationType());
      tubeRadius=detEl->innerTubeRadius();

      // calculate local AMDB position
      Amg::Vector3D locPos = gToStation*mdt->prepRawData()->globalPosition();
      TrkDriftCircleMath::LocPos lpos(locPos.y(),locPos.z());

      double r  = mdt->localParameters()[Trk::locR];
      double dr = Amg::error(mdt->localCovariance(),Trk::locR);

      // create identifier
      TrkDriftCircleMath::MdtId mdtid( m_idHelperTool->mdtIdHelper().isBarrel(id),m_idHelperTool->mdtIdHelper().multilayer(id)-1, 
                                       m_idHelperTool->mdtIdHelper().tubeLayer(id)-1, m_idHelperTool->mdtIdHelper().tube(id)-1 );

      // create new DriftCircle
      TrkDriftCircleMath::DriftCircle dc( lpos, r, dr, TrkDriftCircleMath::DriftCircle::InTime, mdtid, index, mdt );
      TrkDriftCircleMath::DCOnTrack dcOnTrack(dc,1.,1.);
      ATH_MSG_VERBOSE(" new MDT hit " << m_idHelperTool->toString( id ) );
      
      dcs.push_back( dcOnTrack );
      indexIdMap.push_back( std::make_pair(id,false) );

      ++index;
    }    

    double angleYZ = seg.localDirection().angleYZ();
    const Amg::Vector3D lpos = gToStation*seg.globalPosition();

    ATH_MSG_DEBUG(" cleaning segment " << m_printer->print(seg) << std::endl
                         << " local parameters " << lpos.y() << " " << lpos.z() 
                         << "  phi " <<  angleYZ << "  with " << dcs.size() << " hits  " );

    TrkDriftCircleMath::LocPos segPos(lpos.y(),lpos.z());
    TrkDriftCircleMath::Line segPars(segPos,angleYZ);
    
    m_fitter.fit(segPars,dcs);
    TrkDriftCircleMath::Segment segment = m_fitter.result();
    segment.hitsOnTrack(dcs.size());
    ATH_MSG_DEBUG(" segment after fit " << segment.chi2() << " ndof " << segment.ndof() << " local parameters "
                         << segment.line().x0() << " " << segment.line().y0() << "  phi " << segment.line().phi() );

    bool success = m_finder.dropHits(segment);
    if( !success ) {
      ATH_MSG_DEBUG(" drop hits failed " );
      return;
    }

    if( dcs.size() ==  segment.ndof() + 2 ){
      ATH_MSG_DEBUG(" segment unchanges " );
    }else if( dcs.size() !=  segment.ndof() + 3 ){
      ATH_MSG_DEBUG(" more than one hit removed, keeping old segment " );
    }else{
      ATH_MSG_DEBUG(" removed single hit, not using it in fit " );

      TrkDriftCircleMath::MatchDCWithLine matchDC(segment.line(),5.,TrkDriftCircleMath::MatchDCWithLine::Pull,tubeRadius);
      const TrkDriftCircleMath::DCOnTrackVec& matchedDCs = matchDC.match(segment.dcs());

      for( TrkDriftCircleMath::DCOnTrackCit dcit = matchedDCs.begin();dcit!=matchedDCs.end();++dcit ){
        
        if( dcit->state() == TrkDriftCircleMath::DCOnTrack::OnTrack ) continue;
	if( (unsigned int) dcit->index() >= indexIdMap.size() ) continue;
        indexIdMap[dcit->index()].second = true;
      }
      
      std::vector< std::pair<Identifier,bool> >::iterator iit = indexIdMap.begin();
      std::vector< std::pair<Identifier,bool> >::iterator iit_end = indexIdMap.end();
      for( ;iit!=iit_end;++iit ){
        if(iit->second){
          ATH_MSG_DEBUG(" removing hit " << m_idHelperTool->toString( iit->first ) );
          removedIdentifiers.insert( iit->first );
        }
      }
    }
  }

  void MooTrackFitter::removeSegmentOutliers( FitterData& fitterData ) const {

    if( fitterData.startPars->momentum().mag() < 4000. ) return;
       
    std::set<Identifier> removedIdentifiers;
    if( fitterData.firstEntry )  cleanEntry( *fitterData.firstEntry, removedIdentifiers );
    if( fitterData.secondEntry ) cleanEntry( *fitterData.secondEntry, removedIdentifiers );
    
    ATH_MSG_DEBUG(" removing hits  " << removedIdentifiers.size() );

    std::set<Identifier>::iterator iit = removedIdentifiers.begin();
    std::set<Identifier>::iterator iit_end = removedIdentifiers.end();
    for( ;iit!=iit_end;++iit ){
      if( msgLvl(MSG::VERBOSE) ) {
        msg(MSG::VERBOSE) << m_idHelperTool->toString( *iit ) << endmsg;
      }
      m_hitHandler->remove(*iit,fitterData.hitList);
    }
  }

  std::pair<Trk::Track*,Trk::Track*> MooTrackFitter::splitTrack( const Trk::Track& track ) const {

    // access TSOS of track
    const DataVector<const Trk::TrackStateOnSurface>* oldTSOT = track.trackStateOnSurfaces();
    if( !oldTSOT ) return std::make_pair<Trk::Track*,Trk::Track*>(0,0);


    // check whether the perigee is expressed at the point of closes approach or at muon entry
    const Trk::Perigee* perigee = track.perigeeParameters();
    if( perigee ) {
      bool atIP = fabs(perigee->position().dot(perigee->momentum().unit())) < 10 ? true : false;
      if( atIP ){
        ATH_MSG_DEBUG(" track extressed at perigee, cannot split it " );
        return std::make_pair<Trk::Track*,Trk::Track*>(0,0);
      }
    }

    // loop over content input track and count perigees
    unsigned int nperigees(0);
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tit = oldTSOT->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tit_end = oldTSOT->end();
    for( ;tit!=tit_end;++tit ){

      // look whether state is a perigee
      if( (*tit)->type(Trk::TrackStateOnSurface::Perigee)  && dynamic_cast<const Trk::Perigee*>((*tit)->trackParameters()) ) ++nperigees;
    }
    if( nperigees != 2 ) {
      ATH_MSG_DEBUG(" Number of perigees is not one, cannot split it " << nperigees );      
      return std::make_pair<Trk::Track*,Trk::Track*>(0,0);
    }

    struct TrackContent {
      TrackContent() : firstParameters(0),tsos(0),track(0) {}
      const Trk::TrackParameters* firstParameters;
      std::vector<const Trk::TrackStateOnSurface*> tsos;
      Trk::Track* track;
      std::set<MuonStationIndex::StIndex> stations;
    };

    // store content of split tracks
    TrackContent firstTrack;
    firstTrack.tsos.reserve(oldTSOT->size());

    TrackContent secondTrack;
    secondTrack.tsos.reserve(oldTSOT->size());

    // keep track of the current set that is being filled
    TrackContent* currentTrack = &firstTrack;

    // count perigees on track
    nperigees = 0;

    // loop over content input track and extract the content of the two split tracks
    tit = oldTSOT->begin();
    tit_end = oldTSOT->end();
    for( ;tit!=tit_end;++tit ){

      const Trk::TrackParameters* pars = (*tit)->trackParameters();
          
      // look whether state is a perigee
      if( (*tit)->type(Trk::TrackStateOnSurface::Perigee) && dynamic_cast<const Trk::Perigee*>(pars) ){
        ++nperigees;
        
        if( msgLvl(MSG::DEBUG) && nperigees == 1 ) msg(MSG::DEBUG) << MSG::DEBUG << " found first perigee on track " << endmsg;

        // if this is the second perigee, switch to second part of the track
        if( nperigees == 2 ){
          ATH_MSG_DEBUG(" found second perigee, switch to second track" );
          currentTrack = &secondTrack;

        }else if( nperigees > 2 ){
          ATH_MSG_WARNING(" Unexpected number of perigees: " << nperigees );
        }
      }
      
      // we should drop all states inbetween the two perigees 
      if( nperigees == 1 ){
        ATH_MSG_VERBOSE(" state between the two perigees " );
      }
      
      // check whether the current TrackContent has firstParameters, if not set them
      if( !currentTrack->firstParameters ){
        ATH_MSG_VERBOSE(" found first parameters " << m_printer->print(*pars) );
        currentTrack->firstParameters = pars;
      }

      // check whether there are track parameters on the current state
      if( !pars ){
        ATH_MSG_VERBOSE(" adding state without parameters " );
        currentTrack->tsos.push_back(*tit);
        continue;
      }

      // check whether state is a measurement, if not continue
      const Trk::MeasurementBase* meas = (*tit)->measurementOnTrack();
      if( !meas ) {
        ATH_MSG_VERBOSE(" adding state without measurement " << m_printer->print(*pars) );
        currentTrack->tsos.push_back(*tit);
        continue;
      }

      // get identifier, if it has no identifier or is not a muon hit continue
      Identifier id = m_helperTool->getIdentifier(*meas);
      if( !id.is_valid() || !m_idHelperTool->mdtIdHelper().is_muon(id) ) {
        ATH_MSG_VERBOSE(" adding state with measurement without valid identifier " << m_printer->print(*pars) );
        currentTrack->tsos.push_back(*tit);
        continue;
      }

      if( nperigees == 1 ) ATH_MSG_WARNING(" found muon measurement inbetween the two perigees, this should not happen " );

      // add station index for MDT and CSC hits
      if( !m_idHelperTool->isTrigger(id) ) currentTrack->stations.insert(m_idHelperTool->stationIndex(id));

      ATH_MSG_VERBOSE(" adding  " << m_printer->print(*meas) );

      currentTrack->tsos.push_back(*tit);

    }

    if( msgLvl(MSG::DEBUG) ) {

      if( firstTrack.firstParameters )
        msg(MSG::DEBUG) << MSG::DEBUG << " first track content: states " << firstTrack.tsos.size() << " stations " << firstTrack.stations.size() << endmsg
               << " first pars " << m_printer->print(*firstTrack.firstParameters) << endmsg;

      if( secondTrack.firstParameters )
        msg(MSG::DEBUG) << MSG::DEBUG << " second track content: states " << secondTrack.tsos.size() << " stations " << secondTrack.stations.size() << endmsg
               << " first pars " << m_printer->print(*secondTrack.firstParameters) << endmsg;
    }


    // check whether both tracks have start parameters and sufficient stations
    if( (firstTrack.firstParameters && firstTrack.stations.size() > 1) &&
        (secondTrack.firstParameters && secondTrack.stations.size() > 1) ) {

      ATH_MSG_DEBUG(" track candidate can be split, trying to fit split tracks " );
      // fit the two tracks
      firstTrack.track = fitSplitTrack( *firstTrack.firstParameters, firstTrack.tsos );
      if( firstTrack.track ){

        ATH_MSG_DEBUG(" fitted first track, trying second " );


        secondTrack.track = fitSplitTrack( *secondTrack.firstParameters, secondTrack.tsos );

        if( secondTrack.track ){
          ATH_MSG_DEBUG(" fitted second track " );
          
        }else{
          
          ATH_MSG_DEBUG(" failed to fit second track " );

          // delete first track
          delete firstTrack.track;
          firstTrack.track = 0;
        }
      }else{
        ATH_MSG_DEBUG(" failed to fit first track " );
      }

    }

    cleanUp();

    return std::make_pair(firstTrack.track,secondTrack.track);
  }

  Trk::Track* MooTrackFitter::fitSplitTrack( const Trk::TrackParameters& startPars, const std::vector<const Trk::TrackStateOnSurface*>& tsos ) const {
    
    // first create track out of the constituent
    double phi=startPars.momentum().phi();
    double theta=startPars.momentum().theta();
    double qoverp=startPars.charge()/startPars.momentum().mag();
    if( m_slFit ) qoverp = 0;
    Trk::PerigeeSurface persurf(startPars.position());
    Trk::Perigee* perigee = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);
    
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve(tsos.size()+1);
    trackStateOnSurfaces->push_back( MuonTSOSHelper::createPerigeeTSOS(perigee) );

    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tit = tsos.begin();
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tit_end = tsos.end();
    for( ;tit!=tit_end;++tit )  trackStateOnSurfaces->push_back( (*tit)->clone() );

    Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown,Trk::muon);
    Trk::Track* track = new Trk::Track( trackInfo, trackStateOnSurfaces, 0);

    unsigned int nphi = hasPhiConstrain(*track);

    if( nphi > 1 ){
      ATH_MSG_DEBUG("Track has sufficient phi constraints, fitting " );

    }else{

      // loop over the track and find the first and last measurement and the phi hit if any
      const Trk::TrackStateOnSurface* firstMeas = 0;
      const Trk::TrackStateOnSurface* lastMeas = 0;
      const Trk::TrackStateOnSurface* phiMeas = 0;

      std::vector<const Trk::TrackStateOnSurface*>::const_iterator tit = tsos.begin();
      std::vector<const Trk::TrackStateOnSurface*>::const_iterator tit_end = tsos.end();
      for( ;tit!=tit_end;++tit ){

        // require track parameters;
        if( !(*tit)->trackParameters() ) continue;
        
        const Trk::MeasurementBase* meas = (*tit)->measurementOnTrack();
        if( !meas ) continue;
        
        Identifier id = m_helperTool->getIdentifier(*meas);
        if( !id.is_valid() || !m_idHelperTool->mdtIdHelper().is_muon(id) ) continue;

        if( m_idHelperTool->isMdt(id) ){
          if( !firstMeas ) firstMeas = *tit;
          lastMeas = *tit;
        }else if( m_idHelperTool->measuresPhi(id) ) {
          phiMeas = *tit;
        }
      }
      
      if( !firstMeas ){
        ATH_MSG_WARNING(" failed to find first MDT measurement with track parameters" );
        return 0;
      }

      if( !lastMeas ){
        ATH_MSG_WARNING(" failed to find second MDT measurement with track parameters" );
        return 0;
      }

      if( firstMeas == lastMeas ){
        ATH_MSG_WARNING(" first MDT measurement with track parameters equals to second" );
        return 0;
      }
      
      const Trk::TrackStateOnSurface* positionFirstFake = 0;
      const Trk::TrackStateOnSurface* positionSecondFake = 0;

      if( phiMeas ) {
        double distFirst = (firstMeas->trackParameters()->position()-phiMeas->trackParameters()->position()).mag();
        double distSecond = (lastMeas->trackParameters()->position()-phiMeas->trackParameters()->position()).mag();
        ATH_MSG_DEBUG("Track has one phi constraints, adding second: dist to first " << distFirst
                             << " dist to second " << distSecond );
        // add fake at position furthest away from phi measurement
        if( distFirst < distSecond ){
          positionFirstFake = lastMeas;
        }else{
          positionFirstFake = firstMeas;
        }

      }else{
        ATH_MSG_DEBUG("Track has no phi constraints, adding one at beginning and one at the end of the track " );
        
        positionFirstFake = firstMeas;
        positionSecondFake = lastMeas;
      }

      // clean up previous track and create new one with fake hits
      delete track;
      perigee = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);
      trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
      trackStateOnSurfaces->reserve(tsos.size()+3);
      trackStateOnSurfaces->push_back( MuonTSOSHelper::createPerigeeTSOS(perigee) );
      
      tit = tsos.begin();
      tit_end = tsos.end();
      for( ;tit!=tit_end;++tit ){

        // remove existing pseudo measurements
        const Trk::MeasurementBase* meas = (*tit)->measurementOnTrack();
        if( meas && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) ) {
          ATH_MSG_DEBUG("removing existing pseudo measurement " );
          continue;
        }
          
        trackStateOnSurfaces->push_back( (*tit)->clone() );
        if( *tit == positionFirstFake ){
          double fakeError =100.;
          if( positionFirstFake->trackParameters()->covariance() ) {
            fakeError = Amg::error( *positionFirstFake->trackParameters()->covariance(),Trk::loc2);
            ATH_MSG_DEBUG(" Using error of parameters " << fakeError );
          }

          const Trk::MeasurementBase* fake = createFakePhiForMeasurement( *(positionFirstFake->measurementOnTrack()), 
                                                                          &(positionFirstFake->trackParameters()->position()),
                                                                          0, fakeError );
          if( fake ) {
            // need to clone as fake is already added to garbage collection
            trackStateOnSurfaces->push_back( MuonTSOSHelper::createMeasTSOSWithUpdate( **tit, 
                                                                                       fake->clone(),positionFirstFake->trackParameters()->clone(),
                                                                                       Trk::TrackStateOnSurface::Measurement) );
          }else{
            ATH_MSG_WARNING(" failed to create fake at first measurement " );
          }
        }
        if( *tit == positionSecondFake && positionSecondFake ){
	  double fakeError =100.;
          if( positionSecondFake->trackParameters()->covariance() ) {
            fakeError = Amg::error( *positionSecondFake->trackParameters()->covariance(),Trk::loc2);
            ATH_MSG_DEBUG(" Using error of parameters " << fakeError );
          }
          const Trk::MeasurementBase* fake = createFakePhiForMeasurement( *(positionSecondFake->measurementOnTrack()), 
                                                                          &(positionSecondFake->trackParameters()->position()),
                                                                          0, fakeError );
          if( fake ) {
            // need to clone as fake is already added to garbage collection
            trackStateOnSurfaces->push_back( MuonTSOSHelper::createMeasTSOSWithUpdate( **tit,
                                                                                       fake->clone(),
                                                                                       positionSecondFake->trackParameters()->clone(),
                                                                                       Trk::TrackStateOnSurface::Measurement) );
          }else{
            ATH_MSG_WARNING(" failed to create fake at second measurement " );
          }
        }
      }
      
      Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown,Trk::muon);
      track = new Trk::Track( trackInfo, trackStateOnSurfaces, 0);
    }
    
    // refit track
    Trk::Track* refittedTrack = refit(*track);
    delete track;
    if( refittedTrack ) refittedTrack->info().setPatternRecognitionInfo(m_patRecInfo);

    return refittedTrack;
    
  }

  void MooTrackFitter::copyHitList( const MuPatHitList& hitList, MuPatHitList& copy ) const {
    MuPatHitCit it = hitList.begin();
    MuPatHitCit it_end = hitList.end();
    for( ;it!=it_end;++it ){
      const Trk::TrackParameters* pars = (*it)->parameters().clone();
      MuPatHit* hit = new MuPatHit( pars, &(*it)->preciseMeasurement(), (*it)->broadMeasurement().clone(), (*it)->info());
      copy.insert(copy.end(),hit);
      m_parametersToBeDeleted.push_back(pars);
      m_mctbHitsToBeDeleted.push_back(hit);
    }
  }
   
}
