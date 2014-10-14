/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MdtRegionDefiner.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TrigL2MuonSA/MdtRegion.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MdtRegionDefiner::MdtRegionDefiner(MsgStream* msg) :
  m_msg(msg), m_mdtGeometry(0)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MdtRegionDefiner::MdtRegionDefiner(MsgStream* msg,
						 const MDTGeometry* mdtGeometry) :
  m_msg(msg), m_mdtGeometry(mdtGeometry)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


TrigL2MuonSA::MdtRegionDefiner::~MdtRegionDefiner(void)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::setMdtGeometry(const MDTGeometry* mdtGeometry)
{
  m_mdtGeometry = mdtGeometry;
}

void TrigL2MuonSA::MdtRegionDefiner::setRpcGeometry(bool use_rpc)
{
  m_use_rpc = use_rpc;
  return;
}
void TrigL2MuonSA::MdtRegionDefiner::setGeometry(bool use_new_geometry)
{
   m_use_new_geometry = use_new_geometry;
   return;
 }

// set the pointers for the new cabling and geometry
void TrigL2MuonSA::MdtRegionDefiner::setMdtGeometry(const MdtIdHelper* mdtIdHelper, 
						    const MuonGM::MuonDetectorManager* muonMgr)
{
  m_mdtIdHelper = mdtIdHelper;
  m_muonMgr = muonMgr;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::getMdtRegions(const LVL1::RecMuonRoI* p_roi,
							 const TrigL2MuonSA::RpcFitResult& rpcFitResult,
							 TrigL2MuonSA::MuonRoad& muonRoad,
							 TrigL2MuonSA::MdtRegion& mdtRegion)
{
  mdtRegion.Clear();
  
  int sectors[2];
    
  sectors[0] = muonRoad.MDT_sector_trigger;
  sectors[1] = muonRoad.MDT_sector_overlap;

  for(int i_station=0; i_station<3; i_station++) {
    int station = i_station;
    for(int i_sector=0; i_sector<2; i_sector++) { // 0: normal, 1: overlap
      int sector = sectors[i_sector];
      msg() << MSG::DEBUG << "--- station/sector=" << i_station << "/" << i_sector << endreq;

      if( sector==99 ) continue;
      float zMin   = 0;
      float zMax   = 0;
      float rMin   = 0;
      float rMax   = 0;
      float etaMin = 0;
      float etaMax = 0;
      float phiMin = 0;
      float phiMax = 0;
      int types[2];
      int& ty1 = types[0];
      int& ty2 = types[1];

      if(m_use_new_geometry) {
        float tmp_rMin = 0;
        float tmp_rMax = 0;
        ty1 = -1;
        ty2 = -1;

        for(int sta_iter=0; sta_iter< (int)muonRoad.stationList.size(); sta_iter++){

          Identifier id = muonRoad.stationList[sta_iter];
	  int stationPhi = m_mdtIdHelper->stationPhi(id);
          std::string name = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
	  int station_this = 99;
	  int sector_this = 99;
	  bool isEndcap;

	  find_station_sector(name, stationPhi, isEndcap, station_this, sector_this);
	  
	  if(station_this == station && sector_this == sector && !isEndcap){
	    if(ty1 == -1)
	      ty1 = m_mdtIdHelper->stationNameIndex(name)+1;
	    else if(ty2 == -1)
	      ty2 = m_mdtIdHelper->stationNameIndex(name)+1;
	    m_mdtReadout = m_muonMgr->getMdtReadoutElement(id);	
	    m_muonStation = m_mdtReadout->parentMuonStation();
	    float scale = 10.;
	    
	    Amg::Transform3D trans = Amg::CLHEPTransformToEigen(*m_muonStation->getNominalAmdbLRSToGlobal());
	    //HepGeom::Transform3D* trans = m_muonStation->getNominalAmdbLRSToGlobal();

	    Amg::Vector3D OrigOfMdtInAmdbFrame = 
	      Amg::Hep3VectorToEigen( m_muonStation->getBlineFixedPointInAmdbLRS() );	    
	    //	    HepPoint3D OrigOfMdtInAmdbFrame = m_muonStation->getBlineFixedPointInAmdbLRS();

	    tmp_rMin = (trans*OrigOfMdtInAmdbFrame).perp()/scale;
	    tmp_rMax = tmp_rMin+m_muonStation->Rsize()/scale;

	    if(rMin==0 || tmp_rMin < rMin)rMin = tmp_rMin;
	    if(rMax==0 || tmp_rMax > rMax)rMax = tmp_rMax;	

	  }
        }
      }
      else {
	m_mdtGeometry->getBstatR(sector, station, rMin, rMax);
	m_mdtGeometry->getBstatT(sector, station, ty1, ty2);
      }

      float max_road = muonRoad.MaxWidth(i_station);
      find_barrel_road_dim(max_road,
			   muonRoad.aw[station][i_sector],
			   muonRoad.bw[station][i_sector],
			   rMin,rMax,
			   &zMin,&zMax);
      
      msg() << MSG::DEBUG << "...zMin/zMax/ty1/ty2=" << zMin << "/" << zMax << "/" << types[0] << "/" << types[1] << endreq;
      msg() << MSG::DEBUG << "...rMin/rMax=" << rMin << "/" << rMax << endreq;
      
      find_eta_min_max(zMin, rMin, zMax, rMax, etaMin, etaMax);

      find_phi_min_max(muonRoad.phiRoI, phiMin, phiMax);

      msg() << MSG::DEBUG << "...etaMin/etaMax/phiMin/phiMax=" << etaMin << "/" << etaMax << "/" << phiMin << "/" << phiMax << endreq;

      mdtRegion.zMin[i_station][i_sector] = zMin;
      mdtRegion.zMax[i_station][i_sector] = zMax;
      mdtRegion.rMin[i_station][i_sector] = rMin;
      mdtRegion.rMax[i_station][i_sector] = rMax;
      mdtRegion.etaMin[i_station][i_sector] = etaMin;
      mdtRegion.etaMax[i_station][i_sector] = etaMax;
      mdtRegion.phiMin[i_station][i_sector] = phiMin;
      mdtRegion.phiMax[i_station][i_sector] = phiMax;
      for(int i_type=0; i_type<2; i_type++) {
	int type = types[i_type];
	if( type == -1 ) continue;
	mdtRegion.chamberType[i_station][i_sector][i_type] = type;
      }
    }
  }
 
  if (!m_use_rpc) {
    for (int i=0; i<3; i++){
      for (int j=0; j<2; j++){
        muonRoad.phi[i][j] = p_roi->phi();
      }
    }
  }
  else {
    StatusCode sc = computePhi(p_roi, rpcFitResult, mdtRegion, muonRoad);
    if (sc.isFailure()) {
      msg() << MSG::ERROR << "Error in comupting phi" << endreq;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::getMdtRegions(const LVL1::RecMuonRoI* p_roi,
							 const TrigL2MuonSA::TgcFitResult& tgcFitResult,
							 TrigL2MuonSA::MuonRoad& muonRoad,
							 TrigL2MuonSA::MdtRegion& mdtRegion)
{
  mdtRegion.Clear();
  
  int sectors[2];
  
  sectors[0] = muonRoad.MDT_sector_trigger;
  sectors[1] = muonRoad.MDT_sector_overlap;

  const float MiddleMargin = 1000;

  for(int i_station=0; i_station<5; i_station++) {
    int station = i_station;
    int chamber = 0;
    if (i_station==0) chamber = 3;//endcap inner
    if (i_station==1) chamber = 4;//endcap middle
    if (i_station==2) chamber = 5;//endcap outer
    if (i_station==3) chamber = 6;//endcap extra
    if (i_station==4) chamber = 0;//barrel inner
    for(int i_sector=0; i_sector<2; i_sector++) { // 0: normal, 1: overlap
      int sector = sectors[i_sector];
      msg() << MSG::DEBUG << "--- chamber/sector=" << chamber << "/" << i_sector << endreq;
      if( sector==99 ) continue;
      float zMin   = 0;
      float zMax   = 0;
      float rMin   = 0;
      float rMax   = 0;
      float etaMin = 0;
      float etaMax = 0;
      float phiMin = 0;
      float phiMax = 0;
      int types[2];
      int& ty1 = types[0];
      int& ty2 = types[1];
      float sta_zMin = 0;                                                                              
      float sta_zMax = 0;                                                                                                                 
      
      if (chamber==0) { // barrel inner

	if(m_use_new_geometry) {
 
	  float tmp_rMin = 0;
	  float tmp_rMax = 0;
	  ty1 = -1;
	  ty2 = -1;
	  
	  for(int sta_iter=0; sta_iter<(int)muonRoad.stationList.size(); sta_iter++) {
	    Identifier id = muonRoad.stationList[sta_iter];
	    int stationPhi = m_mdtIdHelper->stationPhi(id);
	    std::string name = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
	    int station_this = 99;
	    int sector_this = 99;
 
	    bool isEndcap;
	    find_station_sector(name, stationPhi, isEndcap, station_this, sector_this);

	    if(station_this == station && sector_this == sector && !isEndcap){
	      if(ty1 == -1)
		ty1 = m_mdtIdHelper->stationNameIndex(name)+1;
	      else if(ty2 == -1)
		ty2 = m_mdtIdHelper->stationNameIndex(name)+1;
	      m_mdtReadout = m_muonMgr->getMdtReadoutElement(id);	
	      m_muonStation = m_mdtReadout->parentMuonStation();
	      float scale = 10.;
	      Amg::Transform3D trans = Amg::CLHEPTransformToEigen(*m_muonStation->getNominalAmdbLRSToGlobal());
	      //HepGeom::Transform3D* trans = m_muonStation->getNominalAmdbLRSToGlobal();
	      
	      Amg::Vector3D OrigOfMdtInAmdbFrame = 
		Amg::Hep3VectorToEigen( m_muonStation->getBlineFixedPointInAmdbLRS() );	    
	      //	    HepPoint3D OrigOfMdtInAmdbFrame = m_muonStation->getBlineFixedPointInAmdbLRS();
	      
	      tmp_rMin = (trans*OrigOfMdtInAmdbFrame).perp()/scale;
	      tmp_rMax = tmp_rMin+m_muonStation->Rsize()/scale;
	      if(rMin==0 || tmp_rMin < rMin)rMin = tmp_rMin;
	      if(rMax==0 || tmp_rMax > rMax)rMax = tmp_rMax;	
	    }
	  }
	}
	else { // use the old cabling
	  m_mdtGeometry->getBstatR(sector, 0, rMin, rMax);//calculate rMin and rMax
	  m_mdtGeometry->getBstatT(sector, 0, ty1, ty2);//calculate ty1 and ty2
	  rMin = 10*rMin;
	  rMax = 10*rMax;
	  //  zMin = ( p_roi->eta()<0 )? -6000:4000;
	  //  zMax = ( p_roi->eta()<0 )? -4000:6000;
	}
	zMin = ( p_roi->eta()<0 )? -6000:4000;
	zMax = ( p_roi->eta()<0 )? -4000:6000;

	float max_road = muonRoad.MaxWidth(i_station);
	find_barrel_road_dim(max_road,
			     muonRoad.aw[station][i_sector],
			     muonRoad.bw[station][i_sector],
			     rMin,rMax,
			     &zMin,&zMax);
      }
      
      else {//for endcap
	
	if (m_use_new_geometry) {
	  float tmp_zMin = 0;
	  float tmp_zMax = 0;
	  int sign = 1;
	  ty1 = -1;
	  ty2 = -1;
	  
	  for(int sta_iter=0; sta_iter<(int)muonRoad.stationList.size(); sta_iter++){
	    Identifier id = muonRoad.stationList[sta_iter];
	    int stationPhi = m_mdtIdHelper->stationPhi(id);
	    std::string name = m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
	    int station_this = 99;
	    int sector_this = 99;
	    bool isEndcap;
	    find_station_sector(name, stationPhi, isEndcap, station_this, sector_this);
	    
	    if(station_this == station && sector_this == sector && isEndcap){
	      if(ty1 == -1)
		ty1 = m_mdtIdHelper->stationNameIndex(name)+1;
	      else if(ty2 == -1)
		ty2 = m_mdtIdHelper->stationNameIndex(name)+1;
	      m_mdtReadout = m_muonMgr->getMdtReadoutElement(id);	
	      m_muonStation = m_mdtReadout->parentMuonStation();
	      
	      Amg::Transform3D trans = Amg::CLHEPTransformToEigen(*m_muonStation->getNominalAmdbLRSToGlobal());
	      //HepGeom::Transform3D* trans = m_muonStation->getNominalAmdbLRSToGlobal();
	      
	      Amg::Vector3D OrigOfMdtInAmdbFrame = 
		Amg::Hep3VectorToEigen( m_muonStation->getBlineFixedPointInAmdbLRS() );	    
	      //	    HepPoint3D OrigOfMdtInAmdbFrame = m_muonStation->getBlineFixedPointInAmdbLRS();

	      tmp_zMin = (trans*OrigOfMdtInAmdbFrame).z();
	      if(tmp_zMin < 0) sign = -1;
	      else if(tmp_zMin > 0) sign = 1;
	      tmp_zMax = tmp_zMin + sign*m_muonStation->Zsize();

	      if(sta_zMin==0 || tmp_zMin<sta_zMin) sta_zMin = tmp_zMin;
	      if(sta_zMin==0 || tmp_zMax<sta_zMin) sta_zMin = tmp_zMax;
	      if(sta_zMax==0 || tmp_zMax>sta_zMax) sta_zMax = tmp_zMax;
	      if(sta_zMax==0 || tmp_zMin>sta_zMax) sta_zMax = tmp_zMin;
	    }
	  }
	  
	}

	if (chamber==4) { // Endcap

	  if (tgcFitResult.isSuccess) { // TGC data is properly read
	    if (tgcFitResult.tgcMid1[3] < tgcFitResult.tgcMid2[3]) {
	      zMin = tgcFitResult.tgcMid1[3];
	      zMax = tgcFitResult.tgcMid2[3];
	    } else {
	      zMin = tgcFitResult.tgcMid2[3];
	      zMax = tgcFitResult.tgcMid1[3];
	    }
	    
	  } else { // Failed to read TGC data 

	    if (m_use_new_geometry) {
	      zMin = sta_zMin;
	      zMax = sta_zMax;
	    } else {
	      m_mdtGeometry->getEstatZ(sector, i_station, muonRoad.side, zMin, zMax);
	      zMin *= 10;
	      zMax *= 10;
	      zMin -= MiddleMargin;
	      zMax += MiddleMargin;
	    }
	  }
	} else {
	  if (m_use_new_geometry) {
	    zMin = sta_zMin;
	    zMax = sta_zMax;
	  } else {
	    m_mdtGeometry->getEstatZ(sector, i_station, muonRoad.side, zMin, zMax);
	    zMin *= 10;
	    zMax *= 10;
	  }
	}
	if (!m_use_new_geometry)
	  m_mdtGeometry->getEstatT(sector, i_station, muonRoad.side, ty1, ty2); 

	find_endcap_road_dim(muonRoad.rWidth[chamber][0],
			     muonRoad.aw[chamber][i_sector],
			     muonRoad.bw[chamber][i_sector],
			     zMin,zMax,
			     &rMin,&rMax);
      }  //end endcap

      msg() << MSG::DEBUG << "...zMin/zMax/ty1/ty2=" << zMin << "/" << zMax << "/" << types[0] << "/" << types[1] << endreq;
      msg() << MSG::DEBUG << "...rMin/rMax=" << rMin << "/" << rMax << endreq;
      
      find_eta_min_max(zMin, rMin, zMax, rMax, etaMin, etaMax);
      
      find_phi_min_max(muonRoad.phiRoI, phiMin, phiMax);
      
      msg() << MSG::DEBUG << "...etaMin/etaMax/phiMin/phiMax=" << etaMin << "/" << etaMax << "/" << phiMin << "/" << phiMax << endreq;
      
      mdtRegion.zMin[chamber][i_sector] = zMin;
      mdtRegion.zMax[chamber][i_sector] = zMax;
      mdtRegion.rMin[chamber][i_sector] = rMin;
      mdtRegion.rMax[chamber][i_sector] = rMax;
      mdtRegion.etaMin[chamber][i_sector] = etaMin;
      mdtRegion.etaMax[chamber][i_sector] = etaMax;
      mdtRegion.phiMin[chamber][i_sector] = phiMin;
      mdtRegion.phiMax[chamber][i_sector] = phiMax;
      for(int i_type=0; i_type<2; i_type++) {
	int type = types[i_type];
	if( type == -1 ) continue;
	mdtRegion.chamberType[chamber][i_sector][i_type] = type;
      }
    } 
  }
  
  StatusCode sc = computePhi(p_roi, tgcFitResult, mdtRegion, muonRoad);
  if (sc.isFailure()) {
    msg() << MSG::ERROR << "Erro in comupting phi" << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_station_sector(std::string name, int phi, bool& endcap, 
							 int& station, int& sector)
{   
   if(name[0]=='E' || name[0]=='F')
     endcap = true;
   else 
     endcap = false;
   int largeSmall = 0; 
   if(name[2]=='S') largeSmall = 1;
   sector = (phi-1)*2 + largeSmall;
   if(name[1]=='I' || name[1]=='E')
     station = 0;
   if(name[1]=='M')
     station = 1;
   if(name[1]=='O')
     station = 2;
     
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_phi_min_max(float phiMiddle, float& phiMin, float& phiMax)
{   
   phiMin = phiMiddle - 0.1;
   phiMax = phiMiddle + 0.1;
   if ( phiMin<0.      )     phiMin += 2*CLHEP::pi;
   if ( phiMax>2*CLHEP::pi ) phiMax -= 2*CLHEP::pi;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_eta_min_max(float zMin, float rMin, float zMax, float rMax,
						      float& etaMin, float& etaMax)
{   
   const bool doEmulateMuFast = true;
   // const bool doEmulateMuFast = false;

   etaMin = 0.;
   etaMax = 0.;

   if(rMin > 0 && rMax > 0)
   {
      float eta[4];
      float theta;

      theta  = (fabs(zMin)>0.1)? atan(rMin/fabsf(zMin)): CLHEP::pi/2.;
      eta[0] = (zMin>0.)?  -log(tan(theta/2.)) : log(tan(theta/2.));

      theta  = (fabs(zMax)>0.1)? atan(rMin/fabsf(zMax)): CLHEP::pi/2.;
      eta[1] = (zMax>0.)?  -log(tan(theta/2.)) : log(tan(theta/2.));
      if(doEmulateMuFast) eta[1] = eta[0];

      theta  = (fabs(zMax)>0.1)? atan(rMax/fabsf(zMax)): CLHEP::pi/2.;
      eta[2] = (zMax>0.)?  -log(tan(theta/2.)) : log(tan(theta/2.)); 

      theta  = (fabs(zMin)>0.1)? atan(rMax/fabsf(zMin)): CLHEP::pi/2.;
      eta[3] = (zMin>0.)?  -log(tan(theta/2.)) : log(tan(theta/2.)); 
      if(doEmulateMuFast) eta[3] = eta[2];

      etaMin = eta[0];
      etaMax = eta[0];
      for(int i=1; i<4; i++) {
	 if( eta[i] < etaMin ) etaMin = eta[i];
	 if( eta[i] > etaMax ) etaMax = eta[i];
      }

      if (etaMin > etaMax) {
	 float tmp = etaMax;
	 etaMax = etaMin;
	 etaMin = tmp;
      }
   }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_barrel_road_dim(float max_road, float aw, float bw,
							  float rMin,float rMax,float *zMin,float *zMax)
{
  float ia,iaq,dz,z,points[4];
  int i;
  
  *zMin =  9999.;
  *zMax = -9999.;
  
  if(aw) {
    ia  = 1/aw;
    iaq = ia*ia;
  } else {
    ia  = 0;
    iaq = 0;
  }
  
  dz = max_road*sqrt(1.+iaq);
  z  = (ia)? (rMin-bw)*ia : bw;
  points[0] = z - dz;
  points[1] = z + dz;
  z  = (ia)? (rMax-bw)*ia : bw;
  points[2] = z - dz;
  points[3] = z + dz;
  
  for(i=0;i<4;i++) {
    if(*zMin>=points[i]) *zMin = points[i];
    if(*zMax<=points[i]) *zMax = points[i];
  }

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_endcap_road_dim(float road,float aw, float bw, float zMin,
							  float zMax,float *rMin,float *rMax)
{
   float r,points[4];
   int i;
  
   *rMin = 999999.;
   *rMax = 0.;
    
   if(zMin>=99999. && zMax<=-99999.)
   {
      *rMin = 0.;
      *rMax = 0.;
      return;
   }
    
   r = zMin*aw + bw;
   points[0] = r - road;
   points[1] = r + road;
   r = zMax*aw + bw;
   points[2] = r - road;
   points[3] = r + road;
    
   for(i=0;i<4;i++) {
      if(*rMin>=points[i]) *rMin = points[i];
      if(*rMax<=points[i]) *rMax = points[i];
   }
    
   if(*rMin<0.) *rMin = 0.;
   if(*rMax<0.) *rMax = 0.;
    
   return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits)
{
   const double PHI_BOUNDARY = 0.2;

   // loop over TGC digits.
   unsigned int iHit;
   for (iHit = 0; iHit < tgcHits.size(); iHit++)
   {
      // Get the digit point.
      const TrigL2MuonSA::TgcHitData& hit = tgcHits[iHit];

      // reject width=0 hits
      const double ZERO_LIMIT = 1e-5;
      if( fabs(hit.width) < ZERO_LIMIT ) continue;

      double w = 12.0 / hit.width / hit.width;
      if (hit.isStrip)
      {
	 w *= hit.r * hit.r;
	 double phi = hit.phi;
	 if( phi < 0 && ( (CLHEP::pi+phi)<PHI_BOUNDARY) ) phi += CLHEP::pi*2;
	 if      ( hit.sta < 3 ) { m_tgcStripMidPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, phi, w)); }
	 else if ( hit.sta ==3 ) { m_tgcStripInnPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, phi, w)); }
      }
      else
      {
	 if      ( hit.sta < 3 ) { m_tgcWireMidPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, hit.r, w)); }
	 else if ( hit.sta ==3 ) { m_tgcWireInnPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, hit.r, w)); }
      }
   }

   return StatusCode::SUCCESS;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::computePhi(const LVL1::RecMuonRoI* p_roi,
						      const TrigL2MuonSA::RpcFitResult& rpcFitResult,
						      const TrigL2MuonSA::MdtRegion& mdtRegion,
						      TrigL2MuonSA::MuonRoad& muonRoad)
{

  for(int i_station=0; i_station<3; i_station++) {
    for( int i_sector=0; i_sector<2; i_sector++) {

      if ( rpcFitResult.isSuccess ) {

	if (i_station==1 || !rpcFitResult.isPhiDir) {
	  muonRoad.phi[i_station][i_sector] = rpcFitResult.phi;
	  continue;
	}
	
	double dz = 0.;
	
	if (i_station==2) {
	  
	  double MiddleR = fabs(mdtRegion.rMin[1][i_sector] + mdtRegion.rMax[1][i_sector])/2.;
	  double MiddleZ = MiddleR*muonRoad.aw[1][i_sector] + muonRoad.bw[1][i_sector];
	  
	  double mm = (muonRoad.aw[2][i_sector]!=0)? 1./muonRoad.aw[2][i_sector] : 0.;
	  double OuterR  = fabs(mdtRegion.rMin[2][i_sector]+mdtRegion.rMax[2][i_sector])/2.;
	  double OuterZ  = (mm)? (OuterR-muonRoad.bw[2][i_sector])/mm : muonRoad.bw[2][i_sector];
	  double DzOuter = fabs(OuterZ-MiddleZ);
	  dz = sqrt((OuterR-MiddleR)*(OuterR-MiddleR) + DzOuter*DzOuter);
	  dz = (OuterR-MiddleR);
	  
	} else if (i_station==0) {
	  
	  double MiddleR = 0;
	  double MiddleZ = 0;
	  
	  double mm = (muonRoad.aw[0][i_sector]!=0)? 1./muonRoad.aw[0][i_sector] : 0.;
	  double InnerR  = fabs(mdtRegion.rMin[0][i_sector]+mdtRegion.rMax[0][i_sector])/2.;
	  double InnerZ  = (mm)? (InnerR-muonRoad.bw[0][i_sector])/mm  : muonRoad.bw[0][i_sector];
	  double DzInner = fabs(InnerZ-MiddleZ);
	  dz = -sqrt((InnerR-MiddleR)*(InnerR-MiddleR) + DzInner*DzInner);
	  dz = - fabsf(InnerR-MiddleR);
	  
	}
        
	muonRoad.phi[i_station][i_sector] = (dz)* rpcFitResult.dPhidZ + rpcFitResult.phi;
	
	while (muonRoad.phi[i_station][i_sector] > CLHEP::pi)
	  muonRoad.phi[i_station][i_sector] -= 2*CLHEP::pi;
	
	while (muonRoad.phi[i_station][i_sector] <-CLHEP::pi)
	  muonRoad.phi[i_station][i_sector] += 2*CLHEP::pi;

      } else {
        // RPC data is not read -> use RoI
        muonRoad.phi[i_station][i_sector] = p_roi->phi();
      }
    }
  }
  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::computePhi(const LVL1::RecMuonRoI* p_roi,
						      const TrigL2MuonSA::TgcFitResult& tgcFitResult,
						      const TrigL2MuonSA::MdtRegion& mdtRegion,
						      TrigL2MuonSA::MuonRoad& muonRoad)
{

  for(int i_station=0; i_station<5; i_station++) {
    int chamber = 0;
    if (i_station==0) chamber = 3;//endcap inner
    if (i_station==1) chamber = 4;//endcap middle
    if (i_station==2) chamber = 5;//endcap outer
    if (i_station==3) chamber = 6;//endcap extra
    if (i_station==4) chamber = 0;//barrel inner
    for( int i_sector=0; i_sector<2; i_sector++) {

      if ( tgcFitResult.isSuccess ) {
	if (chamber==4 || !tgcFitResult.isPhiDir) {
	  muonRoad.phi[chamber][i_sector] = tgcFitResult.phi;
	  continue;
	}
	
	muonRoad.phi[chamber][i_sector] = 0.;
	double dz = 0.;
	
	if (chamber==5) {
	  
	  double MiddleZ = fabs(mdtRegion.zMin[4][i_sector] + mdtRegion.zMax[4][i_sector])/2.;
	  double MiddleR = MiddleZ*muonRoad.aw[4][i_sector] + muonRoad.bw[4][i_sector];
	  
	  double OuterZ  = fabs(mdtRegion.zMin[5][i_sector] + mdtRegion.zMax[5][i_sector])/2.;
	  double OuterR  = OuterZ*muonRoad.aw[5][i_sector] + muonRoad.bw[5][i_sector];
	  double DrOuter = fabs(OuterR-MiddleR);
	  dz = sqrt((OuterZ-MiddleZ)*(OuterZ-MiddleZ) + DrOuter*DrOuter);
	  dz = (OuterZ-MiddleZ);
	  
	} if (chamber==3 || chamber==0) {

	  double MiddleZ = 0;
	  double MiddleR = 0;
	  
	  if(tgcFitResult.tgcInn[2] != 0.) {
	    muonRoad.phi[chamber][i_sector] = tgcFitResult.tgcInn[1];
	    continue;
	  }
	  double InnerZ  = fabs(mdtRegion.zMin[3][i_sector] + mdtRegion.zMax[3][i_sector])/2.;
	  double InnerR  = InnerZ*muonRoad.aw[3][i_sector] + muonRoad.bw[3][i_sector];
	  double DrInner = fabs(InnerR-MiddleR);
	  dz = -sqrt((InnerZ-MiddleZ)*(InnerZ-MiddleZ) + DrInner*DrInner);
	  dz = -fabsf(InnerZ-MiddleZ);
	}
        
	muonRoad.phi[chamber][i_sector] = (dz)* tgcFitResult.dPhidZ + tgcFitResult.phi;
	while (muonRoad.phi[chamber][i_sector] > CLHEP::pi)  muonRoad.phi[chamber][i_sector] -= 2*CLHEP::pi;
	while (muonRoad.phi[chamber][i_sector] <-CLHEP::pi)  muonRoad.phi[chamber][i_sector] += 2*CLHEP::pi;

      } else {
        // TGC data is not read -> use RoI
        muonRoad.phi[chamber][i_sector] = p_roi->phi();
      }
    }
  }
  return true;
}  

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

