/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/PtLUTSvc.h"
#include "TrigmuFast/AlphaBetaEstimate.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

double const PI_OVER_4 = M_PI / 4.0;
double const PI_OVER_8 = M_PI / 8.0;
double const PHI_RANGE = 12.0 / PI_OVER_8;

using namespace std;

const string AlphaBetaEstimate::s_name = "Alpha and Beta computing";

AlphaBetaEstimate::AlphaBetaEstimate(MuMessageInterface& msg,
             const PtLUTSvc* LUTsvc,
             const TriggerData& triggerData,
             const TrackFit& track_fit,
             const SpointData super_points[],
             MuFeature& feature)
    : SagEstimateStd(msg,triggerData, track_fit, super_points, feature),
      pPtLUTSvc(LUTsvc)
{
}

AlphaBetaEstimate::~AlphaBetaEstimate(void)
{
}

bool
AlphaBetaEstimate::run(MuonFeatureDetails* det)
{
    bool retval = true;

    // load super points data
    float phim = 0.;
    int count  = 0;
 
    for(int st=0;st<MAXSTATION;st++) {
        if(m_super_points[st].R > 0.) {
            //if (add < 0) add = (st%3)? st-st%3 : st;
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

    
    m_feature.Saddress = -1;
    m_feature.NSpoint = count;
    
    
    // computing ALPHA, BETA and RADIUS
    float InnerSlope = 0;
    float MiddleSlope = 0;
    float MiddleIntercept = 0;
    float OuterSlope  = 0;
    float OuterIntercept = 0;
    float InnerR  = 0;
    float InnerZ  = 0;
    float InnerEta = m_trig_data.tgcInn[0];
    float MiddleR = 0; 
    float MiddleZ = 0;
    float OuterR  = 0;
    float OuterZ  = 0;
      
    for (int st=0;st<MAXSTATION;++st) {
      
        if( m_super_points[st].Npoint>2 && m_super_points[st].R > 0.) {
	    if(st%3==0) { 
	        InnerSlope = m_super_points[st].Alin;
	        InnerR = m_super_points[st].R;
	        InnerZ = m_super_points[st].Z;
		double theta = atan2(InnerR,fabs(InnerZ));
		double signZ = InnerZ/fabsf(InnerZ);
		InnerEta = (-log(tan(theta/2.)))*signZ;
	    }
	    if(st%3==1) { 
	        MiddleSlope = m_super_points[st].Alin;
	        MiddleIntercept = m_super_points[st].R -
		                   MiddleSlope*m_super_points[st].Z;
	        MiddleR = m_super_points[st].R;
		MiddleZ = m_super_points[st].Z;
	    }
	    if(st%3==2) {
	        OuterSlope  = m_super_points[st].Alin;
                OuterIntercept = m_super_points[st].R -
		                  OuterSlope*m_super_points[st].Z;
	        OuterR = m_super_points[st].R;
		OuterZ = m_super_points[st].Z;
	    }
	}  
	  
    }
    
    // set EtaMap and phi
    double phi = 0.;
    if(MiddleZ==0.)
    {
	phi = m_track_fit.phi;
	if(m_trig_data.tgcMid1[0]!=0 && m_trig_data.tgcMid2[0]!=0) {
	    m_feature.EtaMap=(m_trig_data.tgcMid1[0]+m_trig_data.tgcMid2[0])/2;
	} else {
	    m_feature.EtaMap=(m_trig_data.tgcMid1[0]!=0)? 
	                      m_trig_data.tgcMid1[0] : m_trig_data.tgcMid2[0];
	}
    }
    
    if(InnerR)
    {
        phi = (m_trig_data.tgcInn[3] !=0)? m_trig_data.tgcInn[1] : 
	                                   m_track_fit.extrInnerPhi;
        double theta = atan(InnerR/fabsf(InnerZ));
        double etaInner = -log(tan(theta/2.))*InnerZ/fabsf(InnerZ);
        m_feature.EtaMap = etaInner;
    }
    else if (MiddleZ)
    {
        phi = m_track_fit.phi;
        double theta = atan(MiddleR/fabsf(MiddleZ));
        double etaMiddle = -log(tan(theta/2.))*MiddleZ/fabsf(MiddleZ);
        m_feature.EtaMap = etaMiddle;  
    }
    
    // ----- in case strange etaMap
    const float ETA_LIMIT       = 4.0;
    const float DELTA_ETA_LIMIT = 1.0;
    float roiEta = m_trig_data.roiEta;
    if( fabs(m_feature.EtaMap) > ETA_LIMIT || fabs(m_feature.EtaMap-roiEta) > DELTA_ETA_LIMIT ) {
       retval = false;
       m_feature.EtaMap = roiEta;
       // char buf[256];
       // m_msg->lock();
       // sprintf(buf,"Strange EtaMap value=%f, replacing to RoIEta=%f",m_feature.EtaMap,roiEta);
       // m_msg->displayWarning(buf);
       // sprintf(buf,"..tgcMid1=%f/%f/%f/%f",m_trig_data.tgcMid1[0],m_trig_data.tgcMid1[1],m_trig_data.tgcMid1[2],m_trig_data.tgcMid1[3]);
       // m_msg->displayWarning(buf);
       // sprintf(buf,"..InnerR/InnerZ=%f/%f",InnerR,InnerZ);
       // m_msg->displayWarning(buf);
       // sprintf(buf,"..MiddleR/MiddleZ=%f/%f",MiddleR,MiddleZ);
       // m_msg->displayWarning(buf);
       // m_msg->unlock();
    }
    // ----- in case strange etaMap
        
    if(phim>M_PI+0.1) phim = phim - 2*M_PI;
    if(phim>=0) m_feature.PhiMap = (phi>=0.)? phi - phim : phim - fabs(phi);
    else m_feature.PhiMap = phi - phim;
        
    m_feature.beta    = 0.0;
    m_feature.phi     = phi;
    m_feature.dir_phi = m_track_fit.ap;
    m_feature.Pt      = fabs(m_trig_data.tgcPT);
    m_feature.Charge  = (m_trig_data.tgcPT!=0.0)?
                         m_trig_data.tgcPT/fabs(m_trig_data.tgcPT) : +1;
    
    int Octant = (int)(m_trig_data.tgcMid1[1] / PI_OVER_4);
    double PhiInOctant = fabs(m_trig_data.tgcMid1[1] - Octant * PI_OVER_4);
    if (PhiInOctant > PI_OVER_8) PhiInOctant = PI_OVER_4 - PhiInOctant;
    m_feature.phiBin = static_cast<int>(PhiInOctant * PHI_RANGE);
    m_feature.etaBin = static_cast<int>((fabs(m_trig_data.tgcMid1[0])-1.)/0.05);
    
    if(MiddleZ && OuterZ) 
    {
        double slope = (OuterR-MiddleR)/(OuterZ-MiddleZ);
	double inter = MiddleR - slope*MiddleZ;
	  
	m_feature.alpha  = pPtLUTSvc->alpha(MiddleZ,MiddleR,OuterZ,OuterR);
	m_feature.slope  = slope;
	m_feature.intercept  = inter;
	  
	if(InnerR) 
	{
	    m_feature.beta   = fabsf( atan(InnerSlope) - atan(slope) ); 
	    m_feature.deltaR = slope * InnerZ + MiddleIntercept - InnerR;
	    m_feature.Radius = compute_radius(InnerSlope, InnerR,  InnerZ,
	                                      slope,      MiddleR, MiddleZ);
	}
	
    } 
    else 
    {
        if( m_feature.Pt >= 8.) 
	{
	    if(MiddleZ) 
	    {
	        double Ze = MiddleZ+(fabsf(MiddleZ)/MiddleZ)*1000.;
		double Re = MiddleSlope*(Ze) + MiddleIntercept;
	        m_feature.alpha = pPtLUTSvc->alpha(MiddleZ,MiddleR,Ze,Re);
		m_feature.slope  = MiddleSlope;
 	        m_feature.intercept  = MiddleIntercept;
	    }

	} 
	
	if(MiddleZ && InnerZ) 
	{
	      m_feature.beta   = fabsf( atan(InnerSlope) - atan(MiddleSlope) );
	      m_feature.deltaR = MiddleSlope*InnerZ + MiddleIntercept - InnerR;
	      m_feature.Radius = compute_radius(InnerSlope,  InnerR,  InnerZ,
	                                        MiddleSlope, MiddleR, MiddleZ);
	      
	}
    }
    
    m_feature.ok = 1;
    
    if (det!=0) {
        det->setAddress(m_feature.Saddress);
	det->setRadius(m_feature.Radius);
	det->setSlope(m_feature.Spoint[0][2]);
	det->setIntercept(m_feature.Spoint[0][1]);
	det->setEta(m_feature.EtaMap);
	det->setPhiMap(m_feature.PhiMap);
	det->setPhi(m_feature.phi);
	det->setPhiDir(m_feature.dir_phi);
	det->setCharge(m_feature.Charge);
	det->setAlpha(m_feature.alpha);
        det->setBeta(m_feature.beta);
        det->setDeltaR(m_feature.deltaR);
	det->setPt(m_feature.Pt*m_feature.Charge);
    }
    
    return retval;
} 
  
double 
AlphaBetaEstimate::compute_radius(double InnerSlope, double InnerR, double InnerZ,
                          double MiddleSlope, double MiddleR, double MiddleZ)
{
    double cr1 = 0.080/400;
    double cr2 = cr1;
    double x1 = InnerZ;
    if(fabs(x1)>=0.1) 
    {
        double x2 = MiddleZ;
        double y1 = InnerR;
        double y2 = MiddleR;
        double A1 = InnerSlope;
        double A2 = MiddleSlope;
        if(!(fabs(MiddleSlope+999)<0.1)) 
        {
            A2 = MiddleSlope;
            cr2 = cr1/10;
        }
		  
        // find centre of circonference
        double xm = (x1+x2)/2.;
        double ym = (y1+y2)/2.;
        double c1 = (x2-x1)*xm+(y2-y1)*ym;
        double c2_1 = -x1-A1*y1;
        double c2_2 = -x2-A2*y2;
        double yR1 = (-c1-c2_1*(x2-x1))/(A1*(x2-x1)-(y2-y1));
        double yR2 = (-c1-c2_2*(x2-x1))/(A2*(x2-x1)-(y2-y1));
        double xR1 = -A1*yR1-c2_1;
        double xR2 = -A2*yR2-c2_2;

        double xR = ((1./cr1)*xR1+(1./cr2)*xR2)/((1./cr1)+(1./cr2));
        double yR = ((1./cr1)*yR1+(1./cr2)*yR2)/((1./cr1)+(1./cr2));
        double sign = m_feature.deltaR / fabs(m_feature.deltaR);
        double radius = 0.5*(sqrt((xR-x1)*(xR-x1)+(yR-y1)*(yR-y1))+sqrt((xR-x2)*(xR-x2)+(yR-y2)*(yR-y2)));
        return sign * radius;    
    }
    
    return 0.;
}
