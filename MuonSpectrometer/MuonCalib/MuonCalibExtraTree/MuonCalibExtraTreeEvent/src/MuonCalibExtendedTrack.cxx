/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"
#include "MuonCalibExtraUtils/MuonCalibSimpleHoleSearch.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include <sstream>

namespace MuonCalib {

  MuonCalibExtendedTrack::MuonCalibExtendedTrack( const MuonCalibTrack_E& track , int pdgCode, int barCode) : MuonCalibTrack_E(track), m_pdgCode(pdgCode), m_barCode(barCode) {

    m_pos = Amg::Vector3D(x0(),y0(),z0());
    m_dir = Amg::Vector3D(0.,0.,1.);
    Amg::setTheta(m_dir, theta());
    Amg::setPhi(m_dir, phi());

    MuonFixedIdManipulator idManip;
    // MuonFixedIdPrinter     printer;

    m_summary.nhits  = 0;
    m_summary.nscatters = 0; 
    m_summary.nmdtHits = 0;
    m_summary.nrpcPhiHits = 0;
    m_summary.nrpcEtaHits = 0;
    m_summary.ntgcPhiHits = 0;
    m_summary.ntgcEtaHits = 0;
    m_summary.ncscPhiHits = 0;
    m_summary.ncscEtaHits = 0;
    m_summary.isTrack = true;

    MuonCalibTrackSummary::ChamberHitSummary* currentChamberSummary = 0;

    MuonCalibTrack_E::HitVector::const_iterator it_hit = track.beginHit();
    MuonCalibTrack_E::HitVector::const_iterator it_hit_end = track.endHit();
    for (;it_hit != it_hit_end; ++it_hit) {

      int type = (*it_hit)->type();
      // count scatters 
      if ( type > 10 ) {
	++m_summary.nscatters;
	continue;
      }

      MuonFixedId id = (*it_hit)->identify();
      if( !id.isValid() ) {
	if( type == 3 ) ++m_summary.npseudo;
	else{
	  // these are the ID hits
	  if((*it_hit)->position().perp() < 200.) {
	    ++m_summary.npixel;
	  } else if ((*it_hit)->position().perp() < 540.) {
	    ++m_summary.nsct;
	  } else if ((*it_hit)->position().perp() < 1100.) {
	    if(fabs((*it_hit)->position().z()) < 750.) ++m_summary.ntrtBarrel;
	    else                                       ++m_summary.ntrtEndcap;
	  }
	}
	continue;
      }
      
      //else std::cout << " WARNING duplicate hit on track found " << printer.fullIdentifier(id) << std::endl;

      MuonFixedId chId = idManip.chamberIdentifier(id);

      bool measuresPhi = idManip.measuresPhi(id);
      bool isMdt       = id.is_mdt();
      bool isFirst     = isMdt ? id.mdtMultilayer()==1 : !measuresPhi;


      // check whether first chamber or new chamber
      if( !currentChamberSummary || currentChamberSummary->chId != chId ){
	m_summary.chamberHitSummary.push_back( MuonCalibTrackSummary::ChamberHitSummary(chId) );
	currentChamberSummary = &m_summary.chamberHitSummary.back();
      }

      MuonCalibTrackSummary::ChamberHitSummary::Projection& proj = 
	isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

      MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
      MuonFixedId chamberId = idManip.chamberIdentifier(id);


      if( type == 6 ){
	++m_summary.nholes;
	++proj.nholes;
      }else if( type == 4 ){
	++m_summary.noutliers;
	++proj.noutliers;
      }else if( type == 5 ){
	++m_summary.noutliers;
	++proj.ndeltas;
      }else{

	IdHitMap::iterator hitPos = m_hitIdMap.find(id);
	if( hitPos == m_hitIdMap.end() ) m_hitIdMap[id] = *it_hit;
	m_hitsPerChamber[chamberId].push_back(*it_hit);
	
	++proj.nhits;
	// hits counts
	++m_summary.nhits;
	if( (**it_hit).position().y() < 0. ) ++m_summary.nhitsLowerHemisphere;
	else                                 ++m_summary.nhitsUpperHemisphere;

	if(id.is_mdt()){
	  ++m_summary.nmdtHits;
	  m_mdtHitsPerStationIndex[stationIndex].push_back(*it_hit);
	  if( idManip.isEndcap(id) ){
	    if( id.eta() < 0. ) m_summary.hasEndcapA = true;
	    else                m_summary.hasEndcapC = true;
	    if( stationIndex == MuonFixedIdManipulator::EIA || stationIndex == MuonFixedIdManipulator::EIC ||
		stationIndex == MuonFixedIdManipulator::EMA || stationIndex == MuonFixedIdManipulator::EMC ) m_summary.hasEndcapLayersWithTGC = true;
	  }else{
	    m_summary.hasBarrel = true;
	    if( stationIndex == MuonFixedIdManipulator::BM || stationIndex == MuonFixedIdManipulator::BO ) m_summary.hasBarrelLayersWithRPC = true;
	  }

	  m_summary.precisionStationLayers.insert(stationIndex);
	  if( currentChamberSummary->nMdtHitsMl1() > 0 && currentChamberSummary->nMdtHitsMl2() && currentChamberSummary->nhits() > 3 ){
	    m_summary.goodPrecisionStationLayers.insert(stationIndex);
	  }
	}else if (id.is_rpc()) {
	  if( id.rpcMeasuresPhi() ){
	    ++m_summary.nrpcPhiHits;
	  }else{
	    ++m_summary.nrpcEtaHits;
	  }
	  m_rpcHitsPerStationIndex[stationIndex].push_back(*it_hit);
	  m_summary.hasBarrel = true;
	  
	  MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
	  if( measuresPhi ) m_summary.phiStationLayers.insert(phiStationIndex);
	  if( currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0 ){
	    m_summary.phiEtaStationLayers.insert(phiStationIndex);
	  }
	}else if (id.is_tgc()) {
	  if( id.tgcIsStrip() )     ++m_summary.ntgcPhiHits;
	  else                      ++m_summary.ntgcEtaHits;
	  m_tgcHitsPerStationIndex[stationIndex].push_back(*it_hit);

	  if( id.eta() < 0. ) m_summary.hasEndcapA = true;
	  else                m_summary.hasEndcapC = true;
	  m_summary.hasEndcapLayersWithTGC = true;

	  MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
	  if( measuresPhi ) m_summary.phiStationLayers.insert(phiStationIndex);
	  if( currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0 ){
	    m_summary.phiEtaStationLayers.insert(phiStationIndex);
	  }
	}else if (id.is_csc()) {
	  if( id.cscMeasuresPhi() ) ++m_summary.ncscPhiHits;
	  else                      ++m_summary.ncscEtaHits;
	  m_cscHitsPerStationIndex[stationIndex].push_back(*it_hit);
	  if( id.eta() < 0. ) m_summary.hasEndcapA = true;
	  else                m_summary.hasEndcapC = true;

	  MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
	  if( measuresPhi ){
	    m_summary.phiStationLayers.insert(phiStationIndex);
	  }
	  m_summary.precisionStationLayers.insert(stationIndex);
	  if( currentChamberSummary->netaHits() > 2 && currentChamberSummary->nphiHits() > 2 ){
	    m_summary.goodPrecisionStationLayers.insert(stationIndex);
	    m_summary.phiEtaStationLayers.insert(phiStationIndex);
	  }
	}  
      }
    }
    
    const StationIndexHitsMap& mdtStHitMap = mdtStationIndexHitsMap();
    const StationIndexHitsMap& rpcStHitMap = rpcStationIndexHitsMap();
    const StationIndexHitsMap& tgcStHitMap = tgcStationIndexHitsMap();
    const StationIndexHitsMap& cscStHitMap = cscStationIndexHitsMap();

    MuonCalibSimpleHoleSearch holeSearch;
    double tolerance = 100.;
    MuonCalibSimpleHoleSearch::ResultVec intersectedLayers = holeSearch.intersectsWithGeometry(position(),direction(),tolerance);
    MuonCalibSimpleHoleSearch::ResultIt rit = intersectedLayers.begin();
    MuonCalibSimpleHoleSearch::ResultIt rit_end = intersectedLayers.end();
    for( ;rit!=rit_end;++rit ) {
      m_intersectedLayers[rit->stationLayerId] = *rit;
      const MuonFixedId& stId = rit->stationLayerId;
      MuonFixedIdManipulator::StationIndex stIndex = idManip.stationLayerIndex(stId);
      bool isEndcap = idManip.isEndcap(stId);

      StationIndexHitsMap::const_iterator mdtIt = mdtStHitMap.find(stIndex); 
      unsigned int nmdtHits = mdtIt != mdtStHitMap.end() ? mdtIt->second.size() : 0;

      StationIndexHitsMap::const_iterator rpcIt = rpcStHitMap.find(stIndex); 
      unsigned int nrpcHits = rpcIt != rpcStHitMap.end() ? rpcIt->second.size() : 0;

      StationIndexHitsMap::const_iterator tgcIt = tgcStHitMap.find(stIndex); 
      unsigned int ntgcHits = tgcIt != tgcStHitMap.end() ? tgcIt->second.size() : 0;

      StationIndexHitsMap::const_iterator cscIt = cscStHitMap.find(stIndex); 
      unsigned int ncscHits = cscIt != cscStHitMap.end() ? cscIt->second.size() : 0;

      // flag intersected layers without any hits
      unsigned int ntrig = isEndcap ? ntgcHits : nrpcHits;
      unsigned int nprec = nmdtHits + ncscHits;
      if( nprec + ntrig == 0 ) m_intersectedLayersWithoutHits.insert(stIndex);

      // flag layers where we would expect RPC hits but they are not on the track
      if( !isEndcap && stIndex != MuonFixedIdManipulator::BI && 
	  stIndex != MuonFixedIdManipulator::BE && nrpcHits == 0 ) m_intersectedRpcLayerWithoutHits.insert(stIndex);

      // flag layers where we would expect TGC hits but they are not on the track
      if( isEndcap && stIndex != MuonFixedIdManipulator::EIA && 
	  stIndex != MuonFixedIdManipulator::EIC && ntgcHits == 0 ) m_intersectedTgcLayerWithoutHits.insert(stIndex);

    }
  }
  
