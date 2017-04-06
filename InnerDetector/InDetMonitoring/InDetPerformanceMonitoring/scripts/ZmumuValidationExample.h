/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUVALIDATIONEXAMPLE_h
#define ZMUMUVALIDATIONEXAMPLE_h

#include <vector>
#include <list>
#include <string>

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile2D.h"
#include "TLorentzVector.h"
#include "TCanvas.h"

class ZmumuValidationExample 
{
  private:

   const std::list<std::string> m_fileNames;
   string m_outfilename;
   
   TFile    m_file;
      
   TChain m_eventChain;
   TChain m_truthChain;
   
   int m_eventCurrent;
   int m_PrintLevel;
   int m_EtaBins;
   int m_PhiBins;

  
   bool m_isMC;
   int n_iteration;         
   double m_shift;
   double m_factor;
   
   /////////////////////////////////////////////
   // Declaration of leafs for the track tree
   /////////////////////////////////////////////
   double m_px_pos;
   double m_py_pos;
   double m_pz_pos;
   
   double m_px_neg;
   double m_py_neg;
   double m_pz_neg;
   
  double m_z0_pos;
  double m_z0_neg;
  double m_d0_pos;
  double m_d0_neg;
   
   double m_truth_px_pos;
   double m_truth_py_pos;   
   double m_truth_pz_pos;
   
   double m_truth_px_neg;
   double m_truth_py_neg;
   double m_truth_pz_neg;
  	           
   /////////////////////////////////////
   // List of branches for track tree
   /////////////////////////////////////
   TBranch * b_px_pos;
   TBranch * b_py_pos;
   TBranch * b_pz_pos;
   
   TBranch * b_px_neg;
   TBranch * b_py_neg;
   TBranch * b_pz_neg;      

   TBranch * b_z0_pos;
   TBranch * b_z0_neg;
   TBranch * b_d0_pos;
   TBranch * b_d0_neg;

   TBranch * b_truth_px_pos;
   TBranch * b_truth_py_pos;
   TBranch * b_truth_pz_pos;
   	            
   TBranch * b_truth_px_neg;
   TBranch * b_truth_py_neg;
   TBranch * b_truth_pz_neg;	  
      
   /////////////////////
   // Root histograms
   /////////////////////

   TH1 * h_DELTA; 

   TH1 * h_pt; 
   TH1 * h_pt_pos;
   TH1 * h_pt_neg;

   TH1 * h_pt_truth;
   TH1 * h_pt_pos_truth;
   TH1 * h_pt_neg_truth;
   
   TH1 * h_z0;
   TH1 * h_z0_pos;
   TH1 * h_z0_neg;
   TH1 * h_d0;
   TH1 * h_d0_pos;
   TH1 * h_d0_neg;

   TH1 * h_mass; 
   TH1 * pcorrected_mass; 
   TH1 * lambdacorrected_mass;

   TH3 * pcorrected_mass_vs_etaphi; 
   TH3 * lambdacorrected_mass_vs_etaphi;

   TH2 * etaphi_pos;
   TH2 * etaphi_neg;

   TProfile2D * prof_pt_vs_etaphi;

   //delta corrections p->p(1+delta) - charge symmetric
   TH3 * delta_vs_etaphi;         

   TH2 * deltacorrections_vs_etaphi; 

   TH2 * deltacorrections_vs_etaphi_err; 

   //lambda corrections p->p(1+Q*lambda*pT) - charge anti-symmetric
   TH3 * lambda_vs_etaphi;         

   TH2 * lambdacorrections_vs_etaphi;
   TH2 * lambdacorrections_vs_etaphi_err;
   TH2 * lambdacorrections_vs_etaphi_RMS;

   TH2 * lambda_vs_eta;   
   TH2 * lambda_vs_eta_pos;
   TH2 * lambda_vs_eta_neg;

   TH1 * lambdacorrections_vs_eta;
    
   TH1 * lambda;
   TH1 * lambda_pos;
   TH1 * lambda_neg;

   TH1 * lambda_etaphi;
   TH1 * lambda_etaphi_pos;
   TH1 * lambda_etaphi_neg;

   TH1 * lambda_eta;
   TH1 * lambda_eta_pos;
   TH1 * lambda_eta_neg;
   
   //z0 histograms 
   TH2 * entries;
   
   TH3 * z0delta_vs_etaphi;         

   TH2 * z0deltacorrections_vs_etaphi;
   TH2 * z0deltacorrections_vs_etaphi_err;

