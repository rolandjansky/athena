/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/TrackLZN.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string TrackLZN::s_name = "Track fit from RPC points";

TrackLZN::TrackLZN(MuMessageInterface& msg,
                   const TriggerData& trig_data, TrackFit& track,
		   RoadRegions& roads)
    : TrackStd(msg,trig_data, track, roads)
{
}

TrackLZN::~TrackLZN(void)
{
}

bool
TrackLZN::run(MuonFeatureDetails* det)
{
    int nit,nitmx=10;
    float x1,y1,x2,y2,x3,y3,x4,y4;
    float c0,c1,c2,c3,c22,c33,e2,e3,c2q,c3q,d,da,db,a,b;
    float dx,dy,x0,xn=0;
    //float phi1,phi2;
    float eps = 0.001;
    float y0  = 423.;                  // radius of calorimeter.
    
    const TriggerData& roi = m_trig_data;
    TrackFit& rpc_track    = m_track;
    RoadRegions& muon_roads = m_roads;
    
    rpc_track.isEndcap = false;

    rpc_track.Xcenter = 0.;
    rpc_track.Ycenter = 0.;
    rpc_track.Radius  = 0.;

    rpc_track.Type = roi.type - 1;
    
    if(roi.type==2) {               // fill superpoint value for High Pt trig.
      
        rpc_track.x1 = 0.;                    // A. Di Mattia version.
        rpc_track.y1 = 0.;
        rpc_track.x2 = roi.rpc1[2];
        rpc_track.y2 = roi.rpc1[3];
        rpc_track.x3 = roi.rpc3[2];
        rpc_track.y3 = roi.rpc3[3];
        x4 = roi.rpc2[2];
        y4 = roi.rpc2[3];
	
	setPhi(roi.rpc1[0],roi.rpc1[1],roi.rpc3[0],roi.rpc3[1]);
        //ap1 = roi.rpc3[1]/roi.rpc3[0];
        //ap2 = roi.rpc1[1]/roi.rpc1[0];
        //rpc_track.ap = (ap1+ap2)/2.;
	//phi1 = atan2(roi.rpc3[1],roi.rpc3[0]);
	//phi2 = atan2(roi.rpc1[1],roi.rpc1[0]);
	//rpc_track.phi = (phi1+phi2)/2.;
    } else {                         // fill superpoint value for Low Pt trig.
        rpc_track.x1 = 0.;
        rpc_track.y1 = 0.;
        rpc_track.x2 = roi.rpc1[2];
        rpc_track.y2 = roi.rpc1[3];
        rpc_track.x3 = roi.rpc2[2];
        rpc_track.y3 = roi.rpc2[3];
	
	setPhi(roi.rpc1[0],roi.rpc1[1],roi.rpc2[0],roi.rpc2[1]);
	
        //ap1 = roi.rpc2[1]/roi.rpc2[0];
        //ap2 = roi.rpc1[1]/roi.rpc1[0];
        //rpc_track.ap = (ap1+ap2)/2.;
	//phi1 = atan2(roi.rpc2[1],roi.rpc2[0]);
	//phi2 = atan2(roi.rpc1[1],roi.rpc1[0]);
	//rpc_track.phi = (phi1+phi2)/2.;
        x4 = rpc_track.x3;
        y4 = rpc_track.y3;
    }
    
   // rpc_track.aw1 = 0;
   // rpc_track.bw1 = 0;
   // rpc_track.aw2 = 0; 
   // rpc_track.bw2 = 0;  
   // rpc_track.aw3 = 0;
   // rpc_track.bw3 = 0;
    
    x1 = rpc_track.x1;
    y1 = rpc_track.y1;
    x2 = rpc_track.x2;
    y2 = rpc_track.y2;
    x3 = rpc_track.x3;
    y3 = rpc_track.y3;
    
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
        rpc_track.x1 = x1;
        rpc_track.y1 = y1;
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
    
    rpc_track.Xcenter = -a/2. + x1;
    rpc_track.Ycenter = -b/2. + y1;
    rpc_track.Radius  = sqrt(rpc_track.Xcenter*rpc_track.Xcenter + rpc_track.Ycenter*rpc_track.Ycenter);
    rpc_track.is = 1;
    if(a<=0.) rpc_track.is = -1;
    rpc_track.a2 = a/2.;
    rpc_track.aq = a*a;
    rpc_track.b = b;
    
    if(x4-x2) {
        muon_roads.aw2[0] = (y4-y2)/(x4-x2);
        muon_roads.bw2[0] = y2 - muon_roads.aw2[0]*x2;
    } else {
        muon_roads.aw2[0] = 0.;
        muon_roads.bw2[0] = x2;
    }
    
    muon_roads.aw2[1] = muon_roads.aw2[0];
    muon_roads.bw2[1] = muon_roads.bw2[0];
    
    if(det!=0) {
        det->setType(roi.type);
	det->setT_x_center(rpc_track.Xcenter);
        det->setT_y_center(rpc_track.Ycenter);
        det->setT_radius(rpc_track.Radius);
	
	det->setRpc1(roi.rpc1[0],roi.rpc1[1],roi.rpc1[2]);
	det->setRpc2(roi.rpc2[0],roi.rpc2[1],roi.rpc2[2]);
	det->setRpc3(roi.rpc3[0],roi.rpc3[1],roi.rpc3[2]);
	
	det->setAw(0,1,muon_roads.aw2[0]);
	det->setBw(0,1,muon_roads.bw2[0]);
	
	det->setAw(1,1,muon_roads.aw2[1]);
	det->setBw(1,1,muon_roads.bw2[1]);
	
	det->setT_ap(m_track.ap);
        det->setT_aphi(m_track.aphi);
        det->setT_phi(m_track.phi);
    }
    
    
    return true;
}
  
