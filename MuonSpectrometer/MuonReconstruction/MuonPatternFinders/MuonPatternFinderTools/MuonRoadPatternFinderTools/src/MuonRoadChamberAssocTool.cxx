/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRoadChamberAssocTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "GaudiKernel/MsgStream.h"

using HepGeom::Point3D;

namespace Muon {

  MuonRoadChamberAssocTool::MuonRoadChamberAssocTool (const std::string& type, const std::string& name,
				      const IInterface* parent)
    : 
    AthAlgTool(type, name, parent) {
    declareInterface<IMuonRoadChamberAssocTool>(this);    
    declareProperty("MdtStoreGateKey",mdtkey = "MDT_DriftCircles");
    declareProperty("RpcStoreGateKey",rpckey = "RPC_Measurements");
    declareProperty("TgcStoreGateKey",tgckey = "TGC_Measurements");
    declareProperty("CscStoreGateKey",csckey = "CSC_Clusters");
  }
    
  StatusCode MuonRoadChamberAssocTool::initialize() {
    MsgStream mLog( msgSvc(), name() );
    m_PI = 3.14159265;
    
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      mLog << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc" << endreq;
      return sc;
    }
    StoreGateSvc* detStore = 0;
    sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
      mLog << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
    }   
    sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
    if(sc.isFailure()) {
      mLog << MSG::ERROR << "Can't retieve MdtIdHelper" << endreq;
      return StatusCode::FAILURE;
    }
    sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
    if (sc.isFailure()) {
      mLog << MSG::ERROR << "Can't retrieve RpcIdHelper" << endreq;
      return sc;
    }
    sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
    if (sc.isFailure()) {
      mLog << MSG::ERROR << "Can't retrieve TgcIdHelper" << endreq;
      return sc;
    }
    sc = detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
    if (sc.isFailure()) {
      mLog << MSG::ERROR << "Can't retrieve CscIdHelper" << endreq;
      return sc;
    }


    return StatusCode::SUCCESS;
  }
  
  MuonRoadChamberAssocTool::~MuonRoadChamberAssocTool() {
    
  }

  MuonPatternCombination* MuonRoadChamberAssocTool::execute(const Trk::TrackRoad road) {
										
    MsgStream mLog( msgSvc(), name() );
    std::vector<MuonPatternChamberIntersect> chambers;
    Amg::Vector3D tvertex(0,0,0);
    Amg::Vector3D roadmom(road.globalDirection());//"momentum" is unit vector in the road direction
    Trk::TrackParameters* trkpars = new Trk::Perigee(road.globalPosition(),roadmom,1,tvertex);
    
    double roadThetaMax = 2*atan(exp(-1*(road.globalPosition().eta() - road.deltaEta())));
    double roadThetaMin = 2*atan(exp(-1*(road.globalPosition().eta() + road.deltaEta())));
    double thetaWidth = fabs(road.globalPosition().theta() - roadThetaMin);
    if(thetaWidth < fabs(road.globalPosition().theta() - roadThetaMax)) thetaWidth = fabs(road.globalPosition().theta() - roadThetaMax);
    
    ////loop on chambers and select those inside the road
    double roadPhi = road.globalPosition().phi();
    double phiWidth = road.deltaPhi();
    
    //MDT chambers and hits
    const MdtPrepDataContainer* mdtPrds = 0;
    if(StatusCode::SUCCESS != m_storeGate->retrieve(mdtPrds, mdtkey)) {
      mLog << MSG::ERROR << " Cannot retrieve MDT PRD Container " << mdtkey << endreq;
      return 0;
    }
    MdtPrepDataContainer::const_iterator mdtit     = mdtPrds->begin();
    MdtPrepDataContainer::const_iterator mdtit_end = mdtPrds->end();
    for(; mdtit!=mdtit_end; ++mdtit) {
      if( (*mdtit)->size() == 0 ) continue;
      //get the detector element from the first PRD
      const MuonGM::MuonReadoutElement* detEl = (*mdtit)->at(0)->detectorElement();
      Amg::Vector3D chCenter = detEl->center();
      if(fabs(chCenter.theta() - road.globalPosition().theta()) > 1) continue;
      //get the chamber dimensions
      double Zsize = detEl->getZsize();
      double Ssize = detEl->getSsize();
      double Rsize = detEl->getRsize();
      //chamber ranges in theta
      double chThetaMax = atan2(chCenter.perp(),chCenter.z() - Zsize/2.); 
      double chThetaMin = atan2(chCenter.perp(),chCenter.z() + Zsize/2.); 
      //endcap chambers
      if(detEl->endcap()) {
	chThetaMax = atan2(chCenter.perp() - Rsize/2.,chCenter.z());
	chThetaMin = atan2(chCenter.perp() + Rsize/2.,chCenter.z());
      }
      //chamber phi range
      double deltaPhi = atan(Ssize/(2.*chCenter.perp()));
      double chPhiMax = chCenter.phi() + deltaPhi;
      double chPhiMin = chCenter.phi() - deltaPhi;

      //check if the chamber is in the road
      if( !( isInPhiBounds(chPhiMax,roadPhi,phiWidth) && isInPhiBounds(chPhiMin,roadPhi,phiWidth) ) ) continue;
      if( !( (chThetaMin > roadThetaMin && chThetaMin < roadThetaMax) ||
	     (chThetaMax > roadThetaMax && chThetaMax < roadThetaMax) ) ) continue;
      
      //if the chamber is in the road, associate hits to it
      std::vector< const Trk::PrepRawData* > rios;
      MdtPrepDataCollection::const_iterator it = (*mdtit)->begin();
      MdtPrepDataCollection::const_iterator it_end = (*mdtit)->end();
      for(; it != it_end; ++it) {
	if( (*it)->status() != 1) continue;
	double tubeTheta = (*it)->globalPosition().theta();
	if(tubeTheta > roadThetaMin && tubeTheta < roadThetaMax) rios.push_back( *it );	
      }
      
      Amg::Vector3D patpose;
      Amg::Vector3D patdire;
      m_muonHoughMathUtils.extrapolateCurvedRoad(road.globalPosition(),roadmom,chCenter,patpose,patdire);
      MuonPatternChamberIntersect chIntersect(patpose,patdire,rios);
      chambers.push_back(chIntersect);
    }//end loop on MDT chambers
    
    //RPCs
    const RpcPrepDataContainer* rpcPrds = 0;
    if(StatusCode::SUCCESS != m_storeGate->retrieve(rpcPrds,rpckey)) {
      mLog << MSG::ERROR << "Cannot retrieve the RPC PRD container " << rpckey << endreq;
      return 0;
    }
    RpcPrepDataContainer::const_iterator rpcit     = rpcPrds->begin();
    RpcPrepDataContainer::const_iterator rpcit_end = rpcPrds->end();
    for(; rpcit!=rpcit_end; ++rpcit) {
      if( (*rpcit)->size() == 0 ) continue;
      //get the detector element from the first PRD
      const MuonGM::MuonReadoutElement* detEl = (*rpcit)->at(0)->detectorElement();
      Amg::Vector3D chCenter = detEl->center();
      if(fabs(chCenter.theta() - road.globalPosition().theta()) > 1) continue;
      //get the chamber dimensions
      double Zsize = detEl->getZsize();
      double Ssize = detEl->getSsize();
      //double Rsize = detEl->getRsize();
      //chamber ranges in theta
      double chThetaMax = atan2(chCenter.perp(),chCenter.z() - Zsize/2.);
      double chThetaMin = atan2(chCenter.perp(),chCenter.z() + Zsize/2.);
      //chamber phi range
      double deltaPhi = atan(Ssize/(2.*chCenter.perp()));
      double chPhiMax = chCenter.phi() + deltaPhi;
      double chPhiMin = chCenter.phi() - deltaPhi;
      
      //check if the chamber is in the road
      if( !( isInPhiBounds(chPhiMax,roadPhi,phiWidth) && isInPhiBounds(chPhiMin,roadPhi,phiWidth) ) ) continue;
      if( !( (chThetaMin > roadThetaMin && chThetaMin < roadThetaMax) ||
	     (chThetaMax > roadThetaMax && chThetaMax < roadThetaMax) ) ) continue;
      
      //if the chamber is in the road, associate hits in road
      std::vector< const Trk::PrepRawData* > rios;
      RpcPrepDataCollection::const_iterator it = (*rpcit)->begin();
      RpcPrepDataCollection::const_iterator it_end = (*rpcit)->end();
      for(; it != it_end; ++it) {
	double rpcTheta = (*it)->globalPosition().theta();
	double rpcPhi = (*it)->globalPosition().phi();
	if( ( rpcTheta > roadThetaMin && rpcTheta < roadThetaMax ) && 
	    isInPhiBounds(rpcPhi,roadPhi,phiWidth) ) rios.push_back( *it );
      }
      
      Amg::Vector3D patpose;
      Amg::Vector3D patdire;
      m_muonHoughMathUtils.extrapolateCurvedRoad(road.globalPosition(),roadmom,chCenter,patpose,patdire);
      MuonPatternChamberIntersect chIntersect(patpose,patdire,rios);
      chambers.push_back(chIntersect);
      
    }//end loop on RPC chambers

    //TGCs
    const TgcPrepDataContainer* tgcPrds = 0;
    if(StatusCode::SUCCESS != m_storeGate->retrieve(tgcPrds,tgckey)) {
      mLog << MSG::ERROR << "Cannot retrieve the TGC PRD container " << tgckey << endreq;
      return 0;
    }
    TgcPrepDataContainer::const_iterator tgcit     = tgcPrds->begin();
    TgcPrepDataContainer::const_iterator tgcit_end = tgcPrds->end();
    for(; tgcit!=tgcit_end; ++tgcit) {
      if( (*tgcit)->size() == 0 ) continue;
      //get the detector element from the first PRD
      const MuonGM::MuonReadoutElement* detEl = (*tgcit)->at(0)->detectorElement();
      Amg::Vector3D chCenter = detEl->center();
      if(fabs(chCenter.theta() - road.globalPosition().theta()) > 1) continue;
      //get the chamber dimensions
      //double Zsize = detEl->getZsize();
      double Ssize = detEl->getSsize();
      double Rsize = detEl->getRsize();
      //chamber range in theta
      double chThetaMax = atan2(chCenter.perp() - Rsize/2.,chCenter.z());
      double chThetaMin = atan2(chCenter.perp() + Rsize/2.,chCenter.z());
      //chamber phi ranges
      double deltaPhi = atan(Ssize/(2.*chCenter.perp()));
      double chPhiMax = chCenter.phi() + deltaPhi;
      double chPhiMin = chCenter.phi() - deltaPhi;
      
      //check if the chamber is in the road
      if( !( isInPhiBounds(chPhiMax,roadPhi,phiWidth) && isInPhiBounds(chPhiMin,roadPhi,phiWidth) ) ) continue;
      if( !( (chThetaMin > roadThetaMin && chThetaMin < roadThetaMax) ||
	     (chThetaMax > roadThetaMax && chThetaMax < roadThetaMax) ) ) continue;
      
      //if the chamber is in the road, associate hits to it
      std::vector< const Trk::PrepRawData* > rios;
      TgcPrepDataCollection::const_iterator it = (*tgcit)->begin();
      TgcPrepDataCollection::const_iterator it_end = (*tgcit)->end();
      for(; it != it_end; ++it) {
	double tgcTheta = (*it)->globalPosition().theta();
	double tgcPhi = (*it)->globalPosition().phi();
	if( ( tgcTheta > roadThetaMin && tgcTheta < roadThetaMax ) &&
	    isInPhiBounds(tgcPhi,roadPhi,phiWidth) ) rios.push_back( *it );
      }
      Amg::Vector3D patpose;
      Amg::Vector3D patdire;
      m_muonHoughMathUtils.extrapolateCurvedRoad(road.globalPosition(),roadmom,chCenter,patpose,patdire);
      MuonPatternChamberIntersect chIntersect(patpose,patdire,rios);
      chambers.push_back(chIntersect);
      
    }//end loop on TGC chambers

    
    //CSCs
    const CscPrepDataContainer* cscPrds = 0;
    if(StatusCode::SUCCESS != m_storeGate->retrieve(cscPrds,csckey)) {
      mLog << MSG::ERROR << "Cannot retrieve the CSC PRD container " << csckey << endreq;
      return 0;
    }
    CscPrepDataContainer::const_iterator cscit     = cscPrds->begin();
    CscPrepDataContainer::const_iterator cscit_end = cscPrds->end();
    for(; cscit!=cscit_end; ++cscit) {	
      if( (*cscit)->size() == 0 ) continue;
      //get the detector element from the first PRD
      const MuonGM::MuonReadoutElement* detEl = (*cscit)->at(0)->detectorElement();
      Amg::Vector3D chCenter = detEl->center();
      if(fabs(chCenter.theta() - road.globalPosition().theta()) > 1) continue;
      //get the chamber dimensions
      //double Zsize = detEl->getZsize();
      double Ssize = detEl->getSsize();
      double Rsize = detEl->getRsize();
      //chamber range in theta
      double chThetaMax = atan2(chCenter.perp() - Rsize/2.,chCenter.z());
      double chThetaMin = atan2(chCenter.perp() + Rsize/2.,chCenter.z());
      //chamber phi ranges
      double deltaPhi = atan(Ssize/(2.*chCenter.perp()));
      double chPhiMax = chCenter.phi() + deltaPhi;
      double chPhiMin = chCenter.phi() - deltaPhi;
      
      //check if the chamber is in the road
      if( !( isInPhiBounds(chPhiMax,roadPhi,phiWidth) && isInPhiBounds(chPhiMin,roadPhi,phiWidth) ) ) continue;
      if( !( (chThetaMin > roadThetaMin && chThetaMin < roadThetaMax) ||
	     (chThetaMax > roadThetaMax && chThetaMax < roadThetaMax) ) ) continue;
      
      //if the chamber is in the road, associate hits to it
      std::vector< const Trk::PrepRawData* > rios;
      CscPrepDataCollection::const_iterator it = (*cscit)->begin();
      CscPrepDataCollection::const_iterator it_end = (*cscit)->end();
      for(; it != it_end; ++it) {
	double cscTheta = (*it)->globalPosition().theta();
	double cscPhi = (*it)->globalPosition().phi();
	if( ( cscTheta > roadThetaMin && cscTheta < roadThetaMax ) &&
	    isInPhiBounds(cscPhi,roadPhi,phiWidth) ) rios.push_back( *it );
      }
      Amg::Vector3D patpose;
      Amg::Vector3D patdire;
      m_muonHoughMathUtils.extrapolateCurvedRoad(road.globalPosition(),roadmom,chCenter,patpose,patdire);
      MuonPatternChamberIntersect chIntersect(patpose,patdire,rios);
      chambers.push_back(chIntersect);
      
    }//end loop on CSC chambers

    MuonPatternCombination *patterncombo = new MuonPatternCombination(trkpars, chambers);
    patterncombo->setTrackRoadType((int)road.type());
    return patterncombo;
  }

  bool MuonRoadChamberAssocTool::isInPhiBounds(double phi, double roadPhi, double phiWidth) {
    //check if the phi is in the proper range
    double dphi = phi - roadPhi;
    if(dphi < -m_PI) dphi += 2*m_PI;
    else if(dphi > m_PI) dphi -= 2*m_PI;
    if(fabs(dphi) < phiWidth) return true;
    
    return false;
  }

  StatusCode MuonRoadChamberAssocTool::finalize() {
    return StatusCode::SUCCESS;
  }
}
