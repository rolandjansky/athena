/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTrigTPEfficiencyPlots.cxx
 *
 *  Created on: Mar 2, 2015
 *      Author: goblirsc / m.a.
 */

#include "MuonPerformanceHistUtils/MuonTrigTPEfficiencyPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"
#include "Math/QuantFuncMathCore.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cctype>

using namespace std;


//----------------------------------------------------------------------
MuonTrigTPEfficiencyPlots::MuonTrigTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
    m_isMatched(isMatched)
{
    pt      = Book1D( "_pt"     ," pt; Pt, GeV; Entries / 1 GeV ",50,0.0,100.);
    eta     = Book1D("_eta"     ," eta; #eta; Entries ",50,-2.5,2.5);
    phi     = Book1D("_phi"     ," phi; #phi; Entries ",64,-TMath::Pi(),TMath::Pi());
    eta_phi = Book2D("_eta_phi" , "eta vs phi",50,-2.5,2.5,32,-TMath::Pi(),TMath::Pi());
    
    dRL1    = Book1D("_dRL1"    ," dRL1;   dR; Entries ",100,0,0.3);
    dRHLT   = Book1D("_dRHLT"   ," dRHLT;  dR; Entries ",100,0,0.001);
    mll     = Book1D("_mll"     ," mll;	GeV; Entries / 1 GeV ",100,50.0,130.);
}



//----------------------------------------------------------------------
float MuonTrigTPEfficiencyPlots::GetTrigThreshold_HLT(std::string str)
{
    std::string temp;
    float number=0;
    
    for (unsigned int i=0; i < str.size(); i++)
    {
        if (isdigit(str[i]))
        {
            for (unsigned int a=i; a<str.size(); a++)
            {
                temp += str[a];               
            }
          break;
        }    
    }  
    std::istringstream stream(temp);
    stream >> number;
    return number;
} 



//----------------------------------------------------------------------
float MuonTrigTPEfficiencyPlots::GetTrigThreshold_L1(std::string str)
{
    std::string temp;
    float number=0;
    
    for (unsigned int i=3; i < str.size(); i++)
    {
        if (isdigit(str[i]))
        {
            for (unsigned int a=i; a<str.size(); a++)
            {
                temp += str[a];               
            }
          break;
        }    
    }
    std::istringstream stream(temp);
    stream >> number;
    return number;
} 



//----------------------------------------------------------------------
void MuonTrigTPEfficiencyPlots::fill(Probe& probe)
{
    std::string CurrentTrigger = probe.GetCurrentTrigger();

    float TriggerThreshold = 1;
    if ( CurrentTrigger.find("L1_MU")   != std::string::npos) TriggerThreshold = this->GetTrigThreshold_L1(CurrentTrigger);
    if ( CurrentTrigger.find("L1_All")  != std::string::npos) TriggerThreshold = 1.;
    if ( CurrentTrigger.find("HLT_" )   != std::string::npos) TriggerThreshold = this->GetTrigThreshold_HLT(CurrentTrigger);
    if ( CurrentTrigger.find("HLT_All") != std::string::npos) TriggerThreshold = 1.;
   
    if(m_isMatched && !probe.isMatched()) return;
    float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);
    pt->Fill(probe.pt() / 1000.,sfweight);
    if ( probe.pt()/1000. > TriggerThreshold*1.05)
    {
        eta->Fill(probe.eta(),sfweight);
        phi->Fill(probe.phi(),sfweight);
        eta_phi->Fill(probe.eta(),probe.phi());

        dRL1->Fill(probe.dRL1,sfweight);
        dRHLT->Fill(probe.dRHLT,sfweight);
        TLorentzVector z = probe.probeTrack().p4() + probe.tagTrack().p4();
        mll->Fill(z.M() / 1000.,sfweight);
    }
}
