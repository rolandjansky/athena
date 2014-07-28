/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/SagEstimateStd.h"
#include "TrigmuFast/Datatypes.h"
#include "MuonCablingTools/OutputDef.h"

#include <iomanip>
#include <cstring>

using namespace std;

const string SagEstimateStd::s_type = "Sagitta";

SagEstimateStd::SagEstimateStd(MuMessageInterface& msg,
                               const TriggerData& triggerData,
                               const TrackFit& track_fit,
                               const SpointData super_points[],
                               MuFeature& feature)
    : m_msg(&msg),m_trig_data(triggerData), m_track_fit(track_fit), 
      m_super_points(super_points), m_feature(feature)
{
    // reset feature
    memset(&m_feature, 0, sizeof(m_feature));
}

SagEstimateStd::~SagEstimateStd(void)
{
}

void
SagEstimateStd::printout(void) const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[200];
    for (int i=0;i<100;++i) buffer[i] = '\0';
    __osstream display(buffer,200);
#endif
    std::string test = m_msg->testString();
    
    display << test << name() << " started!" << endl;
    
    if(m_trig_data.isEndcap)
    {
        if(m_feature.ok)
	{
            display << test << "TGC slope     =" 
	            << setw(12) << setprecision(6) << m_trig_data.slope
	            << ",  MDT slope     =" << setw(12) << setprecision(6)
		    << m_feature.slope << endl;
            display << test << "TGC intercept =" 
	            << setw(12) << setprecision(6) << m_trig_data.intercept
	            << ",  MDT intercept =" << setw(12) << setprecision(6)
		    << m_feature.intercept << endl;
            display << test << "  Alpha  computed from MDT: " << setw(12) 
	            << setprecision(6) << m_feature.alpha << endl;
	    display << test << "  Beta   computed from MDT: " << setw(12) 
	            << setprecision(6) << m_feature.beta << endl;
	    display << test << "  DeltaR computed from MDT: " << setw(12)
	            << setprecision(6) << m_feature.deltaR << endl;
	    display << test << "  radius computed from MDT: " << setw(12)
	            << setprecision(6) << m_feature.Radius << endl;
	}
    }
    else
    {
        display << test << "  Sagitta "; 
        display.setf(ios::fixed);
        if(m_feature.ok) display << "esimated = " << setw(12) << setprecision(6)
	    		         << m_feature.Sagitta << " cm" << endl;
        else display << "NOT esimated." << endl;
        display << test << "  Radius ";
        display.setf(ios::fixed);
        if(m_feature.ok) display << "esimated = " << setw(12) << setprecision(6)
			         << m_feature.Radius << " cm" << endl;
        else display << "NOT esimated." << endl;
    }
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}

