/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_ANTracking.h"
// #include "TrkValHistUtils/ANTrackingPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetPerfPlot_ANTracking::InDetPerfPlot_ANTracking(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir)
{
  // nop
}

void
InDetPerfPlot_ANTracking::initializePlots() {

  book(m_effANT_eta, "efficiencySplit_vs_eta", "efficiencyANT_vs_eta");
  book(m_effSTD_eta, "efficiencySplit_vs_eta", "efficiencySTD_vs_eta");
  book(m_effBAT_eta, "efficiencySplit_vs_eta", "efficiencyBAT_vs_eta");
  book(m_effANT_pt, "efficiencySplit_vs_pt", "efficiencyANT_vs_pt");
  book(m_effSTD_pt, "efficiencySplit_vs_pt", "efficiencySTD_vs_pt");
  book(m_effBAT_pt, "efficiencySplit_vs_pt", "efficiencyBAT_vs_pt");

  book(m_effANT_phi, "efficiencySplit_vs_phi", "efficiencyANT_vs_phi");
  book(m_effSTD_phi, "efficiencySplit_vs_phi", "efficiencySTD_vs_phi");
  book(m_effBAT_phi, "efficiencySplit_vs_phi", "efficiencyBAT_vs_phi");

  book(m_effANT_d0, "efficiencySplit_vs_d0", "efficiencyANT_vs_d0");
  book(m_effSTD_d0, "efficiencySplit_vs_d0", "efficiencySTD_vs_d0");
  book(m_effBAT_d0, "efficiencySplit_vs_d0", "efficiencyBAT_vs_d0");

  book(m_effANT_z0, "efficiencySplit_vs_z0", "efficiencyANT_vs_z0");
  book(m_effSTD_z0, "efficiencySplit_vs_z0", "efficiencySTD_vs_z0");
  book(m_effBAT_z0, "efficiencySplit_vs_z0", "efficiencyBAT_vs_z0");

  book(m_effANT_radius, "efficiencySplit_vs_radius", "efficiencyANT_vs_radius");
  book(m_effSTD_radius, "efficiencySplit_vs_radius", "efficiencySTD_vs_radius");
  book(m_effBAT_radius, "efficiencySplit_vs_radius", "efficiencyBAT_vs_radius");

  book(m_effANT_mu, "efficiencySplit_vs_mu", "efficiencyANT_vs_mu");
  book(m_effSTD_mu, "efficiencySplit_vs_mu", "efficiencySTD_vs_mu");
  book(m_effBAT_mu, "efficiencySplit_vs_mu", "efficiencyBAT_vs_mu");

  book(m_effANT_nvertices, "efficiencySplit_vs_nvertices", "efficiencyANT_vs_nvertices");
  book(m_effSTD_nvertices, "efficiencySplit_vs_nvertices", "efficiencySTD_vs_nvertices");
  book(m_effBAT_nvertices, "efficiencySplit_vs_nvertices", "efficiencyBAT_vs_nvertices");

  book(m_fakeANT_eta, "fakeSplit_vs_eta", "fakeANT_vs_eta");
  book(m_fakeSTD_eta, "fakeSplit_vs_eta", "fakeSTD_vs_eta");
  book(m_fakeBAT_eta, "fakeSplit_vs_eta", "fakeBAT_vs_eta");

  book(m_fakeANT_pt, "fakeSplit_vs_pt", "fakeANT_vs_pt");
  book(m_fakeSTD_pt, "fakeSplit_vs_pt", "fakeSTD_vs_pt");
  book(m_fakeBAT_pt, "fakeSplit_vs_pt", "fakeBAT_vs_pt");

  book(m_fakeANT_phi, "fakeSplit_vs_phi", "fakeANT_vs_phi");
  book(m_fakeSTD_phi, "fakeSplit_vs_phi", "fakeSTD_vs_phi");
  book(m_fakeBAT_phi, "fakeSplit_vs_phi", "fakeBAT_vs_phi");

  book(m_fakeANT_d0, "fakeSplit_vs_d0", "fakeANT_vs_d0");
  book(m_fakeSTD_d0, "fakeSplit_vs_d0", "fakeSTD_vs_d0");
  book(m_fakeBAT_d0, "fakeSplit_vs_d0", "fakeBAT_vs_d0");

  book(m_fakeANT_z0, "fakeSplit_vs_z0", "fakeANT_vs_z0");
  book(m_fakeSTD_z0, "fakeSplit_vs_z0", "fakeSTD_vs_z0");
  book(m_fakeBAT_z0, "fakeSplit_vs_z0", "fakeBAT_vs_z0");

  book(m_fakeANT_mu, "fakeSplit_vs_mu", "fakeANT_vs_mu");
  book(m_fakeSTD_mu, "fakeSplit_vs_mu", "fakeSTD_vs_mu");
  book(m_fakeBAT_mu, "fakeSplit_vs_mu", "fakeBAT_vs_mu");

  book(m_fakeANT_nvertices, "fakeSplit_vs_nvertices", "fakeANT_vs_nvertices");
  book(m_fakeSTD_nvertices, "fakeSplit_vs_nvertices", "fakeSTD_vs_nvertices");
  book(m_fakeBAT_nvertices, "fakeSplit_vs_nvertices", "fakeBAT_vs_nvertices");

  book(m_unlinkedANT_eta, "fakeSplit_vs_eta", "unlinkedANT_vs_eta");
  book(m_unlinkedSTD_eta, "fakeSplit_vs_eta", "unlinkedSTD_vs_eta");
  book(m_unlinkedBAT_eta, "fakeSplit_vs_eta", "unlinkedBAT_vs_eta");

  book(m_unlinkedANT_pt, "fakeSplit_vs_pt", "unlinkedANT_vs_pt");
  book(m_unlinkedSTD_pt, "fakeSplit_vs_pt", "unlinkedSTD_vs_pt");
  book(m_unlinkedBAT_pt, "fakeSplit_vs_pt", "unlinkedBAT_vs_pt");

  book(m_unlinkedANT_phi, "fakeSplit_vs_phi", "unlinkedANT_vs_phi");
  book(m_unlinkedSTD_phi, "fakeSplit_vs_phi", "unlinkedSTD_vs_phi");
  book(m_unlinkedBAT_phi, "fakeSplit_vs_phi", "unlinkedBAT_vs_phi");

  book(m_unlinkedANT_d0, "fakeSplit_vs_d0", "unlinkedANT_vs_d0");
  book(m_unlinkedSTD_d0, "fakeSplit_vs_d0", "unlinkedSTD_vs_d0");
  book(m_unlinkedBAT_d0, "fakeSplit_vs_d0", "unlinkedBAT_vs_d0");

  book(m_unlinkedANT_z0, "fakeSplit_vs_z0", "unlinkedANT_vs_z0");
  book(m_unlinkedSTD_z0, "fakeSplit_vs_z0", "unlinkedSTD_vs_z0");
  book(m_unlinkedBAT_z0, "fakeSplit_vs_z0", "unlinkedBAT_vs_z0");

  book(m_unlinkedANT_mu, "fakeSplit_vs_mu", "unlinkedANT_vs_mu");
  book(m_unlinkedSTD_mu, "fakeSplit_vs_mu", "unlinkedSTD_vs_mu");
  book(m_unlinkedBAT_mu, "fakeSplit_vs_mu", "unlinkedBAT_vs_mu");

  book(m_unlinkedANT_nvertices, "fakeSplit_vs_nvertices", "unlinkedANT_vs_nvertices");
  book(m_unlinkedSTD_nvertices, "fakeSplit_vs_nvertices", "unlinkedSTD_vs_nvertices");
  book(m_unlinkedBAT_nvertices, "fakeSplit_vs_nvertices", "unlinkedBAT_vs_nvertices");

  book(m_trkpropANT_eta, "trkpropSplit_vs_eta", "trkpropANT_vs_eta");
  book(m_trkpropBAT_eta, "trkpropSplit_vs_eta", "trkpropBAT_vs_eta");
  book(m_trkpropSTD_eta, "trkpropSplit_vs_eta", "trkpropSTD_vs_eta");
  
  book(m_trkpropANT_phi, "trkpropSplit_vs_phi", "trkpropANT_vs_phi");
  book(m_trkpropBAT_phi, "trkpropSplit_vs_phi", "trkpropBAT_vs_phi");
  book(m_trkpropSTD_phi, "trkpropSplit_vs_phi", "trkpropSTD_vs_phi");
  
  book(m_trkpropANT_pt, "trkpropSplit_vs_pt", "trkpropANT_vs_pt");
  book(m_trkpropBAT_pt, "trkpropSplit_vs_pt", "trkpropBAT_vs_pt");
  book(m_trkpropSTD_pt, "trkpropSplit_vs_pt", "trkpropSTD_vs_pt");
  
  book(m_trkpropANT_d0, "trkpropSplit_vs_d0", "trkpropANT_vs_d0");
  book(m_trkpropBAT_d0, "trkpropSplit_vs_d0", "trkpropBAT_vs_d0");
  book(m_trkpropSTD_d0, "trkpropSplit_vs_d0", "trkpropSTD_vs_d0");

  book(m_trkpropANT_z0, "trkpropSplit_vs_z0", "trkpropANT_vs_z0");
  book(m_trkpropBAT_z0, "trkpropSplit_vs_z0", "trkpropBAT_vs_z0");
  book(m_trkpropSTD_z0, "trkpropSplit_vs_z0", "trkpropSTD_vs_z0");

  book(m_ntrkANT_mu, "ntrkSplit_vs_mu", "ntrkANT_vs_mu");
  book(m_ntrkBAT_mu, "ntrkSplit_vs_mu", "ntrkBAT_vs_mu");
  book(m_ntrkSTD_mu, "ntrkSplit_vs_mu", "ntrkSTD_vs_mu");

  book(m_ntrkANT_nvertices, "ntrkSplit_vs_nvertices", "ntrkANT_vs_nvertices");
  book(m_ntrkBAT_nvertices, "ntrkSplit_vs_nvertices", "ntrkBAT_vs_nvertices");
  book(m_ntrkSTD_nvertices, "ntrkSplit_vs_nvertices", "ntrkSTD_vs_nvertices");


  book(m_nPixelHitsANT_vs_eta, "nPixelHits_vs_eta","nPixelHitsANT_vs_eta");
  book(m_nPixelHitsSTD_vs_eta, "nPixelHits_vs_eta","nPixelHitsBAT_vs_eta");
  book(m_nPixelHitsBAT_vs_eta, "nPixelHits_vs_eta","nPixelHitsSTD_vs_eta");

  book(m_nSCTHitsANT_vs_eta, "nSCTHits_vs_eta","nSCTHitsANT_vs_eta");
  book(m_nSCTHitsSTD_vs_eta, "nSCTHits_vs_eta","nSCTHitsBAT_vs_eta");
  book(m_nSCTHitsBAT_vs_eta, "nSCTHits_vs_eta","nSCTHitsSTD_vs_eta");

  book(m_nTRTHitsANT_vs_eta, "nTRTHits_vs_eta","nTRTHitsANT_vs_eta");
  book(m_nTRTHitsSTD_vs_eta, "nTRTHits_vs_eta","nTRTHitsBAT_vs_eta");
  book(m_nTRTHitsBAT_vs_eta, "nTRTHits_vs_eta","nTRTHitsSTD_vs_eta");

  book(m_nPixelHolesANT_vs_eta, "nPixelHoles_vs_eta","nPixelHolesANT_vs_eta");
  book(m_nPixelHolesSTD_vs_eta, "nPixelHoles_vs_eta","nPixelHolesBAT_vs_eta");
  book(m_nPixelHolesBAT_vs_eta, "nPixelHoles_vs_eta","nPixelHolesSTD_vs_eta");

  book(m_nSCTHolesANT_vs_eta, "nSCTHoles_vs_eta","nSCTHolesANT_vs_eta");
  book(m_nSCTHolesSTD_vs_eta, "nSCTHoles_vs_eta","nSCTHolesBAT_vs_eta");
  book(m_nSCTHolesBAT_vs_eta, "nSCTHoles_vs_eta","nSCTHolesSTD_vs_eta");

  book(m_nTRTHTHitsANT_vs_eta, "nTRTHighThresholdHits_vs_eta","nTRTHTHitsANT_vs_eta");
  book(m_nTRTHTHitsSTD_vs_eta, "nTRTHighThresholdHits_vs_eta","nTRTHTHitsBAT_vs_eta");
  book(m_nTRTHTHitsBAT_vs_eta, "nTRTHighThresholdHits_vs_eta","nTRTHTHitsSTD_vs_eta");


  book(m_nPixelSharedHitsANT_vs_eta, "nPixelSharedHits_vs_eta","nPixelSharedHitsANT_vs_eta");
  book(m_nPixelSharedHitsSTD_vs_eta, "nPixelSharedHits_vs_eta","nPixelSharedHitsBAT_vs_eta");
  book(m_nPixelSharedHitsBAT_vs_eta, "nPixelSharedHits_vs_eta","nPixelSharedHitsSTD_vs_eta");

  book(m_nSCTSharedHitsANT_vs_eta, "nSCTSharedHits_vs_eta","nSCTSharedHitsANT_vs_eta");
  book(m_nSCTSharedHitsSTD_vs_eta, "nSCTSharedHits_vs_eta","nSCTSharedHitsBAT_vs_eta");
  book(m_nSCTSharedHitsBAT_vs_eta, "nSCTSharedHits_vs_eta","nSCTSharedHitsSTD_vs_eta");




}

