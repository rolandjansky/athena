/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "InDetVertexValidation/VertexAnalysis.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"

/*#ifdef ROOTCORE
#include "MCTruthClassifier/MCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#endif*/

using std::cout;
using std::endl;
using std::string;

// Variable x-axis binnings
Double_t mu_binsx[] = {0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200};
Double_t sumpt_binsx[]  = {0,10,20,30,40,50,75,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000};
Double_t sumpt_squared_binsx[]  = {0,500,1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000};
Double_t lPUdz_binsx[] = {0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0};
Double_t ntracks_binsx[] = {0,1,2,3,4,5,10,20,40,60,80,100,120,140,200};
Double_t zPosition_binsx[] = {-200,-180,-160,-140,-120,-100,-80,-60,-40,-20,0,20,40,60,80,100,120,140,160,180,200};

Double_t eta_binsx[] = {-4,-3.5,-3,-2.5,-2.0,-1.5,-1.0,-0.5,0,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0};

// Average Local Pile-up Density
// Nominal r8209: "exp(-0.5 * Z * Z / 50. / 50.) / 50. / sqrt(6.28) * 200."
// Stage 1 r8210: "exp(-0.5 * Z * Z / 34. / 34.) / 34. / sqrt(6.28) * 200."

// Beam spot size
double lPUd_factor = 50.;

// Average mu value
double lPUd_mu = 200.;

int Truth_matched_hs_track_cut = 0;

VertexAnalysis::VertexAnalysis() {
  m_entryNumber = 0;
}

VertexAnalysis::~VertexAnalysis() {
}

