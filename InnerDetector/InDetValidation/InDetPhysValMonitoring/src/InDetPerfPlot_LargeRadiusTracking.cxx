/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_LargeRadiusTracking.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetPerfPlot_LargeRadiusTracking::InDetPerfPlot_LargeRadiusTracking(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir)
{}

void
InDetPerfPlot_LargeRadiusTracking::initializePlots() {
  
  book(m_effLRT_eta,  "efficiency_vs_eta_LRT");
  book(m_effSTD_eta,  "efficiency_vs_eta_STD");
  book(m_effConv_eta, "efficiency_vs_eta_Conv");
  
  book(m_effLRT_pt,  "efficiency_vs_pt_LRT");
  book(m_effSTD_pt,  "efficiency_vs_pt_STD");
  book(m_effConv_pt, "efficiency_vs_pt_Conv");

  book(m_effLRT_phi,  "efficiency_vs_phi_LRT");
  book(m_effSTD_phi,  "efficiency_vs_phi_STD");
  book(m_effConv_phi, "efficiency_vs_phi_Conv");

  book(m_effLRT_d0,  "efficiency_vs_d0_LRT");
  book(m_effSTD_d0,  "efficiency_vs_d0_STD");
  book(m_effConv_d0, "efficiency_vs_d0_Conv");

  book(m_effLRT_z0,  "efficiency_vs_z0_LRT");
  book(m_effSTD_z0,  "efficiency_vs_z0_STD");
  book(m_effConv_z0, "efficiency_vs_z0_Conv");

  book(m_effLRT_radius,  "efficiency_vs_radius_LRT");
  book(m_effSTD_radius,  "efficiency_vs_radius_STD");
  book(m_effConv_radius, "efficiency_vs_radius_Conv");

  book(m_effLRT_mu,  "efficiency_vs_mu_LRT");
  book(m_effSTD_mu,  "efficiency_vs_mu_STD");
  book(m_effConv_mu, "efficiency_vs_mu_Conv");

  book(m_effLRT_nvertices,  "efficiency_vs_nvertices_LRT");
  book(m_effSTD_nvertices,  "efficiency_vs_nvertices_STD");
  book(m_effConv_nvertices, "efficiency_vs_nvertices_Conv");

  book(m_fakeLRT_eta,  "fake_vs_eta_LRT");
  book(m_fakeSTD_eta,  "fake_vs_eta_STD");
  book(m_fakeConv_eta, "fake_vs_eta_Conv");

  book(m_fakeLRT_pt,  "fake_vs_pt_LRT");
  book(m_fakeSTD_pt,  "fake_vs_pt_STD");
  book(m_fakeConv_pt, "fake_vs_pt_Conv");

  book(m_fakeLRT_phi,  "fake_vs_phi_LRT");
  book(m_fakeSTD_phi,  "fake_vs_phi_STD");
  book(m_fakeConv_phi, "fake_vs_phi_Conv");

  book(m_fakeLRT_d0,  "fake_vs_d0_LRT");
  book(m_fakeSTD_d0,  "fake_vs_d0_STD");
  book(m_fakeConv_d0, "fake_vs_d0_Conv");

  book(m_fakeLRT_z0,  "fake_vs_z0_LRT");
  book(m_fakeSTD_z0,  "fake_vs_z0_STD");
  book(m_fakeConv_z0, "fake_vs_z0_Conv");

  book(m_fakeLRT_mu,  "fake_vs_mu_LRT");
  book(m_fakeSTD_mu,  "fake_vs_mu_STD");
  book(m_fakeConv_mu, "fake_vs_mu_Conv");

  book(m_fakeLRT_nvertices,  "fake_vs_nvertices_LRT");
  book(m_fakeSTD_nvertices,  "fake_vs_nvertices_STD");
  book(m_fakeConv_nvertices, "fake_vs_nvertices_Conv");

  book(m_unlinkedLRT_eta,  "fake_vs_eta_unlinked_LRT");
  book(m_unlinkedSTD_eta,  "fake_vs_eta_unlinked_STD");
  book(m_unlinkedConv_eta, "fake_vs_eta_unlinked_Conv");

  book(m_unlinkedLRT_pt,  "fake_vs_pt_unlinked_LRT");
  book(m_unlinkedSTD_pt,  "fake_vs_pt_unlinked_STD");
  book(m_unlinkedConv_pt, "fake_vs_pt_unlinked_Conv");

  book(m_unlinkedLRT_phi,  "fake_vs_phi_unlinked_LRT");
  book(m_unlinkedSTD_phi,  "fake_vs_phi_unlinked_STD");
  book(m_unlinkedConv_phi, "fake_vs_phi_unlinked_Conv");

  book(m_unlinkedLRT_d0,  "fake_vs_d0_unlinked_LRT");
  book(m_unlinkedSTD_d0,  "fake_vs_d0_unlinked_STD");
  book(m_unlinkedConv_d0, "fake_vs_d0_unlinked_Conv");

  book(m_unlinkedLRT_z0,  "fake_vs_z0_unlinked_LRT");
  book(m_unlinkedSTD_z0,  "fake_vs_z0_unlinked_STD");
  book(m_unlinkedConv_z0, "fake_vs_z0_unlinked_Conv");

  book(m_unlinkedLRT_mu,  "fake_vs_mu_unlinked_LRT");
  book(m_unlinkedSTD_mu,  "fake_vs_mu_unlinked_STD");
  book(m_unlinkedConv_mu, "fake_vs_mu_unlinked_Conv");

  book(m_unlinkedLRT_nvertices,  "fake_vs_nvertices_unlinked_LRT");
  book(m_unlinkedSTD_nvertices,  "fake_vs_nvertices_unlinked_STD");
  book(m_unlinkedConv_nvertices, "fake_vs_nvertices_unlinked_Conv");
  
  book(m_linkedLRT_eta,  "fake_vs_eta_linked_LRT");
  book(m_linkedSTD_eta,  "fake_vs_eta_linked_STD");
  book(m_linkedConv_eta, "fake_vs_eta_linked_Conv");

  book(m_linkedLRT_pt,  "fake_vs_pt_linked_LRT");
  book(m_linkedSTD_pt,  "fake_vs_pt_linked_STD");
  book(m_linkedConv_pt, "fake_vs_pt_linked_Conv");

  book(m_linkedLRT_phi,  "fake_vs_phi_linked_LRT");
  book(m_linkedSTD_phi,  "fake_vs_phi_linked_STD");
  book(m_linkedConv_phi, "fake_vs_phi_linked_Conv");

  book(m_linkedLRT_d0,  "fake_vs_d0_linked_LRT");
  book(m_linkedSTD_d0,  "fake_vs_d0_linked_STD");
  book(m_linkedConv_d0, "fake_vs_d0_linked_Conv");

  book(m_linkedLRT_z0,  "fake_vs_z0_linked_LRT");
  book(m_linkedSTD_z0,  "fake_vs_z0_linked_STD");
  book(m_linkedConv_z0, "fake_vs_z0_linked_Conv");

  book(m_linkedLRT_mu,  "fake_vs_mu_linked_LRT");
  book(m_linkedSTD_mu,  "fake_vs_mu_linked_STD");
  book(m_linkedConv_mu, "fake_vs_mu_linked_Conv");

  book(m_linkedLRT_nvertices,  "fake_vs_nvertices_linked_LRT");
  book(m_linkedSTD_nvertices,  "fake_vs_nvertices_linked_STD");
  book(m_linkedConv_nvertices, "fake_vs_nvertices_linked_Conv");

  book(m_trkpropLRT_eta,  "trkprop_vs_eta_LRT");
  book(m_trkpropSTD_eta,  "trkprop_vs_eta_STD");
  book(m_trkpropConv_eta, "trkprop_vs_eta_Conv");
  
  book(m_trkpropLRT_phi,  "trkprop_vs_phi_LRT");
  book(m_trkpropSTD_phi,  "trkprop_vs_phi_STD");
  book(m_trkpropConv_phi, "trkprop_vs_phi_Conv");
  
  book(m_trkpropLRT_pt,  "trkprop_vs_pt_LRT");
  book(m_trkpropSTD_pt,  "trkprop_vs_pt_STD");
  book(m_trkpropConv_pt, "trkprop_vs_pt_Conv");
  
  book(m_trkpropLRT_d0,  "trkprop_vs_d0_LRT");
  book(m_trkpropSTD_d0,  "trkprop_vs_d0_STD");
  book(m_trkpropConv_d0, "trkprop_vs_d0_Conv");

  book(m_trkpropLRT_z0,  "trkprop_vs_z0_LRT");
  book(m_trkpropSTD_z0,  "trkprop_vs_z0_STD");
  book(m_trkpropConv_z0, "trkprop_vs_z0_Conv");

  book(m_ntrkLRT_mu,  "ntrk_vs_mu_LRT");
  book(m_ntrkSTD_mu,  "ntrk_vs_mu_STD");
  book(m_ntrkConv_mu, "ntrk_vs_mu_Conv");

  book(m_ntrkLRT_nvertices,  "ntrk_vs_nvertices_LRT");
  book(m_ntrkSTD_nvertices,  "ntrk_vs_nvertices_STD");
  book(m_ntrkConv_nvertices, "ntrk_vs_nvertices_Conv");

  book(m_nPixelHitsLRT_vs_eta,  "nPixelHits_vs_eta_LRT");
  book(m_nPixelHitsSTD_vs_eta,  "nPixelHits_vs_eta_STD");
  book(m_nPixelHitsConv_vs_eta, "nPixelHits_vs_eta_Conv");

  book(m_nSCTHitsLRT_vs_eta,  "nSCTHits_vs_eta_LRT");
  book(m_nSCTHitsSTD_vs_eta,  "nSCTHits_vs_eta_STD");
  book(m_nSCTHitsConv_vs_eta, "nSCTHits_vs_eta_Conv");

  book(m_nPixelHolesLRT_vs_eta,  "nPixelHoles_vs_eta_LRT");
  book(m_nPixelHolesSTD_vs_eta,  "nPixelHoles_vs_eta_STD");
  book(m_nPixelHolesConv_vs_eta, "nPixelHoles_vs_eta_Conv");

  book(m_nSCTHolesLRT_vs_eta,  "nSCTHoles_vs_eta_LRT");
  book(m_nSCTHolesSTD_vs_eta,  "nSCTHoles_vs_eta_STD");
  book(m_nSCTHolesConv_vs_eta, "nSCTHoles_vs_eta_Conv");

  book(m_nPixelSharedHitsLRT_vs_eta,  "nPixelSharedHits_vs_eta_LRT");
  book(m_nPixelSharedHitsSTD_vs_eta,  "nPixelSharedHits_vs_eta_STD");
  book(m_nPixelSharedHitsConv_vs_eta, "nPixelSharedHits_vs_eta_Conv");

  book(m_nSCTSharedHitsLRT_vs_eta,  "nSCTSharedHits_vs_eta_LRT");
  book(m_nSCTSharedHitsSTD_vs_eta,  "nSCTSharedHits_vs_eta_STD");
  book(m_nSCTSharedHitsConv_vs_eta, "nSCTSharedHits_vs_eta_Conv");

}