   TH1 * z0delta;
   TH1 * z0delta_pos;
   TH1 * z0delta_neg;

   TH1 * z0delta_etaphi;
   TH1 * z0delta_etaphi_pos;
   TH1 * z0delta_etaphi_neg;

   //d0 histograms
   TH3 * d0delta_vs_etaphi;         

   TH2 * d0deltacorrections_vs_etaphi;
   TH2 * d0deltacorrections_vs_etaphi_err;

   TH1 * d0delta;
   TH1 * d0delta_pos;
   TH1 * d0delta_neg;

   TH1 * d0delta_etaphi;
   TH1 * d0delta_etaphi_pos;
   TH1 * d0delta_etaphi_neg;

   //truth histograms	   
   TH3 * delta_vs_etaphi_truth;     
   TH2 * deltacorrections_vs_etaphi_truth; 
   TH2 * deltacorrections_vs_etaphi_truth_err; 

   TH3 * lambda_vs_etaphi_truth;    
   TH2 * lambdacorrections_vs_etaphi_truth;
   TH2 * lambdacorrections_vs_etaphi_truth_err;

   TH2 * lambda_vs_eta_truth;       
   TH1 * lambdacorrections_vs_eta_truth;

   TH2 * truth_mom_bias_vs_eta;       
   TH1 * truth_mom_biascorrections_vs_eta;

   TH2 * truth_mass_bias_vs_eta;
   TH1 * truth_mass_biascorrections_vs_eta;

   TH1 * lambda_truth;
   TH1 * lambda_truth_pos;
   TH1 * lambda_truth_neg;

   TH1 * delta_phi_truth;

   TH1 * delta_eta_truth;
    
   TH2* delta_M2_vs_zpt_truth; 
   TH2* delta_M2_vs_zpt;
    
   TProfile2D* delta_M2_vs_etaphi_pos; 
   TProfile2D* delta_M2_vs_etaphi_neg; 

   // q/Pt histograms
   TH2* h_QoverPt; 
   TH3* h_QoverPt3D; 
   
   ////////////////////////////
   // canvas to show fit resutls
   ////////////////////////////
   TCanvas* TempCanvasIterGaussFit;

   ////////////////////////////////////////////
   // more helper methods
   // 
   // these methods are used to initialize
   // histograms, perform cuts, and fill
   // histograms
   ////////////////////////////////////////////    
   void loadChains();
   void setBranchAddresses();
   void bookHistograms();
    
   void loopThroughEvents( unsigned int maxItr );
   void fillHistograms();
   
   void correctMomentum(TH1* h_corrections, TLorentzVector* muon_pos, TLorentzVector* muon_neg, int use_lambda);

   void fillEtaPhiHistogram(TH3* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int use_lambda);
   void fillEtaHistogram(TH2* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int use_lambda, int charge=0);
   void fillHistogram(TH1* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int fill_lambda, int charge=0);

   void fillZd0EtaPhiHistogram(TH3* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, double z0_muon_p, double z0_muon_n);
   void fillZd0Histogram(TH1* hist, double z0_muon_p, double z0_muon_n, int pn);
   void correctZd0( TH1* h_corrections, TLorentzVector* muon_pos, TLorentzVector* muon_neg, double& zd0_muon_p, double& zd0_muon_n);

   void profileZwithIterativeGaussFit(TH3* hist, TH2* mu_graph, TH2* sigma_graph, int num_bins, TH2* mu_err_graph=0, TH2* sigma_err_graph=0);
   void profileYwithIterativeGaussFit(TH2* hist, TH1* mu_graph = 0, TH1* sigma_graph = 0, int num_bins = 1);

   int  IterativeGaussFit(TH1* hist, double &mu, double &mu_err, double &sigma, double &sigma_err);

   //
   void fillQoverPtHistograms(TLorentzVector* v_pos, TLorentzVector* v_neg);
   void HistogramConditioning (TH1* hist);
   void ResetHistograms ();
   void SymmetrizeHisto (TH2* hist);
    
  public:
   ZmumuValidationExample( std::list<std::string> const & s_fileNames
		       , string s_treeName = "DefaultParams"
      		       , std::string const & s_outFileName = "ZmumuValidationExampleOutput.root"
		       , bool isMC = false
      		       );
   ~ZmumuValidationExample();
   
   void loop( unsigned maxEvents = 0 );
   void SetPrintLevel (int newprintlevel=0);
   void SetEtaBins (int newbins=20);
   void SetPhiBins (int newbins=20);
   void writeToFile(int iteration);
   void DrawMap(); 
};
#endif

