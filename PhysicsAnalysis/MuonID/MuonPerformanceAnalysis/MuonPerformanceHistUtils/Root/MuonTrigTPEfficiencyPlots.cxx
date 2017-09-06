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

    double ptbins[] = {0.,5.,10.,15.,20.,25.,30.,35.,40.,45.,50.,55.,60.,65.,70.,75,80.,85,90.,95,100.,120.,140.,160.,180.,200.};
    int nptbins = sizeof (ptbins) / sizeof (double) - 1;
    TH1D ptdummy ("ptdummy","dummy",nptbins,ptbins) ;
    m_pt      = Book1D("pt" ,&ptdummy, " Large Pt; Muon Transverse Momentum [GeV];Entries / 1 GeV");
    m_eta     = Book1D("eta"     ," eta; #eta; Entries ",50,-2.5,2.5);
    m_phi     = Book1D("phi"     ," phi; #phi; Entries ",64,-TMath::Pi(),TMath::Pi());
    m_eta_phi = Book2D("eta_phi" , "eta vs phi",50,-2.5,2.5,32,-TMath::Pi(),TMath::Pi());
 
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
    m_pt->Fill(probe.pt() / 1000.,sfweight);
    if ( probe.pt()/1000. > TriggerThreshold*1.05)
    {
        m_eta->Fill(probe.eta(),sfweight);
        if(CurrentTrigger=="HLT_mu60_0eta105_msonly")
        {
            if(fabs(probe.eta())<=1.05)
                m_phi->Fill(probe.phi(),sfweight);
        }
        else
        {
            m_phi->Fill(probe.phi(),sfweight);
        }
        m_eta_phi->Fill(probe.eta(),probe.phi());
    }
}