void 
InDetPerfPlot_LargeRadiusTracking::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx) {
 
  const float undefinedValue = -9999;

  double eta = truth.eta();
  double pt = truth.pt() / Gaudi::Units::GeV; // convert MeV to GeV
  double phi = truth.phi();

  double d0 = truth.isAvailable<float>("d0") ? truth.auxdata<float>("d0") : undefinedValue;
  double z0 = truth.isAvailable<float>("z0") ? truth.auxdata<float>("z0") : undefinedValue;
  double radius =  truth.isAvailable<float>("prodR") ? truth.auxdata<float>("prodR") : undefinedValue;
 
  bool isConv = false; bool isLRT = false; bool isSTD = false;
  if(isGood){
    const std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo> &patternInfo{track.patternRecoInfo()};
    isConv = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_ROIConvTracks);
    isLRT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
    isSTD = not isConv and not isLRT;
 
    if(isLRT){
      fillHisto(m_effLRT_eta, eta, isGood);
      fillHisto(m_effLRT_pt, pt, isGood);
      fillHisto(m_effLRT_phi, phi, isGood);
      fillHisto(m_effLRT_d0, d0, isGood);
      fillHisto(m_effLRT_z0, z0, isGood);
      fillHisto(m_effLRT_radius, radius, isGood);
      fillHisto(m_effLRT_mu, mu, isGood);
      fillHisto(m_effLRT_nvertices, nVtx, isGood);
    } else if(isSTD){
      fillHisto(m_effSTD_eta, eta, isGood);
      fillHisto(m_effSTD_pt, pt, isGood);
      fillHisto(m_effSTD_phi, phi, isGood);
      fillHisto(m_effSTD_d0, d0, isGood);
      fillHisto(m_effSTD_z0, z0, isGood);
      fillHisto(m_effSTD_radius, radius, isGood);
      fillHisto(m_effSTD_mu, mu, isGood);
      fillHisto(m_effSTD_nvertices, nVtx, isGood);
    } else if(isConv){
      fillHisto(m_effConv_eta, eta, isGood);
      fillHisto(m_effConv_pt, pt, isGood);
      fillHisto(m_effConv_phi, phi, isGood);
      fillHisto(m_effConv_d0, d0, isGood);
      fillHisto(m_effConv_z0, z0, isGood);
      fillHisto(m_effConv_radius, radius, isGood);
      fillHisto(m_effConv_mu, mu, isGood);
      fillHisto(m_effConv_nvertices, nVtx, isGood);
    }

  } else {
    fillHisto(m_effLRT_eta, eta, isGood);
    fillHisto(m_effLRT_pt, pt, isGood);
    fillHisto(m_effLRT_phi, phi, isGood);
    fillHisto(m_effLRT_d0, d0, isGood);
    fillHisto(m_effLRT_z0, z0, isGood);
    fillHisto(m_effLRT_radius, radius, isGood);
    fillHisto(m_effLRT_mu, mu, isGood);
    fillHisto(m_effLRT_nvertices, nVtx, isGood);
    fillHisto(m_effSTD_eta, eta, isGood);
    fillHisto(m_effSTD_pt, pt, isGood);
    fillHisto(m_effSTD_phi, phi, isGood);
    fillHisto(m_effSTD_d0, d0, isGood);
    fillHisto(m_effSTD_z0, z0, isGood);
    fillHisto(m_effSTD_radius, radius, isGood);
    fillHisto(m_effSTD_mu, mu, isGood);
    fillHisto(m_effSTD_nvertices, nVtx, isGood);
    fillHisto(m_effConv_eta, eta, isGood);
    fillHisto(m_effConv_pt, pt, isGood);
    fillHisto(m_effConv_phi, phi, isGood);
    fillHisto(m_effConv_d0, d0, isGood);
    fillHisto(m_effConv_z0, z0, isGood);
    fillHisto(m_effConv_radius, radius, isGood);
    fillHisto(m_effConv_mu, mu, isGood);
    fillHisto(m_effConv_nvertices, nVtx, isGood);
  }

}

