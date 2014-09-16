/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RpcRoadDefiner.h"
#include "CLHEP/Units/PhysicalConstants.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcRoadDefiner::RpcRoadDefiner(MsgStream* msg)
  : m_msg(msg), m_mdtGeometry(0), m_roadData(0),
    m_rWidth_RPC_Failed(0)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcRoadDefiner::~RpcRoadDefiner(void)
{
  if (m_roadData) delete m_roadData;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcRoadDefiner::setRoadWidthForFailure(double rWidth_RPC_Failed)
{
  m_rWidth_RPC_Failed = rWidth_RPC_Failed;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcRoadDefiner::setMdtGeometry(const MDTGeometry* mdtGeometry)
{
  m_mdtGeometry = mdtGeometry;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcRoadDefiner::defineRoad(const LVL1::RecMuonRoI*      p_roi,
						    TrigL2MuonSA::MuonRoad&      muonRoad,
						    TrigL2MuonSA::RpcFitResult&  rpcFitResult)
{

  if (!m_roadData) m_roadData = new BarrelRoadData();
  const TrigL2MuonSA::BarrelRoadData::Roads roadWidth = m_roadData->roads();
  
  int nit,nitmx=10;
  float x1,y1,x2,y2,x3,y3,x4,y4;
  float c0,c1,c2,c3,c22,c33,e2,e3,c2q,c3q,d,da,db,a,b;
  float dx,dy,x0,xn=0;
  float eps = 0.001;
  float y0  = 423.;                  // radius of calorimeter.
  const double ZERO_LIMIT = 1e-5;
  
  const int N_LAYER = 3; // 0: inner, 1: middle, 2: outer
  const int N_SECTOR = 2; // 0: normal, 1:overlap

  if (rpcFitResult.isSuccess) {
    // RPC data is properly read

    if(rpcFitResult.type==2) {               // fill superpoint value for High Pt trig.
      
      x1 = 0.;                    // A. Di Mattia version.
      y1 = 0.;
      x2 = rpcFitResult.rpc1[2];
      y2 = rpcFitResult.rpc1[3];
      x3 = rpcFitResult.rpc3[2];
      y3 = rpcFitResult.rpc3[3];
      x4 = rpcFitResult.rpc2[2];
      y4 = rpcFitResult.rpc2[3];
      
    } else {                         // fill superpoint value for Low Pt trig.
      x1 = 0.;
      y1 = 0.;
      x2 = rpcFitResult.rpc1[2];
      y2 = rpcFitResult.rpc1[3];
      x4 = x3 = rpcFitResult.rpc2[2];
      y4 = y3 = rpcFitResult.rpc2[3];
      
    }
    
    double phi1 = atan2(rpcFitResult.rpc1[1],rpcFitResult.rpc1[0]);
    double phi2 = atan2(rpcFitResult.rpc2[1],rpcFitResult.rpc2[0]);
    double rad1 = sqrt(rpcFitResult.rpc1[0]*rpcFitResult.rpc1[0]+rpcFitResult.rpc1[1]*rpcFitResult.rpc1[1]);
    double rad2 = sqrt(rpcFitResult.rpc2[0]*rpcFitResult.rpc2[0]+rpcFitResult.rpc2[1]*rpcFitResult.rpc2[1]);
    
    double ap1 = rpcFitResult.rpc1[1]/rpcFitResult.rpc1[0]; 
    double ap2 = rpcFitResult.rpc2[1]/rpcFitResult.rpc2[0];
    rpcFitResult.phiDir = (ap1+ap2)/2.;
    
    if(fabs(rad1)<ZERO_LIMIT || fabs(rad2)<ZERO_LIMIT) {
      rpcFitResult.isPhiDir = false;
      if(fabs(rad1)>=ZERO_LIMIT) rpcFitResult.phi = phi1;
      if(fabs(rad2)>=ZERO_LIMIT) rpcFitResult.phi = phi2;
    } else {   
      rpcFitResult.isPhiDir = true;
      if( phi1*phi2 < 0 && fabsf(phi1)>CLHEP::pi/2. ) {
	double tmp1 = (phi1>0)? phi1 - CLHEP::pi : phi1 + CLHEP::pi;
	double tmp2 = (phi2>0)? phi2 - CLHEP::pi : phi2 + CLHEP::pi;
	double tmp  = (tmp1+tmp2)/2.;
	rpcFitResult.dPhidZ = (tmp2-tmp1)/fabsf(rad2-rad1);
	rpcFitResult.phi = (tmp>0.)? tmp - CLHEP::pi : tmp + CLHEP::pi;
      } else {
	rpcFitResult.dPhidZ = (phi2-phi1)/fabsf(rad2-rad1);
	rpcFitResult.phi = (phi2+phi1)/2.;
      }
    }
    
    if(!y1) {                         // compute 1st point for Low Pt trig.
      dx = x3 - x2;
      dy = y3 - y2;
      
      x0 = y0*x2/y2;                  // starting point.
      
      c3  = dy;
      c2  = -y0*dx + 2.*(y2*x3-y3*x2);
      c1  = -dy*(y2*y3-y0*y0)+ y3*x2*x2 - y2*x3*x3;
      c0  = y0*x2*x3*dx + y0*x2*(y3-y0)*(y3-y0) - y0*x3*(y2-y0)*(y2-y0);
      c22 = 2.*c2;
      c33 = 3.*c3;
      
      nit = 1;
      while((nit++)<=nitmx&&fabs(x0-xn)>=eps) {
	xn = x0 - f(x0,c0,c1,c2,c3)/fp(x0,c33,c22,c1);
	x0 = xn;
      }
      
      x1 = xn;
      y1 = y0;
    }
    
    c2 = x2 - x1;                     // compute track parameters. 
    c3 = x3 - x1;
    e2 = y2 - y1;
    e3 = y3 - y1;
    c2q = c2*c2 + e2*e2;
    c3q = c3*c3 + e3*e3;
    d  = c2*e3 - c3*e2;
    da = -c2q*e3 + c3q*e2;
    db = -c2*c3q + c3*c2q;
    a = da/d;
    b = db/d;
    
    float is = 1;
    if(a<=0.) is = -1;
    
    float a2 = a/2.;
    float aq = a*a;
    
    muonRoad.isEndcap  = false;
    muonRoad.phiMiddle = atan2(rpcFitResult.rpc2[1],rpcFitResult.rpc2[0]);
    muonRoad.phiRoI    = p_roi->phi();
    muonRoad.side      = (rpcFitResult.rpc2[2]<0.)? 0 : 1;
    muonRoad.LargeSmall    = ((rpcFitResult.SectorId + 1)/2 )%2;
    
    int PhysicsSector = ((rpcFitResult.SectorId + 1)/4 )%8 + 1;
    
    int special = 0;
    if (muonRoad.LargeSmall == 0 && (PhysicsSector == 6 || PhysicsSector == 8 ))
      special = 1;
    if (muonRoad.LargeSmall == 1 && (PhysicsSector == 6 || PhysicsSector == 7 ))
      special = 1;
    muonRoad.Special = special;
    
    int type = rpcFitResult.type - 1;
    for (int i_station=0; i_station<3; i_station++) {
      int position = i_station + 3*muonRoad.Special + 6*muonRoad.LargeSmall; 
      for (int i_layer=0; i_layer<8; i_layer++) {
	muonRoad.rWidth[i_station][i_layer] = muonRoad.scales[i_station]*roadWidth[type][position][i_layer];
      }
    }
    
    int sector_trigger = 0;
    int sector_overlap = 0;
    m_mdtGeometry->getBsects(1,muonRoad.phiMiddle,sector_trigger, sector_overlap);
    int MDT_tr = (PhysicsSector - 1)*2 + muonRoad.LargeSmall;
    if (MDT_tr == sector_overlap) {
      sector_overlap = sector_trigger;
      sector_trigger = MDT_tr;
    }
    muonRoad.MDT_sector_trigger = sector_trigger;
    muonRoad.MDT_sector_overlap = sector_overlap;
    
    if(x4-x2) {
      muonRoad.aw[1][0] = (y4-y2)/(x4-x2);
      muonRoad.bw[1][0] = y2 - muonRoad.aw[1][0]*x2;
    } else {
      muonRoad.aw[1][0] = 0.;
      muonRoad.bw[1][0] = x2;
    }
    muonRoad.aw[1][1] = muonRoad.aw[1][0];
    muonRoad.bw[1][1] = muonRoad.bw[1][0];
    
    int sectors[2];
    sectors[0] = sector_trigger;
    sectors[1] = sector_overlap;
    
    for (int i_station=0; i_station<N_LAYER; i_station++) {
      if (i_station == 1) continue; 
      int station = i_station;
      
      for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
	int sector = sectors[i_sector];
	
	float rMin, rMax;
	
	m_mdtGeometry->getBstatR(sector, station, rMin, rMax);
	
	float e0,dd,del,xx1,xx2,dy,dx;
	
	dy = rMax - rMin;
	e0 = rMin - y1;
	dd = aq - 4*(e0*e0+b*e0);
	if(dd>=0.) {
	  del = sqrt(dd);
	  xx1 = -a2 + is*del/2.;
	  e0 = rMax - y1;
	  dd = aq - 4*(e0*e0 + b*e0);
	  if(dd>=0.) {
	    del = sqrt(dd);
	    xx2 = -a2 + is*del/2.;
	    dx = xx2-xx1;
	    if(dx!=0) {
	      muonRoad.aw[i_station][i_sector] = dy/dx;
	      muonRoad.bw[i_station][i_sector] = rMin - muonRoad.aw[i_station][i_sector]*(xx1 + x1);
	    } else {
	      muonRoad.aw[i_station][i_sector] = 0.;
	      muonRoad.bw[i_station][i_sector] = xx1 + x1;
	    }
	  }
	}
      }
    }

  } else {
    // RPC data is not available -> use RoI 

    muonRoad.isEndcap   = false;
    muonRoad.phiMiddle  = p_roi->phi();
    muonRoad.phiRoI     = p_roi->phi();
    muonRoad.side       = (p_roi->phi()<0.)? 0 : 1;
    muonRoad.LargeSmall = ((p_roi->sectorID() + 1)/2 )%2;
    
    int PhysicsSector = ((p_roi->sectorID() + 1)/4 )%8 + 1;
    
    int special = 0;
    if (muonRoad.LargeSmall == 0 && (PhysicsSector == 6 || PhysicsSector == 8 ))
      special = 1;
    if (muonRoad.LargeSmall == 1 && (PhysicsSector == 6 || PhysicsSector == 7 ))
      special = 1;
    muonRoad.Special = special;

    for (int i_station=0; i_station<3; i_station++) {
      for (int i_layer=0; i_layer<8; i_layer++) {
        muonRoad.rWidth[i_station][i_layer] = m_rWidth_RPC_Failed;
      }
    }

    int sector_trigger = 0;
    int sector_overlap = 0;
    m_mdtGeometry->getBsects(1,muonRoad.phiMiddle,sector_trigger, sector_overlap);
    int MDT_tr = (PhysicsSector - 1)*2 + muonRoad.LargeSmall;
    if (MDT_tr == sector_overlap) {
      sector_overlap = sector_trigger;
      sector_trigger = MDT_tr;
    }
    muonRoad.MDT_sector_trigger = sector_trigger;
    muonRoad.MDT_sector_overlap = sector_overlap;
    
    double roiEta = p_roi->eta();
    double theta  = atan(exp(-fabs(roiEta)))*2.;
    double aw     = (fabs(roiEta) > ZERO_LIMIT)? tan(theta)*(fabs(roiEta)/roiEta): 0.;

    for (int i_station=0; i_station<N_LAYER; i_station++) {
      for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
        muonRoad.aw[i_station][i_sector] = aw;
        muonRoad.bw[i_station][i_sector] = 0;
      }
    }
  }
        
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
