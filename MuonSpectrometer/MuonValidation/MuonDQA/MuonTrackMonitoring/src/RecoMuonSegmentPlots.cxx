/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonSegmentPlots.h"
#include "MuonIdHelpers/MuonStationIndex.h"


RecoMuonSegmentPlots::RecoMuonSegmentPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)
  ,  m_oMuonSegmentPlots(this,"/")
  //declare the histograms
{}

//when the plot function called with a Muon Container
//loop through each muon, get the corresponding link and fill it
void RecoMuonSegmentPlots::fill(const xAOD::MuonContainer& muContainer){
  for (const auto mu : muContainer) {
    fill(*mu);
  }
}

//when the plot function called with a Muon
//get's the corresponding link and fill it
void RecoMuonSegmentPlots::fill(const xAOD::Muon& mu){

  const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments = mu.muonSegmentLinks();
  for(const auto Mu_Segment : Mu_Segments){
    const ElementLink<DataVector<xAOD::MuonSegment_v1>> Mu_seg = Mu_Segment;
    if(Mu_seg.isValid()){
      const xAOD::MuonSegment* seg = *Mu_seg;
      fill(*seg);
    }
  }
}

void RecoMuonSegmentPlots::fill(const xAOD::MuonSegment& muonSeg) {
  //General Plots
  m_oMuonSegmentPlots.fill(muonSeg);
}

void RecoMuonSegmentPlots::initializePlots()
{
  //initialize temp plots
}

