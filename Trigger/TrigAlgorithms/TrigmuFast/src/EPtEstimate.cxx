/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/PtLUTSvc.h"
#include "TrigmuFast/EPtEstimate.h"
#include "TrigmuFast/MuLUT.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>
#include <sstream>
#include <stdarg.h>

using namespace std;

const string EPtEstimate::s_name = "Pt estimate";

EPtEstimate::EPtEstimate(MuMessageInterface& msg,
                         bool tilt_field,
                         const PtLUTSvc* LUTsvc,
                         MuFeature& feature,
                         const MuLUT* mu_lut)
        : PtEstimateStd(msg,feature,mu_lut), m_tilt_field(tilt_field), pPtLUTSvc(LUTsvc)
{}

EPtEstimate::~EPtEstimate(void)
{}

// CID 23035, 37: NOT_EMITTED, BAD_VA_START
// inline void display(MuMessageInterface* msg, const char* fmt, ...)
void display(MuMessageInterface* msg, const char* fmt, ...)
{
    char buf[512];
    va_list args;
    va_start(args,(char*)fmt);
    vsprintf(buf, fmt, args);
    msg->lock();
    msg->displayDebug(buf);
    msg->unlock();
}

bool EPtEstimate::run(MuonFeatureDetails* det)
{
    if (m_feature.etaBin < -1)
    {
        m_feature.ok = 0;
        return false;
    }

    //cout << "EPtEstimate::run initial m_feature.Pt=" << m_feature.Pt << ", m_feature.Charge=" << m_feature.Charge << endl;

    // use the TGC PT if the MDT fit is not available
    if (m_feature.slope == 0.0 && m_feature.intercept == 0.0) return true;

    int side = (m_feature.EtaMap <= 0.0) ? 0 : 1;
    int charge = (m_feature.intercept * m_feature.EtaMap) < 0.0 ? 1 : 0;
    float mdtPt = pPtLUTSvc->lookup(side, charge, PtLUTSvc::ALPHA, m_feature.etaBin,
                                    m_feature.phiBin, m_feature.alpha) / 1000.;
    if (charge == 0) mdtPt = -mdtPt;
    if (m_tilt_field) mdtPt = -mdtPt;
    //cout << "EPtEstimate::run mdtPt=" << mdtPt << endl;

    if (fabs(mdtPt) > 10.0 && m_feature.beta != 0.0)
    {
        float betaPt = pPtLUTSvc->lookup(side, charge, PtLUTSvc::BETA, m_feature.etaBin,
                                         m_feature.phiBin, m_feature.beta) / 1000.0;
        if (charge == 0) betaPt = -betaPt;
        if (m_tilt_field) betaPt = -betaPt;
        //cout << "EPtEstimate::run betaPt=" << betaPt << endl;
        // Skip diff test if no outer Z
        if (m_feature.Spoint[2][1] == 0.0 || fabs((betaPt - mdtPt) / mdtPt) < 0.5)
            mdtPt = betaPt;
    }

    if(mdtPt!=0.0)
    {
        m_feature.Pt = fabs(mdtPt);
        m_feature.Charge = mdtPt / fabs(m_feature.Pt);
    }
    //cout << "EPtEstimate::run m_feature.Pt=" << m_feature.Pt << ", m_feature.Charge=" << m_feature.Charge << endl;

    if(det!=0) {
        det->setCharge(m_feature.Charge);
        det->setPt(m_feature.Pt*m_feature.Charge);
    }
    
    return true;
}
