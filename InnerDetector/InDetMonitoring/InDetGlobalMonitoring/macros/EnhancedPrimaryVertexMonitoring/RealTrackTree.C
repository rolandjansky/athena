/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define RealTrackTree_cxx
#include "RealTrackTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <TMath.h>
#include "TrigDecisionToolD3PD.h"

RealTrackTree::RealTrackTree(TTree *tree, TTree* trigTree) : InDetTrackTree(tree), trigMetaDataTree(trigTree) 
{
  m_maxEvents = -1;
  m_mcAvailable = false;
  triggerName = "";
  m_outfileName = "";
}

void RealTrackTree::Loop()
{
   if (fChain == 0) return;

//Settings
//------------------------------
   int cfgSplitVertexTrkInvFraction = 2;
   float cfgDistanceSplitVxMatch = 5;
   int cfgSplitMatchingMetric = 3;

   bool debug = false;
   //bool debug = true;

   // settings
   //VtxDistanceMetric cfgMetric = VtxDistM_deltaZ;
   //Float_t cfgMaxDistance = 0.5;
   VtxDistanceMetric cfgMetric = VtxDistM_deltaZsig;
   Float_t cfgMaxDistance = 5;
   //Float_t cfgMaxDistance = 3;
   //VtxDistanceMetric cfgMetric = VtxDistM_3Dsig;
   //Float_t cfgMaxDistance = 5;
   cout << "True Vtx-Reco matching using cfgMetric: " << cfgMetric << ", distance: " << cfgMaxDistance << endl;


//HISTOGRAMS
//------------------------------
// histo names and values should be identical to what is in InnerDetector/InDetMonitoring/InDetGlobalMonitoring/src/InDetGlobalEnhancedPrimaryVertexMonTool.cxx
   float minBinNTrk(-0.5); float maxBinNTrk(149.5); unsigned int numBinsNTrk((unsigned int)(maxBinNTrk-minBinNTrk));
   float minBinSumPt2(0.); float maxBinSumPt2(25.); unsigned int numBinsSumPt2((unsigned int)( (maxBinSumPt2-minBinSumPt2) / 0.25 )); //bins size = 0.25

   // PRIMARY VERTEX WITH NO BEAM CONSTRAINT
   TH2F* h_Vrt_Xerr_vs_ntrk = define2DHistogram("Vrt_Xerr_vs_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 200, 0., 1., "Number of Tracks" ,"x vertex Error [mm]");
   TH2F* h_Vrt_Yerr_vs_ntrk = define2DHistogram("Vrt_Yerr_vs_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 200, 0., 1., "Number of Tracks" ,"y vertex Error [mm]");
   TH2F* h_Vrt_Zerr_vs_ntrk = define2DHistogram("Vrt_Zerr_vs_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 400, 0., 2., "Number of Tracks" ,"z vertex Error [mm]");

   TH2F* h_Vrt_Xerr_vs_pt2 = define2DHistogram("Vrt_Xerr_vs_pt2", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 200, 0., 1., "#sqrt{#sum p_{T}^{2}} [GeV]" ,"x vertex Error [mm]");
   TH2F* h_Vrt_Yerr_vs_pt2 = define2DHistogram("Vrt_Yerr_vs_pt2", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 200, 0., 1., "#sqrt{#sum p_{T}^{2}} [GeV]" ,"y vertex Error [mm]");
   TH2F* h_Vrt_Zerr_vs_pt2 = define2DHistogram("Vrt_Zerr_vs_pt2", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 400, 0., 2., "#sqrt{#sum p_{T}^{2}} [GeV]" ,"z vertex Error [mm]");

   TH1F* h_Vrt_Ntrk = define1DHistogram("Vrt_Ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, "Number of tracks @ vertex", "Events");
   TH1F* h_Vrt_Tag_Ntrk = define1DHistogram("Vrt_Tag_Ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, "Number of tracks @ Tagged vertex", "Events");
   TH1F* h_Vrt_SumPt2 = define1DHistogram("Vrt_SumPt2", 100, 0, 25., "#sqrt{#sum p_{T}^{2}} [GeV]", "Events");
   TH1F* h_Vrt_Tag_SumPt2 = define1DHistogram("Vrt_Tag_SumPt2", 100, 0, 25., "#sqrt{#sum p_{T}^{2}} [GeV]", "Events");

   TH1F* h_VrtBC_Ntrk = define1DHistogram("VrtBC_Ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, "Number of tracks @ vertex (BC)", "Events");
   TH1F* h_VrtBC_Tag_Ntrk = define1DHistogram("VrtBC_Tag_Ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, "Number of tracks @ Tagged vertex (BC)", "Events");
   //TH1F* h_VrtBC_SumPt2 = define1DHistogram("VrtBC_SumPt2", 100, 0, 25., "#sqrt{#sum p_{T}^{2}} (BC Vtx) [GeV]", "Events");
   //TH1F* h_VrtBC_Tag_SumPt2 = define1DHistogram("VrtBC_Tag_SumPt2", 100, 0, 25., "#sqrt{#sum p_{T}^{2}} (BC Vtx) [GeV]", "Events");
   //Increased maximum SumPt2 to 100 GeV to look at high-pT events
   TH1F* h_VrtBC_SumPt2 = define1DHistogram("VrtBC_SumPt2", 100, 0, 100., "#sqrt{#sum p_{T}^{2}} (BC Vtx) [GeV]", "Events");
   TH1F* h_VrtBC_Tag_SumPt2 = define1DHistogram("VrtBC_Tag_SumPt2", 100, 0, 100., "#sqrt{#sum p_{T}^{2}} (BC Vtx) [GeV]", "Events");

   // error of tagged vertex only
   TH2F* h_Vrt_Tag_Xerr_vs_ntrk = define2DHistogram("Vrt_Tag_Xerr_vs_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 200, 0., 1., "Number of Tracks" ,"x vertex Error [mm]");
   TH2F* h_Vrt_Tag_Yerr_vs_ntrk = define2DHistogram("Vrt_Tag_Yerr_vs_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 200, 0., 1., "Number of Tracks" ,"y vertex Error [mm]");
   TH2F* h_Vrt_Tag_Zerr_vs_ntrk = define2DHistogram("Vrt_Tag_Zerr_vs_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 400, 0., 2., "Number of Tracks" ,"z vertex Error [mm]");

   TH2F* h_Vrt_Tag_Xerr_vs_pt2 = define2DHistogram("Vrt_Tag_Xerr_vs_pt2", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 200, 0., 1., "#sqrt{#sum p_{T}^{2}} [GeV]" ,"x vertex Error [mm]");
   TH2F* h_Vrt_Tag_Yerr_vs_pt2 = define2DHistogram("Vrt_Tag_Yerr_vs_pt2", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 200, 0., 1., "#sqrt{#sum p_{T}^{2}} [GeV]" ,"y vertex Error [mm]");
   TH2F* h_Vrt_Tag_Zerr_vs_pt2 = define2DHistogram("Vrt_Tag_Zerr_vs_pt2", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 400, 0., 2., "#sqrt{#sum p_{T}^{2}} [GeV]" ,"z vertex Error [mm]");

   // Fixed binning
   TH2F* h_Vrt_XpullVsNtrkAverage_split = define2DHistogram("Vrt_XpullVsNtrkAverage_split", numBinsNTrk, minBinNTrk, maxBinNTrk, 60, -6., 6., "N Tracks average", "x Pull value");
   TH2F* h_Vrt_YpullVsNtrkAverage_split = define2DHistogram("Vrt_YpullVsNtrkAverage_split", numBinsNTrk, minBinNTrk, maxBinNTrk, 60, -6., 6., "N Tracks average", "y Pull value");
   TH2F* h_Vrt_ZpullVsNtrkAverage_split = define2DHistogram("Vrt_ZpullVsNtrkAverage_split", numBinsNTrk, minBinNTrk, maxBinNTrk, 60, -6., 6., "N Tracks average", "z Pull value");

   TH2F* h_Vrt_XpullVsPt2Average_split = define2DHistogram("Vrt_XpullVsPt2Average_split", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 60, -6., 6., "#sqrt{#sum p_{T}^{2}} average [GeV]" ,"x pull value");
   TH2F* h_Vrt_YpullVsPt2Average_split = define2DHistogram("Vrt_YpullVsPt2Average_split", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 60, -6., 6., "#sqrt{#sum p_{T}^{2}} average [GeV]" ,"y pull value");
   TH2F* h_Vrt_ZpullVsPt2Average_split = define2DHistogram("Vrt_ZpullVsPt2Average_split", numBinsSumPt2, minBinSumPt2, maxBinSumPt2, 60, -6., 6., "#sqrt{#sum p_{T}^{2}} average [GeV]" ,"z pull value");

   // SPLIT VERTICES: efficiency
   // number of tag and probe split vertices reconstructed as a function of the number of tracks at the probe vertex                                
   TH1F* h_Vrt_split_tag_ntrk = define1DHistogram("Vrt_split_tag_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk, 
					    "Number of tracks @ tag vertex", "N. events with reconstructed tag");
   TH1F* h_Vrt_split_probe_ntrk = define1DHistogram("Vrt_split_probe_ntrk",  numBinsNTrk, minBinNTrk, maxBinNTrk,
						    "Number of tracks @ probe vertex", "N. events with both tag and probe reconstructed");
   // number of tag and probe split vertices reconstructed and matched to original non-BC vertex                                                    
   // as a function of the number of tracks at the probe vertex                                                                                     
   TH1F* h_Vrt_split_matched_tag_ntrk = define1DHistogram("Vrt_split_matched_tag_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk,
							  "Number of tracks @ tag vertex", "N. events with reconstructed tag");
   TH1F *h_Vrt_split_matched_probe_ntrk = define1DHistogram("Vrt_split_matched_probe_ntrk", numBinsNTrk, minBinNTrk, maxBinNTrk,
							    "Number of tracks @ probe vertex", "N. events with both tag and probe reconstructed");
   // distance between split vertices and non-BC vertex
   TH1F *h_Vrt_split_dist_tag   = define1DHistogram("Vrt_split_dist_tag", 100, 0., 20.,
						    "Distance of Tag split vertex from reconstructed", "Entries");
   TH1F *h_Vrt_split_dist_probe = define1DHistogram("Vrt_split_dist_probe", 100, 0., 20.,
						    "Distance of Probe split vertex from reconstructed", "Entries");

   // number of vertices per event
   TH1F *h_Vrt_BCNumberOfVertices   = define1DHistogram("Vrt_BCNumberOfVertices", 20, -0.5, 19.5, "BC N Vertices", "Entries");
   TH1F *h_Vrt_nBCNumberOfVertices  = define1DHistogram("Vrt_nBCNumberOfVertices", 20, -0.5, 19.5, "nBC N Vertices", "Entries");

   // Extra plots for D3PD-only analysis
   TH1F *h_Vrt_BCNumberOfVertices_4trk   = define1DHistogram("Vrt_BCNumberOfVertices_4trk", 20, -0.5, 19.5, "BC N Vertices w/ N_{tracks} >= 4", "Entries");
   TH1F *h_Vrt_nBCNumberOfVertices_4trk  = define1DHistogram("Vrt_nBCNumberOfVertices_4trk", 20, -0.5, 19.5, "nBC N Vertices  w/ N_{tracks} >= 4", "Entries");

   // Extra MC-based plots
   TH1F *h_Vrt_trueNumberOfVertices = 0;
   TH1F *h_Vrt_BC_truth_Nvtx = 0;
   TH1F *h_Vrt_BC_truth_Ratio_Nvtx = 0;
   TH1F *h_Vrt_BC_truth_Delta_Nvtx = 0;
   TH1F *h_Vrt_BC_truth_Sigma_Delta_Nvtx = 0;
   TH1F *h_Vrt_truth_delta_z=0;
   if(m_mcAvailable)  {
     h_Vrt_trueNumberOfVertices = define1DHistogram("Vrt_trueNumberOfVertices", 20, -0.5, 19.5, "True N Vertices", "Entries");
     h_Vrt_BC_truth_Nvtx = define1DHistogram("h_Vrt_BC_truth_Nvtx", 20, -0.5, 19.5, "True N Vertices", "Reconstructed N Vertices");
     h_Vrt_BC_truth_Ratio_Nvtx = define1DHistogram("h_Vrt_BC_truth_Ratio_Nvtx", 20, -0.5, 19.5, 
						   "True N Vertices", "<Reconstructed/True> N Vertices");
     h_Vrt_BC_truth_Delta_Nvtx = define1DHistogram("h_Vrt_BC_truth_Delta_Nvtx", 20, -0.5, 19.5, 
						   "True N Vertices", "<True - Reconstructed> N Vertices");
     h_Vrt_BC_truth_Sigma_Delta_Nvtx = define1DHistogram("h_Vrt_BC_truth_Sigma_Delta_Nvtx", 20, -0.5, 19.5, 
							 "True N Vertices", "#sigma(True - Reconstructed) N Vertices");
     h_Vrt_truth_delta_z = define1DHistogram("h_Vrt_truth_delta_z",1000, 0, 250.,
					     "#Delta z (mm)", "Entries");
   }

   // Vertex reco-true association plots (denominator: h_Vrt_trueNumberOfVertices)
   //histograms
   TH1F *h_MCVrt_RecoSimu_VtxM[VtxM_NMatch];
   TH1F *h_MCVrt_Num_SimuVtxGood; //events as function of simulated good vertices
   TH1F *h_MCVrt_Num_RecoVtxGood; // events as function of reconstructed good vertices
   for (int nv=0; nv<VtxM_NMatch;nv++)
     h_MCVrt_RecoSimu_VtxM[nv] = 0;
   if(m_mcAvailable)  {
     h_MCVrt_Num_SimuVtxGood = define1DHistogram("h_MCVrt_Num_SimuVtxGood", 20, -0.5, 19.5, "Number of generated good Vertices", "Events");
     h_MCVrt_Num_RecoVtxGood = define1DHistogram("h_MCVrt_Num_RecoVtxGood", 20, -0.5, 19.5, "Number of reconstructed good Vertices", "Events");
     h_MCVrt_RecoSimu_VtxM[VtxM_Match] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Match", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Match");
     h_MCVrt_RecoSimu_VtxM[VtxM_Split] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Split", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Split");
     h_MCVrt_RecoSimu_VtxM[VtxM_Merge] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Merge", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Merge");
     h_MCVrt_RecoSimu_VtxM[VtxM_Ineff] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Ineff", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Ineff");
     h_MCVrt_RecoSimu_VtxM[VtxM_Fake] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Fake", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Fake");
     h_MCVrt_RecoSimu_VtxM[VtxM_Match_2] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Match_2", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Match 2 Vtx");
     h_MCVrt_RecoSimu_VtxM[VtxM_Others] = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Others", 20, -0.5, 19.5, "Number of generated Vertices", "Fraction of Vertices: Others");
   }
   // Distances for reco-true matching
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Match = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Match", 100, 0, cfgMaxDistance, "Distance reco'ed-generated vertex", "Matched vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1 = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1", 100, 0, cfgMaxDistance, 
								    "Distance nearest_reco-generated vertex", "Split vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2 = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2", 100, 0, cfgMaxDistance, 
								    "Distance second_reco-generated vertex", "Split vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Split_rec = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Split_rec", 100, 0, cfgMaxDistance, "Distance reco-reco vertex", "Split vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen", 100, 0, cfgMaxDistance, 
								       "Distance nearest reco'ed-generated vertex", "Merge vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen", 100, 0, cfgMaxDistance, 
									"Distance reco'ed-second_generated vertex", "Merge vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Fake = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Fake", 100, 0, cfgMaxDistance*10, "Distance nearest generated vertex", "Fake vertices");
   TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Ineff = define1DHistogram("h_MCVrt_RecoSimu_VtxM_Dist_Ineff", 100, 0, cfgMaxDistance*10, "Distance nearest reco'ed vertex", "Ineff vertices");

   //Histogram for merging measurement: events with 2 vertices only
   TH1F *h_2vtx_merge_deltaZ = define1DHistogram("h_2vtx_merge_deltaZ", 1000, -250, 250, "#Delta z (2-vertex events)", "Events/mm");
   TH1F *h_2vtx_merge_deltaZsig = define1DHistogram("h_2vtx_merge_deltaZsig", 500, -500, 500, "#Delta z/#sigma (2-vertex events)", "Events/mm");
   
   //Setup trigger D3PD decoding tool
   //------------------------------
   D3PD::TrigDecisionToolD3PD* triggerTool(0);
   if (trigMetaDataTree!=0) triggerTool = new D3PD::TrigDecisionToolD3PD(fChain, trigMetaDataTree);
   cout << "Trigger filter:" << triggerName << endl;

   //LOOP over D3PD events
   //------------------------------
   //Long64_t nentries = fChain->GetEntries();
   Long64_t nentries = fChain->GetEntriesFast();
   //   nentries = 100000;

   Long64_t numEventsGRL(0);
   Long64_t numEventsPassTrigger(0);
   Long64_t numEventsGoodVtxBC(0);
   Long64_t numEventsGoodVtxNoBC(0);
   Long64_t numEventsResolution(0);
   Long64_t numEventsEfficiency(0);
   Long64_t numVertexSimu(0);
   Long64_t numVertexReco(0);
   Long64_t numVertexSimuGood(0);
   Long64_t numVertexRecoGood(0);
   Long64_t numVertexMatch[VtxM_NMatch];
   for (int nv=0; nv<VtxM_NMatch;nv++)
     numVertexMatch[nv] = 0;

   if (m_maxEvents >= 0)
     nentries = m_maxEvents;
   std::cout << "There are " << nentries << " events in the input files." << std::endl;

   Long64_t nbytes = 0, nb = 0;
   Long64_t jentry=0;
   for (jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (triggerTool) { nb = triggerTool->GetEntry(jentry); nbytes += nb; }

      //print out progress
      if (jentry % 20000 == 0)
	cout << "Processed " << jentry << " events so far." << endl;

      // GRL
      if ( ! isGoodRun(ei_RunNumber, ei_lbn) )
	continue;

      numEventsGRL++;

      // Trigger filtering
      if (triggerTool) { if (!(triggerTool->IsPassed(triggerName.Data())) ) continue; }

      numEventsPassTrigger++;

      // True info vertices
      //-------------------     
      VtxMatch VtxBC_Class[vxbc_n-1]; //keep track of classification result for BC vertices
      for (int rvx=0; rvx < vxbc_n-1; rvx++)
	VtxBC_Class[rvx]=VtxM_NMatch; //init
      if(m_mcAvailable)
      {
       unsigned int numVerticesInTime(0);
       for (unsigned int ii = 0 ; ii < pevt_n ; ++ii)
       {
        if (pevt_time->at(ii) == 0. && (pevt_type->at(ii) == 0 || pevt_type->at(ii) == 1) ) numVerticesInTime++;
       } 
       if (numVerticesInTime != mcVx_n) {
	 cerr << "WARNING: numVerticesInTime ( " << numVerticesInTime << ") != mcVx_n (" << mcVx_n << ")" << endl;
	 cerr << "\t [mcVx_x] = " << mcVx_x->size() << ", [mcVx_y] = " << mcVx_y->size() << ", [mcVx_z] = " << mcVx_z->size() << endl;
       }
       h_Vrt_trueNumberOfVertices->Fill(numVerticesInTime);
       numVertexSimu += mcVx_n;
       numVertexReco += vxbc_n-1;

       h_Vrt_BC_truth_Nvtx->Fill(numVerticesInTime, vxbc_n - 1);
       h_Vrt_BC_truth_Delta_Nvtx->Fill(numVerticesInTime, numVerticesInTime - (vxbc_n - 1) );
       h_Vrt_BC_truth_Sigma_Delta_Nvtx->Fill(numVerticesInTime, TMath::Power(numVerticesInTime - (vxbc_n - 1), 2)); //store suqare for RMS
       //Fill distance between generated vertices
       for (unsigned int iv=0; iv < mcVx_n-1; iv++)
	 for (unsigned int iv2=iv+1; iv2 < mcVx_n; iv2++) {
	   h_Vrt_truth_delta_z->Fill(TMath::Abs((*mcVx_z)[iv] - (*mcVx_z)[iv2]));
	   double delta_z = TMath::Abs((*mcVx_z)[iv] - (*mcVx_z)[iv2]);
	   // debug if "duplicates" are found in the same event
	   if (delta_z < 0.00001) {
	     cout << "Two VERY-NEAR generated vertices: Evt " << ei_EventNumber << ", mcVx_z[" << iv << "]=" << (*mcVx_z)[iv] << ", mcVx_z[" << iv2 << "]=" << (*mcVx_z)[iv2] << endl;
	   }
	 }

       // Fill vertex match distributions
       //-------------------     
       // We focus on numbers with denominator: simulated vertices
       // every reco/generated vertex counts only once 
       // every generated vertex counts 1
       // (every non-matched reconstructed vertex counts 1 for fakes)
       //debug print-out of the event
       if (debug) {
	 cout << "Event " << ei_EventNumber << endl;
	 cout << "N. simulated vertices (idx, x, y, z): " << mcVx_n << endl;
	 for (int gvx=0; gvx < mcVx_n; gvx++) {
	   Int_t ngoodTracks = 0;
	   for (int it=0; it < mc_n; it++) 
	     if (mc_event_number->at(it) == mcVx_event_number->at(gvx))
	       if (mc_gen_pt->at(it) >= 150. and TMath::Abs(mc_gen_eta->at(it)) < 2.5 and TMath::Abs(mc_charge->at(it)) == 1)
		 ngoodTracks++;
	   cout << " " << gvx << ": " << (*mcVx_x)[gvx] << ", " << (*mcVx_y)[gvx] << ", " << (*mcVx_z)[gvx] << ", nGoodTracks= " << ngoodTracks
		<< ", isGood=" << isGoodVertex(gvx, VtxSimulated) << ", Evt# " << mcVx_event_number->at(gvx) << endl;
	   cout << " Gen Tracks (id, pT, charge, eta, phi, type, status, mass, mc_event_number):" << endl;
	   for (int it=0; it < mc_n; it++) 
	     if (mc_event_number->at(it) == mcVx_event_number->at(gvx))
	       if (mc_gen_pt->at(it) >= 150. and TMath::Abs(mc_gen_eta->at(it)) < 2.5 and TMath::Abs(mc_charge->at(it)) == 1)
		 cout << "  " << it << " " << mc_gen_pt->at(it) << ", " << mc_charge->at(it) <<", " << mc_gen_eta->at(it) << ", " << mc_gen_phi->at(it)  << ", " << mc_gen_type->at(it)  << ", " 
		      << mc_gen_status->at(it) << ", " << TMath::Sqrt(TMath::Power(mc_gen_E->at(it),2) - TMath::Power(mc_gen_pt->at(it)/TMath::Sin(TMath::ATan(TMath::Exp(-(mc_gen_eta->at(it))))),2)) << ", " << mc_event_number->at(it) << endl;
	 }
	 cout << "N. reconstructed BC (idx, x, y, z) [includes dummy]: " << vxbc_n << endl;
	 for (int rvx=0; rvx < vxbc_n; rvx++) 
	   cout << " " << rvx << ": " << (*vxbc_x)[rvx] << ", " << (*vxbc_y)[rvx] << ", " << (*vxbc_z)[rvx] << "+/-" << (*vxbc_cov_z)[rvx] << ", nGoodTracks= " << vxbc_trk_n->at(rvx)
		<< ", isGood=" << isGoodVertex(rvx, VtxReconstructedBC) << endl;
	 cout << "Total n. reco tracks: " << trk_n << endl;
	 cout << " Tracks (pt, eta, phi, probMC, MCidx)" << endl;
	 for (int it=0; it < trk_n; it++)
	   cout << " " << it << trk_pt->at(it) << ", " << trk_eta->at(it) << ", " << trk_phi_wrtPV->at(it) << ", " << trk_mc_probability->at(it) << ", " << trk_mc_index->at(it) << endl;
       }
       //keep track of analyzed vertices
       bool recoVtxCounted[vxbc_n-1]; //keep track of reconstructed vertices already counted
       bool genVtxCounted[mcVx_n]; //keep track of generated vertices already counted
       Int_t numVtxGood(0);
       for (int gvx=0; gvx < mcVx_n; gvx++) 
	 if (isGoodVertex(gvx, VtxSimulated)) {
	   genVtxCounted[gvx]=false;
	   numVtxGood++;
	 } else {
	   genVtxCounted[gvx]=true; // do not count this vertex
	 }
       h_MCVrt_Num_SimuVtxGood->Fill(numVtxGood);
       numVertexSimuGood += numVtxGood;
       numVtxGood=0;
       for (int rvx=0; rvx < vxbc_n-1; rvx++)
	 if (isGoodVertex(rvx, VtxReconstructedBC)) {
	   recoVtxCounted[rvx]=false;
	   numVtxGood++;
	 } else {
	   recoVtxCounted[rvx]=true; // do not count this vertex
	 }
       numVertexRecoGood += numVtxGood;
       h_MCVrt_Num_RecoVtxGood->Fill(numVtxGood);
	   
       // loop over reconstructed vertices
       for (int rvx=0; rvx < vxbc_n-1; rvx++) {
	 if (recoVtxCounted[rvx])
	   continue; //this vertex has already been counted
	 recoVtxCounted[rvx]=true; //now we will count this vertex somehow
	 //get nearby generated vertices
	 Int_t numNearbyGen = 0;
	 vector<Int_t> idxNearbyGen;
	 for (int gvx=0; gvx < mcVx_n; gvx++) {
	   double dist = TMath::Abs(GetVtxDistance(gvx, VtxSimulated, rvx, VtxReconstructedBC, cfgMetric));
	   if (dist < cfgMaxDistance and not genVtxCounted[gvx]) {	     
	     numNearbyGen++;
	     idxNearbyGen.push_back(gvx);
	     // do not use this generated vertex anymore
	     genVtxCounted[gvx]=true;
	   }
	 }
	 //get other nearby reconstructed vertices
	 Int_t numNearbyReco = 0;
	 vector<Int_t> idxNearbyReco;
	 for (int rvx2=0; rvx2 < vxbc_n-1; rvx2++) {
	   if (rvx2 == rvx) continue; //skip same vertex
	   double dist = TMath::Abs(GetVtxDistance(rvx, VtxReconstructedBC, rvx2, VtxReconstructedBC, cfgMetric));
	   if (dist < cfgMaxDistance and not recoVtxCounted[rvx2]) {
	     idxNearbyReco.push_back(rvx2);
	     numNearbyReco++;
	     // do not look at the other reco vertices
	     recoVtxCounted[rvx2]=true;
	   }
	 }
	 if (debug) {
	   cout << rvx << ": numNearbyGen = " << numNearbyGen << ", numNearbyReco = " << numNearbyReco << endl;
	 }
	 if (numNearbyGen == 0) {
	   // 0-1: fake vertex
	   h_MCVrt_RecoSimu_VtxM[VtxM_Fake]->Fill(mcVx_n);
	   numVertexMatch[VtxM_Fake]++;
	   VtxBC_Class[rvx] = VtxM_Fake;
	   // here's an exception: we still want to consider the other reconstructed vertices
	   for (int iv=0; iv < idxNearbyReco.size(); iv++)
	     recoVtxCounted[idxNearbyReco[iv]] = false;
	   if (debug)
	     cout << "numVertexMatch[VtxM_Fake]++" << endl;
	   //get nearest generated vertex
	   double nearestDist=-1;
	   for (int gvx=0; gvx < mcVx_n; gvx++) {
	     double dist = TMath::Abs(GetVtxDistance(gvx, VtxSimulated, rvx, VtxReconstructedBC, cfgMetric));
	     if ((nearestDist < 0) or (dist < nearestDist)) //abs not needed: always positive distances defined up to now
	       nearestDist = dist;
	   }
	   h_MCVrt_RecoSimu_VtxM_Dist_Fake->Fill(nearestDist);
	 } else
	 if ( (numNearbyGen == 1) and (numNearbyReco == 0) ) {
	   // 1-1: match
	   h_MCVrt_RecoSimu_VtxM[VtxM_Match]->Fill(mcVx_n);
	   numVertexMatch[VtxM_Match]++;
	   VtxBC_Class[rvx] = VtxM_Match;
	   if (debug)
	     cout << "numVertexMatch[VtxM_Match]++" << endl;
	   h_MCVrt_RecoSimu_VtxM_Dist_Match->Fill(TMath::Abs(GetVtxDistance(idxNearbyGen[0],VtxSimulated, rvx, VtxReconstructedBC, cfgMetric)));
	 } else
	 if ( (numNearbyGen == 1) and (numNearbyReco == 1) ) {
	   // 1-2: split
	   // Note: The other vertex has been flagged in recoVtxCounted 
	   // and will not be double-counted
	   h_MCVrt_RecoSimu_VtxM[VtxM_Split]->Fill(mcVx_n);
	   numVertexMatch[VtxM_Split]++;
	   VtxBC_Class[rvx] = VtxM_Split;
	   VtxBC_Class[idxNearbyReco[0]] = VtxM_Split;
	   if (debug)
	     cout << "numVertexMatch[VtxM_Split]++" << endl;
	   double dist1 = TMath::Abs(GetVtxDistance(idxNearbyGen[0],VtxSimulated, rvx, VtxReconstructedBC, cfgMetric));
	   double dist2 = TMath::Abs(GetVtxDistance(idxNearbyGen[0],VtxSimulated, idxNearbyReco[0], VtxReconstructedBC, cfgMetric));
	   if (dist1 <= dist2) {
	     h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1->Fill(dist1);
	     h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2->Fill(dist2);
	   } else {
	     h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1->Fill(dist2);
	     h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2->Fill(dist1);
	   }
	   h_MCVrt_RecoSimu_VtxM_Dist_Split_rec->Fill(TMath::Abs(GetVtxDistance(rvx, VtxReconstructedBC, idxNearbyReco[0], VtxReconstructedBC, cfgMetric)));
	 } else	 
	 if ( (numNearbyGen == 1) and (numNearbyReco >= 2) ) {
	   // 1-X: split --> goes to VtxM_Split right now
	   // Note: The other vertices has been flagged in recoVtxCounted 
	   // and will not be double-counted
	   h_MCVrt_RecoSimu_VtxM[VtxM_Split]->Fill(mcVx_n);
	   numVertexMatch[VtxM_Split]++;
	   VtxBC_Class[rvx] = VtxM_Split;
	   for (int svx=0; svx < idxNearbyReco.size(); svx++)
	     VtxBC_Class[svx] = VtxM_Split;
	   if (debug)
	     cout << "numVertexMatch[VtxM_Split]++" << endl;
	   //Note: do not fill histograms in this case.
	 } else
	 if ( (numNearbyGen == 2) and (numNearbyReco == 0) ) {
	   // 2-1: merge
	   // Count x2, since we want to use as denominator
	   // the simulated vertices
	   h_MCVrt_RecoSimu_VtxM[VtxM_Merge]->Fill(mcVx_n, 2);
	   numVertexMatch[VtxM_Merge]+=2;
	   VtxBC_Class[rvx] = VtxM_Merge;
	   if (debug)
	     cout << "numVertexMatch[VtxM_Merge]++" << endl;
	   double dist1 = TMath::Abs(GetVtxDistance(idxNearbyGen[0], VtxSimulated, rvx, VtxReconstructedBC, cfgMetric));
	   double dist2 = TMath::Abs(GetVtxDistance(idxNearbyGen[1], VtxSimulated, rvx, VtxReconstructedBC, cfgMetric));	   
	   if (dist1 <= dist2) {
	     h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen->Fill(dist1);
	     h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen->Fill(dist2);
	   } else {
	     h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen->Fill(dist2);
	     h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen->Fill(dist1);
	   }
	 } else
	 if ( (numNearbyGen == 2) and (numNearbyReco == 1) ) {
	   // 2-2: match, but 2 vertices close together -> VtxM_Match_2
	   // Note: The other vertices has been flagged in recoVtxCounted 
	   // and will not be double-counted, we need to account them here
	   h_MCVrt_RecoSimu_VtxM[VtxM_Match_2]->Fill(mcVx_n, 2);
	   numVertexMatch[VtxM_Match_2]+=2; 
	   VtxBC_Class[rvx] = VtxM_Match_2;
	   VtxBC_Class[idxNearbyGen[0]] = VtxM_Match_2;
	   if (debug)
	     cout << "numVertexMatch[VtxM_Match_2]++" << endl;
	 } else {
	   // 2-X, Y-Z: Other (X>=2, Y>=3, Z>=0)
	   // count as the number of nearby generated vertices
	   h_MCVrt_RecoSimu_VtxM[VtxM_Others]->Fill(mcVx_n, numNearbyGen);
	   numVertexMatch[VtxM_Others]+= numNearbyGen;
	   if (debug) 
	     cout << "numVertexMatch[VtxM_Others]++" << endl;
	 }
       } 
       //loop over generated vertices
       if (debug)
	 cout << "Loop over generated vertices." << endl;
       for (int gvx=0; gvx < mcVx_n; gvx++) {
	 if (not isGoodVertex(gvx, VtxSimulated))
	   continue;
	 Int_t numNearbyReco(0);
	 for (int rvx=0; rvx < vxbc_n-1; rvx++) {
	   if ( (TMath::Abs(GetVtxDistance(gvx, VtxSimulated, rvx, VtxReconstructedBC, cfgMetric)) < cfgMaxDistance) and
		(isGoodVertex(rvx, VtxReconstructedBC)) ) {
	     numNearbyReco++;
	   }
	 }
	 if (numNearbyReco == 0) {
	   //1-0: Ineff
	   h_MCVrt_RecoSimu_VtxM[VtxM_Ineff]->Fill(mcVx_n);
	   numVertexMatch[VtxM_Ineff]++;
	   if (debug) 
	     cout << gvx << ": numVertexMatch[VtxM_Ineff]++" << endl;
	   //sanity check
	   if (genVtxCounted[gvx])
	     cerr << "WARNING: generated vertex counted as inefficient but already used! Should never happen." << endl;
	   double nearestDist=-1;
	   for (int rvx=0; rvx < vxbc_n-1; rvx++) {
	     if (not isGoodVertex(rvx, VtxReconstructedBC))
	       continue;
	     double dist = TMath::Abs(GetVtxDistance(gvx, VtxSimulated, rvx, VtxReconstructedBC, cfgMetric));
	     if ((nearestDist < 0) or (dist < nearestDist)) 
	       nearestDist = dist;
	   }
	   h_MCVrt_RecoSimu_VtxM_Dist_Ineff->Fill(nearestDist);
	 }
       }
          
      }//end of mc availability check

      //Fill vertex multiplicity
      //-------------------     
      h_Vrt_BCNumberOfVertices->Fill(vxbc_n-1);
      h_Vrt_nBCNumberOfVertices->Fill(vxnbc_n-1);
      int nvx4trk=0;
      for (int itvx=0; itvx < vxbc_n - 1; itvx++) //do not count dummy
	if (vxbc_nTracks->at(itvx) >= 4)
	  nvx4trk++;
      h_Vrt_BCNumberOfVertices_4trk->Fill(nvx4trk);
      nvx4trk=0;
      for (int itvx=0; itvx < vxnbc_n - 1; itvx++) //do not count dummy
	if (vxnbc_nTracks->at(itvx) >= 4)
	  nvx4trk++;
      h_Vrt_nBCNumberOfVertices_4trk->Fill(nvx4trk);

      //Fill BC histograms
      //-------------------
      if (vxbc_n > 1) 
      {
        for (unsigned int uu = 0; uu < vxbc_n-1 ; uu++) {
	  // Filter on vertex truth-matching class
	  //if (m_mcAvailable)
	  //  if (VtxBC_Class[uu] != VtxM_Merge)
	  //    continue;

          float sum_pt2 = 0.;
          for(int trk = 0; trk < vxbc_trk_n->at(uu); ++trk) { sum_pt2 += TMath::Power(trk_pt->at(vxbc_trk_index->at(uu)[trk])/1e3, 2); }
	  h_VrtBC_Ntrk->Fill(vxbc_trk_n->at(uu));
	  h_VrtBC_SumPt2->Fill(TMath::Sqrt(sum_pt2));
	  if (uu == 0) {
	    h_VrtBC_Tag_Ntrk->Fill(vxbc_trk_n->at(uu));
	    h_VrtBC_Tag_SumPt2->Fill(TMath::Sqrt(sum_pt2));
	  }
	}
	numEventsGoodVtxBC++;
      }
      
      // Fill non-BC histograms
      //-------------------
      if (vxnbc_n > 1)
      {
	numEventsGoodVtxNoBC++;
        // comment out loop (NOT ITS CONTENT!) and uncomment next line to only look at reso of first vertex
        // unsigned int uu = 0;
        // for errors, default track D3PD stores cov not error
        double error_x(0.);
        double error_y(0.);
        double error_z(0.);
        for (unsigned int uu = 0; uu < vxnbc_n-1 ; uu++)
        {
          error_x = TMath::Sqrt(vxnbc_cov_x->at(uu));
          error_y = TMath::Sqrt(vxnbc_cov_y->at(uu));
          error_z = TMath::Sqrt(vxnbc_cov_z->at(uu));

          h_Vrt_Xerr_vs_ntrk->Fill(vxnbc_trk_n->at(uu), error_x);
          h_Vrt_Yerr_vs_ntrk->Fill(vxnbc_trk_n->at(uu), error_y);
          h_Vrt_Zerr_vs_ntrk->Fill(vxnbc_trk_n->at(uu), error_z);

          float sum_pt2 = 0.;
          for(int trk = 0; trk < vxnbc_trk_n->at(uu); ++trk) { sum_pt2 += TMath::Power(trk_pt->at(vxnbc_trk_index->at(uu)[trk])/1e3, 2); }

          h_Vrt_Xerr_vs_pt2->Fill(TMath::Sqrt(sum_pt2), error_x);
          h_Vrt_Yerr_vs_pt2->Fill(TMath::Sqrt(sum_pt2), error_y);
          h_Vrt_Zerr_vs_pt2->Fill(TMath::Sqrt(sum_pt2), error_z);

	  //if (vxnbc_trk_n->at(uu) >= 4) {
	  h_Vrt_Ntrk->Fill(vxnbc_trk_n->at(uu));
	  h_Vrt_SumPt2->Fill(TMath::Sqrt(sum_pt2));
	  //}

	  if (uu == 0) {
	    //Tagged vertex
	    h_Vrt_Tag_Xerr_vs_ntrk->Fill(vxnbc_trk_n->at(uu), error_x);
	    h_Vrt_Tag_Yerr_vs_ntrk->Fill(vxnbc_trk_n->at(uu), error_y);
	    h_Vrt_Tag_Zerr_vs_ntrk->Fill(vxnbc_trk_n->at(uu), error_z);
	    h_Vrt_Tag_Xerr_vs_pt2->Fill(TMath::Sqrt(sum_pt2), error_x);
	    h_Vrt_Tag_Yerr_vs_pt2->Fill(TMath::Sqrt(sum_pt2), error_y);
	    h_Vrt_Tag_Zerr_vs_pt2->Fill(TMath::Sqrt(sum_pt2), error_z);
	    //if (vxnbc_trk_n->at(uu) >= 4) {
	    h_Vrt_Tag_Ntrk->Fill(vxnbc_trk_n->at(uu));
	    h_Vrt_Tag_SumPt2->Fill(TMath::Sqrt(sum_pt2));
	    //}
	  }
        }
      }

      // Fill split-vertex resolution
      //-------------------
      if(vxsplit_n >= 2 && (vxsplit_type->at(0) == 1) && (vxsplit_type->at(1) == 1) ) // two split vertices. First two succesfully reconstructed
      {
	numEventsResolution++;

        float x_distance = vxsplit_x->at(0) - vxsplit_x->at(1);
        float y_distance = vxsplit_y->at(0) - vxsplit_y->at(1);
        float z_distance = vxsplit_z->at(0) - vxsplit_z->at(1);

        // cov = err**2
        float x_error = TMath::Sqrt(vxsplit_cov_x->at(0) + vxsplit_cov_x->at(1));
        float y_error = TMath::Sqrt(vxsplit_cov_y->at(0) + vxsplit_cov_y->at(1));
        float z_error = TMath::Sqrt(vxsplit_cov_z->at(0) + vxsplit_cov_z->at(1));

        float x_split_pull = x_distance/x_error;
        float y_split_pull = y_distance/y_error;
        float z_split_pull = z_distance/z_error;

        int ntrk_odd  =  vxsplit_trk_n->at(0);
        int ntrk_even =  vxsplit_trk_n->at(1);
        int ntrk_av = (ntrk_odd + ntrk_even)/2;

        h_Vrt_XpullVsNtrkAverage_split->Fill(ntrk_av, x_split_pull);
        h_Vrt_YpullVsNtrkAverage_split->Fill(ntrk_av, y_split_pull);
        h_Vrt_ZpullVsNtrkAverage_split->Fill(ntrk_av, z_split_pull);

       float sum_pt2_1 = 0.;
       float sum_pt2_2 = 0.;
       for(int trk = 0; trk < vxsplit_trk_n->at(0); ++trk)
       {
         sum_pt2_1 += TMath::Power(trk_pt->at(vxsplit_trk_index->at(0)[trk])/1e3,2);
       }
       float sqrt_sum_pt2_1(TMath::Sqrt(sum_pt2_1));

       for(int trk = 0; trk < vxsplit_trk_n->at(1); ++trk)
       {
         sum_pt2_2 += TMath::Power(trk_pt->at(vxsplit_trk_index->at(1)[trk] )/1e3,2);
       }
       float sqrt_sum_pt2_2(TMath::Sqrt(sum_pt2_2));

       float sqrt_sum_pt2_average((sqrt_sum_pt2_1+sqrt_sum_pt2_2)/2.);
       h_Vrt_XpullVsPt2Average_split->Fill(sqrt_sum_pt2_average, x_split_pull);
       h_Vrt_YpullVsPt2Average_split->Fill(sqrt_sum_pt2_average, y_split_pull);
       h_Vrt_ZpullVsPt2Average_split->Fill(sqrt_sum_pt2_average, z_split_pull);
      } //end: split vertex resolution

      // Fill split-vertex efficiency
      //-------------------
      const int Trk__PriVtx = 1; //type of vertex: Trk::PriVtx
      const int splitTagIdx = 0; //tag vertex in split-vertex collection
      const int splitProbeIdx = 1; //probe vertex in split-vertex collection
      const int origVxIdx = 0; //original vertex in non-BC collection
      // exactly two split vertices and one reco non-BC vertex. Tag vertex succesfully reconstructed.
      int nSplitPairsGood(0);
      for (int sid=0; sid < vxsplit_n / 2; sid++)
	if (vxsplit_type->at(2*sid) == Trk__PriVtx || vxsplit_type->at(2*sid+1) == Trk__PriVtx)
	  nSplitPairsGood++; //count split pairs with at least one reconstructed split. Safe against "split-dummy"
      //      if ((vxsplit_n == 2) && (vxnbc_n == 2) && (vxsplit_type->at(splitTag) == Trk__PriVtx)) { 
      //      if ((vxsplit_n == 4) && (vxnbc_n == 2) && (vxsplit_type->at(splitTagIdx) == Trk__PriVtx)) {  //split vertex with dummy
      if (debug) {
	cout << "vxContainerWithoutBeamConstraint.size = " << vxnbc_n << endl;
	cout << "origVertexNoBC.vertexType = "             << vxnbc_type->at(0) << endl;
	cout << "vxContainerSplit.size = "                 << vxsplit_n << endl;
	cout << "splitVxCandiate1.vertexType = "           << (vxsplit_n > 0 ? vxsplit_type->at(0) : -1) << endl;
	cout << "splitVxCandiate2.vertexType = "           << (vxsplit_n > 1 ? vxsplit_type->at(1) : -1) << endl;
	cout << "nSplitPairsGood = "                       << nSplitPairsGood << endl;
      }
      if ((nSplitPairsGood == 1) && (vxnbc_n == 2) && (vxsplit_type->at(splitTagIdx) == Trk__PriVtx)) {  
	numEventsEfficiency++;
	//number of tracks for efficiency
	int nTracksForEff = vxnbc_nTracks->at(0) * (cfgSplitVertexTrkInvFraction - 1) / (cfgSplitVertexTrkInvFraction); 
	h_Vrt_split_tag_ntrk->Fill(nTracksForEff);
	if (debug)
	  cout << "nTracksForEff = " << nTracksForEff << endl;
	//fill tag histograms
	float distanceMatchTag = GetSplitMatchDistance(splitTagIdx, origVxIdx, cfgSplitMatchingMetric);
	h_Vrt_split_dist_tag->Fill(distanceMatchTag);
	//check if tag is matched
	if (distanceMatchTag <= cfgDistanceSplitVxMatch) 
	    h_Vrt_split_matched_tag_ntrk->Fill(nTracksForEff);
	//select events with probe vertex (second) reconstructed (type=Trk::PriVtx=1)...
	if (vxsplit_type->at(splitProbeIdx) == Trk__PriVtx) {
	  //fill probe histograms
	  h_Vrt_split_probe_ntrk->Fill(nTracksForEff);
	  float distanceMatchProbe = GetSplitMatchDistance(splitProbeIdx, origVxIdx, cfgSplitMatchingMetric);
	  h_Vrt_split_dist_probe->Fill(distanceMatchProbe);
	  //and near the primary reconstructed vertex
	  if (distanceMatchProbe <= cfgDistanceSplitVxMatch && distanceMatchTag <= cfgDistanceSplitVxMatch) 
	    h_Vrt_split_matched_probe_ntrk->Fill(nTracksForEff);
	}
      }
      
      // Merging histograms
      //-------------------
      if (vxbc_n == 2+1) {
	//plot delta z between vertices	
	h_2vtx_merge_deltaZ->Fill(GetVtxDistance(0, VtxReconstructedBC, 1, VtxReconstructedBC, VtxDistM_deltaZ));
	h_2vtx_merge_deltaZsig->Fill(GetVtxDistance(0, VtxReconstructedBC, 1, VtxReconstructedBC, VtxDistM_deltaZsig));	
      }

   } //end loop: d3pd entries

   //====================
   //post-processing
   //====================
   // Calculate average
   if (m_mcAvailable) {
     h_Vrt_trueNumberOfVertices->Sumw2();
     h_Vrt_BC_truth_Nvtx->Sumw2();
     h_Vrt_BC_truth_Delta_Nvtx->Sumw2();
     h_Vrt_BC_truth_Sigma_Delta_Nvtx->Sumw2();
     h_Vrt_BC_truth_Ratio_Nvtx->Sumw2();

     h_Vrt_BC_truth_Ratio_Nvtx->Divide(h_Vrt_BCNumberOfVertices, h_Vrt_trueNumberOfVertices);	 
     for (int nb=1; nb <= h_Vrt_BC_truth_Nvtx->GetXaxis()->GetNbins(); nb++) {
       if (h_Vrt_trueNumberOfVertices->GetBinContent(nb) > 0) {
	 h_Vrt_BC_truth_Nvtx->SetBinError(nb, TMath::Sqrt(h_Vrt_BC_truth_Nvtx->GetBinContent(nb)) / h_Vrt_trueNumberOfVertices->GetBinContent(nb));
	 h_Vrt_BC_truth_Nvtx->SetBinContent(nb, h_Vrt_BC_truth_Nvtx->GetBinContent(nb) / h_Vrt_trueNumberOfVertices->GetBinContent(nb));
	 h_Vrt_BC_truth_Delta_Nvtx->SetBinContent(nb, h_Vrt_BC_truth_Delta_Nvtx->GetBinContent(nb) / h_Vrt_trueNumberOfVertices->GetBinContent(nb));
	 h_Vrt_BC_truth_Delta_Nvtx->SetBinError(nb, h_Vrt_BC_truth_Nvtx->GetBinError(nb));
	 h_Vrt_BC_truth_Sigma_Delta_Nvtx->SetBinContent(nb, TMath::Sqrt((h_Vrt_BC_truth_Sigma_Delta_Nvtx->GetBinContent(nb) - TMath::Power(h_Vrt_BC_truth_Delta_Nvtx->GetBinContent(nb),2)) / h_Vrt_trueNumberOfVertices->GetBinContent(nb)) );
	 h_Vrt_BC_truth_Sigma_Delta_Nvtx->SetBinError(nb, 0);
       } else {
	 h_Vrt_BC_truth_Nvtx->SetBinContent(nb, 0);
	 h_Vrt_BC_truth_Delta_Nvtx->SetBinContent(nb, 0);
	 h_Vrt_BC_truth_Sigma_Delta_Nvtx->SetBinContent(nb, 0);
       }
     }
     // Vertex Match
     // normalize to number of vertices for each bin. Use h_Vrt_trueNumberOfVertices * n vertices (for each bin).
     TH1F *h_ntrueVertices = (TH1F*)h_MCVrt_Num_SimuVtxGood->Clone();
     h_ntrueVertices->SetName("h_ntrueVertices");
     for (int nbin=1; nbin <= h_ntrueVertices->GetXaxis()->GetNbins(); nbin++) {
       double nv = h_ntrueVertices->GetXaxis()->GetBinCenter(nbin);
       //cout << nbin << " " << h_ntrueVertices->GetBinContent(nbin) << " -> " << nv << endl;
       h_ntrueVertices->SetBinContent(nbin, nv*h_ntrueVertices->GetBinContent(nbin));
       h_ntrueVertices->SetBinError(nbin, nv*h_ntrueVertices->GetBinError(nbin));
     }
     // Normalize histograms per-event or per-vertex
     for (int nv=0; nv<VtxM_NMatch;nv++) {
       //normalize to total generated vertices       
       h_MCVrt_RecoSimu_VtxM[nv]->Sumw2();
       // per-vertex normalization
       h_MCVrt_RecoSimu_VtxM[nv]->Divide(h_ntrueVertices);
       // per-event normalization
       //h_MCVrt_RecoSimu_VtxM[nv]->Divide(h_MCVrt_Num_SimuVtxGood);
     }

   } //mc evailable


   // print stuff
   std::cout << " total events: "       << jentry               << std::endl;
   std::cout << " GRL events: "         << numEventsGRL         << std::endl;
   std::cout << " pass the trigger "    << triggerName << ": " << numEventsPassTrigger << std::endl;
   std::cout << " good vertex events (BC): " << numEventsGoodVtxBC    << std::endl;
   std::cout << " good vertex events (noBC): " << numEventsGoodVtxNoBC    << std::endl;
   std::cout << " good events Resolution: " << numEventsResolution    << std::endl;
   std::cout << " good events Efficiency: " << numEventsEfficiency    << std::endl;
   std::cout << " Average number of BC vertices: " << h_Vrt_BCNumberOfVertices->GetMean() << endl;
   std::cout << " Average number of nBC vertices: " << h_Vrt_nBCNumberOfVertices->GetMean() << endl;
   std::cout << " Average number of BC vertices w/ >= 4 tracks: " << h_Vrt_BCNumberOfVertices_4trk->GetMean() << endl;
   std::cout << " Average number of nBC vertices w/ >= 4 tracks: " << h_Vrt_nBCNumberOfVertices_4trk->GetMean() << endl;
   if (m_mcAvailable) {
     std::cout << " Average number of True vertices: " << h_Vrt_trueNumberOfVertices->GetMean() << endl;
     std::cout << " Total number of simulated vertices: " << numVertexSimu << endl;
     std::cout << " Total number of reconstructed BC vertices: " << numVertexReco << endl;
     std::cout << " Vertex Matches (denominator: number of simulated vertices): " << endl;
     std::cout << " Match: " << numVertexMatch[VtxM_Match]     << " (" << static_cast<double>(numVertexMatch[VtxM_Match]) / numVertexSimu << ")" << endl;
     std::cout << " Split: " << numVertexMatch[VtxM_Split]     << " (" << static_cast<double>(numVertexMatch[VtxM_Split]) / numVertexSimu << ")" << endl;
     std::cout << " Merge: " << numVertexMatch[VtxM_Merge]     << " (" << static_cast<double>(numVertexMatch[VtxM_Merge]) / numVertexSimu << ")" << endl;
     std::cout << " Ineff: " << numVertexMatch[VtxM_Ineff]     << " (" << static_cast<double>(numVertexMatch[VtxM_Ineff]) / numVertexSimu << ")" << endl;
     std::cout << " Fake: " << numVertexMatch[VtxM_Fake]       << " (" << static_cast<double>(numVertexMatch[VtxM_Fake]) / numVertexSimu << ")" << endl;
     std::cout << " Match_2: " << numVertexMatch[VtxM_Match_2] << " (" << static_cast<double>(numVertexMatch[VtxM_Match_2]) / numVertexSimu << ")" << endl;
     std::cout << " Others: " << numVertexMatch[VtxM_Others]   << " (" << static_cast<double>(numVertexMatch[VtxM_Others]) / numVertexSimu << ")" << endl;
     std::cout << " Check Sum Match+Split+Merge+Match_2+Ineff+Others (==numVertexSim): " << numVertexMatch[VtxM_Match]+numVertexMatch[VtxM_Split]+numVertexMatch[VtxM_Merge]+numVertexMatch[VtxM_Match_2]+numVertexMatch[VtxM_Ineff]+numVertexMatch[VtxM_Others] << std::endl;
   }


   // plot stuff
   // plot true number of vertices
   
   if(m_mcAvailable)
   {
    TCanvas* ctrue = new TCanvas("Truth Canvas", "Truth Canvas", 600,600);
   
    h_Vrt_trueNumberOfVertices->Draw();  
    
    h_Vrt_BCNumberOfVertices->SetLineColor(2);
    h_Vrt_BCNumberOfVertices->Draw("SAME");
  
    h_Vrt_nBCNumberOfVertices->SetLineColor(3);
    h_Vrt_nBCNumberOfVertices->Draw("SAME");
   } 
 
 
   
 
}//end of loop method

TH2F* RealTrackTree::define2DHistogram(TString name, unsigned int xbins, Double_t lxbin, Double_t uxbin, unsigned int ybins, Double_t lybin, Double_t uybin, TString xaxistitle, TString yaxistitle)
{
  TH2F* histogram = new TH2F(name, name, xbins, lxbin, uxbin, ybins, lybin, uybin);
  TAxis* xaxis = histogram->GetXaxis();
  xaxis->SetTitle(xaxistitle);
  TAxis* yaxis = histogram->GetYaxis();
  yaxis->SetTitle(yaxistitle);
  return histogram;
}

TH2F* RealTrackTree::define2DHistogram(TString name, unsigned int xbins, double* xrange, unsigned int ybins, Double_t lybin, Double_t uybin, TString xaxistitle, TString yaxistitle)
{
  TH2F* histogram = new TH2F(name, name, xbins, xrange, ybins, lybin, uybin);
  TAxis* xaxis = histogram->GetXaxis();
  xaxis->SetTitle(xaxistitle);
  TAxis* yaxis = histogram->GetYaxis();
  yaxis->SetTitle(yaxistitle);
  return histogram;
}

TH1F* RealTrackTree::define1DHistogram(TString name, unsigned int bins, Double_t lbin, Double_t ubin, TString xaxistitle, TString yaxistitle)
{
  TH1F* histogram = new TH1F(name, name, bins, lbin, ubin);
  TAxis* xaxis = histogram->GetXaxis();
  xaxis->SetTitle(xaxistitle);
  TAxis* yaxis = histogram->GetYaxis();
  yaxis->SetTitle(yaxistitle);
  return histogram;
}                   

Float_t RealTrackTree::GetSplitMatchDistance(Int_t splitVxIndex, Int_t recoVxIndex, Int_t cfgMetric)
{
  Int_t recoVxIndexReal = recoVxIndex;
  //evaluate distance between splitVx and recoVx
  if (recoVxIndexReal == -1) 
    recoVxIndexReal = splitVxIndex / 2; //integer division

  double dx = vxsplit_x->at(splitVxIndex) - vxnbc_x->at(recoVxIndexReal);
  double dy = vxsplit_y->at(splitVxIndex) - vxnbc_y->at(recoVxIndexReal);
  double dz = vxsplit_z->at(splitVxIndex) - vxnbc_z->at(recoVxIndexReal);
  //Error on distance is partially correlated with original vertex error: assume split vertex error dominant.
  double dxErr = TMath::Sqrt(vxsplit_cov_x->at(splitVxIndex));
  double dyErr = TMath::Sqrt(vxsplit_cov_y->at(splitVxIndex));
  double dzErr = TMath::Sqrt(vxsplit_cov_z->at(splitVxIndex));

  //there are different possibilities
  if (cfgMetric == 0) {
    // dummy metric: returns zero distance 
    return 0;
  } else if (cfgMetric == 1) {
    // simple delta-z metric
    return TMath::Abs(dz); // = |z_split - z_vtx|
  } else if (cfgMetric == 2) {
    //3-D distance divided by the error (dominated by Delta z)
    double d = TMath::Sqrt(dx*dx+dy*dy+dz*dz);
    double dErr = TMath::Sqrt(TMath::Power(dx*dxErr/d,2)+TMath::Power(dy*dyErr/d,2)+TMath::Power(dz*dzErr/d,2));
    return d/dErr;
  } else if (cfgMetric == 3) {
    //quadratic sum of significance distances in the 3 directions. Expected RMS = 1.
    return TMath::Sqrt(TMath::Power(dx/dxErr,2)+TMath::Power(dy/dyErr,2)+TMath::Power(dz/dzErr,2));
  }

  //error. invalid metric.
  cerr << "Invalid choice for the vertex selection efficiency metric." << endl;
  return 999.0;

}

Float_t RealTrackTree::GetVtxDistance(Int_t VxIndex1, VtxType VxType1, Int_t VxIndex2, VtxType VxType2, VtxDistanceMetric cfgMetric)
{
  double x[2] = {0,0}; //positions
  double y[2] = {0,0};
  double z[2] = {0,0};
  double e2x[2] = {0,0}; //errors^2
  double e2y[2] = {0,0};
  double e2z[2] = {0,0};
  Int_t VxIndex[2] = {VxIndex1, VxIndex2};
  VtxType VxType[2] = {VxType1, VxType2};
  for (int ivx=0; ivx < 2; ivx++) {
    switch (VxType[ivx]) {
    case VtxSimulated:
      x[ivx] = mcVx_x->at(VxIndex[ivx]);
      y[ivx] = mcVx_y->at(VxIndex[ivx]);
      z[ivx] = mcVx_z->at(VxIndex[ivx]);
      //leave errors at 0
    break;
    case VtxReconstructedBC:
      x[ivx] = vxbc_x->at(VxIndex[ivx]);
      y[ivx] = vxbc_y->at(VxIndex[ivx]);
      z[ivx] = vxbc_z->at(VxIndex[ivx]);
      e2x[ivx] = vxbc_cov_x->at(VxIndex[ivx]);
      e2y[ivx] = vxbc_cov_y->at(VxIndex[ivx]);
      e2z[ivx] = vxbc_cov_z->at(VxIndex[ivx]);
      break;
    case VtxReconstructedNoBC:
      x[ivx] = vxnbc_x->at(VxIndex[ivx]);
      y[ivx] = vxnbc_y->at(VxIndex[ivx]);
      z[ivx] = vxnbc_z->at(VxIndex[ivx]);
      e2x[ivx] = vxnbc_cov_x->at(VxIndex[ivx]);
      e2y[ivx] = vxnbc_cov_y->at(VxIndex[ivx]);
      e2z[ivx] = vxnbc_cov_z->at(VxIndex[ivx]);
      break;
    case VtxReconstructedSplit:
      x[ivx] = vxsplit_x->at(VxIndex[ivx]);
      y[ivx] = vxsplit_y->at(VxIndex[ivx]);
      z[ivx] = vxsplit_z->at(VxIndex[ivx]);
      e2x[ivx] = vxsplit_cov_x->at(VxIndex[ivx]);
      e2y[ivx] = vxsplit_cov_y->at(VxIndex[ivx]);
      e2z[ivx] = vxsplit_cov_z->at(VxIndex[ivx]);
      break;
    default:
      std::cerr << "WARNING: Wrong type of vertex in RealTrackTree::GetVtxDistance" << std::endl;
      x[ivx] = y[ivx] = z[ivx] = 0;
    }
  }

  double dx,dy,dz;
  double dxErr, dyErr, dzErr;
  dx = x[0] - x[1];
  dy = y[0] - y[1];
  dz = z[0] - z[1];
  dxErr = TMath::Sqrt(e2x[0] + e2x[1]);
  dyErr = TMath::Sqrt(e2y[0] + e2y[1]);
  dzErr = TMath::Sqrt(e2z[0] + e2z[1]);

  if (cfgMetric == VtxDistM_deltaZ) {
    // Simple delta z
    return dz;
  } 
  if (cfgMetric == VtxDistM_deltaZsig) {
    // Delta z / error
    return dz/dzErr;
  }
  if (cfgMetric == VtxDistM_3Dsig) {
    return TMath::Sqrt(dx*dx/dxErr/dxErr + dy*dy/dyErr/dyErr + dz*dz/dzErr/dzErr);
  }

  //nope
  cerr << "ERROR: Invalid cfgMetric in RealTrackTree::GetVtxDistance" << endl;
  return 0;
}

Bool_t RealTrackTree::isGoodVertex(Int_t VxIdx, RealTrackTree::VtxType vtxType)
{
  //  return true;

  // Set vertices to be "counted":
  //   GenVertices: at least 4 generated tracks with pT > 150 MeV and |eta| < 2.5, |q|=1
  //   RecoVertices: at least 4 reconstructed tracks with pT > 150 MeV and |eta| < 2.5
  //   SplitVertices: at least 2 reconstructed tracks with pT > 150 MeV and |eta| < 2.5
  bool goodVtx = false;
  if (vtxType == VtxSimulated) {
    return true; // HACK:always consider genEvents
    Int_t ngoodTracks = 0;
    for (int it=0; it < mc_n; it++) 
      if (mc_event_number->at(it) == mcVx_event_number->at(VxIdx))
	if (mc_gen_pt->at(it) >= 150. and TMath::Abs(mc_gen_eta->at(it)) < 2.5 and TMath::Abs(mc_charge->at(it)) == 1)
	  ngoodTracks++;
    if (ngoodTracks >= 4)
      goodVtx = true;
  } else if (vtxType == VtxReconstructedNoBC) {
    if (vxnbc_trk_n->at(VxIdx) >= 4) // already pT > 150 and |eta| < 2.5
      goodVtx = true;    
    //exclude dummy
    if (VxIdx == vxnbc_n-1)
      goodVtx = false;
  } else if (vtxType == VtxReconstructedBC) {
    if (vxbc_trk_n->at(VxIdx) >= 4) // already pT > 150 and |eta| < 2.5
      goodVtx = true;
    //exclude dummy
    if (VxIdx == vxbc_n-1)
      goodVtx = false;
  } else if (vtxType == VtxReconstructedSplit) {
    if (vxsplit_trk_n->at(VxIdx) >= 2) // already pT > 150 and |eta| < 2.5
      goodVtx = true;
  } 
  return goodVtx;

}

Bool_t RealTrackTree::isGoodRun(UInt_t run, UInt_t lb)
{
  static UInt_t prevRun=-1;
  bool isgood = false;
  switch (run) {
  case 152166:
    if (lb >= 206 && lb <= 300)
      isgood = true;
  case 167776:
    if (lb >= 124 && lb <= 546)
      isgood = true;
    break;   
  default:
    if (prevRun != run) {
      //print warning message only once for each run
      cout << "WARNING: Run " << run << " not defined in present GRL. Accepting all events." << endl;
      prevRun = run;
    }
    isgood = true;
  }
  return isgood;
}
