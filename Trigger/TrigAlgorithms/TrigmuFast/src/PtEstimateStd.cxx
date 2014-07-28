/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/PtEstimateStd.h"
#include "TrigmuFast/Datatypes.h"
#include "MuonCablingTools/OutputDef.h"

#include <iomanip>

using namespace std;

const string PtEstimateStd::s_type = "Estimate";

PtEstimateStd::PtEstimateStd(MuMessageInterface& msg,
                             MuFeature& feature,
                             const MuLUT* mu_lut)
  : m_msg(&msg),m_feature(feature),m_lut(mu_lut)
{
}

PtEstimateStd::~PtEstimateStd(void)
{
}

void
PtEstimateStd::printout(void) const
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
    display << test << "  Features extracted:" << endl;

    if(m_feature.ok) {
        display << test << "    Pt     = " << setw(12) << setprecision(6)
		<< m_feature.Pt << " GeV" << endl;
	display << test << "    Eta    = " << setw(12) << setprecision(6)
		<< m_feature.EtaMap << endl;
	display << test << "    Phi    = " << setw(12) << setprecision(6)
		<< m_feature.phi << endl;
	display << test << "    PhiMap = " << setw(12) << setprecision(6)
		<< m_feature.PhiMap << endl;
	display << test << "    Charge = " << setw(12) << setprecision(6)
		<< m_feature.Charge << endl;
    } else display << test <<"    no features extracted!" << endl;

#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}

