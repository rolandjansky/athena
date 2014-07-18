/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define RealTrackTree_cxx
#include "RealTrackTree.h"
#include <iostream>
#include <bitset>

#include <TAxis.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TText.h>
#include <TString.h>
#include <TLatex.h>

void RealTrackTree::fitFailed()
{
  std::cout << "Fit failed. Will exit without results. Most likely not enough events in input. I need several thousand and there are " << fChain->GetEntries() << std::endl;
  return;
}

void ATLAS_PRELIMINARY_LABEL(Double_t x,Double_t y,bool Preliminary=true,Color_t color=1) 
{
  TLatex l;
  
  l.SetNDC();
  
  l.SetTextFont(72);
  
  l.SetTextColor(color);
  
  double delx = 0.115*696*gPad->GetWh()/(472*gPad->GetWw());
  
  l.DrawLatex(x,y,"ATLAS");
  
  if (Preliminary) 
  {   
    TLatex p;
     
    p.SetNDC();
    
    p.SetTextFont(42);
    
    p.SetTextColor(color);
    
    p.DrawLatex(x+delx,y,"Preliminary");
    
    TLatex u;
    
    u.SetNDC();
    
    u.SetTextFont(42);

    u.SetTextColor(color);
    
    u.DrawLatex(x,y-0.065,"#sqrt{s} = 7 TeV");
  }
}//end of preliminary label method


RealTrackTree::RealTrackTree(TTree *tree, const TString& outfileName) : InDetTrackTree(tree), m_outfileName(outfileName) {}

