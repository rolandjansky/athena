/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuDetRegionEfficiencyPlots.cxx
 *
 *  Created on: Apr 8, 2015
 *      Author: goblirsc
 */

#include "MuonPerformanceHistUtils/ZmumuDetRegionEfficiencyPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/Muon.h"

ZmumuDetRegionEfficiencyPlots::ZmumuDetRegionEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
    m_isMatched(isMatched) {}

void ZmumuDetRegionEfficiencyPlots::initializePlots()
{
  detregions  = Book1D("DetRegions" ," DetRegions; Detector region; Entries ",(int)(EtaPhiBinning::nBINREGIONS),(int)(EtaPhiBinning::binUNKNOWN)-0.5, (int)(EtaPhiBinning::nBINREGIONS)-0.5);
  detregions_A  = Book1D("DetRegions_Aside" ," DetRegions; Detector region; Entries ",(int)(EtaPhiBinning::nBINREGIONS),(int)(EtaPhiBinning::binUNKNOWN)-0.5, (int)(EtaPhiBinning::nBINREGIONS)-0.5);
  detregions_C  = Book1D("DetRegions_Cside" ,"DetRegions; Detector region; Entries ",(int)(EtaPhiBinning::nBINREGIONS),(int)(EtaPhiBinning::binUNKNOWN)-0.5, (int)(EtaPhiBinning::nBINREGIONS)-0.5);
  
  for (int k = 1; k < detregions->GetNbinsX()+1;++k){
    std::string label = m_etaphi.GetRegionLabel(detregions->GetXaxis()->GetBinCenter(k));
    detregions->GetXaxis()->SetBinLabel(k, label.c_str());
    detregions_A->GetXaxis()->SetBinLabel(k, label.c_str());
    detregions_C->GetXaxis()->SetBinLabel(k, label.c_str());
  }
}

void ZmumuDetRegionEfficiencyPlots::fill(Probe& probe){
  if(m_isMatched && !probe.isMatched()) return;
  float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);

  TLorentzVector lv = probe.probeTrack().p4();

  detregions->Fill(m_etaphi.symmetricBin(lv), sfweight);
  if (fabs(lv.Eta()) > 0){
    detregions_A->Fill(m_etaphi.symmetricBin(lv), sfweight);
  }
  else {
    detregions_C->Fill(m_etaphi.symmetricBin(lv), sfweight);
  }
}
