/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/PtEstimate.h"
#include "TrigmuFast/MuLUT.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <math.h>

using namespace std;

const string PtEstimate::s_name = "Pt estimate";

PtEstimate::PtEstimate(MuMessageInterface& msg,
                       MuFeature& feature,
                       const MuLUT* mu_lut)
    : PtEstimateStd(msg,feature,mu_lut)
{
}

PtEstimate::~PtEstimate(void)
{
}

bool
PtEstimate::run(MuonFeatureDetails* det)
{
    const LUT& lut = m_lut->lut();

    int etabin,phibin,neweta,newphi,ch,add;
    float phistep,etastep,pstep,dist,distp,disteta,distphi;
    float A0[6]={0.,0.,0.,0.,0.,0.},A1[6]={0.,0.,0.,0.,0.,0.};
    
    if(m_feature.ok) {
        add = m_feature.Saddress;
        etabin = (int)((m_feature.EtaMap - lut.EtaMin[add])/lut.EtaStep[add]);
        phibin = (int)((m_feature.PhiMap - lut.PhiMin[add])/lut.PhiStep[add]);
	
	if(etabin<=-1) etabin = 0;
	if(etabin>=lut.NbinEta[add]) etabin = lut.NbinEta[add]-1;
	if(phibin<=-1) phibin = 0;
	if(phibin>=lut.NbinPhi[add]) phibin = lut.NbinPhi[add]-1;
	
        disteta = m_feature.EtaMap - (etabin*lut.EtaStep[add] +
                                     lut.EtaStep[add]/2. + lut.EtaMin[add]);
        distphi = m_feature.PhiMap - (phibin*lut.PhiStep[add] + 
                                     lut.PhiStep[add]/2. + lut.PhiMin[add]);
        neweta  = (disteta >= 0.) ? etabin+1 : etabin-1;
        newphi  = (distphi >= 0.) ? phibin+1 : phibin-1;
        etastep = (disteta >= 0.) ? lut.EtaStep[add] : -lut.EtaStep[add];
        phistep = (distphi >= 0.) ? lut.PhiStep[add] : -lut.PhiStep[add];
        if(m_feature.NSpoint>=3) {
            m_feature.Charge = m_feature.Sagitta/fabs(m_feature.Sagitta);
            ch = (m_feature.Charge>=0.)? 1 : 0;
            if(det!=0) det->setCharge(m_feature.Charge);
	    A0[0] = lut.table[add][ch][etabin][phibin][0];
            A1[0] = lut.table[add][ch][etabin][phibin][1];
            if((neweta<0||neweta>=lut.NbinEta[add])&&
               (newphi<0||newphi>=lut.NbinPhi[add])) {
                m_feature.Pt = (1/m_feature.Sagitta)*A0[0] + A1[0];
		if(det!=0) det->setPt(m_feature.Pt*m_feature.Charge);
                return true;
            } else if (neweta<0||neweta>=lut.NbinEta[add]) {
                A0[1] = lut.table[add][ch][etabin][newphi][0];
                A1[1] = lut.table[add][ch][etabin][newphi][1];
                A0[2] = A0[0] + ((A0[1] - A0[0])/phistep)*distphi;
                A1[2] = A1[0] + ((A1[1] - A1[0])/phistep)*distphi;
                m_feature.Pt = (1/m_feature.Sagitta)*A0[2] + A1[2];
		if(det!=0) det->setPt(m_feature.Pt*m_feature.Charge);
                return true;
            } else if (newphi<0||newphi>=lut.NbinPhi[add]) {
                A0[1] = lut.table[add][ch][neweta][phibin][0];
                A1[1] = lut.table[add][ch][neweta][phibin][1];
                A0[2] = A0[0] + ((A0[1] - A0[0])/etastep)*disteta;
                A1[2] = A1[0] + ((A1[1] - A1[0])/etastep)*disteta;
                m_feature.Pt = (1/m_feature.Sagitta)*A0[2] + A1[2];
		if(det!=0) det->setPt(m_feature.Pt*m_feature.Charge);
                return true;
            } else {
                if(disteta >= distphi*lut.EtaStep[add]/lut.PhiStep[add]) {
                    A0[1] = lut.table[add][ch][neweta][phibin][0];
                    A1[1] = lut.table[add][ch][neweta][phibin][1];
                    A0[2] = lut.table[add][ch][neweta][newphi][0];
                    A1[2] = lut.table[add][ch][neweta][newphi][1];
                    A0[3] = A0[0] + ((A0[1] - A0[0])/etastep)*disteta;
                    A1[3] = A1[0] + ((A1[1] - A1[0])/etastep)*disteta;
                    dist  = sqrt(phistep*phistep + etastep*etastep);
                    distp = sqrt(disteta*disteta + distphi*distphi);
                    A0[4] = A0[0] + ((A0[2] - A0[0])/dist)*distp;
                    A1[4] = A1[0] + ((A1[2] - A1[0])/dist)*distp;
                    pstep = (phistep/dist)*distp;
                    A0[5] = A0[3] + ((A0[4] - A0[3])/pstep)*distphi;
                    A1[5] = A1[3] + ((A1[4] - A1[3])/pstep)*distphi;
                    m_feature.Pt = (1/m_feature.Sagitta)*A0[5] + A1[5];
		    if(det!=0) det->setPt(m_feature.Pt*m_feature.Charge);
                    return true;
                } else {
                    A0[1] = lut.table[add][ch][etabin][newphi][0];
                    A1[1] = lut.table[add][ch][etabin][newphi][1];
                    A0[2] = lut.table[add][ch][neweta][newphi][0];
                    A1[2] = lut.table[add][ch][neweta][newphi][1];
                    A0[3] = A0[0] + ((A0[1] - A0[0])/phistep)*distphi;
                    A1[3] = A1[0] + ((A1[1] - A1[0])/phistep)*distphi;
                    dist  = sqrt(phistep*phistep + etastep*etastep);
                    distp = sqrt(disteta*disteta + distphi*distphi);
                    A0[4] = A0[0] + ((A0[2] - A0[0])/dist)*distp;
                    A1[4] = A1[0] + ((A1[2] - A1[0])/dist)*distp;
                    pstep = (etastep/dist)*distp;
                    A0[5] = A0[3] + ((A0[4] - A0[3])/pstep)*disteta;
                    A1[5] = A1[3] + ((A1[4] - A1[3])/pstep)*disteta;
                    m_feature.Pt = (1/m_feature.Sagitta)*A0[5] + A1[5];
		    if(det!=0) det->setPt(m_feature.Pt*m_feature.Charge);
                    return true;
                }
                //	m_feature.Pt = (1/m_feature.Sagitta)*
                //     lut.table[add][ch][etabin][phibin][0] +
                //	     lut.table[add][ch][etabin][phibin][1];
            }
        }
    }

    return true;
} 

  
