/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZmumuBasicTPEfficiencyPlots.cxx

#include "MuonPerformanceHistUtils/ZmumuTagProbeDileptonPlots.h"

ZmumuTagProbeDileptonPlots::ZmumuTagProbeDileptonPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
    m_isMatched(isMatched) {}

void ZmumuTagProbeDileptonPlots::initializePlots()
{
  m_ll = Book1D("m_ll","mll; m_{ll} [GeV]; Entries",80,71000.,111000.);
  m_opening_angle = Book1D("opening_angle","opening_angle; Dilepton opening angle; Entries",40,0.0,TMath::Pi());
  m_z_pt  = Book1D("z_pt","z_pt; Z boson p_{T} [GeV] ; Entries",100,0.0,400000.);
}

void ZmumuTagProbeDileptonPlots::fill(Probe& probe)
{
  if(m_isMatched && !probe.isMatched()) return;
  float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);
  TLorentzVector z = probe.probeTrack().p4() + probe.tagTrack().p4();
  m_ll->Fill(z.M(),sfweight );
  m_z_pt->Fill(z.Pt(),sfweight);
  m_opening_angle->Fill(fabs(probe.probeTrack().p4().Angle(probe.tagTrack().p4().Vect())),sfweight);

}
