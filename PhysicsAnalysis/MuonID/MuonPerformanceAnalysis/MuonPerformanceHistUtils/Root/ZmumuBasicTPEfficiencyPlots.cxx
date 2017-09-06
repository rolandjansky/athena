/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuBasicTPEfficiencyPlots.cxx
 *
 *  Created on: Sep 2, 2014
 *      Author: goblirsc
 */

#include "MuonPerformanceHistUtils/ZmumuBasicTPEfficiencyPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"

ZmumuBasicTPEfficiencyPlots::ZmumuBasicTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
    m_isMatched(isMatched) {  
}

void ZmumuBasicTPEfficiencyPlots::initializePlots()
{
  double ptbins[] = {10.,15.,20.,25.,30.,35.,40.,45.,50.,60.,70.,80.,90.,100.,120.};
  int nptbins = sizeof (ptbins) / sizeof (double) - 1;
  TH1D ptdummy ("ptdummy","dummy",nptbins,ptbins) ;
  m_pt   = Book1D("pt" ,&ptdummy, " pt; Transverse Momentum [GeV];Entries / 1 GeV");
  m_eta  = Book1D("eta" ," eta; #eta; Entries ",50,-2.5,2.5);
  m_phi  = Book1D("phi" ," phi; #phi; Entries ",32,-TMath::Pi(),TMath::Pi());
  m_integrated  = Book1D("integrated" ," integrated;  ; Entries ",1,0.5,1.5);
}


void ZmumuBasicTPEfficiencyPlots::fill(Probe& probe)
{
  if(m_isMatched && !probe.isMatched()) return;
  float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);
  m_pt->Fill(probe.pt() / 1000.,sfweight);
  m_eta->Fill(probe.eta() ,sfweight);
  m_phi->Fill(probe.phi() ,sfweight);
  m_integrated->Fill(1,sfweight);

}