void 
InDetPerfPlot_ANTracking::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx) {
 
  const float undefinedValue = -9999;

  double eta = truth.eta();
  double pt = truth.pt() / Gaudi::Units::GeV; // convert MeV to GeV
  double phi = truth.phi();

  double d0 = truth.isAvailable<float>("d0") ? truth.auxdata<float>("d0") : undefinedValue;
  double z0 = truth.isAvailable<float>("z0") ? truth.auxdata<float>("z0") : undefinedValue;
  double radius =  truth.isAvailable<float>("prodR") ? truth.auxdata<float>("prodR") : undefinedValue;
 
  bool isBAT = false; bool isANT = false; bool isSTD = false;
  if(isGood){
    const std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo> &patternInfo{track.patternRecoInfo()};
    isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
    isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
    isSTD = not isBAT and not isANT;
 
    if(isANT){
      fillHisto(m_effANT_eta, eta, isGood);
      fillHisto(m_effANT_pt, pt, isGood);
      fillHisto(m_effANT_phi, phi, isGood);
      fillHisto(m_effANT_d0, d0, isGood);
      fillHisto(m_effANT_z0, z0, isGood);
      fillHisto(m_effANT_radius, radius, isGood);
      fillHisto(m_effANT_mu, mu, isGood);
      fillHisto(m_effANT_nvertices, nVtx, isGood);
    } else if(isSTD){
      fillHisto(m_effSTD_eta, eta, isGood);
      fillHisto(m_effSTD_pt, pt, isGood);
      fillHisto(m_effSTD_phi, phi, isGood);
      fillHisto(m_effSTD_d0, d0, isGood);
      fillHisto(m_effSTD_z0, z0, isGood);
      fillHisto(m_effSTD_radius, radius, isGood);
      fillHisto(m_effSTD_mu, mu, isGood);
      fillHisto(m_effSTD_nvertices, nVtx, isGood);
    } else if(isBAT){
      fillHisto(m_effBAT_eta, eta, isGood);
      fillHisto(m_effBAT_pt, pt, isGood);
      fillHisto(m_effBAT_phi, phi, isGood);
      fillHisto(m_effBAT_d0, d0, isGood);
      fillHisto(m_effBAT_z0, z0, isGood);
      fillHisto(m_effBAT_radius, radius, isGood);
      fillHisto(m_effBAT_mu, mu, isGood);
      fillHisto(m_effBAT_nvertices, nVtx, isGood);
    }

  } else {
    fillHisto(m_effANT_eta, eta, isGood);
    fillHisto(m_effANT_pt, pt, isGood);
    fillHisto(m_effANT_phi, phi, isGood);
    fillHisto(m_effANT_d0, d0, isGood);
    fillHisto(m_effANT_z0, z0, isGood);
    fillHisto(m_effANT_radius, radius, isGood);
    fillHisto(m_effANT_mu, mu, isGood);
    fillHisto(m_effANT_nvertices, nVtx, isGood);
    fillHisto(m_effSTD_eta, eta, isGood);
    fillHisto(m_effSTD_pt, pt, isGood);
    fillHisto(m_effSTD_phi, phi, isGood);
    fillHisto(m_effSTD_d0, d0, isGood);
    fillHisto(m_effSTD_z0, z0, isGood);
    fillHisto(m_effSTD_radius, radius, isGood);
    fillHisto(m_effSTD_mu, mu, isGood);
    fillHisto(m_effSTD_nvertices, nVtx, isGood);
    fillHisto(m_effBAT_eta, eta, isGood);
    fillHisto(m_effBAT_pt, pt, isGood);
    fillHisto(m_effBAT_phi, phi, isGood);
    fillHisto(m_effBAT_d0, d0, isGood);
    fillHisto(m_effBAT_z0, z0, isGood);
    fillHisto(m_effBAT_radius, radius, isGood);
    fillHisto(m_effBAT_mu, mu, isGood);
    fillHisto(m_effBAT_nvertices, nVtx, isGood);
  }

}

