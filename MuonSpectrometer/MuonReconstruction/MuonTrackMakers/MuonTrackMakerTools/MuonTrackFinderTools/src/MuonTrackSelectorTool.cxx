/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackSelectorTool.h"

#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include <map>

namespace Muon {

  MuonTrackSelectorTool::MuonTrackSelectorTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"), 
      m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_trackSummaryTool("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"),
      m_ntotalTracks(0),
      m_failedChi2NDofCut(0),
      m_failedSingleStationCut(0),
      m_failedRPCAveMinTimeCut(0),
      m_failedRPCAveMaxTimeCut(0),
      m_failedRPCSpreadTimeCut(0),
      m_failedTwoStationsCut(0),
      m_failedTwoStationsMaxMDTHoleCut(0),
      m_failedTwoStationsMaxHoleCut(0),
      m_failedTwoStationsGoodStationCut(0),
      m_failedTriggerStationCut(0),
      m_failedMaxMDTHoleCut(0),
      m_failedMaxHoleCut(0)
  {
    declareInterface<ITrackSelectorTool>(this);
    declareProperty("Chi2NDofCut",                        m_chi2NDofCut = 20. );
    declareProperty("RemoveSingleStationTracks",          m_removeSingleStationTracks = false );
    declareProperty("MinimumNumberOfMdtHitsPerStation",   m_minMdtHitsPerStation = 3 );
    declareProperty("MinimumNumberOfCscHitsPerStation",   m_minCscHitsPerStation = 3 );
    declareProperty("HolesToHitsRatioCutPerStation",      m_holeHitRatioCutPerStation = 1.1 );
    declareProperty("MaxMdtHolesOnTwoStationTrack",       m_maxMdtHolesPerTwoStationTrack = 5 );
    declareProperty("MaxMdtHolesOnTrack",                 m_maxMdtHolesPerTrack = 5 );
    declareProperty("UseRPCHoles",                        m_useRPCHoles = true );
    declareProperty("UseTGCHoles",                        m_useTGCHoles = true );
    declareProperty("UseCSCHoles",                        m_useCSCHoles = true );
    declareProperty("UseMDTHoles",                        m_useMDTHoles = true );
    declareProperty("IgnoreTriggerHolesInChamberWithHits", m_ignoreTriggerHolesInLayersWithHits = true );
    declareProperty("ApplyRPCTimeWindow",                 m_useRPCTimeWindow = false );
    declareProperty("RemoveTwoStationTrackWithoutTriggerHits",m_removeTwoStationTrackWithoutTriggerHits = false );
    declareProperty("CountMDTOutlierAsHoles",             m_countMdtOutliersAsHoles = false );
    declareProperty("TightSingleStationCuts",             m_tightSingleStationCuts = false );
  }


  MuonTrackSelectorTool::~MuonTrackSelectorTool(){}


  StatusCode MuonTrackSelectorTool::initialize()
  {
    if ( AthAlgTool::initialize().isFailure() ) {
      return StatusCode::FAILURE;
    }

    if (m_helperTool.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_helperTool ); 
      return  StatusCode::FAILURE;
    }
   