void
InDetPerfPlot_LargeRadiusTracking::fillFakeRate(const xAOD::TrackParticle& track, const bool isFake, const float mu, const unsigned int nVtx){

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isConv = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_ROIConvTracks);
  bool isLRT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isConv and not isLRT;

  if(isLRT){
    fillHisto(m_fakeLRT_eta, eta, isFake);
    fillHisto(m_fakeLRT_pt, pt, isFake);
    fillHisto(m_fakeLRT_phi, phi, isFake);
    fillHisto(m_fakeLRT_d0, d0, isFake);
    fillHisto(m_fakeLRT_z0, z0, isFake);
    fillHisto(m_fakeLRT_mu, mu, isFake);
    fillHisto(m_fakeLRT_nvertices, nVtx, isFake);
  } else if(isSTD){
    fillHisto(m_fakeSTD_eta, eta, isFake);
    fillHisto(m_fakeSTD_pt, pt, isFake);
    fillHisto(m_fakeSTD_phi, phi, isFake);
    fillHisto(m_fakeSTD_d0, d0, isFake);
    fillHisto(m_fakeSTD_z0, z0, isFake);
    fillHisto(m_fakeSTD_mu, mu, isFake);
    fillHisto(m_fakeSTD_nvertices, nVtx, isFake);
  } else if(isConv){
    fillHisto(m_fakeConv_eta, eta, isFake);
    fillHisto(m_fakeConv_pt, pt, isFake);
    fillHisto(m_fakeConv_phi, phi, isFake);
    fillHisto(m_fakeConv_d0, d0, isFake);
    fillHisto(m_fakeConv_z0, z0, isFake);
    fillHisto(m_fakeConv_mu, mu, isFake);
    fillHisto(m_fakeConv_nvertices, nVtx, isFake);
  }


}