void
InDetPerfPlot_ANTracking::fillFakeRate(const xAOD::TrackParticle& track, const bool isFake, const float mu, const unsigned int nVtx){

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;

  if(isANT){
    fillHisto(m_fakeANT_eta, eta, isFake);
    fillHisto(m_fakeANT_pt, pt, isFake);
    fillHisto(m_fakeANT_phi, phi, isFake);
    fillHisto(m_fakeANT_d0, d0, isFake);
    fillHisto(m_fakeANT_z0, z0, isFake);
    fillHisto(m_fakeANT_mu, mu, isFake);
    fillHisto(m_fakeANT_nvertices, nVtx, isFake);
  } else if(isSTD){
    fillHisto(m_fakeSTD_eta, eta, isFake);
    fillHisto(m_fakeSTD_pt, pt, isFake);
    fillHisto(m_fakeSTD_phi, phi, isFake);
    fillHisto(m_fakeSTD_d0, d0, isFake);
    fillHisto(m_fakeSTD_z0, z0, isFake);
    fillHisto(m_fakeSTD_mu, mu, isFake);
    fillHisto(m_fakeSTD_nvertices, nVtx, isFake);
  } else if(isBAT){
    fillHisto(m_fakeBAT_eta, eta, isFake);
    fillHisto(m_fakeBAT_pt, pt, isFake);
    fillHisto(m_fakeBAT_phi, phi, isFake);
    fillHisto(m_fakeBAT_d0, d0, isFake);
    fillHisto(m_fakeBAT_z0, z0, isFake);
    fillHisto(m_fakeBAT_mu, mu, isFake);
    fillHisto(m_fakeBAT_nvertices, nVtx, isFake);
  }


}