// Return pointers to histograms and a stream name to help categorize output
std::vector< std::pair<TH1*,std::string> > VertexAnalysis::bookHistograms(float m_MaxLayoutEta) {

  std::vector< std::pair<TH1*,std::string> > histoAndStream;

  std::string streamName = "";

  h_events = new TH1D("h_events","Total number of events", 1, 0, 2);
  histoAndStream.push_back(std::make_pair(h_events,streamName));

  h_run_number = new TH1D("h_run_number","Event Number", 1, 0, 10);
  histoAndStream.push_back(std::make_pair(h_run_number,streamName));

  h_event_number = new TH1D("h_event_number","Event Number", 1, 0, 1);
  histoAndStream.push_back(std::make_pair(h_event_number,streamName));

  Double_t bias_binsx[10001] = {};
  for (int i = 0; i < ((sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1) + 1; i++){
    bias_binsx[i] = -10 + (i*20/10000);
  }

  //---
  // Plots that depend on the tracking within the InDetTrackParticles Container
  //---

  streamName = "Tracks";

  h_all_track_pt = new TH1D("h_all_track_pt","All tracks p_{T}", 100, 0.0, 20.0);
  h_all_track_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  histoAndStream.push_back(std::make_pair(h_all_track_pt,streamName));

  h_all_track_eta = new TH1D("h_all_track_eta","All tracks #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_all_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_all_track_eta,streamName));

  h_all_track_d0 = new TH1D("h_all_track_d0","All tracks d_{0} (Impact parameter)", 100, -2.0, 2.0);
  h_all_track_d0->GetXaxis()->SetTitle("d_{0} [mm]");
  histoAndStream.push_back(std::make_pair(h_all_track_d0,streamName));

    h_all_track_d0_eta = new TH2D("h_all_track_d0_eta","All tracks d_{0} (Impact parameter) vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 100, -2.0, 2.0);
    h_all_track_d0_eta->GetYaxis()->SetTitle("d_{0} [mm]");
    h_all_track_d0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_all_track_d0_eta,streamName));

  h_all_track_err_d0 = new TH1D("h_all_track_err_d0","All tracks d_{0} error", 50, 0.0, 1.0);
  h_all_track_err_d0->GetXaxis()->SetTitle("#sigma(d_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_all_track_err_d0,streamName));

    h_all_track_err_d0_eta = new TH2D("h_all_track_err_d0_eta","All tracks d_{0} error vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 50, 0.0, 1.0);
    h_all_track_err_d0_eta->GetYaxis()->SetTitle("#sigma(d_{0}) [mm]");
    h_all_track_err_d0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_all_track_err_d0_eta,streamName));

  h_all_track_d0_over_err_d0 = new TH1D("h_all_track_d0_over_err_d0","All tracks d_{0} / d_{0} error (Impact parameter significance)", 400, -10.0, 10.0);
  h_all_track_d0_over_err_d0->GetXaxis()->SetTitle("d_{0} [mm] / #sigma(d_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_all_track_d0_over_err_d0,streamName));

    h_all_track_d0_over_err_d0_eta = new TH2D("h_all_track_d0_over_err_d0_eta","All tracks d_{0} / d_{0} error (Impact parameter significance) vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 400, -10.0, 10.0);
    h_all_track_d0_over_err_d0_eta->GetYaxis()->SetTitle("d_{0} [mm] / #sigma(d_{0}) [mm]");
    h_all_track_d0_over_err_d0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_all_track_d0_over_err_d0_eta,streamName));

  h_all_track_z0 = new TH1D("h_all_track_z0","All tracks z_{0}", 100, -5.0, 5.0);
  h_all_track_z0->GetXaxis()->SetTitle("z_{0} [mm]");
  histoAndStream.push_back(std::make_pair(h_all_track_z0,streamName));

    h_all_track_z0_eta = new TH2D("h_all_track_z0_eta","All tracks z_{0} vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 100, -5.0, 5.0);
    h_all_track_z0_eta->GetYaxis()->SetTitle("z_{0} [mm]");
    h_all_track_z0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_all_track_z0_eta,streamName));

  h_all_track_err_z0 = new TH1D("h_all_track_err_z0","All tracks z_{0} error", 50, 0.0, 5.0);
  h_all_track_err_z0->GetXaxis()->SetTitle("#sigma(z_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_all_track_err_z0,streamName));

    h_all_track_err_z0_eta = new TH2D("h_all_track_err_z0_eta","All tracks z_{0} error vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 50, 0.0, 5.0);
    h_all_track_err_z0_eta->GetYaxis()->SetTitle("#sigma(z_{0}) [mm]");
    h_all_track_err_z0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_all_track_err_z0_eta,streamName));

  h_all_track_z0_over_err_z0 = new TH1D("h_all_track_z0_over_err_z0","All tracks z_{0} / z_{0} error", 50, 0.0, 5000.0);
  h_all_track_z0_over_err_z0->GetXaxis()->SetTitle("z_{0} [mm] / #sigma(z_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_all_track_z0_over_err_z0,streamName));

    h_all_track_z0_over_err_z0_eta = new TH2D("h_all_track_z0_over_err_z0_eta","All tracks z_{0} / z_{0} error vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 50, 0.0, 5000.0);
    h_all_track_z0_over_err_z0_eta->GetYaxis()->SetTitle("z_{0} [mm] / #sigma(z_{0}) [mm]");
    h_all_track_z0_over_err_z0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_all_track_z0_over_err_z0_eta,streamName));

  //---
  // Plots that depend on the primary vertices in the vertexContainer and event information, will work even if sample is data or no truth matching performed 
  //---

  streamName = "Vertices";

  // Pure mu plots
  h_mu_UnCorrected = new TH1D("h_mu_UnCorrected","Actual Interactions Per Crossing (#mu)", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
  h_mu_UnCorrected->GetYaxis()->SetTitle("Events / 20");
  h_mu_UnCorrected->GetXaxis()->SetTitle("#mu");
  histoAndStream.push_back(std::make_pair(h_mu_UnCorrected,streamName));

  h_mu_Corrected = new TH1D("h_mu_Corrected","Actual Interactions Per Crossing [#mu] (Corrected by Pile-up ReWeighting)", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
  h_mu_Corrected->GetYaxis()->SetTitle("Events / 20");
  h_mu_Corrected->GetXaxis()->SetTitle("#mu");
  histoAndStream.push_back(std::make_pair(h_mu_Corrected,streamName));

  // Number of vertices in event
  h_nVtx = new TH1D("h_nVtx", "Number of vertices per event", 100, 0.0, 200);
  h_nVtx->GetYaxis()->SetTitle("Events / 2");
  h_nVtx->GetXaxis()->SetTitle("Number of vertices per event");
  histoAndStream.push_back(std::make_pair(h_nVtx,streamName));

    h_nVtx_Mu = new TH2D("h_nVtx_Mu", "Number of vertices vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nVtx_Mu->GetYaxis()->SetTitle("Number of vertices");
    h_nVtx_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nVtx_Mu,streamName));

  // Vertex position
  h_x = new TH1D("h_x","X position of vertex", 200, -2.0, 2.0);
  h_x->GetXaxis()->SetTitle("x [mm]");
  histoAndStream.push_back(std::make_pair(h_x,streamName));

  h_y = new TH1D("h_y","Y position of vertex", 200, -2.0, 2.0);
  h_y->GetXaxis()->SetTitle("y [mm]");
  histoAndStream.push_back(std::make_pair(h_y,streamName));

  h_z = new TH1D("h_z","Z position of vertex", 200, -200.0, 200.0);
  h_z->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_z,streamName));

  h_x_err = new TH1D("h_x_err","X position error of vertex", 200, 0.0, 0.2);
  h_x_err->GetXaxis()->SetTitle("#sigma(X) [mm]");
  histoAndStream.push_back(std::make_pair(h_x_err,streamName));

  h_y_err = new TH1D("h_y_err","Y position error of vertex", 200, 0.0, 0.2);
  h_y_err->GetXaxis()->SetTitle("#sigma(Y) [mm]");
  histoAndStream.push_back(std::make_pair(h_y_err,streamName));

  h_z_err = new TH1D("h_z_err","Z position error of vertex", 200, 0.0, 2.0);
  h_z_err->GetXaxis()->SetTitle("#sigma(Z) [mm]");
  histoAndStream.push_back(std::make_pair(h_z_err,streamName));

  h_lPUdz = new TH1D("h_lPUdz", "Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_lPUdz,streamName));

  // Delta z between vertices
  h_Dz = new TH1D("h_Dz", "Z position difference between all vertices (#Delta(Z))", 10000, -100.0, 100.0);
  h_Dz->GetXaxis()->SetTitle("#Deltaz [mm]");
  histoAndStream.push_back(std::make_pair(h_Dz,streamName));

    h_highest_pt_Dz = new TH1D("h_highest_pt_Dz", "Z position difference between the highest p_{T} vertex and all other vertices (#Delta(Z))", 10000, -100.0, 100.0);
    h_highest_pt_Dz->GetXaxis()->SetTitle("#Deltaz [mm]");
    histoAndStream.push_back(std::make_pair(h_highest_pt_Dz,streamName));

    h_Dz_Mu = new TH2D("h_Dz_Mu", "Z position difference between vertices (#Delta(Z)) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -100.0, 100.0);
    h_Dz_Mu->GetYaxis()->SetTitle("#Deltaz [mm]");
    h_Dz_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_Dz_Mu,streamName));

  // Vertex properties
  h_type = new TH1D("h_type","Vertex type", 7, 0.0, 7.0);
  h_type->GetXaxis()->SetTitle("Vertex type");
  h_type->GetXaxis()->SetBinLabel(1, "Primary");
  h_type->GetXaxis()->SetBinLabel(2, "Secondary");
  h_type->GetXaxis()->SetBinLabel(3, "Pile-up");
  h_type->GetXaxis()->SetBinLabel(4, "Conversion");
  h_type->GetXaxis()->SetBinLabel(5, "V0 decay");
  h_type->GetXaxis()->SetBinLabel(6, "Kink");
  histoAndStream.push_back(std::make_pair(h_type,streamName));

  h_Vtx_sumpt_squared = new TH1D("h_Vtx_sumpt_squared", "Vertex #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_Vtx_sumpt_squared->GetYaxis()->SetTitle("Number of vertices");
  h_Vtx_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_Vtx_sumpt_squared,streamName));

  h_Vtx_lPUdz = new TH1D("h_Vtx_lPUdz", "Vertex Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_Vtx_lPUdz->GetYaxis()->SetTitle("Number of vertices");
  h_Vtx_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_Vtx_lPUdz,streamName));

  h_Vtx_ntracks = new TH1D("h_Vtx_ntracks", "Vertex Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_Vtx_ntracks->GetYaxis()->SetTitle("Number of vertices");
  h_Vtx_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_Vtx_ntracks,streamName));

  h_Vtx_zPosition = new TH1D("h_Vtx_zPosition", "Vertex Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_Vtx_zPosition->GetYaxis()->SetTitle("Number of vertices");
  h_Vtx_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_Vtx_zPosition,streamName));

  h_chi2_over_ndf = new TH1D("h_chi2_over_ndf","Vertex #chi^{2} / ndf", 50, 0.0, 10.0);
  h_chi2_over_ndf->GetXaxis()->SetTitle("#chi^{2} / ndf");
  histoAndStream.push_back(std::make_pair(h_chi2_over_ndf,streamName));

    h_chi2_over_ndf_Mu = new TH2D("h_chi2_over_ndf_Mu", "Vertex #chi^{2} / ndf vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 50, 0.0, 10.0);
    h_chi2_over_ndf_Mu->GetYaxis()->SetTitle("#chi^{2}/ndf");
    h_chi2_over_ndf_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_chi2_over_ndf_Mu,streamName));

    h_chi2_over_ndf_sumpt_squared = new TH2D("h_chi2_over_ndf_sumpt_squared", "Vertex #chi^{2} / ndf vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 50, 0.0, 10.0);
    h_chi2_over_ndf_sumpt_squared->GetYaxis()->SetTitle("#chi^{2}/ndf");
    h_chi2_over_ndf_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_chi2_over_ndf_sumpt_squared,streamName));

    h_chi2_over_ndf_lPUdz = new TH2D("h_chi2_over_ndf_lPUdz", "Vertex #chi^{2} / ndf vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 50, 0.0, 10.0);
    h_chi2_over_ndf_lPUdz->GetYaxis()->SetTitle("#chi^{2}/ndf");
    h_chi2_over_ndf_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_chi2_over_ndf_lPUdz,streamName));

    h_chi2_over_ndf_ntracks = new TH2D("h_chi2_over_ndf_ntracks", "Vertex #chi^{2} / ndf vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 50, 0.0, 10.0);
    h_chi2_over_ndf_ntracks->GetYaxis()->SetTitle("#chi^{2}/ndf");
    h_chi2_over_ndf_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_chi2_over_ndf_ntracks,streamName));

    h_chi2_over_ndf_zPosition = new TH2D("h_chi2_over_ndf_zPosition", "Vertex #chi^{2} / ndf vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 50, 0.0, 10.0);
    h_chi2_over_ndf_zPosition->GetYaxis()->SetTitle("#chi^{2}/ndf");
    h_chi2_over_ndf_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_chi2_over_ndf_zPosition,streamName));

  //---
  // Plots that depend on the vertexContainer, event information and the track links for each vertex within the vertexContainer, will work even if sample is data or no truth matching performed 
  //---

  streamName = "TracksAtVertices";

  // Number of tracks at Vertex
  h_ntracks = new TH1D("h_ntracks","Number of tracks at vertex", 300, 0.0, 300.0);
  h_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_ntracks,streamName));

    h_ntracks_Mu = new TH2D("h_ntracks_Mu", "Number of tracks at vertex vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 300, 0.0, 300.0);
    h_ntracks_Mu->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_ntracks_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_ntracks_Mu,streamName));

    h_ntracks_sumpt_squared = new TH2D("h_ntracks_sumpt_squared", "Number of tracks at vertex vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 300, 0.0, 300.0);
    h_ntracks_sumpt_squared->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_ntracks_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_ntracks_sumpt_squared,streamName));

    h_ntracks_lPUdz = new TH2D("h_ntracks_lPUdz", "Number of tracks at vertex vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 300, 0.0, 300.0);
    h_ntracks_lPUdz->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_ntracks_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_ntracks_lPUdz,streamName));

    h_ntracks_zPosition = new TH2D("h_ntracks_zPosition", "Number of tracks at vertex vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 300, 0.0, 300.0);
    h_ntracks_zPosition->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_ntracks_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_ntracks_zPosition,streamName));

  // Track at Vertex properties
  h_track_weights = new TH1D("h_track_weights","Weights of tracks at vertex", 100, 0.0, 10.0);
  h_track_weights->GetXaxis()->SetTitle("Weight");
  histoAndStream.push_back(std::make_pair(h_track_weights,streamName));

  h_track_pt = new TH1D("h_track_pt","Tracks at vertex p_{T}", 100, 0.0, 20.0);
  h_track_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  histoAndStream.push_back(std::make_pair(h_track_pt,streamName));

  h_track_eta = new TH1D("h_track_eta","Tracks at vertex #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_track_eta,streamName));

  h_track_t = new TH1D("h_track_t","", 200, -100.0, 100.0);
  h_track_t->GetXaxis()->SetTitle("Time [ps]");
  histoAndStream.push_back(std::make_pair(h_track_t,streamName));

  h_track_t_with_cut = new TH1D("h_track_t_with_cut","", 120, -60.0, 60.0);
  h_track_t_with_cut->GetXaxis()->SetTitle("Time [ps]");
  histoAndStream.push_back(std::make_pair(h_track_t_with_cut,streamName));

  h_track_nSiHits = new TH1D("h_track_nSiHits","Tracks at vertex number of Silicon Hits", 15, 5.0, 20.0);
  h_track_nSiHits->GetXaxis()->SetTitle("Number of silicon hits");
  histoAndStream.push_back(std::make_pair(h_track_nSiHits,streamName));

  h_track_nSiHoles = new TH1D("h_track_nSiHoles","Tracks at vertex number of Silicon Holes", 5, 0.0, 5.0);
  h_track_nSiHoles->GetXaxis()->SetTitle("Number of silicon holes");
  histoAndStream.push_back(std::make_pair(h_track_nSiHoles,streamName));

  h_track_d0 = new TH1D("h_track_d0","Tracks at vertex d_{0} (Impact parameter)", 100, -2.0, 2.0);
  h_track_d0->GetXaxis()->SetTitle("d_{0} [mm]");
  histoAndStream.push_back(std::make_pair(h_track_d0,streamName));

  h_track_err_d0 = new TH1D("h_track_err_d0","Tracks at vertex d_{0} error", 50, 0.0, 1.0);
  h_track_err_d0->GetXaxis()->SetTitle("#sigma(d_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_track_err_d0,streamName));

  h_track_d0_over_err_d0 = new TH1D("h_track_d0_over_err_d0","Tracks at vertex d_{0} / d_{0} error (Impact parameter significance)", 400, -10.0, 10.0);
  h_track_d0_over_err_d0->GetXaxis()->SetTitle("d_{0} [mm] / #sigma(d_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_track_d0_over_err_d0,streamName));

    h_track_d0_over_err_d0_Mu = new TH2D("h_track_d0_over_err_d0_Mu", "Tracks at vertex d_{0} / d_{0} error (Impact parameter significance) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 400, -20.0, 20.0);
    h_track_d0_over_err_d0_Mu->GetYaxis()->SetTitle("d_{0} [mm] / #sigma(d_{0}) [mm]");
    h_track_d0_over_err_d0_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_track_d0_over_err_d0_Mu,streamName));

  h_track_z0 = new TH1D("h_track_z0","Tracks at vertex z_{0}", 100, -5.0, 5.0);
  h_track_z0->GetXaxis()->SetTitle("z_{0} [mm]");
  histoAndStream.push_back(std::make_pair(h_track_z0,streamName));

  h_track_err_z0 = new TH1D("h_track_err_z0","Tracks at vertex z_{0} error", 50, 0.0, 5.0);
  h_track_err_z0->GetXaxis()->SetTitle("#sigma(z_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_track_err_z0,streamName));

    h_track_err_z0_Mu = new TH2D("h_track_err_z0_Mu", "Tracks at vertex z_{0} error vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 400, -20.0, 20.0);
    h_track_err_z0_Mu->GetYaxis()->SetTitle("d_{0} [mm] / #sigma(d_{0}) [mm]");
    h_track_err_z0_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_track_err_z0_Mu,streamName));

    h_track_err_z0_eta = new TH2D("h_track_err_z0_eta", "Tracks at vertex z_{0} error vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx, 400, -20.0, 20.0);
    h_track_err_z0_eta->GetYaxis()->SetTitle("d_{0} [mm] / #sigma(d_{0}) [mm]");
    h_track_err_z0_eta->GetXaxis()->SetTitle("#eta");
    histoAndStream.push_back(std::make_pair(h_track_err_z0_eta,streamName));

  h_track_z0_over_err_z0 = new TH1D("h_track_z0_over_err_z0","Tracks at vertex z_{0} / z_{0} error", 50, 0.0, 5000.0);
  h_track_z0_over_err_z0->GetXaxis()->SetTitle("z_{0} [mm] / #sigma(z_{0}) [mm]");
  histoAndStream.push_back(std::make_pair(h_track_z0_over_err_z0,streamName));

  h_track_z0_Vtx_z0 = new TH1D("h_track_z0_Vtx_z0","z_{0}^{tracks at vertex} - z^{vertex}", 100, -5.0, 5.0);
  h_track_z0_Vtx_z0->GetXaxis()->SetTitle("z_{0}^{tracks at vertex} - z^{vertex} [mm]");
  histoAndStream.push_back(std::make_pair(h_track_z0_Vtx_z0,streamName));

  h_track_bias_d0 = new TH1D("h_track_bias_d0","Tracks at vertex d_{0}^{rec} - d_{0}^{tru}", 1000, -1.0, 1.0);
  h_track_bias_d0->GetXaxis()->SetTitle("d_{0}^{rec} - d_{0}^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_track_bias_d0,streamName));

  h_track_bias_z0 = new TH1D("h_track_bias_z0","Tracks at vertex z_{0}^{rec} - z_{0}^{tru}", 1000, -1.0, 1.0);
  h_track_bias_z0->GetXaxis()->SetTitle("z_{0}^{rec} - z_{0}^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_track_bias_z0,streamName));

  h_track_bias_phi = new TH1D("h_track_bias_phi","Tracks at vertex #phi^{rec} - #phi^{tru}", 1000, -1.0, 1.0);
  h_track_bias_phi->GetXaxis()->SetTitle("#phi^{rec} - #phi^{tru}");
  histoAndStream.push_back(std::make_pair(h_track_bias_phi,streamName));

  h_track_pull_d0 = new TH1D("h_track_pull_d0","Pull : Tracks at vertex (d_{0}^{rec} - d_{0}^{tru}) / #sigma_{d_{0}}", 200, -5.0, 5.0);
  h_track_pull_d0->GetXaxis()->SetTitle("(d_{0}^{rec} - d_{0}^{tru}) / #sigma_{d_{0}}");
  histoAndStream.push_back(std::make_pair(h_track_pull_d0,streamName));

  h_track_pull_z0 = new TH1D("h_track_pull_z0","Pull : Tracks at vertex (z_{0}^{rec} - z_{0}^{tru}) / #sigma_{z_{0}}", 200, -5.0, 5.0);
  h_track_pull_z0->GetXaxis()->SetTitle("(z_{0}^{rec} - z_{0}^{tru}) / #sigma_{z_{0}}");
  histoAndStream.push_back(std::make_pair(h_track_pull_z0,streamName));

  h_track_pull_phi = new TH1D("h_track_pull_phi","Pull : Tracks at vertex (#phi^{rec} - #phi^{tru}) / #sigma_{#phi}", 200, -5.0, 5.0);
  h_track_pull_phi->GetXaxis()->SetTitle("(#phi^{rec} - #phi^{tru}) / #sigma_{#phi}");
  histoAndStream.push_back(std::make_pair(h_track_pull_phi,streamName));

  h_eff_track_pt = new TH1D("h_eff_track_pt","Efficiency of track selection for vertices vs p_{T}", 100, 0.0, 20.0);
  h_eff_track_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  histoAndStream.push_back(std::make_pair(h_eff_track_pt,streamName));

  h_eff_track_eta = new TH1D("h_eff_track_eta","Efficiency of track selection for vertices vs #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_eff_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_eff_track_eta,streamName));

  //---
  // Plots that depend on truth matching info decorated vertexContainer.
  //---

  streamName = "VerticesTruthMatching/All";

  // Class of decorated primary vertex (Matched, Merged, Split or Fake) from truth matching
  h_nClass = new TH1D("h_nClass", "Truth-Match type for reconstructed vertices", 4, -0.5, 3.5);
  h_nClass->GetXaxis()->SetTitle("Category");
  h_nClass->GetXaxis()->SetBinLabel(1, "Matched");
  h_nClass->GetXaxis()->SetBinLabel(2, "Merged");
  h_nClass->GetXaxis()->SetBinLabel(3, "Split");
  h_nClass->GetXaxis()->SetBinLabel(4, "Fake");
  histoAndStream.push_back(std::make_pair(h_nClass,streamName));

  h_nReconstruct = new TH1D("h_nReconstruct", "Number of reconstructed vertices per event", 100, 0.0, 200);
  h_nReconstruct->GetXaxis()->SetTitle("n_{Reconstruct}");
  histoAndStream.push_back(std::make_pair(h_nReconstruct,streamName));

    h_nReconstruct_mu = new TH2D("h_nReconstruct_mu", "Number of reconstructed vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nReconstruct_mu->GetYaxis()->SetTitle("n_{Reconstruct}");
    h_nReconstruct_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nReconstruct_mu,streamName));

  h_Reconstruct_sumpt_squared = new TH1D("h_Reconstruct_sumpt_squared", "Reconstructed vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_Reconstruct_sumpt_squared->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_Reconstruct_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_Reconstruct_sumpt_squared,streamName));

  h_Reconstruct_lPUdz = new TH1D("h_Reconstruct_lPUdz", "Reconstructed vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_Reconstruct_lPUdz->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_Reconstruct_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_Reconstruct_lPUdz,streamName));

  h_Reconstruct_ntracks = new TH1D("h_Reconstruct_ntracks", "Reconstructed vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_Reconstruct_ntracks->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_Reconstruct_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_Reconstruct_ntracks,streamName));

  h_Reconstruct_zPosition = new TH1D("h_Reconstruct_zPosition", "Reconstructed vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_Reconstruct_zPosition->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_Reconstruct_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_Reconstruct_zPosition,streamName));

  h_nMatch = new TH1D("h_nMatch", "Number of matched vertices per event", 100, 0.0, 200);
  h_nMatch->GetXaxis()->SetTitle("n_{Match}");
  histoAndStream.push_back(std::make_pair(h_nMatch,streamName));

    h_nMatch_mu = new TH2D("h_nMatch_mu", "Number of matched vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nMatch_mu->GetYaxis()->SetTitle("n_{Match}");
    h_nMatch_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nMatch_mu,streamName));

  h_Match_sumpt_squared = new TH1D("h_Match_sumpt_squared", "Matched vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_Match_sumpt_squared->GetYaxis()->SetTitle("n_{Match}");
  h_Match_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_Match_sumpt_squared,streamName));

  h_Match_lPUdz = new TH1D("h_Match_lPUdz", "Matched vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_Match_lPUdz->GetYaxis()->SetTitle("n_{Match}");
  h_Match_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_Match_lPUdz,streamName));

  h_Match_ntracks = new TH1D("h_Match_ntracks", "Matched vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_Match_ntracks->GetYaxis()->SetTitle("n_{Match}");
  h_Match_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_Match_ntracks,streamName));

  h_Match_zPosition = new TH1D("h_Match_zPosition", "Matched vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_Match_zPosition->GetYaxis()->SetTitle("n_{Match}");
  h_Match_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_Match_zPosition,streamName));

  h_nMerge = new TH1D("h_nMerge", "Number of merged vertices per event", 100, 0.0, 200);
  h_nMerge->GetXaxis()->SetTitle("n_{Merged}");
  histoAndStream.push_back(std::make_pair(h_nMerge,streamName));

    h_nMerge_mu = new TH2D("h_nMerge_mu", "Number of merged vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nMerge_mu->GetYaxis()->SetTitle("n_{Merge}");
    h_nMerge_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nMerge_mu,streamName));

  h_Merge_sumpt_squared = new TH1D("h_Merge_sumpt_squared", "Merged vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_Merge_sumpt_squared->GetYaxis()->SetTitle("n_{Merge}");
  h_Merge_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_Merge_sumpt_squared,streamName));

  h_Merge_lPUdz = new TH1D("h_Merge_lPUdz", "Merged vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_Merge_lPUdz->GetYaxis()->SetTitle("n_{Merge}");
  h_Merge_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_Merge_lPUdz,streamName));

  h_Merge_ntracks = new TH1D("h_Merge_ntracks", "Merged vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_Merge_ntracks->GetYaxis()->SetTitle("n_{Merge}");
  h_Merge_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_Merge_ntracks,streamName));

  h_Merge_zPosition = new TH1D("h_Merge_zPosition", "Merged vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_Merge_zPosition->GetYaxis()->SetTitle("n_{Merge}");
  h_Merge_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_Merge_zPosition,streamName));

  h_nSplit = new TH1D("h_nSplit", "Number of split vertices per event", 100, 0.0, 200);
  h_nSplit->GetXaxis()->SetTitle("n_{Split}");
  histoAndStream.push_back(std::make_pair(h_nSplit,streamName));

    h_nSplit_mu = new TH2D("h_nSplit_mu", "Number of split vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nSplit_mu->GetYaxis()->SetTitle("n_{Split}");
    h_nSplit_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nSplit_mu,streamName));

  h_Split_sumpt_squared = new TH1D("h_Split_sumpt_squared", "Split vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_Split_sumpt_squared->GetYaxis()->SetTitle("n_{Split}");
  h_Split_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_Split_sumpt_squared,streamName));

  h_Split_lPUdz = new TH1D("h_Split_lPUdz", "Split vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_Split_lPUdz->GetYaxis()->SetTitle("n_{Split}");
  h_Split_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_Split_lPUdz,streamName));

  h_Split_ntracks = new TH1D("h_Split_ntracks", "Split vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_Split_ntracks->GetYaxis()->SetTitle("n_{Split}");
  h_Split_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_Split_ntracks,streamName));

  h_Split_zPosition = new TH1D("h_Split_zPosition", "Split vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_Split_zPosition->GetYaxis()->SetTitle("n_{Split}");
  h_Split_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_Split_zPosition,streamName));

  h_nFake = new TH1D("h_nFake", "Number of fake vertices per event", 100, 0.0, 200);
  h_nFake->GetXaxis()->SetTitle("n_{Fake}");
  histoAndStream.push_back(std::make_pair(h_nFake,streamName));

    h_nFake_mu = new TH2D("h_nFake_mu", "Number of fake vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nFake_mu->GetYaxis()->SetTitle("n_{Fake}");
    h_nFake_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nFake_mu,streamName));

  h_Fake_sumpt_squared = new TH1D("h_Fake_sumpt_squared", "Fake vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_Fake_sumpt_squared->GetYaxis()->SetTitle("n_{Fake}");
  h_Fake_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_Fake_sumpt_squared,streamName));

  h_Fake_lPUdz = new TH1D("h_Fake_lPUdz", "Fake vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_Fake_lPUdz->GetYaxis()->SetTitle("n_{Fake}");
  h_Fake_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_Fake_lPUdz,streamName));

  h_Fake_ntracks = new TH1D("h_Fake_ntracks", "Fake vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_Fake_ntracks->GetYaxis()->SetTitle("n_{Fake}");
  h_Fake_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_Fake_ntracks,streamName));

  h_Fake_zPosition = new TH1D("h_Fake_zPosition", "Fake vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_Fake_zPosition->GetYaxis()->SetTitle("n_{Fake}");
  h_Fake_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_Fake_zPosition,streamName));

  // Z distance between splits matching same true interaction
  h_DzSplits = new TH1D("h_DzSplits", "Z distance between split vertices", 200, -100.0, 100.0);
  h_DzSplits->GetXaxis()->SetTitle("#Deltaz [mm]");
  histoAndStream.push_back(std::make_pair(h_DzSplits,streamName));

  // Hard scatter categorization
  h_HSCategory = new TH1D("h_HSCategory", "Hard scatter categorization", 5, -0.5, 4.5);
  h_HSCategory->GetXaxis()->SetTitle("Categorization");
  h_HSCategory->GetXaxis()->SetBinLabel(1, "Clean");
  h_HSCategory->GetXaxis()->SetBinLabel(2, "Low PU");
  h_HSCategory->GetXaxis()->SetBinLabel(3, "High PU");
  h_HSCategory->GetXaxis()->SetBinLabel(4, "Split");
  h_HSCategory->GetXaxis()->SetBinLabel(5, "No match");  
  histoAndStream.push_back(std::make_pair(h_HSCategory,streamName));

  // Hard scatter categorization v. mu
  h_HSCategory_Mu = new TH2D("h_HSCategory_vMu","Hard scatter categorization vs #mu",20,0.0,200,5,-0.5,4.5);
  h_HSCategory_Mu->GetYaxis()->SetBinLabel(1, "Clean");
  h_HSCategory_Mu->GetYaxis()->SetBinLabel(2, "Low PU");
  h_HSCategory_Mu->GetYaxis()->SetBinLabel(3, "High PU");
  h_HSCategory_Mu->GetYaxis()->SetBinLabel(4, "Split");
  h_HSCategory_Mu->GetYaxis()->SetBinLabel(5, "No match");  
  histoAndStream.push_back(std::make_pair(h_HSCategory_Mu,streamName));

  //---
  // Plots that depend on truth matching info decorated vertexContainer that has been flagged as clostest vertex to the true HS vertex.
  //---

  streamName = "VerticesTruthMatching/HSOnly";

  // Class of decorated primary vertex (Matched, Merged, Split or Fake) from truth matching
  h_HS_nClass = new TH1D("h_HS_nClass", "Truth-Match type for reconstructed vertices clostest to HS", 4, -0.5, 3.5);
  h_HS_nClass->GetXaxis()->SetTitle("Category");
  h_HS_nClass->GetXaxis()->SetBinLabel(1, "Matched");
  h_HS_nClass->GetXaxis()->SetBinLabel(2, "Merged");
  h_HS_nClass->GetXaxis()->SetBinLabel(3, "Split");
  h_HS_nClass->GetXaxis()->SetBinLabel(4, "Fake");
  histoAndStream.push_back(std::make_pair(h_HS_nClass,streamName));

  h_nHS_best = new TH1D("h_nHS_best", "Number of HS vertices per event", 1, 0, 2);
  h_nHS_best->GetXaxis()->SetTitle("Number of HS vertices");
  histoAndStream.push_back(std::make_pair(h_nHS_best,streamName));

    h_nHS_best_mu = new TH2D("h_nHS_best_mu", "Number of HS vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nHS_best_mu->GetYaxis()->SetTitle("Number of HS vertices");
    h_nHS_best_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nHS_best_mu,streamName));

  h_HS_best_track_eta = new TH1D("h_HS_best_track_eta","Tracks at HS vertex #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_HS_best_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_HS_best_track_eta,streamName));

  h_HS_best_track_pt = new TH1D("h_HS_best_track_pt","Tracks at HS vertex p_{T}", 100, 0.0, 20.0);
  h_HS_best_track_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  histoAndStream.push_back(std::make_pair(h_HS_best_track_pt,streamName));

  h_HS_best_track_association_eta = new TH1D("h_HS_best_track_association_eta","Tracks Associated to HS vertices vs #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_HS_best_track_association_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_HS_best_track_association_eta,streamName));

  h_HS_best_truth_association_eta = new TH1D("h_HS_best_truth_association_eta","Truth particles Associated to HS vertices vs #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_HS_best_truth_association_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_HS_best_truth_association_eta,streamName));

  h_HS_best_sqrt_sumpt_squared = new TH1D("h_HS_best_sqrt_sumpt_squared", "HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx);
  h_HS_best_sqrt_sumpt_squared->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sqrt_sumpt_squared->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
  histoAndStream.push_back(std::make_pair(h_HS_best_sqrt_sumpt_squared,streamName));

  h_HS_best_sumpt_squared = new TH1D("h_HS_best_sumpt_squared", "HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_sumpt_squared->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared,streamName));

  h_HS_best_lPUdz = new TH1D("h_HS_best_lPUdz", "HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_lPUdz->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_lPUdz,streamName));

  h_HS_best_ntracks = new TH1D("h_HS_best_ntracks", "HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_ntracks->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_ntracks,streamName));

  h_HS_best_zPosition = new TH1D("h_HS_best_zPosition", "HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_zPosition->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_zPosition,streamName));

  h_HS_best_sumpt_squared_highest = new TH1D("h_HS_best_sumpt_squared_highest", "HS vertices that have highest #Sigma p_{T}^{2}", 1, 0, 2);
  h_HS_best_sumpt_squared_highest->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared_highest->GetXaxis()->SetTitle("");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared_highest,streamName));

  h_HS_best_sumpt_squared_highest_eta_bin_000_120 = new TH1D("h_HS_best_sumpt_squared_highest_eta_bin_000_120", "HS vertices that have highest #Sigma p_{T}^{2} in region |#eta| < 1.2", 1, 0, 2);
  h_HS_best_sumpt_squared_highest_eta_bin_000_120->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared_highest_eta_bin_000_120->GetXaxis()->SetTitle("");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared_highest_eta_bin_000_120,streamName));

  h_HS_best_sumpt_squared_highest_sumpt_squared = new TH1D("h_HS_best_sumpt_squared_highest_sumpt_squared", "HS vertices that have highest #Sigma p_{T}^{2} vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_sumpt_squared_highest_sumpt_squared->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared_highest_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared_highest_sumpt_squared,streamName));

  h_HS_best_sumpt_squared_highest_lPUdz = new TH1D("h_HS_best_sumpt_squared_highest_lPUdz", "HS vertices that have highest #Sigma p_{T}^{2} vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_sumpt_squared_highest_lPUdz->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared_highest_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared_highest_lPUdz,streamName));

  h_HS_best_sumpt_squared_highest_ntracks = new TH1D("h_HS_best_sumpt_squared_highest_ntracks", "HS vertices that have highest #Sigma p_{T}^{2} vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_sumpt_squared_highest_ntracks->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared_highest_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared_highest_ntracks,streamName));

  h_HS_best_sumpt_squared_highest_zPosition = new TH1D("h_HS_best_sumpt_squared_highest_zPosition", "HS vertices that have highest #Sigma p_{T}^{2} vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_sumpt_squared_highest_zPosition->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_sumpt_squared_highest_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_sumpt_squared_highest_zPosition,streamName));

  h_HS_best_not_sumpt_squared_highest_sumpt_squared = new TH1D("h_HS_best_not_sumpt_squared_highest_sumpt_squared", "HS vertices that do not have highest #Sigma p_{T}^{2} vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_not_sumpt_squared_highest_sumpt_squared->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_not_sumpt_squared_highest_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_not_sumpt_squared_highest_sumpt_squared,streamName));

  h_HS_best_not_sumpt_squared_highest_lPUdz = new TH1D("h_HS_best_not_sumpt_squared_highest_lPUdz", "HS vertices that do not have highest #Sigma p_{T}^{2} vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_not_sumpt_squared_highest_lPUdz->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_not_sumpt_squared_highest_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_not_sumpt_squared_highest_lPUdz,streamName));

  h_HS_best_not_sumpt_squared_highest_ntracks = new TH1D("h_HS_best_not_sumpt_squared_highest_ntracks", "HS vertices that do not have highest #Sigma p_{T}^{2} vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_not_sumpt_squared_highest_ntracks->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_not_sumpt_squared_highest_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_not_sumpt_squared_highest_ntracks,streamName));

  h_HS_best_not_sumpt_squared_highest_zPosition = new TH1D("h_HS_best_not_sumpt_squared_highest_zPosition", "HS vertices that do not have highest #Sigma p_{T}^{2} vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_not_sumpt_squared_highest_zPosition->GetYaxis()->SetTitle("Number of HS vertices");
  h_HS_best_not_sumpt_squared_highest_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_not_sumpt_squared_highest_zPosition,streamName));

  h_nHS_best_Reconstruct = new TH1D("h_nHS_best_Reconstruct", "Number of reconstructed to HS vertices per event", 100, 0.0, 200);
  h_nHS_best_Reconstruct->GetXaxis()->SetTitle("n_{Reconstruct}");
  histoAndStream.push_back(std::make_pair(h_nHS_best_Reconstruct,streamName));

    h_nHS_best_Reconstruct_mu = new TH2D("h_nHS_best_Reconstruct_mu", "Number of reconstructed to HS vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nHS_best_Reconstruct_mu->GetYaxis()->SetTitle("n_{Reconstruct}");
    h_nHS_best_Reconstruct_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nHS_best_Reconstruct_mu,streamName));

  h_HS_best_Reconstruct_sumpt_squared = new TH1D("h_HS_best_Reconstruct_sumpt_squared", "Reconstructed of HS vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_Reconstruct_sumpt_squared->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_HS_best_Reconstruct_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Reconstruct_sumpt_squared,streamName));

  h_HS_best_Reconstruct_lPUdz = new TH1D("h_HS_best_Reconstruct_lPUdz", "Reconstructed to HS vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_Reconstruct_lPUdz->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_HS_best_Reconstruct_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Reconstruct_lPUdz,streamName));

  h_HS_best_Reconstruct_ntracks = new TH1D("h_HS_best_Reconstruct_ntracks", "Reconstructed to HS vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_Reconstruct_ntracks->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_HS_best_Reconstruct_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_Reconstruct_ntracks,streamName));

  h_HS_best_Reconstruct_zPosition = new TH1D("h_HS_best_Reconstruct_zPosition", "Reconstructed to HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_Reconstruct_zPosition->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_HS_best_Reconstruct_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Reconstruct_zPosition,streamName));

  h_HS_best_nMatch = new TH1D("h_HS_best_nMatch", "Number of matched HS vertices per event", 100, 0.0, 200);
  h_HS_best_nMatch->GetXaxis()->SetTitle("n_{Match}");
  histoAndStream.push_back(std::make_pair(h_HS_best_nMatch,streamName));

    h_HS_best_nMatch_mu = new TH2D("h_HS_best_nMatch_mu", "Number of matched HS vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_HS_best_nMatch_mu->GetYaxis()->SetTitle("n_{Match}");
    h_HS_best_nMatch_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_HS_best_nMatch_mu,streamName));

  h_HS_best_Match_sumpt_squared = new TH1D("h_HS_best_Match_sumpt_squared", "Matched HS vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_Match_sumpt_squared->GetYaxis()->SetTitle("n_{Match}");
  h_HS_best_Match_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Match_sumpt_squared,streamName));

  h_HS_best_Match_lPUdz = new TH1D("h_HS_best_Match_lPUdz", "Matched HS vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_Match_lPUdz->GetYaxis()->SetTitle("n_{Match}");
  h_HS_best_Match_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Match_lPUdz,streamName));

  h_HS_best_Match_ntracks = new TH1D("h_HS_best_Match_ntracks", "Matched HS vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_Match_ntracks->GetYaxis()->SetTitle("n_{Match}");
  h_HS_best_Match_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_Match_ntracks,streamName));

  h_HS_best_Match_zPosition = new TH1D("h_HS_best_Match_zPosition", "Matched HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_Match_zPosition->GetYaxis()->SetTitle("n_{Match}");
  h_HS_best_Match_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Match_zPosition,streamName));

  h_HS_best_nMerge = new TH1D("h_HS_best_nMerge", "Number of merged HS vertices per event", 100, 0.0, 200);
  h_HS_best_nMerge->GetXaxis()->SetTitle("n_{Merged}");
  histoAndStream.push_back(std::make_pair(h_HS_best_nMerge,streamName));

    h_HS_best_nMerge_mu = new TH2D("h_HS_best_nMerge_mu", "Number of merged HS vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_HS_best_nMerge_mu->GetYaxis()->SetTitle("n_{Merge}");
    h_HS_best_nMerge_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_HS_best_nMerge_mu,streamName));

  h_HS_best_Merge_sumpt_squared = new TH1D("h_HS_best_Merge_sumpt_squared", "Merged HS vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_Merge_sumpt_squared->GetYaxis()->SetTitle("n_{Merge}");
  h_HS_best_Merge_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Merge_sumpt_squared,streamName));

  h_HS_best_Merge_lPUdz = new TH1D("h_HS_best_Merge_lPUdz", "Merged HS vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_Merge_lPUdz->GetYaxis()->SetTitle("n_{Merge}");
  h_HS_best_Merge_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Merge_lPUdz,streamName));

  h_HS_best_Merge_ntracks = new TH1D("h_HS_best_Merge_ntracks", "Merged HS vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_Merge_ntracks->GetYaxis()->SetTitle("n_{Merge}");
  h_HS_best_Merge_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_Merge_ntracks,streamName));

  h_HS_best_Merge_zPosition = new TH1D("h_HS_best_Merge_zPosition", "Merged HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_Merge_zPosition->GetYaxis()->SetTitle("n_{Merge}");
  h_HS_best_Merge_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Merge_zPosition,streamName));

  h_HS_best_nSplit = new TH1D("h_HS_best_nSplit", "Number of split HS vertices per event", 100, 0.0, 200);
  h_HS_best_nSplit->GetXaxis()->SetTitle("n_{Split}");
  histoAndStream.push_back(std::make_pair(h_HS_best_nSplit,streamName));

    h_HS_best_nSplit_mu = new TH2D("h_HS_best_nSplit_mu", "Number of split HS vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_HS_best_nSplit_mu->GetYaxis()->SetTitle("n_{Split}");
    h_HS_best_nSplit_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_HS_best_nSplit_mu,streamName));

  h_HS_best_Split_sumpt_squared = new TH1D("h_HS_best_Split_sumpt_squared", "Split HS vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_Split_sumpt_squared->GetYaxis()->SetTitle("n_{Split}");
  h_HS_best_Split_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Split_sumpt_squared,streamName));

  h_HS_best_Split_lPUdz = new TH1D("h_HS_best_Split_lPUdz", "Split HS vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_Split_lPUdz->GetYaxis()->SetTitle("n_{Split}");
  h_HS_best_Split_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Split_lPUdz,streamName));

  h_HS_best_Split_ntracks = new TH1D("h_HS_best_Split_ntracks", "Split HS vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_Split_ntracks->GetYaxis()->SetTitle("n_{Split}");
  h_HS_best_Split_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_Split_ntracks,streamName));

  h_HS_best_Split_zPosition = new TH1D("h_HS_best_Split_zPosition", "Split HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_Split_zPosition->GetYaxis()->SetTitle("n_{Split}");
  h_HS_best_Split_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Split_zPosition,streamName));

  h_HS_best_nFake = new TH1D("h_HS_best_nFake", "Number of fake HS vertices per event", 100, 0.0, 200);
  h_HS_best_nFake->GetXaxis()->SetTitle("n_{Fake}");
  histoAndStream.push_back(std::make_pair(h_HS_best_nFake,streamName));

    h_HS_best_nFake_mu = new TH2D("h_HS_best_nFake_mu", "Number of fake HS vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_HS_best_nFake_mu->GetYaxis()->SetTitle("n_{Fake}");
    h_HS_best_nFake_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_HS_best_nFake_mu,streamName));

  h_HS_best_Fake_sumpt_squared = new TH1D("h_HS_best_Fake_sumpt_squared", "Fake HS vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_HS_best_Fake_sumpt_squared->GetYaxis()->SetTitle("n_{Fake}");
  h_HS_best_Fake_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Fake_sumpt_squared,streamName));

  h_HS_best_Fake_lPUdz = new TH1D("h_HS_best_Fake_lPUdz", "Fake HS vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_HS_best_Fake_lPUdz->GetYaxis()->SetTitle("n_{Fake}");
  h_HS_best_Fake_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Fake_lPUdz,streamName));

  h_HS_best_Fake_ntracks = new TH1D("h_HS_best_Fake_ntracks", "Fake HS vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_HS_best_Fake_ntracks->GetYaxis()->SetTitle("n_{Fake}");
  h_HS_best_Fake_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_HS_best_Fake_ntracks,streamName));

  h_HS_best_Fake_zPosition = new TH1D("h_HS_best_Fake_zPosition", "Fake HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_HS_best_Fake_zPosition->GetYaxis()->SetTitle("n_{Fake}");
  h_HS_best_Fake_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_HS_best_Fake_zPosition,streamName));

  //---
  // Plots that depend on truth matching info decorated vertexContainer that have not been flagged as clostest vertex to the true HS vertex.
  //---

  streamName = "VerticesTruthMatching/PUOnly";

  // Class of decorated primary vertex (Matched, Merged, Split or Fake) from truth matching
  h_PU_nClass = new TH1D("h_PU_nClass", "Truth-Match type for reconstructed vertices not clostest to PU", 4, -0.5, 3.5);
  h_PU_nClass->GetXaxis()->SetTitle("Category");
  h_PU_nClass->GetXaxis()->SetBinLabel(1, "Matched");
  h_PU_nClass->GetXaxis()->SetBinLabel(2, "Merged");
  h_PU_nClass->GetXaxis()->SetBinLabel(3, "Split");
  h_PU_nClass->GetXaxis()->SetBinLabel(4, "Fake");
  histoAndStream.push_back(std::make_pair(h_PU_nClass,streamName));

  h_nPU_Reconstruct = new TH1D("h_nPU_Reconstruct", "Number of reconstructed to PU vertices per event", 100, 0.0, 200);
  h_nPU_Reconstruct->GetXaxis()->SetTitle("n_{Reconstruct}");
  histoAndStream.push_back(std::make_pair(h_nPU_Reconstruct,streamName));

    h_nPU_Reconstruct_mu = new TH2D("h_nPU_Reconstruct_mu", "Number of reconstructed to PU vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_nPU_Reconstruct_mu->GetYaxis()->SetTitle("n_{Reconstruct}");
    h_nPU_Reconstruct_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_nPU_Reconstruct_mu,streamName));

  h_PU_Reconstruct_track_eta = new TH1D("h_PU_Reconstruct_track_eta","Tracks at PU vertex #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_PU_Reconstruct_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_track_eta,streamName));

  h_PU_Reconstruct_track_pt = new TH1D("h_PU_Reconstruct_track_pt","Tracks at PU vertex p_{T}", 100, 0.0, 20.0);
  h_PU_Reconstruct_track_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_track_pt,streamName));

  h_PU_Reconstruct_sqrt_sumpt_squared = new TH1D("h_PU_Reconstruct_sqrt_sumpt_squared", "Reconstructed of PU vertices #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx);
  h_PU_Reconstruct_sqrt_sumpt_squared->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_PU_Reconstruct_sqrt_sumpt_squared->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_sqrt_sumpt_squared,streamName));

  h_PU_Reconstruct_sumpt_squared = new TH1D("h_PU_Reconstruct_sumpt_squared", "Reconstructed of PU vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_PU_Reconstruct_sumpt_squared->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_PU_Reconstruct_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_sumpt_squared,streamName));

  h_PU_Reconstruct_lPUdz = new TH1D("h_PU_Reconstruct_lPUdz", "Reconstructed to PU vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_PU_Reconstruct_lPUdz->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_PU_Reconstruct_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_lPUdz,streamName));

  h_PU_Reconstruct_ntracks = new TH1D("h_PU_Reconstruct_ntracks", "Reconstructed to PU vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_PU_Reconstruct_ntracks->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_PU_Reconstruct_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_ntracks,streamName));

  h_PU_Reconstruct_zPosition = new TH1D("h_PU_Reconstruct_zPosition", "Reconstructed to PU vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_PU_Reconstruct_zPosition->GetYaxis()->SetTitle("n_{Reconstruct}");
  h_PU_Reconstruct_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Reconstruct_zPosition,streamName));

  h_PU_nMatch = new TH1D("h_PU_nMatch", "Number of matched PU vertices per event", 100, 0.0, 200);
  h_PU_nMatch->GetXaxis()->SetTitle("n_{Match}");
  histoAndStream.push_back(std::make_pair(h_PU_nMatch,streamName));

    h_PU_nMatch_mu = new TH2D("h_PU_nMatch_mu", "Number of matched PU vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_PU_nMatch_mu->GetYaxis()->SetTitle("n_{Match}");
    h_PU_nMatch_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_PU_nMatch_mu,streamName));

  h_PU_Match_sumpt_squared = new TH1D("h_PU_Match_sumpt_squared", "Matched PU vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_PU_Match_sumpt_squared->GetYaxis()->SetTitle("n_{Match}");
  h_PU_Match_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_PU_Match_sumpt_squared,streamName));

  h_PU_Match_lPUdz = new TH1D("h_PU_Match_lPUdz", "Matched PU vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_PU_Match_lPUdz->GetYaxis()->SetTitle("n_{Match}");
  h_PU_Match_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Match_lPUdz,streamName));

  h_PU_Match_ntracks = new TH1D("h_PU_Match_ntracks", "Matched PU vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_PU_Match_ntracks->GetYaxis()->SetTitle("n_{Match}");
  h_PU_Match_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_PU_Match_ntracks,streamName));

  h_PU_Match_zPosition = new TH1D("h_PU_Match_zPosition", "Matched PU vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_PU_Match_zPosition->GetYaxis()->SetTitle("n_{Match}");
  h_PU_Match_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Match_zPosition,streamName));

  h_PU_nMerge = new TH1D("h_PU_nMerge", "Number of merged PU vertices per event", 100, 0.0, 200);
  h_PU_nMerge->GetXaxis()->SetTitle("n_{Merged}");
  histoAndStream.push_back(std::make_pair(h_PU_nMerge,streamName));

    h_PU_nMerge_mu = new TH2D("h_PU_nMerge_mu", "Number of merged PU vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_PU_nMerge_mu->GetYaxis()->SetTitle("n_{Merge}");
    h_PU_nMerge_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_PU_nMerge_mu,streamName));

  h_PU_Merge_sumpt_squared = new TH1D("h_PU_Merge_sumpt_squared", "Merged PU vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_PU_Merge_sumpt_squared->GetYaxis()->SetTitle("n_{Merge}");
  h_PU_Merge_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_PU_Merge_sumpt_squared,streamName));

  h_PU_Merge_lPUdz = new TH1D("h_PU_Merge_lPUdz", "Merged PU vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_PU_Merge_lPUdz->GetYaxis()->SetTitle("n_{Merge}");
  h_PU_Merge_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Merge_lPUdz,streamName));

  h_PU_Merge_ntracks = new TH1D("h_PU_Merge_ntracks", "Merged PU vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_PU_Merge_ntracks->GetYaxis()->SetTitle("n_{Merge}");
  h_PU_Merge_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_PU_Merge_ntracks,streamName));

  h_PU_Merge_zPosition = new TH1D("h_PU_Merge_zPosition", "Merged PU vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_PU_Merge_zPosition->GetYaxis()->SetTitle("n_{Merge}");
  h_PU_Merge_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Merge_zPosition,streamName));

  h_PU_nSplit = new TH1D("h_PU_nSplit", "Number of split PU vertices per event", 100, 0.0, 200);
  h_PU_nSplit->GetXaxis()->SetTitle("n_{Split}");
  histoAndStream.push_back(std::make_pair(h_PU_nSplit,streamName));

    h_PU_nSplit_mu = new TH2D("h_PU_nSplit_mu", "Number of split PU vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_PU_nSplit_mu->GetYaxis()->SetTitle("n_{Split}");
    h_PU_nSplit_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_PU_nSplit_mu,streamName));

  h_PU_Split_sumpt_squared = new TH1D("h_PU_Split_sumpt_squared", "Split PU vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_PU_Split_sumpt_squared->GetYaxis()->SetTitle("n_{Split}");
  h_PU_Split_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_PU_Split_sumpt_squared,streamName));

  h_PU_Split_lPUdz = new TH1D("h_PU_Split_lPUdz", "Split PU vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_PU_Split_lPUdz->GetYaxis()->SetTitle("n_{Split}");
  h_PU_Split_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Split_lPUdz,streamName));

  h_PU_Split_ntracks = new TH1D("h_PU_Split_ntracks", "Split PU vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_PU_Split_ntracks->GetYaxis()->SetTitle("n_{Split}");
  h_PU_Split_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_PU_Split_ntracks,streamName));

  h_PU_Split_zPosition = new TH1D("h_PU_Split_zPosition", "Split PU vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_PU_Split_zPosition->GetYaxis()->SetTitle("n_{Split}");
  h_PU_Split_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Split_zPosition,streamName));

  h_PU_nFake = new TH1D("h_PU_nFake", "Number of fake PU vertices per event", 100, 0.0, 200);
  h_PU_nFake->GetXaxis()->SetTitle("n_{Fake}");
  histoAndStream.push_back(std::make_pair(h_PU_nFake,streamName));

    h_PU_nFake_mu = new TH2D("h_PU_nFake_mu", "Number of fake PU vertices per event vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_PU_nFake_mu->GetYaxis()->SetTitle("n_{Fake}");
    h_PU_nFake_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_PU_nFake_mu,streamName));

  h_PU_Fake_sumpt_squared = new TH1D("h_PU_Fake_sumpt_squared", "Fake PU vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_PU_Fake_sumpt_squared->GetYaxis()->SetTitle("n_{Fake}");
  h_PU_Fake_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_PU_Fake_sumpt_squared,streamName));

  h_PU_Fake_lPUdz = new TH1D("h_PU_Fake_lPUdz", "Fake PU vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_PU_Fake_lPUdz->GetYaxis()->SetTitle("n_{Fake}");
  h_PU_Fake_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Fake_lPUdz,streamName));

  h_PU_Fake_ntracks = new TH1D("h_PU_Fake_ntracks", "Fake PU vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_PU_Fake_ntracks->GetYaxis()->SetTitle("n_{Fake}");
  h_PU_Fake_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_PU_Fake_ntracks,streamName));

  h_PU_Fake_zPosition = new TH1D("h_PU_Fake_zPosition", "Fake PU vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_PU_Fake_zPosition->GetYaxis()->SetTitle("n_{Fake}");
  h_PU_Fake_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_PU_Fake_zPosition,streamName));

  //---
  // Plots that depend on the primary true vertices from the TruthEventContainer and the TruthPileupEventContainer.
  //---

  streamName = "TruthVertices/All";

  // Number of truth vertices in truth event
  h_truth_nVtx = new TH1D("h_truth_nVtx", "Number of true primary vertices per event", 100, 0.0, 200);
  h_truth_nVtx->GetYaxis()->SetTitle("Events / 2");
  h_truth_nVtx->GetXaxis()->SetTitle("Number of true primary vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_nVtx,streamName));

    h_truth_nVtx_Mu = new TH2D("h_truth_nVtx_Mu", "Number of true primary vertices vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_truth_nVtx_Mu->GetYaxis()->SetTitle("Number of true primary vertices");
    h_truth_nVtx_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_truth_nVtx_Mu,streamName));

  h_truth_Vtx_sumpt_squared = new TH1D("h_truth_Vtx_sumpt_squared", "True primary vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_truth_Vtx_sumpt_squared->GetYaxis()->SetTitle("Number of true primary vertices");
  h_truth_Vtx_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_truth_Vtx_sumpt_squared,streamName));

  h_truth_Vtx_lPUdz = new TH1D("h_truth_Vtx_lPUdz", "True primary vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_truth_Vtx_lPUdz->GetYaxis()->SetTitle("Number of true primary vertices");
  h_truth_Vtx_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_truth_Vtx_lPUdz,streamName));

  h_truth_Vtx_ntracks = new TH1D("h_truth_Vtx_ntracks", "True primary vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_truth_Vtx_ntracks->GetYaxis()->SetTitle("Number of true primary vertices");
  h_truth_Vtx_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_truth_Vtx_ntracks,streamName));

  h_truth_Vtx_zPosition = new TH1D("h_truth_Vtx_zPosition", "True primary vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_truth_Vtx_zPosition->GetYaxis()->SetTitle("Number of true primary vertices");
  h_truth_Vtx_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_truth_Vtx_zPosition,streamName));

  //---
  // Plots that depend only on the primary true vertices from the TruthEventContainer i.e. the HS vertices.
  //---

  streamName = "TruthVertices/HSOnly";

  h_truth_HS_nVtx = new TH1D("h_truth_HS_nVtx", "Number of true HS vertices per event", 100, 0.0, 200);
  h_truth_HS_nVtx->GetYaxis()->SetTitle("Events / 2");
  h_truth_HS_nVtx->GetXaxis()->SetTitle("Number of true HS vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_HS_nVtx,streamName));

    h_truth_HS_nVtx_Mu = new TH2D("h_truth_HS_nVtx_Mu", "Number of true HS vertices vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_truth_HS_nVtx_Mu->GetYaxis()->SetTitle("Number of true HS vertices");
    h_truth_HS_nVtx_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_truth_HS_nVtx_Mu,streamName));

  h_truth_HS_Vtx_sumpt_squared = new TH1D("h_truth_HS_Vtx_sumpt_squared", "True HS vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_truth_HS_Vtx_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_truth_HS_Vtx_sumpt_squared,streamName));

  h_truth_HS_nVtx_accepted = new TH1D("h_truth_HS_nVtx_accepted", "Number of true HS vertices that are accepted in track selection per event", 10, 0.0, 2.0);
  h_truth_HS_nVtx_accepted->GetYaxis()->SetTitle("Events");
  h_truth_HS_nVtx_accepted->GetXaxis()->SetTitle("Number of true HS vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_HS_nVtx_accepted,streamName));

  h_truth_HS_nVtx_accepted_lower_coverage = new TH1D("h_truth_HS_nVtx_accepted_lower_coverage", "Number of true HS vertices that are accepted in track selection per event", 10, 0.0, 2.0);
  h_truth_HS_nVtx_accepted_lower_coverage->GetYaxis()->SetTitle("Events");
  h_truth_HS_nVtx_accepted_lower_coverage->GetXaxis()->SetTitle("Number of true HS vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_HS_nVtx_accepted_lower_coverage,streamName));

  h_truth_HS_Vtx_lPUdz = new TH1D("h_truth_HS_Vtx_lPUdz", "True HS vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_truth_HS_Vtx_lPUdz->GetYaxis()->SetTitle("Number of true primary vertices");
  h_truth_HS_Vtx_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_truth_HS_Vtx_lPUdz,streamName));

  h_truth_HS_Vtx_ntracks = new TH1D("h_truth_HS_Vtx_ntracks", "True HS vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_truth_HS_Vtx_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_truth_HS_Vtx_ntracks,streamName));

  h_truth_HS_Vtx_zPosition = new TH1D("h_truth_HS_Vtx_zPosition", "True HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_truth_HS_Vtx_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_truth_HS_Vtx_zPosition,streamName));

  //---
  // Plots that depend only on the primary true vertices from the TruthPileupEventContainer i.e. the PU vertices.
  //---

  streamName = "TruthVertices/PUOnly";

  h_truth_PU_nVtx = new TH1D("h_truth_PU_nVtx", "Number of true PU vertices per event", 150, 0.0, 300);
  h_truth_PU_nVtx->GetYaxis()->SetTitle("Events / 2");
  h_truth_PU_nVtx->GetXaxis()->SetTitle("Number of true PU vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_PU_nVtx,streamName));

    h_truth_PU_nVtx_Mu = new TH2D("h_truth_PU_nVtx_Mu", "Number of true PU vertices vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 100, 0.0, 200);
    h_truth_PU_nVtx_Mu->GetYaxis()->SetTitle("Number of true PU vertices");
    h_truth_PU_nVtx_Mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_truth_PU_nVtx_Mu,streamName));

  h_truth_PU_Vtx_sumpt_squared = new TH1D("h_truth_PU_Vtx_sumpt_squared", "True PU vertices #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_truth_PU_Vtx_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_truth_PU_Vtx_sumpt_squared,streamName));

  h_truth_PU_nVtx_accepted = new TH1D("h_truth_PU_nVtx_accepted", "Number of true PU vertices that are accepted in track selection per event", 100, 0.0, 200);
  h_truth_PU_nVtx_accepted->GetYaxis()->SetTitle("Events / 2");
  h_truth_PU_nVtx_accepted->GetXaxis()->SetTitle("Number of true PU vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_PU_nVtx_accepted,streamName));

  h_truth_PU_nVtx_accepted_lower_coverage = new TH1D("h_truth_PU_nVtx_accepted_lower_coverage", "Number of true PU vertices that are accepted in track selection per event", 100, 0.0, 200);
  h_truth_PU_nVtx_accepted_lower_coverage->GetYaxis()->SetTitle("Events / 2");
  h_truth_PU_nVtx_accepted_lower_coverage->GetXaxis()->SetTitle("Number of true PU vertices per event");
  histoAndStream.push_back(std::make_pair(h_truth_PU_nVtx_accepted_lower_coverage,streamName));

  h_truth_PU_Vtx_lPUdz = new TH1D("h_truth_PU_Vtx_lPUdz", "True PU vertices Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_truth_PU_Vtx_lPUdz->GetYaxis()->SetTitle("Number of true primary vertices");
  h_truth_PU_Vtx_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_truth_PU_Vtx_lPUdz,streamName));

  h_truth_PU_Vtx_ntracks = new TH1D("h_truth_PU_Vtx_ntracks", "True PU vertices Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_truth_PU_Vtx_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_truth_PU_Vtx_ntracks,streamName));

  h_truth_PU_Vtx_zPosition = new TH1D("h_truth_PU_Vtx_zPosition", "True PU vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_truth_PU_Vtx_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_truth_PU_Vtx_zPosition,streamName));

  h_truth_PU_track_pt = new TH1D("h_truth_PU_track_pt","True PU vertices tracks p_{T}", 100, 0.0, 20.0);
  h_truth_PU_track_pt->GetXaxis()->SetTitle("p_{T} [GeV]");
  histoAndStream.push_back(std::make_pair(h_truth_PU_track_pt,streamName));

  //---
  // Plots that depend on truth matching info decorated vertexContainer and the primary true vertices from the TruthEventContainer and the TruthPileupEventContainer.
  //---

  streamName = "TruthVerticesVsVertices/All";

  // Vertex efficiencies
  h_eff_Vtx_sumpt_squared = new TH1D("h_eff_Vtx_sumpt_squared", "Reconstruction Efficiency vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_eff_Vtx_sumpt_squared->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Vtx_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_eff_Vtx_sumpt_squared,streamName));

  h_eff_Vtx_lPUdz = new TH1D("h_eff_Vtx_lPUdz", "Reconstruction Efficiency vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_eff_Vtx_lPUdz->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Vtx_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_eff_Vtx_lPUdz,streamName));

  h_eff_Vtx_ntracks = new TH1D("h_eff_Vtx_ntracks", "Reconstruction Efficiency vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_eff_Vtx_ntracks->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Vtx_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_eff_Vtx_ntracks,streamName));

  h_eff_Vtx_zPosition = new TH1D("h_eff_Vtx_zPosition", "Reconstruction Efficiency vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_eff_Vtx_zPosition->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Vtx_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_eff_Vtx_zPosition,streamName));

  // Vertex efficiencies (Require Match, Merge or Split tags)
  h_eff_Reconstruct_sumpt_squared = new TH1D("h_eff_Reconstruct_sumpt_squared", "Reconstruction Efficiency vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_eff_Reconstruct_sumpt_squared->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Reconstruct_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_eff_Reconstruct_sumpt_squared,streamName));

  h_eff_Reconstruct_lPUdz = new TH1D("h_eff_Reconstruct_lPUdz", "Reconstruction Efficiency vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_eff_Reconstruct_lPUdz->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Reconstruct_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_eff_Reconstruct_lPUdz,streamName));

  h_eff_Reconstruct_ntracks = new TH1D("h_eff_Reconstruct_ntracks", "Reconstruction Efficiency vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_eff_Reconstruct_ntracks->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Reconstruct_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_eff_Reconstruct_ntracks,streamName));

  h_eff_Reconstruct_zPosition = new TH1D("h_eff_Reconstruct_zPosition", "Reconstruction Efficiency vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_eff_Reconstruct_zPosition->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_Reconstruct_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_eff_Reconstruct_zPosition,streamName));

  // Vertex position biases
  h_bias_Vtx_x = new TH1D("h_bias_Vtx_x","x^{rec} - x^{tru}", 10000, -10.0, 10.0);
  h_bias_Vtx_x->GetXaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Vtx_x,streamName));

    h_bias_Vtx_x_mu = new TH2D("h_bias_Vtx_x_mu", "x^{rec} - x^{tru} vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_x_mu->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Vtx_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_x_mu,streamName));

    h_bias_Vtx_x_sumpt_squared = new TH2D("h_bias_Vtx_x_sumpt_squared", "x^{rec} - x^{tru} vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_x_sumpt_squared->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Vtx_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_x_sumpt_squared,streamName));

    h_bias_Vtx_x_lPUdz = new TH2D("h_bias_Vtx_x_lPUdz", "x^{rec} - x^{tru} vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_x_lPUdz->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Vtx_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_x_lPUdz,streamName));

    h_bias_Vtx_x_ntracks = new TH2D("h_bias_Vtx_x_ntracks", "x^{rec} - x^{tru} vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_x_ntracks->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Vtx_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_x_ntracks,streamName));

    h_bias_Vtx_x_zPosition = new TH2D("h_bias_Vtx_x_zPosition", "x^{rec} - x^{tru} vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_x_zPosition->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Vtx_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_x_zPosition,streamName));

  h_bias_Vtx_y = new TH1D("h_bias_Vtx_y","y^{rec} - y^{tru}", 10000, -10.0, 10.0);
  h_bias_Vtx_y->GetXaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Vtx_y,streamName));

    h_bias_Vtx_y_mu = new TH2D("h_bias_Vtx_y_mu", "y^{rec} - y^{tru} vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_y_mu->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Vtx_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_y_mu,streamName));

    h_bias_Vtx_y_sumpt_squared = new TH2D("h_bias_Vtx_y_sumpt_squared", "y^{rec} - y^{tru} vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_y_sumpt_squared->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Vtx_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_y_sumpt_squared,streamName));

    h_bias_Vtx_y_lPUdz = new TH2D("h_bias_Vtx_y_lPUdz", "y^{rec} - y^{tru} vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_y_lPUdz->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Vtx_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_y_lPUdz,streamName));

    h_bias_Vtx_y_ntracks = new TH2D("h_bias_Vtx_y_ntracks", "y^{rec} - y^{tru} vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_y_ntracks->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Vtx_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_y_ntracks,streamName));

    h_bias_Vtx_y_zPosition = new TH2D("h_bias_Vtx_y_zPosition", "y^{rec} - y^{tru} vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_y_zPosition->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Vtx_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_y_zPosition,streamName));

  h_bias_Vtx_r = new TH1D("h_bias_Vtx_r","r^{rec} - r^{tru}", 10000, -10.0, 10.0);
  h_bias_Vtx_r->GetXaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Vtx_r,streamName));

    h_bias_Vtx_r_mu = new TH2D("h_bias_Vtx_r_mu", "r^{rec} - r^{tru} vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_r_mu->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_Vtx_r_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_r_mu,streamName));

    h_bias_Vtx_r_sumpt_squared = new TH2D("h_bias_Vtx_r_sumpt_squared", "r^{rec} - r^{tru} vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_r_sumpt_squared->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_Vtx_r_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_r_sumpt_squared,streamName));

    h_bias_Vtx_r_lPUdz = new TH2D("h_bias_Vtx_r_lPUdz", "r^{rec} - r^{tru} vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_r_lPUdz->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_Vtx_r_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_r_lPUdz,streamName));

    h_bias_Vtx_r_ntracks = new TH2D("h_bias_Vtx_r_ntracks", "r^{rec} - r^{tru} vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_r_ntracks->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_Vtx_r_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_r_ntracks,streamName));

    h_bias_Vtx_r_zPosition = new TH2D("h_bias_Vtx_r_zPosition", "r^{rec} - r^{tru} vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_r_zPosition->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_Vtx_r_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_r_zPosition,streamName));

  h_bias_Vtx_z = new TH1D("h_bias_Vtx_z","z^{rec} - z^{tru}", 10000, -10.0, 10.0);
  h_bias_Vtx_z->GetXaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Vtx_z,streamName));

    h_bias_Vtx_z_mu = new TH2D("h_bias_Vtx_z_mu", "z^{rec} - z^{tru} vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_z_mu->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Vtx_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_z_mu,streamName));

    h_bias_Vtx_z_sumpt_squared = new TH2D("h_bias_Vtx_z_sumpt_squared", "z^{rec} - z^{tru} vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_z_sumpt_squared->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Vtx_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_z_sumpt_squared,streamName));

    h_bias_Vtx_z_lPUdz = new TH2D("h_bias_Vtx_z_lPUdz", "z^{rec} - z^{tru} vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_z_lPUdz->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Vtx_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_z_lPUdz,streamName));

    h_bias_Vtx_z_ntracks = new TH2D("h_bias_Vtx_z_ntracks", "z^{rec} - z^{tru} vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_z_ntracks->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Vtx_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_z_ntracks,streamName));

    h_bias_Vtx_z_zPosition = new TH2D("h_bias_Vtx_z_zPosition", "z^{rec} - z^{tru} vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Vtx_z_zPosition->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Vtx_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Vtx_z_zPosition,streamName));

  h_bias_Reconstruct_x = new TH1D("h_bias_Reconstruct_x","x^{rec} - x^{tru} (Reconstructed vertices)", 10000, -10.0, 10.0);
  h_bias_Reconstruct_x->GetXaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_x,streamName));

    h_bias_Reconstruct_x_mu = new TH2D("h_bias_Reconstruct_x_mu", "x^{rec} - x^{tru} (Reconstructed vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_x_mu->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Reconstruct_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_x_mu,streamName));

    h_bias_Reconstruct_x_sumpt_squared = new TH2D("h_bias_Reconstruct_x_sumpt_squared", "x^{rec} - x^{tru} (Reconstructed vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_x_sumpt_squared->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Reconstruct_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_x_sumpt_squared,streamName));

    h_bias_Reconstruct_x_lPUdz = new TH2D("h_bias_Reconstruct_x_lPUdz", "x^{rec} - x^{tru} (Reconstructed vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_x_lPUdz->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Reconstruct_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_x_lPUdz,streamName));

    h_bias_Reconstruct_x_ntracks = new TH2D("h_bias_Reconstruct_x_ntracks", "x^{rec} - x^{tru} (Reconstructed vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_x_ntracks->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Reconstruct_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_x_ntracks,streamName));

    h_bias_Reconstruct_x_zPosition = new TH2D("h_bias_Reconstruct_x_zPosition", "x^{rec} - x^{tru} (Reconstructed vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_x_zPosition->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_Reconstruct_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_x_zPosition,streamName));

  h_bias_Reconstruct_y = new TH1D("h_bias_Reconstruct_y","y^{rec} - y^{tru} (Reconstructed vertices)", 10000, -10.0, 10.0);
  h_bias_Reconstruct_y->GetXaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_y,streamName));

    h_bias_Reconstruct_y_mu = new TH2D("h_bias_Reconstruct_y_mu", "y^{rec} - y^{tru} (Reconstructed vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_y_mu->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Reconstruct_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_y_mu,streamName));

    h_bias_Reconstruct_y_sumpt_squared = new TH2D("h_bias_Reconstruct_y_sumpt_squared", "y^{rec} - y^{tru} (Reconstructed vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_y_sumpt_squared->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Reconstruct_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_y_sumpt_squared,streamName));

    h_bias_Reconstruct_y_lPUdz = new TH2D("h_bias_Reconstruct_y_lPUdz", "y^{rec} - y^{tru} (Reconstructed vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_y_lPUdz->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Reconstruct_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_y_lPUdz,streamName));

    h_bias_Reconstruct_y_ntracks = new TH2D("h_bias_Reconstruct_y_ntracks", "y^{rec} - y^{tru} (Reconstructed vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_y_ntracks->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Reconstruct_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_y_ntracks,streamName));

    h_bias_Reconstruct_y_zPosition = new TH2D("h_bias_Reconstruct_y_zPosition", "y^{rec} - y^{tru} (Reconstructed vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_y_zPosition->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_Reconstruct_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_y_zPosition,streamName));

  h_bias_Reconstruct_z = new TH1D("h_bias_Reconstruct_z","z^{rec} - z^{tru} (Reconstructed vertices)", 10000, -10.0, 10.0);
  h_bias_Reconstruct_z->GetXaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_z,streamName));

    h_bias_Reconstruct_z_mu = new TH2D("h_bias_Reconstruct_z_mu", "z^{rec} - z^{tru} (Reconstructed vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_z_mu->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Reconstruct_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_z_mu,streamName));

    h_bias_Reconstruct_z_sumpt_squared = new TH2D("h_bias_Reconstruct_z_sumpt_squared", "z^{rec} - z^{tru} (Reconstructed vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_z_sumpt_squared->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Reconstruct_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_z_sumpt_squared,streamName));

    h_bias_Reconstruct_z_lPUdz = new TH2D("h_bias_Reconstruct_z_lPUdz", "z^{rec} - z^{tru} (Reconstructed vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_z_lPUdz->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Reconstruct_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_z_lPUdz,streamName));

    h_bias_Reconstruct_z_ntracks = new TH2D("h_bias_Reconstruct_z_ntracks", "z^{rec} - z^{tru} (Reconstructed vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_z_ntracks->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Reconstruct_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_z_ntracks,streamName));

    h_bias_Reconstruct_z_zPosition = new TH2D("h_bias_Reconstruct_z_zPosition", "z^{rec} - z^{tru} (Reconstructed vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_Reconstruct_z_zPosition->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_Reconstruct_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_Reconstruct_z_zPosition,streamName));

  // Vertex position resolutions
  h_res_Vtx_x = new TH1D("h_res_Vtx_x","Resolution #sigma(x)", 10, 0.0, 1);
  h_res_Vtx_x->GetXaxis()->SetTitle("Resolution #sigma(x) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Vtx_x,streamName));

    h_res_Vtx_x_mu = new TH1D("h_res_Vtx_x_mu", "Resolution #sigma(x) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Vtx_x_mu->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Vtx_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_x_mu,streamName));

    h_res_Vtx_x_sumpt_squared = new TH1D("h_res_Vtx_x_sumpt_squared", "Resolution #sigma(x) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Vtx_x_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Vtx_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_x_sumpt_squared,streamName));

    h_res_Vtx_x_lPUdz = new TH1D("h_res_Vtx_x_lPUdz", "Resolution #sigma(x) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Vtx_x_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Vtx_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_x_lPUdz,streamName));

    h_res_Vtx_x_ntracks = new TH1D("h_res_Vtx_x_ntracks", "Resolution #sigma(x) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Vtx_x_ntracks->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Vtx_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_x_ntracks,streamName));

    h_res_Vtx_x_zPosition = new TH1D("h_res_Vtx_x_zPosition", "Resolution #sigma(x) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Vtx_x_zPosition->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Vtx_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_x_zPosition,streamName));

  h_res_Vtx_y = new TH1D("h_res_Vtx_y","Resolution #sigma(y)", 10, 0.0, 1);
  h_res_Vtx_y->GetXaxis()->SetTitle("Resolution #sigma(y) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Vtx_y,streamName));

    h_res_Vtx_y_mu = new TH1D("h_res_Vtx_y_mu", "Resolution #sigma(y) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Vtx_y_mu->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Vtx_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_y_mu,streamName));

    h_res_Vtx_y_sumpt_squared = new TH1D("h_res_Vtx_y_sumpt_squared", "Resolution #sigma(y) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Vtx_y_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Vtx_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_y_sumpt_squared,streamName));

    h_res_Vtx_y_lPUdz = new TH1D("h_res_Vtx_y_lPUdz", "Resolution #sigma(y) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Vtx_y_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Vtx_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_y_lPUdz,streamName));

    h_res_Vtx_y_ntracks = new TH1D("h_res_Vtx_y_ntracks", "Resolution #sigma(y) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Vtx_y_ntracks->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Vtx_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_y_ntracks,streamName));

    h_res_Vtx_y_zPosition = new TH1D("h_res_Vtx_y_zPosition", "Resolution #sigma(y) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Vtx_y_zPosition->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Vtx_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_y_zPosition,streamName));

  h_res_Vtx_r = new TH1D("h_res_Vtx_r","Resolution #sigma(r)", 10, 0.0, 1);
  h_res_Vtx_r->GetXaxis()->SetTitle("Resolution #sigma(r) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Vtx_r,streamName));

    h_res_Vtx_r_mu = new TH1D("h_res_Vtx_r_mu", "Resolution #sigma(r) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Vtx_r_mu->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_Vtx_r_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_r_mu,streamName));

    h_res_Vtx_r_sumpt_squared = new TH1D("h_res_Vtx_r_sumpt_squared", "Resolution #sigma(r) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Vtx_r_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_Vtx_r_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_r_sumpt_squared,streamName));

    h_res_Vtx_r_lPUdz = new TH1D("h_res_Vtx_r_lPUdz", "Resolution #sigma(r) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Vtx_r_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_Vtx_r_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_r_lPUdz,streamName));

    h_res_Vtx_r_ntracks = new TH1D("h_res_Vtx_r_ntracks", "Resolution #sigma(r) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Vtx_r_ntracks->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_Vtx_r_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_r_ntracks,streamName));

    h_res_Vtx_r_zPosition = new TH1D("h_res_Vtx_r_zPosition", "Resolution #sigma(r) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Vtx_r_zPosition->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_Vtx_r_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_r_zPosition,streamName));

  h_res_Vtx_z = new TH1D("h_res_Vtx_z","Resolution #sigma(z)", 10, 0.0, 1);
  h_res_Vtx_z->GetXaxis()->SetTitle("Resolution #sigma(z) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Vtx_z,streamName));

    h_res_Vtx_z_mu = new TH1D("h_res_Vtx_z_mu", "Resolution #sigma(z) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Vtx_z_mu->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Vtx_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_z_mu,streamName));

    h_res_Vtx_z_sumpt_squared = new TH1D("h_res_Vtx_z_sumpt_squared", "Resolution #sigma(z) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Vtx_z_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Vtx_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_z_sumpt_squared,streamName));

    h_res_Vtx_z_lPUdz = new TH1D("h_res_Vtx_z_lPUdz", "Resolution #sigma(z) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Vtx_z_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Vtx_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_z_lPUdz,streamName));

    h_res_Vtx_z_ntracks = new TH1D("h_res_Vtx_z_ntracks", "Resolution #sigma(z) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Vtx_z_ntracks->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Vtx_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_z_ntracks,streamName));

    h_res_Vtx_z_zPosition = new TH1D("h_res_Vtx_z_zPosition", "Resolution #sigma(z) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Vtx_z_zPosition->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Vtx_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Vtx_z_zPosition,streamName));

  h_res_Reconstruct_x = new TH1D("h_res_Reconstruct_x","Resolution #sigma(x) (Reconstructed vertices)", 10, 0.0, 1);
  h_res_Reconstruct_x->GetXaxis()->SetTitle("Resolution #sigma(x) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Reconstruct_x,streamName));

    h_res_Reconstruct_x_mu = new TH1D("h_res_Reconstruct_x_mu", "Resolution #sigma(x) (Reconstructed vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Reconstruct_x_mu->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Reconstruct_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_x_mu,streamName));

    h_res_Reconstruct_x_sumpt_squared = new TH1D("h_res_Reconstruct_x_sumpt_squared", "Resolution #sigma(x) (Reconstructed vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Reconstruct_x_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Reconstruct_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_x_sumpt_squared,streamName));

    h_res_Reconstruct_x_lPUdz = new TH1D("h_res_Reconstruct_x_lPUdz", "Resolution #sigma(x) (Reconstructed vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Reconstruct_x_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Reconstruct_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_x_lPUdz,streamName));

    h_res_Reconstruct_x_ntracks = new TH1D("h_res_Reconstruct_x_ntracks", "Resolution #sigma(x) (Reconstructed vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Reconstruct_x_ntracks->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Reconstruct_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_x_ntracks,streamName));

    h_res_Reconstruct_x_zPosition = new TH1D("h_res_Reconstruct_x_zPosition", "Resolution #sigma(x) (Reconstructed vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Reconstruct_x_zPosition->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_Reconstruct_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_x_zPosition,streamName));

  h_res_Reconstruct_y = new TH1D("h_res_Reconstruct_y","Resolution #sigma(y) (Reconstructed vertices)", 10, 0.0, 1);
  h_res_Reconstruct_y->GetXaxis()->SetTitle("Resolution #sigma(y) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Reconstruct_y,streamName));

    h_res_Reconstruct_y_mu = new TH1D("h_res_Reconstruct_y_mu", "Resolution #sigma(y) (Reconstructed vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Reconstruct_y_mu->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Reconstruct_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_y_mu,streamName));

    h_res_Reconstruct_y_sumpt_squared = new TH1D("h_res_Reconstruct_y_sumpt_squared", "Resolution #sigma(y) (Reconstructed vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Reconstruct_y_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Reconstruct_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_y_sumpt_squared,streamName));

    h_res_Reconstruct_y_lPUdz = new TH1D("h_res_Reconstruct_y_lPUdz", "Resolution #sigma(y) (Reconstructed vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Reconstruct_y_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Reconstruct_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_y_lPUdz,streamName));

    h_res_Reconstruct_y_ntracks = new TH1D("h_res_Reconstruct_y_ntracks", "Resolution #sigma(y) (Reconstructed vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Reconstruct_y_ntracks->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Reconstruct_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_y_ntracks,streamName));

    h_res_Reconstruct_y_zPosition = new TH1D("h_res_Reconstruct_y_zPosition", "Resolution #sigma(y) (Reconstructed vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Reconstruct_y_zPosition->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_Reconstruct_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_y_zPosition,streamName));

  h_res_Reconstruct_z = new TH1D("h_res_Reconstruct_z","Resolution #sigma(z) (Reconstructed vertices)", 10, 0.0, 1);
  h_res_Reconstruct_z->GetXaxis()->SetTitle("Resolution #sigma(z) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_Reconstruct_z,streamName));

    h_res_Reconstruct_z_mu = new TH1D("h_res_Reconstruct_z_mu", "Resolution #sigma(z) (Reconstructed vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_Reconstruct_z_mu->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Reconstruct_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_z_mu,streamName));

    h_res_Reconstruct_z_sumpt_squared = new TH1D("h_res_Reconstruct_z_sumpt_squared", "Resolution #sigma(z) (Reconstructed vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_Reconstruct_z_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Reconstruct_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_z_sumpt_squared,streamName));

    h_res_Reconstruct_z_lPUdz = new TH1D("h_res_Reconstruct_z_lPUdz", "Resolution #sigma(z) (Reconstructed vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_Reconstruct_z_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Reconstruct_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_z_lPUdz,streamName));

    h_res_Reconstruct_z_ntracks = new TH1D("h_res_Reconstruct_z_ntracks", "Resolution #sigma(z) (Reconstructed vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_Reconstruct_z_ntracks->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Reconstruct_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_z_ntracks,streamName));

    h_res_Reconstruct_z_zPosition = new TH1D("h_res_Reconstruct_z_zPosition", "Resolution #sigma(z) (Reconstructed vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_Reconstruct_z_zPosition->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_Reconstruct_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_Reconstruct_z_zPosition,streamName));

  streamName = "TruthVerticesVsVertices/HSOnly";

  h_hsMatch_best_closest_z_diffference_x = new TH1D("h_hsMatch_best_closest_z_diffference_x","x^{Truth Matched} - x^{Clostest in z}", 10000, -100.0, 100.0);
  h_hsMatch_best_closest_z_diffference_y = new TH1D("h_hsMatch_best_closest_z_diffference_y","y^{Truth Matched} - y^{Clostest in z}", 10000, -100.0, 100.0);
  h_hsMatch_best_closest_z_diffference_z = new TH1D("h_hsMatch_best_closest_z_diffference_z","z^{Truth Matched} - z^{Clostest in z}", 10000, -100.0, 100.0);

  // Vertex efficiencies (Require Match, Merge or Split tags)
  h_eff_HS_best_Reconstruct_sumpt_squared = new TH1D("h_eff_HS_best_Reconstruct_sumpt_squared", "Reconstruction Efficiency (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_eff_HS_best_Reconstruct_sumpt_squared->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_HS_best_Reconstruct_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_Reconstruct_sumpt_squared,streamName));

  h_eff_HS_best_Reconstruct_lPUdz = new TH1D("h_eff_HS_best_Reconstruct_lPUdz", "Reconstruction Efficiency (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_eff_HS_best_Reconstruct_lPUdz->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_HS_best_Reconstruct_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_Reconstruct_lPUdz,streamName));

  h_eff_HS_best_Reconstruct_ntracks = new TH1D("h_eff_HS_best_Reconstruct_ntracks", "Reconstruction Efficiency (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_eff_HS_best_Reconstruct_ntracks->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_HS_best_Reconstruct_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_Reconstruct_ntracks,streamName));

  h_eff_HS_best_Reconstruct_zPosition = new TH1D("h_eff_HS_best_Reconstruct_zPosition", "Reconstruction Efficiency (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_eff_HS_best_Reconstruct_zPosition->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_HS_best_Reconstruct_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_Reconstruct_zPosition,streamName));

  h_eff_HS_best_is_sumpt_squared_highest = new TH1D("h_eff_HS_best_is_sumpt_squared_highest", "Efficiency that HS vertex has highest #Sigma p_{T}^{2} value", 1, 0, 2);
  h_eff_HS_best_is_sumpt_squared_highest->GetYaxis()->SetTitle("Efficiency");
  h_eff_HS_best_is_sumpt_squared_highest->GetXaxis()->SetTitle("");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_is_sumpt_squared_highest,streamName));

  h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120 = new TH1D("h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120", "Efficiency that HS vertex has highest #Sigma p_{T}^{2} value in region |#eta| < 1.2", 1, 0, 2);
  h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120->GetYaxis()->SetTitle("Efficiency");
  h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120->GetXaxis()->SetTitle("");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120,streamName));

  h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared = new TH1D("h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared", "Efficiency that HS vertex has highest #Sigma p_{T}^{2} value vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared->GetYaxis()->SetTitle("Efficiency");
  h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared,streamName));

  h_eff_HS_best_is_sumpt_squared_highest_lPUdz = new TH1D("h_eff_HS_best_is_sumpt_squared_highest_lPUdz", "Efficiency that HS vertex has highest #Sigma p_{T}^{2} value vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_eff_HS_best_is_sumpt_squared_highest_lPUdz->GetYaxis()->SetTitle("Efficiency");
  h_eff_HS_best_is_sumpt_squared_highest_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_is_sumpt_squared_highest_lPUdz,streamName));

  h_eff_HS_best_is_sumpt_squared_highest_ntracks = new TH1D("h_eff_HS_best_is_sumpt_squared_highest_ntracks", "Efficiency that HS vertex has highest #Sigma p_{T}^{2} value vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_eff_HS_best_is_sumpt_squared_highest_ntracks->GetYaxis()->SetTitle("Efficiency");
  h_eff_HS_best_is_sumpt_squared_highest_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_is_sumpt_squared_highest_ntracks,streamName));

  h_eff_HS_best_is_sumpt_squared_highest_zPosition = new TH1D("h_eff_HS_best_is_sumpt_squared_highest_zPosition", "Efficiency that HS vertex has highest #Sigma p_{T}^{2} value vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_eff_HS_best_is_sumpt_squared_highest_zPosition->GetYaxis()->SetTitle("Efficiency");
  h_eff_HS_best_is_sumpt_squared_highest_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_eff_HS_best_is_sumpt_squared_highest_zPosition,streamName));

  h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared = new TH1D("h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared", "Impurity that HS vertex has highest #Sigma p_{T}^{2} value vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared->GetYaxis()->SetTitle("Efficiency");
  h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared,streamName));

  h_impurity_HS_best_is_sumpt_squared_highest_lPUdz = new TH1D("h_impurity_HS_best_is_sumpt_squared_highest_lPUdz", "Impurity that HS vertex has highest #Sigma p_{T}^{2} value vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_impurity_HS_best_is_sumpt_squared_highest_lPUdz->GetYaxis()->SetTitle("Efficiency");
  h_impurity_HS_best_is_sumpt_squared_highest_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_best_is_sumpt_squared_highest_lPUdz,streamName));

  h_impurity_HS_best_is_sumpt_squared_highest_ntracks = new TH1D("h_impurity_HS_best_is_sumpt_squared_highest_ntracks", "Impurity that HS vertex has highest #Sigma p_{T}^{2} value vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_impurity_HS_best_is_sumpt_squared_highest_ntracks->GetYaxis()->SetTitle("Efficiency");
  h_impurity_HS_best_is_sumpt_squared_highest_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_best_is_sumpt_squared_highest_ntracks,streamName));

  h_impurity_HS_best_is_sumpt_squared_highest_zPosition = new TH1D("h_impurity_HS_best_is_sumpt_squared_highest_zPosition", "Impurity that HS vertex has highest #Sigma p_{T}^{2} value vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_impurity_HS_best_is_sumpt_squared_highest_zPosition->GetYaxis()->SetTitle("Efficiency");
  h_impurity_HS_best_is_sumpt_squared_highest_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_best_is_sumpt_squared_highest_zPosition,streamName));

  // Vertex position biases
  h_bias_HS_best_Reconstruct_x = new TH1D("h_bias_HS_best_Reconstruct_x","x^{rec} - x^{tru} (Reconstructed HS vertices)", 10000, -10.0, 10.0);
  h_bias_HS_best_Reconstruct_x->GetXaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x,streamName));

    h_bias_HS_best_Reconstruct_x_mu = new TH2D("h_bias_HS_best_Reconstruct_x_mu", "x^{rec} - x^{tru} (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_x_mu->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_HS_best_Reconstruct_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x_mu,streamName));

    h_bias_HS_best_Reconstruct_x_sumpt_squared = new TH2D("h_bias_HS_best_Reconstruct_x_sumpt_squared", "x^{rec} - x^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_x_sumpt_squared->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_HS_best_Reconstruct_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x_sumpt_squared,streamName));

    h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks = new TH3D("h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks", "x^{rec} - x^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks->GetZaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks,streamName));

    h_bias_HS_best_Reconstruct_x_lPUdz = new TH2D("h_bias_HS_best_Reconstruct_x_lPUdz", "x^{rec} - x^{tru} (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_x_lPUdz->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_HS_best_Reconstruct_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x_lPUdz,streamName));

    h_bias_HS_best_Reconstruct_x_ntracks = new TH2D("h_bias_HS_best_Reconstruct_x_ntracks", "x^{rec} - x^{tru} (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_x_ntracks->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_HS_best_Reconstruct_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x_ntracks,streamName));

    h_bias_HS_best_Reconstruct_x_zPosition = new TH2D("h_bias_HS_best_Reconstruct_x_zPosition", "x^{rec} - x^{tru} (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_x_zPosition->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_HS_best_Reconstruct_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_x_zPosition,streamName));

  h_bias_HS_best_Reconstruct_y = new TH1D("h_bias_HS_best_Reconstruct_y","y^{rec} - y^{tru} (Reconstructed HS vertices)", 10000, -10.0, 10.0);
  h_bias_HS_best_Reconstruct_y->GetXaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y,streamName));

    h_bias_HS_best_Reconstruct_y_mu = new TH2D("h_bias_HS_best_Reconstruct_y_mu", "y^{rec} - y^{tru} (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_y_mu->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_HS_best_Reconstruct_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y_mu,streamName));

    h_bias_HS_best_Reconstruct_y_sumpt_squared = new TH2D("h_bias_HS_best_Reconstruct_y_sumpt_squared", "y^{rec} - y^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_y_sumpt_squared->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_HS_best_Reconstruct_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y_sumpt_squared,streamName));

    h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks = new TH3D("h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks", "y^{rec} - y^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks->GetZaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks,streamName));

    h_bias_HS_best_Reconstruct_y_lPUdz = new TH2D("h_bias_HS_best_Reconstruct_y_lPUdz", "y^{rec} - y^{tru} (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_y_lPUdz->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_HS_best_Reconstruct_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y_lPUdz,streamName));

    h_bias_HS_best_Reconstruct_y_ntracks = new TH2D("h_bias_HS_best_Reconstruct_y_ntracks", "y^{rec} - y^{tru} (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_y_ntracks->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_HS_best_Reconstruct_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y_ntracks,streamName));

    h_bias_HS_best_Reconstruct_y_zPosition = new TH2D("h_bias_HS_best_Reconstruct_y_zPosition", "y^{rec} - y^{tru} (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_y_zPosition->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_HS_best_Reconstruct_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_y_zPosition,streamName));

  h_bias_HS_best_Reconstruct_r = new TH1D("h_bias_HS_best_Reconstruct_r","r^{rec} - r^{tru} (Reconstructed HS vertices)", 10000, -10.0, 10.0);
  h_bias_HS_best_Reconstruct_r->GetXaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r,streamName));

    h_bias_HS_best_Reconstruct_r_mu = new TH2D("h_bias_HS_best_Reconstruct_r_mu", "r^{rec} - r^{tru} (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_r_mu->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_HS_best_Reconstruct_r_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r_mu,streamName));

    h_bias_HS_best_Reconstruct_r_sumpt_squared = new TH2D("h_bias_HS_best_Reconstruct_r_sumpt_squared", "r^{rec} - r^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_r_sumpt_squared->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_HS_best_Reconstruct_r_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r_sumpt_squared,streamName));

    h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks = new TH3D("h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks", "r^{rec} - r^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks->GetZaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks,streamName));

    h_bias_HS_best_Reconstruct_r_lPUdz = new TH2D("h_bias_HS_best_Reconstruct_r_lPUdz", "r^{rec} - r^{tru} (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_r_lPUdz->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_HS_best_Reconstruct_r_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r_lPUdz,streamName));

    h_bias_HS_best_Reconstruct_r_ntracks = new TH2D("h_bias_HS_best_Reconstruct_r_ntracks", "r^{rec} - r^{tru} (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_r_ntracks->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_HS_best_Reconstruct_r_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r_ntracks,streamName));

    h_bias_HS_best_Reconstruct_r_zPosition = new TH2D("h_bias_HS_best_Reconstruct_r_zPosition", "r^{rec} - r^{tru} (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_r_zPosition->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_HS_best_Reconstruct_r_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_r_zPosition,streamName));

  h_bias_HS_best_Reconstruct_z = new TH1D("h_bias_HS_best_Reconstruct_z","z^{rec} - z^{tru} (Reconstructed HS vertices)", 10000, -10.0, 10.0);
  h_bias_HS_best_Reconstruct_z->GetXaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z,streamName));

    h_bias_HS_best_Reconstruct_z_mu = new TH2D("h_bias_HS_best_Reconstruct_z_mu", "z^{rec} - z^{tru} (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_z_mu->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_HS_best_Reconstruct_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z_mu,streamName));

    h_bias_HS_best_Reconstruct_z_sumpt_squared = new TH2D("h_bias_HS_best_Reconstruct_z_sumpt_squared", "z^{rec} - z^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_z_sumpt_squared->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_HS_best_Reconstruct_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z_sumpt_squared,streamName));

    h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks = new TH3D("h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks", "z^{rec} - z^{tru} (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks->GetZaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks,streamName));

    h_bias_HS_best_Reconstruct_z_lPUdz = new TH2D("h_bias_HS_best_Reconstruct_z_lPUdz", "z^{rec} - z^{tru} (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_z_lPUdz->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_HS_best_Reconstruct_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z_lPUdz,streamName));

    h_bias_HS_best_Reconstruct_z_ntracks = new TH2D("h_bias_HS_best_Reconstruct_z_ntracks", "z^{rec} - z^{tru} (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_z_ntracks->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_HS_best_Reconstruct_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z_ntracks,streamName));

    h_bias_HS_best_Reconstruct_z_zPosition = new TH2D("h_bias_HS_best_Reconstruct_z_zPosition", "z^{rec} - z^{tru} (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_HS_best_Reconstruct_z_zPosition->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_HS_best_Reconstruct_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_HS_best_Reconstruct_z_zPosition,streamName));

  h_res_HS_best_Reconstruct_x = new TH1D("h_res_HS_best_Reconstruct_x","Resolution #sigma(x) (Reconstructed HS vertices)", 10, 0.0, 1);
  h_res_HS_best_Reconstruct_x->GetXaxis()->SetTitle("Resolution #sigma(x) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x,streamName));

    h_res_HS_best_Reconstruct_x_mu = new TH1D("h_res_HS_best_Reconstruct_x_mu", "Resolution #sigma(x) (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_HS_best_Reconstruct_x_mu->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_HS_best_Reconstruct_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x_mu,streamName));

    h_res_HS_best_Reconstruct_x_sumpt_squared = new TH1D("h_res_HS_best_Reconstruct_x_sumpt_squared", "Resolution #sigma(x) (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_HS_best_Reconstruct_x_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_HS_best_Reconstruct_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x_sumpt_squared,streamName));

    h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks = new TH2D("h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks", "Resolution #sigma(x) (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks,streamName));

    h_res_HS_best_Reconstruct_x_lPUdz = new TH1D("h_res_HS_best_Reconstruct_x_lPUdz", "Resolution #sigma(x) (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_HS_best_Reconstruct_x_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_HS_best_Reconstruct_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x_lPUdz,streamName));

    h_res_HS_best_Reconstruct_x_ntracks = new TH1D("h_res_HS_best_Reconstruct_x_ntracks", "Resolution #sigma(x) (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_x_ntracks->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_HS_best_Reconstruct_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x_ntracks,streamName));

    h_res_HS_best_Reconstruct_x_zPosition = new TH1D("h_res_HS_best_Reconstruct_x_zPosition", "Resolution #sigma(x) (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_HS_best_Reconstruct_x_zPosition->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_HS_best_Reconstruct_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_x_zPosition,streamName));

  h_res_HS_best_Reconstruct_y = new TH1D("h_res_HS_best_Reconstruct_y","Resolution #sigma(y) (Reconstructed HS vertices)", 10, 0.0, 1);
  h_res_HS_best_Reconstruct_y->GetXaxis()->SetTitle("Resolution #sigma(y) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y,streamName));

    h_res_HS_best_Reconstruct_y_mu = new TH1D("h_res_HS_best_Reconstruct_y_mu", "Resolution #sigma(y) (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_HS_best_Reconstruct_y_mu->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_HS_best_Reconstruct_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y_mu,streamName));

    h_res_HS_best_Reconstruct_y_sumpt_squared = new TH1D("h_res_HS_best_Reconstruct_y_sumpt_squared", "Resolution #sigma(y) (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_HS_best_Reconstruct_y_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_HS_best_Reconstruct_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y_sumpt_squared,streamName));

    h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks = new TH2D("h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks", "Resolution #sigma(y) (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks,streamName));

    h_res_HS_best_Reconstruct_y_lPUdz = new TH1D("h_res_HS_best_Reconstruct_y_lPUdz", "Resolution #sigma(y) (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_HS_best_Reconstruct_y_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_HS_best_Reconstruct_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y_lPUdz,streamName));

    h_res_HS_best_Reconstruct_y_ntracks = new TH1D("h_res_HS_best_Reconstruct_y_ntracks", "Resolution #sigma(y) (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_y_ntracks->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_HS_best_Reconstruct_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y_ntracks,streamName));

    h_res_HS_best_Reconstruct_y_zPosition = new TH1D("h_res_HS_best_Reconstruct_y_zPosition", "Resolution #sigma(y) (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_HS_best_Reconstruct_y_zPosition->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_HS_best_Reconstruct_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_y_zPosition,streamName));

  h_res_HS_best_Reconstruct_r = new TH1D("h_res_HS_best_Reconstruct_r","Resolution #sigma(r) (Reconstructed HS vertices)", 10, 0.0, 1);
  h_res_HS_best_Reconstruct_r->GetXaxis()->SetTitle("Resolution #sigma(r) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r,streamName));

    h_res_HS_best_Reconstruct_r_mu = new TH1D("h_res_HS_best_Reconstruct_r_mu", "Resolution #sigma(r) (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_HS_best_Reconstruct_r_mu->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_HS_best_Reconstruct_r_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r_mu,streamName));

    h_res_HS_best_Reconstruct_r_sumpt_squared = new TH1D("h_res_HS_best_Reconstruct_r_sumpt_squared", "Resolution #sigma(r) (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_HS_best_Reconstruct_r_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_HS_best_Reconstruct_r_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r_sumpt_squared,streamName));

    h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks = new TH2D("h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks", "Resolution #sigma(r) (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks,streamName));

    h_res_HS_best_Reconstruct_r_lPUdz = new TH1D("h_res_HS_best_Reconstruct_r_lPUdz", "Resolution #sigma(r) (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_HS_best_Reconstruct_r_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_HS_best_Reconstruct_r_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r_lPUdz,streamName));

    h_res_HS_best_Reconstruct_r_ntracks = new TH1D("h_res_HS_best_Reconstruct_r_ntracks", "Resolution #sigma(r) (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_r_ntracks->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_HS_best_Reconstruct_r_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r_ntracks,streamName));

    h_res_HS_best_Reconstruct_r_zPosition = new TH1D("h_res_HS_best_Reconstruct_r_zPosition", "Resolution #sigma(r) (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_HS_best_Reconstruct_r_zPosition->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_HS_best_Reconstruct_r_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_r_zPosition,streamName));

  h_res_HS_best_Reconstruct_z = new TH1D("h_res_HS_best_Reconstruct_z","Resolution #sigma(z) (Reconstructed HS vertices)", 10, 0.0, 1);
  h_res_HS_best_Reconstruct_z->GetXaxis()->SetTitle("Resolution #sigma(z) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z,streamName));

    h_res_HS_best_Reconstruct_z_mu = new TH1D("h_res_HS_best_Reconstruct_z_mu", "Resolution #sigma(z) (Reconstructed HS vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_HS_best_Reconstruct_z_mu->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_HS_best_Reconstruct_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z_mu,streamName));

    h_res_HS_best_Reconstruct_z_sumpt_squared = new TH1D("h_res_HS_best_Reconstruct_z_sumpt_squared", "Resolution #sigma(z) (Reconstructed HS vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_HS_best_Reconstruct_z_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_HS_best_Reconstruct_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z_sumpt_squared,streamName));

    h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks = new TH2D("h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks", "Resolution #sigma(z) (Reconstructed HS vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks,streamName));

    h_res_HS_best_Reconstruct_z_lPUdz = new TH1D("h_res_HS_best_Reconstruct_z_lPUdz", "Resolution #sigma(z) (Reconstructed HS vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_HS_best_Reconstruct_z_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_HS_best_Reconstruct_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z_lPUdz,streamName));

    h_res_HS_best_Reconstruct_z_ntracks = new TH1D("h_res_HS_best_Reconstruct_z_ntracks", "Resolution #sigma(z) (Reconstructed HS vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_HS_best_Reconstruct_z_ntracks->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_HS_best_Reconstruct_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z_ntracks,streamName));

    h_res_HS_best_Reconstruct_z_zPosition = new TH1D("h_res_HS_best_Reconstruct_z_zPosition", "Resolution #sigma(z) (Reconstructed HS vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_HS_best_Reconstruct_z_zPosition->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_HS_best_Reconstruct_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_HS_best_Reconstruct_z_zPosition,streamName));

  h_eff_HS_truth_ntracks_association = new TH1D("h_eff_HS_truth_ntracks_association", "Truth Association Efficiency of number of tracks of HS vertices", 1000, 0.0, 1);
  h_eff_HS_truth_ntracks_association->GetYaxis()->SetTitle("Truth Association Efficiency");
  histoAndStream.push_back(std::make_pair(h_eff_HS_truth_ntracks_association,streamName));

  h_truth_associated_hs_ntracks_lPUdz = new TH2D("h_truth_associated_hs_ntracks_lPUdz", "Number of tracks of HS vertices associated with HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 300, 0.0, 300.0);
  h_truth_associated_hs_ntracks_lPUdz->GetYaxis()->SetTitle("Number of tracks of HS vertices associated with HS vertex");
  h_truth_associated_hs_ntracks_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_truth_associated_hs_ntracks_lPUdz,streamName));

  h_true_hs_ntracks_lPUdz = new TH2D("h_true_hs_ntracks_lPUdz", "Number of tracks associated with HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 300, 0.0, 300.0);
  h_true_hs_ntracks_lPUdz->GetYaxis()->SetTitle("Number of tracks associated with HS vertex");
  h_true_hs_ntracks_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_true_hs_ntracks_lPUdz,streamName));

    h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared = new TH2D("h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared", "Truth Association Efficiency of number of tracks of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared,streamName));

    h_eff_HS_truth_ntracks_association_sumpt_squared = new TH2D("h_eff_HS_truth_ntracks_association_sumpt_squared", "Truth Association Efficiency of number of tracks of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_ntracks_association_sumpt_squared->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_ntracks_association_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_ntracks_association_sumpt_squared,streamName));

    h_eff_HS_truth_ntracks_association_lPUdz = new TH2D("h_eff_HS_truth_ntracks_association_lPUdz", "Truth Association Efficiency of number of tracks of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_ntracks_association_lPUdz->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_ntracks_association_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_ntracks_association_lPUdz,streamName));

    h_eff_HS_truth_ntracks_association_ntracks = new TH2D("h_eff_HS_truth_ntracks_association_ntracks", "Truth Association Efficiency of number of tracks of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_ntracks_association_ntracks->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_ntracks_association_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_ntracks_association_ntracks,streamName));

    h_eff_HS_truth_ntracks_association_zPosition = new TH2D("h_eff_HS_truth_ntracks_association_zPosition", "Truth Association Efficiency of number of tracks of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_ntracks_association_zPosition->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_ntracks_association_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_ntracks_association_zPosition,streamName));

  h_eff_HS_truth_association = new TH1D("h_eff_HS_truth_association", "Truth Association Efficiency of #Sigma p_{T} of HS vertices", 1000, 0.0, 1);
  h_eff_HS_truth_association->GetYaxis()->SetTitle("Truth Association Efficiency");
  histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association,streamName));

    h_eff_HS_truth_association_mu = new TH2D("h_eff_HS_truth_association_mu", "Truth Association Efficiency of #Sigma p_{T} of HS vertices vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_mu->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_mu,streamName));

    h_eff_HS_truth_association_sqrt_sumpt_squared = new TH2D("h_eff_HS_truth_association_sqrt_sumpt_squared", "Truth Association Efficiency of #Sigma p_{T} of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sqrt_sumpt_squared->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sqrt_sumpt_squared->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sqrt_sumpt_squared,streamName));

    h_eff_HS_truth_association_sumpt_squared = new TH2D("h_eff_HS_truth_association_sumpt_squared", "Truth Association Efficiency of #Sigma p_{T} of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sumpt_squared->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sumpt_squared,streamName));

    h_eff_HS_truth_association_lPUdz = new TH2D("h_eff_HS_truth_association_lPUdz", "Truth Association Efficiency of #Sigma p_{T} of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_lPUdz->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_lPUdz,streamName));

    h_eff_HS_truth_association_ntracks = new TH2D("h_eff_HS_truth_association_ntracks", "Truth Association Efficiency of #Sigma p_{T} of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_ntracks->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_ntracks,streamName));

    h_eff_HS_truth_association_zPosition = new TH2D("h_eff_HS_truth_association_zPosition", "Truth Association Efficiency of #Sigma p_{T} of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_zPosition->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_zPosition,streamName));

    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120 = new TH2D("h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120", "Truth Association Efficiency of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120,streamName));

    h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120 = new TH2D("h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120", "Truth Association Efficiency of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120,streamName));

    h_eff_HS_truth_association_lPUdz_eta_bin_000_120 = new TH2D("h_eff_HS_truth_association_lPUdz_eta_bin_000_120", "Truth Association Efficiency of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_lPUdz_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_lPUdz_eta_bin_000_120->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_lPUdz_eta_bin_000_120,streamName));

    h_eff_HS_truth_association_ntracks_eta_bin_000_120 = new TH2D("h_eff_HS_truth_association_ntracks_eta_bin_000_120", "Truth Association Efficiency of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_ntracks_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_ntracks_eta_bin_000_120->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_ntracks_eta_bin_000_120,streamName));

    h_eff_HS_truth_association_zPosition_eta_bin_000_120 = new TH2D("h_eff_HS_truth_association_zPosition_eta_bin_000_120", "Truth Association Efficiency of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_zPosition_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_zPosition_eta_bin_000_120->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_zPosition_eta_bin_000_120,streamName));

    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270 = new TH2D("h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270", "Truth Association Efficiency of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270,streamName));

    h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270 = new TH2D("h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270", "Truth Association Efficiency of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270,streamName));

    h_eff_HS_truth_association_lPUdz_eta_bin_120_270 = new TH2D("h_eff_HS_truth_association_lPUdz_eta_bin_120_270", "Truth Association Efficiency of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_lPUdz_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_lPUdz_eta_bin_120_270->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_lPUdz_eta_bin_120_270,streamName));

    h_eff_HS_truth_association_ntracks_eta_bin_120_270 = new TH2D("h_eff_HS_truth_association_ntracks_eta_bin_120_270", "Truth Association Efficiency of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_ntracks_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_ntracks_eta_bin_120_270->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_ntracks_eta_bin_120_270,streamName));

    h_eff_HS_truth_association_zPosition_eta_bin_120_270 = new TH2D("h_eff_HS_truth_association_zPosition_eta_bin_120_270", "Truth Association Efficiency of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_zPosition_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_zPosition_eta_bin_120_270->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_zPosition_eta_bin_120_270,streamName));

    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340 = new TH2D("h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340", "Truth Association Efficiency of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340,streamName));

    h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340 = new TH2D("h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340", "Truth Association Efficiency of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340,streamName));

    h_eff_HS_truth_association_lPUdz_eta_bin_270_340 = new TH2D("h_eff_HS_truth_association_lPUdz_eta_bin_270_340", "Truth Association Efficiency of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_lPUdz_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_lPUdz_eta_bin_270_340->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_lPUdz_eta_bin_270_340,streamName));

    h_eff_HS_truth_association_ntracks_eta_bin_270_340 = new TH2D("h_eff_HS_truth_association_ntracks_eta_bin_270_340", "Truth Association Efficiency of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_ntracks_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_ntracks_eta_bin_270_340->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_ntracks_eta_bin_270_340,streamName));

    h_eff_HS_truth_association_zPosition_eta_bin_270_340 = new TH2D("h_eff_HS_truth_association_zPosition_eta_bin_270_340", "Truth Association Efficiency of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_zPosition_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_zPosition_eta_bin_270_340->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_zPosition_eta_bin_270_340,streamName));

    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400 = new TH2D("h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400", "Truth Association Efficiency of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400,streamName));

    h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400 = new TH2D("h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400", "Truth Association Efficiency of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400,streamName));

    h_eff_HS_truth_association_lPUdz_eta_bin_340_400 = new TH2D("h_eff_HS_truth_association_lPUdz_eta_bin_340_400", "Truth Association Efficiency of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_lPUdz_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_lPUdz_eta_bin_340_400->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_lPUdz_eta_bin_340_400,streamName));

    h_eff_HS_truth_association_ntracks_eta_bin_340_400 = new TH2D("h_eff_HS_truth_association_ntracks_eta_bin_340_400", "Truth Association Efficiency of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_ntracks_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_ntracks_eta_bin_340_400->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_ntracks_eta_bin_340_400,streamName));

    h_eff_HS_truth_association_zPosition_eta_bin_340_400 = new TH2D("h_eff_HS_truth_association_zPosition_eta_bin_340_400", "Truth Association Efficiency of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_zPosition_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_zPosition_eta_bin_340_400->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_zPosition_eta_bin_340_400,streamName));

    h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest = new TH2D("h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest", "Truth Association Efficiency of #Sigma p_{T} of HS vertices that are also the highest #Sigma p_{T}^{2} vertex vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest,streamName));

    h_eff_HS_truth_association_sumpt_squared_is_highest = new TH2D("h_eff_HS_truth_association_sumpt_squared_is_highest", "Truth Association Efficiency of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_sumpt_squared_is_highest->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_sumpt_squared_is_highest->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_sumpt_squared_is_highest,streamName));

    h_eff_HS_truth_association_lPUdz_is_highest = new TH2D("h_eff_HS_truth_association_lPUdz_is_highest", "Truth Association Efficiency of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_lPUdz_is_highest->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_lPUdz_is_highest->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_lPUdz_is_highest,streamName));

    h_eff_HS_truth_association_ntracks_is_highest = new TH2D("h_eff_HS_truth_association_ntracks_is_highest", "Truth Association Efficiency of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_ntracks_is_highest->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_ntracks_is_highest->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_ntracks_is_highest,streamName));

    h_eff_HS_truth_association_zPosition_is_highest = new TH2D("h_eff_HS_truth_association_zPosition_is_highest", "Truth Association Efficiency of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_eff_HS_truth_association_zPosition_is_highest->GetYaxis()->SetTitle("Truth Association Efficiency");
    h_eff_HS_truth_association_zPosition_is_highest->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_eff_HS_truth_association_zPosition_is_highest,streamName));

  h_impurity_HS_truth_ntrack_association = new TH1D("h_impurity_HS_truth_ntrack_association", "Truth Association Impurity of the number of tracks of HS vertices", 1000, 0.0, 1);
  h_impurity_HS_truth_ntrack_association->GetYaxis()->SetTitle("Truth Association Impurity");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_ntrack_association,streamName));

  h_truth_associated_pu_ntracks_lPUdz = new TH2D("h_truth_associated_pu_ntracks_lPUdz", "Number of tracks of HS vertices associated with PU vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 300, 0.0, 300.0);
  h_truth_associated_pu_ntracks_lPUdz->GetYaxis()->SetTitle("Number of tracks of HS vertices associated with PU vertices");
  h_truth_associated_pu_ntracks_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_truth_associated_pu_ntracks_lPUdz,streamName));

  h_reconstructed_hs_ntracks_lPUdz = new TH2D("h_reconstructed_hs_ntracks_lPUdz", "Number of tracks of HS vertices with truth information vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 300, 0.0, 300.0);
  h_reconstructed_hs_ntracks_lPUdz->GetYaxis()->SetTitle("Number of tracks of HS vertices");
  h_reconstructed_hs_ntracks_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_reconstructed_hs_ntracks_lPUdz,streamName));

    h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared = new TH2D("h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared", "Truth Association Impurity of number of tracks of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared,streamName));

    h_impurity_HS_truth_ntrack_association_sumpt_squared = new TH2D("h_impurity_HS_truth_ntrack_association_sumpt_squared", "Truth Association Impurity of number of tracks of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_ntrack_association_sumpt_squared->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_ntrack_association_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_ntrack_association_sumpt_squared,streamName));

    h_impurity_HS_truth_ntrack_association_lPUdz = new TH2D("h_impurity_HS_truth_ntrack_association_lPUdz", "Truth Association Impurity of number of tracks of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_ntrack_association_lPUdz->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_ntrack_association_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_ntrack_association_lPUdz,streamName));

    h_impurity_HS_truth_ntrack_association_ntracks = new TH2D("h_impurity_HS_truth_ntrack_association_ntracks", "Truth Association Impurity of number of tracks of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_ntrack_association_ntracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_ntrack_association_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_ntrack_association_ntracks,streamName));

    h_impurity_HS_truth_ntrack_association_zPosition = new TH2D("h_impurity_HS_truth_ntrack_association_zPosition", "Truth Association Impurity of number of tracks of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_ntrack_association_zPosition->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_ntrack_association_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_ntrack_association_zPosition,streamName));

  h_impurity_HS_truth_association = new TH1D("h_impurity_HS_truth_association", "Truth Association Impurity of #Sigma p_{T} of HS vertices", 1000, 0.0, 1);
  h_impurity_HS_truth_association->GetYaxis()->SetTitle("Truth Association Impurity");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association,streamName));

    h_impurity_HS_truth_association_mu = new TH2D("h_impurity_HS_truth_association_mu", "Truth Association Impurity of #Sigma p_{T} of HS vertices vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_mu->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_mu,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared", "Truth Association Impurity of #Sigma p_{T} of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared,streamName));

    h_impurity_HS_truth_association_sumpt_squared = new TH2D("h_impurity_HS_truth_association_sumpt_squared", "Truth Association Impurity of #Sigma p_{T} of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared,streamName));

    h_impurity_HS_truth_association_lPUdz = new TH2D("h_impurity_HS_truth_association_lPUdz", "Truth Association Impurity of #Sigma p_{T} of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz,streamName));

    h_impurity_HS_truth_association_ntracks = new TH2D("h_impurity_HS_truth_association_ntracks", "Truth Association Impurity of #Sigma p_{T} of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks,streamName));

    h_impurity_HS_truth_association_zPosition = new TH2D("h_impurity_HS_truth_association_zPosition", "Truth Association Impurity of #Sigma p_{T} of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120 = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120", "Truth Association Impurity of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120,streamName));

    h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120 = new TH2D("h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120", "Truth Association Impurity of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120,streamName));

    h_impurity_HS_truth_association_lPUdz_eta_bin_000_120 = new TH2D("h_impurity_HS_truth_association_lPUdz_eta_bin_000_120", "Truth Association Impurity of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz_eta_bin_000_120->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz_eta_bin_000_120,streamName));

    h_impurity_HS_truth_association_ntracks_eta_bin_000_120 = new TH2D("h_impurity_HS_truth_association_ntracks_eta_bin_000_120", "Truth Association Impurity of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks_eta_bin_000_120->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks_eta_bin_000_120,streamName));

    h_impurity_HS_truth_association_zPosition_eta_bin_000_120 = new TH2D("h_impurity_HS_truth_association_zPosition_eta_bin_000_120", "Truth Association Impurity of #Sigma p_{T} in range |#eta| < 1.2 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition_eta_bin_000_120->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition_eta_bin_000_120->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition_eta_bin_000_120,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270 = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270", "Truth Association Impurity of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270,streamName));

    h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270 = new TH2D("h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270", "Truth Association Impurity of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270,streamName));

    h_impurity_HS_truth_association_lPUdz_eta_bin_120_270 = new TH2D("h_impurity_HS_truth_association_lPUdz_eta_bin_120_270", "Truth Association Impurity of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz_eta_bin_120_270->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz_eta_bin_120_270,streamName));

    h_impurity_HS_truth_association_ntracks_eta_bin_120_270 = new TH2D("h_impurity_HS_truth_association_ntracks_eta_bin_120_270", "Truth Association Impurity of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks_eta_bin_120_270->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks_eta_bin_120_270,streamName));

    h_impurity_HS_truth_association_zPosition_eta_bin_120_270 = new TH2D("h_impurity_HS_truth_association_zPosition_eta_bin_120_270", "Truth Association Impurity of #Sigma p_{T} in range 1.2 < |#eta| < 2.7 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition_eta_bin_120_270->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition_eta_bin_120_270->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition_eta_bin_120_270,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340 = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340", "Truth Association Impurity of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340,streamName));

    h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340 = new TH2D("h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340", "Truth Association Impurity of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340,streamName));

    h_impurity_HS_truth_association_lPUdz_eta_bin_270_340 = new TH2D("h_impurity_HS_truth_association_lPUdz_eta_bin_270_340", "Truth Association Impurity of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz_eta_bin_270_340->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz_eta_bin_270_340,streamName));

    h_impurity_HS_truth_association_ntracks_eta_bin_270_340 = new TH2D("h_impurity_HS_truth_association_ntracks_eta_bin_270_340", "Truth Association Impurity of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks_eta_bin_270_340->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks_eta_bin_270_340,streamName));

    h_impurity_HS_truth_association_zPosition_eta_bin_270_340 = new TH2D("h_impurity_HS_truth_association_zPosition_eta_bin_270_340", "Truth Association Impurity of #Sigma p_{T} in range 2.7 < |#eta| < 3.4 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition_eta_bin_270_340->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition_eta_bin_270_340->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition_eta_bin_270_340,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400 = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400", "Truth Association Impurity of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400,streamName));

    h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400 = new TH2D("h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400", "Truth Association Impurity of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400,streamName));

    h_impurity_HS_truth_association_lPUdz_eta_bin_340_400 = new TH2D("h_impurity_HS_truth_association_lPUdz_eta_bin_340_400", "Truth Association Impurity of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz_eta_bin_340_400->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz_eta_bin_340_400,streamName));

    h_impurity_HS_truth_association_ntracks_eta_bin_340_400 = new TH2D("h_impurity_HS_truth_association_ntracks_eta_bin_340_400", "Truth Association Impurity of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks_eta_bin_340_400->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks_eta_bin_340_400,streamName));

    h_impurity_HS_truth_association_zPosition_eta_bin_340_400 = new TH2D("h_impurity_HS_truth_association_zPosition_eta_bin_340_400", "Truth Association Impurity of #Sigma p_{T} in range 3.4 < |#eta| < 4.0 of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition_eta_bin_340_400->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition_eta_bin_340_400->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition_eta_bin_340_400,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks", "Truth Association Impurity of #Sigma p_{T} for high weight tracks of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks,streamName));

    h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks = new TH2D("h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks", "Truth Association Impurity of #Sigma p_{T} for high weight tracks of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks,streamName));

    h_impurity_HS_truth_association_lPUdz_high_weight_tracks = new TH2D("h_impurity_HS_truth_association_lPUdz_high_weight_tracks", "Truth Association Impurity of #Sigma p_{T} for high weight tracks of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz_high_weight_tracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz_high_weight_tracks->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz_high_weight_tracks,streamName));

    h_impurity_HS_truth_association_ntracks_high_weight_tracks = new TH2D("h_impurity_HS_truth_association_ntracks_high_weight_tracks", "Truth Association Impurity of #Sigma p_{T} for high weight tracks of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks_high_weight_tracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks_high_weight_tracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks_high_weight_tracks,streamName));

    h_impurity_HS_truth_association_zPosition_high_weight_tracks = new TH2D("h_impurity_HS_truth_association_zPosition_high_weight_tracks", "Truth Association Impurity of #Sigma p_{T} for high weight tracks of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition_high_weight_tracks->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition_high_weight_tracks->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition_high_weight_tracks,streamName));

    h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest = new TH2D("h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest", "Truth Association Impurity of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs #sqrt(#Sigma p_{T}^{2})", (sizeof(sumpt_binsx) / sizeof(*sumpt_binsx)) - 1, sumpt_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest->GetXaxis()->SetTitle("#sqrt(#Sigma p_{T}^{2}) [GeV]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest,streamName));

    h_impurity_HS_truth_association_sumpt_squared_is_highest = new TH2D("h_impurity_HS_truth_association_sumpt_squared_is_highest", "Truth Association Impurity of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_sumpt_squared_is_highest->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_sumpt_squared_is_highest->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_sumpt_squared_is_highest,streamName));

    h_impurity_HS_truth_association_lPUdz_is_highest = new TH2D("h_impurity_HS_truth_association_lPUdz_is_highest", "Truth Association Impurity of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_lPUdz_is_highest->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_lPUdz_is_highest->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_lPUdz_is_highest,streamName));

    h_impurity_HS_truth_association_ntracks_is_highest = new TH2D("h_impurity_HS_truth_association_ntracks_is_highest", "Truth Association Impurity of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_ntracks_is_highest->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_ntracks_is_highest->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_ntracks_is_highest,streamName));

    h_impurity_HS_truth_association_zPosition_is_highest = new TH2D("h_impurity_HS_truth_association_zPosition_is_highest", "Truth Association Impurity of #Sigma p_{T} that are also the highest #Sigma p_{T}^{2} vertex of HS vertices vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 1000, 0.0, 1);
    h_impurity_HS_truth_association_zPosition_is_highest->GetYaxis()->SetTitle("Truth Association Impurity");
    h_impurity_HS_truth_association_zPosition_is_highest->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_zPosition_is_highest,streamName));

  h_eff_HS_track_association_eta = new TH1D("h_eff_HS_track_association_eta","Track Association Efficiency of HS vertices vs #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_eff_HS_track_association_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_eff_HS_track_association_eta,streamName));

  h_impurity_HS_track_association_eta = new TH1D("h_impurity_HS_track_association_eta","Track Association Impurity of HS vertices vs #eta", 100, (-1 * m_MaxLayoutEta), m_MaxLayoutEta);
  h_impurity_HS_track_association_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_track_association_eta,streamName));

  /*h_reconstructed_hs_eta = new TH1D("h_reconstructed_hs_eta", "Tracks at HS vertex #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx);
  h_reconstructed_hs_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_reconstructed_hs_eta,streamName));

  h_associated_pu_track_eta = new TH1D("h_associated_pu_track_eta", "Track associated to PU at HS vertex #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx);
  h_associated_pu_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_associated_pu_track_eta,streamName));

  h_impurity_HS_truth_association_eta = new TH1D("h_impurity_HS_truth_association_eta", "Association Impurity of HS vertices vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx);
  h_impurity_HS_truth_association_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_impurity_HS_truth_association_eta,streamName));

  h_reconstructed_pu_eta = new TH1D("h_reconstructed_pu_eta", "Tracks at PU vertex #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx);
  h_reconstructed_pu_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_reconstructed_pu_eta,streamName));

  h_associated_hs_track_eta = new TH1D("h_associated_hs_track_eta", "Track associated to HS at PU vertex #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx);
  h_associated_hs_track_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_associated_hs_track_eta,streamName));

  h_impurity_PU_association_eta = new TH1D("h_impurity_PU_association_eta", "Association Impurity of PU vertices vs #eta", (sizeof(eta_binsx) / sizeof(*eta_binsx)) - 1, eta_binsx);
  h_impurity_PU_association_eta->GetXaxis()->SetTitle("#eta");
  histoAndStream.push_back(std::make_pair(h_impurity_PU_association_eta,streamName));*/

  streamName = "TruthVerticesVsVertices/PUOnly";

  // Vertex efficiencies (Require Reconstruct)
  h_eff_PU_Reconstruct_sumpt_squared = new TH1D("h_eff_PU_Reconstruct_sumpt_squared", "Reconstruction Efficiency (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
  h_eff_PU_Reconstruct_sumpt_squared->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_PU_Reconstruct_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
  histoAndStream.push_back(std::make_pair(h_eff_PU_Reconstruct_sumpt_squared,streamName));

  h_eff_PU_Reconstruct_lPUdz = new TH1D("h_eff_PU_Reconstruct_lPUdz", "Reconstruction Efficiency (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
  h_eff_PU_Reconstruct_lPUdz->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_PU_Reconstruct_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
  histoAndStream.push_back(std::make_pair(h_eff_PU_Reconstruct_lPUdz,streamName));

  h_eff_PU_Reconstruct_ntracks = new TH1D("h_eff_PU_Reconstruct_ntracks", "Reconstruction Efficiency (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
  h_eff_PU_Reconstruct_ntracks->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_PU_Reconstruct_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
  histoAndStream.push_back(std::make_pair(h_eff_PU_Reconstruct_ntracks,streamName));

  h_eff_PU_Reconstruct_zPosition = new TH1D("h_eff_PU_Reconstruct_zPosition", "Reconstruction Efficiency (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
  h_eff_PU_Reconstruct_zPosition->GetYaxis()->SetTitle("Reconstruction Efficiency");
  h_eff_PU_Reconstruct_zPosition->GetXaxis()->SetTitle("z [mm]");
  histoAndStream.push_back(std::make_pair(h_eff_PU_Reconstruct_zPosition,streamName));

  // Vertex position biases
  h_bias_PU_Reconstruct_x = new TH1D("h_bias_PU_Reconstruct_x","x^{rec} - x^{tru} (Reconstructed PU vertices)", 10000, -10.0, 10.0);
  h_bias_PU_Reconstruct_x->GetXaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x,streamName));

    h_bias_PU_Reconstruct_x_mu = new TH2D("h_bias_PU_Reconstruct_x_mu", "x^{rec} - x^{tru} (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_x_mu->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_PU_Reconstruct_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x_mu,streamName));

    h_bias_PU_Reconstruct_x_sumpt_squared = new TH2D("h_bias_PU_Reconstruct_x_sumpt_squared", "x^{rec} - x^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_x_sumpt_squared->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_PU_Reconstruct_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x_sumpt_squared,streamName));

    h_bias_PU_Reconstruct_x_sumpt_squared_ntracks = new TH3D("h_bias_PU_Reconstruct_x_sumpt_squared_ntracks", "x^{rec} - x^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_PU_Reconstruct_x_sumpt_squared_ntracks->GetZaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_PU_Reconstruct_x_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_PU_Reconstruct_x_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x_sumpt_squared_ntracks,streamName));

    h_bias_PU_Reconstruct_x_lPUdz = new TH2D("h_bias_PU_Reconstruct_x_lPUdz", "x^{rec} - x^{tru} (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_x_lPUdz->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_PU_Reconstruct_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x_lPUdz,streamName));

    h_bias_PU_Reconstruct_x_ntracks = new TH2D("h_bias_PU_Reconstruct_x_ntracks", "x^{rec} - x^{tru} (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_x_ntracks->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_PU_Reconstruct_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x_ntracks,streamName));

    h_bias_PU_Reconstruct_x_zPosition = new TH2D("h_bias_PU_Reconstruct_x_zPosition", "x^{rec} - x^{tru} (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_x_zPosition->GetYaxis()->SetTitle("x^{rec} - x^{tru} [mm]");
    h_bias_PU_Reconstruct_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_x_zPosition,streamName));

  h_bias_PU_Reconstruct_y = new TH1D("h_bias_PU_Reconstruct_y","y^{rec} - y^{tru} (Reconstructed PU vertices)", 10000, -10.0, 10.0);
  h_bias_PU_Reconstruct_y->GetXaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y,streamName));

    h_bias_PU_Reconstruct_y_mu = new TH2D("h_bias_PU_Reconstruct_y_mu", "y^{rec} - y^{tru} (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_y_mu->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_PU_Reconstruct_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y_mu,streamName));

    h_bias_PU_Reconstruct_y_sumpt_squared = new TH2D("h_bias_PU_Reconstruct_y_sumpt_squared", "y^{rec} - y^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_y_sumpt_squared->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_PU_Reconstruct_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y_sumpt_squared,streamName));

    h_bias_PU_Reconstruct_y_sumpt_squared_ntracks = new TH3D("h_bias_PU_Reconstruct_y_sumpt_squared_ntracks", "y^{rec} - y^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_PU_Reconstruct_y_sumpt_squared_ntracks->GetZaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_PU_Reconstruct_y_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_PU_Reconstruct_y_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y_sumpt_squared_ntracks,streamName));

    h_bias_PU_Reconstruct_y_lPUdz = new TH2D("h_bias_PU_Reconstruct_y_lPUdz", "y^{rec} - y^{tru} (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_y_lPUdz->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_PU_Reconstruct_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y_lPUdz,streamName));

    h_bias_PU_Reconstruct_y_ntracks = new TH2D("h_bias_PU_Reconstruct_y_ntracks", "y^{rec} - y^{tru} (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_y_ntracks->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_PU_Reconstruct_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y_ntracks,streamName));

    h_bias_PU_Reconstruct_y_zPosition = new TH2D("h_bias_PU_Reconstruct_y_zPosition", "y^{rec} - y^{tru} (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_y_zPosition->GetYaxis()->SetTitle("y^{rec} - y^{tru} [mm]");
    h_bias_PU_Reconstruct_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_y_zPosition,streamName));

  h_bias_PU_Reconstruct_r = new TH1D("h_bias_PU_Reconstruct_r","r^{rec} - r^{tru} (Reconstructed PU vertices)", 10000, -10.0, 10.0);
  h_bias_PU_Reconstruct_r->GetXaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r,streamName));

    h_bias_PU_Reconstruct_r_mu = new TH2D("h_bias_PU_Reconstruct_r_mu", "r^{rec} - r^{tru} (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_r_mu->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_PU_Reconstruct_r_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r_mu,streamName));

    h_bias_PU_Reconstruct_r_sumpt_squared = new TH2D("h_bias_PU_Reconstruct_r_sumpt_squared", "r^{rec} - r^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_r_sumpt_squared->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_PU_Reconstruct_r_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r_sumpt_squared,streamName));

    h_bias_PU_Reconstruct_r_sumpt_squared_ntracks = new TH3D("h_bias_PU_Reconstruct_r_sumpt_squared_ntracks", "r^{rec} - r^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_PU_Reconstruct_r_sumpt_squared_ntracks->GetZaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_PU_Reconstruct_r_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_PU_Reconstruct_r_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r_sumpt_squared_ntracks,streamName));

    h_bias_PU_Reconstruct_r_lPUdz = new TH2D("h_bias_PU_Reconstruct_r_lPUdz", "r^{rec} - r^{tru} (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_r_lPUdz->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_PU_Reconstruct_r_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r_lPUdz,streamName));

    h_bias_PU_Reconstruct_r_ntracks = new TH2D("h_bias_PU_Reconstruct_r_ntracks", "r^{rec} - r^{tru} (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_r_ntracks->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_PU_Reconstruct_r_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r_ntracks,streamName));

    h_bias_PU_Reconstruct_r_zPosition = new TH2D("h_bias_PU_Reconstruct_r_zPosition", "r^{rec} - r^{tru} (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_r_zPosition->GetYaxis()->SetTitle("r^{rec} - r^{tru} [mm]");
    h_bias_PU_Reconstruct_r_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_r_zPosition,streamName));

  h_bias_PU_Reconstruct_z = new TH1D("h_bias_PU_Reconstruct_z","z^{rec} - z^{tru} (Reconstructed PU vertices)", 10000, -10.0, 10.0);
  h_bias_PU_Reconstruct_z->GetXaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
  histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z,streamName));

    h_bias_PU_Reconstruct_z_mu = new TH2D("h_bias_PU_Reconstruct_z_mu", "z^{rec} - z^{tru} (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_z_mu->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_PU_Reconstruct_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z_mu,streamName));

    h_bias_PU_Reconstruct_z_sumpt_squared = new TH2D("h_bias_PU_Reconstruct_z_sumpt_squared", "z^{rec} - z^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_z_sumpt_squared->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_PU_Reconstruct_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z_sumpt_squared,streamName));

    h_bias_PU_Reconstruct_z_sumpt_squared_ntracks = new TH3D("h_bias_PU_Reconstruct_z_sumpt_squared_ntracks", "z^{rec} - z^{tru} (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, (sizeof(bias_binsx) / sizeof(*bias_binsx)) - 1, bias_binsx);
    h_bias_PU_Reconstruct_z_sumpt_squared_ntracks->GetZaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_PU_Reconstruct_z_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_bias_PU_Reconstruct_z_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z_sumpt_squared_ntracks,streamName));

    h_bias_PU_Reconstruct_z_lPUdz = new TH2D("h_bias_PU_Reconstruct_z_lPUdz", "z^{rec} - z^{tru} (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_z_lPUdz->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_PU_Reconstruct_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z_lPUdz,streamName));

    h_bias_PU_Reconstruct_z_ntracks = new TH2D("h_bias_PU_Reconstruct_z_ntracks", "z^{rec} - z^{tru} (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_z_ntracks->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_PU_Reconstruct_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z_ntracks,streamName));

    h_bias_PU_Reconstruct_z_zPosition = new TH2D("h_bias_PU_Reconstruct_z_zPosition", "z^{rec} - z^{tru} (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx, 10000, -10.0, 10.0);
    h_bias_PU_Reconstruct_z_zPosition->GetYaxis()->SetTitle("z^{rec} - z^{tru} [mm]");
    h_bias_PU_Reconstruct_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_bias_PU_Reconstruct_z_zPosition,streamName));

  h_res_PU_Reconstruct_x = new TH1D("h_res_PU_Reconstruct_x","Resolution #sigma(x) (Reconstructed PU vertices)", 10, 0.0, 1);
  h_res_PU_Reconstruct_x->GetXaxis()->SetTitle("Resolution #sigma(x) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x,streamName));

    h_res_PU_Reconstruct_x_mu = new TH1D("h_res_PU_Reconstruct_x_mu", "Resolution #sigma(x) (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_PU_Reconstruct_x_mu->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_PU_Reconstruct_x_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x_mu,streamName));

    h_res_PU_Reconstruct_x_sumpt_squared = new TH1D("h_res_PU_Reconstruct_x_sumpt_squared", "Resolution #sigma(x) (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_PU_Reconstruct_x_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_PU_Reconstruct_x_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x_sumpt_squared,streamName));

    h_res_PU_Reconstruct_x_sumpt_squared_ntracks = new TH2D("h_res_PU_Reconstruct_x_sumpt_squared_ntracks", "Resolution #sigma(x) (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_x_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_PU_Reconstruct_x_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_PU_Reconstruct_x_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x_sumpt_squared_ntracks,streamName));

    h_res_PU_Reconstruct_x_lPUdz = new TH1D("h_res_PU_Reconstruct_x_lPUdz", "Resolution #sigma(x) (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_PU_Reconstruct_x_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_PU_Reconstruct_x_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x_lPUdz,streamName));

    h_res_PU_Reconstruct_x_ntracks = new TH1D("h_res_PU_Reconstruct_x_ntracks", "Resolution #sigma(x) (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_x_ntracks->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_PU_Reconstruct_x_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x_ntracks,streamName));

    h_res_PU_Reconstruct_x_zPosition = new TH1D("h_res_PU_Reconstruct_x_zPosition", "Resolution #sigma(x) (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_PU_Reconstruct_x_zPosition->GetYaxis()->SetTitle("Resolution #sigma(x) [mm]");
    h_res_PU_Reconstruct_x_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_x_zPosition,streamName));

  h_res_PU_Reconstruct_y = new TH1D("h_res_PU_Reconstruct_y","Resolution #sigma(y) (Reconstructed PU vertices)", 10, 0.0, 1);
  h_res_PU_Reconstruct_y->GetXaxis()->SetTitle("Resolution #sigma(y) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y,streamName));

    h_res_PU_Reconstruct_y_mu = new TH1D("h_res_PU_Reconstruct_y_mu", "Resolution #sigma(y) (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_PU_Reconstruct_y_mu->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_PU_Reconstruct_y_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y_mu,streamName));

    h_res_PU_Reconstruct_y_sumpt_squared = new TH1D("h_res_PU_Reconstruct_y_sumpt_squared", "Resolution #sigma(y) (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_PU_Reconstruct_y_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_PU_Reconstruct_y_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y_sumpt_squared,streamName));

    h_res_PU_Reconstruct_y_sumpt_squared_ntracks = new TH2D("h_res_PU_Reconstruct_y_sumpt_squared_ntracks", "Resolution #sigma(y) (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_y_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_PU_Reconstruct_y_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_PU_Reconstruct_y_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y_sumpt_squared_ntracks,streamName));

    h_res_PU_Reconstruct_y_lPUdz = new TH1D("h_res_PU_Reconstruct_y_lPUdz", "Resolution #sigma(y) (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_PU_Reconstruct_y_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_PU_Reconstruct_y_lPUdz->GetXaxis()->SetTitle("Local PU Density [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y_lPUdz,streamName));

    h_res_PU_Reconstruct_y_ntracks = new TH1D("h_res_PU_Reconstruct_y_ntracks", "Resolution #sigma(y) (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_y_ntracks->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_PU_Reconstruct_y_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y_ntracks,streamName));

    h_res_PU_Reconstruct_y_zPosition = new TH1D("h_res_PU_Reconstruct_y_zPosition", "Resolution #sigma(y) (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_PU_Reconstruct_y_zPosition->GetYaxis()->SetTitle("Resolution #sigma(y) [mm]");
    h_res_PU_Reconstruct_y_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_y_zPosition,streamName));

  h_res_PU_Reconstruct_r = new TH1D("h_res_PU_Reconstruct_r","Resolution #sigma(r) (Reconstructed PU vertices)", 10, 0.0, 1);
  h_res_PU_Reconstruct_r->GetXaxis()->SetTitle("Resolution #sigma(r) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r,streamName));

    h_res_PU_Reconstruct_r_mu = new TH1D("h_res_PU_Reconstruct_r_mu", "Resolution #sigma(r) (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_PU_Reconstruct_r_mu->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_PU_Reconstruct_r_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r_mu,streamName));

    h_res_PU_Reconstruct_r_sumpt_squared = new TH1D("h_res_PU_Reconstruct_r_sumpt_squared", "Resolution #sigma(r) (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_PU_Reconstruct_r_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_PU_Reconstruct_r_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r_sumpt_squared,streamName));

    h_res_PU_Reconstruct_r_sumpt_squared_ntracks = new TH2D("h_res_PU_Reconstruct_r_sumpt_squared_ntracks", "Resolution #sigma(r) (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_r_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_PU_Reconstruct_r_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_PU_Reconstruct_r_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r_sumpt_squared_ntracks,streamName));

    h_res_PU_Reconstruct_r_lPUdz = new TH1D("h_res_PU_Reconstruct_r_lPUdz", "Resolution #sigma(r) (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_PU_Reconstruct_r_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_PU_Reconstruct_r_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r_lPUdz,streamName));

    h_res_PU_Reconstruct_r_ntracks = new TH1D("h_res_PU_Reconstruct_r_ntracks", "Resolution #sigma(r) (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_r_ntracks->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_PU_Reconstruct_r_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r_ntracks,streamName));

    h_res_PU_Reconstruct_r_zPosition = new TH1D("h_res_PU_Reconstruct_r_zPosition", "Resolution #sigma(r) (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_PU_Reconstruct_r_zPosition->GetYaxis()->SetTitle("Resolution #sigma(r) [mm]");
    h_res_PU_Reconstruct_r_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_r_zPosition,streamName));

  h_res_PU_Reconstruct_z = new TH1D("h_res_PU_Reconstruct_z","Resolution #sigma(z) (Reconstructed PU vertices)", 10, 0.0, 1);
  h_res_PU_Reconstruct_z->GetXaxis()->SetTitle("Resolution #sigma(z) [mm]");
  histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z,streamName));

    h_res_PU_Reconstruct_z_mu = new TH1D("h_res_PU_Reconstruct_z_mu", "Resolution #sigma(z) (Reconstructed PU vertices) vs #mu", (sizeof(mu_binsx) / sizeof(*mu_binsx)) - 1, mu_binsx);
    h_res_PU_Reconstruct_z_mu->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_PU_Reconstruct_z_mu->GetXaxis()->SetTitle("#mu");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z_mu,streamName));

    h_res_PU_Reconstruct_z_sumpt_squared = new TH1D("h_res_PU_Reconstruct_z_sumpt_squared", "Resolution #sigma(z) (Reconstructed PU vertices) vs #Sigma p_{T}^{2}", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx);
    h_res_PU_Reconstruct_z_sumpt_squared->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_PU_Reconstruct_z_sumpt_squared->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z_sumpt_squared,streamName));

    h_res_PU_Reconstruct_z_sumpt_squared_ntracks = new TH2D("h_res_PU_Reconstruct_z_sumpt_squared_ntracks", "Resolution #sigma(z) (Reconstructed PU vertices) vs #Sigma p_{T}^{2} and Number of tracks at vertex", (sizeof(sumpt_squared_binsx) / sizeof(*sumpt_squared_binsx)) - 1, sumpt_squared_binsx, (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_z_sumpt_squared_ntracks->GetZaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_PU_Reconstruct_z_sumpt_squared_ntracks->GetYaxis()->SetTitle("Number of tracks at vertex");
    h_res_PU_Reconstruct_z_sumpt_squared_ntracks->GetXaxis()->SetTitle("#Sigma p_{T}^{2} [GeV^{2}]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z_sumpt_squared_ntracks,streamName));

    h_res_PU_Reconstruct_z_lPUdz = new TH1D("h_res_PU_Reconstruct_z_lPUdz", "Resolution #sigma(z) (Reconstructed PU vertices) vs Average Local Pile-up Density", (sizeof(lPUdz_binsx) / sizeof(*lPUdz_binsx)) - 1, lPUdz_binsx);
    h_res_PU_Reconstruct_z_lPUdz->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_PU_Reconstruct_z_lPUdz->GetXaxis()->SetTitle("Local PU Densitz [Events / mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z_lPUdz,streamName));

    h_res_PU_Reconstruct_z_ntracks = new TH1D("h_res_PU_Reconstruct_z_ntracks", "Resolution #sigma(z) (Reconstructed PU vertices) vs Number of tracks at vertex", (sizeof(ntracks_binsx) / sizeof(*ntracks_binsx)) - 1, ntracks_binsx);
    h_res_PU_Reconstruct_z_ntracks->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_PU_Reconstruct_z_ntracks->GetXaxis()->SetTitle("Number of tracks at vertex");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z_ntracks,streamName));

    h_res_PU_Reconstruct_z_zPosition = new TH1D("h_res_PU_Reconstruct_z_zPosition", "Resolution #sigma(z) (Reconstructed PU vertices) vs Z position of vertex", (sizeof(zPosition_binsx) / sizeof(*zPosition_binsx)) - 1, zPosition_binsx);
    h_res_PU_Reconstruct_z_zPosition->GetYaxis()->SetTitle("Resolution #sigma(z) [mm]");
    h_res_PU_Reconstruct_z_zPosition->GetXaxis()->SetTitle("z [mm]");
    histoAndStream.push_back(std::make_pair(h_res_PU_Reconstruct_z_zPosition,streamName));

  return histoAndStream;
}

int VertexAnalysis::execute(const xAOD::VertexContainer &vertexContainer,
                            const xAOD::TruthVertexContainer &truthvertexContainer, 
                            const xAOD::EventInfo &eventInfo,
                            double m_muCorrected,
                            double m_muWeigth,
                            const xAOD::TrackParticleContainer &trackContainer,
                            const xAOD::TruthEventContainer &truthEventContainer,
                            const xAOD::TruthPileupEventContainer &truthPileupEventContainer,
                            xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_loose,
                            xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_tight,
                            bool Truthevents_available,
                            bool TruthPUevents_available) {

  // This namespace has a few useful things from truth matching code typedefs for the decoration types, enums of vertex classification, and helper analysis methods
  using namespace InDetVertexTruthMatchUtils;

  double m_muUnCorrected = eventInfo.actualInteractionsPerCrossing();

  // Up the event count.
  m_entryNumber++;
  h_run_number->Fill(eventInfo.runNumber());
  h_event_number->Fill(eventInfo.eventNumber());

  //cout << "eventNumber = " << eventInfo.eventNumber() << " number " << endl;

  //---
  // Reconstructed Tracks loop from InDetTrackParticles Container
  //---

  size_t trackparticleContainerSize = trackContainer.size();
  double vtx_sumpt_squared_highest = 0;
  int vtx_sumpt_squared_highest_index = -1;

  double vtx_sumpt_squared_highest_eta_bin_000_120 = 0;
  int vtx_sumpt_squared_highest_eta_bin_000_120_index = -1;

  double vtx_sumpt_squared_highest_high_weight = 0;
  int vtx_sumpt_squared_highest_high_weight_index = -1;

  // Fill information from ALL tracks from InDetTrackParticles
  for(size_t i = 0; i < trackparticleContainerSize; ++i) {
    if (trackContainer[i] == 0) continue;

    // pt cut for reconstruction track selection
    if (trackContainer[i]->pt() < 500) continue;
    h_all_track_eta->Fill(trackContainer[i]->eta());
    h_all_track_pt->Fill(trackContainer[i]->pt() / 1000.);

    const xAOD::ParametersCovMatrix_t covTrk = trackContainer[i]->definingParametersCovMatrix();

    h_all_track_d0->Fill(trackContainer[i]->d0());
    h_all_track_d0_eta->Fill(trackContainer[i]->eta(), trackContainer[i]->d0());

    h_all_track_err_d0->Fill(Amg::error(covTrk, 0));
    h_all_track_err_d0_eta->Fill(trackContainer[i]->eta(), Amg::error(covTrk, 0));

    h_all_track_d0_over_err_d0->Fill(trackContainer[i]->d0() / Amg::error(covTrk,0));
    h_all_track_d0_over_err_d0_eta->Fill(trackContainer[i]->eta(), trackContainer[i]->d0() / Amg::error(covTrk,0));

    h_all_track_z0->Fill(trackContainer[i]->z0());
    h_all_track_z0_eta->Fill(trackContainer[i]->eta(), trackContainer[i]->z0());

    h_all_track_err_z0->Fill(Amg::error(covTrk, 1));
    h_all_track_err_z0_eta->Fill(trackContainer[i]->eta(), Amg::error(covTrk, 1));

    h_all_track_z0_over_err_z0->Fill(trackContainer[i]->z0() / Amg::error(covTrk,1));
    h_all_track_z0_over_err_z0_eta->Fill(trackContainer[i]->eta(), trackContainer[i]->z0() / Amg::error(covTrk,1));

  }

  //---
  // Reconstructed Vertex loop from PrimaryVertices Container
  //---

  xAOD::Vertex::ConstAccessor<xAOD::Vertex::TrackParticleLinks_t> trkAcc("trackParticleLinks");
  xAOD::Vertex::ConstAccessor<std::vector<float> > weightAcc("trackWeights");

  int nVtx = 0;
  size_t vertexContainerSize = vertexContainer.size();
  for(size_t i = 0; i < vertexContainerSize; ++i) {
    if(xAOD::VxType::NoVtx == vertexContainer[i]->vertexType()) continue;

    // Increase vertex counter
    nVtx++;
    //cout << "nVtx = " << nVtx << endl;

    // Fill position plots
    h_x->Fill(vertexContainer[i]->x());
    h_y->Fill(vertexContainer[i]->y());
    h_z->Fill(vertexContainer[i]->z());

    double lPUdz = exp(-0.5 * vertexContainer[i]->z() * vertexContainer[i]->z() / lPUd_factor / lPUd_factor) / lPUd_factor / sqrt(6.28) * lPUd_mu;

    // Fill error plots

    //if (vertexContainer[i]->isAvailable<AmgSymMatrix(3)>("covariancePosition")) {
    //if (eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION)) {
      const AmgSymMatrix(3)& covariance = vertexContainer[i]->covariancePosition();
      //double vtx_sigma_x = Amg::error(covariance,0);
      //double vtx_sigma_y = Amg::error(covariance,1);
      //double vtx_sigma_z = Amg::error(covariance,2);

      h_x_err->Fill(Amg::error(covariance,0));
      h_y_err->Fill(Amg::error(covariance,1));
      h_z_err->Fill(Amg::error(covariance,2));
    //}

    h_lPUdz->Fill(lPUdz);

    if(vertexContainerSize > 1){
      for(size_t j = i + 1; j < vertexContainerSize; ++j) {
        if(xAOD::VxType::NoVtx != vertexContainer[j]->vertexType()) {  
          h_Dz->Fill(vertexContainer[j]->z() - vertexContainer[i]->z());
          h_Dz_Mu->Fill(m_muCorrected, vertexContainer[j]->z() - vertexContainer[i]->z(), m_muWeigth);
        }
      }
    }

    // Fill vertex tracks properties
    int nTracks = vertexContainer[i]->nTrackParticles();
    h_ntracks->Fill(nTracks);
    if (vertexContainer[i]->isAvailable<std::vector<float>>("trackWeights")) {
      for (const float& weight : vertexContainer[i]->trackWeights()) {
       h_track_weights->Fill(weight);
      }
    }

    double vtx_sumpt_squared = 0;
    double vtx_sumpt_squared_eta_bin_000_120 = 0;
    double vtx_sumpt_squared_high_weight = 0;
    int trackWeighti = 0;
    // Loop over tracks at Reconstructed Vertex
    for (const auto& elTrk : vertexContainer[i]->trackParticleLinks()) {
      const xAOD::TrackParticle* trk = *elTrk;
      
      // pt cut for reconstruction track selection
      if (trk->pt() < 500) continue;
      h_track_pt->Fill(trk->pt() / 1000.); // MeV -> GeV
      vtx_sumpt_squared = vtx_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
      if(vertexContainer[i]->trackWeight(trackWeighti) > 0.5){
        vtx_sumpt_squared_high_weight = vtx_sumpt_squared_high_weight + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
      }
      if(trk->eta() > -1.2 && trk->eta() < 1.2) {
        vtx_sumpt_squared_eta_bin_000_120 = vtx_sumpt_squared_eta_bin_000_120 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
      }
      const xAOD::ParametersCovMatrix_t covTrk = trk->definingParametersCovMatrix();
      
      // Fill general track properties
      h_track_eta->Fill(trk->eta());
      h_track_d0->Fill(trk->d0());
      h_track_err_d0->Fill(Amg::error(covTrk, 0));
      h_track_d0_over_err_d0->Fill(trk->d0() / Amg::error(covTrk,0));
      h_track_d0_over_err_d0_Mu->Fill(m_muCorrected, trk->d0() / Amg::error(covTrk,0), m_muWeigth);
      
      h_track_z0->Fill(trk->z0());
      h_track_err_z0->Fill(Amg::error(covTrk, 1));
      h_track_err_z0_Mu->Fill(m_muCorrected,Amg::error(covTrk, 1));
      h_track_err_z0_eta->Fill(trk->eta(),Amg::error(covTrk, 1));

      h_track_z0_over_err_z0->Fill(trk->z0() / Amg::error(covTrk,1));
      h_track_z0_Vtx_z0->Fill(trk->z0() - vertexContainer[i]->z());

      h_ntracks_Mu->Fill(m_muCorrected, vertexContainer[i]->z(), m_muWeigth);

      bool successfulRetrieval(false);
      uint8_t iPixHits, iSctHits, iPixHoles, iSctHoles;
      successfulRetrieval = trk->summaryValue(iPixHits, xAOD::numberOfPixelHits);
      successfulRetrieval &= trk->summaryValue(iSctHits, xAOD::numberOfSCTHits);
      if (successfulRetrieval) h_track_nSiHits->Fill(iPixHits+iSctHits);
      successfulRetrieval = trk->summaryValue(iPixHoles, xAOD::numberOfPixelHoles);
      successfulRetrieval &= trk->summaryValue(iSctHoles, xAOD::numberOfSCTHoles);
      if (successfulRetrieval) h_track_nSiHoles->Fill(iPixHoles+iSctHoles);

      // Get truth particle associated with a track particle
      typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
      const xAOD::TruthParticle * tresult = 0;
      // Is there any truth...?
      if (trk->isAvailable<ElementTruthLink_t>("truthParticleLink")) {
        // ...then get link
        const ElementTruthLink_t ptruthContainer= trk->auxdata<ElementTruthLink_t>("truthParticleLink");
        if (ptruthContainer.isValid()){
          //cout << "Truth Track Link Available" << endl;
          tresult= *ptruthContainer;
        }
      }

      const xAOD::TruthParticle *associatedTruthT = tresult;

      // Track pull values
      if(associatedTruthT){
        std::vector<string> m_paramNames {"d0","z0", "phi", "theta","qOverP"};
        int nParm = m_paramNames.size();
        for (int i = 0; i < nParm; i++){
          const std::string errName = m_paramNames[i] + std::string("err");
          const bool truthIsAvailable = associatedTruthT->isAvailable<float>(m_paramNames[i]);
          const bool sigmaAvailable = associatedTruthT->isAvailable<float>(errName);
          if(truthIsAvailable and sigmaAvailable){
            //cout << "Truth pull values are available" << endl;
            float track_res = trk->auxdata<float>(m_paramNames[i]) - associatedTruthT->auxdata< float >(m_paramNames[i]);
            float track_sigma = trk->auxdata<float>(errName);
            float track_pull = track_res / track_sigma;
            if(m_paramNames[i] == "d0"){h_track_bias_d0->Fill(track_res);h_track_pull_d0->Fill(track_pull);}
            if(m_paramNames[i] == "z0"){h_track_bias_z0->Fill(track_res);h_track_pull_z0->Fill(track_pull);}
            if(m_paramNames[i] == "phi"){h_track_bias_phi->Fill(track_res);h_track_pull_phi->Fill(track_pull);}
          }
        }
      }
      trackWeighti++;
    }

    if(vtx_sumpt_squared_highest < vtx_sumpt_squared){
      vtx_sumpt_squared_highest = vtx_sumpt_squared;
      vtx_sumpt_squared_highest_index = vertexContainer[i]->index();
    }
    if(vtx_sumpt_squared_highest_eta_bin_000_120 < vtx_sumpt_squared_eta_bin_000_120){
      vtx_sumpt_squared_highest_eta_bin_000_120 = vtx_sumpt_squared_eta_bin_000_120;
      vtx_sumpt_squared_highest_eta_bin_000_120_index = vertexContainer[i]->index();
    }
    if(vtx_sumpt_squared_highest_high_weight < vtx_sumpt_squared_high_weight){
      vtx_sumpt_squared_highest_high_weight = vtx_sumpt_squared_high_weight;
      vtx_sumpt_squared_highest_high_weight_index = vertexContainer[i]->index();
    }

    // Fill general vertex properties
    h_Vtx_sumpt_squared->Fill(vtx_sumpt_squared);
    h_Vtx_lPUdz->Fill(lPUdz);
    h_Vtx_ntracks->Fill(nTracks);
    h_Vtx_zPosition->Fill(vertexContainer[i]->z());

    // Fill vertex quality and type
    h_type->Fill(vertexContainer[i]->vertexType());
    float ndf = 0;
    if (vertexContainer[i]->isAvailable<float>("numberDoF")) {ndf = vertexContainer[i]->numberDoF();}
    if (ndf != 0) {
      h_chi2_over_ndf->Fill(vertexContainer[i]->chiSquared() / ndf);
      h_chi2_over_ndf_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->chiSquared() / ndf);
      h_chi2_over_ndf_lPUdz->Fill(lPUdz, vertexContainer[i]->chiSquared() / ndf);
      h_chi2_over_ndf_ntracks->Fill(nTracks, vertexContainer[i]->chiSquared() / ndf);
      h_chi2_over_ndf_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->chiSquared() / ndf);
    }
    else {h_chi2_over_ndf->Fill(-1);}
  }

  h_nVtx->Fill(nVtx);
  h_nVtx_Mu->Fill(m_muCorrected,nVtx, m_muWeigth);
  h_mu_Corrected->Fill(m_muCorrected, m_muWeigth);
  h_mu_UnCorrected->Fill(m_muUnCorrected, m_muWeigth);

  /*if(vertexContainerSize > 1){
    for(size_t i = 0; i < vertexContainerSize; ++i) {
      if(xAOD::VxType::NoVtx != vertexContainer[i]->vertexType() && vtx_sumpt_squared_highest_index != i) {  
        h_highest_pt_Dz->Fill(vertexContainer[i]->z() - vertexContainer[vtx_sumpt_squared_highest_index]->z());
      }
    }
  }*/

  //---
  // Plots that depend on matching info decorated vertexContainer.
  //---

  // Decorators needed for vertex analysis.
  xAOD::Vertex::Decorator< InDetVertexTruthMatchUtils::VertexMatchType > matchTypeDecorator("VertexMatchType");
  xAOD::Vertex::Decorator< std::vector< InDetVertexTruthMatchUtils::VertexTruthMatchInfo > > truthMatchInfoDecorator("TruthEventMatchingInfos");
  xAOD::Vertex::Decorator< std::vector< ElementLink< xAOD::VertexContainer > > > splitPartnerDecorator("SplitPartners");

  //check that there is more than just a dummy vertex, and that it has matching information
  bool matchingDone = ( vertexContainer.size() > 0 &&
                        vertexContainer[0]->vertexType() != xAOD::VxType::NoVtx &&
                        matchTypeDecorator.isAvailable(*(vertexContainer[0])) );

  if(matchingDone == true){
    const xAOD::Vertex *hsMatch_best = InDetVertexTruthMatchUtils::bestHardScatterMatch( vertexContainer );
    //const xAOD::Vertex *hsMatch_best_tmp = InDetVertexTruthMatchUtils::bestHardScatterMatch( vertexContainer );

    //---
    // Plots that depend on Truth Vertices from TruthEventContainer
    //---

    int truth_nVtx = 0;

    // Get HS vertex from Truth Event Container
    double sumpt_squared_HS_truth = 0;
    auto true_HS_Event = truthEventContainer.at(0);
    int nTruthParticles_HS = true_HS_Event->nTruthParticles();
    auto true_HS_vtx = true_HS_Event->truthVertex(1);
    if(true_HS_vtx->t() != 0) {
      cout << "true_HS_vtx->t() = " << true_HS_vtx->t() << endl;
    }
    int index_hs_map[nTruthParticles_HS] = {0};
    int counter_hs = 0;
    truth_nVtx++;
    double true_HS_vtx_z = true_HS_vtx->z();
    for (int i = 0; i < nTruthParticles_HS; i++){
      if(true_HS_Event->truthParticle(i)->barcode() < 200000 && true_HS_Event->truthParticle(i)->status() == 1 && true_HS_Event->truthParticle(i)->isCharged()){
        sumpt_squared_HS_truth = sumpt_squared_HS_truth + ((true_HS_Event->truthParticle(i)->pt() / 1000.) * (true_HS_Event->truthParticle(i)->pt() / 1000.));
        index_hs_map[counter_hs] = true_HS_Event->truthParticle(i)->index();
        counter_hs++;
      }
    }
    // Find the reco vertex that is closest to the true HS vertex in z direction
    double closest_to_true_HS_z_first = 999999;
    double closest_to_true_HS_z_second = 999999;
    double closest_to_true_HS_z_third = 999999;
    int closest_to_true_HS_z_first_index = -1;
    int closest_to_true_HS_z_second_index = -1;
    int closest_to_true_HS_z_third_index = -1;
    for(size_t i = 0; i < vertexContainerSize; ++i) {
      if(xAOD::VxType::NoVtx == vertexContainer[i]->vertexType()) continue;
        double Difference_z = sqrt((true_HS_vtx_z - vertexContainer[i]->z()) * (true_HS_vtx_z - vertexContainer[i]->z()));
        if(Difference_z < closest_to_true_HS_z_first) {
          closest_to_true_HS_z_first = Difference_z;
          closest_to_true_HS_z_first_index = i;
        }
    }
    for(size_t i = 0; i < vertexContainerSize; ++i) {
      if(xAOD::VxType::NoVtx == vertexContainer[i]->vertexType()) continue;
        double Difference_z = sqrt((true_HS_vtx_z - vertexContainer[i]->z()) * (true_HS_vtx_z - vertexContainer[i]->z()));
        if((Difference_z < closest_to_true_HS_z_second) && (Difference_z != closest_to_true_HS_z_first)) {
          closest_to_true_HS_z_second = Difference_z;
          closest_to_true_HS_z_second_index = i;
        }
    }
    for(size_t i = 0; i < vertexContainerSize; ++i) {
      if(xAOD::VxType::NoVtx == vertexContainer[i]->vertexType()) continue;
        double Difference_z = sqrt((true_HS_vtx_z - vertexContainer[i]->z()) * (true_HS_vtx_z - vertexContainer[i]->z()));
        if((Difference_z < closest_to_true_HS_z_third) && (Difference_z != closest_to_true_HS_z_first) && (Difference_z != closest_to_true_HS_z_second)) {
          closest_to_true_HS_z_third = Difference_z;
          closest_to_true_HS_z_third_index = i;
        }
    }

    // Uncomment this to use clostest to true HS in z postion as the best truth matched HS vertex
    hsMatch_best = vertexContainer[closest_to_true_HS_z_first_index];
    /*if(closest_to_true_HS_z_first_index != hsMatch_best_tmp->index()) {
      h_hsMatch_best_closest_z_diffference_x->Fill(vertexContainer[closest_to_true_HS_z_first_index]->x() - vertexContainer[hsMatch_best_tmp->index()]->x());
      h_hsMatch_best_closest_z_diffference_y->Fill(vertexContainer[closest_to_true_HS_z_first_index]->y() - vertexContainer[hsMatch_best_tmp->index()]->y());
      h_hsMatch_best_closest_z_diffference_z->Fill(vertexContainer[closest_to_true_HS_z_first_index]->z() - vertexContainer[hsMatch_best_tmp->index()]->z());
    }*/

    //int accepted_tracks_count = 0;
    /*double high_eta_truth_count = 0;
    for (const auto& elTrk : true_HS_vtx->outgoingParticleLinks()) {
      const xAOD::TruthParticle* trk = *elTrk;
      // Apply track selection cuts for True HS vertex
      //if(trk->pt() > 0.9 || trk->eta() < 4.0) {accepted_tracks_count++;}
      if(trk->eta() > 2.7) {
        high_eta_truth_count++;
        cout << "Truth track is forward" << endl;
      }
    }

    if(high_eta_truth_count / true_HS_vtx->nOutgoingParticles() > 0.5){cout << "Truth HS is forward" << endl;}

    if(accepted_tracks_count > 1){h_truth_HS_nVtx_accepted->Fill(1);}
    h_truth_HS_nVtx->Fill(1);*/

    // Save some best HS Match properties
    //double hsMatch_best_z = hsMatch_best->z();
    double hsMatch_lPUdz = exp(-0.5 * hsMatch_best->z() * hsMatch_best->z() / lPUd_factor / lPUd_factor) / lPUd_factor / sqrt(6.28) * lPUd_mu;
    h_nHS_best->Fill(1);
    for (size_t i = 0; i < (trkvxassoUniqueMap_loose[hsMatch_best]).size(); ++i) {
      const xAOD::TrackParticle* trk = trkvxassoUniqueMap_loose[hsMatch_best][i];
      // Get truth particle associated with a track particle
      typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
      const xAOD::TruthParticle * tresult = 0;
      // Is there any truth...?
      if (trk->isAvailable<ElementTruthLink_t>("truthParticleLink")) {
        // ...then get link
        const ElementTruthLink_t ptruthContainer= trk->auxdata<ElementTruthLink_t>("truthParticleLink");
        if (ptruthContainer.isValid()){
          //cout << "Truth Track Link Available" << endl;
          tresult= *ptruthContainer;
        }
      }
      
      const xAOD::TruthParticle *associatedTruthT = tresult;

      if(associatedTruthT) {
        h_HS_best_track_association_eta->Fill(trk->eta());
        h_HS_best_truth_association_eta->Fill(associatedTruthT->eta());
      }
    }

    InDetVertexTruthMatchUtils::HardScatterType HStype = InDetVertexTruthMatchUtils::classifyHardScatter( vertexContainer );
    h_HSCategory->Fill(HStype);
    h_HSCategory_Mu->Fill(m_muCorrected, HStype, m_muWeigth);

    int nTypes[InDetVertexTruthMatchUtils::NTYPES] = { 0 };
    int nTypes_HS[InDetVertexTruthMatchUtils::NTYPES] = { 0 };
    int nTypes_PU[InDetVertexTruthMatchUtils::NTYPES] = { 0 };

    int nReconstruct = 0;
    int nMatch = 0;
    int nMerge = 0;
    int nSplit = 0;
    int nFake = 0;

    for(size_t i = 0; i < vertexContainerSize; ++i) {
      if(xAOD::VxType::NoVtx == vertexContainer[i]->vertexType()) continue;

      VertexMatchType & type = matchTypeDecorator( *(vertexContainer[i]) );
      //std::vector<VertexTruthMatchInfo> & info = truthMatchInfoDecorator( *(truthvertexContainer[0]) );
      std::vector<VertexTruthMatchInfo> &truthMatchInfo = truthMatchInfoDecorator(*(vertexContainer[i]));

      if (truthMatchInfo[0].first != 0) {
        double vtx_sumpt_squared = 0;
        int nTracks = vertexContainer[i]->nTrackParticles();

        // Loop over tracks at Reconstructed Vertex
        for (const auto& elTrk : vertexContainer[i]->trackParticleLinks()) {
          const xAOD::TrackParticle* trk = *elTrk;
          if (trk == 0) continue;
          vtx_sumpt_squared = vtx_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));

          if(hsMatch_best->index() == i) {
            h_HS_best_track_eta->Fill(trk->eta());
            h_HS_best_track_pt->Fill(trk->pt() / 1000.);
          }
          if(hsMatch_best->index() != i) {
            h_PU_Reconstruct_track_eta->Fill(trk->eta());
            h_PU_Reconstruct_track_pt->Fill(trk->pt() / 1000.);
          }
        }

        double lPUdz = exp(-0.5 * vertexContainer[i]->z() * vertexContainer[i]->z() / lPUd_factor / lPUd_factor) / lPUd_factor / sqrt(6.28) * lPUd_mu;

        // Always count the types
        nTypes[type]++;

        auto truthMatchVtx = (*truthMatchInfo[0].first)->truthVertex(0);
        double sumpt_squared_truthMatch = 0;
        /*for (const auto& elTrk : truthMatchVtx->outgoingParticleLinks()) {
          const xAOD::TruthParticle* trk = *elTrk;
          sumpt_squared_truthMatch = sumpt_squared_truthMatch + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
        }*/

        h_bias_Vtx_x_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->x() - truthMatchVtx->x());
        h_bias_Vtx_y_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->y() - truthMatchVtx->y());
        h_bias_Vtx_z_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->z() - truthMatchVtx->z());
        h_bias_Vtx_r_sumpt_squared->Fill(vtx_sumpt_squared, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

        h_bias_Vtx_x_lPUdz->Fill(lPUdz, vertexContainer[i]->x() - truthMatchVtx->x());
        h_bias_Vtx_y_lPUdz->Fill(lPUdz, vertexContainer[i]->y() - truthMatchVtx->y());
        h_bias_Vtx_z_lPUdz->Fill(lPUdz, vertexContainer[i]->z() - truthMatchVtx->z());
        h_bias_Vtx_r_lPUdz->Fill(lPUdz, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

        h_bias_Vtx_x_ntracks->Fill(nTracks, vertexContainer[i]->x() - truthMatchVtx->x());
        h_bias_Vtx_y_ntracks->Fill(nTracks, vertexContainer[i]->y() - truthMatchVtx->y());
        h_bias_Vtx_z_ntracks->Fill(nTracks, vertexContainer[i]->z() - truthMatchVtx->z());
        h_bias_Vtx_r_ntracks->Fill(nTracks, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

        h_bias_Vtx_x_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->x() - truthMatchVtx->x());
        h_bias_Vtx_y_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->y() - truthMatchVtx->y());
        h_bias_Vtx_z_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->z() - truthMatchVtx->z());
        h_bias_Vtx_r_zPosition->Fill(vertexContainer[i]->z(), sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

        // Can make special plots for types of interest
        if(type == MATCHED || type == MERGED || type == SPLIT) {
          // Fill general vertex properties for Reconstructed vertices
          nReconstruct++;

          h_Reconstruct_sumpt_squared->Fill(vtx_sumpt_squared);
          h_Reconstruct_lPUdz->Fill(lPUdz);
          h_Reconstruct_ntracks->Fill(nTracks);
          h_Reconstruct_zPosition->Fill(vertexContainer[i]->z());

          h_bias_Reconstruct_x_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->x() - truthMatchVtx->x());
          h_bias_Reconstruct_y_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->y() - truthMatchVtx->y());
          h_bias_Reconstruct_z_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->z() - truthMatchVtx->z());

          h_bias_Reconstruct_x_lPUdz->Fill(lPUdz, vertexContainer[i]->x() - truthMatchVtx->x());
          h_bias_Reconstruct_y_lPUdz->Fill(lPUdz, vertexContainer[i]->y() - truthMatchVtx->y());
          h_bias_Reconstruct_z_lPUdz->Fill(lPUdz, vertexContainer[i]->z() - truthMatchVtx->z());

          h_bias_Reconstruct_x_ntracks->Fill(nTracks, vertexContainer[i]->x() - truthMatchVtx->x());
          h_bias_Reconstruct_y_ntracks->Fill(nTracks, vertexContainer[i]->y() - truthMatchVtx->y());
          h_bias_Reconstruct_z_ntracks->Fill(nTracks, vertexContainer[i]->z() - truthMatchVtx->z());

          h_bias_Reconstruct_x_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->x() - truthMatchVtx->x());
          h_bias_Reconstruct_y_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->y() - truthMatchVtx->y());
          h_bias_Reconstruct_z_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->z() - truthMatchVtx->z());
        }

        if(type == MATCHED) {
          // Fill general vertex properties for Matched vertices
          nMatch++;

          h_Match_sumpt_squared->Fill(vtx_sumpt_squared);
          h_Match_lPUdz->Fill(lPUdz);
          h_Match_ntracks->Fill(nTracks);
          h_Match_zPosition->Fill(vertexContainer[i]->z());
        }

        if(type == MERGED) {
          // Fill general vertex properties for Mergeed vertices
          nMerge++;

          h_Merge_sumpt_squared->Fill(vtx_sumpt_squared);
          h_Merge_lPUdz->Fill(lPUdz);
          h_Merge_ntracks->Fill(nTracks);
          h_Merge_zPosition->Fill(vertexContainer[i]->z());
        }

        if(type == SPLIT) {
          // Fill general vertex properties for Split vertices
          nSplit++;

          h_Split_sumpt_squared->Fill(vtx_sumpt_squared);
          h_Split_lPUdz->Fill(lPUdz);
          h_Split_ntracks->Fill(nTracks);
          h_Split_zPosition->Fill(vertexContainer[i]->z());
        }

        if(type == FAKE) {
          // Fill general vertex properties for Fake vertices
          nFake++;

          h_Fake_sumpt_squared->Fill(vtx_sumpt_squared);
          h_Fake_lPUdz->Fill(lPUdz);
          h_Fake_ntracks->Fill(nTracks);
          h_Fake_zPosition->Fill(vertexContainer[i]->z());
        }

        // If the vertex is closest to the true HS then fill clostest to HS plots
        if(hsMatch_best->index() == i){
          nTypes_HS[type]++;

          h_HS_best_sqrt_sumpt_squared->Fill(sqrt(vtx_sumpt_squared));
          h_HS_best_sumpt_squared->Fill(vtx_sumpt_squared);
          h_HS_best_lPUdz->Fill(lPUdz);
          h_HS_best_ntracks->Fill(nTracks);
          h_HS_best_zPosition->Fill(vertexContainer[i]->z());

          if(hsMatch_best->nTrackParticles() > Truth_matched_hs_track_cut) {
            if (vtx_sumpt_squared_highest_index == hsMatch_best->index()) {
              h_HS_best_sumpt_squared_highest->Fill(1);
              h_HS_best_sumpt_squared_highest_sumpt_squared->Fill(vtx_sumpt_squared);
              h_HS_best_sumpt_squared_highest_lPUdz->Fill(lPUdz);
              h_HS_best_sumpt_squared_highest_ntracks->Fill(nTracks);
              h_HS_best_sumpt_squared_highest_zPosition->Fill(vertexContainer[i]->z());
            }
          }
          if (vtx_sumpt_squared_highest_eta_bin_000_120_index == hsMatch_best->index()) {
            h_HS_best_sumpt_squared_highest_eta_bin_000_120->Fill(1);
          }
          if (vtx_sumpt_squared_highest_index != hsMatch_best->index()) {
            h_HS_best_not_sumpt_squared_highest_sumpt_squared->Fill(vtx_sumpt_squared);
            h_HS_best_not_sumpt_squared_highest_lPUdz->Fill(lPUdz);
            h_HS_best_not_sumpt_squared_highest_ntracks->Fill(nTracks);
            h_HS_best_not_sumpt_squared_highest_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == MATCHED || type == MERGED || type == SPLIT) {
            // Fill general vertex properties for Reconstructed vertices
            h_HS_best_Reconstruct_sumpt_squared->Fill(vtx_sumpt_squared);
            h_HS_best_Reconstruct_lPUdz->Fill(lPUdz);
            h_HS_best_Reconstruct_ntracks->Fill(nTracks);
            h_HS_best_Reconstruct_zPosition->Fill(vertexContainer[i]->z());

            h_bias_HS_best_Reconstruct_x_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_HS_best_Reconstruct_y_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_HS_best_Reconstruct_z_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_HS_best_Reconstruct_r_sumpt_squared->Fill(vtx_sumpt_squared, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_HS_best_Reconstruct_x_lPUdz->Fill(lPUdz, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_HS_best_Reconstruct_y_lPUdz->Fill(lPUdz, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_HS_best_Reconstruct_z_lPUdz->Fill(lPUdz, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_HS_best_Reconstruct_r_lPUdz->Fill(lPUdz, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_HS_best_Reconstruct_x_ntracks->Fill(nTracks, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_HS_best_Reconstruct_y_ntracks->Fill(nTracks, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_HS_best_Reconstruct_z_ntracks->Fill(nTracks, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_HS_best_Reconstruct_r_ntracks->Fill(nTracks, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_HS_best_Reconstruct_x_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_HS_best_Reconstruct_y_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_HS_best_Reconstruct_z_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_HS_best_Reconstruct_r_zPosition->Fill(vertexContainer[i]->z(), sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));
          }

          if(type == MATCHED) {
            // Fill general vertex properties for Matched vertices
            h_HS_best_Match_sumpt_squared->Fill(vtx_sumpt_squared);
            h_HS_best_Match_lPUdz->Fill(lPUdz);
            h_HS_best_Match_ntracks->Fill(nTracks);
            h_HS_best_Match_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == MERGED) {
            // Fill general vertex properties for Mergeed vertices
            h_HS_best_Merge_sumpt_squared->Fill(vtx_sumpt_squared);
            h_HS_best_Merge_lPUdz->Fill(lPUdz);
            h_HS_best_Merge_ntracks->Fill(nTracks);
            h_HS_best_Merge_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == SPLIT) {
            // Fill general vertex properties for Split vertices
            h_HS_best_Split_sumpt_squared->Fill(vtx_sumpt_squared);
            h_HS_best_Split_lPUdz->Fill(lPUdz);
            h_HS_best_Split_ntracks->Fill(nTracks);
            h_HS_best_Split_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == FAKE) {
            // Fill general vertex properties for Fake vertices
            h_HS_best_Fake_sumpt_squared->Fill(vtx_sumpt_squared);
            h_HS_best_Fake_lPUdz->Fill(lPUdz);
            h_HS_best_Fake_ntracks->Fill(nTracks);
            h_HS_best_Fake_zPosition->Fill(vertexContainer[i]->z());
          }
        }

        // If the vertex is not closest to the true HS then fill PU vertices plots
        if(hsMatch_best->index() != i){
          nTypes_PU[type]++;

          if(type == MATCHED || type == MERGED || type == SPLIT) {
            // Fill general vertex properties for Reconstructed vertices
            h_PU_Reconstruct_sqrt_sumpt_squared->Fill(sqrt(vtx_sumpt_squared));
            h_PU_Reconstruct_sumpt_squared->Fill(vtx_sumpt_squared);
            h_PU_Reconstruct_lPUdz->Fill(lPUdz);
            h_PU_Reconstruct_ntracks->Fill(nTracks);
            h_PU_Reconstruct_zPosition->Fill(vertexContainer[i]->z());

            h_bias_PU_Reconstruct_x_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_PU_Reconstruct_y_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_PU_Reconstruct_z_sumpt_squared->Fill(vtx_sumpt_squared, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_PU_Reconstruct_r_sumpt_squared->Fill(vtx_sumpt_squared, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_PU_Reconstruct_x_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_PU_Reconstruct_y_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_PU_Reconstruct_z_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_PU_Reconstruct_r_sumpt_squared_ntracks->Fill(vtx_sumpt_squared, nTracks, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_PU_Reconstruct_x_lPUdz->Fill(lPUdz, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_PU_Reconstruct_y_lPUdz->Fill(lPUdz, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_PU_Reconstruct_z_lPUdz->Fill(lPUdz, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_PU_Reconstruct_r_lPUdz->Fill(lPUdz, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_PU_Reconstruct_x_ntracks->Fill(nTracks, vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_PU_Reconstruct_y_ntracks->Fill(nTracks, vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_PU_Reconstruct_z_ntracks->Fill(nTracks, vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_PU_Reconstruct_r_ntracks->Fill(nTracks, sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));

            h_bias_PU_Reconstruct_x_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->x() - truthMatchVtx->x());
            h_bias_PU_Reconstruct_y_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->y() - truthMatchVtx->y());
            h_bias_PU_Reconstruct_z_zPosition->Fill(vertexContainer[i]->z(), vertexContainer[i]->z() - truthMatchVtx->z());
            h_bias_PU_Reconstruct_r_zPosition->Fill(vertexContainer[i]->z(), sqrt((vertexContainer[i]->x() * vertexContainer[i]->x()) + (vertexContainer[i]->y() * vertexContainer[i]->y()))
                                                                              - sqrt((truthMatchVtx->x() * truthMatchVtx->x()) + (truthMatchVtx->y() * truthMatchVtx->y())));
          }

          if(type == MATCHED) {
            // Fill general vertex properties for Matched vertices
            h_PU_Match_sumpt_squared->Fill(vtx_sumpt_squared);
            h_PU_Match_lPUdz->Fill(lPUdz);
            h_PU_Match_ntracks->Fill(nTracks);
            h_PU_Match_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == MERGED) {
            // Fill general vertex properties for Mergeed vertices
            h_PU_Merge_sumpt_squared->Fill(vtx_sumpt_squared);
            h_PU_Merge_lPUdz->Fill(lPUdz);
            h_PU_Merge_ntracks->Fill(nTracks);
            h_PU_Merge_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == SPLIT) {
            // Fill general vertex properties for Split vertices
            h_PU_Split_sumpt_squared->Fill(vtx_sumpt_squared);
            h_PU_Split_lPUdz->Fill(lPUdz);
            h_PU_Split_ntracks->Fill(nTracks);
            h_PU_Split_zPosition->Fill(vertexContainer[i]->z());
          }

          if(type == FAKE) {
            // Fill general vertex properties for Fake vertices
            h_PU_Fake_sumpt_squared->Fill(vtx_sumpt_squared);
            h_PU_Fake_lPUdz->Fill(lPUdz);
            h_PU_Fake_ntracks->Fill(nTracks);
            h_PU_Fake_zPosition->Fill(vertexContainer[i]->z());
          }
        }
      }
    }

    // Fill Class plots with number of types
    h_nClass->Fill(MATCHED, nTypes[MATCHED]);
    h_nClass->Fill(MERGED,  nTypes[MERGED]);
    h_nClass->Fill(SPLIT,   nTypes[SPLIT]);
    h_nClass->Fill(FAKE,    nTypes[FAKE]);

    h_HS_nClass->Fill(MATCHED, nTypes_HS[MATCHED]);
    h_HS_nClass->Fill(MERGED,  nTypes_HS[MERGED]);
    h_HS_nClass->Fill(SPLIT,   nTypes_HS[SPLIT]);
    h_HS_nClass->Fill(FAKE,    nTypes_HS[FAKE]);

    h_PU_nClass->Fill(MATCHED, nTypes_PU[MATCHED]);
    h_PU_nClass->Fill(MERGED,  nTypes_PU[MERGED]);
    h_PU_nClass->Fill(SPLIT,   nTypes_PU[SPLIT]);
    h_PU_nClass->Fill(FAKE,    nTypes_PU[FAKE]);

    h_nReconstruct->Fill(nReconstruct);
    h_nMatch->Fill(nMatch);
    h_nMerge->Fill(nMerge);
    h_nSplit->Fill(nSplit);
    h_nFake->Fill(nFake);

    //---
    // Plots that depend on Truth Vertices from TruthPileupEventContainer
    //---

    double lPUdz_HS_truth = exp(-0.5 * true_HS_vtx->z() * true_HS_vtx->z() / lPUd_factor / lPUd_factor) / lPUd_factor / sqrt(6.28) * lPUd_mu;

    // Fill general truth vertex properties
    h_truth_Vtx_sumpt_squared->Fill(sumpt_squared_HS_truth);
    h_truth_Vtx_lPUdz->Fill(lPUdz_HS_truth);
    h_truth_Vtx_ntracks->Fill(true_HS_vtx->nOutgoingParticles());
    h_truth_Vtx_zPosition->Fill(true_HS_vtx->z());

    // Fill general HS truth vertex properties
    h_truth_HS_Vtx_sumpt_squared ->Fill(sumpt_squared_HS_truth);
    h_truth_HS_Vtx_lPUdz->Fill(lPUdz_HS_truth);
    h_truth_HS_Vtx_ntracks->Fill(true_HS_vtx->nOutgoingParticles());
    h_truth_HS_Vtx_zPosition->Fill(true_HS_vtx->z());

    int accepted_tracks_count_HS = 0;
    int accepted_tracks_count_HS_lower_coverage = 0;
    int truth_nVtx_HS_accepted = 0;
    int truth_nVtx_HS_accepted_lower_coverage = 0;
    for (const auto& elTrk : true_HS_vtx->outgoingParticleLinks()) {
      const xAOD::TruthParticle* trk = *elTrk;
      // Apply track selection cuts for True HS vertex
      if(((trk->pt() / 1000.) > 0.9) && (sqrt(trk->eta() * trk->eta()) < 4.0)) {accepted_tracks_count_HS++;}
      if(((trk->pt() / 1000.) > 1.2) && (sqrt(trk->eta() * trk->eta()) < 4.0)) {accepted_tracks_count_HS_lower_coverage++;}
    }

    if(accepted_tracks_count_HS > 1){truth_nVtx_HS_accepted++;}
    if(accepted_tracks_count_HS_lower_coverage > 1){truth_nVtx_HS_accepted_lower_coverage++;}
    h_truth_HS_nVtx_accepted->Fill(truth_nVtx_HS_accepted);
    h_truth_HS_nVtx_accepted_lower_coverage->Fill(truth_nVtx_HS_accepted_lower_coverage);

    if(TruthPUevents_available == true) {
      if(truthPileupEventContainer.size() != 0) {
        // Get PU primary vertex from Truth Pileup Event Container
        int truthPUEvents = truthPileupEventContainer.size();
        int truth_nVtx_PU = 0;
        int truth_nVtx_PU_accepted = 0;
        int truth_nVtx_PU_accepted_lower_coverage = 0;
        int nTruthParticles_PU_TOTAL = 0;
        for (int i = 0; i < truthPUEvents; i++) {
          auto true_PU_Event = truthPileupEventContainer.at(i);
          int nTruthParticles_PU = true_PU_Event->nTruthParticles();
          nTruthParticles_PU_TOTAL = nTruthParticles_PU_TOTAL + nTruthParticles_PU;
        }
        int index_pu_map[nTruthParticles_PU_TOTAL] = {0};
        int counter_pu = 0;
        for (int i = 0; i < truthPUEvents; i++) {
          truth_nVtx++;
          truth_nVtx_PU++;
          double sumpt_squared_PU_truth = 0;
          auto true_PU_Event = truthPileupEventContainer.at(i);
          int nTruthParticles_PU = true_PU_Event->nTruthParticles();
          for (int i = 0; i < nTruthParticles_PU; i++){
            if(true_PU_Event->truthParticle(i)->barcode() < 200000 && true_PU_Event->truthParticle(i)->status() == 1 && true_PU_Event->truthParticle(i)->isCharged()){
              sumpt_squared_PU_truth = sumpt_squared_PU_truth + ((true_PU_Event->truthParticle(i)->pt() / 1000.) * (true_PU_Event->truthParticle(i)->pt() / 1000.));
              index_pu_map[counter_pu] = true_PU_Event->truthParticle(i)->index();
              counter_pu++;
            }
          }

          // Taking the Primary Vertex for each true PU event
          auto true_PU_vtx = true_PU_Event->truthVertex(0);
          if(true_PU_vtx->t() != 0) {
            cout << "true_PU_vtx->t() = " << true_PU_vtx->t() << endl;
          }
          int accepted_tracks_count = 0;
          int accepted_tracks_count_lower_coverage = 0;
          for (const auto& elTrk : true_PU_vtx->outgoingParticleLinks()) {
            const xAOD::TruthParticle* trk = *elTrk;
            // Apply track selection cuts for True PU vertex
            if(((trk->pt() / 1000.) > 0.9) && (sqrt(trk->eta() * trk->eta()) < 4.0)) {accepted_tracks_count++;}
            if(((trk->pt() / 1000.) > 0.9) && (sqrt(trk->eta() * trk->eta()) < 2.5)) {accepted_tracks_count_lower_coverage++;}
            h_truth_PU_track_pt->Fill(trk->pt() / 1000.);
          }
          if(accepted_tracks_count > 1){truth_nVtx_PU_accepted++;}
          if(accepted_tracks_count_lower_coverage > 1){truth_nVtx_PU_accepted_lower_coverage++;}
          
          double lPUdz_PU_truth = exp(-0.5 * true_PU_vtx->z() * true_PU_vtx->z() / lPUd_factor / lPUd_factor) / lPUd_factor / sqrt(6.28) * lPUd_mu;

          // Fill general truth vertex properties
          h_truth_Vtx_sumpt_squared->Fill(sumpt_squared_PU_truth);
          h_truth_Vtx_lPUdz->Fill(lPUdz_PU_truth);
          h_truth_Vtx_ntracks->Fill(true_PU_vtx->nOutgoingParticles());
          h_truth_Vtx_zPosition->Fill(true_PU_vtx->z());

          // Fill general PU truth vertex properties
          h_truth_PU_Vtx_sumpt_squared ->Fill(sumpt_squared_PU_truth);
          h_truth_PU_Vtx_lPUdz->Fill(lPUdz_PU_truth);
          h_truth_PU_Vtx_ntracks->Fill(true_PU_vtx->nOutgoingParticles());
          h_truth_PU_Vtx_zPosition->Fill(true_PU_vtx->z());
        }
        h_truth_PU_nVtx->Fill(truth_nVtx_PU);
        h_truth_PU_nVtx_accepted->Fill(truth_nVtx_PU_accepted);
        h_truth_PU_nVtx_accepted_lower_coverage->Fill(truth_nVtx_PU_accepted_lower_coverage);
        h_truth_nVtx->Fill(truth_nVtx);

        //---
        // Plots that depend on Truth Vertices from TruthEventContainer and TruthPileupEventContainer and the primary vertices in the vertexContainer that associate 
        //---

        double hsMatch_best_sumpt_squared = 0;
        for (const auto& elTrk : hsMatch_best->trackParticleLinks()) {
          const xAOD::TrackParticle* trk = *elTrk;
          hsMatch_best_sumpt_squared = hsMatch_best_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
        }

        int pass_requirements = 0;

        double true_hs_ntracks = 0;
        double true_hs_sumpt = 0;
        double true_hs_sumpt_squared = 0;

        double true_hs_sumpt_eta_bin_000_120 = 0;
        double true_hs_sumpt_squared_eta_bin_000_120 = 0;
        double true_hs_sumpt_eta_bin_120_270 = 0;
        double true_hs_sumpt_squared_eta_bin_120_270 = 0;
        double true_hs_sumpt_eta_bin_270_340 = 0;
        double true_hs_sumpt_squared_eta_bin_270_340 = 0;
        double true_hs_sumpt_eta_bin_340_400 = 0;
        double true_hs_sumpt_squared_eta_bin_340_400 = 0;

        double reconstructed_hs_ntracks= 0;
        double reconstructed_pu_ntracks = 0;
        double truth_associated_hs_ntracks = 0;
        double truth_associated_pu_ntracks = 0;

        double reconstructed_hs_sumpt = 0;
        double reconstructed_hs_sumpt_squared = 0;
        double reconstructed_pu_sumpt = 0;
        double reconstructed_pu_sumpt_squared = 0;
        double truth_associated_hs_track_sumpt = 0;
        double truth_associated_hs_track_sumpt_squared = 0;
        double truth_associated_pu_track_sumpt = 0;
        double truth_associated_pu_track_sumpt_squared = 0;

        double reconstructed_hs_sumpt_is_highest = 0;
        double reconstructed_hs_sumpt_squared_is_highest = 0;
        double truth_associated_hs_track_sumpt_is_highest = 0;
        double truth_associated_hs_track_sumpt_squared_is_highest = 0;
        double truth_associated_pu_track_sumpt_is_highest = 0;
        double truth_associated_pu_track_sumpt_squared_is_highest = 0;

        double reconstructed_hs_sumpt_eta_bin_000_120 = 0;
        double reconstructed_hs_sumpt_squared_eta_bin_000_120 = 0;
        double reconstructed_pu_sumpt_eta_bin_000_120 = 0;
        double reconstructed_pu_sumpt_squared_eta_bin_000_120 = 0;
        double truth_associated_hs_track_sumpt_eta_bin_000_120 = 0;
        double truth_associated_hs_track_sumpt_squared_eta_bin_000_120 = 0;
        double truth_associated_pu_track_sumpt_eta_bin_000_120 = 0;
        double truth_associated_pu_track_sumpt_squared_eta_bin_000_120 = 0;

        double reconstructed_hs_sumpt_eta_bin_120_270 = 0;
        double reconstructed_hs_sumpt_squared_eta_bin_120_270 = 0;
        double reconstructed_pu_sumpt_eta_bin_120_270 = 0;
        double reconstructed_pu_sumpt_squared_eta_bin_120_270 = 0;
        double truth_associated_hs_track_sumpt_eta_bin_120_270 = 0;
        double truth_associated_hs_track_sumpt_squared_eta_bin_120_270 = 0;
        double truth_associated_pu_track_sumpt_eta_bin_120_270 = 0;
        double truth_associated_pu_track_sumpt_squared_eta_bin_120_270 = 0;

        double reconstructed_hs_sumpt_eta_bin_270_340 = 0;
        double reconstructed_hs_sumpt_squared_eta_bin_270_340 = 0;
        double reconstructed_pu_sumpt_eta_bin_270_340 = 0;
        double reconstructed_pu_sumpt_squared_eta_bin_270_340 = 0;
        double truth_associated_hs_track_sumpt_eta_bin_270_340 = 0;
        double truth_associated_hs_track_sumpt_squared_eta_bin_270_340 = 0;
        double truth_associated_pu_track_sumpt_eta_bin_270_340 = 0;
        double truth_associated_pu_track_sumpt_squared_eta_bin_270_340 = 0;

        double reconstructed_hs_sumpt_eta_bin_340_400 = 0;
        double reconstructed_hs_sumpt_squared_eta_bin_340_400 = 0;
        double reconstructed_pu_sumpt_eta_bin_340_400 = 0;
        double reconstructed_pu_sumpt_squared_eta_bin_340_400 = 0;
        double truth_associated_hs_track_sumpt_eta_bin_340_400 = 0;
        double truth_associated_hs_track_sumpt_squared_eta_bin_340_400 = 0;
        double truth_associated_pu_track_sumpt_eta_bin_340_400 = 0;
        double truth_associated_pu_track_sumpt_squared_eta_bin_340_400 = 0;

        double reconstructed_hs_sumpt_high_weight_tracks = 0;
        double reconstructed_hs_sumpt_squared_high_weight_tracks = 0;
        double truth_associated_pu_track_sumpt_high_weight_tracks = 0;
        double truth_associated_pu_track_sumpt_squared_high_weight_tracks = 0;

        //ElementLink<DataVector<xAOD::TruthEventBase_v1>> truthPEvent = (*truthLink)->auxdata<ElementLink<xAOD::TruthEventBaseContainer>>("TruthEventLink"); // now we have the truth HS event for a given track
        //if (truthHSEvent == *truthPEvent) goodTruthAssoc = true; // check the two truth HS events (one from vertex, the other from track->particle) are the same or not, can be used for checking truth PU event as well (change truthHSEvent to truthPUEvent)

        for(size_t i = 0; i < vertexContainerSize; ++i) {
          VertexMatchType & type = matchTypeDecorator( *(vertexContainer[i]) );
          int trackWeighti = 0;
          for (const auto& elTrk : vertexContainer[i]->trackParticleLinks()) {
            const xAOD::TrackParticle* trk = *elTrk;
            // Get truth particle associated with a track particle
            typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
            const xAOD::TruthParticle * tresult = 0;
            // Is there any truth...?
            if (trk->isAvailable<ElementTruthLink_t>("truthParticleLink")) {
              // ...then get link
              const ElementTruthLink_t ptruthContainer= trk->auxdata<ElementTruthLink_t>("truthParticleLink");
              if (ptruthContainer.isValid()){
                //cout << "Truth Track Link Available" << endl;
                tresult= *ptruthContainer;
              }
            }

            const xAOD::TruthParticle *associatedTruthT = tresult;

            /*#ifdef ROOTCORE
            MCTruthClassifier myClassifier("myClassifier");
            std::pair<unsigned int, unsigned int> res;
            res = myClassifier.particleTruthClassifier(trk);
            if (myClassifier.getProbTrktoTruth() > 0.7){
              MCTruthPartClassifier::ParticleDef partDef;
              const xAOD::TruthParticle *thePart = myClassifier.getGenPart();
              if(thePart != NULL) {
                if(thePart->status() == 1) {
                  if(thePart->barcode() < 20000) {
                    const xAOD::TruthVertex *truthvertex = thePart->prodVtx();
                    if (truthvertex != 0) {
                      if(truthvertex->t() != 0) {
                          long double truth_time = truthvertex->t();
                          cout << "truth_time = " << truth_time << endl;
                          TF1 *g = new TF1("g","gaus",-100,100);
                          g->SetParameters(1,truth_time,30);
                          double r = g->GetRandom();
                          h_track_t->Fill(r);
                          h_track_t_with_cut->Fill(r);
                      }
                    }
                  }
                }
              }
            }
            #endif*/
    
            //if(!associatedTruthT){cout << "Truth Track Link Not Available" << endl;}

            //if(associatedTruthT && (type == MATCHED || type == MERGED)) {
            //if(type == MATCHED || type == MERGED) {
            //if(associatedTruthT) {
            //if(hsMatch_best->nTrackParticles() > Truth_matched_hs_track_cut) {
              pass_requirements++;
              if(vtx_sumpt_squared_highest_index == i) {
                reconstructed_hs_sumpt_is_highest = reconstructed_hs_sumpt_is_highest + (trk->pt() / 1000.);
                reconstructed_hs_sumpt_squared_is_highest = reconstructed_hs_sumpt_squared_is_highest + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
              }
              if(hsMatch_best->index() == i) {
                reconstructed_hs_ntracks++;
                reconstructed_hs_sumpt = reconstructed_hs_sumpt + (trk->pt() / 1000.);
                reconstructed_hs_sumpt_squared = reconstructed_hs_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                if(trk->eta() > -1.2 && trk->eta() < 1.2) {
                  reconstructed_hs_sumpt_eta_bin_000_120 = reconstructed_hs_sumpt_eta_bin_000_120 + (trk->pt() / 1000.);
                  reconstructed_hs_sumpt_squared_eta_bin_000_120 = reconstructed_hs_sumpt_squared_eta_bin_000_120 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if((trk->eta() > -2.7 && trk->eta() < -1.2) || (trk->eta() > 1.2 && trk->eta() < 2.7)) {
                  reconstructed_hs_sumpt_eta_bin_120_270 = reconstructed_hs_sumpt_eta_bin_120_270 + (trk->pt() / 1000.);
                  reconstructed_hs_sumpt_squared_eta_bin_120_270 = reconstructed_hs_sumpt_squared_eta_bin_120_270 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if((trk->eta() > -3.4 && trk->eta() < -2.7) || (trk->eta() > 2.7 && trk->eta() < 3.4)) {
                  reconstructed_hs_sumpt_eta_bin_270_340 = reconstructed_hs_sumpt_eta_bin_270_340 + (trk->pt() / 1000.);
                  reconstructed_hs_sumpt_squared_eta_bin_270_340 = reconstructed_hs_sumpt_squared_eta_bin_270_340 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if((trk->eta() > -4.0 && trk->eta() < -3.4) || (trk->eta() > 3.4 && trk->eta() < 4.0)) {
                  reconstructed_hs_sumpt_eta_bin_340_400 = reconstructed_hs_sumpt_eta_bin_340_400 + (trk->pt() / 1000.);
                  reconstructed_hs_sumpt_squared_eta_bin_340_400 = reconstructed_hs_sumpt_squared_eta_bin_340_400 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if(vertexContainer[i]->trackWeight(trackWeighti) > 0.5){
                  reconstructed_hs_sumpt_high_weight_tracks = reconstructed_hs_sumpt_high_weight_tracks + (trk->pt() / 1000.);
                  reconstructed_hs_sumpt_squared_high_weight_tracks = reconstructed_hs_sumpt_squared_high_weight_tracks + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
              }
              if(hsMatch_best->index() != i) {
                reconstructed_pu_ntracks++;
                reconstructed_pu_sumpt = reconstructed_pu_sumpt + (trk->pt() / 1000.);
                reconstructed_pu_sumpt_squared = reconstructed_pu_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                if(trk->eta() > -1.2 && trk->eta() < 1.2) {
                  reconstructed_pu_sumpt_eta_bin_000_120 = reconstructed_pu_sumpt_eta_bin_000_120 + (trk->pt() / 1000.);
                  reconstructed_pu_sumpt_squared_eta_bin_000_120 = reconstructed_pu_sumpt_squared_eta_bin_000_120 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if((trk->eta() > -2.7 && trk->eta() < -1.2) || (trk->eta() > 1.2 && trk->eta() < 2.7)) {
                  reconstructed_pu_sumpt_eta_bin_120_270 = reconstructed_pu_sumpt_eta_bin_120_270 + (trk->pt() / 1000.);
                  reconstructed_pu_sumpt_squared_eta_bin_120_270 = reconstructed_pu_sumpt_squared_eta_bin_120_270 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if((trk->eta() > -3.4 && trk->eta() < -2.7) || (trk->eta() > 2.7 && trk->eta() < 3.4)) {
                  reconstructed_pu_sumpt_eta_bin_270_340 = reconstructed_pu_sumpt_eta_bin_270_340 + (trk->pt() / 1000.);
                  reconstructed_pu_sumpt_squared_eta_bin_270_340 = reconstructed_pu_sumpt_squared_eta_bin_270_340 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
                if((trk->eta() > -4.0 && trk->eta() < -3.4) || (trk->eta() > 3.4 && trk->eta() < 4.0)) {
                  reconstructed_pu_sumpt_eta_bin_340_400 = reconstructed_pu_sumpt_eta_bin_340_400 + (trk->pt() / 1000.);
                  reconstructed_pu_sumpt_squared_eta_bin_340_400 = reconstructed_pu_sumpt_squared_eta_bin_340_400 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                }
              }
              if(associatedTruthT) {
                bool is_associated_hs = false;
                for (int counter_index = 0; counter_index < counter_hs; counter_index++) {
                  int associatedTruthT_index = associatedTruthT->index();
                  if(associatedTruthT_index == index_hs_map[counter_index]) {
                    true_hs_ntracks++;
                    true_hs_sumpt = true_hs_sumpt + (trk->pt() / 1000.);
                    true_hs_sumpt_squared = true_hs_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    if(trk->eta() > -1.2 && trk->eta() < 1.2) {
                      true_hs_sumpt_eta_bin_000_120 = true_hs_sumpt_eta_bin_000_120 + (trk->pt() / 1000.);
                      true_hs_sumpt_squared_eta_bin_000_120 = true_hs_sumpt_squared_eta_bin_000_120 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if((trk->eta() > -2.7 && trk->eta() < -1.2) || (trk->eta() > 1.2 && trk->eta() < 2.7)) {
                      true_hs_sumpt_eta_bin_120_270 = true_hs_sumpt_eta_bin_120_270 + (trk->pt() / 1000.);
                      true_hs_sumpt_squared_eta_bin_120_270 = true_hs_sumpt_squared_eta_bin_120_270 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if((trk->eta() > -3.4 && trk->eta() < -2.7) || (trk->eta() > 2.7 && trk->eta() < 3.4)) {
                      true_hs_sumpt_eta_bin_270_340 = true_hs_sumpt_eta_bin_270_340 + (trk->pt() / 1000.);
                      true_hs_sumpt_squared_eta_bin_270_340 = true_hs_sumpt_squared_eta_bin_270_340 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if((trk->eta() > -4.0 && trk->eta() < -3.4) || (trk->eta() > 3.4 && trk->eta() < 4.0)) {
                      true_hs_sumpt_eta_bin_340_400 = true_hs_sumpt_eta_bin_340_400 + (trk->pt() / 1000.);
                      true_hs_sumpt_squared_eta_bin_340_400 = true_hs_sumpt_squared_eta_bin_340_400 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if(vtx_sumpt_squared_highest_index == i){
                      truth_associated_hs_track_sumpt_is_highest = truth_associated_hs_track_sumpt_is_highest + (trk->pt() / 1000.);
                      truth_associated_hs_track_sumpt_squared_is_highest = truth_associated_hs_track_sumpt_squared_is_highest + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if(hsMatch_best->index() == i){
                      truth_associated_hs_ntracks++;
                      truth_associated_hs_track_sumpt = truth_associated_hs_track_sumpt + (trk->pt() / 1000.);
                      truth_associated_hs_track_sumpt_squared = truth_associated_hs_track_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                      if(trk->eta() > -1.2 && trk->eta() < 1.2) {
                        truth_associated_hs_track_sumpt_eta_bin_000_120 = truth_associated_hs_track_sumpt_eta_bin_000_120 + (trk->pt() / 1000.);
                        truth_associated_hs_track_sumpt_squared_eta_bin_000_120 = truth_associated_hs_track_sumpt_squared_eta_bin_000_120 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                      }
                      if((trk->eta() > -2.7 && trk->eta() < -1.2) || (trk->eta() > 1.2 && trk->eta() < 2.7)) {
                        truth_associated_hs_track_sumpt_eta_bin_120_270 = truth_associated_hs_track_sumpt_eta_bin_120_270 + (trk->pt() / 1000.);
                        truth_associated_hs_track_sumpt_squared_eta_bin_120_270 = truth_associated_hs_track_sumpt_squared_eta_bin_120_270 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                      }
                      if((trk->eta() > -3.4 && trk->eta() < -2.7) || (trk->eta() > 2.7 && trk->eta() < 3.4)) {
                        truth_associated_hs_track_sumpt_eta_bin_270_340 = truth_associated_hs_track_sumpt_eta_bin_270_340 + (trk->pt() / 1000.);
                        truth_associated_hs_track_sumpt_squared_eta_bin_270_340 = truth_associated_hs_track_sumpt_squared_eta_bin_270_340 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                      }
                      if((trk->eta() > -4.0 && trk->eta() < -3.4) || (trk->eta() > 3.4 && trk->eta() < 4.0)) {
                        truth_associated_hs_track_sumpt_eta_bin_340_400 = truth_associated_hs_track_sumpt_eta_bin_340_400 + (trk->pt() / 1000.);
                        truth_associated_hs_track_sumpt_squared_eta_bin_340_400 = truth_associated_hs_track_sumpt_squared_eta_bin_340_400 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                      }
                    }
                    is_associated_hs = true;
                  }
                }
                //for (int counter_index = 0; counter_index < counter_pu; counter_index++) {
                int associatedTruthT_index = associatedTruthT->index();
                if(is_associated_hs == false){
                //if(associatedTruthT_index == index_pu_map[counter_index]) {
                  if(vtx_sumpt_squared_highest_index == i) {
                    truth_associated_pu_track_sumpt_is_highest = truth_associated_pu_track_sumpt_is_highest + (trk->pt() / 1000.);
                    truth_associated_pu_track_sumpt_squared_is_highest = truth_associated_pu_track_sumpt_squared_is_highest + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                  }
                  if(hsMatch_best->index() == i){
                    truth_associated_pu_ntracks++;
                    truth_associated_pu_track_sumpt = truth_associated_pu_track_sumpt + (trk->pt() / 1000.);
                    truth_associated_pu_track_sumpt_squared = truth_associated_pu_track_sumpt_squared + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    if(trk->eta() > -1.2 && trk->eta() < 1.2) {
                      truth_associated_pu_track_sumpt_eta_bin_000_120 = truth_associated_pu_track_sumpt_eta_bin_000_120 + (trk->pt() / 1000.);
                      truth_associated_pu_track_sumpt_squared_eta_bin_000_120 = truth_associated_pu_track_sumpt_squared_eta_bin_000_120 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if((trk->eta() > -2.7 && trk->eta() < -1.2) || (trk->eta() > 1.2 && trk->eta() < 2.7)) {
                      truth_associated_pu_track_sumpt_eta_bin_120_270 = truth_associated_pu_track_sumpt_eta_bin_120_270 + (trk->pt() / 1000.);
                      truth_associated_pu_track_sumpt_squared_eta_bin_120_270 = truth_associated_pu_track_sumpt_squared_eta_bin_120_270 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if((trk->eta() > -3.4 && trk->eta() < -2.7) || (trk->eta() > 2.7 && trk->eta() < 3.4)) {
                      truth_associated_pu_track_sumpt_eta_bin_270_340 = truth_associated_pu_track_sumpt_eta_bin_270_340 + (trk->pt() / 1000.);
                      truth_associated_pu_track_sumpt_squared_eta_bin_270_340 = truth_associated_pu_track_sumpt_squared_eta_bin_270_340 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if((trk->eta() > -4.0 && trk->eta() < -3.4) || (trk->eta() > 3.4 && trk->eta() < 4.0)) {
                      truth_associated_pu_track_sumpt_eta_bin_340_400 = truth_associated_pu_track_sumpt_eta_bin_340_400 + (trk->pt() / 1000.);
                      truth_associated_pu_track_sumpt_squared_eta_bin_340_400 = truth_associated_pu_track_sumpt_squared_eta_bin_340_400 + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                    if(vertexContainer[i]->trackWeight(trackWeighti) > 0.5){
                      truth_associated_pu_track_sumpt_high_weight_tracks = truth_associated_pu_track_sumpt_high_weight_tracks + (trk->pt() / 1000.);
                      truth_associated_pu_track_sumpt_squared_high_weight_tracks = truth_associated_pu_track_sumpt_squared_high_weight_tracks + ((trk->pt() / 1000.) * (trk->pt() / 1000.));
                    }
                  }
                }
              }
            //}
            trackWeighti++;
          }
        }

        if(pass_requirements != 0) {

          h_eff_HS_truth_ntracks_association->Fill(truth_associated_hs_ntracks / true_hs_ntracks);
          h_truth_associated_hs_ntracks_lPUdz->Fill(hsMatch_lPUdz, truth_associated_hs_ntracks);
          h_true_hs_ntracks_lPUdz->Fill(hsMatch_lPUdz, true_hs_ntracks);
          h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_ntracks / true_hs_ntracks);
          h_eff_HS_truth_ntracks_association_sumpt_squared->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_ntracks / true_hs_ntracks);
          h_eff_HS_truth_ntracks_association_lPUdz->Fill(hsMatch_lPUdz, truth_associated_hs_ntracks / true_hs_ntracks);
          h_eff_HS_truth_ntracks_association_ntracks->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_ntracks / true_hs_ntracks);
          h_eff_HS_truth_ntracks_association_zPosition->Fill(hsMatch_best->z(), truth_associated_hs_ntracks / true_hs_ntracks);

          h_eff_HS_truth_association->Fill(truth_associated_hs_track_sumpt / true_hs_sumpt);
          h_eff_HS_truth_association_sqrt_sumpt_squared->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_track_sumpt / true_hs_sumpt);
          h_eff_HS_truth_association_sumpt_squared->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_track_sumpt / true_hs_sumpt);
          h_eff_HS_truth_association_lPUdz->Fill(hsMatch_lPUdz, truth_associated_hs_track_sumpt / true_hs_sumpt);
          h_eff_HS_truth_association_ntracks->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_track_sumpt / true_hs_sumpt);
          h_eff_HS_truth_association_zPosition->Fill(hsMatch_best->z(), truth_associated_hs_track_sumpt / true_hs_sumpt);

          h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_track_sumpt_eta_bin_000_120 / true_hs_sumpt_eta_bin_000_120);
          h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_track_sumpt_eta_bin_000_120 / true_hs_sumpt_eta_bin_000_120);
          h_eff_HS_truth_association_lPUdz_eta_bin_000_120->Fill(hsMatch_lPUdz, truth_associated_hs_track_sumpt_eta_bin_000_120 / true_hs_sumpt_eta_bin_000_120);
          h_eff_HS_truth_association_ntracks_eta_bin_000_120->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_track_sumpt_eta_bin_000_120 / true_hs_sumpt_eta_bin_000_120);
          h_eff_HS_truth_association_zPosition_eta_bin_000_120->Fill(hsMatch_best->z(), truth_associated_hs_track_sumpt_eta_bin_000_120 / true_hs_sumpt_eta_bin_000_120);

          h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_track_sumpt_eta_bin_120_270 / true_hs_sumpt_eta_bin_120_270);
          h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_track_sumpt_eta_bin_120_270 / true_hs_sumpt_eta_bin_120_270);
          h_eff_HS_truth_association_lPUdz_eta_bin_120_270->Fill(hsMatch_lPUdz, truth_associated_hs_track_sumpt_eta_bin_120_270 / true_hs_sumpt_eta_bin_120_270);
          h_eff_HS_truth_association_ntracks_eta_bin_120_270->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_track_sumpt_eta_bin_120_270 / true_hs_sumpt_eta_bin_120_270);
          h_eff_HS_truth_association_zPosition_eta_bin_120_270->Fill(hsMatch_best->z(), truth_associated_hs_track_sumpt_eta_bin_120_270 / true_hs_sumpt_eta_bin_120_270);

          h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_track_sumpt_eta_bin_270_340 / true_hs_sumpt_eta_bin_270_340);
          h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_track_sumpt_eta_bin_270_340 / true_hs_sumpt_eta_bin_270_340);
          h_eff_HS_truth_association_lPUdz_eta_bin_270_340->Fill(hsMatch_lPUdz, truth_associated_hs_track_sumpt_eta_bin_270_340 / true_hs_sumpt_eta_bin_270_340);
          h_eff_HS_truth_association_ntracks_eta_bin_270_340->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_track_sumpt_eta_bin_270_340 / true_hs_sumpt_eta_bin_270_340);
          h_eff_HS_truth_association_zPosition_eta_bin_270_340->Fill(hsMatch_best->z(), truth_associated_hs_track_sumpt_eta_bin_270_340 / true_hs_sumpt_eta_bin_270_340);

          h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_track_sumpt_eta_bin_340_400 / true_hs_sumpt_eta_bin_340_400);
          h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_track_sumpt_eta_bin_340_400 / true_hs_sumpt_eta_bin_340_400);
          h_eff_HS_truth_association_lPUdz_eta_bin_340_400->Fill(hsMatch_lPUdz, truth_associated_hs_track_sumpt_eta_bin_340_400 / true_hs_sumpt_eta_bin_340_400);
          h_eff_HS_truth_association_ntracks_eta_bin_340_400->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_track_sumpt_eta_bin_340_400 / true_hs_sumpt_eta_bin_340_400);
          h_eff_HS_truth_association_zPosition_eta_bin_340_400->Fill(hsMatch_best->z(), truth_associated_hs_track_sumpt_eta_bin_340_400 / true_hs_sumpt_eta_bin_340_400);

          h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_hs_track_sumpt_is_highest / true_hs_sumpt);
          h_eff_HS_truth_association_sumpt_squared_is_highest->Fill(reconstructed_hs_sumpt_squared, truth_associated_hs_track_sumpt_is_highest / true_hs_sumpt);
          h_eff_HS_truth_association_lPUdz_is_highest->Fill(hsMatch_lPUdz, truth_associated_hs_track_sumpt_is_highest / true_hs_sumpt);
          h_eff_HS_truth_association_ntracks_is_highest->Fill(hsMatch_best->nTrackParticles(), truth_associated_hs_track_sumpt_is_highest / true_hs_sumpt);
          h_eff_HS_truth_association_zPosition_is_highest->Fill(hsMatch_best->z(), truth_associated_hs_track_sumpt_is_highest / true_hs_sumpt);

          h_impurity_HS_truth_ntrack_association->Fill(truth_associated_pu_ntracks / reconstructed_hs_ntracks);
          h_truth_associated_pu_ntracks_lPUdz->Fill(hsMatch_lPUdz, truth_associated_pu_ntracks);
          h_reconstructed_hs_ntracks_lPUdz->Fill(hsMatch_lPUdz, reconstructed_hs_ntracks);
          h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_ntracks / reconstructed_hs_ntracks);
          h_impurity_HS_truth_ntrack_association_sumpt_squared->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_ntracks / reconstructed_hs_ntracks);
          h_impurity_HS_truth_ntrack_association_lPUdz->Fill(hsMatch_lPUdz, truth_associated_pu_ntracks / reconstructed_hs_ntracks);
          h_impurity_HS_truth_ntrack_association_ntracks->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_ntracks / reconstructed_hs_ntracks);
          h_impurity_HS_truth_ntrack_association_zPosition->Fill(hsMatch_best->z(), truth_associated_pu_ntracks / reconstructed_hs_ntracks);

          h_impurity_HS_truth_association->Fill(truth_associated_pu_track_sumpt / reconstructed_hs_sumpt);
          h_impurity_HS_truth_association_sqrt_sumpt_squared->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt / reconstructed_hs_sumpt);
          h_impurity_HS_truth_association_sumpt_squared->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt / reconstructed_hs_sumpt);
          h_impurity_HS_truth_association_lPUdz->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt / reconstructed_hs_sumpt);
          h_impurity_HS_truth_association_ntracks->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt / reconstructed_hs_sumpt);
          h_impurity_HS_truth_association_zPosition->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt / reconstructed_hs_sumpt);

          h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt_eta_bin_000_120 / reconstructed_hs_sumpt_eta_bin_000_120);
          h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt_eta_bin_000_120 / reconstructed_hs_sumpt_eta_bin_000_120);
          h_impurity_HS_truth_association_lPUdz_eta_bin_000_120->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt_eta_bin_000_120 / reconstructed_hs_sumpt_eta_bin_000_120);
          h_impurity_HS_truth_association_ntracks_eta_bin_000_120->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt_eta_bin_000_120 / reconstructed_hs_sumpt_eta_bin_000_120);
          h_impurity_HS_truth_association_zPosition_eta_bin_000_120->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt_eta_bin_000_120 / reconstructed_hs_sumpt_eta_bin_000_120);

          h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt_eta_bin_120_270 / reconstructed_hs_sumpt_eta_bin_120_270);
          h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt_eta_bin_120_270 / reconstructed_hs_sumpt_eta_bin_120_270);
          h_impurity_HS_truth_association_lPUdz_eta_bin_120_270->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt_eta_bin_120_270 / reconstructed_hs_sumpt_eta_bin_120_270);
          h_impurity_HS_truth_association_ntracks_eta_bin_120_270->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt_eta_bin_120_270 / reconstructed_hs_sumpt_eta_bin_120_270);
          h_impurity_HS_truth_association_zPosition_eta_bin_120_270->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt_eta_bin_120_270 / reconstructed_hs_sumpt_eta_bin_120_270);

          h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt_eta_bin_270_340 / reconstructed_hs_sumpt_eta_bin_270_340);
          h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt_eta_bin_270_340 / reconstructed_hs_sumpt_eta_bin_270_340);
          h_impurity_HS_truth_association_lPUdz_eta_bin_270_340->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt_eta_bin_270_340 / reconstructed_hs_sumpt_eta_bin_270_340);
          h_impurity_HS_truth_association_ntracks_eta_bin_270_340->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt_eta_bin_270_340 / reconstructed_hs_sumpt_eta_bin_270_340);
          h_impurity_HS_truth_association_zPosition_eta_bin_270_340->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt_eta_bin_270_340 / reconstructed_hs_sumpt_eta_bin_270_340);

          h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt_eta_bin_340_400 / reconstructed_hs_sumpt_eta_bin_340_400);
          h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt_eta_bin_340_400 / reconstructed_hs_sumpt_eta_bin_340_400);
          h_impurity_HS_truth_association_lPUdz_eta_bin_340_400->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt_eta_bin_340_400 / reconstructed_hs_sumpt_eta_bin_340_400);
          h_impurity_HS_truth_association_ntracks_eta_bin_340_400->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt_eta_bin_340_400 / reconstructed_hs_sumpt_eta_bin_340_400);
          h_impurity_HS_truth_association_zPosition_eta_bin_340_400->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt_eta_bin_340_400 / reconstructed_hs_sumpt_eta_bin_340_400);

          h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt_high_weight_tracks / reconstructed_hs_sumpt_high_weight_tracks);
          h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt_high_weight_tracks / reconstructed_hs_sumpt_high_weight_tracks);
          h_impurity_HS_truth_association_lPUdz_high_weight_tracks->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt_high_weight_tracks / reconstructed_hs_sumpt_high_weight_tracks);
          h_impurity_HS_truth_association_ntracks_high_weight_tracks->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt_high_weight_tracks / reconstructed_hs_sumpt_high_weight_tracks);
          h_impurity_HS_truth_association_zPosition_high_weight_tracks->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt_high_weight_tracks / reconstructed_hs_sumpt_high_weight_tracks);

          h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest->Fill(sqrt(reconstructed_hs_sumpt_squared), truth_associated_pu_track_sumpt_is_highest / reconstructed_hs_sumpt_is_highest);
          h_impurity_HS_truth_association_sumpt_squared_is_highest->Fill(reconstructed_hs_sumpt_squared, truth_associated_pu_track_sumpt_is_highest / reconstructed_hs_sumpt_is_highest);
          h_impurity_HS_truth_association_lPUdz_is_highest->Fill(hsMatch_lPUdz, truth_associated_pu_track_sumpt_is_highest / reconstructed_hs_sumpt_is_highest);
          h_impurity_HS_truth_association_ntracks_is_highest->Fill(hsMatch_best->nTrackParticles(), truth_associated_pu_track_sumpt_is_highest / reconstructed_hs_sumpt_is_highest);
          h_impurity_HS_truth_association_zPosition_is_highest->Fill(hsMatch_best->z(), truth_associated_pu_track_sumpt_is_highest / reconstructed_hs_sumpt_is_highest);
        }
      }
    }
  }

  return 0;
}

int VertexAnalysis::CalcEff(TH1D *Reco, TH1D *Truth, TH1D *Eff) {
  Reco->Sumw2();
  Truth->Sumw2();
  Eff->Sumw2();
  Eff->Divide(Reco, Truth, 1.0, 1.0, "B");
  return 0;
}

int VertexAnalysis::CalcResolution(TH2D *Bias, TH1D *Resolution) {
  for (int i = 0; i < Bias->GetNbinsX() + 1; i++){
    TString Name_Projection = "Projection";
    TH1D *histo1_temp = Bias->ProjectionY(Name_Projection,i,i);
    double RMS = histo1_temp->GetRMS();
    double itr_RMS = -1.0;
    double Mean = histo1_temp->GetMean();
    double RMS_error = histo1_temp->GetRMSError();
    int safety_counter = 0;
    while (sqrt((itr_RMS - RMS) * (itr_RMS - RMS)) > 0.001) {
      RMS = histo1_temp->GetRMS();
      histo1_temp->SetAxisRange(Mean - 3*RMS, Mean + 3*RMS,"X");
      itr_RMS = histo1_temp->GetRMS();
      RMS_error = histo1_temp->GetRMSError();
      if(safety_counter == 5){
        cout << "Too meny intertions reached in CalcResolution for Resolution plot " << Resolution->GetName() << endl;
        cout << "RMS = " << RMS << endl;
        cout << "Intertion RMS = " << itr_RMS << endl;
        break;
      }
      safety_counter++;
    }
    Resolution->SetBinContent(i,itr_RMS);
    Resolution->SetBinError(i,RMS_error);
  }
  return 0;
}

int VertexAnalysis::CalcResolution_twoD(TH3D *Bias, TH2D *Resolution) {
  for (int i = 0; i < Bias->GetNbinsX() + 1; i++){
    for (int j = 0; j < Bias->GetNbinsY() + 1; j++){
      TString Name_Projection = "Projection";
      TH1D *histo1_temp = Bias->ProjectionZ(Name_Projection,i,i,j,j);
      double RMS = histo1_temp->GetRMS();
      double itr_RMS = -1.0;
      double Mean = histo1_temp->GetMean();
      double RMS_error = histo1_temp->GetRMSError();
      int safety_counter = 0;
      while (sqrt((itr_RMS - RMS) * (itr_RMS - RMS)) > 0.001) {
        RMS = histo1_temp->GetRMS();
        histo1_temp->SetAxisRange(Mean - 3*RMS, Mean + 3*RMS,"X");
        itr_RMS = histo1_temp->GetRMS();
        RMS_error = histo1_temp->GetRMSError();
        if(safety_counter == 5){
          cout << "Too meny intertions reached in CalcResolution for Resolution plot " << Resolution->GetName() << endl;
          cout << "RMS = " << RMS << endl;
          cout << "Intertion RMS = " << itr_RMS << endl;
          break;
        }
        safety_counter++;
      }
      Resolution->SetBinContent(i,j,itr_RMS);
      Resolution->SetBinError(i,j,RMS_error);
    }
  }
  return 0;
}

int VertexAnalysis::procHistograms() {

  h_events->SetBinContent(1,m_entryNumber);

  //---
  // Plots that depend on the calculation of efficiency
  //---

  // Efficiency (no matching requirements)
  CalcEff(h_Vtx_sumpt_squared, h_truth_Vtx_sumpt_squared, h_eff_Vtx_sumpt_squared);
  CalcEff(h_Vtx_lPUdz, h_truth_Vtx_lPUdz, h_eff_Vtx_lPUdz);
  CalcEff(h_Vtx_ntracks, h_truth_Vtx_ntracks, h_eff_Vtx_ntracks);
  CalcEff(h_Vtx_zPosition, h_truth_Vtx_zPosition, h_eff_Vtx_zPosition);

  // Efficiency (Required to be reconstructed to truth vertex)
  CalcEff(h_Reconstruct_sumpt_squared, h_truth_Vtx_sumpt_squared, h_eff_Reconstruct_sumpt_squared);
  CalcEff(h_Reconstruct_lPUdz, h_truth_Vtx_lPUdz, h_eff_Reconstruct_lPUdz);
  CalcEff(h_Reconstruct_ntracks, h_truth_Vtx_ntracks, h_eff_Reconstruct_ntracks);
  CalcEff(h_Reconstruct_zPosition, h_truth_Vtx_zPosition, h_eff_Reconstruct_zPosition);

  // Efficiency (Required to be reconstructed to truth HS)
  CalcEff(h_HS_best_Reconstruct_sumpt_squared, h_truth_HS_Vtx_sumpt_squared, h_eff_HS_best_Reconstruct_sumpt_squared);
  CalcEff(h_HS_best_Reconstruct_lPUdz, h_truth_HS_Vtx_lPUdz, h_eff_HS_best_Reconstruct_lPUdz);
  CalcEff(h_HS_best_Reconstruct_ntracks, h_truth_HS_Vtx_ntracks, h_eff_HS_best_Reconstruct_ntracks);
  CalcEff(h_HS_best_Reconstruct_zPosition, h_truth_HS_Vtx_zPosition, h_eff_HS_best_Reconstruct_zPosition);

  CalcEff(h_PU_Reconstruct_sumpt_squared, h_truth_PU_Vtx_sumpt_squared, h_eff_PU_Reconstruct_sumpt_squared);
  CalcEff(h_PU_Reconstruct_lPUdz, h_truth_PU_Vtx_lPUdz, h_eff_PU_Reconstruct_lPUdz);
  CalcEff(h_PU_Reconstruct_ntracks, h_truth_PU_Vtx_ntracks, h_eff_PU_Reconstruct_ntracks);
  CalcEff(h_PU_Reconstruct_zPosition, h_truth_PU_Vtx_zPosition, h_eff_PU_Reconstruct_zPosition);

  CalcEff(h_HS_best_sumpt_squared_highest, h_nHS_best, h_eff_HS_best_is_sumpt_squared_highest);
  CalcEff(h_HS_best_sumpt_squared_highest_eta_bin_000_120, h_nHS_best, h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120);
  CalcEff(h_HS_best_sumpt_squared_highest_sumpt_squared, h_HS_best_sumpt_squared, h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared);
  CalcEff(h_HS_best_sumpt_squared_highest_lPUdz, h_HS_best_lPUdz, h_eff_HS_best_is_sumpt_squared_highest_lPUdz);
  CalcEff(h_HS_best_sumpt_squared_highest_ntracks, h_HS_best_ntracks, h_eff_HS_best_is_sumpt_squared_highest_ntracks);
  CalcEff(h_HS_best_sumpt_squared_highest_zPosition, h_HS_best_zPosition, h_eff_HS_best_is_sumpt_squared_highest_zPosition);

  CalcEff(h_HS_best_not_sumpt_squared_highest_sumpt_squared, h_HS_best_sumpt_squared, h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared);
  CalcEff(h_HS_best_not_sumpt_squared_highest_lPUdz, h_HS_best_lPUdz, h_impurity_HS_best_is_sumpt_squared_highest_lPUdz);
  CalcEff(h_HS_best_not_sumpt_squared_highest_ntracks, h_HS_best_ntracks, h_impurity_HS_best_is_sumpt_squared_highest_ntracks);
  CalcEff(h_HS_best_not_sumpt_squared_highest_zPosition, h_HS_best_zPosition, h_impurity_HS_best_is_sumpt_squared_highest_zPosition);

  CalcEff(h_HS_best_track_association_eta, h_HS_best_truth_association_eta, h_eff_HS_track_association_eta);
  //CalcEff(h_impurity_HS_track_association_eta);

  //CalcEff(h_associated_pu_track_eta, h_reconstructed_hs_eta, h_impurity_HS_truth_association_eta);
  //CalcEff(h_associated_hs_track_eta, h_reconstructed_pu_eta, h_impurity_PU_association_eta);

  CalcEff(h_track_pt, h_all_track_pt, h_eff_track_pt);
  CalcEff(h_track_eta, h_all_track_eta, h_eff_track_eta);

  // Resolutions (no matching requirement)
  CalcResolution(h_bias_Vtx_x_sumpt_squared, h_res_Vtx_x_sumpt_squared);
  CalcResolution(h_bias_Vtx_y_sumpt_squared, h_res_Vtx_y_sumpt_squared);
  CalcResolution(h_bias_Vtx_r_sumpt_squared, h_res_Vtx_r_sumpt_squared);
  CalcResolution(h_bias_Vtx_z_sumpt_squared, h_res_Vtx_z_sumpt_squared);

  CalcResolution(h_bias_Vtx_x_lPUdz, h_res_Vtx_x_lPUdz);
  CalcResolution(h_bias_Vtx_y_lPUdz, h_res_Vtx_y_lPUdz);
  CalcResolution(h_bias_Vtx_r_lPUdz, h_res_Vtx_r_lPUdz);
  CalcResolution(h_bias_Vtx_z_lPUdz, h_res_Vtx_z_lPUdz);

  CalcResolution(h_bias_Vtx_x_ntracks, h_res_Vtx_x_ntracks);
  CalcResolution(h_bias_Vtx_y_ntracks, h_res_Vtx_y_ntracks);
  CalcResolution(h_bias_Vtx_r_ntracks, h_res_Vtx_r_ntracks);
  CalcResolution(h_bias_Vtx_z_ntracks, h_res_Vtx_z_ntracks);

  CalcResolution(h_bias_Vtx_x_zPosition, h_res_Vtx_x_zPosition);
  CalcResolution(h_bias_Vtx_y_zPosition, h_res_Vtx_y_zPosition);
  CalcResolution(h_bias_Vtx_r_zPosition, h_res_Vtx_r_zPosition);
  CalcResolution(h_bias_Vtx_z_zPosition, h_res_Vtx_z_zPosition);

  // Resolutions (Reconstructed to HS only)
  CalcResolution(h_bias_HS_best_Reconstruct_x_sumpt_squared, h_res_HS_best_Reconstruct_x_sumpt_squared);
  CalcResolution(h_bias_HS_best_Reconstruct_y_sumpt_squared, h_res_HS_best_Reconstruct_y_sumpt_squared);
  CalcResolution(h_bias_HS_best_Reconstruct_r_sumpt_squared, h_res_HS_best_Reconstruct_r_sumpt_squared);
  CalcResolution(h_bias_HS_best_Reconstruct_z_sumpt_squared, h_res_HS_best_Reconstruct_z_sumpt_squared);

  CalcResolution_twoD(h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks, h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks);
  CalcResolution_twoD(h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks, h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks);
  CalcResolution_twoD(h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks, h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks);
  CalcResolution_twoD(h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks, h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks);

  CalcResolution(h_bias_HS_best_Reconstruct_x_lPUdz, h_res_HS_best_Reconstruct_x_lPUdz);
  CalcResolution(h_bias_HS_best_Reconstruct_y_lPUdz, h_res_HS_best_Reconstruct_y_lPUdz);
  CalcResolution(h_bias_HS_best_Reconstruct_r_lPUdz, h_res_HS_best_Reconstruct_r_lPUdz);
  CalcResolution(h_bias_HS_best_Reconstruct_z_lPUdz, h_res_HS_best_Reconstruct_z_lPUdz);

  CalcResolution(h_bias_HS_best_Reconstruct_x_ntracks, h_res_HS_best_Reconstruct_x_ntracks);
  CalcResolution(h_bias_HS_best_Reconstruct_y_ntracks, h_res_HS_best_Reconstruct_y_ntracks);
  CalcResolution(h_bias_HS_best_Reconstruct_r_ntracks, h_res_HS_best_Reconstruct_r_ntracks);
  CalcResolution(h_bias_HS_best_Reconstruct_z_ntracks, h_res_HS_best_Reconstruct_z_ntracks);

  CalcResolution(h_bias_HS_best_Reconstruct_x_zPosition, h_res_HS_best_Reconstruct_x_zPosition);
  CalcResolution(h_bias_HS_best_Reconstruct_y_zPosition, h_res_HS_best_Reconstruct_y_zPosition);
  CalcResolution(h_bias_HS_best_Reconstruct_r_zPosition, h_res_HS_best_Reconstruct_r_zPosition);
  CalcResolution(h_bias_HS_best_Reconstruct_z_zPosition, h_res_HS_best_Reconstruct_z_zPosition);

  // Resolutions (Reconstructed to PU only)
  CalcResolution(h_bias_PU_Reconstruct_x_sumpt_squared, h_res_PU_Reconstruct_x_sumpt_squared);
  CalcResolution(h_bias_PU_Reconstruct_y_sumpt_squared, h_res_PU_Reconstruct_y_sumpt_squared);
  CalcResolution(h_bias_PU_Reconstruct_r_sumpt_squared, h_res_PU_Reconstruct_r_sumpt_squared);
  CalcResolution(h_bias_PU_Reconstruct_z_sumpt_squared, h_res_PU_Reconstruct_z_sumpt_squared);

  CalcResolution_twoD(h_bias_PU_Reconstruct_x_sumpt_squared_ntracks, h_res_PU_Reconstruct_x_sumpt_squared_ntracks);
  CalcResolution_twoD(h_bias_PU_Reconstruct_y_sumpt_squared_ntracks, h_res_PU_Reconstruct_y_sumpt_squared_ntracks);
  CalcResolution_twoD(h_bias_PU_Reconstruct_r_sumpt_squared_ntracks, h_res_PU_Reconstruct_r_sumpt_squared_ntracks);
  CalcResolution_twoD(h_bias_PU_Reconstruct_z_sumpt_squared_ntracks, h_res_PU_Reconstruct_z_sumpt_squared_ntracks);

  CalcResolution(h_bias_PU_Reconstruct_x_lPUdz, h_res_PU_Reconstruct_x_lPUdz);
  CalcResolution(h_bias_PU_Reconstruct_y_lPUdz, h_res_PU_Reconstruct_y_lPUdz);
  CalcResolution(h_bias_PU_Reconstruct_r_lPUdz, h_res_PU_Reconstruct_r_lPUdz);
  CalcResolution(h_bias_PU_Reconstruct_z_lPUdz, h_res_PU_Reconstruct_z_lPUdz);

  CalcResolution(h_bias_PU_Reconstruct_x_ntracks, h_res_PU_Reconstruct_x_ntracks);
  CalcResolution(h_bias_PU_Reconstruct_y_ntracks, h_res_PU_Reconstruct_y_ntracks);
  CalcResolution(h_bias_PU_Reconstruct_r_ntracks, h_res_PU_Reconstruct_r_ntracks);
  CalcResolution(h_bias_PU_Reconstruct_z_ntracks, h_res_PU_Reconstruct_z_ntracks);

  CalcResolution(h_bias_PU_Reconstruct_x_zPosition, h_res_PU_Reconstruct_x_zPosition);
  CalcResolution(h_bias_PU_Reconstruct_y_zPosition, h_res_PU_Reconstruct_y_zPosition);
  CalcResolution(h_bias_PU_Reconstruct_r_zPosition, h_res_PU_Reconstruct_r_zPosition);
  CalcResolution(h_bias_PU_Reconstruct_z_zPosition, h_res_PU_Reconstruct_z_zPosition);

  return 0;
}
