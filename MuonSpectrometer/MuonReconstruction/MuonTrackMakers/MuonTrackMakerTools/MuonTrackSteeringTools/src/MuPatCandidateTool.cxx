/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuPatCandidateTool.h"
#include "MuPatHitTool.h"
#include "MuPatTrack.h"
#include "MuPatCandidateBase.h"
#include "MuPatSegment.h"
#include "SortMuPatHits.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"

#include <sstream>
#include <string>
#include <set>
#include <cstdlib>  // for std::abs( int )

#define BETTER_BOOKKEEPING


namespace Muon {

  MuPatCandidateTool::MuPatCandidateTool(const std::string& t, const std::string& n, const IInterface* p)    
    : AthAlgTool(t,n,p)
  {
    declareInterface<MuPatCandidateTool>(this);
  }
        
  StatusCode MuPatCandidateTool::initialize(){
    ATH_CHECK( m_idHelperSvc.retrieve() );
    ATH_CHECK( m_mdtRotCreator.retrieve() );
    if (!m_cscRotCreator.empty()) {
      if (!m_idHelperSvc->hasCSC()) ATH_MSG_WARNING("The current layout does not have any CSC chamber but you gave a CscRotCreator, ignoring it, but double-check configuration");
      else ATH_CHECK( m_cscRotCreator.retrieve() );
    }
    ATH_CHECK( m_compClusterCreator.retrieve() );
    ATH_CHECK( m_hitHandler.retrieve() );
    ATH_CHECK( m_edmHelperSvc.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_segmentSelector.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode MuPatCandidateTool::finalize() {
    return StatusCode::SUCCESS;
  }

  MuPatSegment* MuPatCandidateTool::createSegInfo( const MuonSegment& segment ) const
  {
    Identifier chid = m_edmHelperSvc->chamberId(segment);
    if( m_idHelperSvc->isTrigger(chid) ){
      ATH_MSG_WARNING("Trigger hit only segments not supported " << m_idHelperSvc->toStringChamber(chid) );
      return 0;
    }
    MuPatSegment* info = new MuPatSegment();
    info->segment = &segment;
    info->chid = chid;
    info->chIndex = m_idHelperSvc->chamberIndex(info->chid);
    std::set<Identifier> chIds = m_edmHelperSvc->chamberIds(segment);
    std::set<Identifier>::iterator chit = chIds.begin();
    std::set<Identifier>::iterator chit_end = chIds.end();
    for( ;chit!=chit_end;++chit ){
      MuonStationIndex::ChIndex chIdx = m_idHelperSvc->chamberIndex(*chit);
      info->addChamber(chIdx);
      // set default name
      if ( !info->name.empty() ) info->name += "+";
      // stationname_eta_phi
      std::ostringstream oss;
      oss << MuonStationIndex::chName(chIdx)
          << "_" << m_idHelperSvc->stationEta(*chit) << "_" << m_idHelperSvc->stationPhi(*chit);
      info->name += oss.str();
    }
    info->stIndex = m_idHelperSvc->stationIndex(info->chid);
    info->isEndcap = m_idHelperSvc->isEndcap(info->chid);
    info->isMdt = m_idHelperSvc->isMdt(info->chid);
    info->usedInFit = 0;
    info->quality = m_segmentSelector->quality( segment );
    info->segQuality = dynamic_cast<const MuonSegmentQuality*>(segment.fitQuality());

    info->segPars = m_edmHelperSvc->createTrackParameters( segment, 5000., 0. );    
    if( !info->segPars ) {
      ATH_MSG_WARNING(" failed to create track parameter for segment " );
    }

    updateHits(*info,info->segment->containedMeasurements(),m_doMdtRecreation,m_doCscRecreation, true );
    MuPatHitList& hitList = info->hitList();
    m_hitHandler->create( segment, hitList );

    return info;
  }


  bool MuPatCandidateTool::extendWithSegment( MuPatTrack& can, MuPatSegment& segInfo, const Trk::Track* track ) const {
    // add segment to candidate
    can.addSegment(&segInfo,track);
    return recalculateCandidateSegmentContent( can );
  }
  
  MuPatTrack* MuPatCandidateTool::copyCandidate( MuPatTrack& canIn ) const {
    // copy and update hits
    MuPatTrack* can = new MuPatTrack(canIn);
#ifdef BETTER_BOOKKEEPING
    // nothing needed here
#else
    updateHits(*can,can->track().measurementsOnTrack()->stdcont());
#endif
    return can;
  }

  MuPatTrack* MuPatCandidateTool::copyCandidateAndTransferTrack( MuPatTrack& canIn ) const {
    MuPatTrack* can = copyCandidate( canIn ); // does not copy track (lazy copy)
    // transfer ownership of track (to avoid copy of track if possible)
    if ( canIn.ownsTrack() ) {
      // canIn owns it, can transfer ownership
      can->updateTrack( &canIn.releaseTrack() );
    } else {
      // someone else owns it. Have to make a copy.
      can->updateTrack( new Trk::Track(canIn.track()) );
    }
    return can;
  }


  MuPatTrack* MuPatCandidateTool::createCandidate( MuPatSegment& segInfo, const Trk::Track* track ) const {

    // create the new candidate
    MuPatTrack* candidate = new MuPatTrack(&segInfo,track);

#ifdef BETTER_BOOKKEEPING
    recalculateCandidateSegmentContent( *candidate );
#else
    m_hitHandler->create( candidate->track(),candidate->hitList() );

    // update the hit summary
    updateHits(*candidate,candidate->track().measurementsOnTrack()->stdcont());
#endif

    return candidate;
  }

  bool MuPatCandidateTool::updateTrack( MuPatTrack& candidate, const Trk::Track* track ) const {
    candidate.updateTrack( track );
    return recalculateCandidateSegmentContent( candidate );
  }

  
  MuPatTrack* MuPatCandidateTool::createCandidate( MuPatSegment& segInfo1, MuPatSegment& segInfo2, 
						       const Trk::Track* track ) const {

    // create the new candidate
    MuPatTrack* candidate = new MuPatTrack(&segInfo1,&segInfo2,track);

#ifdef BETTER_BOOKKEEPING
    recalculateCandidateSegmentContent( *candidate );
#else
    m_hitHandler->create( candidate->track(),candidate->hitList() ); 	  

    // update the hit summary
    updateHits(*candidate,candidate->track().measurementsOnTrack()->stdcont());
#endif

    return candidate;
  }


  MuPatTrack* MuPatCandidateTool::createCandidate( const Trk::Track* track ) const {

    // create a dummy segment vector
    std::vector<MuPatSegment*> segments;

    // create the new candidate
    MuPatTrack* candidate = new MuPatTrack(segments,track);

#ifdef BETTER_BOOKKEEPING
    recalculateCandidateSegmentContent( *candidate );
#else
    m_hitHandler->create( candidate->track(), candidate->hitList() );

    // update the hit summary
    updateHits(*candidate,candidate->track().measurementsOnTrack()->stdcont());
#endif

    return candidate;
  }

  void MuPatCandidateTool::updateHits( MuPatCandidateBase& entry, const MuPatCandidateTool::MeasVec& measurements, 
				     bool recreateMDT, bool recreateCSC, bool createComp ) const {

    std::lock_guard<std::mutex> lock{m_mutex};    
    const EventContext& ctx = Gaudi::Hive::currentContext();
    CacheEntry* ent{m_cache.get(ctx)};
    if (ent->m_evt != ctx.evt()) {
      ent->m_evt = ctx.evt();
      ent->cleanUp();
    }

    MeasVec etaHits;
    MeasVec phiHits;
    MeasVec fakePhiHits;
    MeasVec allHits;

    unsigned int nmdtHitsMl1(0);
    unsigned int nmdtHitsMl2(0);
    unsigned int ncscHitsEta(0);
    unsigned int ncscHitsPhi(0);
    unsigned int nrpcHitsEta(0);
    unsigned int nrpcHitsPhi(0);
    unsigned int ntgcHitsEta(0);
    unsigned int ntgcHitsPhi(0);
    
    entry.clearChambers(); // also clears stations and chamberIds

    bool hasEndcap = false;
    bool hasSmall = false;
    bool hasLarge = false;
    bool hassloverlap=false;
    bool previssmall=false;
    MuonStationIndex::StIndex prevstIndex=MuonStationIndex::StUnknown;

    Trk::MagneticFieldProperties magProps;

    // vector to store trigger hits in case we have special treatment for them
    std::vector<const MuonClusterOnTrack*> triggerHitsPhi;
    std::vector<const MuonClusterOnTrack*> triggerHitsEta;

    // loop over hits
    MeasCit sit = measurements.begin();
    MeasCit sit_end = measurements.end();
    for( ;sit!=sit_end;++sit ){

      const Trk::MeasurementBase* meas = *sit;
      
      Identifier id = m_edmHelperSvc->getIdentifier(*meas);
      if( !id.is_valid() ) {
	fakePhiHits.push_back(meas);
	continue;
      }

      // skip ID hits
      if( !m_idHelperSvc->isMuon(id) ) continue;

      Identifier chId = m_idHelperSvc->chamberId(id);
      MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(chId);
      MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(chId);      
      if( !hasEndcap ) hasEndcap = m_idHelperSvc->isEndcap(id);

      bool measuresPhi = false;
      
      bool isMdt =  m_idHelperSvc->isMdt(id);
      bool isCsc = m_idHelperSvc->isCsc(id); 
      bool isNSW = m_idHelperSvc->isMM(id) || m_idHelperSvc->issTgc(id); 
      // only add precision hits 
      if( isMdt || isCsc || isNSW ) { 
	entry.addChamber(chIndex); // will also add stationIndex 
	entry.chamberIds().insert(chId); 
      }       
      if( isMdt ){

	// if in MDT recreation mode, recreate MDT using ROT creator. Store the pointer as we have to delete it ourselfs.
	if( recreateMDT ){
	  
	  if( recreateMDT && !m_doMdtRecreation ){
	    ATH_MSG_WARNING(" recreation of MDTs requiered although not configured via jobO, ignoring request. Please check jobO " );
	  }else{	
	    const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	    if( !mdt ){
	      ATH_MSG_WARNING(" found MdtDriftCircleOnTrack without MDT identifier " << m_idHelperSvc->toString(id) );
	      continue;
	    }
	    ATH_MSG_DEBUG(" recreating MdtDriftCircleOnTrack " );
	    const MdtDriftCircleOnTrack* newMdt = m_mdtRotCreator->createRIO_OnTrack(*mdt->prepRawData(),mdt->globalPosition());
	    ent->m_measurementsToBeDeleted.push_back(newMdt);
	    meas = newMdt;
	  }
	}
      

	bool isSmall = m_idHelperSvc->isSmallChamber(id);
	if( isSmall ) hasSmall = true;
	else          hasLarge = true;

        if (isSmall!=previssmall && sit != measurements.begin() && stIndex==prevstIndex) hassloverlap=true;
        previssmall=isSmall;
        prevstIndex=stIndex;
 
	unsigned int ml = m_idHelperSvc->mdtIdHelper().multilayer(id);
	if( ml == 1 ) ++nmdtHitsMl1;
	else          ++nmdtHitsMl2;
      }else{

	measuresPhi = m_idHelperSvc->measuresPhi(id);
	
	bool isRpc = m_idHelperSvc->isRpc(id);
	if( isRpc ){
	  if( measuresPhi ) ++nrpcHitsPhi;
	  else              ++nrpcHitsEta;
	}

	bool isTgc = m_idHelperSvc->isTgc(id);
	if( isTgc ){
	  if( measuresPhi ) ++ntgcHitsPhi;
	  else              ++ntgcHitsEta;
	}

	if( isCsc ){
	  
	  if( recreateCSC ) {
	    const CscClusterOnTrack* csc = dynamic_cast<const CscClusterOnTrack*>(meas);
	    if( !csc ){
	      ATH_MSG_WARNING(" found CscClusterOnTrack without CSC identifier " << m_idHelperSvc->toString(id) );
	      continue;
	    }
	    ATH_MSG_DEBUG(" recreating CscClusterOnTrack " );
	    const MuonClusterOnTrack* newCsc = m_cscRotCreator->createRIO_OnTrack(*csc->prepRawData(),csc->globalPosition());
	    ent->m_measurementsToBeDeleted.push_back(newCsc);
	    meas = newCsc;

	  }

	  if( measuresPhi ) ++ncscHitsPhi;
	  else              ++ncscHitsEta;
	}

	// if selected create competing ROTs for trigger hits
	if( createComp && (isRpc||isTgc)  ){
	  if( measuresPhi && m_createCompetingROTsPhi ) {
	    addCluster(*meas,triggerHitsPhi);
	    continue;
	  }else if( !measuresPhi && m_createCompetingROTsEta ){
	    addCluster(*meas,triggerHitsEta);
	    continue;
	  }
	}
      }

      allHits.push_back(meas);

      if( measuresPhi ){
	phiHits.push_back(meas);
      }else{
	etaHits.push_back(meas);
      }
    }
    
    if( createComp ){
      if( m_createCompetingROTsEta && !triggerHitsEta.empty() ) createAndAddCompetingROTs(triggerHitsEta, etaHits, allHits, ent->m_measurementsToBeDeleted);
      if( m_createCompetingROTsPhi && !triggerHitsPhi.empty() ) createAndAddCompetingROTs(triggerHitsPhi, phiHits, allHits, ent->m_measurementsToBeDeleted);
    }

    entry.nmdtHitsMl1 = nmdtHitsMl1;
    entry.nmdtHitsMl2 = nmdtHitsMl2;
    entry.ncscHitsEta = ncscHitsEta;
    entry.ncscHitsPhi = ncscHitsPhi;
    entry.nrpcHitsEta = nrpcHitsEta;
    entry.nrpcHitsPhi = nrpcHitsPhi;
    entry.ntgcHitsEta = ntgcHitsEta;
    entry.ntgcHitsPhi = ntgcHitsPhi;
    if (!triggerHitsEta.empty() && etaHits.size()==0) ATH_MSG_WARNING("did not find any eta hits");
    entry.setEtaHits(etaHits);
    if (!triggerHitsPhi.empty() && phiHits.size()==0) ATH_MSG_WARNING("did not find any phi hits");
    entry.setPhiHits(phiHits);
    entry.setFakePhiHits(fakePhiHits);
    entry.setAllHits(allHits);
    entry.hasEndcap(hasEndcap);
    entry.hasSmallChamber(hasSmall);
    entry.hasLargeChamber(hasLarge);
    entry.hasSLOverlap(hassloverlap);
  }

  void MuPatCandidateTool::addCluster( const Trk::MeasurementBase& meas, std::vector<const MuonClusterOnTrack*>& rots ) const {
    const MuonClusterOnTrack* clus = dynamic_cast<const MuonClusterOnTrack*>(&meas);
    if( clus ) rots.push_back(clus);
    else{
      const CompetingMuonClustersOnTrack* compclus = dynamic_cast<const CompetingMuonClustersOnTrack*>(&meas);
      if( compclus ){
	rots.insert(rots.end(),compclus->containedROTs().begin(),compclus->containedROTs().end());
      }else{
	Identifier id = m_edmHelperSvc->getIdentifier(meas);
	ATH_MSG_WARNING(" Trigger Measurement is not a MuonClusterOnTrack or CompetingMuonClustersOnTrack!!  " 
	       << m_idHelperSvc->toString(id) );	    
      }
    }
  }

  void MuPatCandidateTool::createAndAddCompetingROTs( const std::vector<const MuonClusterOnTrack*>& rots, 
                                                      MuPatCandidateTool::MeasVec& hits,
                                                      MuPatCandidateTool::MeasVec& allHits,
                                                      MuPatCandidateTool::MeasVec& measurementsToBeDeleted ) const {

    typedef std::map<Identifier, std::vector<const MuonClusterOnTrack*> > IdClusMap;
    typedef IdClusMap::iterator IdClusIt;
    IdClusMap idClusters;

    // loop over hits and sort by detector element
    std::vector<const MuonClusterOnTrack*>::const_iterator it = rots.begin();
    std::vector<const MuonClusterOnTrack*>::const_iterator it_end = rots.end();
    for( ;it!=it_end;++it ){
      
      const MuonClusterOnTrack* clus = *it;
      
      Identifier id = clus->identify();
      Identifier detId = m_idHelperSvc->detElId(id);
      idClusters[detId].push_back(clus);
    }

    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " creating CompetingMuonClustersOnTrack for " << idClusters.size() << " chambers " << std::endl;
    
    IdClusIt chit = idClusters.begin();
    IdClusIt chit_end = idClusters.end();
    for( ;chit!=chit_end;++chit ){
      if( msgLvl(MSG::VERBOSE) ) {
        msg(MSG::VERBOSE) << " in " << m_idHelperSvc->toStringDetEl(chit->first)
          << "  clusters: " << chit->second.size() << std::endl;

        std::vector<const MuonClusterOnTrack*>::iterator cl_it = chit->second.begin();
        std::vector<const MuonClusterOnTrack*>::iterator cl_it_end = chit->second.end();
        for( ;cl_it!=cl_it_end;++cl_it){
          msg(MSG::VERBOSE) << "   " << m_idHelperSvc->toString((*cl_it)->identify());
	  
          // hack to get correct print-out
          if( cl_it+1 == cl_it_end ) msg(MSG::VERBOSE) << endmsg;
          else                     msg(MSG::VERBOSE) << std::endl;
        }
      }

      if( chit->second.empty() ){
        ATH_MSG_WARNING(" empty list, could not create CompetingMuonClustersOnTrack in chamber   "
          << m_idHelperSvc->toString(chit->first) );
        continue;
      }

      // only create competing ROT if there is more than one PRD
      if( chit->second.size() == 1 ) {
        hits.push_back(chit->second.front());
        allHits.push_back(chit->second.front());
        continue;
      }

      // create list of PRDs 
      std::list<const Trk::PrepRawData*> prds;
      std::vector<const MuonClusterOnTrack*>::iterator cl_it = chit->second.begin();
      std::vector<const MuonClusterOnTrack*>::iterator cl_it_end = chit->second.end();
      for( ;cl_it!=cl_it_end;++cl_it){
        const Trk::PrepRawData* prd = (*cl_it)->prepRawData();
        if (prd){ 
          prds.push_back( prd );
        } else {
          ATH_MSG_WARNING("MuonClusterOnTrack has no PRD.");
        }
      }

      const CompetingMuonClustersOnTrack* comprot = m_compClusterCreator->createBroadCluster(prds,0);
      if( !comprot ){
        ATH_MSG_WARNING(" could not create CompetingMuonClustersOnTrack in chamber   " << m_idHelperSvc->toString(chit->first));
        continue;
      }
      hits.push_back(comprot);
      allHits.push_back(comprot);
      
      // add to garbage collection
      measurementsToBeDeleted.push_back(comprot);
    }
  }


  bool MuPatCandidateTool::recalculateCandidateSegmentContent( MuPatTrack& candidate ) const {

    // loop over track and get the chambers on the track
    const DataVector<const Trk::TrackStateOnSurface>* states = candidate.track().trackStateOnSurfaces();
    if( !states ) return false;

    std::set<MuonStationIndex::ChIndex> chambers;

    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){

      // only look at measurements
      if( !(*tsit)->type(Trk::TrackStateOnSurface::Measurement) || (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) continue;

      // get chamber index
      Identifier id = m_edmHelperSvc->getIdentifier(*meas);
      if( !id.is_valid() || !m_idHelperSvc->isMuon(id) ) continue;
      
      // don't include trigger hits
      if( m_idHelperSvc->isTrigger(id) ) continue;

      chambers.insert(m_idHelperSvc->chamberIndex(id));
      
      if( msgLvl(MSG::VERBOSE) ) {
	msg(MSG::VERBOSE) << " in recal " << m_idHelperSvc->toString(id) << endmsg;
      }

    }
    if( msgLvl(MSG::VERBOSE) ) {
      msg(MSG::VERBOSE) << " recalculateCandidateSegmentContent, old chambers " << candidate.chambers().size()
	     << " new chambers " << chambers.size() << endmsg;
      std::set<MuonStationIndex::ChIndex>::iterator chit = candidate.chambers().begin();
      std::set<MuonStationIndex::ChIndex>::iterator chit_end = candidate.chambers().end();
      for( ;chit!=chit_end;++chit ){
	if( !chambers.count(*chit) ){
	  msg(MSG::VERBOSE) << " removing chamber index from candidate " << MuonStationIndex::chName(*chit) << endmsg;
	}
      }
    }


    // reset chamber content
    bool bRemovedSegments = candidate.resetChambersOnCandidate(chambers);

    // recalculate hit list
    candidate.hitList().clear();
    m_hitHandler->create( candidate.track(),candidate.hitList() ); 	  
    // update the hit summary
    updateHits(candidate,candidate.track().measurementsOnTrack()->stdcont());

    if( msgLvl(MSG::VERBOSE) ){
      msg(MSG::VERBOSE)  << m_hitHandler->print(candidate.hitList()) << endmsg;
    }

    return bRemovedSegments;
  }


  std::set<const MuonGM::MuonReadoutElement*> MuPatCandidateTool::readoutElements( const MuPatCandidateBase& entry ) const {
    std::set<const MuonGM::MuonReadoutElement*> roEls;

    MuPatHitCit it = entry.hitList().begin();
    MuPatHitCit it_end = entry.hitList().end();
    for(  ; it!=it_end; ++it ){
      if( !(*it)->info().id.is_valid() ) continue;
      // only want MDT or CSC detEls
      if ( !m_idHelperSvc->isMdt( (*it)->info().id ) && !m_idHelperSvc->isCsc( (*it)->info().id ) ) continue;

      const Trk::MeasurementBase& meas = (*it)->measurement();
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&meas);
      if ( !rot ) {
        const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&meas);
	if ( crot ) {
          const std::vector<const MuonClusterOnTrack*>& rots = crot->containedROTs();
          if ( !rots.empty() ) rot = rots.front();
        }
      }
      if ( rot ) {
        const MuonGM::MuonReadoutElement* roEl = dynamic_cast<const MuonGM::MuonReadoutElement*>(rot->detectorElement());
        if ( roEl ) roEls.insert( roEl );
      }
    }
    return roEls;
  }
  

  
  void MuPatCandidateTool::cleanUp() const {
    // delete segments and clear vector
    std::lock_guard<std::mutex> lock{m_mutex};
    const EventContext& ctx = Gaudi::Hive::currentContext();
    CacheEntry* ent{m_cache.get(ctx)};
    if (ent->m_evt != ctx.evt()) {
      ent->m_evt = ctx.evt();
    }
    m_hitHandler->cleanUp();
    ent->cleanUp();
  }

