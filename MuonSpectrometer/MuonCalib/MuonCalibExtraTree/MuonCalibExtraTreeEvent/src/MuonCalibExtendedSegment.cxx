/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <sstream>

namespace MuonCalib {

  MuonCalibExtendedSegment::MuonCalibExtendedSegment( const MuonCalibSegment& segment ) : MuonCalibSegment(segment) {

     MuonFixedIdManipulator idManip;

    m_summary.nhits  = 0;
    m_summary.nscatters = 0; 
    m_summary.nmdtHits = 0;
    m_summary.nrpcPhiHits = 0;
    m_summary.nrpcEtaHits = 0;
    m_summary.ntgcPhiHits = 0;
    m_summary.ntgcEtaHits = 0;
    m_summary.ncscPhiHits = 0;
    m_summary.ncscEtaHits = 0;

    MuonCalibTrackSummary::ChamberHitSummary* currentChamberSummary = 0;


    MuonCalibSegment::MdtHitCit mdtIt = segment.mdtHOTBegin();
    MuonCalibSegment::MdtHitCit mdtIt_end = segment.mdtHOTEnd();
    for( ;mdtIt!=mdtIt_end;++mdtIt ){
      const MdtCalibHitBase& hit = **mdtIt;
      MuonFixedId id = hit.identify();
      if( !id.isValid() ) {
        MsgStream log(Athena::getMessageSvc(),"MuonCalibExtendedSegment");
        log<<MSG::WARNING<<"invalid MDT id!"<<endmsg;
        continue;
      }

      m_idSet.insert(id);

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

      ++proj.nhits;

      ++m_summary.nhits;
      ++m_summary.nmdtHits;

      if( hit.adcCount() < 70 ) ++m_summary.nmdtHitsBelowADCCut;

      MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
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
    }
    
    MuonCalibSegment::RpcHitCit rpcIt = segment.rpcHOTBegin();
    MuonCalibSegment::RpcHitCit rpcIt_end = segment.rpcHOTEnd();
    for( ;rpcIt!=rpcIt_end;++rpcIt ){
      const RpcCalibHitBase& hit = **rpcIt;
      MuonFixedId id = hit.identify();
      if( !id.isValid() ) {
        MsgStream log(Athena::getMessageSvc(),"MuonCalibExtendedSegment");
        log<<MSG::WARNING<<"invalid RPC id!"<<endmsg;
        continue;
      }

      m_idSet.insert(id);

      MuonFixedId chId = idManip.chamberIdentifier(id);

      bool measuresPhi = idManip.measuresPhi(id);
      bool isMdt       = id.is_mdt();
      bool isFirst     = isMdt ? id.mdtMultilayer()==1 : measuresPhi;

      // check whether first chamber or new chamber
      if( !currentChamberSummary || currentChamberSummary->chId != chId ){
	m_summary.chamberHitSummary.push_back( MuonCalibTrackSummary::ChamberHitSummary(chId) );
	currentChamberSummary = &m_summary.chamberHitSummary.back();
      }

      MuonCalibTrackSummary::ChamberHitSummary::Projection& proj = 
	isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

      ++proj.nhits;

      ++m_summary.nhits;
      if( measuresPhi ) ++m_summary.nrpcPhiHits;
      else              ++m_summary.nrpcEtaHits;

      MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
      if( measuresPhi ) m_summary.phiStationLayers.insert(phiStationIndex);
      if( currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0 ){
	m_summary.phiEtaStationLayers.insert(phiStationIndex);
      }
    }

    if( m_summary.nrpcEtaHits + m_summary.nrpcPhiHits > 0 ) {
      m_summary.hasBarrel = true;
      m_summary.hasBarrelLayersWithRPC = true;
    }


    MuonCalibSegment::TgcHitCit tgcIt = segment.tgcHOTBegin();
    MuonCalibSegment::TgcHitCit tgcIt_end = segment.tgcHOTEnd();
    for( ;tgcIt!=tgcIt_end;++tgcIt ){
      const TgcCalibHitBase& hit = **tgcIt;
      MuonFixedId id = hit.identify();
      if( !id.isValid() ) {
        MsgStream log(Athena::getMessageSvc(),"MuonCalibExtendedSegment");
        log<<MSG::WARNING<<"invalid TGC id!"<<endmsg;
        continue;
      }

      m_idSet.insert(id);

      MuonFixedId chId = idManip.chamberIdentifier(id);

      bool measuresPhi = idManip.measuresPhi(id);
      bool isMdt       = id.is_mdt();
      bool isFirst     = isMdt ? id.mdtMultilayer()==1 : measuresPhi;

      // check whether first chamber or new chamber
      if( !currentChamberSummary || currentChamberSummary->chId != chId ){
	m_summary.chamberHitSummary.push_back( MuonCalibTrackSummary::ChamberHitSummary(chId) );
	currentChamberSummary = &m_summary.chamberHitSummary.back();
      }

      MuonCalibTrackSummary::ChamberHitSummary::Projection& proj = 
	isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

      ++proj.nhits;

      ++m_summary.nhits;
      if( measuresPhi ) ++m_summary.ntgcPhiHits;
      else              ++m_summary.ntgcEtaHits;

      if( id.eta() < 0 ) m_summary.hasEndcapA = true;
      else               m_summary.hasEndcapC = true;

      
      MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
      if( measuresPhi ) m_summary.phiStationLayers.insert(phiStationIndex);
      if( currentChamberSummary->netaHits() > 0 && currentChamberSummary->nphiHits() > 0 ){
	m_summary.phiEtaStationLayers.insert(phiStationIndex);
      }
    }


    MuonCalibSegment::CscHitCit cscIt = segment.cscHOTBegin();
    MuonCalibSegment::CscHitCit cscIt_end = segment.cscHOTEnd();
    for( ;cscIt!=cscIt_end;++cscIt ){
      const CscCalibHitBase& hit = **cscIt;
      MuonFixedId id = hit.identify();
      if( !id.isValid() ) {
        MsgStream log(Athena::getMessageSvc(),"MuonCalibExtendedSegment");
        log<<MSG::WARNING<<"invalid CSC id!"<<endmsg;
        continue;
      }

      m_idSet.insert(id);

      MuonFixedId chId = idManip.chamberIdentifier(id);

      bool measuresPhi = idManip.measuresPhi(id);
      bool isMdt       = id.is_mdt();
      bool isFirst     = isMdt ? id.mdtMultilayer()==1 : measuresPhi;

      // check whether first chamber or new chamber
      if( !currentChamberSummary || currentChamberSummary->chId != chId ){
	m_summary.chamberHitSummary.push_back( MuonCalibTrackSummary::ChamberHitSummary(chId) );
	currentChamberSummary = &m_summary.chamberHitSummary.back();
      }

      MuonCalibTrackSummary::ChamberHitSummary::Projection& proj = 
	isFirst ? currentChamberSummary->etaProjection() : currentChamberSummary->phiProjection();

      ++proj.nhits;

      ++m_summary.nhits;
      if( measuresPhi ) ++m_summary.ncscPhiHits;
      else              ++m_summary.ncscEtaHits;

      MuonFixedIdManipulator::PhiStationIndex phiStationIndex = idManip.phiStationLayerIndex(id);
      if( measuresPhi ){
	m_summary.phiStationLayers.insert(phiStationIndex);
      }
      MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
      m_summary.precisionStationLayers.insert(stationIndex);
      if( currentChamberSummary->netaHits() > 2 && currentChamberSummary->nphiHits() > 2 ){
	m_summary.goodPrecisionStationLayers.insert(stationIndex);
	m_summary.phiEtaStationLayers.insert(phiStationIndex);
      }

    }
  }

  std::string MuonCalibExtendedSegment::dump() const{
    std::ostringstream sout;
    sout << dumpPars() << std::endl
	 << dumpSummary();
    return sout.str();
  }

  std::string MuonCalibExtendedSegment::dumpPars() const{
    std::ostringstream sout;
    sout << "Extended Segment: " << std::setprecision(4) << "chi2 " << chi2()
	 << " r " << (int)globalPosition().perp()
	 << " z " << (int)globalPosition().z() << std::setprecision(5) 
	 << " phi " << globalDirection().phi() << " theta " << globalDirection().theta() << " nassociatedTracks " << m_associatedTracks.size();
    return sout.str();
  }

  std::string MuonCalibExtendedSegment::dumpSummary() const{
    std::ostringstream sout;
    sout << m_summary.dump();
    return sout.str();
  }

}