  std::string MuonCalibExtendedTrack::dump() const{
    std::ostringstream sout;
    sout << dumpPars() << std::endl
	 << dumpSummary()
	 << dumpIntersects() << std::endl;
    return sout.str();
  }

  std::string MuonCalibExtendedTrack::dumpPars() const{
    std::ostringstream sout;
    double sign = qOverP() < 0. ? -1 : 1;
    sout << "Author " << std::setw(4) << author() << std::setprecision(4) << " chi2 " << chi2() << " ndof " << ndof()
	 << " r " << (int)position().perp()
	 << " z " << (int)z0() << std::setprecision(5) 
	 << " phi " << phi() << " theta " << theta() 
	 << std::setw(6) << " q*mom " << (int)p()*sign
	 << " pt " << std::setw(5) << (int)pt() << " association: segments " << m_associatedSegments.size() << " tracks " << m_associatedTracks.size();
    if( author() == 0 ){
      const MuonCalibExtendedTrack* ipTrack = getAssociatedTrack(1);
      if( ipTrack ) {
	sout << std::endl << "  at ip "
	     << " r " << (int)ipTrack->position().perp()
	     << " z " << (int)ipTrack->z0() << std::setprecision(5) 
	     << " phi " << ipTrack->phi() << " theta " << ipTrack->theta() 
	     << std::setw(6) << " q*mom " << (int)ipTrack->p()*sign
	     << " pt " << std::setw(5) << (int)ipTrack->pt();
      }
    }
    return sout.str();
  }