  std::string MuPatCandidateTool::print( const MuPatSegment& segment, int level ) const {
    std::ostringstream oss;
    oss << segment.name << ": " << m_printer->print( *segment.segment ) << " q " << segment.quality;
    if( level == 0 ) return oss.str();

    if ( segment.hitList().size() >= 2 ) {
      MuPatHitDistanceAlongParameters distCal;
      oss << " length " << distCal( segment.hitList().front(), segment.hitList().back() );
    }
    oss << std::endl << m_hitHandler->print( segment.hitList(), true, false, false );
    return oss.str();
  }

  std::string MuPatCandidateTool::print( const std::vector<MuPatSegment*>& segments, int level ) const {
    std::vector<MuPatSegment*>::const_iterator it = segments.begin();
    std::vector<MuPatSegment*>::const_iterator it_end = segments.end();
    std::ostringstream oss;
    oss << " Segment candidate vector: " << segments.size();
    for( ;it!=it_end;++it ) oss << std::endl << print(**it,level);
    
    return oss.str();
  }

  std::string MuPatCandidateTool::print( const MuPatTrack& track, int level ) const{
    std::ostringstream oss;
    oss << m_printer->print(track.track()) << std::endl
	<< m_printer->printStations(track.track());
    
    if( level == 0 ) return oss.str();
    
    const std::vector<MuPatSegment*>& segs = track.segments();
    oss << std::endl << print( segs, 0 );
    
    return oss.str();
  }

  std::string MuPatCandidateTool::print( const std::vector<MuPatTrack*>& tracks, int level ) const{
    std::ostringstream oss;
    oss << "MuPatTracks " << tracks.size() << std::endl;
    std::vector<MuPatTrack*>::const_iterator tit = tracks.begin();
    std::vector<MuPatTrack*>::const_iterator tit_end = tracks.end();
    for( ;tit!=tit_end;++tit ) oss << " " << print(**tit,level) << std::endl;
    
    return oss.str();
  }


  std::string MuPatCandidateTool::print( const MuPatCandidateBase& candidate, int level ) const{
    const MuPatSegment* seg = dynamic_cast<const MuPatSegment*>(&candidate);
    if( seg ) return print(*seg, level);

    const MuPatTrack* track = dynamic_cast<const MuPatTrack*>(&candidate);
    if( track ) return print(*track,level);

    return "Unknown candidate type";
  }

} // namespace Muon

