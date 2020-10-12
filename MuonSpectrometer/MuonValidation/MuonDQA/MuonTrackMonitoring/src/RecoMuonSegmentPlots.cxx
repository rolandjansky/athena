/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonSegmentPlots.h"

RecoMuonSegmentPlots::RecoMuonSegmentPlots(PlotBase* pParent, std::string sDir, bool detail):PlotBase(pParent, sDir)
  ,  m_oMuonSegmentPlots(nullptr)
  ,  m_oMuonSegmentSlimPlots(nullptr)
  //declare the histograms
{
  Detail = detail;
  if(Detail) m_oMuonSegmentPlots = new Muon::MuonSegmentPlots(this,"/");
  else m_oMuonSegmentSlimPlots = new Muon::MuonSegmentSlimPlots(this,"/");
}

//when the plot function called with a Muon Container
//loop through each muon, get the corresponding link and fill it
void RecoMuonSegmentPlots::fill(const xAOD::MuonContainer& muContainer){
  for (const auto mu : muContainer) {
    fill(*mu);
  }
}

//when the plot function called with a Muon
//get's the corresponding link and fill it
//Tony 2015.9.21: fix to go around the segment link problem for now
void RecoMuonSegmentPlots::fill(const xAOD::Muon& mu){
  const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments = mu.muonSegmentLinks();
  fill(Mu_Segments);
}

//use the link to fill
void RecoMuonSegmentPlots::fill(const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments){

  for(const auto& Mu_Segment : Mu_Segments){
    const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
    if(Mu_seg.isValid()){
      const xAOD::MuonSegment* seg = *Mu_seg;
      fill(*seg);
    }
  }
}


void RecoMuonSegmentPlots::fill(const xAOD::MuonSegment& muonSeg) {
  //General Plots
  if (Detail) m_oMuonSegmentPlots->fill(muonSeg);
  else m_oMuonSegmentSlimPlots->fill(muonSeg);
}

void RecoMuonSegmentPlots::initializePlots()
{
  //initialize temp plots
}

