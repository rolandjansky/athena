/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/TrackStd.h"
#include "TrigmuFast/MuData.h"
#include "TrigmuFast/Datatypes.h"
#include "MuonCablingTools/OutputDef.h"

#include <math.h>

#include <iomanip>

using namespace std;

const string TrackStd::s_type = "Track";

TrackStd::TrackStd(MuMessageInterface& msg,
                   const TriggerData& trig_data, TrackFit& track,
		   RoadRegions& roads)
    : m_msg(&msg),m_trig_data(trig_data), m_track(track) , m_roads (roads)
{
}

TrackStd::~TrackStd(void)
{
}

void
TrackStd::printout(void) const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[5000];
    for (int i=0;i<5000;++i) buffer[i] = '\0';
    __osstream display(buffer,5000);
#endif
    
    std::string test = m_msg->testString();
    
    display << test << name() << " started!" << endl;

    display.setf(ios::fixed);
    
    if(m_trig_data.isEndcap)
    {
        display << test << "Muon Pt estimated with TGC data = " 
	        << m_trig_data.tgcPT << endl;
    }
    else
    {
        if(m_trig_data.type==1) display << test << "  Low Pt trigger, ";
        else display << test << "  Hi Pt trigger, ";
        display << "RPC superpoints:" << endl;
        display << test << "   First superpoint    R=" << setw(9) << setprecision(2)
	        << m_track.y1 << "  Z=" << setw(9) << setprecision(2)
	        << m_track.x1 << endl;
        display << test << "   Second superpoint   R=" << setw(9) << setprecision(2)
	        << m_track.y2 << "  Z=" << setw(9) << setprecision(2)
	        << m_track.x2 << endl;
        display << test << "   Third superpoint    R=" << setw(9) << setprecision(2)
	        << m_track.y3 << "  Z=" << setw(9) << setprecision(2)
	        << m_track.x3 << endl;
        display << test << "   Track fit: Xcenter=" << setw(10) << setprecision(2)
	        << m_track.Xcenter << " ,  Ycenter=" << setw(10) << setprecision(2)
	        << m_track.Ycenter << ",  Radius=" << setw(10) << setprecision(2)
	        << m_track.Radius;
    }
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}

void 
TrackStd::setPhi(double X1, double Y1, double X2, double Y2)
{
    double phi1 = atan2(Y1,X1);
    double phi2 = atan2(Y2,X2);
    double rad1 = sqrt(X1*X1+Y1*Y1);
    double rad2 = sqrt(X2*X2+Y2*Y2);
    
    double ap1 = Y1/X1; 
    double ap2 = Y2/X2;
    m_track.ap = (ap1+ap2)/2.;
    
    setPhiDir(phi1,rad1,phi2,rad2);
}



void 
TrackStd::setPhiDir(double phi1, double X1, double phi2, double X2)
{
    if(X1==0. || X2==0.) 
    {
        m_track.isPhidir = false;
	if(X1!=0.) m_track.phi = phi1;
	if(X2!=0.) m_track.phi = phi2;
	return;
    }
    
    m_track.isPhidir = true;

    if( phi1*phi2 < 0 && fabsf(phi1)>M_PI/2. ) 
    {
        double tmp1 = (phi1>0)? phi1 - M_PI : phi1 + M_PI;
	double tmp2 = (phi2>0)? phi2 - M_PI : phi2 + M_PI;
	double tmp  = (tmp1+tmp2)/2.;
	m_track.aphi = (tmp2-tmp1)/fabsf(X2-X1);
	m_track.phi = (tmp>0.)? tmp - M_PI : tmp + M_PI;
    } 
    else 
    {
        m_track.aphi = (phi2-phi1)/fabsf(X2-X1);
	m_track.phi = (phi2+phi1)/2.;
    }
}