void
InDetPerfPlot_LargeRadiusTracking::fillLinkedandUnlinked(const xAOD::TrackParticle& track, float Unlinked_w,
                                                         const float mu, const unsigned int nVtx) {

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isConv = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_ROIConvTracks);
  bool isLRT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isConv and not isLRT;

  if (Unlinked_w == 0 ) {
    if(isLRT){
      fillHisto(m_linkedLRT_eta, eta, Unlinked_w);
      fillHisto(m_linkedLRT_pt, pt, Unlinked_w);
      fillHisto(m_linkedLRT_phi, phi, Unlinked_w);
      fillHisto(m_linkedLRT_d0, d0, Unlinked_w);
      fillHisto(m_linkedLRT_z0, z0, Unlinked_w);
      fillHisto(m_linkedLRT_mu, mu, Unlinked_w);
      fillHisto(m_linkedLRT_nvertices, nVtx, Unlinked_w);
    } else if(isSTD){
      fillHisto(m_linkedSTD_eta, eta, Unlinked_w);
      fillHisto(m_linkedSTD_pt, pt, Unlinked_w);
      fillHisto(m_linkedSTD_phi, phi, Unlinked_w);
      fillHisto(m_linkedSTD_d0, d0, Unlinked_w);
      fillHisto(m_linkedSTD_z0, z0, Unlinked_w);
      fillHisto(m_linkedSTD_mu, mu, Unlinked_w);
      fillHisto(m_linkedSTD_nvertices, nVtx, Unlinked_w);
    } else if(isConv){
      fillHisto(m_linkedConv_eta, eta, Unlinked_w);
      fillHisto(m_linkedConv_pt, pt, Unlinked_w);
      fillHisto(m_linkedConv_phi, phi, Unlinked_w);
      fillHisto(m_linkedConv_d0, d0, Unlinked_w);
      fillHisto(m_linkedConv_z0, z0, Unlinked_w);
      fillHisto(m_linkedConv_mu, mu, Unlinked_w);
      fillHisto(m_linkedConv_nvertices, nVtx, Unlinked_w);
    }    
  }
  
  
  if(isLRT){
    fillHisto(m_unlinkedLRT_eta, eta, Unlinked_w);
    fillHisto(m_unlinkedLRT_pt, pt, Unlinked_w);
    fillHisto(m_unlinkedLRT_phi, phi, Unlinked_w);
    fillHisto(m_unlinkedLRT_d0, d0, Unlinked_w);
    fillHisto(m_unlinkedLRT_z0, z0, Unlinked_w);
    fillHisto(m_unlinkedLRT_mu, mu, Unlinked_w);
    fillHisto(m_unlinkedLRT_nvertices, nVtx, Unlinked_w);
  } else if(isSTD){
    fillHisto(m_unlinkedSTD_eta, eta, Unlinked_w);
    fillHisto(m_unlinkedSTD_pt, pt, Unlinked_w);
    fillHisto(m_unlinkedSTD_phi, phi, Unlinked_w);
    fillHisto(m_unlinkedSTD_d0, d0, Unlinked_w);
    fillHisto(m_unlinkedSTD_z0, z0, Unlinked_w);
    fillHisto(m_unlinkedSTD_mu, mu, Unlinked_w);
    fillHisto(m_unlinkedSTD_nvertices, nVtx, Unlinked_w);
  } else if(isConv){
    fillHisto(m_unlinkedConv_eta, eta, Unlinked_w);
    fillHisto(m_unlinkedConv_pt, pt, Unlinked_w);
    fillHisto(m_unlinkedConv_phi, phi, Unlinked_w);
    fillHisto(m_unlinkedConv_d0, d0, Unlinked_w);
    fillHisto(m_unlinkedConv_z0, z0, Unlinked_w);
    fillHisto(m_unlinkedConv_mu, mu, Unlinked_w);
    fillHisto(m_unlinkedConv_nvertices, nVtx, Unlinked_w);
  }


}

