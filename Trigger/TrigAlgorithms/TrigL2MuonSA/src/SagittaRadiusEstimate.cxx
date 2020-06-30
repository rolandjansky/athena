/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "SagittaRadiusEstimate.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::SagittaRadiusEstimate::SagittaRadiusEstimate(const std::string& type,
							   const std::string& name,
							   const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::SagittaRadiusEstimate::setMCFlag(BooleanProperty use_mcLUT,
						    const AlignmentBarrelLUTSvc* alignmentBarrelLUTSvc)
{
  m_use_mcLUT = use_mcLUT;
  if ( alignmentBarrelLUTSvc ) m_alignmentBarrelLUT = alignmentBarrelLUTSvc->alignmentBarrelLUT();
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::SagittaRadiusEstimate::setSagittaRadius(const LVL1::RecMuonRoI*     p_roi,
								 TrigL2MuonSA::RpcFitResult& rpcFitResult,
								 TrigL2MuonSA::TrackPattern& trackPattern)
{
  const int MAX_STATION = 4;
  const float ZERO_LIMIT = 1e-5;

  int nit;
  const int nitmx=10;
  int count=0;
  double a3,theta,rad,phi,one,phim=0,signZ;
  
  double c0,c1,c2,c3,c22,c33,e2,e3,c2q,c3q,d,da,db,a,b,dx,dy;
  double m = 0.;
  double cost = 0.;
  double x0 = 0., y0 = 0., x1 = 0., y1 = 0., x2 = 0., y2 = 0., x3 = 0., y3 = 0.;
  double tm = 0.;
  double xn = 0.;
  const double eps = 0.005;
  
  TrigL2MuonSA::SuperPoint* superPoints[4];

  for (int i_station=0; i_station<MAX_STATION; i_station++) {

    int chamberID = -1;
    if ( i_station == 0 ) chamberID = xAOD::L2MuonParameters::Chamber::BarrelInner;
    if ( i_station == 1 ) chamberID = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
    if ( i_station == 2 ) chamberID = xAOD::L2MuonParameters::Chamber::BarrelOuter;
    if ( i_station == 3 ) chamberID = xAOD::L2MuonParameters::Chamber::EndcapInner;
    superPoints[i_station] = &(trackPattern.superPoints[chamberID]);

    if (superPoints[i_station]->R > ZERO_LIMIT)  {
      count++;
      if ( i_station != 3 ){
        phim = superPoints[i_station]->Phim;
      }
    }
  }
  
  if ( superPoints[3] -> R >ZERO_LIMIT ) count--; // Not use Endcap Inner
  if ( count==2 ) {
    y0  = 4230.;    // radius of calorimeter.
    
    if (superPoints[0]->R < ZERO_LIMIT) {
      x2 = superPoints[1]->Z;
      y2 = superPoints[1]->R;
      x3 = superPoints[2]->Z;
      y3 = superPoints[2]->R;
    } else if (superPoints[1]->R < ZERO_LIMIT) {
      x2 = superPoints[0]->Z;
      y2 = superPoints[0]->R;
      x3 = superPoints[2]->Z;
      y3 = superPoints[2]->R;
    } else if (superPoints[2]->R < ZERO_LIMIT) {
      x2 = superPoints[0]->Z;
      y2 = superPoints[0]->R;
      x3 = superPoints[1]->Z;
      y3 = superPoints[1]->R;
    }

    dx = x3 - x2;
    dy = y3 - y2;
      
    x0 = y0*x2/y2;
    
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
    if (fabs(xn)<ZERO_LIMIT) xn = ZERO_LIMIT;//To avoid divergence
    
    x1 = xn;
    y1 = y0;    
    
    if (superPoints[0]->R > ZERO_LIMIT ) {
      rad = superPoints[0]->R;
      theta = atan2(rad,(double)fabs(superPoints[0]->Z));
      signZ = (fabs(superPoints[0]->Z) > ZERO_LIMIT)? superPoints[0]->Z/fabs(superPoints[0]->Z): 1.;
    } else {
      rad = y1;
      theta = atan2(rad,(double)fabs(x1));
      signZ = (fabs(x1) > ZERO_LIMIT)? x1/fabs(x1): 1.;
    }
        
    trackPattern.etaMap = (-log(tan(theta/2.)))*signZ;
    if (rpcFitResult.isSuccess ) {
      //      one = (fabs(rpcFitResult.rpc1[0]) > 0.)? 1. * rpcFitResult.rpc1[0] / fabs(rpcFitResult.rpc1[0]): 1.;
      one = (cos(rpcFitResult.phi)>0)? 1: -1;
    } else {
      one = (cos(p_roi->phi())>0)? 1: -1;
    }
    phi = atan2(trackPattern.phiMSDir*one,one);

    if(phim>=M_PI+0.1) phim = phim - 2*M_PI;
    
    if(phim>=0) trackPattern.phiMap = (phi>=0.)? phi - phim : phim -fabs(phi);
    else trackPattern.phiMap = phi - phim;
    
    trackPattern.phiMS = phi;
    
    c2 = x2 - x1;                      
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
    
    x0 = -a/2. + x1;
    y0 = -b/2. + y1;
    trackPattern.barrelRadius  = sqrt(x0*x0 + y0*y0);
    trackPattern.charge = -1;
    if(a<=0.) trackPattern.charge = 1;
    
  } else if (count==3) {
        
    rad = superPoints[0]->R;
    theta = atan2(rad,(double)fabs(superPoints[0]->Z));
    signZ = superPoints[0]->Z/fabs(superPoints[0]->Z);

    trackPattern.etaMap = (-log(tan(theta/2.)))*signZ;

    if (rpcFitResult.isSuccess ) {
      //      one = (fabs(rpcFitResult.rpc1[0]) > 0.)? 1. * rpcFitResult.rpc1[0] / fabs(rpcFitResult.rpc1[0]): 1;
      one = (cos(rpcFitResult.phi)>0)? 1: -1;
    } else {
      one = (cos(p_roi->phi())>0)? 1: -1;
    }
    phi = atan2(trackPattern.phiMSDir*one,one);
    if(phim>=M_PI+0.1) phim = phim - 2*M_PI;

    if(phim>=0) trackPattern.phiMap = (phi>=0.)? phi - phim : phim -fabs(phi);
    else trackPattern.phiMap = phi - phim;

    trackPattern.phiMS = phi;

    // Alignment correation to LargeSpecial
    if ( trackPattern.s_address==1) {

      if ( !m_alignmentBarrelLUT ) {
	ATH_MSG_ERROR("Alignment correction service is not prepared");
	return StatusCode::FAILURE;
      }

      double dZ = (*m_alignmentBarrelLUT)->GetDeltaZ(trackPattern.s_address,
						     trackPattern.etaMap,
						     trackPattern.phiMap,
						     trackPattern.phiMS,
						     superPoints[0]->R);
      superPoints[1]->Z += 10*dZ;
    }

    a3 = ( superPoints[2]->Z - superPoints[0]->Z ) / ( superPoints[2]->R - superPoints[0]->R );
    
    trackPattern.barrelSagitta = superPoints[1]->Z - superPoints[1]->R*a3 - superPoints[0]->Z + superPoints[0]->R*a3;
    
    m = a3;
    cost = cos(atan(m));
    x2 = superPoints[1]->R - superPoints[0]->R;
    y2 = superPoints[1]->Z - superPoints[0]->Z;
    x3 = superPoints[2]->R - superPoints[0]->R;
    y3 = superPoints[2]->Z - superPoints[0]->Z;
    
    tm = x2;
    x2 = ( x2   + y2*m)*cost;
    y2 = (-tm*m + y2  )*cost;
    
    tm = x3;
    x3 = ( x3   + y3*m)*cost;
    y3 = (-tm*m + y3  )*cost;
    
    x0 = x3/2.;
    y0 = (y2*y2 + x2*x2 -x2*x3)/(2*y2);
    
    trackPattern.barrelRadius = sqrt(x0*x0 + y0*y0);
    trackPattern.charge = (trackPattern.barrelSagitta!=0)? -1.*trackPattern.barrelSagitta/fabs(trackPattern.barrelSagitta): 0;

  }

  if ( m_use_endcapInner == true && count == 1 && superPoints[3]->R > ZERO_LIMIT ) {
    y0  = 4230.;    // radius of calorimeter.
    
    if (superPoints[0]->R > ZERO_LIMIT) {
      x2 = superPoints[0]->Z; //BI
      y2 = superPoints[0]->R;
      x3 = superPoints[3]->Z; //EI
      y3 = superPoints[3]->R;
    } else if (superPoints[1]->R > ZERO_LIMIT) {
      x2 = superPoints[3]->Z; //EI
      y2 = superPoints[3]->R;
      x3 = superPoints[1]->Z; //BM
      y3 = superPoints[1]->R;
    } else if (superPoints[2]->R > ZERO_LIMIT) {
      x2 = superPoints[3]->Z; //EI
      y2 = superPoints[3]->R;
      x3 = superPoints[2]->Z; //BO
      y3 = superPoints[2]->R;
    }

    dx = x3 - x2;
    dy = y3 - y2;

    x0 = y0*x2/y2;

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
    if (fabs(xn)<ZERO_LIMIT) xn = ZERO_LIMIT;//To avoid divergence

    x1 = xn;
    y1 = y0;    

    if (superPoints[0]->R > ZERO_LIMIT ) {
      rad = superPoints[0]->R;
      theta = atan2(rad,(double)fabs(superPoints[0]->Z));
      signZ = (fabs(superPoints[0]->Z) > ZERO_LIMIT)? superPoints[0]->Z/fabs(superPoints[0]->Z): 1.;
    } else {
      rad = y1;
      theta = atan2(rad,(double)fabs(x1));
      signZ = (fabs(x1) > ZERO_LIMIT)? x1/fabs(x1): 1.;
    }

    trackPattern.etaMap = (-log(tan(theta/2.)))*signZ;
    if (rpcFitResult.isSuccess ) {
      //      one = (fabs(rpcFitResult.rpc1[0]) > 0.)? 1. * rpcFitResult.rpc1[0] / fabs(rpcFitResult.rpc1[0]): 1.;
      one = (cos(rpcFitResult.phi)>0)? 1: -1;
    } else {
      one = (cos(p_roi->phi())>0)? 1: -1;
    }
    phi = atan2(trackPattern.phiMSDir*one,one);

    if(phim>=M_PI+0.1) phim = phim - 2*M_PI;

    if(phim>=0) trackPattern.phiMap = (phi>=0.)? phi - phim : phim -fabs(phi);
    else trackPattern.phiMap = phi - phim;

    trackPattern.phiMS = phi;

    c2 = x2 - x1;                      
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
    
    x0 = -a/2. + x1;
    y0 = -b/2. + y1;
    double barrelRadius = sqrt(x0*x0 + y0*y0);
    trackPattern.barrelRadius  = barrelRadius;
    trackPattern.charge = -1;
    if(a<=0.) trackPattern.charge = 1;
  }

  ATH_MSG_DEBUG("... count/trackPattern.barrelSagitta/barrelRadius/charge/s_address/phi="
		<< count << " / " << trackPattern.barrelSagitta << "/" << trackPattern.barrelRadius << "/"
		<< trackPattern.charge << "/" << trackPattern.s_address << "/"
		<< trackPattern.phiMS);
  
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

