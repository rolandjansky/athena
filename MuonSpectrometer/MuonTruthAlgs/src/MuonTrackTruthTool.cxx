/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTruthAlgs/MuonTrackTruthTool.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkEventUtils/RoT_Extractor.h"

#include <iostream>

namespace Muon {

  MuonTrackTruthTool::MuonTrackTruthTool(const std::string& ty,const std::string& na,const IInterface* pa) :
      AthAlgTool(ty,na,pa), 
      m_detMgr(nullptr) {
    declareInterface<IMuonTrackTruthTool>(this);
  }

  StatusCode MuonTrackTruthTool::initialize()
  {
    ATH_CHECK(detStore()->retrieve( m_detMgr ));
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_truthTrajectoryBuilder.retrieve());

    // add muons 
    if( m_pdgsToBeConsidered.value().empty() ){
      m_selectedPdgs.insert(13);
      m_selectedPdgs.insert(-13);
    }else{
      // add pdgs
      for( auto pdg : m_pdgsToBeConsidered.value() ) {
        m_selectedPdgs.insert(pdg);
      }
      ATH_MSG_DEBUG(" PDG codes used for matching");
      for( auto val : m_selectedPdgs ) {
        ATH_MSG_DEBUG(" " << val);
      }
    }
    return StatusCode::SUCCESS;
  }

  int MuonTrackTruthTool::manipulateBarCode( int barcode ) const {
    if( m_manipulateBarCode ) return barcode%10000;
    return barcode;
  }

  bool MuonTrackTruthTool::SortResultByMatchedHits::operator()(const MuonTrackTruthTool::MatchResult& r1,const MuonTrackTruthTool::MatchResult& r2) const {
    return operator()(r1.second,r2.second);
  }

  bool MuonTrackTruthTool::SortResultByMatchedHits::operator()(const MuonTrackTruthTool::SegmentMatchResult& r1,const MuonTrackTruthTool::SegmentMatchResult& r2) const {
    return operator()(r1.second,r2.second);
  }

  bool MuonTrackTruthTool::SortResultByMatchedHits::operator()(const MuonTrackTruth& t1,
							       const MuonTrackTruth& t2 ) const {    
    // if not from same track sort by address (should be unique)
    if( t1.truthTrack && !t2.truthTrack )  return true;
    if( !t1.truthTrack && t2.truthTrack )  return false;
    if( !t1.truthTrack && !t2.truthTrack ) return false;
    if( t1.truthTrack->GetBarCode() == t2.truthTrack->GetBarCode() ) return t1.numberOfMatchedHits() > t2.numberOfMatchedHits();
    return t1.truthTrack->GetBarCode() < t2.truthTrack->GetBarCode();
  }



  MuonTrackTruthTool::ResultVec MuonTrackTruthTool::match( const TrackCollection& tracks ) const {
    ResultVec result;
    result.reserve(tracks.size());
    
    // loop over tracks and match each of them with truth
    TrackCollection::const_iterator tit = tracks.begin();
    TrackCollection::const_iterator tit_end = tracks.end();
    for( ;tit!=tit_end;++tit ){

      MuonTrackTruth match = getTruth(**tit);
      if( !match.truthTrack ) continue;

      if( match.numberOfMatchedHits() == 0 ) continue;

      // create truth association 
      result.push_back( std::make_pair(*tit,match) );    
    }
    
    // sort result per muon and per number of matched hits
    std::stable_sort(result.begin(),result.end(),SortResultByMatchedHits());

    return result;
  }

  MuonTrackTruthTool::SegmentResultVec MuonTrackTruthTool::match(const std::vector<const MuonSegment*>& segments ) const {
    SegmentResultVec result;
    result.reserve(segments.size());

    // loop over tracks and match each of them with truth
    std::vector<const MuonSegment*>::const_iterator sit = segments.begin();
    std::vector<const MuonSegment*>::const_iterator sit_end = segments.end();
    for( ;sit!=sit_end;++sit ){
      
      // create truth association 
      result.push_back( std::make_pair(*sit,getTruth(**sit)) );    
    }
    
    // sort result per muon and per number of matched hits
    std::stable_sort(result.begin(),result.end(),SortResultByMatchedHits());

    return result;
  }

  const MuonTrackTruthTool::TruthTree MuonTrackTruthTool::createTruthTree(const TrackRecordCollection* truthTrackCol, const McEventCollection* mcEventCollection,
									   std::vector<const MuonSimDataCollection*> muonSimData, const CscSimDataCollection* cscSimDataMap) const {

    
    clear();

    if( truthTrackCol->empty() ) {
      ATH_MSG_WARNING(" TrackRecordCollection is empty ");
      return m_truthTree;
    }

    const HepMC::GenEvent* genEvent = nullptr;
    if( !mcEventCollection->empty() ) {
      ATH_MSG_VERBOSE( "McEventCollection size " << mcEventCollection->size());
      if( mcEventCollection->size() == 1 ) genEvent = mcEventCollection ->front();
    }
    
    ATH_MSG_VERBOSE(" creating truth tree from track record " << truthTrackCol->size());

    TrackRecordConstIterator tr_it = truthTrackCol->begin();
    TrackRecordConstIterator tr_it_end = truthTrackCol->end();
    for(;tr_it!=tr_it_end; ++tr_it){
      int PDGCode( (*tr_it).GetPDGCode() ) ;
      //       int barcode( manipulateBarCode( (*tr_it).GetBarCode()) );	
      int barcode = (*tr_it).GetBarCode();	
      if( !m_matchAllParticles && !selectPdg(PDGCode) ) {
	ATH_MSG_VERBOSE(" discarding truth track: pdg " <<  PDGCode << "  barcode " << barcode);
	continue;
      }

      // check whether barcode is already in, skip if that is the case
      if( m_barcodeMap.count(barcode) ){
	ATH_MSG_VERBOSE(" barcode " << barcode << " already in map, final state barcode " << m_barcodeMap[barcode]);	  
	continue;
      }
      ATH_MSG_VERBOSE(" found new particle with pdgid " << PDGCode << " in truth record, barcode " << barcode);

      TruthTrajectory* truthTrajectory = 0;
      // associate the muon truth with the gen event info
      if( genEvent ){
	HepMC::GenParticle* genParticle = genEvent->barcode_to_particle( (*tr_it).GetBarCode() );
	if( genParticle ){
	  truthTrajectory = new TruthTrajectory();
	  m_truthTrajectoryBuilder->buildTruthTrajectory(truthTrajectory,genParticle);
	  if( !truthTrajectory->empty() ){
	    
	    // always use barcode of the 'final' particle in chain in map
	    barcode = truthTrajectory->front()->barcode();
	    
	    if( msgLvl(MSG::VERBOSE) ) {
	      ATH_MSG_VERBOSE(" found GenParticle: size " 
                                << truthTrajectory->size() << " fs barcode " << barcode << " pdg " << truthTrajectory->front()->pdg_id()
                                << " p " << truthTrajectory->front()->momentum().rho());
	      if( truthTrajectory->front()->production_vertex() ) {
                ATH_MSG_VERBOSE(" vertex: r  " << truthTrajectory->front()->production_vertex()->position().perp() 
                                  << " z " << truthTrajectory->front()->production_vertex()->position().z());
              }
	    }
	    
	    // now collect all barcodes beloning to this TruthTrajectory
	    std::vector<HepMcParticleLink>::const_iterator pit = truthTrajectory->begin();
	    std::vector<HepMcParticleLink>::const_iterator pit_end = truthTrajectory->end();
	    for( ;pit!=pit_end;++pit ){
	      int code = (*pit)->barcode();
              
              if( msgLvl(MSG::VERBOSE) && code != barcode ) {
                ATH_MSG_VERBOSE("  secondary barcode: " << code << " pdg " << (*pit)->pdg_id() 
                                  << " p " << (*pit)->momentum().rho());
                if( (*pit)->production_vertex() ) ATH_MSG_VERBOSE(" vertex: r  " << (*pit)->production_vertex()->position().perp() 
                                                                    << " z " << (*pit)->production_vertex()->position().z());
		// sanity check 
		if( m_barcodeMap.count(code) ) ATH_MSG_VERBOSE("  pre-existing barcode " << code);
	      }
		
	      // enter barcode
	      m_barcodeMap[code] = barcode;
	    }
	  }else{
	    ATH_MSG_WARNING(" empty truth trajectory " << barcode);
	  }
	}
      }else{
	// add one to one relation
	m_barcodeMap[barcode] = barcode;
      }
      
      if( m_truthTree.count(barcode) ) {
	ATH_MSG_WARNING(" found muon barcode twice in truth record: " << barcode);
	continue;
      }
	
      TruthTreeEntry& entry = m_truthTree[barcode];
      entry.truthTrack = &(*tr_it);
      entry.truthTrajectory = truthTrajectory;
      m_truthTrajectoriesToBeDeleted.push_back(truthTrajectory);
    }
    
    // add sim data collections
    for(const MuonSimDataCollection* simDataMap : muonSimData){
      addSimDataToTree(simDataMap);
    }
    if(cscSimDataMap){
      addCscSimDataToTree(cscSimDataMap);
    }

    unsigned int ngood(0);
    std::vector<int> badBarcodes;
    // erase entries with too few hits or no track record
    TruthTreeIt it = m_truthTree.begin();
    for( ;it!=m_truthTree.end();++it ){
      bool erase = false;
      unsigned int nhits = it->second.mdtHits.size() + it->second.rpcHits.size() + it->second.tgcHits.size() + 
	it->second.cscHits.size() + it->second.stgcHits.size() + it->second.mmHits.size();
      if( !it->second.truthTrack ) erase = true;
      if(  nhits < m_minHits ) erase = true;
      
      if( erase ){
	ATH_MSG_VERBOSE(" Erasing entry: barcode " << it->second.truthTrack->GetBarCode() 
                        << " manip " << manipulateBarCode(it->second.truthTrack->GetBarCode()) << " hits " << nhits);
	badBarcodes.push_back(it->first);
      }else{
	++ngood;
	ATH_MSG_VERBOSE(" Keeping entry: barcode " << it->second.truthTrack->GetBarCode() 
                        << " manip " << manipulateBarCode(it->second.truthTrack->GetBarCode()) << " hits " << nhits);
      }
    }
    
    std::vector<int>::iterator badIt = badBarcodes.begin();
    std::vector<int>::iterator badIt_end = badBarcodes.end();
    for( ;badIt!=badIt_end;++badIt ) m_truthTree.erase(*badIt);

    if( ngood != m_truthTree.size() ){
      ATH_MSG_WARNING(" Problem cleaning map: size " << m_truthTree.size() << " accepted entries " << ngood);
    }
    
    
    if( m_doSummary || msgLvl(MSG::DEBUG) ){
      ATH_MSG_INFO(" summarizing truth tree: number of particles " << m_truthTree.size());
      TruthTreeIt it = m_truthTree.begin();
      TruthTreeIt it_end = m_truthTree.end();
      for( ;it!=it_end;++it ){
	if( !it->second.truthTrack ) ATH_MSG_INFO(" no TrackRecord ");
	else{
	  ATH_MSG_INFO(" PDG " << it->second.truthTrack->GetPDGCode() << " barcode " << it->second.truthTrack->GetBarCode() 
                         << " manip " << manipulateBarCode(it->second.truthTrack->GetBarCode()));
	}
	if( !it->second.mdtHits.empty() )  ATH_MSG_INFO(" mdt  " << it->second.mdtHits.size());
	if( !it->second.rpcHits.empty() )  ATH_MSG_INFO(" rpc  " << it->second.rpcHits.size());
	if( !it->second.tgcHits.empty() )  ATH_MSG_INFO(" tgc  " << it->second.tgcHits.size());
	if( !it->second.cscHits.empty() )  ATH_MSG_INFO(" csc  " << it->second.cscHits.size());
	if( !it->second.stgcHits.empty() ) ATH_MSG_INFO(" stgc " << it->second.stgcHits.size());
	if( !it->second.mmHits.empty() )   ATH_MSG_INFO(" mm   " << it->second.mmHits.size());
	if( it->second.mdtHits.empty() && it->second.rpcHits.empty() && it->second.tgcHits.empty() && 
	    it->second.cscHits.empty() && it->second.stgcHits.empty()  && it->second.mmHits.empty() ) ATH_MSG_INFO(" no hits ");
      }
    }

    return m_truthTree;
  }


  void MuonTrackTruthTool::addSimDataToTree( const MuonSimDataCollection* simDataCol ) const {

    // loop over sim collection and check whether identifiers are on track
    MuonSimDataCollection::const_iterator it = simDataCol->begin();
    MuonSimDataCollection::const_iterator it_end = simDataCol->end();
    for( ;it!=it_end;++it ){

      Identifier id = it->first;
      
      // loop over deposits
      std::vector<MuonSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
      std::vector<MuonSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
      for( ;dit!=dit_end;++dit ){
        int barcodeIn = dit->first.barcode();
	std::map<int,int>::iterator bit = m_barcodeMap.find(barcodeIn);
	if( bit == m_barcodeMap.end() ){
	  ATH_MSG_VERBOSE( " discarding " << "  " << m_idHelperSvc->toString(id) << "   barcode " << barcodeIn);
	  continue;
	}
	// replace barcode with barcode from map
	int barcode = bit->second;

	TruthTreeIt eit = m_truthTree.find(barcode);
	if( eit == m_truthTree.end() ){
	  ATH_MSG_VERBOSE( " discarding " << "  " << m_idHelperSvc->toString(id) << "   barcode " << barcode);
	  continue;
	}

	if( m_idHelperSvc->isMdt(id) ){
	  if( m_detMgr && !m_detMgr->getMdtReadoutElement(id) ) {
	    ATH_MSG_VERBOSE( " discarding: no detEl " << "  " << m_idHelperSvc->toString(id) 
                             << "   barcode " << barcode);
	    continue;
	  }
	  eit->second.mdtHits.insert(*it);
	}else if( m_idHelperSvc->isRpc(id) ){
	  if( m_detMgr && !m_detMgr->getRpcReadoutElement(id) ) {
	    ATH_MSG_VERBOSE( " discarding: no detEl " << "  " << m_idHelperSvc->toString(id) 
                             << "   barcode " << barcode);
	    continue;
	  }
	  
	  if( m_idHelperSvc->stationIndex(id) == MuonStationIndex::BO && m_idHelperSvc->rpcIdHelper().doubletR(id) == 2 ){
	    ATH_MSG_VERBOSE(" Discarding non existing RPC hit " << m_idHelperSvc->toString(id));
	    continue;
	  }
	  
	  eit->second.rpcHits.insert(*it);
	}else if( m_idHelperSvc->isTgc(id) ) {
	  if( m_detMgr && !m_detMgr->getTgcReadoutElement(id) ) {
	    ATH_MSG_VERBOSE( " discarding: no detEl " << "  " << m_idHelperSvc->toString(id) 
                             << "   barcode " << barcode);
	    continue;
	  }
	  eit->second.tgcHits.insert(*it);
	}else if( m_idHelperSvc->issTgc(id) ) {
	  if( m_detMgr && !m_detMgr->getsTgcReadoutElement(id) ) {
	    ATH_MSG_VERBOSE( " discarding: no detEl " << "  " << m_idHelperSvc->toString(id) 
                             << "   barcode " << barcode);
	    continue;
	  }
	  eit->second.stgcHits.insert(*it);
	}else if( m_idHelperSvc->isMM(id) ) {
	  if( m_detMgr && !m_detMgr->getMMReadoutElement(id) ) {
	    ATH_MSG_VERBOSE( " discarding: no detEl " << "  " << m_idHelperSvc->toString(id) 
                             << "   barcode " << barcode);
	    continue;
	  }
	  eit->second.mmHits.insert(*it);
	}
        if( msgLvl(MSG::VERBOSE) ){
          ATH_MSG_VERBOSE(" adding hit " << m_idHelperSvc->toString(id) << "   barcode " << barcode);
	  if( barcode != barcodeIn ) ATH_MSG_VERBOSE(" hit barcode " << barcodeIn);
	}
      }
    }    
  }

  void MuonTrackTruthTool::addCscSimDataToTree( const CscSimDataCollection* simDataCol ) const {

    // loop over sim collection and check whether identifiers are on track
    CscSimDataCollection::const_iterator it = simDataCol->begin();
    CscSimDataCollection::const_iterator it_end = simDataCol->end();
    for( ;it!=it_end;++it ){

      Identifier id = it->first;

      // loop over deposits
      std::vector<CscSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
      std::vector<CscSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
      for( ;dit!=dit_end;++dit ){

	int barcodeIn = manipulateBarCode(dit->first.barcode());
	std::map<int,int>::iterator bit = m_barcodeMap.find(barcodeIn);
	if( bit == m_barcodeMap.end() ){
	  ATH_MSG_VERBOSE( " discarding " << "  " << m_idHelperSvc->toString(id) << "   barcode " << barcodeIn);
	  continue;
	}
	// replace barcode with barcode from map
	int barcode = bit->second;

	TruthTreeIt eit = m_truthTree.find(barcode);
	if( eit == m_truthTree.end() ){
	  ATH_MSG_VERBOSE( " discarding " << "  " << m_idHelperSvc->toString(id) << "   barcode " << barcode);
	  continue;
	}

        if( msgLvl(MSG::VERBOSE) ){
          ATH_MSG_VERBOSE(" adding hit " << m_idHelperSvc->toString(id) << "   barcode " << barcode);
          if( barcode != barcodeIn ) ATH_MSG_VERBOSE(" hit barcode " << barcodeIn);
        }
        eit->second.cscHits.insert(*it);
      }
    }    
  }

  MuonTrackTruth MuonTrackTruthTool::getTruth( const Muon::MuonSegment& segment ) const {

    return getTruth(segment.containedMeasurements(),true);
  }


  MuonTrackTruth MuonTrackTruthTool::getTruth( const Trk::Track& track, bool restrictedTruth ) const {
    if( track.measurementsOnTrack() ) return getTruth(track.measurementsOnTrack()->stdcont(),restrictedTruth);
    return MuonTrackTruth();
  }

  MuonTrackTruth MuonTrackTruthTool::getTruth( const std::vector<const MuonSegment*>& segments, bool restrictedTruth ) const {
    Trk::RoT_Extractor rotExtractor;
    std::set<Identifier> ids;
    std::vector<const Trk::MeasurementBase*> measurements;
    std::vector<const MuonSegment*>::const_iterator sit = segments.begin();
    std::vector<const MuonSegment*>::const_iterator sit_end = segments.end();
    for( ;sit!=sit_end;++sit ){
      std::vector<const Trk::MeasurementBase*>::const_iterator mit = (*sit)->containedMeasurements().begin();
      std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = (*sit)->containedMeasurements().end();
      for( ;mit!=mit_end;++mit ){
	const Trk::MeasurementBase* meas = *mit;
	const Trk::RIO_OnTrack* rot = 0;
	rotExtractor.extract(rot,meas);
	if( !rot ) {
	  if( !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) ) ATH_MSG_WARNING(" Could not get rot from measurement ");
	  continue;
	}
	Identifier id = rot->identify();
	if( !id.is_valid() || !m_idHelperSvc->mdtIdHelper().is_muon(id) ) continue;
	if( ids.count(id) ) continue;
	measurements.push_back(meas);
	ids.insert(id);
      }
    }
    return getTruth(measurements,restrictedTruth);
  }


  MuonTrackTruth MuonTrackTruthTool::getTruth( const std::vector<const Trk::MeasurementBase*>& measurements,
					       bool restrictedTruth ) const {

    MuonTrackTruth bestMatch;
    bestMatch.truthTrack = 0;
    bestMatch.truthTrajectory = 0;

    unsigned int   nmatchedHitsBest = 0;
    // loop over muons and match hits
    TruthTreeIt tit = m_truthTree.begin();
    TruthTreeIt tit_end = m_truthTree.end();
    for( ;tit!=tit_end;++tit ){
      
      unsigned int nhits = tit->second.mdtHits.size() + tit->second.cscHits.size() + tit->second.rpcHits.size() + tit->second.tgcHits.size() 
	+ tit->second.stgcHits.size() + tit->second.mmHits.size();
      if( nhits == 0 ) continue; 

      MuonTrackTruth trackTruth = getTruth(measurements,tit->second,restrictedTruth);
      unsigned int nmatchedHits = trackTruth.numberOfMatchedHits();
      ATH_MSG_DEBUG(" performed truth match for particle with barcode: " << tit->first << " overlap " << nmatchedHits 
                    << " fraction  "<< (double)nmatchedHits/(double)nhits);
      if( nmatchedHits > 0 && nmatchedHits > nmatchedHitsBest ){
        bestMatch = trackTruth;
        nmatchedHitsBest = nmatchedHits;
      }
    }
    
    return bestMatch;
  }

  MuonTrackTruth MuonTrackTruthTool::getTruth( const std::vector<const Trk::MeasurementBase*>& measurements,
                                               TruthTreeEntry& truthEntry, bool restrictedTruth ) const {
    
    Trk::RoT_Extractor rotExtractor;
    MuonTrackTruth trackTruth;
    trackTruth.truthTrack = truthEntry.truthTrack;
    trackTruth.truthTrajectory = truthEntry.truthTrajectory;
    
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = measurements.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = measurements.end();
    for( ; mit!=mit_end ; ++mit ){
      
      // check whether state is a measurement
      const Trk::MeasurementBase* meas = *mit;
      if( !meas ){	
        continue;
      }

      const Trk::RIO_OnTrack* rot = 0;
      rotExtractor.extract(rot,meas);
      if( !rot ) {
        if( !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) ) ATH_MSG_WARNING(" Could not get rot from measurement ");
        continue;
      }
      Identifier id = rot->identify();
      if( !id.is_valid() || !m_idHelperSvc->mdtIdHelper().is_muon(id) ) continue;

            
      if( m_idHelperSvc->isMdt(id) ){
        addMdtTruth( trackTruth.mdts, id, *meas, truthEntry.mdtHits );
      }else if( m_idHelperSvc->isCsc(id) ){
        addClusterTruth( trackTruth.cscs, id, *meas, truthEntry.cscHits );
      }else if( m_idHelperSvc->isRpc(id) ){
        addClusterTruth( trackTruth.rpcs, id, *meas, truthEntry.rpcHits );
      }else if( m_idHelperSvc->isTgc(id) ){
        addClusterTruth( trackTruth.tgcs, id, *meas, truthEntry.tgcHits );
      }else if( m_idHelperSvc->issTgc(id) ){
        addClusterTruth( trackTruth.stgcs, id, *meas, truthEntry.stgcHits );
      }else if( m_idHelperSvc->isMM(id) ){
        addClusterTruth( trackTruth.mms, id, *meas, truthEntry.mmHits );
      }
    }

    addMissedHits(trackTruth.mdts,trackTruth.mdts.matchedHits,trackTruth.mdts.matchedChambers,truthEntry.mdtHits,restrictedTruth);
    addMissedHits(trackTruth.cscs,trackTruth.cscs.matchedHits,trackTruth.cscs.matchedChambers,truthEntry.cscHits,restrictedTruth);
    addMissedHits(trackTruth.rpcs,trackTruth.rpcs.matchedHits,trackTruth.rpcs.matchedChambers,truthEntry.rpcHits,restrictedTruth);
    addMissedHits(trackTruth.tgcs,trackTruth.tgcs.matchedHits,trackTruth.tgcs.matchedChambers,truthEntry.tgcHits,restrictedTruth);
    addMissedHits(trackTruth.stgcs,trackTruth.stgcs.matchedHits,trackTruth.stgcs.matchedChambers,truthEntry.stgcHits,restrictedTruth);
    addMissedHits(trackTruth.mms,trackTruth.mms.matchedHits,trackTruth.mms.matchedChambers,truthEntry.mmHits,restrictedTruth);
    
    return trackTruth;
  }

  void MuonTrackTruthTool::addMissedHits( MuonTechnologyTruth& truth, const std::set<Identifier>& ids, const std::set<Identifier>& chids, 
                                          const MuonSimDataCollection& simCol, bool restrictedTruth ) const {
    
    // loop over sim collection and check whether identifiers are on track
    MuonSimDataCollection::const_iterator it = simCol.begin();
    MuonSimDataCollection::const_iterator it_end = simCol.end();
    for( ;it!=it_end;++it ){
      
      Identifier id = it->first;
      // for trigger chambers use layer id
      if( m_idHelperSvc->isTrigger(id) || m_idHelperSvc->isCsc(id) ) id = m_idHelperSvc->layerId(id);

      int isOnTrack = ids.count(id);
      if( isOnTrack ) continue;

      // if restricted truth mode, skip if chamber has not hits
      Identifier chid = m_idHelperSvc->chamberId(id);
      bool chamberHasHits = chids.count(chid);
      if( restrictedTruth && !chamberHasHits ) continue;

      // loop over deposits
      std::vector<MuonSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
      std::vector<MuonSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
      for( ;dit!=dit_end;++dit ){
        truth.missedHits.insert(id);
        // only fill a missed chamber if the chamber had no hits
        if( !chamberHasHits ) truth.missedChambers.insert(chid);
	
      }
    }
  }

  void MuonTrackTruthTool::addMissedHits( MuonTechnologyTruth& truth, const std::set<Identifier>& ids, const std::set<Identifier>& chids,
                                          const CscSimDataCollection& simCol, bool restrictedTruth ) const {
    
    // loop over sim collection and check whether identifiers are on track
    CscSimDataCollection::const_iterator it = simCol.begin();
    CscSimDataCollection::const_iterator it_end = simCol.end();
    for( ;it!=it_end;++it ){
      
      Identifier id = m_idHelperSvc->layerId(it->first);

      int isOnTrack = ids.count(id);
      if( isOnTrack ) continue;

      // if restricted truth mode, skip if chamber has not hits
      Identifier chid = m_idHelperSvc->chamberId(id);
      bool chamberHasHits = chids.count(chid);
      if( restrictedTruth && !chamberHasHits ) continue;
      
      // loop over deposits
      std::vector<CscSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
      std::vector<CscSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
      for( ;dit!=dit_end;++dit ){
        truth.missedHits.insert(id);
        // only fill a missed chamber if the chamber had no hits
        if( !chamberHasHits ) truth.missedChambers.insert(chid);
      }
    }
  }

  void MuonTrackTruthTool::addMdtTruth( MuonTechnologyTruth& truth, const Identifier& id, const Trk::MeasurementBase& meas, 
                                        const MuonSimDataCollection& simCol ) const {

    const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(&meas);
    if( !mdt ){
      ATH_MSG_WARNING(" dynamic_cast to MdtDriftCircleOnTrack failed for measurement with id " << m_idHelperSvc->toString(id));
      return;
    }
    
    Identifier chid = m_idHelperSvc->chamberId(id);

    // find SimData corresponding to identifier
    MuonSimDataCollection::const_iterator it = simCol.find(id);
    if( it == simCol.end() ) {
      truth.wrongHits.insert(id);
      truth.wrongChambers.insert(chid);
      return;
    }
    
    std::vector<MuonSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
    std::vector<MuonSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
    for( ;dit!=dit_end;++dit ){
      double radius = dit->second.firstEntry();

      // check whether SDO and hit have same sign, don't check sign if drift radius < 0.3 mm as it will be badly determined
      double checkSign = fabs(mdt->driftRadius()) > 0.3 ? radius*mdt->driftRadius() : 1;
      if( checkSign >= 0 ) {
        truth.matchedHits.insert(id);
        truth.matchedChambers.insert(chid);
      }else{
        truth.wrongHits.insert(id);
        truth.wrongChambers.insert(chid);
      }
    }
  }

  void MuonTrackTruthTool::addClusterTruth( MuonTechnologyTruth& truth, const Identifier& id, const Trk::MeasurementBase& meas, 
                                            const MuonSimDataCollection& simCol ) const {
    
    Trk::RoT_Extractor rotExtractor;
    Identifier layid = m_idHelperSvc->layerId(id);
    Identifier chid = m_idHelperSvc->chamberId(id);

    MuonSimDataCollection::const_iterator it = simCol.end();

    bool goodCluster = false;
    const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&meas);
    if( crot ){
      for( unsigned int i=0;i<crot->numberOfContainedROTs();++i){
        const MuonClusterOnTrack* cluster = &crot->rioOnTrack(i);
        if( !cluster ) continue;
        // find SimData corresponding to identifier
        it = simCol.find(cluster->identify());
        if( it != simCol.end() ) {
          goodCluster = true;
          break;
        }
      }
    }else{
      // Find SimData corresponding to identifier
      const Trk::RIO_OnTrack* rot = 0;
      rotExtractor.extract(rot,&meas);
      const Trk::PrepRawData* prd = rot->prepRawData();
      if(prd) {
      // check if an identifier from the list of RDOs is matched to that in the SDO collection
        const std::vector<Identifier> rdoList = prd->rdoList();
        std::vector<Identifier>::const_iterator rit = rdoList.begin();
        std::vector<Identifier>::const_iterator rit_end = rdoList.end();
        for( ;rit!=rit_end;++rit ){
          it = simCol.find(*rit);
          if( it != simCol.end() ) {
            goodCluster = true;
            break;
          }
        }
      } else { 
        it = simCol.find(id);
        if( it != simCol.end() ) goodCluster = true;
      }
    }
    
    if( !goodCluster || it == simCol.end() ){
      truth.wrongHits.insert(id);
      return;
    }

    std::vector<MuonSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
    std::vector<MuonSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
    for( ;dit!=dit_end;++dit ){
      truth.matchedHits.insert(layid);
      truth.matchedChambers.insert(chid);
    }

  }

  void MuonTrackTruthTool::addClusterTruth( MuonTechnologyTruth& truth, const Identifier& id, const Trk::MeasurementBase& /*meas*/, 
                                            const CscSimDataCollection& simCol ) const {

    Identifier layid = m_idHelperSvc->layerId(id);
    Identifier chid = m_idHelperSvc->chamberId(id);
    // find SimData corresponding to identifier
    CscSimDataCollection::const_iterator it = simCol.find(id);
    if( it == simCol.end() ) {
      truth.wrongHits.insert(layid);
      truth.wrongChambers.insert(chid);
      return;
    }
    
    std::vector<CscSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
    std::vector<CscSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
    for( ;dit!=dit_end;++dit ){
      truth.matchedHits.insert(layid);
      truth.matchedChambers.insert(chid);
    }
  }

  void MuonTrackTruthTool::clear() const {
    m_truthTree.clear();
    m_barcodeMap.clear();
    std::vector<TruthTrajectory*>::iterator dit = m_truthTrajectoriesToBeDeleted.begin();
    std::vector<TruthTrajectory*>::iterator dit_end = m_truthTrajectoriesToBeDeleted.end();
    for( ;dit!=dit_end;++dit ) delete *dit;
    m_truthTrajectoriesToBeDeleted.clear();
  }

  const HepMC::GenParticle* MuonTrackTruthTool::getMother( const TruthTrajectory& traj, const int barcodeIn ) const {
    ATH_MSG_DEBUG( "getMother() : size = " << traj.size() );
    int pdgFinal = ( (traj.size()==0)?  -999 : traj.front()->pdg_id());
    bool foundBC = false;
    for( auto pit: traj){
      if (!pit) continue;
      if (pit->barcode()==barcodeIn || foundBC){
        foundBC = true;
        ATH_MSG_DEBUG( "getMother() : pdg = " << pit->pdg_id() << " barcode = " << pit->barcode () );
        if( pit->pdg_id() != pdgFinal ) { // the first case a track had a different flavour
          if (pit->pdg_id()==pdgFinal) ATH_MSG_ERROR( "Wrong pdgId association in getMother() " );
          return pit;
          break;
        }
      }
    }
   return nullptr;
  }

  const HepMC::GenParticle* MuonTrackTruthTool::getAncestor( const TruthTrajectory& traj, const int barcodeIn ) const {
    bool foundBC = false;
    for(auto pit: traj){
      if (!pit) continue;
      if (pit->barcode()==barcodeIn || foundBC){
        foundBC = true;
        if( pit->status() > 1 ) {//first non final state particle
          return pit;
          break;
        }
      }
    }
    return nullptr;
  }

  const std::pair<const HepMC::GenParticle*, unsigned int> MuonTrackTruthTool::getInitialPair( const TruthTrajectory& traj, const int barcodeIn ) const {
    std::pair<const HepMC::GenParticle*,unsigned int> thePair(nullptr,0);
    unsigned int scat = 0;
    ATH_MSG_DEBUG( "getFirst() : size = " << traj.size() );
    bool foundBC = false;
    int pdgFinal = 0;
    double ePrev = 0.;
    const HepMC::GenParticle* theFirst=nullptr;
    for(auto pit=traj.begin();pit!=traj.end();++pit){
      if ((*pit)->barcode()==barcodeIn || foundBC){
        if (!foundBC){
          foundBC = true;
          theFirst = *pit;
          pdgFinal = (*pit)->pdg_id();
        } else {
          if( (*pit)->pdg_id() == pdgFinal ) {
            auto pit_p = *pit;
            if ( (theFirst != pit_p.cptr()) && ((*pit)->momentum().t()!=ePrev) ) ++scat; // if the particle has not changed pdgid after the first step count as scatter. also avoid counting pure interface changes as scatter
          } else { // the first time this particle appears
            --pit;
            theFirst = *pit;
            break;
          }
        }
        ATH_MSG_DEBUG( "getFirst() : pt = " << (*pit)->momentum().perp() << " scat = " << scat );
        ePrev = (*pit)->momentum().t(); // prepare for comparing this entry with the next one
      }
    }
    // sanity check
    if (theFirst && theFirst->pdg_id()!=pdgFinal) ATH_MSG_ERROR("Wrong pdgId association in getFirst()");
    ATH_MSG_DEBUG( "Number of scatters = " << scat << " pdgId = " << pdgFinal );
  
    thePair.first = theFirst;
    thePair.second = scat;
    return thePair;
  }

  const HepMC::GenParticle* MuonTrackTruthTool::getInitial( const TruthTrajectory& traj, const int barcodeIn ) const {
    return  getInitialPair( traj, barcodeIn ).first;
  }

  unsigned int MuonTrackTruthTool::getNumberOfScatters( const TruthTrajectory& traj, const int barcodeIn ) const {
    return (getInitialPair( traj, barcodeIn )).second;
  }
}
/// eof