void
InDetPerfPlot_LargeRadiusTracking::fill(const xAOD::TrackParticle& track) {

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isConv = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_ROIConvTracks);
  bool isLRT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isConv and not isLRT;

  uint8_t iPixHits(0), iSctHits(0);
  uint8_t iPixHoles(0), iSCTHoles(0);
  uint8_t iPixelShared(0), iSCTShared(0);

  if (track.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    if(isLRT) fillHisto(m_nPixelHitsLRT_vs_eta, eta, iPixHits);
    if(isSTD) fillHisto(m_nPixelHitsSTD_vs_eta, eta, iPixHits);
    if(isConv) fillHisto(m_nPixelHitsConv_vs_eta, eta, iPixHits);
  }
  if (track.summaryValue(iSctHits, xAOD::numberOfSCTHits)) {
    if(isLRT) fillHisto(m_nSCTHitsLRT_vs_eta, eta, iSctHits);
    if(isSTD) fillHisto(m_nSCTHitsSTD_vs_eta, eta, iSctHits);
    if(isConv) fillHisto(m_nSCTHitsConv_vs_eta, eta, iSctHits);
  }
  if (track.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    if(isLRT) fillHisto(m_nPixelHolesLRT_vs_eta, eta, iPixHoles);
    if(isSTD) fillHisto(m_nPixelHolesSTD_vs_eta, eta, iPixHoles);
    if(isConv) fillHisto(m_nPixelHolesConv_vs_eta, eta, iPixHoles);
  }
  if (track.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    if(isLRT) fillHisto(m_nSCTHolesLRT_vs_eta, eta, iSCTHoles);
    if(isSTD) fillHisto(m_nSCTHolesSTD_vs_eta, eta, iSCTHoles);
    if(isConv) fillHisto(m_nSCTHolesConv_vs_eta, eta, iSCTHoles);
  }
  if (track.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
    if(isLRT) fillHisto(m_nPixelSharedHitsLRT_vs_eta, eta, iPixelShared);
    if(isSTD) fillHisto(m_nPixelSharedHitsSTD_vs_eta, eta, iPixelShared);
    if(isConv) fillHisto(m_nPixelSharedHitsConv_vs_eta, eta, iPixelShared);
  }
  if (track.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
    if(isLRT) fillHisto(m_nSCTSharedHitsLRT_vs_eta, eta, iSCTShared);
    if(isSTD) fillHisto(m_nSCTSharedHitsSTD_vs_eta, eta, iSCTShared);
    if(isConv) fillHisto(m_nSCTSharedHitsConv_vs_eta, eta, iSCTShared);
  }


  if(isLRT){
    fillHisto(m_trkpropLRT_eta, eta);
    fillHisto(m_trkpropLRT_phi, phi);
    fillHisto(m_trkpropLRT_pt, pt);
    fillHisto(m_trkpropLRT_d0, d0);
    fillHisto(m_trkpropLRT_z0, z0);
  }
  if(isConv){
    fillHisto(m_trkpropConv_eta, eta);
    fillHisto(m_trkpropConv_phi, phi);
    fillHisto(m_trkpropConv_pt, pt);
    fillHisto(m_trkpropConv_d0, d0);
    fillHisto(m_trkpropConv_z0, z0);
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
InDetPerfPlot_LargeRadiusTracking::fill(const unsigned int nTrkLRT, const unsigned int nTrkSTD, const unsigned int nTrkConv, const float mu, const unsigned int nVtx) {

  fillHisto(m_ntrkLRT_mu, mu, nTrkLRT);
  fillHisto(m_ntrkSTD_mu, mu, nTrkSTD);
  fillHisto(m_ntrkConv_mu, mu, nTrkConv);

  fillHisto(m_ntrkLRT_nvertices, nVtx, nTrkLRT);
  fillHisto(m_ntrkSTD_nvertices, nVtx, nTrkSTD);
  fillHisto(m_ntrkConv_nvertices, nVtx, nTrkConv);

}

void
InDetPerfPlot_LargeRadiusTracking::finalizePlots() {
}