  std::string MuonCalibExtendedTrack::dumpSummary() const{ 
    std::ostringstream sout;
    sout << " " << m_summary.dump();
    return sout.str();
  }

  std::string MuonCalibExtendedTrack::dumpIntersects() const {
    MuonFixedIdPrinter printer;
    MuonFixedIdManipulator idManip;

    const StationIndexHitsMap& mdtStHitMap = mdtStationIndexHitsMap();
    const StationIndexHitsMap& rpcStHitMap = rpcStationIndexHitsMap();
    const StationIndexHitsMap& tgcStHitMap = tgcStationIndexHitsMap();
    const StationIndexHitsMap& cscStHitMap = cscStationIndexHitsMap();

    std::ostringstream sout;
    sout << " Intersected layers: " << m_intersectedLayers.size() 
	 << " without: any hits " << m_intersectedLayersWithoutHits.size()
	 << "  rpc hits " << m_intersectedRpcLayerWithoutHits.size() 
	 << "  tgc hits " << m_intersectedTgcLayerWithoutHits.size() << std::endl;
    unsigned int nentries = m_intersectedLayers.size();
    unsigned int currentEntry = 0;
    StationIntersectedLayerMap::const_iterator it = m_intersectedLayers.begin();
    StationIntersectedLayerMap::const_iterator it_end = m_intersectedLayers.end();
    sout.setf(std::ios::left);
    for( ;it!=it_end;++it ) {

      const MuonFixedId& stId = it->first;
      MuonFixedIdManipulator::StationIndex stIndex = idManip.stationLayerIndex(stId);
      bool isEndcap = idManip.isEndcap(stId);

      StationIndexHitsMap::const_iterator mdtIt = mdtStHitMap.find(stIndex); 
      unsigned int nmdtHits = mdtIt != mdtStHitMap.end() ? mdtIt->second.size() : 0;

      StationIndexHitsMap::const_iterator rpcIt = rpcStHitMap.find(stIndex); 
      unsigned int nrpcHits = rpcIt != rpcStHitMap.end() ? rpcIt->second.size() : 0;

      StationIndexHitsMap::const_iterator tgcIt = tgcStHitMap.find(stIndex); 
      unsigned int ntgcHits = tgcIt != tgcStHitMap.end() ? tgcIt->second.size() : 0;

      StationIndexHitsMap::const_iterator cscIt = cscStHitMap.find(stIndex); 
      unsigned int ncscHits = cscIt != cscStHitMap.end() ? cscIt->second.size() : 0;
      
      unsigned int ntrig = isEndcap ? ntgcHits : nrpcHits;
      unsigned int nprec = nmdtHits + ncscHits;

      sout << "    " << std::setw(5) << printer.stationLayerIdentifier(stId);
      if( nprec + ntrig != 0 ) sout << " presicion hits " << std::setw(3) << nprec << " trigger hits " << std::setw(3) << ntrig;
      else                     sout << std::setw(36) << " no hits in layer";
      sout << "  intersect position " << it->second.intersectPosition;
      
      // increase count before check to allow equals check
      ++currentEntry;
      if( currentEntry != nentries ) sout << std::endl;
    }
    return sout.str();
  }

