/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "MuonTruthAlgs/MuonTrackTruthTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "TrkToolInterfaces/ITruthTrajectoryBuilder.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "TrkTruthData/TruthTrajectory.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonSimData/MuonSimData.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegment/MuonSegment.h"

#include <iostream>
#include <set>

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkEventUtils/RoT_Extractor.h"

#include "StoreGate/StoreGateSvc.h"

namespace Muon {

  MuonTrackTruthTool::MuonTrackTruthTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa), 
      m_detMgr(0),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_truthTrajectoryBuilder("Muon::MuonDecayTruthTrajectoryBuilder/MuonDecayTruthTrajectoryBuilder"),
      m_log(0)
  {
    declareInterface<IMuonTrackTruthTool>(this);

    declareProperty("MDT_SDO_Container",          m_MDT_SimDataMapName = "MDT_SDO" );
    declareProperty("CSC_SDO_Container",          m_CSC_SimDataMapName = "CSC_SDO" );
    declareProperty("RPC_SDO_Container",          m_RPC_SimDataMapName = "RPC_SDO" );
    declareProperty("TGC_SDO_Container",          m_TGC_SimDataMapName = "TGC_SDO" );
    declareProperty("STGC_SDO_Container",         m_STGC_SimDataMapName = "STGC_SDO" );
    declareProperty("MM_SDO_Container",           m_MM_SimDataMapName = "MM_SDO" );
    declareProperty("DoSummary",                  m_doSummary = false );
    declareProperty("ManipulateBarCode",          m_manipulateBarCode = false );
    declareProperty("MinHits",                    m_minHits = 4 );
    declareProperty("MatchAllParticles",          m_matchAllParticles = true );

  }


  MuonTrackTruthTool::~MuonTrackTruthTool(){}


  StatusCode MuonTrackTruthTool::initialize()
  {
    
    m_log = new MsgStream(msgSvc(),name());
    m_debug = m_log->level() <= MSG::DEBUG;
    m_verbose = m_log->level() <= MSG::VERBOSE;

    StatusCode sc = AlgTool::initialize(); 
    if ( sc.isFailure() ) return sc;

    sc=service("StoreGateSvc",m_storeGate);
    if (sc.isFailure()) {
      *m_log << MSG::ERROR << "StoreGate service not found !" << endreq;
      return sc;
    } 
    
    StoreGateSvc* detStore=0;
    sc = serviceLocator()->service("DetectorStore", detStore);
    
    if( sc.isFailure() ){
      *m_log << MSG::ERROR << " Could not get DetectorStore " << endreq;
      return StatusCode::FAILURE;
    }

    sc = detStore->retrieve( m_detMgr );
    if ( sc.isFailure() ) {
      *m_log << MSG::WARNING << " Cannot retrieve MuonDetDescrMgr " << endreq;
      //return StatusCode::FAILURE;
    }

    sc = m_idHelperTool.retrieve();
    if (sc.isSuccess()){
      *m_log<<MSG::DEBUG << "Retrieved " << m_idHelperTool << endreq;
    }else{
      *m_log<<MSG::ERROR<<"Could not get " << m_idHelperTool <<endreq; 
      return sc;
    }
    
    if( m_debug ){
      sc = m_printer.retrieve();
      if (sc.isSuccess()){
	*m_log<<MSG::DEBUG << "Retrieved " << m_printer << endreq;
      }else{
	*m_log<<MSG::ERROR<<"Could not get " << m_printer <<endreq; 
	return sc;
      }
    }

    sc = m_truthTrajectoryBuilder.retrieve();
    if (sc.isSuccess()){
      *m_log<<MSG::DEBUG << "Retrieved " << m_truthTrajectoryBuilder << endreq;
    }else{
      *m_log<<MSG::ERROR<<"Could not get " << m_truthTrajectoryBuilder <<endreq; 
      return sc;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonTrackTruthTool::finalize()
  {
    delete m_log;
    StatusCode sc = AlgTool::finalize(); 
    if( sc.isFailure() ) return StatusCode::FAILURE;
    return sc;
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
    std::sort(result.begin(),result.end(),SortResultByMatchedHits());

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
    std::sort(result.begin(),result.end(),SortResultByMatchedHits());

    return result;
  }

  const MuonTrackTruthTool::TruthTree& MuonTrackTruthTool::createTruthTree() const {

    
    clear();

    const TrackRecordCollection* truthTrackCol = getTruthTrackCollection();
    if( !truthTrackCol ) {
      *m_log << MSG::WARNING << " failed to retrieve TrackRecordCollection " << endreq;
      return m_truthTree;
    }

    if( truthTrackCol->empty() ) {
      *m_log << MSG::WARNING << " TrackRecordCollection is empty " << endreq;
      return m_truthTree;
    }

    const McEventCollection* mcEventCollection = 0;
    std::string mcLocation = "TruthEvent";
    const HepMC::GenEvent*    genEvent = 0;
    if ( m_storeGate->contains<McEventCollection>(mcLocation) ) {
      if( m_storeGate->retrieve(mcEventCollection,mcLocation ).isSuccess() ){
	if( !mcEventCollection->empty() ) {
	  if( m_debug ) *m_log << MSG::VERBOSE << "McEventCollection retrieved at location " << mcLocation << " size " << mcEventCollection->size() << endreq;
	  if( mcEventCollection->size() == 1 ) genEvent = mcEventCollection ->front();
	}
      }
    }
    
    if( m_debug ) *m_log << MSG::VERBOSE << " creating truth tree from track record " << truthTrackCol->size() << endreq;

    TrackRecordConstIterator tr_it = truthTrackCol->begin();
    TrackRecordConstIterator tr_it_end = truthTrackCol->end();
    for(;tr_it!=tr_it_end; ++tr_it){
      int PDGCode( (*tr_it).GetPDGCode() ) ;
//       int barcode( manipulateBarCode( (*tr_it).GetBarCode()) );	
      int barcode = (*tr_it).GetBarCode();	
      if( !m_matchAllParticles && abs(PDGCode) != 13 ) {
	if( m_debug ) *m_log << MSG::VERBOSE << " discarding truth track: pdg " <<  PDGCode << "  barcode " << barcode << endreq;
	continue;
      }

      // check whether barcode is already in, skip if that is the case
      if( m_barcodeMap.count(barcode) ){
	if( m_debug ) *m_log << MSG::VERBOSE << " barcode " << barcode << " already in map, final state barcode " << m_barcodeMap[barcode] << endreq;	  
	continue;
      }
      if( m_debug ) *m_log << MSG::VERBOSE << " found new particle with pdgid " << PDGCode << " in truth record, barcode " << barcode << endreq;

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
	    
	    if( m_debug ) {
	      *m_log << MSG::VERBOSE << " found GenParticle: size " 
		     << truthTrajectory->size() << " fs barcode " << barcode << " pdg " << truthTrajectory->front()->pdg_id()
		     << " p " << truthTrajectory->front()->momentum().rho();
	      if( truthTrajectory->front()->production_vertex() ) *m_log << " vertex: r  " << truthTrajectory->front()->production_vertex()->point3d().perp() 
									 << " z " << truthTrajectory->front()->production_vertex()->point3d().z();
	      *m_log << endreq;
	    }
	    
	    // now collect all barcodes beloning to this TruthTrajectory
	    std::vector<HepMcParticleLink>::const_iterator pit = truthTrajectory->begin();
	    std::vector<HepMcParticleLink>::const_iterator pit_end = truthTrajectory->end();
	    for( ;pit!=pit_end;++pit ){
	      int code = (*pit)->barcode();
		
	      if( m_verbose ) {
		if( code != barcode ) {
		  *m_log << MSG::VERBOSE << "  secondary barcode: " << code << " pdg " << (*pit)->pdg_id() 
			 << " p " << (*pit)->momentum().rho();
		  if( (*pit)->production_vertex() ) *m_log << " vertex: r  " << (*pit)->production_vertex()->point3d().perp() 
							   << " z " << (*pit)->production_vertex()->point3d().z();

		  *m_log << endreq;
		}
		// sanity check 
		if( m_barcodeMap.count(code) ) *m_log << "  pre-existing barcode " << code << endreq;
	      }
		
	      // enter barcode
	      m_barcodeMap[code] = barcode;
	    }
	  }else{
	    *m_log << MSG::WARNING << " empty truth trajectory " << barcode << endreq;
	  }
	}
      }else{
	// add one to one relation
	m_barcodeMap[barcode] = barcode;
      }
      
      if( m_truthTree.count(barcode) ) {
	*m_log << MSG::WARNING << " found muon barcode twice in truth record: " << barcode << endreq;
	continue;
      }
	
      TruthTreeEntry& entry = m_truthTree[barcode];
      entry.truthTrack = &(*tr_it);
      entry.truthTrajectory = truthTrajectory;
      m_truthTrajectoriesToBeDeleted.push_back(truthTrajectory);
    }

    const MuonSimDataCollection* mdtSimDataMap = retrieveTruthCollection( m_MDT_SimDataMapName );
    if( !mdtSimDataMap) {
      *m_log << MSG::WARNING << " failed to retrieve MuonSimDataCollection: " << m_MDT_SimDataMapName << endreq;
    }else{
      addSimDataToTree(*mdtSimDataMap);
    }

    const CscSimDataCollection* cscSimDataMap = retrieveCscTruthCollection( m_CSC_SimDataMapName );
    if( !cscSimDataMap) {
      *m_log << MSG::WARNING << " failed to retrieve CSCSimDataCollection: " << m_CSC_SimDataMapName << endreq;
    }else{
      addSimDataToTree(*cscSimDataMap);
    }
    const MuonSimDataCollection* rpcSimDataMap = retrieveTruthCollection( m_RPC_SimDataMapName );
    if( !rpcSimDataMap) {
      *m_log << MSG::WARNING << " failed to retrieve MuonSimDataCollection: " << m_RPC_SimDataMapName << endreq;
    }else{
      addSimDataToTree(*rpcSimDataMap);
    }
    const MuonSimDataCollection* tgcSimDataMap = retrieveTruthCollection( m_TGC_SimDataMapName );
    if( !tgcSimDataMap) {
      *m_log << MSG::WARNING << " failed to retrieve MuonSimDataCollection: " << m_TGC_SimDataMapName << endreq;
    }else{
      addSimDataToTree(*tgcSimDataMap);
    }

    if( m_storeGate->contains<MuonSimDataCollection>(m_STGC_SimDataMapName) ) {
      const MuonSimDataCollection* stgcSimDataMap = retrieveTruthCollection( m_STGC_SimDataMapName );
      if( !stgcSimDataMap) {
	*m_log << MSG::WARNING << " failed to retrieve MuonSimDataCollection: " << m_STGC_SimDataMapName << endreq;
      }else{
	addSimDataToTree(*stgcSimDataMap);
      }
    }

    if( m_storeGate->contains<MuonSimDataCollection>(m_MM_SimDataMapName) ) {
      const MuonSimDataCollection* mmSimDataMap = retrieveTruthCollection( m_MM_SimDataMapName );
      if( !mmSimDataMap) {
	*m_log << MSG::WARNING << " failed to retrieve MuonSimDataCollection: " << m_MM_SimDataMapName << endreq;
      }else{
	addSimDataToTree(*mmSimDataMap);
      }
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
	if( m_verbose ) *m_log << MSG::VERBOSE << " Erasing entry: barcode " << it->second.truthTrack->GetBarCode() 
			       << " manip " << manipulateBarCode(it->second.truthTrack->GetBarCode()) << " hits " << nhits << endreq;
	badBarcodes.push_back(it->first);
	//m_truthTree.erase(it);
	//it = m_truthTree.begin();
      }else{
	++ngood;
	if( m_verbose ) *m_log << MSG::VERBOSE << " Keeping entry: barcode " << it->second.truthTrack->GetBarCode() 
			       << " manip " << manipulateBarCode(it->second.truthTrack->GetBarCode()) << " hits " << nhits << endreq;
      }
    }
    
    std::vector<int>::iterator badIt = badBarcodes.begin();
    std::vector<int>::iterator badIt_end = badBarcodes.end();
    for( ;badIt!=badIt_end;++badIt ) m_truthTree.erase(*badIt);

    if( ngood != m_truthTree.size() ){
      *m_log << MSG::WARNING << " Problem cleaning map: size " << m_truthTree.size() << " accepted entries " << ngood << endreq;
    }
    
    
    if( m_doSummary || m_log->level() <= MSG::DEBUG ){
      *m_log << MSG::INFO << " summarizing truth tree: number of particles " << m_truthTree.size()  << endreq;
      TruthTreeIt it = m_truthTree.begin();
      TruthTreeIt it_end = m_truthTree.end();
      for( ;it!=it_end;++it ){
	if( !it->second.truthTrack ) *m_log << " no TrackRecord ";
	else{
	  *m_log << " PDG " << it->second.truthTrack->GetPDGCode() << " barcode " << it->second.truthTrack->GetBarCode() 
		 << " manip " << manipulateBarCode(it->second.truthTrack->GetBarCode());
	  if( abs(it->second.truthTrack->GetPDGCode()) != 13 ) *m_log << " NoMuon ";
	}
	if( !it->second.mdtHits.empty() )  *m_log << " mdt  " << it->second.mdtHits.size();
	if( !it->second.rpcHits.empty() )  *m_log << " rpc  " << it->second.rpcHits.size();
	if( !it->second.tgcHits.empty() )  *m_log << " tgc  " << it->second.tgcHits.size();
	if( !it->second.cscHits.empty() )  *m_log << " csc  " << it->second.cscHits.size();
	if( !it->second.stgcHits.empty() ) *m_log << " stgc " << it->second.stgcHits.size();
	if( !it->second.mmHits.empty() )   *m_log << " mm   " << it->second.mmHits.size();
	if( it->second.mdtHits.empty() && it->second.rpcHits.empty() && it->second.tgcHits.empty() && 
	    it->second.cscHits.empty() && it->second.stgcHits.empty()  && it->second.mmHits.empty() ) *m_log <<" no hits ";
	*m_log << endreq;
      }
    }

    return m_truthTree;
  }


  void MuonTrackTruthTool::addSimDataToTree( const MuonSimDataCollection& simDataCol ) const {

    // loop over sim collection and check whether identifiers are on track
    MuonSimDataCollection::const_iterator it = simDataCol.begin();
    MuonSimDataCollection::const_iterator it_end = simDataCol.end();
    for( ;it!=it_end;++it ){

      Identifier id = it->first;
      
      // loop over deposits
      std::vector<MuonSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
      std::vector<MuonSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
      for( ;dit!=dit_end;++dit ){
// 	int barcodeIn = manipulateBarCode(dit->first.barcode());
        int barcodeIn = dit->first.barcode();
	std::map<int,int>::iterator bit = m_barcodeMap.find(barcodeIn);
	if( bit == m_barcodeMap.end() ){
	  if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding " << "  " << m_idHelperTool->toString(id) << "   barcode " << barcodeIn << endreq;
	  continue;
	}
	// replace barcode with barcode from map
	int barcode = bit->second;

	TruthTreeIt eit = m_truthTree.find(barcode);
	if( eit == m_truthTree.end() ){
	  if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding " << "  " << m_idHelperTool->toString(id) << "   barcode " << barcode << endreq;
	  continue;
	}

	if( m_idHelperTool->isMdt(id) ){
	  if( m_detMgr && !m_detMgr->getMdtReadoutElement(id) ) {
	    if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding: no detEl " << "  " << m_idHelperTool->toString(id) 
				   << "   barcode " << barcode << endreq;
	    continue;
	  }
	  eit->second.mdtHits.insert(*it);
	}else if( m_idHelperTool->isRpc(id) ){
	  if( m_detMgr && !m_detMgr->getRpcReadoutElement(id) ) {
	    if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding: no detEl " << "  " << m_idHelperTool->toString(id) 
				   << "   barcode " << barcode << endreq;
	    continue;
	  }
	  
	  if( m_idHelperTool->stationIndex(id) == MuonStationIndex::BO && m_idHelperTool->rpcIdHelper().doubletR(id) == 2 ){
	    if( m_debug ) *m_log << MSG::VERBOSE << " Discarding non existing RPC hit " << m_idHelperTool->toString(id) << endreq;
	    continue;
	  }
	  
	  eit->second.rpcHits.insert(*it);
	}else if( m_idHelperTool->isTgc(id) ) {
	  if( m_detMgr && !m_detMgr->getTgcReadoutElement(id) ) {
	    if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding: no detEl " << "  " << m_idHelperTool->toString(id) 
				   << "   barcode " << barcode << endreq;
	    continue;
	  }
	  eit->second.tgcHits.insert(*it);
	}else if( m_idHelperTool->issTgc(id) ) {
	  if( m_detMgr && !m_detMgr->getsTgcReadoutElement(id) ) {
	    if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding: no detEl " << "  " << m_idHelperTool->toString(id) 
				   << "   barcode " << barcode << endreq;
	    continue;
	  }
	  eit->second.stgcHits.insert(*it);
	}else if( m_idHelperTool->isMM(id) ) {
	  if( m_detMgr && !m_detMgr->getMMReadoutElement(id) ) {
	    if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding: no detEl " << "  " << m_idHelperTool->toString(id) 
				   << "   barcode " << barcode << endreq;
	    continue;
	  }
	  eit->second.mmHits.insert(*it);
	}
	if( m_verbose ){
	  *m_log << MSG::VERBOSE <<  " adding hit " << m_idHelperTool->toString(id) << "   barcode " << barcode;
	  if( barcode != barcodeIn ) *m_log << " hit barcode " << barcodeIn;
	  *m_log << endreq;
	}
      }
    }    
  }

  void MuonTrackTruthTool::addSimDataToTree( const CscSimDataCollection& simDataCol ) const {

    // loop over sim collection and check whether identifiers are on track
    CscSimDataCollection::const_iterator it = simDataCol.begin();
    CscSimDataCollection::const_iterator it_end = simDataCol.end();
    for( ;it!=it_end;++it ){

      Identifier id = it->first;

      // loop over deposits
      std::vector<CscSimData::Deposit>::const_iterator dit = it->second.getdeposits().begin();
      std::vector<CscSimData::Deposit>::const_iterator dit_end = it->second.getdeposits().end();
      for( ;dit!=dit_end;++dit ){

	int barcodeIn = manipulateBarCode(dit->first.barcode());
	std::map<int,int>::iterator bit = m_barcodeMap.find(barcodeIn);
	if( bit == m_barcodeMap.end() ){
	  if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding " << "  " << m_idHelperTool->toString(id) << "   barcode " << barcodeIn << endreq;
	  continue;
	}
	// replace barcode with barcode from map
	int barcode = bit->second;

	TruthTreeIt eit = m_truthTree.find(barcode);
	if( eit == m_truthTree.end() ){
	  if( m_verbose ) *m_log << MSG::VERBOSE <<  " discarding " << "  " << m_idHelperTool->toString(id) << "   barcode " << barcode << endreq;
	  continue;
	}else{

	  if( m_verbose ){
	    *m_log << MSG::VERBOSE <<  " adding hit " << m_idHelperTool->toString(id) << "   barcode " << barcode;
	    if( barcode != barcodeIn ) *m_log << " hit barcode " << barcodeIn;
	    *m_log << endreq;
	  }
	  if( m_idHelperTool->isCsc(id) ) eit->second.cscHits.insert(*it);
	  else{
	    *m_log << MSG::WARNING << " CscSimData object without CSC identifier " << m_idHelperTool->toString(id) << endreq;
	  }
	}	
      }
    }    
  }


  const TrackRecordCollection* MuonTrackTruthTool::getTruthTrackCollection() const {

    const TrackRecordCollection* truthCollection = 0;
    std::string location = "CosmicRecord";
    if ( m_storeGate->contains<TrackRecordCollection>(location) ) {
      if( m_storeGate->retrieve(truthCollection,location ).isSuccess() ){
	if( !truthCollection->empty() ) {
	  if( m_debug ) *m_log << MSG::VERBOSE << "TrackRecordCollection retrieved at location " << location << endreq;
	  return truthCollection;
	}
      }
      if( m_debug ) *m_log << MSG::VERBOSE << "location " << location << " discarded" << endreq;
    }

    location = "MuonEntryLayer";
    if ( m_storeGate->contains<TrackRecordCollection>(location) ) {
      if( m_storeGate->retrieve(truthCollection,location ).isSuccess() ){
	if( !truthCollection->empty() ) {
	  if( m_debug ) *m_log << MSG::VERBOSE << "TrackRecordCollection retrieved at location " << location << endreq;
	  return truthCollection;
	}
      }
      if( m_debug ) *m_log << MSG::VERBOSE << "location " << location << " discarded" << endreq;
    }

    location = "MuonEntryLayerFilter";
    if ( m_storeGate->contains<TrackRecordCollection>(location) ) {
      if( m_storeGate->retrieve(truthCollection,location ).isSuccess() ){
	if( !truthCollection->empty() ) {
	  if( m_debug ) *m_log << MSG::VERBOSE << "TrackRecordCollection retrieved at location " << location << endreq;
	  return truthCollection;
	}
      }
      if( m_debug ) *m_log << MSG::VERBOSE << "location " << location << " discarded" << endreq;
    }

    location ="MuonEntryRecord";
    if ( m_storeGate->contains<TrackRecordCollection>(location) ) {
      if( m_storeGate->retrieve(truthCollection,location ).isFailure() ){
	if( !truthCollection->empty() ) {
	  if( m_debug ) *m_log << MSG::VERBOSE << "TrackRecordCollection retrieved at location " << location << endreq;
	  return truthCollection;
	}
      }
      if( m_debug ) *m_log << MSG::VERBOSE << "location " << location << " discarded" << endreq;
    }

    location ="MuonExitRecord";
    if ( m_storeGate->contains<TrackRecordCollection>(location) ) {
      if( m_storeGate->retrieve(truthCollection,location ).isFailure() ){
	if( !truthCollection->empty() ) {
	  if( m_debug ) *m_log << MSG::VERBOSE << "TrackRecordCollection retrieved at location " << location << endreq;
	  return truthCollection;
	}
      }
      if( m_debug ) *m_log << MSG::VERBOSE << "location " << location << " discarded" << endreq;
    }

    return truthCollection;
  }
  
  const MuonSimDataCollection* MuonTrackTruthTool::retrieveTruthCollection( std::string colName ) const {
    // Retrieve SDO map for this event
    if(!m_storeGate->contains<MuonSimDataCollection>(colName)) return 0;

    const MuonSimDataCollection* truthCol(0);
    if(!m_storeGate->retrieve(truthCol, colName).isSuccess()) {
      if( m_debug ) *m_log << MSG::VERBOSE << "Could NOT find the MuonSimDataMap map key = "<< colName << endreq;
    }	else {
      if( m_debug ) *m_log << MSG::VERBOSE << "Retrieved MuonSimDataCollection for key = " << colName << endreq;
    }
    return truthCol;
  }

  const CscSimDataCollection* MuonTrackTruthTool::retrieveCscTruthCollection( std::string colName ) const {
        // Retrieve SDO map for this event
    if(!m_storeGate->contains<CscSimDataCollection>(colName)) return 0;
    const CscSimDataCollection* truthCol(0);
    if(!m_storeGate->retrieve(truthCol, colName).isSuccess()) {
      if( m_debug ) *m_log << MSG::VERBOSE << "Could NOT find the CscSimDataMap map key = "<< colName << endreq;
    }	else {
      if( m_debug ) *m_log << MSG::VERBOSE << "Retrieved CscSimDataCollection for key = " << colName << endreq;
    }
    return truthCol;
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
	  if( !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) ) *m_log << MSG::WARNING << " Could not get rot from measurement " << endreq;
	  continue;
	}
	Identifier id = rot->identify();
	if( !id.is_valid() || !m_idHelperTool->mdtIdHelper().is_muon(id) ) continue;
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
      if( m_debug ) *m_log << MSG::DEBUG << " performed truth match for particle with barcode: " << tit->first << " overlap " << nmatchedHits 
			   << " fraction  "<< (double)nmatchedHits/(double)nhits<< endreq;
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
	if( !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas) ) *m_log << MSG::WARNING << " Could not get rot from measurement " << endreq;
	continue;
      }
      Identifier id = rot->identify();
      if( !id.is_valid() || !m_idHelperTool->mdtIdHelper().is_muon(id) ) continue;

            
      if( m_idHelperTool->isMdt(id) ){
	addMdtTruth( trackTruth.mdts, id, *meas, truthEntry.mdtHits );
      }else if( m_idHelperTool->isCsc(id) ){
	addClusterTruth( trackTruth.cscs, id, *meas, truthEntry.cscHits );
      }else if( m_idHelperTool->isRpc(id) ){
	addClusterTruth( trackTruth.rpcs, id, *meas, truthEntry.rpcHits );
      }else if( m_idHelperTool->isTgc(id) ){
	addClusterTruth( trackTruth.tgcs, id, *meas, truthEntry.tgcHits );
      }else if( m_idHelperTool->issTgc(id) ){
	addClusterTruth( trackTruth.stgcs, id, *meas, truthEntry.stgcHits );
      }else if( m_idHelperTool->isMM(id) ){
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
      if( m_idHelperTool->isTrigger(id) || m_idHelperTool->isCsc(id) ) id = m_idHelperTool->layerId(id);

      int isOnTrack = ids.count(id);
      if( isOnTrack ) continue;

      // if restricted truth mode, skip if chamber has not hits
      Identifier chid = m_idHelperTool->chamberId(id);
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
      
      Identifier id = m_idHelperTool->layerId(it->first);

      int isOnTrack = ids.count(id);
      if( isOnTrack ) continue;

      // if restricted truth mode, skip if chamber has not hits
      Identifier chid = m_idHelperTool->chamberId(id);
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
      *m_log << MSG::WARNING << " dynamic_cast to MdtDriftCircleOnTrack failed for measurement with id " << m_idHelperTool->toString(id) << endreq;
      return;
    }
    
    Identifier chid = m_idHelperTool->chamberId(id);

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
    
    Identifier layid = m_idHelperTool->layerId(id);
    Identifier chid = m_idHelperTool->chamberId(id);

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
      // find SimData corresponding to identifier
      it = simCol.find(id);
      if( it != simCol.end() ) goodCluster = true;
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

    Identifier layid = m_idHelperTool->layerId(id);
    Identifier chid = m_idHelperTool->chamberId(id);
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

  const HepMC::GenParticle& MuonTrackTruthTool::getMother( const TruthTrajectory& traj, const int barcodeIn ) const {
    std::vector<HepMcParticleLink>::const_iterator pit = traj.begin();
    std::vector<HepMcParticleLink>::const_iterator pit_end = traj.end();
    ATH_MSG_DEBUG( "getMother() : size = " << traj.size() );
    const HepMC::GenParticle* theMother = 0;
    int pdgFinal = (*pit)->pdg_id();
    bool foundBC = false;
    for( ;pit!=pit_end;++pit ){
      if ((*pit)->barcode()==barcodeIn || foundBC){
        foundBC = true;
        ATH_MSG_DEBUG( "getMother() : pdg = " << (*pit)->pdg_id() << " barcode = " << (*pit)->barcode () );
        if( (*pit)->pdg_id() != pdgFinal ) { // the first case a track had a different flavour
          theMother = *pit;
          break;
        }
      }
    }
    
    // sanity check
    if (theMother && theMother->pdg_id()==pdgFinal) ATH_MSG_ERROR( "Wrong pdgId association in getMother() " );
    return *theMother;
  }

  const HepMC::GenParticle& MuonTrackTruthTool::getAncestor( const TruthTrajectory& traj, const int barcodeIn ) const {
    std::vector<HepMcParticleLink>::const_iterator pit = traj.begin();
    std::vector<HepMcParticleLink>::const_iterator pit_end = traj.end();
    const HepMC::GenParticle* theAncestor = 0;
    bool foundBC = false;
    for( ;pit!=pit_end;++pit ){
      if ((*pit)->barcode()==barcodeIn || foundBC){
        foundBC = true;
        if( (*pit)->status() > 1 ) {//first non final state particle
          theAncestor = *pit;
          break;
        }
      }
    }
    return *theAncestor;
  }

  const std::pair<const HepMC::GenParticle*, unsigned int> MuonTrackTruthTool::getInitialPair( const TruthTrajectory& traj, const int barcodeIn ) const {
    std::pair<const HepMC::GenParticle*,unsigned int> thePair;
    unsigned int scat = 0;
    ATH_MSG_DEBUG( "getFirst() : size = " << traj.size() );
    std::vector<HepMcParticleLink>::const_iterator pit = traj.begin();
    std::vector<HepMcParticleLink>::const_iterator pit_end = traj.end();
    const HepMC::GenParticle* theFirst = 0;
    bool foundBC = false;
    int pdgFinal = 0;
    double ePrev = 0.;
    for( ;pit!=pit_end;++pit ){
      if ((*pit)->barcode()==barcodeIn || foundBC){
        if (!foundBC){
          foundBC = true;
          theFirst = *pit;
          pdgFinal = (*pit)->pdg_id();
        } else {
          if( (*pit)->pdg_id() == pdgFinal ) { 
            if ( (theFirst != (*pit)) && ((*pit)->momentum().t()!=ePrev) ) ++scat; // if the particle has not changed pdgid after the first step count as scatter. also avoid counting pure interface changes as scatter
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

  const HepMC::GenParticle& MuonTrackTruthTool::getInitial( const TruthTrajectory& traj, const int barcodeIn ) const {
    return *((getInitialPair( traj, barcodeIn )).first);
  }

  unsigned int MuonTrackTruthTool::getNumberOfScatters( const TruthTrajectory& traj, const int barcodeIn ) const {
    return (getInitialPair( traj, barcodeIn )).second;
  }
}
/// eof

