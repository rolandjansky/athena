/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/TrackTB.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string TrackTB::s_name = "RPC track fit for TestBeam";

TrackTB::TrackTB(MuMessageInterface& msg,
                   const TriggerData& trig_data, TrackFit& track,
		   RoadRegions& roads)
    : TrackStd(msg,trig_data, track, roads)
{
}

TrackTB::~TrackTB(void)
{
}

bool
TrackTB::run(MuonFeatureDetails* det)
{
    
    const TriggerData& roi  = m_trig_data;
    TrackFit& rpc_track     = m_track;
    RoadRegions& muon_roads = m_roads;

    rpc_track.isEndcap = false;

    rpc_track.Xcenter = -99999.;
    rpc_track.Ycenter = -99999.;
    rpc_track.Radius  = 0.;
    rpc_track.is      = 0;
    rpc_track.a2      = 0;
    rpc_track.aq      = 0;
    rpc_track.b       = 0;
    rpc_track.Type = roi.type - 1;
        	
    if(roi.type==2) {               // fill superpoint value for High Pt trig.
      
        rpc_track.x1 = roi.rpc1[2];
        rpc_track.y1 = roi.rpc1[3];
        rpc_track.x2 = roi.rpc2[2];
        rpc_track.y2 = roi.rpc2[3];
        rpc_track.x3 = roi.rpc3[2];
        rpc_track.y3 = roi.rpc3[3];
	
	muon_roads.aw2[0] =(rpc_track.y2-rpc_track.y1)/(rpc_track.x2-rpc_track.x1);
        muon_roads.bw2[0] = rpc_track.y1 - muon_roads.aw2[0]*rpc_track.x1;
	muon_roads.aw3[0] =(rpc_track.y3-rpc_track.y1)/(rpc_track.x3-rpc_track.x1);
        muon_roads.bw3[0] = rpc_track.y1 - muon_roads.aw3[0]*rpc_track.x1;
	muon_roads.aw1[0] = muon_roads.aw3[0];
        muon_roads.bw1[0] = muon_roads.bw3[0];
	
        //float ap1 = (roi.rpc2[1] - roi.rpc1[1])/(rpc_track.y2 - rpc_track.y1);
        //float ap2 = (roi.rpc3[1] - roi.rpc2[1])/(rpc_track.y3 - rpc_track.y2);
	
	setPhi(roi.rpc1[0],roi.rpc1[1],roi.rpc2[0],roi.rpc2[1]);
	
	//float ap1 = roi.rpc2[1]/roi.rpc2[0];
        //float ap2 = roi.rpc1[1]/roi.rpc1[0];
        //rpc_track.ap = (ap1+ap2)/2.;
	//float phi1 = atan2(roi.rpc2[1],roi.rpc2[0]);
        //float phi2 = atan2(roi.rpc1[1],roi.rpc1[0]);
        //rpc_track.phi = (phi1+phi2)/2.;
    
    } else {                         // fill superpoint value for Low Pt trig.
    
        rpc_track.x1 = roi.rpc1[2];
        rpc_track.y1 = roi.rpc1[3];
        rpc_track.x2 = roi.rpc2[2];
        rpc_track.y2 = roi.rpc2[3];
        rpc_track.x3 = 0.;
        rpc_track.y3 = 0.;
	
	muon_roads.aw2[0] =(rpc_track.y2-rpc_track.y1)/(rpc_track.x2-rpc_track.x1);
        muon_roads.bw2[0] = rpc_track.y1 - muon_roads.aw2[0]*rpc_track.x1;
        muon_roads.aw1[0] = muon_roads.aw2[0];
        muon_roads.bw1[0] = muon_roads.bw2[0];
	muon_roads.aw3[0] = muon_roads.aw2[0];
        muon_roads.bw3[0] = muon_roads.bw2[0];
	
        //float ap1 = (roi.rpc2[1] - roi.rpc1[1])/(rpc_track.y2 - rpc_track.y1);
        //float ap2 = (roi.rpc2[1] - roi.rpc1[1])/(rpc_track.y2 - rpc_track.y1);

	setPhi(roi.rpc1[0],roi.rpc1[1],roi.rpc2[0],roi.rpc2[1]);
	
	//float ap1 = roi.rpc2[1]/roi.rpc2[0];
        //float ap2 = roi.rpc1[1]/roi.rpc1[0];
        //rpc_track.ap = (ap1+ap2)/2.;
	//float phi1 = atan2(roi.rpc2[1],roi.rpc2[0]);
        //float phi2 = atan2(roi.rpc1[1],roi.rpc1[0]);
        //rpc_track.phi = (phi1+phi2)/2.;
    }
    
    muon_roads.aw1[1] = muon_roads.aw1[0];
    muon_roads.bw1[1] = muon_roads.bw1[0];
    muon_roads.aw2[1] = muon_roads.aw2[0];
    muon_roads.bw2[1] = muon_roads.bw2[0];
    muon_roads.aw3[1] = muon_roads.aw3[0];
    muon_roads.bw3[1] = muon_roads.bw3[0];

    if(det!=0) {
        det->setType(roi.type);
	det->setT_x_center(rpc_track.Xcenter);
        det->setT_y_center(rpc_track.Ycenter);
        det->setT_radius(rpc_track.Radius);
	
	det->setRpc1(roi.rpc1[0],roi.rpc1[1],roi.rpc1[2]);
	det->setRpc2(roi.rpc2[0],roi.rpc2[1],roi.rpc2[2]);
	det->setRpc3(roi.rpc3[0],roi.rpc3[1],roi.rpc3[2]);
	
	det->setAw(0,0,muon_roads.aw1[0]);
	det->setBw(0,0,muon_roads.bw1[0]);
	det->setAw(0,1,muon_roads.aw2[0]);
	det->setBw(0,1,muon_roads.bw2[0]);
	det->setAw(0,2,muon_roads.aw3[0]);
	det->setBw(0,2,muon_roads.bw3[0]);
	
	det->setAw(1,0,muon_roads.aw1[1]);
	det->setBw(1,0,muon_roads.bw1[1]);
	det->setAw(1,1,muon_roads.aw2[1]);
	det->setBw(1,1,muon_roads.bw2[1]);
	det->setAw(1,2,muon_roads.aw3[1]);
	det->setBw(1,2,muon_roads.bw3[1]);
	
	det->setT_ap(m_track.ap);
        det->setT_aphi(m_track.aphi);
        det->setT_phi(m_track.phi);
    }

    return true;
}
  