//actual loop over the events
void RealTrackTree::Loop()
{

  double maxPT=10.;

  TFile* outputFile=new TFile(m_outfileName,"recreate");

   if (fChain == 0) return;

   Long64_t nentries(fChain->GetEntries());
  
// disable all branches
   fChain->SetBranchStatus("*",1);
   
// activate branches one by one
   fChain->SetBranchStatus("ei_EventNumber",1);
   fChain->SetBranchStatus("vxnbc_n",1); 
   fChain->SetBranchStatus("vxnbc_x",1);  
   fChain->SetBranchStatus("vxnbc_y",1);  
   fChain->SetBranchStatus("vxnbc_z",1);
   
   fChain->SetBranchStatus("vxnbc_cov_x",1);  
   fChain->SetBranchStatus("vxnbc_cov_y",1);   
   fChain->SetBranchStatus("vxnbc_cov_z",1);  
   
   fChain->SetBranchStatus("vxnbc_trk_index",1);
   fChain->SetBranchStatus("trk_pt",1);
   fChain->SetBranchStatus("vxnbc_trk_n",1); 
   
   fChain->SetBranchStatus("vxbc_n",1); 
   fChain->SetBranchStatus("vxbc_x",1);
   fChain->SetBranchStatus("vxbc_y",1);
   fChain->SetBranchStatus("vxbc_z",1);
   fChain->SetBranchStatus("vxbc_trk_n",1);  
   
//split vertices and stuff related to the reslutions
   fChain->SetBranchStatus("vxsplit_n",1);
   fChain->SetBranchStatus("vxsplit_x",1);
   fChain->SetBranchStatus("vxsplit_y",1);
   fChain->SetBranchStatus("vxsplit_z",1);
   fChain->SetBranchStatus("vxsplit_trk_n",1);
   fChain->SetBranchStatus("vxsplit_cov_x",1);
   fChain->SetBranchStatus("vxsplit_cov_y",1);
   fChain->SetBranchStatus("vxsplit_cov_z",1);
   fChain->SetBranchStatus("vxsplit_trk_n",1);
   fChain->SetBranchStatus("vxsplit_trk_index",1);
   
   
   fChain->SetBranchStatus("ei_bcid",1);
 //  fChain->SetBranchStatus("ctp_decisionItems",1);
   fChain->SetBranchStatus("ei_RunNumber",1);
    
//histograms for the reconstructed vertices
   TH1F* h_vx_n      = defineHistogram("Vrt_vx_n", 20, 0., 20., "Number of Primary Vertices", "Entries");
   TH1F* h_Vrt_X_rec = defineHistogram("Vrt_X_rec", 100, -1.5, 1.,  "Primary Vertex x [mm]", "Entries");
   TH1F* h_Vrt_Y_rec = defineHistogram("Vrt_Y_rec", 100, -0.5, 1.5,  "Primary Vertex y [mm]", "Entries");
   TH1F* h_Vrt_Z_rec = defineHistogram("Vrt_Z_rec", 100, -200., 200.,  "Primary Vertex z [mm]", "Entries");
 
   TH2F* h_Vrt_XY_rec = define2DHistogram("Vrt_XY_rec", 100, -2., 2., 100, -2., 4., "Primary Vertex x [mm]", "Primary Vertex y [mm]");
   TH2F* h_Vrt_ZX_rec = define2DHistogram("Vrt_ZX_rec", 100, -200., 200., 100, -3., 3., "Primary Vertex z [mm]", "Primary Vertex y [mm]");
   TH2F* h_Vrt_ZY_rec = define2DHistogram("Vrt_ZY_rec", 100, -200., 200., 100, -3., 3., "Primary Vertex z [mm]", "Primary Vertex y [mm]");
  
   TH1F* h_Vrt_ntracksPV  = defineHistogram("h_Vrt_ntracksPV", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   TH1F* h_Vrt_ntracksSV = defineHistogram("h_Vrt_ntracksSV", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   TH1F* h_Vrt_ntracksPU1 = defineHistogram("h_Vrt_ntracksPU1", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   
// delta z and related histograms
   TH1F* pu_DeltaZ  = defineHistogram("Pile up Delta Z 1-2 ", 200, -100., 100.,  "Pile up Delta Z 1-2", "Entries");
   TH1F* pu_DeltaZ_cut3  = defineHistogram("Pile up Delta Z 1-2 (cut3)", 200, -100., 100.,  "Pile up Delta Z 1-2 (cut3)", "Entries");
   TH1F* pu_DeltaZ_cut4  = defineHistogram("Pile up Delta Z 1-2 (cut4)", 200, -100., 100.,  "Pile up Delta Z 1-2 (cut4)", "Entries");
 
   TH2F* deltaZ_vs_2ndmult = define2DHistogram("deltaZ_vs_2ndmult", 30, 0., 30., 100, -100., 100., "N Tracks Vertex 2", "Delta Z vertices 1-2");

   TH2F* sum_pt2_vs_mult = define2DHistogram("sum_pt2_vs_mult", 70, 0., 70., 1000, 0., 100., "N Tracks Vertex", "sum_pt_2");
   TH2F* sum_pt2_vs_mult_all = define2DHistogram("sum_pt2_vs_mult_all", 70, 0., 70., 1000, 0., 100., "N Tracks Vertex", "sum_pt_2");
   
// vertex errors vs number of tracks and pt
   TH2F* h_Vrt_err_vs_ntrk = define2DHistogram("Vrt_err_vs_ntrk", 70, 0., 70., 50000, 0., 150.,"Number of Tracks", "x vertex Error [mm]");
   TH2F* h_Vrt_err_vs_pt2 = define2DHistogram("Vrt_err_vs_pt2",  100, 0., maxPT, 50000, 0., 150.,"#sqrt{#sum p_{T}^{2}} [GeV]", "x vertex Error [mm]");  
   
   TH2F* h_Vrt_err_z_vs_ntrk = define2DHistogram("Vrt_err_z_vs_ntrk", 70, 0., 70., 50000, 0., 150.,"Number of Tracks", "z vertex Error [mm]");
   TH2F* h_Vrt_err_z_vs_pt2 = define2DHistogram("Vrt_err_z_vs_pt2",  100, 0., maxPT, 50000, 0., 150.," #sqrt{#sum p_{T}^{2}} [GeV]", "z vertex Error [mm]");  
  
   TH2F* h_Vrt_err2_vs_ntrk = define2DHistogram("Vrt_err2_vs_ntrk", 70, 0., 70., 5000, 0., 5000.,"Number of Tracks", "Vertex Error2 , mm2");
   TH2F* h_Vrt_err2_vs_pt2 = define2DHistogram("Vrt_err2_vs_pt2",  100, 0., maxPT, 5000, 0., 5000.,"#sqrt{#sum p_{T}^{2}} [GeV]", "Vertex Error2, mm2"); 
   TH1F* h_vx_dz2    = defineHistogram("Vrt_vx_dz", 100, -100., 100., "Delta Z", "Entries");
   
   
//split vertices histograms
   TH1F* h_vx_n_split	= defineHistogram("Vrt_vx_n_split", 20, 0., 20., "Number of Primary Vertices", "Entries");
   TH1F* h_Vrt_X_pull_split = defineHistogram("Vrt_X_pull_split", 100, -5., 5.,  "Split x Pull", "Entries");
   TH1F* h_Vrt_Y_pull_split = defineHistogram("Vrt_Y_pull_split", 100, -5., 5.,  "Split y Pull", "Entries");
   TH1F* h_Vrt_Z_pull_split = defineHistogram("Vrt_Z_pull_split", 100, -5., 5.,  "Split z Pull", "Entries");
   
   TH1F* h_Vrt_ntracksPV_split  = defineHistogram("h_Vrt_ntracksPV_split", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   TH1F* h_Vrt_ntracksPV_av_split  = defineHistogram("h_Vrt_ntracksPV_av_split", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
  
   TH2F* h_Vrt_XpullVsNtrk_split = define2DHistogram("h_Vrt_XpullVsNtrk_split", 100, -5., 5., 70, 0., 70., "x Pull value", "N Tracks average");
   TH2F* h_Vrt_XpullVsPt2_split = define2DHistogram("h_Vrt_XpullVsPt2_split", 100, -5., 5., 100, 0., maxPT, "x Pull value", "Sqrt(#sum P_{T}^{2}) average [GeV]");
   
   TH2F* h_Vrt_ZpullVsNtrk_split = define2DHistogram("h_Vrt_ZpullVsNtrk_split", 100, -5., 5., 70, 0., 70., "z Pull value", "N Tracks average");
   TH2F* h_Vrt_ZpullVsPt2_split = define2DHistogram("h_Vrt_ZpullVsPt2_split", 100, -5., 5., 100, 0., maxPT, "z Pull value", "Sqrt(#sum P_{T}^{2}) [GeV]");
   
// beam constraint histograms
   TH1F* h_vx_n_bc      = defineHistogram("Vrt_vx_n_bc", 20, 0., 20., "Number of Primary Vertices", "Entries");
   TH1F* h_Vrt_X_rec_bc = defineHistogram("Vrt_X_rec_bc", 100, -1.5, 1.0,   "Primary Vertex x [mm]", "Vertices");
   TH1F* h_Vrt_Y_rec_bc = defineHistogram("Vrt_Y_rec_bc", 100, -0.5, 1.5,   "Primary Vertex y [mm]", "Vertices");
   TH1F* h_Vrt_Z_rec_bc = defineHistogram("Vrt_Z_rec_bc", 100, -200., 200., "Primary Vertex z [mm]", "Vertices");
  
   TH1F* h_vx_dz2_bc    = defineHistogram("Vrt_vx_dz_bc", 100, -100., 100., "Delta Z", "Entries");
  
  
   TH2F* h_Vrt_XY_rec_bc = define2DHistogram("Vrt_XY_rec_bc", 200, -1., 0.5, 200, 0., 1.5, "Primary Vertex x [mm]", "Primary Vertex y [mm]");
   TH2F* h_Vrt_ZX_rec_bc = define2DHistogram("Vrt_ZX_rec_bc", 200, -200., 200., 200, -1., 0.5, "Primary Vertex z [mm]", "Primary Vertex x [mm]");
   TH2F* h_Vrt_ZY_rec_bc = define2DHistogram("Vrt_ZY_rec_bc", 200, -200., 200., 200, 0., 1.5, "Primary Vertex z [mm]", "Primary Vertex y [mm]");
 
   TH1F* h_Vrt_ntracksPV_bc  = defineHistogram("h_Vrt_ntracksPV_bc", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   TH1F* h_Vrt_ntracksSV_bc = defineHistogram("h_Vrt_ntracksSV_bc", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   TH1F* h_Vrt_ntracksPU1_bc = defineHistogram("h_Vrt_ntracksPU1_bc", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   
   TH2F* h_Vrt_nTracksPVvsPU1 = define2DHistogram("h_Vrt_nTracksPVvsPU1", 50, 0., 50., 30, 0., 50., "N Tracks Vertex 1", "N Tracks Vertex 2");
  
   TH1F* h_Vrt_deltaZ_bc = defineHistogram("Delta_Z_bc", 100, 0., 300.,  "Primary Vertex Delta z [mm]", "Entries");
   TH1F* h_Vrt_deltaZ_bc_cut = defineHistogram("Delta_Z_bc_cut", 100, 0., 300.,  "Primary Vertex Delta z (cut) [mm]", "Entries");
   
   
   TH2F* h_Vrt_deltaZ_bc_vs_nvx = define2DHistogram("Delta z vs Nvx", 10, 0., 10., 100, 0., 300., "Number of Vertices", "Nearest delta z");
   TH2F* h_Vrt_deltaZ_bc_vs_nvx_cut = define2DHistogram("Delta z vs Nvx (cut) [mm]", 10, 0., 10., 100, 0., 300., "Number of Vertices", "Nearest delta z");
  
   TH2F* h_Vrt_ntracksPV_bc_vs_nvx = define2DHistogram("h_Vrt_nTracksPVvsNvx", 15, 0., 15., 200, 0., 200., "Number of the Vertex", "N Tracks at Vertex");
   
   TH1F* h_Vrt_Ntrk_bc_one = defineHistogram("Ntrk single collision", 100, 0., 100.,  "Number of tracks at single vertex", "Entries");
  
   TH1F* h_Vrt_Ntrk_bc_pv1 = defineHistogram("Ntrk two collisions 1", 100, 0., 100.,  "Number of tracks at PV1 vertex", "Entries");
   TH1F* h_Vrt_Ntrk_bc_pv2 = defineHistogram("Ntrk two collisions 2", 100, 0., 100.,  "Number of tracks at PV2 vertex", "Entries");
  
   TH1F* h_Vrt_Ntrk_bc_pv1_3 = defineHistogram("Ntrk three collisions 1", 100, 0., 100.,  "Number of tracks at PV1 vertex", "Entries");
   TH1F* h_Vrt_Ntrk_bc_pv2_3 = defineHistogram("Ntrk three collisions 2", 100, 0., 100.,  "Number of tracks at PV2 vertex", "Entries");
   TH1F* h_Vrt_Ntrk_bc_pv3_3 = defineHistogram("Ntrk three collisions 3", 100, 0., 100.,  "Number of tracks at PV3 vertex", "Entries");
  
//first estimating the actual number of merges and splits  we'll do a separate event loop for this, but first let's book  histograms
//to be tranferred to the authomatic booking later..
   unsigned int nbins_study = 20;
   
//number of requested REAL split pairs
   
   TH2F* h_nSplit_vs_nBC = define2DHistogram("N pairs vs Nvx", nbins_study, 0., float(nbins_study), nbins_study, 0., float(nbins_study), " Number of reco Vertices", "Number of Split Real Pairs");
   
   std::cout<<"Working with a d3pd with entries: "<<nentries<<std::endl;
    
   for (Long64_t jentry=0; jentry<nentries;jentry++) 
   {
     Long64_t ientry = this->LoadTree(jentry); 
     if (ientry < 0) break;
     fChain->GetEntry(jentry);
     
     bool nvtx_decision = true;


//looking at the split vertices in the bins of those reconstructed
//with the beam constraint (to be transfered in the loop)
     for(int i = 1;i<=nbins_study;++i)if(split_bin_test(i))
     {
     
     // h_nSplit_vs_nBC->Fill(vxbc_n-1, i);//end of loop over the bins of number of reco vertices
     
//correcting for oly accepting the vertices having at least 4 tracks fitted
      unsigned int  nvx_reco  = 0;     
      for(int j = 0; j<vxbc_n-1; ++j) if(vxbc_trk_n->at(j)>3) ++nvx_reco;
      h_nSplit_vs_nBC->Fill(nvx_reco,i);
       
     
     } 
      
//selecting the split vertices stream. Only events where there are 2 or more split vertices
//of which the first two are real and no matter how many of the rest are dummy..    

    if(vxsplit_n<2 || vxsplit_type->at(0)!=1 || vxsplit_type->at(1)!=1  ) continue;
    

//checking  trigger and good run list 
     if (trigger_L1_MBTS_1() && good_run() )
     {

//split vertices block 
      if(vxsplit_n > 1)
      {
       h_vx_n_split->Fill(vxsplit_n); 
       
//loop over all the vertices in the split container  
//       int half_size  = vxsplit_n/2;
       int half_size  = 1;
                  
       for(int spvx = 0; spvx < half_size; ++spvx)
       {
        
//taking the 2xith  and 2xith +1 vertices	
	float x_distance = vxsplit_x->at(2*spvx) - vxsplit_x->at(2*spvx+1);
	float y_distance = vxsplit_y->at(2*spvx) - vxsplit_y->at(2*spvx+1);
	float z_distance = vxsplit_z->at(2*spvx) - vxsplit_z->at(2*spvx+1);

        int ntrk_odd  =  vxsplit_trk_n->at(2*spvx+1);
	int ntrk_even =  vxsplit_trk_n->at(2*spvx);
	int ntrk_av = (ntrk_odd + ntrk_even)/2;
	
	h_Vrt_ntracksPV_split->Fill(ntrk_odd);
	h_Vrt_ntracksPV_split->Fill(ntrk_even);
	h_Vrt_ntracksPV_av_split->Fill(ntrk_av);

//calculation of the vertex errors
       float x_error = TMath::Sqrt(vxsplit_cov_x->at(2*spvx) + vxsplit_cov_x->at(2*spvx+1));
       float y_error = TMath::Sqrt(vxsplit_cov_y->at(2*spvx) + vxsplit_cov_y->at(2*spvx+1));
       float z_error = TMath::Sqrt(vxsplit_cov_z->at(2*spvx) + vxsplit_cov_z->at(2*spvx+1));
     
        float x_split_pull = x_distance/x_error;
 	float y_split_pull = y_distance/y_error;
 	float z_split_pull = z_distance/z_error;
     
	h_Vrt_X_pull_split->Fill(x_split_pull);
	h_Vrt_Y_pull_split->Fill(y_split_pull);
	h_Vrt_Z_pull_split->Fill(z_split_pull);
	
	h_Vrt_XpullVsNtrk_split->Fill(x_split_pull, ntrk_av);
	h_Vrt_ZpullVsNtrk_split->Fill(z_split_pull, ntrk_av);
	
//calculation of the  average sum of pt2
       float sum_pt2_1 = 0.;
       float sum_pt2_2 = 0.;
       for(int trk = 0; trk < vxsplit_trk_n->at(2*spvx); ++trk) 
           sum_pt2_1 += TMath::Power(trk_pt->at(vxsplit_trk_index->at(2*spvx)[trk])/1e3,2); //end of loop over all tracks at vertex	   
       
       for(int trk = 0; trk < vxsplit_trk_n->at(2*spvx+1); ++trk) 
           sum_pt2_2 += TMath::Power(trk_pt->at(vxsplit_trk_index->at(2*spvx+1)[trk] )/1e3,2); //end of loop over all tracks at vertex
     	
       h_Vrt_XpullVsPt2_split->Fill(x_split_pull, TMath::Sqrt((sum_pt2_1 + sum_pt2_2)/2.));
       
       h_Vrt_ZpullVsPt2_split->Fill(z_split_pull, TMath::Sqrt((sum_pt2_1 + sum_pt2_2)/2.));  
      }//end of loop over all split vertx pairs
     }//end of check that there are split vertices for this event
 
// beam- constrained vertcies---------------------------------------------------------------------      
//vertex with bc block
      h_vx_n_bc->Fill(vxbc_n-1);
     
//2nd bin of the nvx distribution     
      if((vxbc_n-1)==2) h_vx_dz2_bc->Fill(vxbc_z->at(0) - vxbc_z->at(1) );
     
      if (vxbc_n > 1)
      { 
       if(vxbc_trk_n->at(0)>3)
       { 
        h_Vrt_X_rec_bc->Fill(vxbc_x->at(0));
        h_Vrt_Y_rec_bc->Fill(vxbc_y->at(0));
        h_Vrt_Z_rec_bc->Fill(vxbc_z->at(0));
 
       
        h_Vrt_XY_rec_bc->Fill(vxbc_x->at(0), vxbc_y->at(0));
        h_Vrt_ZX_rec_bc->Fill(vxbc_z->at(0), vxbc_x->at(0));
        h_Vrt_ZY_rec_bc->Fill(vxbc_z->at(0), vxbc_y->at(0));
       }
  
       h_Vrt_ntracksPV_bc->Fill(vxbc_trk_n->at(0));

//       h_Vrt_ntracksPV_bc_vs_nvx->Fill(vxbc_trk_n->at(0), vxbc_n-1);
       

// filling the number of tracks for exactly one collision
       if(vxbc_n == 2) 
       { 
        h_Vrt_Ntrk_bc_one->Fill(vxbc_trk_n->at(0));
       }else if(vxbc_n == 3 && vxbc_trk_n->at(1)<3)
       {
       
//calculating the distance between those two vertices
        float dist = TMath::Abs(vxbc_x->at(0) - vxbc_x->at(1));

//closer than 3mm - let us consider this as a pile up
        if(dist<5.) h_Vrt_Ntrk_bc_one->Fill(vxbc_trk_n->at(0) + vxbc_trk_n->at(1));//this is a split?
        else h_Vrt_Ntrk_bc_one->Fill(vxbc_trk_n->at(0)); //this is a fake vertex or so?
       
       }
       
       
//filling the cases when we have 1 and 2 pile up events in addition to 
//the main collision       
       if(vxbc_n == 3 && vxbc_trk_n->at(1)>2)
       {
         h_Vrt_Ntrk_bc_pv1->Fill(vxbc_trk_n->at(0));
         h_Vrt_Ntrk_bc_pv2->Fill(vxbc_trk_n->at(1));
  
  
  
       }else if(vxbc_n == 4)
       {
  
//case these are actually two which look like 3
        unsigned int n_low = 0; 
        unsigned int  loc_ntrk[3];
	for(unsigned int i = 0;i<3;++i) loc_ntrk[i] = vxbc_trk_n->at(i);
	
//sorting	
       const int n = sizeof(loc_ntrk) / sizeof(int);
       std::sort(loc_ntrk, loc_ntrk+n); 
    
  
//case these are 3 interactions  
        h_Vrt_Ntrk_bc_pv1_3->Fill(loc_ntrk[2]);
        h_Vrt_Ntrk_bc_pv2_3->Fill(loc_ntrk[1]);
        h_Vrt_Ntrk_bc_pv3_3->Fill(loc_ntrk[0]);
       }
  
//filling the dependence for the cases of > 1 collision
       if(vxbc_n > 2)
       {       
        int ntrk_temp[vxbc_n-1];
        for(int i=0; i<vxbc_n-1;++i) ntrk_temp[i] = vxbc_trk_n->at(i);
        const int n = sizeof(ntrk_temp) / sizeof(int);
        std::sort(ntrk_temp, ntrk_temp+n);
	
//now the numbers  of tracks are filled in the acsending order
//writing them into the histogram in the reverse order..	
        for(int i =  0; i <vxbc_n-1 ;++i) h_Vrt_ntracksPV_bc_vs_nvx->Fill(i, ntrk_temp[vxbc_n-2 - i]);
       }

// loop over the vertices in the container excluding the dummy
       if(vxbc_n > 2)// there is signal, dummy and something else
       {
           
//z cord of all vertices       
        float z_coord[vxbc_n-1];
	 
        for(int vtx_it = 0; vtx_it<vxbc_n-1 ; ++vtx_it) z_coord[vtx_it]  = vxbc_z->at(vtx_it) ;      //end of loop over all pileup vertices
        const int n = sizeof(z_coord) / sizeof(float);
        std::sort(z_coord, z_coord+n);	
	
//getting the inter-vertex distances if any
//and filling them into the corresponding histogram
        for(int vtx_it =0; vtx_it< vxbc_n-2;++vtx_it) 
	{
	  h_Vrt_deltaZ_bc->Fill(z_coord[vtx_it+1] - z_coord[vtx_it]);//end of loop over all the vertices	
          h_Vrt_deltaZ_bc_vs_nvx->Fill(vxbc_n-1,z_coord[vtx_it+1] - z_coord[vtx_it]);	
	}//end of loop over all the vertices reconstructed in the event
       
// selected z coordinate of vertices (vertices after the track selection cut )
// not the most elegant, but working solution.
//counting the vertices after the pile up cut
        std::vector<float> cut_vrt_z;
	
        cut_vrt_z.push_back(vxbc_z->at(0));
        for(int vtx_it = 1; vtx_it<vxbc_n-1 ; ++vtx_it)	
	{
	 if(vxbc_trk_n->at(vtx_it)>3) cut_vrt_z.push_back(vxbc_z->at(vtx_it));      
	}//end of loop over pile up vertices
	 
        if(cut_vrt_z.size()>1)
	{

//now sorting the z coordinate of verices to identify siblings	
         int n_after_cut  = cut_vrt_z.size();
	 float z_coord_cut[n_after_cut];
	 for(int i = 0; i<n_after_cut;++i)z_coord_cut[i] = cut_vrt_z[i];
	 const int n = sizeof(z_coord) / sizeof(float);
	 std::sort(z_coord_cut, z_coord_cut+n);	
	
	 for(int vtx_it =0; vtx_it<n_after_cut-1; ++vtx_it) 
	 {
	  h_Vrt_deltaZ_bc_cut->Fill(z_coord_cut[vtx_it+1] - z_coord_cut[vtx_it]);//end of loop over all the vertices	
          h_Vrt_deltaZ_bc_vs_nvx_cut->Fill(n_after_cut,z_coord_cut[vtx_it+1] - z_coord_cut[vtx_it]);
	 } //end of loop over all the vertices reconstructed in the event
	}//end of check whether the pile up vertices after the cut are there.
       }//end of selection of events with several pile up vertices
      }//end of vertex with bc block   

//-------------------------------------------------------------------------------------
// vertex block without the beam constraint
      h_vx_n->Fill(vxnbc_n-1);

//2nd bin of the nvx distribution     
      if((vxnbc_n-1)==2) h_vx_dz2->Fill(vxnbc_z->at(0) - vxnbc_z->at(1) );

      if (vxnbc_n > 1)
      {
       h_Vrt_X_rec->Fill(vxnbc_x->at(0));
       h_Vrt_Y_rec->Fill(vxnbc_y->at(0));
       h_Vrt_Z_rec->Fill(vxnbc_z->at(0));
	  
       h_Vrt_XY_rec->Fill(vxnbc_x->at(0), vxnbc_y->at(0));
       h_Vrt_ZX_rec->Fill(vxnbc_z->at(0), vxnbc_x->at(0));
       h_Vrt_ZY_rec->Fill(vxnbc_z->at(0), vxnbc_y->at(0));

       h_Vrt_ntracksPV->Fill(vxnbc_trk_n->at(0));
	   
// reconstructed vertex error as a function of Ntrk and pt2	   
       for(int vx_it =0;  vx_it !=vxnbc_n-1; ++vx_it)
       {
        float ntrk = (float)vxnbc_trk_n->at(vx_it);
	float sum_pt2 = 0.;
        for(int trk = 0; trk < vxnbc_trk_n->at(vx_it); ++trk) sum_pt2 += TMath::Power(trk_pt->at(vxnbc_trk_index->at(vx_it)[trk] )/1e3,2); 	   

        h_Vrt_err_vs_ntrk->Fill(ntrk,TMath::Sqrt(vxnbc_cov_x->at(vx_it)));
        h_Vrt_err_vs_pt2->Fill(TMath::Sqrt(sum_pt2),TMath::Sqrt(vxnbc_cov_x->at(vx_it))); 
       
        h_Vrt_err_z_vs_ntrk->Fill(ntrk,TMath::Sqrt(vxnbc_cov_z->at(vx_it)));
        h_Vrt_err_z_vs_pt2->Fill(TMath::Sqrt(sum_pt2),TMath::Sqrt(vxnbc_cov_z->at(vx_it))); 
       
       
//histograms for errors squared
        h_Vrt_err2_vs_ntrk->Fill(ntrk,vxnbc_cov_x->at(vx_it));
        h_Vrt_err2_vs_pt2->Fill(TMath::Sqrt(sum_pt2),vxnbc_cov_x->at(vx_it)); 
       }//end of loop over all the vertices	   
	
	
		   
//calculating sum of pt2 for the signal vertex
       float sum_pt2 = 0.;
       for(int trk = 0; trk < vxnbc_trk_n->at(0); ++trk) sum_pt2 += TMath::Power(trk_pt->at(vxnbc_trk_index->at(0)[trk] )/1e3,2); //end of loop over all tracks at vertex	   

// a single reconstructed vertex 	
       if(vxnbc_n == 2)
       {
        h_Vrt_ntracksSV->Fill(vxnbc_trk_n->at(0)); 	
        sum_pt2_vs_mult->Fill( (float)vxnbc_trk_n->at(0), sum_pt2);
	
	
       }
       sum_pt2_vs_mult_all->Fill(  (float)vxnbc_trk_n->at(0), sum_pt2);
       
//sum pt and sum pt^2	
	
//events with  2 and  more vertices	
	if (vxnbc_n == 3)
	{
	 h_Vrt_ntracksPU1->Fill(vxnbc_trk_n->at(1));
	 h_Vrt_nTracksPVvsPU1->Fill(vxnbc_trk_n->at(0), vxnbc_trk_n->at(1));
	 
//delta Z between vertices 
 	 double delta_z = vxnbc_z->at(0) - vxnbc_z->at(1);
	 pu_DeltaZ->Fill(delta_z);
	 deltaZ_vs_2ndmult->Fill(vxnbc_trk_n->at(1),delta_z);
	  
//cut on the 2nd reconstructed vertex	 
	 if(vxnbc_trk_n->at(1)>3)
	 {
	  pu_DeltaZ_cut3->Fill(delta_z);
	  if (vxnbc_trk_n->at(1)>4) pu_DeltaZ_cut4->Fill(delta_z);
	 }//end of cut on th 2nd reconstructed verte
	}//end of pile up check
       }//end of vertex presence check
      }//end of trigger selection
   }//end of loop over all the entries

//creating various graphs for the split vertices-----------------------------------------------------
//analysing the 2d histogram which was created during the 
//split vertex scan. First getting number of bins       
       
   int ntrk_z_bin = h_Vrt_ZpullVsNtrk_split->GetNbinsY();
   std::vector<float> rms_z;
   std::vector<float> rms_z_er;
   std::vector<float> sigma_z;
   std::vector<float> sigma_z_er;
   
   std::vector<float> bins_z_nt;
   std::vector<float> bins_z_nt_er;
    
   outputFile->cd();

   TString profileZ_bin("profileZ_Ntrk_bin");

   for(int bin_count= 0; bin_count <ntrk_z_bin;++bin_count)
   {
   
    TString use=profileZ_bin;
    use += bin_count;

    bins_z_nt.push_back((float)bin_count);
    bins_z_nt_er.push_back(1.);    
    TH1D * profileZ = h_Vrt_ZpullVsNtrk_split->ProjectionX("_pz",bin_count,bin_count);
 
    profileZ->SetName(use);
    profileZ->Write();

    rms_z.push_back(profileZ->GetRMS());
    rms_z_er.push_back(profileZ->GetRMSError());   
    
//making a gaussian fit if there is anough entries 
    if(profileZ->GetEntries() > 150.)
    {  
     std::vector<float> fit_res = stableGaussianFit(profileZ);     
     sigma_z.push_back(fit_res[0]);
     sigma_z_er.push_back(fit_res[1]);	
     
    }else{
    
     sigma_z.push_back(0.);
     sigma_z_er.push_back(0.);
    }//end of good number of bins selection
 
    delete profileZ;        
   }//end of loop over all the ntrk bins   
    
   TGraphErrors * krms_z_vs_ntrk = new TGraphErrors(ntrk_z_bin, &(bins_z_nt[0]),&(rms_z[0]),&(bins_z_nt_er[0]), &(rms_z_er[0]) );
   krms_z_vs_ntrk->GetYaxis()->SetTitle("z scale factor");
   krms_z_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks");
   krms_z_vs_ntrk->SetName("scaleFactorZ_RMS");
   krms_z_vs_ntrk->Write();

   TGraphErrors * kgs_z_vs_ntrk = new TGraphErrors(ntrk_z_bin, &(bins_z_nt[0]),&(sigma_z[0]),&(bins_z_nt_er[0]), &(sigma_z_er[0]) );
   kgs_z_vs_ntrk->GetYaxis()->SetTitle("z scale factor");
   kgs_z_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks"); 
   kgs_z_vs_ntrk->SetName("scaleFactorZ_Fit");
   kgs_z_vs_ntrk->Write();
    
// approximating the graph with 2nd order polinomial. 
   Int_t fitStatus = kgs_z_vs_ntrk->Fit("pol2","Q0","",2.,50.);

   
   if (fitStatus != 0) { fitFailed(); return; }
   kgs_z_vs_ntrk->GetFunction("pol2")->SetLineColor(kRed);
   TF1 * kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol2");

//getting the fitted parameter errors    
   Double_t * kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors(); 
     
//playing with the pt2 dependence
   int pt2_z_bin    =  h_Vrt_ZpullVsPt2_split->GetNbinsY();
   
//getting the bin size in Y  
   float bin_z_size =  h_Vrt_ZpullVsPt2_split->GetYaxis()->GetBinWidth(2);
   
   std::vector<float> pt_z_rms;
   std::vector<float> pt_z_rms_er;
   std::vector<float> pt_z_sigma;
   std::vector<float> pt_z_sigma_er;
   
   std::vector<float> pt_z_bins_nt;
   std::vector<float> pt_z_bins_nt_er;
 
   TString profileZ2_bin("profileZ_vsPt_bin");

   for(int bin_count= 1; bin_count <= pt2_z_bin;++bin_count)
   {
   
     TString use=profileZ2_bin;
     use+=bin_count;

    pt_z_bins_nt.push_back((float)bin_count*bin_z_size);
//    pt_z_bins_nt_er.push_back(1.);
  
    pt_z_bins_nt_er.push_back(bin_z_size);
   
    TH1D * profileX = h_Vrt_ZpullVsPt2_split->ProjectionX("_px",bin_count,bin_count);
    profileX->SetName(use);
    profileX->Write();
    
    pt_z_rms.push_back(profileX->GetRMS());
    pt_z_rms_er.push_back(profileX->GetRMSError());
	
//making a gaussina fit if there is enough entries 
    if(profileX->GetEntries() > 150.)
    {
     std::vector<float> fit_res = stableGaussianFit(profileX);     
     pt_z_sigma.push_back(fit_res[0]);
     pt_z_sigma_er.push_back(fit_res[1]);    
    }else{
    
     pt_z_sigma.push_back(0.);
     pt_z_sigma_er.push_back(0.);
    
    }//end of checking enough entries
    
    
    delete profileX; 
   }//end of loop over all the pt2 bins
   
   TGraphErrors * krms_z_vs_pt= new TGraphErrors(pt2_z_bin, &(pt_z_bins_nt[0]),&(pt_z_rms[0]),&(pt_z_bins_nt_er[0]), &(pt_z_rms_er[0]) );
   krms_z_vs_pt->GetYaxis()->SetTitle("z scale factor");
   krms_z_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} (average) [GeV]");
   krms_z_vs_pt->SetName("scaleFactorZvsPt_RMS");
   krms_z_vs_pt->Write();

   TGraphErrors * kgs_z_vs_pt = new TGraphErrors(pt2_z_bin, &(pt_z_bins_nt[0]),&(pt_z_sigma[0]),&(pt_z_bins_nt_er[0]), &(pt_z_sigma_er[0]) );
   kgs_z_vs_pt->GetYaxis()->SetTitle("z scale factor");
   kgs_z_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} (average) [GeV] ");
   kgs_z_vs_pt->SetName("scaleFactorZvsPt_Fit");
   kgs_z_vs_pt->Write();
      
// approximating the graph with 2nd order polinomial.
   fitStatus = kgs_z_vs_pt->Fit("pol2","Q0","",0.2,8.);
   if (fitStatus != 0) { fitFailed(); return; }
   kgs_z_vs_pt->GetFunction("pol2")->SetLineColor(kRed); 
   TF1 * kgs_z_pt_fit = kgs_z_vs_pt->GetFunction("pol2");

//getting the fitted parameter errors    
   Double_t * kgs_z_pt_fit_er = kgs_z_pt_fit->GetParErrors();
    
      
//-------------------------------------------------------------------------------------------------------------------------------------   
//X -pull dependence..-----------------------------------------------------------------------------------------------------------------       
   int ntrk_bin = h_Vrt_XpullVsNtrk_split->GetNbinsY();
   std::vector<float> rms;
   std::vector<float> rms_er;
   std::vector<float> sigma;
   std::vector<float> sigma_er;
   
   std::vector<float> bins_nt;
   std::vector<float> bins_nt_er;

   TString profileX_bin("profileX_Ntrk_bin");

   for(int bin_count= 0; bin_count <ntrk_bin;++bin_count)
   {
   
    TString use=profileX_bin;
    use+=bin_count;

    bins_nt.push_back((float)bin_count);
    bins_nt_er.push_back(1.);
   
    TH1D * profileX = h_Vrt_XpullVsNtrk_split->ProjectionX("_px",bin_count,bin_count);
 
    profileX->SetName(use);
    profileX->Write();

    rms.push_back(profileX->GetRMS());
    rms_er.push_back(profileX->GetRMSError());
	
//making a gaussian fit if there is anough entries 
    if(profileX->GetEntries() > 150.)
    {
     
//making a stable gaussian fit     
     std::vector<float> fit_res = stableGaussianFit(profileX);     
     sigma.push_back(fit_res[0]);
     sigma_er.push_back(fit_res[1]);

    }else{
     sigma.push_back(0.);
     sigma_er.push_back(0.);
    }//end of good number of bins selection
 
    delete profileX;
   }//end of loop over all the bins in ntrk

   TGraphErrors * krms_vs_ntrk = new TGraphErrors(ntrk_bin, &(bins_nt[0]),&(rms[0]),&(bins_nt_er[0]), &(rms_er[0]) );
   krms_vs_ntrk->GetYaxis()->SetTitle("x scale factor");
   krms_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks");
   krms_vs_ntrk->SetName("scaleFactorX_RMS");
   krms_vs_ntrk->Write();
   
   TGraphErrors * kgs_vs_ntrk = new TGraphErrors(ntrk_bin, &(bins_nt[0]),&(sigma[0]),&(bins_nt_er[0]), &(sigma_er[0]) );
   kgs_vs_ntrk->GetYaxis()->SetTitle("x scale factor");
   kgs_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks");
   kgs_vs_ntrk->SetName("scaleFactorX_Fit");
   kgs_vs_ntrk->Write();


//approximating the dependence with the 2nd order polinomial
   fitStatus = kgs_vs_ntrk->Fit("pol2","Q0","",2.,50.);
   if (fitStatus != 0) { fitFailed(); return; };
   kgs_vs_ntrk->GetFunction("pol2")->SetLineColor(kRed); 
   TF1 * kgs_ntrk_fit = kgs_vs_ntrk->GetFunction("pol2");

//getting the fitted parameter errors    
   Double_t * kgs_ntrk_fit_er = kgs_ntrk_fit->GetParErrors();

//playing with the pt2 dependence
   int pt2_bin    =  h_Vrt_XpullVsPt2_split->GetNbinsY();
   
//getting the bin size in Y  
   
   float bin_size =  h_Vrt_XpullVsPt2_split->GetYaxis()->GetBinWidth(2);
   
   std::vector<float> pt_rms;
   std::vector<float> pt_rms_er;
   std::vector<float> pt_sigma;
   std::vector<float> pt_sigma_er;
   
   std::vector<float> pt_bins_nt;
   std::vector<float> pt_bins_nt_er;

   TString profileX2_bin("profileX_vsPt_bin");

   for(int bin_count= 1; bin_count <=pt2_bin;++bin_count)
   {
   
    TString use=profileX2_bin;
    use+=bin_count;

    pt_bins_nt.push_back((float)bin_count*bin_size);
//    pt_bins_nt_er.push_back(1.);
  
    pt_bins_nt_er.push_back(bin_size);
    
    TH1D * profileX = h_Vrt_XpullVsPt2_split->ProjectionX("_px",bin_count,bin_count);
    profileX->SetName(use);
    profileX->Write();
    
    pt_rms.push_back(profileX->GetRMS());
    pt_rms_er.push_back(profileX->GetRMSError());
	
//making a gaussina fit if there is enough entries 
    if(profileX->GetEntries() > 150.)
    { 
     std::vector<float> fit_res = stableGaussianFit(profileX);     
     pt_sigma.push_back(fit_res[0]);
     pt_sigma_er.push_back(fit_res[1]);    
    
    }else{
    
     pt_sigma.push_back(0.);
     pt_sigma_er.push_back(0.);
    }//end of checking enough entries
    
    delete profileX; 
   }//end of loop over all the pt2 bins
   
   TGraphErrors * krms_vs_pt= new TGraphErrors(pt2_bin, &(pt_bins_nt[0]),&(pt_rms[0]),&(pt_bins_nt_er[0]), &(pt_rms_er[0]) );
   krms_vs_pt->GetYaxis()->SetTitle("x scale factor");
   krms_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} (average) [GeV]");
   krms_vs_pt->SetName("scaleFactorXvsPt_RMS");
   krms_vs_pt->Write();

   
   TGraphErrors * kgs_vs_pt = new TGraphErrors(pt2_bin, &(pt_bins_nt[0]),&(pt_sigma[0]),&(pt_bins_nt_er[0]), &(pt_sigma_er[0]) );
   kgs_vs_pt->GetYaxis()->SetTitle("x scale factor");
   kgs_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} (average) [GeV]");
   kgs_vs_pt->SetName("scaleFactorXvsPt_Fit");
   kgs_vs_pt->Write();
        
   fitStatus = kgs_vs_pt->Fit("pol2","Q0","",0.2,8.);
   if (fitStatus != 0) { fitFailed(); return; }

   kgs_vs_pt->GetFunction("pol2")->SetLineColor(kRed); 
   TF1 * kgs_fit = kgs_vs_pt->GetFunction("pol2");
   
//getting the fitted parameter errors    
   Double_t * kgs_fit_er = kgs_fit->GetParErrors();
        
// playing with the averaged errors
   int nbins_z_err_ntrk = h_Vrt_err_z_vs_ntrk->GetNbinsX();  
   
   h_Vrt_err_z_vs_ntrk->SetName("error_z_vs_ntrk");
   h_Vrt_err_z_vs_ntrk->Write();
   
   std::vector<float> av_err_z;
   std::vector<float> av_err_z_er;
   
   std::vector<float> err_bins_z_nt;
   std::vector<float> err_bins_z_nt_er;
   
   std::vector<float> res_z;
   std::vector<float> res_z_er;
    
   for(int bin_count= 1; bin_count <=nbins_z_err_ntrk; ++bin_count)
   {
   
    err_bins_z_nt.push_back((float)bin_count);
    err_bins_z_nt_er.push_back(1.);
   
    TH1D * profileY = h_Vrt_err_z_vs_ntrk->ProjectionY("_py",bin_count,bin_count);
    
    float mean = profileY->GetMean();
    float mean_error = profileY->GetMeanError();
     
    av_err_z.push_back(mean);
    av_err_z_er.push_back(mean_error);
   
   
//estimating the approximate k-factor and the error value   
    double pr_er  =  error_func( bin_count, kgs_z_ntrk_fit_er); 

    res_z.push_back(mean * kgs_z_ntrk_fit->Eval(bin_count));     
    res_z_er.push_back(TMath::Sqrt(TMath::Power(mean_error * kgs_z_ntrk_fit->Eval(bin_count),2) + TMath::Power( pr_er * mean ,2)));      
   }
   
   TGraphErrors * res_z_vs_ntrk = new TGraphErrors(nbins_z_err_ntrk, &(err_bins_z_nt[0]),&(res_z[0]),&(err_bins_z_nt_er[0]), &(res_z_er[0]) );
   res_z_vs_ntrk->GetYaxis()->SetTitle("z Vertex Resolution [mm]");
   res_z_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks");
   
//resolution versus pt2
   int nbins_z_err_pt = h_Vrt_err_z_vs_pt2->GetNbinsX(); 
   float bin_size_z_pt =  h_Vrt_err_z_vs_pt2->GetXaxis()->GetBinWidth(2);
    
   h_Vrt_err_z_vs_pt2->SetName("error_z_vs_pt2");
   h_Vrt_err_z_vs_pt2->Write();

   std::vector<float> av_z_err_pt;
   std::vector<float> av_z_err_pt_er;
   
   std::vector<float> err_z_bins_pt;
   std::vector<float> err_z_bins_pt_er;
   
   std::vector<float> res_z_pt;
   std::vector<float> res_z_pt_er;
  
  
   for(int bin_count= 1; bin_count <=nbins_z_err_pt; ++bin_count)
   {
    err_z_bins_pt.push_back((float)bin_count*bin_size_z_pt);
    err_z_bins_pt_er.push_back(bin_size_z_pt);
    
    TH1D * profileY = h_Vrt_err_z_vs_pt2->ProjectionY("_py",bin_count,bin_count);
    
    float mean = profileY->GetMean();
    float mean_error = profileY->GetMeanError();
     
    av_z_err_pt.push_back(mean);
    av_z_err_pt_er.push_back(mean_error);
   
    res_z_pt.push_back(mean *  kgs_z_pt_fit->Eval(pt_bins_nt[bin_count]));
    double pr_er = error_func( pt_bins_nt[bin_count]  , kgs_z_pt_fit_er); 
    res_z_pt_er.push_back(TMath::Sqrt(TMath::Power(mean_error * kgs_z_pt_fit->Eval(pt_bins_nt[bin_count]),2) + TMath::Power( pr_er * mean ,2))); 
   } 
   
   TGraphErrors * mean_er_z_vs_pt= new TGraphErrors(nbins_z_err_pt, &(err_z_bins_pt[0]),&(av_z_err_pt[0]),&(err_z_bins_pt_er[0]), &(av_z_err_pt_er[0]) );
   mean_er_z_vs_pt->GetYaxis()->SetTitle("Average Vertex Error ");
   mean_er_z_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} [GeV]");
   
   
   TGraphErrors * res_z_vs_pt = new TGraphErrors(nbins_z_err_pt, &(err_z_bins_pt[0]),&(res_z_pt[0]),&(err_z_bins_pt_er[0]), &(res_z_pt_er[0]) );
   res_z_vs_pt->GetYaxis()->SetTitle("z Vertex Resolution [mm]");
   res_z_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} [GeV]");
   
      
   int nbins_err_ntrk = h_Vrt_err_vs_ntrk->GetNbinsX();  
   
   h_Vrt_err_vs_ntrk->SetName("error_vs_ntrk");
   h_Vrt_err_vs_ntrk->Write();

   std::vector<float> av_err;
   std::vector<float> av_err_er;
   
   std::vector<float> err_bins_nt;
   std::vector<float> err_bins_nt_er;
   
   std::vector<float> res;
   std::vector<float> res_er;
   
   for(int bin_count= 1; bin_count <=nbins_err_ntrk; ++bin_count)
   {
   
    err_bins_nt.push_back((float)bin_count);
    err_bins_nt_er.push_back(1.);
   
    TH1D * profileY = h_Vrt_err_vs_ntrk->ProjectionY("_py",bin_count,bin_count);
    
    float mean = profileY->GetMean();
    float mean_error = profileY->GetMeanError();
     
    av_err.push_back(mean);
    av_err_er.push_back(mean_error);
//    res.push_back(mean * sigma[bin_count]);
   
    res.push_back(mean * kgs_ntrk_fit->Eval(bin_count));
    double pr_er = error_func( bin_count, kgs_ntrk_fit_er); 
    
    res_er.push_back(TMath::Sqrt( TMath::Power( mean_error *kgs_ntrk_fit->Eval(bin_count),2) +   TMath::Power(mean,2))* pr_er);
   }//end of loop over all the X bins of the vertex error vs ntrk histo 
   
   TGraphErrors * mean_er_vs_ntrk= new TGraphErrors(nbins_err_ntrk, &(err_bins_nt[0]),&(av_err[0]),&(err_bins_nt_er[0]), &(av_err_er[0]) );
   mean_er_vs_ntrk->GetYaxis()->SetTitle("Average Vertex Error ");
   mean_er_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks");
   
   TGraphErrors * res_vs_ntrk= new TGraphErrors(nbins_err_ntrk, &(err_bins_nt[0]),&(res[0]),&(err_bins_nt_er[0]), &(res_er[0]) );
   res_vs_ntrk->GetYaxis()->SetTitle("x Vertex Resolution [mm]");
   res_vs_ntrk->GetXaxis()->SetTitle("Number of fitted tracks");
   
//resolution versus pt2
   int nbins_err_pt = h_Vrt_err_vs_pt2->GetNbinsX(); 
   float bin_size_pt =  h_Vrt_err_vs_pt2->GetXaxis()->GetBinWidth(2);
   
   h_Vrt_err_vs_pt2->SetName("error_vs_pt2");
   h_Vrt_err_vs_pt2->Write();

   std::vector<float> av_err_pt;
   std::vector<float> av_err_pt_er;
   
   std::vector<float> err_bins_pt;
   std::vector<float> err_bins_pt_er;
   
   std::vector<float> res_pt;
   std::vector<float> res_pt_er;
      
   for(int bin_count= 1; bin_count <=nbins_err_pt; ++bin_count)
   {
    err_bins_pt.push_back((float)bin_count*bin_size_pt);
    err_bins_pt_er.push_back(bin_size_pt);
    
    TH1D * profileY = h_Vrt_err_vs_pt2->ProjectionY("_py",bin_count,bin_count);
    
    float mean = profileY->GetMean();
    float mean_error = profileY->GetMeanError();
     
    av_err_pt.push_back(mean);
    av_err_pt_er.push_back(mean_error);
    
 //   res_pt.push_back(mean * pt_sigma[bin_count]);
    res_pt.push_back(mean * kgs_fit->Eval(pt_bins_nt[bin_count] ));
   
   
//estimating the approximate k-factor and the error value   
    double pr_er = error_func( pt_bins_nt[bin_count]  , kgs_fit_er); 
    res_pt_er.push_back(TMath::Sqrt(TMath::Power(mean_error * kgs_fit->Eval(pt_bins_nt[bin_count] ),2) + TMath::Power(mean ,2) * pr_er )); 
   }
   
   TGraphErrors * mean_er_vs_pt= new TGraphErrors(nbins_err_pt, &(err_bins_pt[0]),&(av_err_pt[0]),&(err_bins_pt_er[0]), &(av_err_pt_er[0]) );
   mean_er_vs_pt->GetYaxis()->SetTitle("Average Vertex Error ");
   mean_er_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} [GeV]");
   
   
   TGraphErrors * res_vs_pt= new TGraphErrors(nbins_err_pt, &(err_bins_pt[0]),&(res_pt[0]),&(err_bins_pt_er[0]), &(res_pt_er[0]) );
   res_vs_pt->GetYaxis()->SetTitle("x Vertex Resolution [mm]");
   res_vs_pt->GetXaxis()->SetTitle("#sqrt{#sum p_{T}^{2}} [GeV]");
   
// Plotting section--------------------------------------------------------------
// canvas and plots

//without beam constraint
   TCanvas* c_Vrt_1 = new TCanvas("VtxCanvas1", "VtxCanvas1", 1000, 600);
   c_Vrt_1->Divide(2,3);
   c_Vrt_1->cd(1) ;  
   fitStatus = h_Vrt_X_rec->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_X_rec->GetFunction("gaus")->SetLineColor(kBlue);
   h_Vrt_X_rec->Draw();
   
   c_Vrt_1->cd(2) ; 
   fitStatus = h_Vrt_Y_rec->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_Y_rec->GetFunction("gaus")->SetLineColor(kBlue);
   h_Vrt_Y_rec->Draw();
  
   c_Vrt_1->cd(3) ;
   fitStatus = h_Vrt_Z_rec->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_Z_rec->GetFunction("gaus")->SetLineColor(kBlue);
   h_Vrt_Z_rec->Draw();
   
   c_Vrt_1->cd(4);  
   c_Vrt_1->cd(4)->SetLogy();  
   h_vx_n->Draw();
   
   c_Vrt_1->cd(5);
   h_vx_dz2->Draw();


//with beam constraint
   TCanvas* c_Vrt_1_bc = new TCanvas("VtxCanvas1_bc", "VtxCanvas1_bc", 1000, 600);
   c_Vrt_1_bc->Divide(2,3);
   
   c_Vrt_1_bc->cd(1) ;  
   fitStatus = h_Vrt_X_rec_bc->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_X_rec_bc->GetFunction("gaus")->SetLineColor(kBlue);
   h_Vrt_X_rec_bc->Draw();
   
   c_Vrt_1_bc->cd(2) ; 
   fitStatus = h_Vrt_Y_rec_bc->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_Y_rec_bc->GetFunction("gaus")->SetLineColor(kBlue);
   h_Vrt_Y_rec_bc->Draw();
  
   c_Vrt_1_bc->cd(3) ;
   fitStatus = h_Vrt_Z_rec_bc->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_Z_rec_bc->GetFunction("gaus")->SetLineColor(kBlue);
   h_Vrt_Z_rec_bc->Draw();
   
   c_Vrt_1_bc->cd(4);  
   c_Vrt_1_bc->cd(4)->SetLogy();  
   h_vx_n_bc->Draw();
   
   c_Vrt_1_bc->cd(5);  
   h_vx_dz2_bc->Draw();

// scatter plots and beam profiles
   TCanvas* c_Vrt_2 = new TCanvas("VtxCanvas2", "VtxCanvas2", 1000, 600);
   c_Vrt_2->Divide(2,2);
   c_Vrt_2->cd(1) ;  
   c_Vrt_2->cd(1)->SetGrid();
   h_Vrt_XY_rec->Draw("COLZ");
   
   c_Vrt_2->cd(2) ;  
   c_Vrt_2->cd(2)->SetGrid();
   h_Vrt_ZX_rec->Draw("COLZ");
   
   c_Vrt_2->cd(3) ; 
   c_Vrt_2->cd(3)->SetGrid();
   h_Vrt_ZY_rec->Draw("COLZ");
   
   TCanvas* pileUpCanvas = new TCanvas("PileUpCanvas", "PileUpCanvas", 1000, 600);
   pileUpCanvas->Divide(2,2);
   
   pileUpCanvas->cd(1)->SetLogy();
   h_Vrt_ntracksSV->Draw();
  
   pileUpCanvas->cd(2)->SetLogy();
   h_Vrt_ntracksPV->Draw();
   
   pileUpCanvas->cd(3)->SetLogy();  
   h_Vrt_ntracksPU1->SetLineColor(2);
   h_Vrt_ntracksPU1->Draw();
   
   pileUpCanvas->cd(4)->SetGrid(); 
   h_Vrt_nTracksPVvsPU1->Draw("COLZ"); 
   
   TCanvas* pileUpCanvas1 = new TCanvas("PileUpCanvas1", "PileUpCanvas1", 1000, 600);
   pileUpCanvas1->Divide(2,2);
   
   pileUpCanvas1->cd(1)->SetGrid();
   deltaZ_vs_2ndmult->Draw("COLZ");
   
   pileUpCanvas1->cd(2);//->SetLogy();
 //  fitStatus = pu_DeltaZ->Fit("gaus","Q0");
  // if (fitStatus != 0) { fitFailed(); return; }
  // pu_DeltaZ->GetFunction("gaus")->SetLineColor(kBlue);
   pu_DeltaZ->Draw(); 
 
   pileUpCanvas1->cd(3);//->SetLogy();
//   fitStatus = pu_DeltaZ_cut3->Fit("gaus","Q0");
//   if (fitStatus != 0) { fitFailed(); return; }
 //  pu_DeltaZ_cut3->GetFunction("gaus")->SetLineColor(kBlue);
   pu_DeltaZ_cut3->Draw();
   
   pileUpCanvas1->cd(4);//->SetLogy();
//   fitStatus = pu_DeltaZ_cut4->Fit("gaus","Q0");
//   if (fitStatus != 0) { fitFailed(); return; }
//   pu_DeltaZ_cut4->GetFunction("gaus")->SetLineColor(kBlue);
   pu_DeltaZ_cut4->Draw();

   TCanvas* pileUpCanvas2 = new TCanvas("PileUpCanvas2", "PileUpCanvas2", 1000, 600);
   pileUpCanvas2->Divide(1,2);
 //  pileUpCanvas2->cd(1)->SetLogx();
   pileUpCanvas2->cd(1);
   sum_pt2_vs_mult->Draw("COLZ"); 
   
   pileUpCanvas2->cd(2);
 //  pileUpCanvas2->cd(2)->SetLogx();
   sum_pt2_vs_mult_all->Draw("COLZ");
   
   TCanvas* splitCanvas = new TCanvas("splitCanvas", "splitCanvas", 1000, 600);
   splitCanvas->Divide(2,3);
   splitCanvas->cd(1);
   
   h_vx_n_split->Draw();
   splitCanvas->cd(2);
   fitStatus = h_Vrt_X_pull_split->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_X_pull_split->Draw();
   splitCanvas->cd(3);
   fitStatus = h_Vrt_Y_pull_split->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_Y_pull_split->Draw();
   splitCanvas->cd(4);
   fitStatus = h_Vrt_Z_pull_split->Fit("gaus","Q0");
   if (fitStatus != 0) { fitFailed(); return; }
   h_Vrt_Z_pull_split->Draw();
   splitCanvas->cd(5);
   h_Vrt_ntracksPV_split->Draw();
   splitCanvas->cd(6);
   h_Vrt_ntracksPV_av_split->Draw();
 
   TCanvas* splitCanvas1 = new TCanvas("splitCanvas1", "splitCanvas1", 1000, 600);
   splitCanvas1->Divide(2,3);
   splitCanvas1->cd(1);
   h_Vrt_XpullVsNtrk_split->Draw("COLZ");
   
   splitCanvas1->cd(2)->SetGrid();
  // splitCanvas1->cd(2)->SetLogy();
   krms_vs_ntrk->Draw("AP");

   splitCanvas1->cd(3)->SetGrid();
  // splitCanvas1->cd(3)->SetLogy();
   kgs_vs_ntrk->Draw("AP");
   
   splitCanvas1->cd(4)->SetLogy();
   h_Vrt_XpullVsPt2_split->Draw("COLZ");
   
   splitCanvas1->cd(5)->SetGrid();
  // splitCanvas1->cd(5)->SetLogy();
   krms_vs_pt->Draw("AP");

   splitCanvas1->cd(6)->SetGrid();
  // splitCanvas1->cd(6)->SetLogy();
   kgs_vs_pt->Draw("AP");
   
   
// split results for the Z coordiate
   TCanvas* splitCanvas2 = new TCanvas("splitCanvas2", "splitCanvas2", 1000, 600);
   splitCanvas2->Divide(2,3);
   splitCanvas2->cd(1);
 
   h_Vrt_ZpullVsNtrk_split->Draw("COLZ");
 
   splitCanvas2->cd(2)->SetLogy();
   h_Vrt_ZpullVsPt2_split->Draw("COLZ");
   
   splitCanvas2->cd(3);
   krms_z_vs_ntrk->Draw("AP");
   
   splitCanvas2->cd(4);
   kgs_z_vs_ntrk->Draw("AP");

   splitCanvas2->cd(5);
   krms_z_vs_pt->Draw("AP");

   splitCanvas2->cd(6);
   kgs_z_vs_pt->Draw("AP");

   
   TCanvas* errorCanvas1 = new TCanvas("errorCanvas1", "errorCanvas1", 1000, 600);
   errorCanvas1->Divide(2,3);
   errorCanvas1->cd(1)->SetLogy();
   h_Vrt_err_vs_ntrk->Draw("COLZ");

   errorCanvas1->cd(2)->SetLogy();
   h_Vrt_err_vs_pt2->Draw("COLZ");
   
 //  errorCanvas1->cd(3)->SetLogy();
 //  h_Vrt_err2_vs_ntrk->Draw("COLZ");
   
 //  errorCanvas1->cd(4)->SetLogy();
 //  h_Vrt_err2_vs_pt2->Draw("COLZ");
   
   errorCanvas1->cd(3)->SetLogy();
   errorCanvas1->cd(3)->SetGrid();
   mean_er_vs_ntrk->Draw("AP");

   errorCanvas1->cd(4)->SetLogy();
   errorCanvas1->cd(4)->SetGrid();
   res_vs_ntrk->Draw("AP");
   
   errorCanvas1->cd(5)->SetLogy();
   errorCanvas1->cd(5)->SetGrid();
   mean_er_vs_pt->Draw("AP");

   errorCanvas1->cd(6)->SetLogy();
   errorCanvas1->cd(6)->SetGrid();
   res_vs_pt->GetHistogram()->GetYaxis()->SetRangeUser(0.001, 1.5); 
   res_vs_pt->Draw("AP");
   
   TCanvas* errorCanvas2 = new TCanvas("errorCanvas2", "errorCanvas2", 1000, 600);
   errorCanvas2->Divide(2,3);
   errorCanvas2->cd(1)->SetLogy();
   h_Vrt_err_z_vs_ntrk->Draw("COLZ");
   
   errorCanvas2->cd(2)->SetLogy();
   h_Vrt_err_z_vs_pt2->Draw("COLZ");
  
   errorCanvas2->cd(3)->SetLogy();
   errorCanvas2->cd(3)->SetGrid();
   res_z_vs_ntrk->Draw("AP");
   
   errorCanvas2->cd(4)->SetLogy();
   errorCanvas2->cd(4)->SetGrid();
   res_z_vs_pt->GetHistogram()->GetYaxis()->SetRangeUser(-0.5, 4.); 
   res_z_vs_pt->Draw("AP");
 
   TCanvas* puCanvas1 = new TCanvas("puCanvas1", "puCanvas1", 1000, 600);
   puCanvas1->Divide(2,3);
   puCanvas1->cd(1);
   TF1 * fit_g = new TF1("half_gaus","gaus", 0.,300.);
   fit_g->FixParameter(1,0.);

   fitStatus = h_Vrt_deltaZ_bc->Fit("half_gaus", "Q0", "", 6.,300.);
   if (fitStatus != 0) { fitFailed(); return; }

   h_Vrt_deltaZ_bc->Draw();

   puCanvas1->cd(2);
   h_Vrt_deltaZ_bc_vs_nvx->Draw("COLZ");
   
   puCanvas1->cd(3);
   h_Vrt_deltaZ_bc_cut->Draw();
   
   puCanvas1->cd(4);
   h_Vrt_deltaZ_bc_vs_nvx_cut->Draw("COLZ");
   
   puCanvas1->cd(5);
   h_Vrt_Ntrk_bc_one->Draw();
   
   puCanvas1->cd(6);
   h_Vrt_ntracksPV_bc_vs_nvx->Draw("COLZ");
   
   TCanvas* puCanvas2 = new TCanvas("puCanvas2", "puCanvas2", 1000, 600);
   puCanvas2->Divide(2,2);
  
   puCanvas2->cd(1);
   h_Vrt_deltaZ_bc_vs_nvx_cut->ProjectionY("3d bin",3,3)->Draw();
  
   puCanvas2->cd(2);
   h_Vrt_deltaZ_bc_vs_nvx_cut->ProjectionY("4th bin",4,4)->Draw();
  
   puCanvas2->cd(3);
   h_Vrt_deltaZ_bc_vs_nvx_cut->ProjectionY("5th bin",5,5)->Draw();
   
   puCanvas2->cd(4);
   h_Vrt_deltaZ_bc_vs_nvx_cut->ProjectionY("6th bin",6,6)->Draw();
   
   TCanvas* ntrkCanvas1 = new TCanvas("ntrkCanvas1", "ntrkCanvas1", 1000, 600);
   ntrkCanvas1->Divide(2,2);
   
   ntrkCanvas1->cd(1);
   h_Vrt_ntracksPV_bc_vs_nvx->ProjectionY("1st",1,1)->Draw();
  
   ntrkCanvas1->cd(2);
   h_Vrt_ntracksPV_bc_vs_nvx->ProjectionY("2nd",2,2)->Draw();
  
   ntrkCanvas1->cd(3);
   h_Vrt_ntracksPV_bc_vs_nvx->ProjectionY("3d",3,3)->Draw();
   
   ntrkCanvas1->cd(4);
   h_Vrt_ntracksPV_bc_vs_nvx->ProjectionY("4th",4,4)->Draw();
   
   TCanvas* puCanvas3 = new TCanvas("TwoVertexCanvas", "TwoVertexCanvas", 1000, 600);
   puCanvas3->Divide(2,2);
   puCanvas3->cd(1)->SetLogy();
   h_Vrt_Ntrk_bc_one->Draw(); 
    
   puCanvas3->cd(2)->SetLogy();
   h_Vrt_Ntrk_bc_pv1->Draw();
   
   puCanvas3->cd(3)->SetLogy();  
   h_Vrt_Ntrk_bc_pv2->Draw();


   TCanvas* puCanvas4 = new TCanvas("ThreeVertexCanvas", "ThreeVertexCanvas", 1000, 600);
   puCanvas4->Divide(2,3);
   puCanvas4->cd(1);
   h_Vrt_Ntrk_bc_pv1_3->Draw();
   
   puCanvas4->cd(2);
   h_Vrt_Ntrk_bc_pv2_3->Draw();
   
   
   puCanvas4->cd(3);
   h_Vrt_Ntrk_bc_pv3_3->Draw();
   

   TCanvas* randomCanvas = new TCanvas("RandomVertexCanvas", "RandomVertexCanvas", 1000, 600);
   randomCanvas->Divide(2,2);
   randomCanvas->cd(1)->SetLogy();
   
  
//creating random distributions 
   TH1F* h_gen_ntracksPV1  = defineHistogram("h_gen_ntracksPV1", 100, 0., 100.,  "Number of tracks per vertex", "Entries");
   
   TH1F* h_gen_ntracksPV2 = defineHistogram("h_gen_ntracksPV2", 100, 0., 100.,  "Number of tracks per vertex", "Entries");   
   
   
   unsigned int gen_limit = 100000;
   
   for(unsigned int i = 0;i<gen_limit;++i)
   {
//generating two random number of tracks, using 
//the single collision as the pdf.
    unsigned int one_vertex =   (unsigned int)h_Vrt_Ntrk_bc_one->GetRandom();  
    unsigned int other_vertex = (unsigned int)h_Vrt_Ntrk_bc_one->GetRandom();  
    if(one_vertex>other_vertex)
    {
     h_gen_ntracksPV1->Fill(one_vertex);
     h_gen_ntracksPV2->Fill(other_vertex);
    }else{
     h_gen_ntracksPV1->Fill(other_vertex);
     h_gen_ntracksPV2->Fill(one_vertex);
    }//end of histogram filling 
   }//end of generation loop
    
    
// for protection, we use the separate  generation loop for 
// cases with 3 vertices
   TH1F* h_gen_ntracks_pv1_3 = defineHistogram("Ntrk gen three collisions 1", 100, 0., 100.,  "Number of tracks gen at PV1 vertex", "Entries");
   TH1F* h_gen_ntracks_pv2_3 = defineHistogram("Ntrk gen three collisions 2", 100, 0., 100.,  "Number of tracks gen at PV2 vertex", "Entries");
   TH1F* h_gen_ntracks_pv3_3 = defineHistogram("Ntrk gen three collisions 3", 100, 0., 100.,  "Number of tracks gen at PV3 vertex", "Entries");
   
   for(unsigned int i = 0;i<gen_limit;++i)
   {
//generating two random number of tracks, using 
//the single collision as the pdf.
    
    unsigned int ntrk_gen[3];
    for(unsigned int i =0;i<3;++i) ntrk_gen[i] = h_Vrt_Ntrk_bc_one->GetRandom();  
    const int N = sizeof(ntrk_gen) / sizeof(unsigned int);
    std::sort(ntrk_gen, ntrk_gen +N);
    
    h_gen_ntracks_pv1_3->Fill(ntrk_gen[2]);
    h_gen_ntracks_pv2_3->Fill(ntrk_gen[1]);
    h_gen_ntracks_pv3_3->Fill(ntrk_gen[0]);
    
   }//end of second generation loop
   
    
   h_gen_ntracksPV1->Draw();
   
   randomCanvas->cd(2)->SetLogy();
   h_gen_ntracksPV2->Draw();
   
   randomCanvas->cd(3)->SetLogy();
   
//   randomCanvas->cd(3);
  
   h_Vrt_Ntrk_bc_pv1->SetLineColor(kRed);
   h_Vrt_Ntrk_bc_pv1->Draw();
   h_gen_ntracksPV1->DrawNormalized("same",h_Vrt_Ntrk_bc_pv1->Integral());

   randomCanvas->cd(4)->SetLogy();
//   randomCanvas->cd(4);
  
   h_Vrt_Ntrk_bc_pv2->SetLineColor(kRed);
   h_Vrt_Ntrk_bc_pv2->Draw("");
   h_gen_ntracksPV2->DrawNormalized("same",h_Vrt_Ntrk_bc_pv2->Integral());
   
   
   TCanvas* randomCanvas1 = new TCanvas("RandomVertexCanvas 1", "RandomVertexCanvas 1", 1000, 600);
   randomCanvas1->Divide(2,3);
   
   randomCanvas1->cd(1);
   h_Vrt_Ntrk_bc_pv1_3->SetLineColor(kRed);
   h_Vrt_Ntrk_bc_pv1_3->Draw();
   h_gen_ntracks_pv1_3->DrawNormalized("same",h_Vrt_Ntrk_bc_pv1_3->Integral());
   
   
   
   randomCanvas1->cd(2);
   h_Vrt_Ntrk_bc_pv2_3->SetLineColor(kRed);
   h_Vrt_Ntrk_bc_pv2_3->Draw();
   h_gen_ntracks_pv2_3->DrawNormalized("same",h_Vrt_Ntrk_bc_pv2_3->Integral());
    
   randomCanvas1->cd(3); 
   
   
   h_Vrt_Ntrk_bc_pv3_3->SetLineColor(kRed);
   h_Vrt_Ntrk_bc_pv3_3->Draw();
   h_gen_ntracks_pv3_3->DrawNormalized("same", h_Vrt_Ntrk_bc_pv3_3->Integral());
   
   outputFile->Write();

   
   TCanvas* systemCanvas = new TCanvas("Systematics Canvas", "Systematics Canvas", 1000, 600);
   h_nSplit_vs_nBC->Draw("COLZ");
   
   
   TCanvas* systemCanvas1 = new TCanvas("Systematics Canvas 1", "Systematics Canvas 1", 1000, 600);
   systemCanvas1->Divide(2,4);
   
   systemCanvas1->cd(1);
   TH1D * bin1 = h_nSplit_vs_nBC->ProjectionX("X slice",2,2);
   bin1->DrawCopy();
   
   systemCanvas1->cd(2);
   TH1D * bin2 = h_nSplit_vs_nBC->ProjectionX("X slice",3,3);
   bin2->DrawCopy();
  
   systemCanvas1->cd(3);
   TH1D * bin3 = h_nSplit_vs_nBC->ProjectionX("X slice",4,4);
   bin3->DrawCopy();
   
   systemCanvas1->cd(4);
   TH1D * bin4 = h_nSplit_vs_nBC->ProjectionX("X slice",5,5);
   bin4->DrawCopy();
   
   systemCanvas1->cd(5);
   TH1D * bin5 = h_nSplit_vs_nBC->ProjectionX("X slice",6,6);
   bin5->DrawCopy();
   
   systemCanvas1->cd(6);
   TH1D * bin6 = h_nSplit_vs_nBC->ProjectionX("X slice",7,7);
   bin6->DrawCopy();
   
   systemCanvas1->cd(7);
   TH1D * bin7 = h_nSplit_vs_nBC->ProjectionX("X slice",8,8); 
   bin7->DrawCopy();
   
   systemCanvas1->cd(8);
   TH1D * bin8 = h_nSplit_vs_nBC->ProjectionX("X slice",9,9);  
   bin8->DrawCopy();

 
//collecting the information from the corresponding bins.. 
   std::vector<unsigned int> n_eff;
   std::vector<unsigned int> n_below;
   std::vector<unsigned int> n_above;
   
   float nvx_bins[nbins_study];
   float nvx_bins_er[nbins_study];
   
   float nvx_eff[nbins_study];
   float nvx_eff_er[nbins_study];
   
   float nvx_split[nbins_study];
   float nvx_split_er[nbins_study];
   
   float nvx_merge[nbins_study];
   float nvx_merge_er[nbins_study];
   
   for(unsigned int i = 1;i<=nbins_study;++i)
   {
    TH1D * slice = h_nSplit_vs_nBC->ProjectionX("X slice",i+1,i+1);
    float  central = slice->GetBinContent(i+1);
    float  below  = slice->Integral(0,i);
    float  above = slice->Integral(i+2,nbins_study);
    float  total = central+below+above;
    
 //   if(slice) delete slice;   
    
    nvx_bins[i-1] = float(i);
    nvx_bins_er[i-1] = 1.;
    if(total>10.)
    {
     nvx_eff[i-1]   = central/total;
     nvx_merge[i-1] = below/total;
     nvx_split[i-1] = above/total;
   
     nvx_eff_er[i-1] = TMath::Sqrt(central)/total;
     nvx_merge_er[i-1] = TMath::Sqrt(below)/total;
     nvx_split_er[i-1] = TMath::Sqrt(above)/total;
     
    }else{
     nvx_eff[i-1]   = 0.;
     nvx_merge[i-1] = 0.;
     nvx_split[i-1] = 0.;
     
     nvx_eff_er[i-1]   = 0.;
     nvx_merge_er[i-1] = 0.;
     nvx_split_er[i-1] = 0.;
    }
 //   std::cout<<"central "<<central<<std::endl;
 //   std::cout<<"below   "<<below<<std::endl;
 //   std::cout<<"abowe   "<<above<<std::endl; 
  
   }//end of loop over all the 2d bins 

//plotting various quantities
   TGraphErrors * pu_bin_eff = new TGraphErrors(nbins_study, &(nvx_bins[0]),&(nvx_eff[0]),&(nvx_bins_er[0]), &(nvx_eff_er[0]) );
   pu_bin_eff->GetYaxis()->SetTitle("Counting efficiency");
   pu_bin_eff->GetXaxis()->SetTitle("Number of reco vertices");
   pu_bin_eff->SetName("Pile Up Bin Efficiency");
  
   
   TGraphErrors * pu_bin_below = new TGraphErrors(nbins_study, &(nvx_bins[0]),&(nvx_merge[0]),&(nvx_bins_er[0]), &(nvx_merge_er[0]) );
   pu_bin_below->GetYaxis()->SetTitle("Merging efficiency");
   pu_bin_below->GetXaxis()->SetTitle("Number of reco vertices");
   pu_bin_below->SetName("Pile Up Merging Efficiency");
 
   TGraphErrors * pu_bin_above = new TGraphErrors(nbins_study, &(nvx_bins[0]),&(nvx_split[0]),&(nvx_bins_er[0]), &(nvx_split_er[0]) );
   pu_bin_above->GetYaxis()->SetTitle("Splitting and fake efficiency");
   pu_bin_above->GetXaxis()->SetTitle("Number of reco vertices");
   pu_bin_above->SetName("Pile Up Splitting Efficiency");
 
   
   TCanvas* puEffCanvas = new TCanvas("Pile Up efficiency Canvas 1", "Pile up efficiency Canvas 1", 1000, 600);
   puEffCanvas->Divide(2,2);
   puEffCanvas->cd(1);
   pu_bin_eff->Draw("AP");  
   
   puEffCanvas->cd(2);
   pu_bin_below->Draw("AP");
   
   puEffCanvas->cd(3);
   pu_bin_above->Draw("AP");
   
   
}//end of loop method

TH1F* RealTrackTree::defineHistogram(TString name, unsigned int bins, Double_t lbin, Double_t ubin, TString xaxistitle, TString yaxistitle)
{
  TH1F* histogram = new TH1F(name, name, bins, lbin, ubin);
  TAxis* xaxis = histogram->GetXaxis();
  xaxis->SetTitle(xaxistitle);
  TAxis* yaxis = histogram->GetYaxis();
  yaxis->SetTitle(yaxistitle);
  return histogram;
}

TH2F* RealTrackTree::define2DHistogram(TString name, unsigned int xbins, Double_t lxbin, Double_t uxbin, unsigned int ybins, Double_t lybin, Double_t uybin, TString xaxistitle, TString yaxistitle)
{
  TH2F* histogram = new TH2F(name, name, xbins, lxbin, uxbin, ybins, lybin, uybin);
  TAxis* xaxis = histogram->GetXaxis();
  xaxis->SetTitle(xaxistitle);
  TAxis* yaxis = histogram->GetYaxis();
  yaxis->SetTitle(yaxistitle);
  return histogram;
}

// this code has been copied from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MinBiasD3PDDocu#CTP_Information
bool RealTrackTree::trigger_L1_MBTS_1()
{


// bool eventPasses = (ei_bcid == 1 && ei_RunNumber == 152166) || ((ei_bcid == 1 || ei_bcid == 201 || ei_bcid == 301 || ei_bcid == 1786 
//      || ei_bcid == 1886 || ei_bcid == 1986 || ei_bcid == 2086)&& ei_RunNumber == 155697);

  bool eventPasses = true;

/*

 if (eventPasses)
 {
   bool triggerOK = false;


   for (unsigned int i = 0 ; i < ctp_decisionItems->size() ; i++)
   {
     if (ctp_decisionItems->at(i) == "L1_MBTS_1") { triggerOK = true; continue; }
   }
   eventPasses = triggerOK;
 } else return false;
*/

 return eventPasses; 
 
}//end of trigger selectio function

bool RealTrackTree::good_run()
{
  bool good_run = true;
  
 // if(ei_RunNumber == 152166 && ei_lbn < 205 ) good_run = false;
  
  return good_run;

}


double RealTrackTree::error_func( float  x, Double_t *par)
{
//calculating the square of the propagated error on the fit values
 return  ( TMath::Power(par[0],2) +  x * TMath::Power(par[1],2) +   TMath::Power(x * par[2],2)) ;

}

//stabilised Gaussian fit: First fitting a  pull histogram
// in a broad range, finding the parameters of the fit and
//re-using them as staring values until the fit is stanilised.
std::vector<float> RealTrackTree::stableGaussianFit(TH1 * histo)
{
 std::vector<float> results;
 
 if(histo)
 {
  double sigmas  = 2.;
  Int_t fitStatus = histo->Fit("gaus","Q0","",-2,2);
  if (fitStatus != 0) { fitFailed(); results.push_back(0.); results.push_back(0.); return results; }
  TF1* func=histo->GetFunction("gaus");
  double actualSigma=func->GetParameter(2);
  double actualSigmaErr=func->GetParError(2);

  for (int u=0;u<10;u++)
  {
   histo->Fit("gaus","Q0","",-sigmas*actualSigma,sigmas*actualSigma);
   if (fitStatus != 0) { fitFailed(); results.push_back(0.); results.push_back(0.); return results; }
   func=histo->GetFunction("gaus");
   actualSigma=func->GetParameter(2);
   actualSigmaErr=func->GetParError(2);
  }//end of the fitting loop
  
  results.push_back(actualSigma);
  results.push_back(actualSigmaErr);
 }else{
  
  results.push_back(0.);
  results.push_back(0.);
 }//end of protection against an empty histogram

 return results; 
}//end of stableGaussian Fit function


bool RealTrackTree::split_bin_test(unsigned int nbin)
{
 if(vxsplit_n < 2*nbin) return false;
 
 bool dummies_after = true;
 bool reals_before = true;
 
 for(int i = 0;i<vxsplit_n;++i)
 {
  if(i< 2*nbin) reals_before = reals_before && vxsplit_type->at(i) == 1;
  else dummies_after = dummies_after && vxsplit_type->at(i) == 0;
 }//end of loop over all the split vertices
 
 return  reals_before&&dummies_after;
}//end of check for events which can be used for the analysis of the current bin
