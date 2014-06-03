/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRegionSelector/LArRegionSelectorCheckOL.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/IdentifierHash.h" 

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrRegion.h"
#include "CaloIdentifier/CaloLVL1_ID.h" 
#include "LArTools/LArCablingService.h" 
#include "CaloTriggerTool/CaloTriggerTowerService.h" 

#include "RegionSelector/IRegionLUT_Creator.h" 
#include "RegionSelector/RegionSelectorLUT.h" 



LArRegionSelectorCheckOL::LArRegionSelectorCheckOL
(const std::string& name, ISvcLocator* pSvcLocator): Algorithm(name, pSvcLocator), 

  m_testTable(true),
  m_checkOVL(true),
  m_detStore(0),
  m_ddman(0),
  m_ttman(0),
  m_TT_ID(0),
  m_toolSvc(0),
  m_cablingSvc(0),
  m_ttSvc(0)
{
  declareProperty("TestTable", m_testTable);
  declareProperty("CheckOverlaps", m_checkOVL);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArRegionSelectorCheckOL::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  log << MSG::INFO << "Algorithm Properties" << endreq;
  log << MSG::INFO << " Test Table:      " 
      << ((m_testTable) ? "true" : "false") <<endreq;

  StatusCode sc;

  // Get DetectorStore service
  sc = service("DetectorStore",m_detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } 

  // Retrieve DD manager:

  sc=m_detStore->retrieve(m_ddman);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not find the CaloDetDescrMgr "
	<< endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "CaloDetDescr Manager found" << endreq;
  }

  // Retrieve TT manager and helper:
  /*
  sc=m_detStore->retrieve(m_ttman);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not find the CaloTTMgr "
	<< endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "CaloTTMgr Manager found" << endreq;
  }
  */
  if(StatusCode::SUCCESS != m_detStore->retrieve(m_TT_ID) ) {
    log << MSG::FATAL << " failed to get CaloLVL1_ID "
	<< endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "CaloLVL1_ID helper found" << endreq;
  } 

  // Retrieve needed tools: LArCablingSvc 

  sc   = service( "ToolSvc",m_toolSvc  );
  if(! sc.isSuccess()) { 
     return sc;
  }  

  sc =m_toolSvc->retrieveTool("LArCablingService",m_cablingSvc);
  if(!sc.isSuccess() ) {
    log << MSG::DEBUG << "Failed to retrieve LArCablingSvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Successfully retrieved LArCablingSvc" << endreq;
  }

  sc =m_toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc);
  if(!sc.isSuccess() ) {
    log << MSG::DEBUG << "Failed to retrieve CaloTriggerTowerSvc" << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Successfully retrieved CaloTriggerTowerSvc" << endreq;
  }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArRegionSelectorCheckOL::execute() {
  MsgStream log(msgSvc(), name());

  StatusCode sc;

  if(!m_ttman){
   sc=m_detStore->retrieve(m_ttman);
   if (sc.isFailure()) {
    log << MSG::FATAL << "Could not find the CaloTTMgr "
	<< endreq;
    return StatusCode::FAILURE;
   } else {
    log << MSG::DEBUG << "CaloTTMgr Manager found" << endreq;
   }
  }
  if (m_checkOVL) sc = checkOverlaps();
  else sc = StatusCode::SUCCESS;

  if (m_testTable) sc = testMaps();
  else sc = StatusCode::SUCCESS;

  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArRegionSelectorCheckOL::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArRegionSelectorCheckOL::checkOverlaps() {

  MsgStream log(msgSvc(), name());
  CaloTTDescrManager::calo_region_const_iterator it2 =
    m_ttman->calo_region_begin();	
  CaloTTDescrManager::calo_region_const_iterator it2_end =
    m_ttman->calo_region_end();	

  int nok=0;
  int nok2=0;
  int nfail=0;
  int n0siz=0;
  int nTTlay=0;

  // loop on CaloTTDescrRegions, which only exist for LAr (no Tile)
  for(; it2!=it2_end;++it2){
    const CaloTTDescrRegion* region = (*it2); 

    ++nTTlay;
    Identifier lay_id = region->identify(); 
    int sam = m_TT_ID->sampling(lay_id);
    int layer = m_TT_ID->layer(lay_id);

    if(layer==0) std::cout << "+++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "lay_id,sampling, layer= " 
	      << m_TT_ID->show_to_string(lay_id) << " "
	      << sam << " " << layer << std::endl;

    std::string det=" TILE";
    bool isB(0), isEC(0);
    if(m_TT_ID->is_emb(lay_id)) {
      det=" EMB";
      isB=true;
    }
    else if(m_TT_ID->is_barrel_end(lay_id)) {
      det="BEND";
      //isBE=true;
    }
    else if(m_TT_ID->is_emec(lay_id)) {
      det="EMEC";
      isEC=true;
    }
    else if(m_TT_ID->is_hec(lay_id)) {
      det=" HEC";
      //isHEC=true;
    }
    else if(m_TT_ID->is_fcal(lay_id)) {
      det="FCAL";
      //isFCAL=true;
    }
    else {
      // should not happen since tile have no CaloTTDescrRegion
    }
    
    float e1,e2,p1,p2,r1,r2,z1,z2 ;
    float dEta = region->deta() ; 
    float dPhi = region->dphi() ; 
    float dRho = region->drho() ; 
    float dZ   = region->dz() ; 
    
    e1 = region->eta()-dEta/2. ; 
    e2 = e1+dEta; 
    
    p1 = region->phi()-dPhi/2. ; 
    p2 = p1+dPhi; 
      
    r1 = region->rho()-dRho/2. ; 
    r2 = r1+dRho; 
      
    // warning: since dZ is approximate, z-dZ might 
    // become negative even ig z>0 (and the other way round for z,0)
    z1 = region->z()-dZ/2. ; 
    z2 = z1+dZ; 
      
    float etaMin=99999.;
    float phiMin=99999.;
    float etaMax=-99999.;
    float phiMax=-99999.;
    float rhoMin=99999.;
    float zMin=99999.;
    float rhoMax=-99999.;
    float zMax=-99999.;
	  
    std::vector<Identifier> vec = m_ttSvc->createCellIDvecLayer(lay_id);
    ++nok2;
    if(vec.size() == 0) {++n0siz;}
    
    if(vec.size() > 0) {
      std::vector<Identifier>::const_iterator it     = vec.begin();
      std::vector<Identifier>::const_iterator it_end = vec.end();
      for(;it!=it_end;++it) {
	Identifier offId = (*it);
	const CaloDetDescrElement* caloDDE = m_ddman->get_element(offId);
	double eta  =caloDDE->eta_raw();
	double phi  =caloDDE->phi_raw();
	double cDeta=caloDDE->deta();
	double cDphi=caloDDE->dphi();
	double rho  =caloDDE->r_raw();
	double cDrho=caloDDE->dr();
	double z    =caloDDE->z_raw();
	double cDz  =caloDDE->dz();
	
	if( (eta-cDeta/2.)<etaMin ) {etaMin=eta-cDeta/2.;}
	if( (eta+cDeta/2.)>etaMax ) {etaMax=eta+cDeta/2.;}
	if( (phi-cDphi/2.)<phiMin ) {phiMin=phi-cDphi/2.;}
	if( (phi+cDphi/2.)>phiMax ) {phiMax=phi+cDphi/2.;}
	if( (rho-cDrho/2.)<rhoMin ) {rhoMin=rho-cDrho/2.;}
	if( (rho+cDrho/2.)>rhoMax ) {rhoMax=rho+cDrho/2.;}
	if( (z-cDz/2.)<zMin ) {zMin=z-cDz/2.;}
	if( (z+cDz/2.)>zMax ) {zMax=z+cDz/2.;}
	/*
	  std::cout << "cell: etamin, etamax, phimin, phimax= " 
	  << eta-cDeta/2. << " "
	  << eta+cDeta/2. << " "
	  << phi-cDphi/2. << " "
	  << phi+cDphi/2. << " "
	  << std::endl;
	  std::cout << "cell: rhomin, rhomax, zmin, zmax= " 
	  << rho-cDrho/2. << " "
	  << rho+cDrho/2. << " "
	  << z-cDz/2. << " "
	  << z+cDz/2. << " "
	  << std::endl;
	*/
      } // end loop on vector elements
    
    
      std::cout << "TT from descriptors:                       e1, e2, de, p1, p2, dp= " 
		<< e1 << " "
		<< e2 << " "
		<< dEta << " "
		<< p1 << " "
		<< p2 << " "
		<< dPhi << " "
		<< std::endl;
      std::cout << "TT from loop on cells: etamin, etamax, deta, phimin, phimax, dphi= " 
		<< etaMin << " "
		<< etaMax << " "
		<< etaMax-etaMin << " "
		<< phiMin << " "
		<< phiMax << " "
		<< phiMax-phiMin << " "
		<< std::endl;
      std::cout << "                     r1, r2, dr, z1, z2, dz= " 
		<< r1 << " "
		<< r2 << " "
		<< dRho << " "
		<< z1 << " "
		<< z2 << " "
		<< dZ << " "
		<< std::endl;
      std::cout << "       rhomin, rhomax, drho, zmin, zmax, dz= " 
		<< rhoMin << " "
		<< rhoMax << " "
		<< rhoMax -rhoMin << " "
		<< zMin << " "
		<< zMax << " "
		<< zMax-zMin << " "
		<< std::endl;

      // where do these shifts come from ??? are they expected ?
      float shiftPhi=0.;
      if(isB) {shiftPhi=+0.0031;}
      if(isEC) {shiftPhi=+0.0041;}
      //      if(isEC) {shiftPhi=-0.0041;}
      
      std::cout << "delta eta min " << (e1-etaMin) << std::endl;
      std::cout << "delta eta max " << (e2-etaMax) << std::endl;
      std::cout << "delta phi min " << (p1+shiftPhi-phiMin) << std::endl;
      std::cout << "delta phi max " << (p2+shiftPhi-phiMax) << std::endl;
      std::cout << "delta rho min " << (r1-rhoMin) << std::endl;
      std::cout << "delta rho max " << (r2-rhoMax) << std::endl;
      std::cout << "delta z min " << (z1-zMin) << std::endl;
      std::cout << "delta z max " << (z2-zMax) << std::endl;
      //      if(fabs(e1-etaMin)>0.001 || fabs(e2-etaMax)>0.001 || 
      //         fabs(p1+shiftPhi-phiMin)>0.001 || fabs(p2+shiftPhi-phiMax)>0.001)
      //	std::cout << " Warning, small mismatch in eta/phi for subdet " << det << std::endl;
      //      if(fabs(r1-rhoMin)>1. || fabs(r2-rhoMax)>1. || 
      //         fabs(z1-zMin)>1. || fabs(z2-zMax)>1.)
      //	std::cout << " Warning, small mismatch in rho/z for subdet " << det << std::endl;
      if(fabs(e1-etaMin)>0.01 || fabs(e2-etaMax)>0.01 || 
	 fabs(p1+shiftPhi-phiMin)>0.01 || fabs(p2+shiftPhi-phiMax)>0.01)
      	std::cout << " Warning, large mismatch in eta/phi for subdet " << det << std::endl;
      if(fabs(r1-rhoMin)>50. || fabs(r2-rhoMax)>50. || 
         fabs(z1-  zMin)>50. || fabs(z2-  zMax)>50.)
	std::cout << " Warning, large mismatch in rho/z for subdet " << det << std::endl;

    } // end condition on vec size
    else {
      std::cout << "tower with vec 0 size= " 
		<< m_TT_ID->show_to_string(lay_id)
		<< " sampling,iLay= " << sam << " " << layer
		<< std::endl;
    }
    
  } // end loop on regions
  std::cout << "ntt(off->on ok), ntt(onlTT got vec of chan), nfail, n0siz= " 
	    << nok << " " << nok2 << " " << nfail << " " << n0siz << std::endl;
  std::cout << "nTTlay= " << nTTlay << std::endl;

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArRegionSelectorCheckOL::testMaps() {

  MsgStream log(msgSvc(), name());
  bool dump=false;
  if (log.level()<=MSG::DEBUG) dump=true;
  
  IRegionLUT_Creator * lutCreator = 0;
  StatusCode sc = m_toolSvc->retrieveTool("LArRegionSelectorTable", lutCreator);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Error retrieving LArRegionSelectorTable Tool "<<endreq;
    return 0;
  } else {
    log << MSG::DEBUG << "Found LArRegionSelectorTable Tool " << endreq;

    const RegionSelectorLUT* ttLutEM=lutCreator->getLUT("EM");

    unsigned int nbElem=ttLutEM->maxHash();
    if(dump) log << MSG::DEBUG << " Retrieved RegionSelectorLUT for LArEM, containing following info (nb Elem): "<< nbElem << endreq;
    if(dump) log << MSG::DEBUG << " i, HashId, sampling, etamin,etamax,phimin,phimax,robid"	<< endreq;
    for(unsigned int i=0;i<nbElem;++i){
      int hashid=ttLutEM->hashId(i);
      double etamin=ttLutEM->etaMin(i);
      double etamax=ttLutEM->etaMax(i);
      double phimin=ttLutEM->phiMin(i);
      double phimax=ttLutEM->phiMax(i);
      int samp=ttLutEM->sampling(i);
      int robid=ttLutEM->robId(i);
      if(dump) log << MSG::DEBUG << i << " " << hashid << " " << samp << " "
	  << etamin << " " << etamax << " "
	  << phimin << " " << phimax << " "
	  << robid
	  << endreq;
    }

    const RegionSelectorLUT* ttLutHEC=lutCreator->getLUT("HEC");
    nbElem=ttLutHEC->maxHash();
    if(dump) log << MSG::DEBUG << " Retrieved RegionSelectorLUT for LArHEC, containing following info (nb Elem): "<< nbElem << endreq;
    if(dump) log << MSG::DEBUG << " i, HashId, sampling, etamin,etamax,phimin,phimax,robid"	<< endreq;
    for(unsigned int i=0;i<nbElem;++i){
      int hashid=ttLutHEC->hashId(i);
      double etamin=ttLutHEC->etaMin(i);
      double etamax=ttLutHEC->etaMax(i);
      double phimin=ttLutHEC->phiMin(i);
      double phimax=ttLutHEC->phiMax(i);
      int samp=ttLutHEC->sampling(i);
      int robid=ttLutHEC->robId(i);
      if(dump) log << MSG::DEBUG << i << " " << hashid << " " << samp << " "
	  << etamin << " " << etamax << " "
	  << phimin << " " << phimax << " "
	  << robid
	  << endreq;
    }

    const RegionSelectorLUT* ttLutFCALem=lutCreator->getLUT("FCALEM");
    nbElem=ttLutFCALem->maxHash();
    if(dump) log << MSG::DEBUG << " Retrieved RegionSelectorLUT for LArFCALem, containing following info (nb Elem): "<< nbElem << endreq;
    if(dump) log << MSG::DEBUG << " i, HashId, sampling, etamin,etamax,phimin,phimax,robid"	<< endreq;
    for(unsigned int i=0;i<nbElem;++i){
      int hashid=ttLutFCALem->hashId(i);
      double etamin=ttLutFCALem->etaMin(i);
      double etamax=ttLutFCALem->etaMax(i);
      double phimin=ttLutFCALem->phiMin(i);
      double phimax=ttLutFCALem->phiMax(i);
      int samp=ttLutFCALem->sampling(i);
      int robid=ttLutFCALem->robId(i);
      if(dump) log << MSG::DEBUG << i << " " << hashid << " " << samp << " "
	  << etamin << " " << etamax << " "
	  << phimin << " " << phimax << " "
	  << robid
	  << endreq;
    }

    const RegionSelectorLUT* ttLutFCALhad=lutCreator->getLUT("FCALHAD");
    nbElem=ttLutFCALhad->maxHash();
    if(dump) log << MSG::DEBUG << " Retrieved RegionSelectorLUT for LArFCALhad, containing following info (nb Elem): "<< nbElem<< endreq;
    if(dump)log << MSG::DEBUG << " i, HashId, sampling, etamin,etamax,phimin,phimax,robid"	<< endreq;
    for(unsigned int i=0;i<nbElem;++i){
      int hashid=ttLutFCALhad->hashId(i);
      double etamin=ttLutFCALhad->etaMin(i);
      double etamax=ttLutFCALhad->etaMax(i);
      double phimin=ttLutFCALhad->phiMin(i);
      double phimax=ttLutFCALhad->phiMax(i);
      int samp=ttLutFCALhad->sampling(i);
      int robid=ttLutFCALhad->robId(i);
      if(dump)log << MSG::DEBUG << i << " " << hashid << " " << samp << " "
	  << etamin << " " << etamax << " "
	  << phimin << " " << phimax << " "
	  << robid
	  << endreq;
    }

  }


  return sc;
}
