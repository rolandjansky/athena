/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONTRUTHHITPLOTS_H
#define MUONHISTUTILS_MUONTRUTHHITPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"

namespace Muon{
  
class MuonTruthHitPlots:public PlotBase {
 public:
  MuonTruthHitPlots(PlotBase* pParent, const std::string& sDir);  
  void fill(const xAOD::Muon& muon, float weight=1.0);
  void fillPlot(TH1* hitPlots, const std::string& sInfo, const Muon::MuonStationIndex::ChIndex& index, const xAOD::Muon& muon, float weight=1.0);
  void fillPlot(TH1* hitPlots, const std::string& sInfo, const Muon::MuonStationIndex::PhiIndex& index, const xAOD::Muon& muon, float weight=1.0);
  void fillPlot(TH1* hitPlots, const std::string& sInfo, const xAOD::Muon& muon, float weight=1.0);
 
  TH1* precMatchedHitsSummed;
  TH1* precMatchedHitsBIS;
  TH1* precMatchedHitsBIL;
  TH1* precMatchedHitsBMS;
  TH1* precMatchedHitsBML;
  TH1* precMatchedHitsBOS;
  TH1* precMatchedHitsBOL;
  TH1* precMatchedHitsBEE;
  TH1* precMatchedHitsEIS;
  TH1* precMatchedHitsEIL;
  TH1* precMatchedHitsEMS;
  TH1* precMatchedHitsEML;
  TH1* precMatchedHitsEOS;
  TH1* precMatchedHitsEOL;
  TH1* precMatchedHitsEES;
  TH1* precMatchedHitsEEL;
  TH1* precMatchedHitsCSS;
  TH1* precMatchedHitsCSL;

  TH1* phiMatchedHitsSummed;
  TH1* phiMatchedHitsBM1;
  TH1* phiMatchedHitsBM2;
  TH1* phiMatchedHitsBO1;
  TH1* phiMatchedHitsBO2;
  TH1* phiMatchedHitsT1;
  TH1* phiMatchedHitsT2;
  TH1* phiMatchedHitsT3;
  TH1* phiMatchedHitsT4;
  TH1* phiMatchedHitsCSC;

  TH1* trigEtaMatchedHitsSummed;
  TH1* trigEtaMatchedHitsBM1;
  TH1* trigEtaMatchedHitsBM2;
  TH1* trigEtaMatchedHitsBO1;
  TH1* trigEtaMatchedHitsBO2;
  TH1* trigEtaMatchedHitsT1;
  TH1* trigEtaMatchedHitsT2;
  TH1* trigEtaMatchedHitsT3;
  TH1* trigEtaMatchedHitsT4;
  // TH1* trigEtaMatchedHitsCSC;

 
};
}

#endif