void
InDetPerfPlot_ANTracking::fillUnlinked(const xAOD::TrackParticle& track, const bool isAssociatedTruth, const float mu, const unsigned int nVtx) {

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;

  if(isANT){
    fillHisto(m_unlinkedANT_eta, eta, isAssociatedTruth);
    fillHisto(m_unlinkedANT_pt, pt, isAssociatedTruth);
    fillHisto(m_unlinkedANT_phi, phi, isAssociatedTruth);
    fillHisto(m_unlinkedANT_d0, d0, isAssociatedTruth);
    fillHisto(m_unlinkedANT_z0, z0, isAssociatedTruth);
    fillHisto(m_unlinkedANT_mu, mu, isAssociatedTruth);
    fillHisto(m_unlinkedANT_nvertices, nVtx, isAssociatedTruth);
  } else if(isSTD){
    fillHisto(m_unlinkedSTD_eta, eta, isAssociatedTruth);
    fillHisto(m_unlinkedSTD_pt, pt, isAssociatedTruth);
    fillHisto(m_unlinkedSTD_phi, phi, isAssociatedTruth);
    fillHisto(m_unlinkedSTD_d0, d0, isAssociatedTruth);
    fillHisto(m_unlinkedSTD_z0, z0, isAssociatedTruth);
    fillHisto(m_unlinkedSTD_mu, mu, isAssociatedTruth);
    fillHisto(m_unlinkedSTD_nvertices, nVtx, isAssociatedTruth);
  } else if(isBAT){
    fillHisto(m_unlinkedBAT_eta, eta, isAssociatedTruth);
    fillHisto(m_unlinkedBAT_pt, pt, isAssociatedTruth);
    fillHisto(m_unlinkedBAT_phi, phi, isAssociatedTruth);
    fillHisto(m_unlinkedBAT_d0, d0, isAssociatedTruth);
    fillHisto(m_unlinkedBAT_z0, z0, isAssociatedTruth);
    fillHisto(m_unlinkedBAT_mu, mu, isAssociatedTruth);
    fillHisto(m_unlinkedBAT_nvertices, nVtx, isAssociatedTruth);
  }


}