    if(m_printer.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_printer ); 
      return  StatusCode::FAILURE;
    }
    
    if (m_idHelperTool.retrieve().isFailure()){
      ATH_MSG_FATAL("Could not get " << m_idHelperTool ); 
      return  StatusCode::FAILURE;
    }    

    if ( m_trackSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Unable to retrieve" << m_trackSummaryTool);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
  StatusCode MuonTrackSelectorTool::finalize()
  {
    // print counters
    ATH_MSG_INFO(" Number of tracks handled by selector " <<  m_ntotalTracks );
    if( m_ntotalTracks != 0 ){
      double failedChi2NDofCutFraction = (double)m_failedChi2NDofCut/(double)m_ntotalTracks;
      double failedRPCAveMinTimeCutFraction = (double)m_failedRPCAveMinTimeCut/(double)m_ntotalTracks;
      double failedRPCAveMaxTimeCutFraction = (double)m_failedRPCAveMaxTimeCut/(double)m_ntotalTracks;
      double failedRPCSpreadTimeCutFraction = (double)m_failedRPCSpreadTimeCut/(double)m_ntotalTracks;
      double failedSingleStationsCutFraction = (double)m_failedSingleStationCut/(double)m_ntotalTracks;
      double failedTwoStationsCutFraction = (double)m_failedTwoStationsCut/(double)m_ntotalTracks;
      double failedTwoStationsMaxMDTHoleCutFraction = (double)m_failedTwoStationsMaxMDTHoleCut/(double)m_ntotalTracks;
      double failedTwoStationsMaxHoleCutFraction = (double)m_failedTwoStationsMaxHoleCut/(double)m_ntotalTracks;
      double failedTwoStationsGoodStationCutFraction = (double)m_failedTwoStationsGoodStationCut/(double)m_ntotalTracks;
      double failedTriggerStationCutFraction = (double)m_failedTriggerStationCut/(double)m_ntotalTracks;
      double failedMaxMDTHoleCutFraction = (double)m_failedMaxMDTHoleCut/(double)m_ntotalTracks;
      double failedMaxHoleCutFraction = (double)m_failedMaxHoleCut/(double)m_ntotalTracks;
      ATH_MSG_INFO(" Fractions failing selection cuts: " << endmsg
		   << std::setw(30) << " Chi2/Ndof Cut                 " << failedChi2NDofCutFraction << endmsg
		   << std::setw(30) << " RPC AveMin Time Cut           " << failedRPCAveMinTimeCutFraction << endmsg
		   << std::setw(30) << " RPC AveMax Time Cut           " << failedRPCAveMaxTimeCutFraction << endmsg
		   << std::setw(30) << " RPC Spread Time Cut           " << failedRPCSpreadTimeCutFraction << endmsg
		   << std::setw(30) << " Single station Cut            " << failedSingleStationsCutFraction << endmsg
		   << std::setw(30) << " Two station Cut               " << failedTwoStationsCutFraction << endmsg
		   << std::setw(30) << " Two station Max MDT hole Cut  " << failedTwoStationsMaxMDTHoleCutFraction << endmsg
		   << std::setw(30) << " Two station Max hole Cut      " << failedTwoStationsMaxHoleCutFraction << endmsg
		   << std::setw(30) << " Two station good station Cut  " << failedTwoStationsGoodStationCutFraction << endmsg
		   << std::setw(30) << " Trigger station cut           " << failedTriggerStationCutFraction << endmsg
		   << std::setw(30) << " MDT hole Cut                  " << failedMaxMDTHoleCutFraction << endmsg
		   << std::setw(30) << " Max hole Cut                  " << failedMaxHoleCutFraction );
    }

    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }

  bool MuonTrackSelectorTool::decision( const Trk::Track& track, const Trk::Vertex* ) const {
    
    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_DEBUG(" track without states, discarding track " );
      return false;
    }

    ++m_ntotalTracks;

    ATH_MSG_VERBOSE(" new track " << m_printer->print(track) << std::endl
		    << m_printer->printStations(track) );

    // get reduced chi2
    const Trk::FitQuality* fq = track.fitQuality();
    if( !fq ) return false;  
    double reducedChi2 = fq->numberDoF() != 0. ? fq->chiSquared()/fq->numberDoF() : 1.;
    if( reducedChi2 > m_chi2NDofCut ){
      ++m_failedChi2NDofCut;
      ATH_MSG_DEBUG(" Track discarded: too large chi2 " << reducedChi2 );
      return false;
    }

    if( m_useRPCTimeWindow ){
      int nrpcs = 0;
      double aveRpcTime = 0.;
      double minTime = 1e9;
      double maxTime = -1e9;
      
      // loop over TSOSs
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
      for( ; tsit!=tsit_end ; ++tsit ){
	
	if( !(*tsit)->type(Trk::TrackStateOnSurface::Measurement) ) continue;
	
	// check wther state is a measurement
	const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
	if( !meas ) continue;
	
	const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(meas);
	if( !rpc ){
	  const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
	  if( crot ){
	    rpc = dynamic_cast<const RpcClusterOnTrack*>(crot->containedROTs().front());
	  }
	}
	if( rpc ) {
	  double time = rpc->prepRawData()->time()-rpc->globalPosition().mag()/300.;
	  ++nrpcs;
	  aveRpcTime += time;
	  if( time < minTime ) minTime = time;
	  if( time > maxTime ) maxTime = time;
	}
      }
     
      if( nrpcs != 0 ){
	aveRpcTime /= nrpcs;
	double timeMinCut = -2.;
	double timeMaxCut = 20.;
	ATH_MSG_VERBOSE(" ave RPC time " << aveRpcTime << "  min " << minTime << " max " << maxTime );
	if( aveRpcTime < timeMinCut ){
	  ATH_MSG_VERBOSE(" rejecting due too small average RPC time " );
	  ++m_failedRPCAveMinTimeCut;
	  return false;
	}
	if( aveRpcTime > timeMaxCut ){
	  ATH_MSG_VERBOSE(" rejecting due too large average RPC time " );
	  ++m_failedRPCAveMaxTimeCut;
	  return false;
	}
	if( maxTime > timeMaxCut && minTime < timeMinCut ){
	  ATH_MSG_VERBOSE(" rejecting due too larger RPC time spread " );
	  ++m_failedRPCSpreadTimeCut;
	  return false;
	}
      }
    }

    unsigned int mdtHoles = 0;
    unsigned int mdtOutliers = 0;
    unsigned int nholes = 0;
    unsigned int nmeasPhi = 0;
    std::map<MuonStationIndex::StIndex,StationData> stations;

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

      const Identifier& chId = chit->chamberId();


      bool isMdt = m_idHelperTool->isMdt(chId);
      bool isCsc = m_idHelperTool->isCsc(chId);
      bool isRpc = m_idHelperTool->isRpc(chId);
      bool isTgc = m_idHelperTool->isTgc(chId);
      bool issTgc = m_idHelperTool->issTgc(chId);
      bool isMM   = m_idHelperTool->isMM(chId);

      // check whether we should use holes in this chamber
      bool useHoles = false;
      if( (isMdt && m_useMDTHoles) ||
	  (isTgc && m_useTGCHoles) ||
	  (isRpc && m_useRPCHoles) ||
	  (isCsc && m_useCSCHoles)
	  ) useHoles = true;


      if( !isMdt ) nmeasPhi += chit->phiProjection().nhits;


      if( isMdt ){

	MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(chId);
	StationData& stData = stations[stIndex];
	stData.netaHits  += chit->nhits();
	if( useHoles ) stData.netaHoles += chit->nholes(); 
	if( !stData.mdtHasHitsinMl1 && chit->mdtMl1().nhits > 0 ) stData.mdtHasHitsinMl1 = true;
	if( !stData.mdtHasHitsinMl2 && chit->mdtMl2().nhits > 0 ) stData.mdtHasHitsinMl2 = true;
	
	if( useHoles ) {
	  mdtHoles += chit->nholes();
	  nholes += chit->nholes();
	  mdtOutliers += chit->noutliers();
	}

	stData.isMdt = true;

      }else if ( isCsc || issTgc || isMM ){

	MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(chId);
	StationData& stData = stations[stIndex];
	stData.netaHits  += chit->etaProjection().nhits;
	if( useHoles ) {
	  stData.netaHoles += chit->etaProjection().nholes; 
	  stData.nphiHoles += chit->phiProjection().nholes; 
	  nholes += chit->nholes();
	}
	stData.nphiHits  += chit->phiProjection().nhits;
	stData.isCsc = isCsc;
	stData.isNSW = issTgc || isMM;

      }else if( isRpc || isTgc ){

	MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(chId);
	StationData& stData = stations[stIndex];
	stData.netaTrigHits  += chit->etaProjection().nhits > 0 ? 1 : 0;
	stData.nphiTrigHits  += chit->phiProjection().nhits > 0 ? 1 : 0;
	if( useHoles ) {
	  // add trigger holes
	  if( !m_ignoreTriggerHolesInLayersWithHits || chit->etaProjection().nhits == 0 ) {
	    stData.netaTrigHoles += chit->etaProjection().nholes; 
	    nholes += chit->etaProjection().nholes; 
	  }
	  if( !m_ignoreTriggerHolesInLayersWithHits || chit->phiProjection().nhits == 0 ) {
	    stData.nphiTrigHoles += chit->phiProjection().nholes; 
	    nholes += chit->phiProjection().nholes; 
	  }
	}

	// make sure this station is not already present as MDT or CSC station
	if( !stData.isMdt && !stData.isCsc ) stData.isTrigger = true;

      }
    }

    unsigned int nGoodStations = 0;
    unsigned int nstations = 0;
    unsigned int nTwoMlStations = 0;
    unsigned int nGoodCscStations = 0;
    unsigned int nGoodTriggerStations = 0;

    std::map<MuonStationIndex::StIndex,StationData>::iterator sit = stations.begin();
    std::map<MuonStationIndex::StIndex,StationData>::iterator sit_end = stations.end();
    for( ;sit!=sit_end;++sit ){
      StationData& stData = sit->second;
      
      if( stData.nphiTrigHits != 0 && stData.netaTrigHits != 0 ){
	++nGoodTriggerStations;
      }
      
      if( stData.isMdt ){
	if( stData.netaHits < m_minMdtHitsPerStation ) {
 	  ATH_MSG_VERBOSE(" Not counting MDT station too few MDT hits: nhits " << stData.netaHits
			  << " cut " << m_minMdtHitsPerStation );
	  continue;
	}	
	++nstations;
	double holeHitRatio = (double)stData.netaHoles/stData.netaHits;
	if( holeHitRatio > m_holeHitRatioCutPerStation ) {
 	  ATH_MSG_VERBOSE(" Not counting MDT station too many holes: nhits " << stData.netaHits
			  << "  nholes " << stData.netaHoles << "  ratio " << holeHitRatio 
			  << " cut " << m_holeHitRatioCutPerStation );
	  continue;
	}
	if( stData.mdtHasHitsinBothMl() ) ++nTwoMlStations;
      }else if( stData.isCsc || stData.isNSW ){
	if( stData.isCsc && stData.nphiHits == 0 ){
	  ATH_MSG_VERBOSE(" Not counting CSC station no phi hits: netaHits " << stData.netaHits
			  << "  nphiHits " << stData.nphiHits );
	  continue;
	}

	if( stData.netaHits < m_minCscHitsPerStation ){
	  ATH_MSG_VERBOSE(" Not counting CSC station too few hits: netaHits " << stData.netaHits
			  << "  nphiHits " << stData.nphiHits  << " cut " << m_minCscHitsPerStation );
	  continue;
	}
	++nstations;
	++nGoodCscStations;
      }
      
      
      ++nGoodStations;
    }

    
    ATH_MSG_DEBUG(" good stations " << nGoodStations 
		  << " MDT stations with two ml " << nTwoMlStations
		  << " MDT holes " << mdtHoles << " outliers " << mdtOutliers
		  << " good CSC stations " << nGoodCscStations 
		  << " trigger stations " << nGoodTriggerStations );

    if( m_removeSingleStationTracks && stations.size() < 2 ){
      ATH_MSG_DEBUG(" Track discarded: too few stations " << stations.size() );
      ++m_failedSingleStationCut;
      return false;
    }

    // special treatment of single station tracks 
    if( stations.size() == 1  ){
      
      if( !stations.count(MuonStationIndex::EM) ) return false;


      StationData& stData =  stations[MuonStationIndex::EM];

      unsigned int nExpectedTriggerStations = m_tightSingleStationCuts ? 3 : 2;
      unsigned int maxHolesSingle           = m_tightSingleStationCuts ? 0 : 1;

      ATH_MSG_DEBUG(" Single station track:  trigger phi " << stData.nphiTrigHits << " eta " << stData.netaTrigHits 
		    << " cut " << nExpectedTriggerStations << " holes " << stData.netaHoles << " cut " << maxHolesSingle  );

      bool ok = true;
      // require two multi layers in MDT
      if( nTwoMlStations == 0 ) ok = false;
      
      // require all three trigger layers
      if( stData.nphiTrigHits < nExpectedTriggerStations || stData.netaTrigHits < nExpectedTriggerStations ) ok = false;

      // no holes
      if( stData.netaHoles > maxHolesSingle ) ok = false;

      if( !ok ) {
	ATH_MSG_DEBUG(" Track discarded: failed single track cuts " );
	++m_failedSingleStationCut;
      }
      return ok;
    }
    
    if( nGoodStations < 2 ){
      ATH_MSG_DEBUG(" Track discarded: too few good stations " << nGoodStations );
      ++m_failedTwoStationsCut;
      return false;
    }
    
    if( m_countMdtOutliersAsHoles ) mdtHoles += mdtOutliers;

    if( nstations < 3 ) {
      
      if( mdtHoles > m_maxMdtHolesPerTwoStationTrack ) {
	ATH_MSG_DEBUG(" Track discarded: good stations " << nGoodStations 
		      << " and " << mdtHoles << " mdt holes "  );
	++m_failedTwoStationsMaxMDTHoleCut;
	return false;
      }
      if( nholes > m_maxMdtHolesPerTwoStationTrack ) {
	ATH_MSG_DEBUG(" Track discarded: good stations " << nGoodStations 
		      << " and " << nholes << " holes "  );
	++m_failedTwoStationsMaxHoleCut;
	return false;
      }
      if( nTwoMlStations == 0 && nGoodCscStations == 0 ) {
	ATH_MSG_DEBUG(" Track discarded: good stations " << nGoodStations 
		      << " but no MDT station with hits in two multilayers nor good CSC station " );
	++m_failedTwoStationsGoodStationCut;
	return false;
      }
      if( m_removeTwoStationTrackWithoutTriggerHits && nGoodTriggerStations == 0 ) {
	ATH_MSG_DEBUG(" Track discarded: stations " << nstations 
		      << " but no trigger hits nor good CSC station " );
	++m_failedTriggerStationCut;
	return false;
      }
    }else{
      if( mdtHoles > m_maxMdtHolesPerTrack ) {
	ATH_MSG_DEBUG(" Track discarded: good stations " << nGoodStations 
		      << " and " << mdtHoles << " mdt holes "  );
	++m_failedMaxMDTHoleCut;
	return false;
      }
      if( nholes > m_maxMdtHolesPerTrack ) {
	ATH_MSG_DEBUG(" Track discarded: good stations " << nGoodStations 
		      << " and " << nholes << " holes "  );
	++m_failedMaxHoleCut;
	return false;
      }

    }
    

    ATH_MSG_DEBUG(" Track passed selection " );
  

    return true;
  }


}
