/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/TgcTrigStd.h"
#include "TrigmuFast/MuData.h"

#include <iomanip>

using namespace std;

const string TgcTrigStd::s_type = "TGC Trigger";

TgcTrigStd::TgcTrigStd(MuMessageInterface& msg,
                       __osstream* dis,
                       const TgcRdo* tgc_rdo,const LVL1::RecMuonRoI* roi,
                       TriggerData& out,
                       RawVec& tgc_raw_data,
                       LUTDigitVec& tgc_lut_digits,
                       const TGCgeometrySvc* pTgcLUTSvc,
                       MuServiceInterface* svc)
: m_msg(&msg),m_dis(dis),
m_pTgcLUTSvc(pTgcLUTSvc),m_svc(svc),m_pMuonRoI(roi),m_tgc_rdo(tgc_rdo),
m_tgc_data(out), m_tgc_raw_data(tgc_raw_data),m_tgc_lut_digits(tgc_lut_digits)
{
}

TgcTrigStd::~TgcTrigStd(void)
{
}

void
TgcTrigStd::printout(void) const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[50000];
    for (int i=0;i<50000;++i) buffer[i] = '\0';
    __osstream display(buffer,50000);
#endif

    display << name() << " started!" << endl;


    std::string test = m_msg->testString();

    if (m_tgc_data.type==1) display << test << "  Trigger n. " << m_tgc_data.triggerNum
        << ", Low Pt with threshold "
        << m_tgc_data.threshold << endl;
    else if (m_tgc_data.type==2) display << test << "  Trigger n. "
        << m_tgc_data.triggerNum
        << ", High Pt with threshold "  << m_tgc_data.threshold
        << endl << endl;
    else display << test << "No TGC trigger found!" << endl;

    char buf[1024];

    display << "TGC digits from LUT" << endl;
    sprintf(buf, "idx          z          r        eta        phi  width station strip?");
    display << buf << endl;
    for (unsigned int iLUT = 0; iLUT < m_tgc_lut_digits.size(); iLUT++)
    {
        sprintf(buf, "%3d %10.3f %10.3f %10.5f %10.5f %6.3f %7d  %s", iLUT+1,
            m_tgc_lut_digits[iLUT].z,
            m_tgc_lut_digits[iLUT].r,
            m_tgc_lut_digits[iLUT].eta,
            m_tgc_lut_digits[iLUT].phi,
            m_tgc_lut_digits[iLUT].width,
            m_tgc_lut_digits[iLUT].sta,
            m_tgc_lut_digits[iLUT].isStrip ? "Yes" : "No "
            );
        display << buf << endl;
    }

    display << test << "TGC fit: eta slope = " << m_tgc_data.slope
    << ",  eta intercept = "   << m_tgc_data.intercept << endl;



#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
    *m_dis  << ends
#endif

    m_msg->displayInfo(display.str());

    if (m_dis) m_msg->displayDebug(m_dis->str());
}