void
InDetPerfPlot_ANTracking::fill(const xAOD::TrackParticle& track) {

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;

  uint8_t iPixHits(0), iSctHits(0), iTrtHits(0);
  uint8_t iPixHoles(0), iSCTHoles(0), iTrtHTHits(0);
  uint8_t iPixelShared(0), iSCTShared(0);

  if (track.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    if(isANT) fillHisto(m_nPixelHitsANT_vs_eta, eta, iPixHits);
    if(isSTD) fillHisto(m_nPixelHitsSTD_vs_eta, eta, iPixHits);
    if(isBAT) fillHisto(m_nPixelHitsBAT_vs_eta, eta, iPixHits);
  }
  if (track.summaryValue(iSctHits, xAOD::numberOfSCTHits)) {
    if(isANT) fillHisto(m_nSCTHitsANT_vs_eta, eta, iSctHits);
    if(isSTD) fillHisto(m_nSCTHitsSTD_vs_eta, eta, iSctHits);
    if(isBAT) fillHisto(m_nSCTHitsBAT_vs_eta, eta, iSctHits);
  }
  if (track.summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
    if(isANT) fillHisto(m_nTRTHitsANT_vs_eta, eta, iTrtHits);
    if(isSTD) fillHisto(m_nTRTHitsSTD_vs_eta, eta, iTrtHits);
    if(isBAT) fillHisto(m_nTRTHitsBAT_vs_eta, eta, iTrtHits);
  }
  if (track.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    if(isANT) fillHisto(m_nPixelHolesANT_vs_eta, eta, iPixHoles);
    if(isSTD) fillHisto(m_nPixelHolesSTD_vs_eta, eta, iPixHoles);
    if(isBAT) fillHisto(m_nPixelHolesBAT_vs_eta, eta, iPixHoles);
  }
  if (track.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    if(isANT) fillHisto(m_nSCTHolesANT_vs_eta, eta, iSCTHoles);
    if(isSTD) fillHisto(m_nSCTHolesSTD_vs_eta, eta, iSCTHoles);
    if(isBAT) fillHisto(m_nSCTHolesBAT_vs_eta, eta, iSCTHoles);
  }
  if (track.summaryValue(iTrtHTHits, xAOD::numberOfTRTHighThresholdHits)) {
    if(isANT) fillHisto(m_nTRTHTHitsANT_vs_eta, eta, iTrtHTHits);
    if(isSTD) fillHisto(m_nTRTHTHitsSTD_vs_eta, eta, iTrtHTHits);
    if(isBAT) fillHisto(m_nTRTHTHitsBAT_vs_eta, eta, iTrtHTHits);
  }
  if (track.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
    if(isANT) fillHisto(m_nPixelSharedHitsANT_vs_eta, eta, iPixelShared);
    if(isSTD) fillHisto(m_nPixelSharedHitsSTD_vs_eta, eta, iPixelShared);
    if(isBAT) fillHisto(m_nPixelSharedHitsBAT_vs_eta, eta, iPixelShared);
  }
  if (track.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
    if(isANT) fillHisto(m_nSCTSharedHitsANT_vs_eta, eta, iSCTShared);
    if(isSTD) fillHisto(m_nSCTSharedHitsSTD_vs_eta, eta, iSCTShared);
    if(isBAT) fillHisto(m_nSCTSharedHitsBAT_vs_eta, eta, iSCTShared);
  }


  if(isANT){
    fillHisto(m_trkpropANT_eta, eta);
    fillHisto(m_trkpropANT_phi, phi);
    fillHisto(m_trkpropANT_pt, pt);
    fillHisto(m_trkpropANT_d0, d0);
    fillHisto(m_trkpropANT_z0, z0);
  }
  if(isBAT){
    fillHisto(m_trkpropBAT_eta, eta);
    fillHisto(m_trkpropBAT_phi, phi);
    fillHisto(m_trkpropBAT_pt, pt);
    fillHisto(m_trkpropBAT_d0, d0);
    fillHisto(m_trkpropBAT_z0, z0);
  }
  if(isSTD){
    fillHisto(m_trkpropSTD_eta, eta);
    fillHisto(m_trkpropSTD_phi, phi);
    fillHisto(m_trkpropSTD_pt, pt);
    fillHisto(m_trkpropSTD_d0, d0);
    fillHisto(m_trkpropSTD_z0, z0);
  }
}

void
InDetPerfPlot_ANTracking::fill(const unsigned int nTrkANT, const unsigned int nTrkSTD, const unsigned int nTrkBAT, const float mu, const unsigned int nVtx) {

  fillHisto(m_ntrkANT_mu, mu, nTrkANT);
  fillHisto(m_ntrkSTD_mu, mu, nTrkSTD);
  fillHisto(m_ntrkBAT_mu, mu, nTrkBAT);

  fillHisto(m_ntrkANT_nvertices, nVtx, nTrkANT);
  fillHisto(m_ntrkSTD_nvertices, nVtx, nTrkSTD);
  fillHisto(m_ntrkBAT_nvertices, nVtx, nTrkBAT);

}

void
InDetPerfPlot_ANTracking::finalizePlots() {
}
