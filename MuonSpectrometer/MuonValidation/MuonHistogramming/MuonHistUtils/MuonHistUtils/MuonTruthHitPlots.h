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
 
  TH1* precMatchedHitsSummed{nullptr};
  TH1* precMatchedHitsBIS{nullptr};
  TH1* precMatchedHitsBIL{nullptr};
  TH1* precMatchedHitsBMS{nullptr};
  TH1* precMatchedHitsBML{nullptr};
  TH1* precMatchedHitsBOS{nullptr};
  TH1* precMatchedHitsBOL{nullptr};
  TH1* precMatchedHitsBEE{nullptr};
  TH1* precMatchedHitsEIS{nullptr};
  TH1* precMatchedHitsEIL{nullptr};
  TH1* precMatchedHitsEMS{nullptr};
  TH1* precMatchedHitsEML{nullptr};
  TH1* precMatchedHitsEOS{nullptr};
  TH1* precMatchedHitsEOL{nullptr};
  TH1* precMatchedHitsEES{nullptr};
  TH1* precMatchedHitsEEL{nullptr};
  TH1* precMatchedHitsCSS{nullptr};
  TH1* precMatchedHitsCSL{nullptr};

  TH1* phiMatchedHitsSummed{nullptr};
  TH1* phiMatchedHitsBM1{nullptr};
  TH1* phiMatchedHitsBM2{nullptr};
  TH1* phiMatchedHitsBO1{nullptr};
  TH1* phiMatchedHitsBO2{nullptr};
  TH1* phiMatchedHitsT1{nullptr};
  TH1* phiMatchedHitsT2{nullptr};
  TH1* phiMatchedHitsT3{nullptr};
  TH1* phiMatchedHitsT4{nullptr};
  TH1* phiMatchedHitsCSC{nullptr};

  TH1* trigEtaMatchedHitsSummed{nullptr};
  TH1* trigEtaMatchedHitsBM1{nullptr};
  TH1* trigEtaMatchedHitsBM2{nullptr};
  TH1* trigEtaMatchedHitsBO1{nullptr};
  TH1* trigEtaMatchedHitsBO2{nullptr};
  TH1* trigEtaMatchedHitsT1{nullptr};
  TH1* trigEtaMatchedHitsT2{nullptr};
  TH1* trigEtaMatchedHitsT3{nullptr};
  TH1* trigEtaMatchedHitsT4{nullptr};

 
};
}

#endif
