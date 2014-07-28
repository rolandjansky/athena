/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/TrackTgc.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuServiceInterface.h"
#include "TrigmuFast/PtLUTSvc.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

double const PI_OVER_4 = M_PI / 4.0;
double const PI_OVER_8 = M_PI / 8.0;
double const PHI_RANGE = 12.0 / PI_OVER_8;

using namespace std;

const string TrackTgc::s_name = "TGC track Pt estimate";

TrackTgc::TrackTgc(MuMessageInterface& msg, const MuServiceInterface* svc,
                   bool tilt_field, bool use_pol2,
                   const TriggerData& trig_data, TrackFit& track,
		   RoadRegions& roads)
    : TrackStd(msg, trig_data, track, roads),
      m_svc(svc),m_tilt_field(tilt_field),m_use_pol2(use_pol2)
{
}

TrackTgc::~TrackTgc(void)
{
}

bool
TrackTgc::run(MuonFeatureDetails* det)
{
    
    const TriggerData& roi  = m_trig_data;
    TrackFit& tgc_track     = m_track;
    RoadRegions& muon_roads = m_roads;

    tgc_track.isEndcap = true;

    tgc_track.Xcenter = -99999.;
    tgc_track.Ycenter = -99999.;
    
    int side = (roi.tgcMid2[3]<=0) ? 0 : 1;
    double alpha = m_svc->ptLUTSvc()->alpha(roi.tgcMid1[3], roi.tgcMid1[2],
                                    roi.tgcMid2[3], roi.tgcMid2[2]);
    int Octant = (int)(roi.tgcMid1[1] / PI_OVER_4);
    double PhiInOctant = fabs(roi.tgcMid1[1] - Octant * PI_OVER_4);
    if (PhiInOctant > PI_OVER_8) PhiInOctant = PI_OVER_4 - PhiInOctant;
    
    int phiBin = static_cast<int>(PhiInOctant * PHI_RANGE);
    int etaBin = static_cast<int>((fabs(roi.tgcMid1[0]) - 1.)/0.05);
      
    int charge = (roi.intercept * roi.tgcMid2[3]) < 0.0 ? 1 : 0;
    float tgcPt = 0;
    if( m_use_pol2 ) {
       tgcPt = m_svc->ptLUTSvc()->lookup(side, charge, PtLUTSvc::ALPHAPOL2, etaBin, phiBin, alpha) / 1000.;
    }
    else {
       tgcPt = m_svc->ptLUTSvc()->lookup(side, charge, PtLUTSvc::ALPHA,     etaBin, phiBin, alpha) / 1000.;
    }
    if (charge == 0) tgcPt = -tgcPt;
    if (m_tilt_field) tgcPt = -tgcPt;
    //cout << "TrackTgc::run roi.intercept=" << roi.intercept
    //     << ", roi.tgcMid2[3]=" << roi.tgcMid2[3]
    //     << ", charge=" << charge
    //     << ", m_tilt_field=" << m_tilt_field
    //     << ", tgcPt=" << tgcPt << endl;
        
    // breaking the constantness of TriggerData  
    (const_cast<TriggerData&>(roi)).tgcPT = tgcPt;
    
    setPhiDir(roi.tgcMid1[1], roi.tgcMid1[3], roi.tgcMid2[1], roi.tgcMid2[3]);
    
    float X1 = roi.tgcMid1[3] * cos(roi.tgcMid1[1]);
    float Y1 = roi.tgcMid1[3] * sin(roi.tgcMid1[1]);
    float X2 = roi.tgcMid2[3] * cos(roi.tgcMid2[1]);
    float Y2 = roi.tgcMid2[3] * sin(roi.tgcMid2[1]);
    m_track.ap = (Y1/X1 + Y2/X2)/2.;
    
    if(roi.tgcInn[3] ==0)
    {
        // use the back extrapolator to retrieve the Etain the Innermost
       double etaMiddle = (roi.tgcMid1[3])? roi.tgcMid1[0] : roi.tgcMid2[0];
       double phiMiddle = (roi.tgcMid1[3])? roi.tgcMid1[1] : roi.tgcMid2[1];
       double eta;
       double sigma_eta;
       if( m_svc->etaInner(-1, etaMiddle, phiMiddle, tgcPt, eta, sigma_eta) )
       {
            tgc_track.extrInnerEta = eta;
       } else tgc_track.extrInnerEta = etaMiddle;
    } else tgc_track.extrInnerEta = 9999.;
    
    if(roi.tgcInn[3] !=0)
    {
        muon_roads.aw1[0] = roi.tgcInn[2]/roi.tgcInn[3];
    } 
    else 
    {
        if (tgc_track.extrInnerEta != 0.)
	{
            double theta = atan(exp(-fabs(tgc_track.extrInnerEta)))*2.;
			
            muon_roads.aw1[0] = tan(theta)*(fabs(tgc_track.extrInnerEta)/
	                                         tgc_track.extrInnerEta);
        } else muon_roads.aw1[0] = 0;
    }
    
    muon_roads.bw1[0] = 0.;
    muon_roads.aw2[0] = roi.slope;
    muon_roads.bw2[0] = roi.intercept;
    muon_roads.aw3[0] = roi.slope;
    muon_roads.bw3[0] = roi.intercept;
    
    muon_roads.aw1[1] = muon_roads.aw1[0];
    muon_roads.bw1[1] = muon_roads.bw1[0];
    muon_roads.aw2[1] = muon_roads.aw2[0];
    muon_roads.bw2[1] = muon_roads.bw2[0];
    muon_roads.aw3[1] = muon_roads.aw3[0];
    muon_roads.bw3[1] = muon_roads.bw3[0];
    
    if(det!=0) {
        det->setType(roi.type);
	
	det->setTgcPt(m_trig_data.tgcPT);

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
	
        det->setT_aphi(m_track.aphi);
        det->setT_phi(m_track.phi);
    }
    
    return true;
}
  
