/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/StationFitStd.h"
#include "MuonCablingTools/OutputDef.h"

#include <stdio.h>
#include <iomanip>
#include <cstring>

using namespace std;

const string StationFitStd::s_type = "StationFit";

StationFitStd::StationFitStd(MuMessageInterface& msg,
                             const TrackFit& track_fit,
                             LayerDigits (*digits)[MAXLAYER],
                             SpointData super_points[])
    : m_msg(&msg),m_track_fit(track_fit), m_digits(digits),
      m_super_points(super_points)
{
    //reset Super Points
    for(int st=0;st<MAXSTATION;st++)
    {
        memset(&(m_super_points[st]), 0, sizeof(SpointData)); 
    }
}


StationFitStd::~StationFitStd(void)
{
}

void StationFitStd::printout(void) const
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
    display << test << "  Superpoint fitted:" << endl;
    display.setf(ios::fixed);
    for (int st=0;st<MAXSTATION;++st) {
      display << test << "   station=" << st+1 << "   superpoint -> R=" << setw(8)
	      << setprecision(3) << m_super_points[st].R << "  Z="
	      << setw(9) << setprecision(3) << m_super_points[st].Z
	      << "  Chi2=" << setw(9) << setprecision(3) 
              << m_super_points[st].Chi2 << "  Npoint=" 
              << m_super_points[st].Npoint << endl;
    }
    
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif    
    
    m_msg->displayInfo(display.str());
}