  MuonCalibExtendedTrackOverlap MuonCalibExtendedTrack::calculateHitOverlap( const MuonCalibExtendedTrack& track ) const {
    
    const IdHitsMap& chHitMap = track.hitsPerChamberMap();
    MuonFixedIdManipulator manip;
    MuonFixedIdPrinter printer;

    MuonCalibExtendedTrackOverlap overlap;

    std::set<MuonFixedId> sharedChambers;

    // loop over stations and check whether they are also present in other list
    IdHitsMap::const_iterator rit = m_hitsPerChamber.begin();
    IdHitsMap::const_iterator rit_end = m_hitsPerChamber.end();
    for( ;rit!=rit_end;++rit ){

      
      IdHitsMap::const_iterator pos = chHitMap.find(rit->first );
      if( pos != chHitMap.end() ) {
	// shared chamber add to list
	sharedChambers.insert(rit->first);

	std::cout << " shared chamber " << printer.fullIdentifier(rit->first) << std::endl;

	std::set<MuonFixedId> foundIds;
	std::set<MuonFixedId> sharedEtaLayers;
	std::set<MuonFixedId> sharedPhiLayers;
	std::set<MuonFixedId> firstEtaLayers;
	std::set<MuonFixedId> firstPhiLayers;
	std::set<MuonFixedId> secondEtaLayers;
	std::set<MuonFixedId> secondPhiLayers;
	unsigned int wrongSign = 0;
	std::vector<const MuonCalibHit_E*>::const_iterator it1 = rit->second.begin();
	std::vector<const MuonCalibHit_E*>::const_iterator it1_end = rit->second.end();
	for( ;it1!=it1_end;++it1){
	  const MuonFixedId& id = (*it1)->identify();
	  bool measuresPhi = manip.measuresPhi(id);
	  MuonFixedId layerId = manip.moduleIdentifier(id,true);
	  //std::cout << " hit  " << printer.fullIdentifier(id);

	  std::vector<const MuonCalibHit_E*>::const_iterator hit = std::find_if( pos->second.begin(),pos->second.end(),SameFixedId(id));
	  if( hit != pos->second.end() ){	   
	    //std::cout << " is Overlap " << std::endl;
	    if( id.is_mdt() ){
	      if( fabs((*it1)->driftRadius()) > 2. && fabs((*hit)->driftRadius()) > 2.&& 
		  (*it1)->driftRadius()*(*hit)->driftRadius() < 0. ) ++wrongSign;
	      else sharedEtaLayers.insert(id);
	      foundIds.insert(id);
	    }else{
	      if( measuresPhi ) sharedPhiLayers.insert(layerId);
	      else              sharedEtaLayers.insert(layerId);;
	      foundIds.insert(layerId);
	    }
	  }else{
	    if( !id.is_mdt() && foundIds.count(layerId) ) continue;
	    //std::cout << " only on this track " << std::endl;
	    if( measuresPhi ) firstPhiLayers.insert(layerId);
	    else              firstEtaLayers.insert(layerId);;
	  }
	}
	std::vector<const MuonCalibHit_E*>::const_iterator it2 = pos->second.begin();
	//std::vector<const MuonCalibHit_E*>::const_iterator it2_end = pos->second.end();
	for( ;it1!=it1_end;++it1){
	  MuonFixedId id = (*it2)->identify().is_mdt() ? (*it2)->identify() : manip.moduleIdentifier((*it2)->identify(),true);
	  if( !foundIds.count(id) ) {
	    //std::cout << " hit  only on second " << printer.fullIdentifier(id) << std::endl;
	    
	    bool measuresPhi = manip.measuresPhi(id);
	    if( measuresPhi ) secondPhiLayers.insert(id);
	    else              secondEtaLayers.insert(id);
	  }
	}
// 	std::cout << " chamber summary: shared " << sharedEtaLayers.size() << " " << sharedPhiLayers.size() << std::endl
// 		  << "              only first " << firstEtaLayers.size() << " " << firstPhiLayers.size() << std::endl
// 		  << "             only second " << secondEtaLayers.size() << " " << secondPhiLayers.size() << std::endl;
	if( rit->first.is_mdt() ){
	  overlap.mdt.shared += sharedEtaLayers.size();
	  overlap.mdt.first  += firstEtaLayers.size();
	  overlap.mdt.second += secondEtaLayers.size();
	}else if( rit->first.is_rpc() ){
	  overlap.rpcEta.shared += sharedEtaLayers.size();
	  overlap.rpcEta.first  += firstEtaLayers.size();
	  overlap.rpcEta.second += secondEtaLayers.size();
	  overlap.rpcPhi.shared += sharedPhiLayers.size();
	  overlap.rpcPhi.first  += firstPhiLayers.size();
	  overlap.rpcPhi.second += secondPhiLayers.size();	  
	}else if( rit->first.is_tgc() ){
	  overlap.tgcEta.shared += sharedEtaLayers.size();
	  overlap.tgcEta.first  += firstEtaLayers.size();
	  overlap.tgcEta.second += secondEtaLayers.size();
	  overlap.tgcPhi.shared += sharedPhiLayers.size();
	  overlap.tgcPhi.first  += firstPhiLayers.size();
	  overlap.tgcPhi.second += secondPhiLayers.size();	  
	}else if( rit->first.is_csc() ){
	  overlap.cscEta.shared += sharedEtaLayers.size();
	  overlap.cscEta.first  += firstEtaLayers.size();
	  overlap.cscEta.second += secondEtaLayers.size();
	  overlap.cscPhi.shared += sharedPhiLayers.size();
	  overlap.cscPhi.first  += firstPhiLayers.size();
	  overlap.cscPhi.second += secondPhiLayers.size();	  
	}
      }else{

	//std::cout << " only first chamber " << printer.fullIdentifier(rit->first) << std::endl;
	std::set<MuonFixedId> foundIds;
	std::vector<const MuonCalibHit_E*>::const_iterator it1 = rit->second.begin();
	std::vector<const MuonCalibHit_E*>::const_iterator it1_end = rit->second.end();
	for( ;it1!=it1_end;++it1){

	  if( rit->first.is_mdt() ){
	    ++overlap.mdt.first;
	  }else{
	    MuonFixedId id = manip.moduleIdentifier((*it1)->identify(),true);
	    if( foundIds.count(id) ) continue;
	    foundIds.insert(id);
	    //std::cout << " hit  only on first " << printer.fullIdentifier(id) << std::endl;
	    bool measuresPhi = manip.measuresPhi((*it1)->identify());
	    if( rit->first.is_rpc() ){
	      if( !measuresPhi) ++overlap.rpcEta.first;
	      else              ++overlap.rpcPhi.first;
	    }else if( rit->first.is_tgc() ){
	      if( !measuresPhi) ++overlap.tgcEta.first;
	      else              ++overlap.tgcPhi.first;
	    }else if( rit->first.is_csc() ){
	      if( !measuresPhi) ++overlap.cscEta.first;
	      else              ++overlap.cscPhi.first;
	    }
	  }
	}

      }
    }

    // loop over stations and check whether they are also present in other list
    rit = chHitMap.begin();
    rit_end = chHitMap.end();
    for( ;rit!=rit_end;++rit ){
      // skip already handled chambers
      if( sharedChambers.count(rit->first) ) continue;

      //std::cout << " only second chamber " << printer.fullIdentifier(rit->first) << std::endl;

      std::set<MuonFixedId> foundIds;
      std::vector<const MuonCalibHit_E*>::const_iterator it1 = rit->second.begin();
      std::vector<const MuonCalibHit_E*>::const_iterator it1_end = rit->second.end();
      for( ;it1!=it1_end;++it1){
	
	if( rit->first.is_mdt() ){
	  ++overlap.mdt.second;
	}else{
	  MuonFixedId id = manip.moduleIdentifier((*it1)->identify(),true);
	  if( foundIds.count(id) ) continue;
	  foundIds.insert(id);
	  //std::cout << " hit  only on second " << printer.fullIdentifier(id) << std::endl;
	  bool measuresPhi = manip.measuresPhi((*it1)->identify());
	  if( rit->first.is_rpc() ){
	    if( !measuresPhi) ++overlap.rpcEta.second;
	    else              ++overlap.rpcPhi.second;
	  }else if( rit->first.is_tgc() ){
	    if( !measuresPhi) ++overlap.tgcEta.second;
	    else              ++overlap.tgcPhi.second;
	  }else if( rit->first.is_csc() ){
	    if( !measuresPhi) ++overlap.cscEta.second;
	    else              ++overlap.cscPhi.second;
	  }
	}
      }
    }

//     std::cout << " First " << std::endl
// 	      << dumpSummary() 
// 	      << " Second " << std::endl
// 	      << track.dumpSummary() << std::endl
// 	      << "  overlap: precicion " << overlap.sharedPrecisionHits() << " frac " << overlap.overlapFracionPrecision() 
// 	      << " Trigger: eta " << overlap.sharedEtaTriggerHits()
// 	      << " phi " << overlap.sharedPhiHits() << std::endl
// 	      << "  only first: precicion " << overlap.firstPrecisionHits()
// 	      << " Trigger: eta " << overlap.firstEtaTriggerHits()
// 	      << " phi " << overlap.firstPhiHits() << std::endl
// 	      << "  only second: precicion " << overlap.secondPrecisionHits()
// 	      << " Trigger: eta " << overlap.secondEtaTriggerHits()
// 	      << " phi " << overlap.secondPhiHits() << std::endl;
    
    return overlap;
  }
}
