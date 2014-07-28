/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/SagEstimate.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string SagEstimate::s_name = "Sagitta computing";

SagEstimate::SagEstimate(MuMessageInterface& msg,
             const TriggerData& triggerData,
             const TrackFit& track_fit,
             const SpointData super_points[],
             MuFeature& feature)
    : SagEstimateStd(msg,triggerData, track_fit, super_points, feature)
{
}

SagEstimate::~SagEstimate(void)
{
}

bool
SagEstimate::run(MuonFeatureDetails* det)
{
    int nit,nitmx=10;
    int st,count=0,add=-1;
    double sagitta,a3,theta,rad,phi,phim=0,one,signZ;
    
    double c0,c1,c2,c3,c22,c33,e2,e3,c2q,c3q,d,da,db,a,b,dx,dy;
    double m = 0.;
    double cost = 0.;
    double x0 = 0.;
    double y0 = 0.;
    double x1 = 0.;
    double y1 = 0.;
    double x2 = 0.;
    double y2 = 0.;
    double x3 = 0.;
    double y3 = 0.;
    double tm = 0.;
    double xn = 0.;
    double eps = 0.0005;
    

    for(st=0;st<MAXSTATION;st++) {
        if(m_super_points[st].R > 0.) {
            if (add < 0) add = (st%3)? st-st%3 : st;
            m_feature.Saddress = st/3;
            m_feature.Spoint[st%3][0] = m_super_points[st].R;
            m_feature.Spoint[st%3][1] = m_super_points[st].Z;
            m_feature.Spoint[st%3][2] = m_super_points[st].Alin;
            m_feature.Sdigi [st%3][0] = m_super_points[st].Ndigi;
            m_feature.Sdigi [st%3][1] = m_super_points[st].Npoint;
            phim = m_super_points[st].Phim;
            count++;
        }
    }
    
    m_feature.NSpoint = count;
    
    if        (count==2) {
        y0  = 423.;    // radius of calorimeter.
        if (!m_feature.Spoint[0][0]) {
            m_feature.Sdigi [0][0] = m_super_points[add].Ndigi;
            m_feature.Sdigi [0][1] = m_super_points[add].Npoint;
	    x2 = m_super_points[add+1].Z;
	    y2 = m_super_points[add+1].R;
	    x3 = m_super_points[add+2].Z;
	    y3 = m_super_points[add+2].R;
        }
        if (!m_feature.Spoint[1][0]) {
            m_feature.Sdigi [1][0] = m_super_points[add+1].Ndigi;
            m_feature.Sdigi [1][1] = m_super_points[add+1].Npoint;
	    x2 = m_super_points[add].Z;
	    y2 = m_super_points[add].R;
	    x3 = m_super_points[add+2].Z;
	    y3 = m_super_points[add+2].R;
	}
        if (!m_feature.Spoint[2][0]) {
            m_feature.Sdigi [2][0] = m_super_points[add+2].Ndigi;
            m_feature.Sdigi [2][1] = m_super_points[add+2].Npoint;
	    x2 = m_super_points[add].Z;
	    y2 = m_super_points[add].R;
	    x3 = m_super_points[add+1].Z;
	    y3 = m_super_points[add+1].R;
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
      
        x1 = xn;
        y1 = y0;
    
    
        if (!m_feature.Spoint[0][0])
	{
	   m_feature.Spoint[0][0] = y1;
	   m_feature.Spoint[0][1] = x1;
	   m_feature.Spoint[0][2] = x1/y1;
	}
    
    
    
        rad = m_feature.Spoint[0][0];
        theta = atan2(rad,(double)fabs(m_feature.Spoint[0][1]));
	signZ = m_feature.Spoint[0][1]/fabs(m_feature.Spoint[0][1]);
        m_feature.EtaMap = (-log(tan(theta/2.)))*signZ;
        one = 1. * m_trig_data.rpc1[0] / fabs(m_trig_data.rpc1[0]);
        phi = atan2(m_track_fit.ap*one,one);
        if(phim>=PI+0.1) phim = phim - 2*PI;
        if(phim>=0) m_feature.PhiMap = (phi>=0.)? phi - phim : phim -fabs(phi);
        else m_feature.PhiMap = phi - phim;
        //m_feature.phi = (phi>=0.)? phi : 2*PI + phi;
	m_feature.phi = phi;
        m_feature.dir_phi = m_track_fit.ap;
    
    
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
        m_feature.Radius  = sqrt(x0*x0 + y0*y0);
        m_feature.Charge = 1;
        if(a<=0.) m_feature.Charge = -1;
    
        m_feature.ok = 1;
	
    } else if (count==3) {
        
	rad = m_feature.Spoint[0][0];
        theta = atan2(rad,(double)fabs(m_feature.Spoint[0][1]));
	signZ = m_feature.Spoint[0][1]/fabs(m_feature.Spoint[0][1]);
        m_feature.EtaMap = (-log(tan(theta/2.)))*signZ;
        one = 1. * m_trig_data.rpc1[0] / fabs(m_trig_data.rpc1[0]);
        phi = atan2(m_track_fit.ap*one,one);
        if(phim>=PI+0.1) phim = phim - 2*PI;
        if(phim>=0) m_feature.PhiMap = (phi>=0.)? phi - phim : phim -fabs(phi);
        else m_feature.PhiMap = phi - phim;
        //m_feature.phi = (phi>=0.)? phi : 2*PI + phi;
        m_feature.phi = phi;
	m_feature.dir_phi = m_track_fit.ap;
    
        
	a3 = (m_feature.Spoint[2][1]-m_feature.Spoint[0][1])/
            (m_feature.Spoint[2][0]-m_feature.Spoint[0][0]);
        
	sagitta = m_feature.Spoint[1][1] - m_feature.Spoint[1][0]*a3 -
            m_feature.Spoint[0][1] + m_feature.Spoint[0][0]*a3;
        m_feature.Sagitta = sagitta;
        
	m = a3;
	cost = cos(atan(m));
	x2 = m_feature.Spoint[1][0] - m_feature.Spoint[0][0];
	y2 = m_feature.Spoint[1][1] - m_feature.Spoint[0][1];
	x3 = m_feature.Spoint[2][0] - m_feature.Spoint[0][0];
	y3 = m_feature.Spoint[2][1] - m_feature.Spoint[0][1];
	
	//std::cout << "dist=" << sqrt(x3*x3 + y3*y3) << std::endl;;
	
	tm = x2;
	x2 = ( x2   + y2*m)*cost;
	y2 = (-tm*m + y2  )*cost;
	
	tm = x3;
	x3 = ( x3   + y3*m)*cost;
	y3 = (-tm*m + y3  )*cost;
	
	x0 = x3/2.;
	y0 = (y2*y2 + x2*x2 -x2*x3)/(2*y2);
	
	m_feature.Radius = sqrt(x0*x0 + y0*y0);
	
	//std::cout << "x3=" << x3 << ",   y3=" << y3 << std::endl;
	//std::cout << "radius=" << m_feature.Radius << std::endl; 
	
	m_feature.Charge = m_feature.Sagitta/fabs(m_feature.Sagitta);
	
	m_feature.ok = 1;
    }

    if (det!=0) {
        det->setAddress(m_feature.Saddress);
	det->setSagitta(m_feature.Sagitta);
	det->setRadius(m_feature.Radius);
	det->setSlope(m_feature.Spoint[0][2]);
	det->setIntercept(m_feature.Spoint[0][1]);
	det->setEta(m_feature.EtaMap);
	det->setPhiMap(m_feature.PhiMap);
	det->setPhi(m_feature.phi);
	det->setPhiDir(m_feature.dir_phi);
	det->setCharge(m_feature.Charge);
    }

    return true;
} 
  
