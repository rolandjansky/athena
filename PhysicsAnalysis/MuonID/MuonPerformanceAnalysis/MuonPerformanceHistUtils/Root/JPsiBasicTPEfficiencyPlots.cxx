/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPSelectionTool.cxx
 *
 *  Created on: Dec 22, 2014
 *  Adapted for J/Psi by Maximiliano Sioli
 *
 */

#include "MuonPerformanceHistUtils/JPsiBasicTPEfficiencyPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"

JPsiBasicTPEfficiencyPlots::JPsiBasicTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
    m_isMatched(isMatched) {}
    

void JPsiBasicTPEfficiencyPlots::initializePlots()
{
  // 23 variable-size bins in pt  
  double ptbins[] = {0., 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7., 7.5, 8., 9., 10., 12., 14., 16., 18., 20.};
  
  int nptbins = sizeof (ptbins) / sizeof (double) - 1;
  TH1D ptdummy ("ptdummy","dummy",nptbins,ptbins) ;
  m_pt  = Book1D("pt" ,&ptdummy, " pt; Transverse Momentum [GeV];Entries / 1 GeV");
  
  // 25 fixed-size bins in eta
  m_eta  = Book1D("eta" ," eta; #eta; Entries ",25,-2.5,2.5);
  
  // 25 fixed-size bins in phi
  m_phi  = Book1D("phi" ," phi; #phi; Entries ",25,-TMath::Pi(),TMath::Pi());
  
  m_d0  = Book1D("d0" ," d0; d_{0} [mm]; Entries ",200,-10,10);
  m_integrated  = Book1D("integrated" ," integrated;  ; Entries ",1,0.5,1.5);
  m_fineEtaPhi  = Book1D("fineEtaPhi" ," fineEtaPhi; fine (#eta, #phi) bin; Entries ",m_fepb.nbins(),m_fepb.firstbin()-0.5, m_fepb.lastbin()+0.5);  
}


void JPsiBasicTPEfficiencyPlots::fill(Probe& probe)
{
  if(m_isMatched && !probe.isMatched()) return;
  float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);
  m_pt->Fill(probe.pt() / 1000.,sfweight);
  m_eta->Fill(probe.eta() ,sfweight);
  m_phi->Fill(probe.phi() ,sfweight);
  m_integrated->Fill(1,sfweight);

  //  case 1: ID track
  const xAOD::TrackParticle* trk = dynamic_cast<const xAOD::TrackParticle*>(&(probe.probeTrack()));
    
  //  in case that didn't work,  try a muon! 
  if (!trk) {
    const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*> (&(probe.probeTrack()));
    if (mu && mu->primaryTrackParticle()) trk = mu->primaryTrackParticle();
    if (!trk && mu && *(mu->inDetTrackParticleLink())) trk = *(mu->inDetTrackParticleLink());
  }
  if (trk) m_d0->Fill(trk->d0(),sfweight);
    

  m_fineEtaPhi->Fill(m_fepb.bin(probe.probeTrack().p4()),sfweight);

}
