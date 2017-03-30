#ifndef ZMUMUVALIDATIONEXAMPLE_h
#define ZMUMUVALIDATIONEXAMPLE_h

#include <vector>
#include <list>
#include <string>

#include "TROOT.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLorentzVector.h"
#include "TProfile2D.h"
#include "TString.h"


class ZmumuValidationExample 
{
  private:

  const double muon_mass = 105.658357;  // In MeV
  const double pion_mass = 139.57018;  // In MeV
  const double JPsi1SPDGMass = 3.096916; //PDG 2010 in GeV
  const double Upsilon1SPDGMass = 9.46030; //PDG 2010 in GeV
  const double KShortPDGMass = 0.497611; //PDG 2010 in GeV
  const double ZPDGMass = 91.1876; //PDG 2010 in GeV
  const double InnerDetectorEtaRange = 2.5; // eta range of the inner detector
  const double NumberPI = 3.141593;

  const std::list<std::string> m_fileNames;
  string m_outfilename;
  std::vector<double> m_JPsi1SMCbias; //in MeV

  TFile    m_file;
  
  TChain m_eventChain;
  TChain m_truthChain;
  
  int m_eventCurrent;
  int m_PrintLevel;
  int m_EtaBins;
  int m_PhiBins;
  bool m_ApplyBTest;
  bool m_ApplySagittaHistogramTest;
  bool m_ApplyRadialDistortion;
  bool m_ApplyTelescopeDistortion;
  bool m_ApplyEndcapExpansion;
  bool m_FitBothMuons;
  bool m_RunDeltaSagittaFitter;
  bool m_RunIPResolStudies;
  
   bool m_isMC;
   int n_iteration;        
   int m_iterationUserInput;
   double m_shift;
   //double m_JPsi1SMCbias;
   double m_factor;
   int m_sampleType;
   int m_analysisType;
   JPsiMassFit* m_JPsiMassFit;
   deltaSagittaFitter* m_deltaSagittaFitter;
   IPResolStudies* m_IPResolStudies;
 
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
  
   double m_d0_err_pos;
   double m_d0_err_neg;
   double m_z0_err_pos;
   double m_z0_err_neg;
	
   double m_z0_pos_PVerr;
   double m_z0_neg_PVerr;
   double m_d0_pos_PVerr;
   double m_d0_neg_PVerr;
 
   int m_prescale;
   int m_runNumber;
   int m_eventNumber;

   double invMass_low, invMass_high;

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

   TBranch * b_z0_pos_PVerr;
   TBranch * b_z0_neg_PVerr;
   TBranch * b_d0_pos_PVerr;
   TBranch * b_d0_neg_PVerr;


   TBranch * b_truth_px_pos;
   TBranch * b_truth_py_pos;
   TBranch * b_truth_pz_pos;
   	            
   TBranch * b_truth_px_neg;
   TBranch * b_truth_py_neg;
   TBranch * b_truth_pz_neg;	  
      
   /////////////////////
   // four momenta
   /////////////////////
   TLorentzVector* vec_pos;
   TLorentzVector* vec_neg; 
   TLorentzVector* vec_dimuon;
   TLorentzVector* vec_truth_pos;
   TLorentzVector* vec_truth_neg;

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
   TH2 * h_pt_pos_vs_neg;

   //
   // dimuon invariant mass vs etaphi of dimuon
   TH3 * h_dimuon_mass_vs_etaphi;
   TH2 * h_dimuon_mass_vs_etaphi_fitresult;
   TH2 * h_dimuon_mass_vs_etaphi_fiterror;
   TH2 * h_dimuon_mass_vs_etaphi_fitresultVsPDG;
   TH2 * h_dimuon_mass_vs_etaphi_fitresultVsPDGpc;
   TH2 * h_dimuon_mass_vs_etaphi_fitsigma;
   TH2 * h_dimuon_mass_vs_etaphi_chi2overndf;
   TH2 * h_dimuon_mass_shfit_significance_vs_etaphi;

   // dimuon invariant mass vs etaphi of positive muon
   TH3 * h_dimuon_mass_vs_etaphiPos;
   TH2 * h_dimuon_mass_vs_etaphiPos_fitresult;
   TH2 * h_dimuon_mass_vs_etaphiPos_fiterror;
   TH2 * h_dimuon_mass_vs_etaphiPos_fitresultVsPDG;
   TH2 * h_dimuon_mass_vs_etaphiPos_fitresultVsPDGpc;
   TH2 * h_dimuon_mass_vs_etaphiPos_fitsigma;
   TH2 * h_dimuon_mass_shfit_significance_vs_etaphiPos;

   // dimuon invariant mass vs etaphi of negative muon
   TH3 * h_dimuon_mass_vs_etaphiNeg;
   TH2 * h_dimuon_mass_vs_etaphiNeg_fitresult;
   TH2 * h_dimuon_mass_vs_etaphiNeg_fiterror;
   TH2 * h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG;
   TH2 * h_dimuon_mass_vs_etaphiNeg_fitresultVsPDGpc;
   TH2 * h_dimuon_mass_vs_etaphiNeg_fitsigma;
   TH2 * h_dimuon_mass_shfit_significance_vs_etaphiNeg;

   TH2 * h_entries_vs_etaphi;

   // pt of the positive and negative muon vs eta-phi
   TH3 * h_ptpos_vs_etaphi;
   TH3 * h_ptneg_vs_etaphi;
   

   // dimuon histograms
   TH1 * h_dimuon_mass;
   TH1 * h_dimuon_pt;
   TH1 * h_dimuon_pz;
   TH1 * h_dimuon_eta;
   TH1 * h_dimuon_phi;
   TH2 * h_dimuon_pt_vs_eta;
   TH2 * h_dimuon_pt_vs_mass;
   TH1 * h_dimuon_deltaphi;
   TH1 * h_dimuon_deltaphi_vs_mass;
   TH1 * h_dimuon_deltaeta;
   TH1 * h_dimuon_cosalpha;
   TH2 * h_dimuon_cosalpha_vs_mass;
   TH2 * h_dimuon_deltaz0_vs_mass;
   TH2 * h_dimuon_deltad0_vs_mass;
   TH2 * h_dimuon_deltaz0_vs_eta;
   TH2 * h_dimuon_ptlead_vs_mass;
   TH2 * h_dimuon_deltad0_vs_eta;
   TH2 * h_d0Significance_vs_mass;
   
   // dimuon DeltaM histos
   TH2 * h_dimuon_DeltaM_vs_ptlead;
   TProfile * p_dimuon_DeltaM_vs_ptlead;
   TH2F * h_mcmassbiasmap;
   // trigger prescale histograms
   TH2 * h_prescale_vs_ptlead;

   // radial distortion 
   TH1 * h_radialdistortion;
   TH2 * h_radialdistortion_vs_mass;
   TH2 * h_radialdistortion_vs_pt;
   TH3 * h_radialdistortion_vs_pt_eta;
   std::vector<TH2F*> v_radialdistortion_vs_massEta;
   std::vector<TH2F*> v_radialdistortion_vs_ptEta;
   TProfile2D * h_radialdistortion_vs_etaphi;
   TProfile * h_radialdistortion_vs_pt_prof;
   TProfile * h_radialdistortion_vs_eta_prof;

   // TProfile2D * h_radialdistortion_vs_etaphi_PreviousIterValues;
   TH2 * h_radialdistortion_vs_etaphi_Integrated;
   TH2 * h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues;
   TH2 * h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues;
   TH2 * h_radialdistortion_pos_neg;

   TProfile2D * prof_pt_vs_etaphi;

   //delta corrections p->p(1+delta) - charge symmetric
   TH3 * delta_vs_etaphi;

   TH2 * deltacorrections_vs_etaphi; 
   TH2 * deltacorrections_vs_etaphi_err; 

   TH2 * delta_vs_eta;         
   TH1 * deltacorrections_vs_eta; 
   TH1 * deltacorrections_vs_eta_RMS; 
   
   //lambda corrections p->p(1+Q*lambda*pT) - charge anti-symmetric
   TH3 * lambda_vs_etaphi;         

   TH2 * lambdacorrections_vs_etaphi;
   TH2 * lambdacorrections_vs_etaphi_err;
   TH2 * lambdacorrections_vs_etaphi_RMS;

   TH2 * lambda_vs_eta;   
   TH2 * lambda_vs_eta_pos;
   TH2 * lambda_vs_eta_neg;

   TH1 * lambdacorrections_vs_eta;
   TH1 * lambdacorrections_vs_eta_RMS;
    
   TH1 * lambda;
   TH1 * lambda_pos;
   TH1 * lambda_neg;

   TH1 * delta;
   
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

   TH2 * z0delta_vs_eta;         
   TH1 * z0deltacorrections_vs_eta;
   TH1 * z0deltacorrections_vs_eta_RMS;

   
   TH1 * z0delta;
   TH1 * z0delta_pos;
   TH1 * z0delta_neg;

   TH1 * z0delta_etaphi;
   TH1 * z0delta_etaphi_pos;
   TH1 * z0delta_etaphi_neg;


   TH3 * z0_vs_etapT;
   TH2 * z0_vs_eta;
   TH1D * z0_vs_eta_RMS;
   TH1D * z0_vs_eta_mean;
   TH1D * z0_vs_eta_chi2;
   
   TH2 * z0_vs_pT;
   TH1D * z0_vs_pT_RMS;
   TH1D * z0_vs_pT_mean;
   TH1D * z0_vs_pT_chi2;

   
   //d0 histograms
   TH3 * d0delta_vs_etaphi;         

   TH2 * d0deltacorrections_vs_etaphi;
   TH2 * d0deltacorrections_vs_etaphi_err;

   TH2 * d0delta_vs_eta;         

   TH1 * d0deltacorrections_vs_eta;
   TH1 * d0deltacorrections_vs_eta_RMS;

   
   TH1 * d0delta;
   TH1 * d0delta_pos;
   TH1 * d0delta_neg;

   TH1 * d0delta_etaphi;
   TH1 * d0delta_etaphi_pos;
   TH1 * d0delta_etaphi_neg;



   TH3 * d0_vs_etapT;
   TH2 * d0_vs_eta;
   TH1D * d0_vs_eta_RMS;
   TH1D * d0_vs_eta_mean;
   TH1D * d0_vs_eta_chi2;

   TH2 * d0_vs_pT;
   TH1D * d0_vs_pT_RMS;
   TH1D * d0_vs_pT_mean;
   TH1D * d0_vs_pT_chi2;


   
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

   // Histogram to retrieve the delta_sagitta map from deltaSgittaFitter
   TH2* h_deltaSagittaFitterMaps;

   // histogram for input delta_sagitta corrections
   TH2F* h_deltaSagittaCorrMaps;
   
   // histogram for the input radial distortions
   TH2F* h_RadialDistortion_input;
   
   ////////////////////////////
   // canvas to show fit resutls
   ////////////////////////////
   TCanvas* TempCanvasIterGaussFit;


   // strings with modes for info
   std::vector <TString> m_infoSampleType;
   std::vector <TString> m_infoAnalysisType;

   ////////////////////////////////////////////
   // more helper methods
   // 
   // these methods are used to initialize
   // histograms, perform cuts, and fill
   // histograms
   ////////////////////////////////////////////    
   void correctMomentum (TH1* h_corrections, TLorentzVector* muon_pos, TLorentzVector* muon_neg, int use_lambda);
   void defineInfoStrings ();
   bool EventSelection (int theEvent, TLorentzVector* vec_pos, TLorentzVector* vec_neg, TLorentzVector* vec_dimuon);
   void loadChains();
   void loopThroughEvents( unsigned int maxItr );
   void fillHistograms (int);
   void setBranchAddresses();
   float GetMCbiasMass (TLorentzVector* vec_pos, TLorentzVector* vec_neg);
    
   

   void fillEtaPhiHistogram(TH3* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int use_lambda);
   void fillEtaHistogram(TH2* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int use_lambda, int charge=0);
   void fillHistogram(TH1* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int fill_lambda, int charge=0);

   void fillZd0EtaPhiHistogram(TH3* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, double z0_muon_p, double z0_muon_n);
   void fillZd0EtaHistogram(TH2* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, double z0_muon_p, double z0_muon_n);
   void fillZd0Histogram(TH1* hist, double z0_muon_p, double z0_muon_n, int pn);
   void correctZd0( TH1* h_corrections, TLorentzVector* muon_pos, TLorentzVector* muon_neg, double& zd0_muon_p, double& zd0_muon_n);


   void fillEtaPtHistogram(TH3* hist, TH2* vsEta, TH2* vspT, TLorentzVector* V_pos, TLorentzVector* V_neg, double deltaTrackPar_pos, double deltaTrackPar_neg);

   
   void profileZwithIterativeGaussFit(TH3* hist, TH2* mu_graph, TH2* sigma_graph, int num_bins, TH2* mu_err_graph=0, TH2* sigma_err_graph=0);
   void profileYwithIterativeGaussFit(TH2* hist, TH1* mu_graph = 0, TH1* sigma_graph = 0, int num_bins = 1);

   void IntegratedRadialDistortions();
   int  IterativeGaussFit(TH1* hist, double &mu, double &mu_err, double &sigma, double &sigma_err);

   //
   void fillQoverPtHistograms(TLorentzVector* v_pos, TLorentzVector* v_neg);
   void HistogramConditioning (TH1* hist);
   void ResetHistograms ();
   void SymmetrizeHisto (TH2* hist);

   void FitSlices(TH2 * hist);
   
   double ComputeDeltaPhi (TLorentzVector* v_pos, TLorentzVector* v_neg);
   double ComputeDeltaPhi (TLorentzVector v_pos, TLorentzVector v_neg);
   double ComputeDeltaEta (TLorentzVector* v_pos, TLorentzVector* v_neg);

   void  RadialDistortionCorrection (TLorentzVector* v_pos, TLorentzVector* v_neg, TLorentzVector* v_dimuon);
   void  SetDistortedMomenta (TLorentzVector* vec_pos, TLorentzVector* vec_neg);
   void  SetMCbiasJPsi ();
   void  SetMCbiasUpsilon ();
   void  SetMCbiasZ ();
   
   void  WelcomeBanner ();

  public:
   ZmumuValidationExample( std::list<std::string> const & s_fileNames
		       , string s_treeName = "DefaultParams"
      		       , std::string const & s_outFileName = "ZmumuValidationExampleOutput.root"
		       , bool isMC = false
      		       );
   ~ZmumuValidationExample();
  
   void        ApplyBFieldRotationTest(bool btest) {m_ApplyBTest = btest;}
   void        ApplyEndcapExpansion (bool telescopetest) {m_ApplyEndcapExpansion = telescopetest;} 
   void        ApplyRadialDeformation(bool radialtest) {m_ApplyRadialDistortion = radialtest;} 
   void        ApplySagittaHistogramTest(bool sagittatest) {m_ApplySagittaHistogramTest = sagittatest;}   
   void        ApplyTelescopeDeformation (bool telescopetest) {m_ApplyTelescopeDistortion = telescopetest;} 
   void        bookHistograms();
   void        configurationReport ();
   void        DrawMap(); 
   void        FitPositiveAndNegativeMuon(bool fitmuons) {m_FitBothMuons = fitmuons;}
   void        loop( unsigned maxEvents = 0 );
   inline void RunDeltaSagittaFitter (bool actionValue) {m_RunDeltaSagittaFitter = actionValue; return;}
   inline void RunIPResolStudies (bool actionValue) {m_RunIPResolStudies = actionValue; return;}
   inline void SetAnalysisType (int newtype) {m_analysisType = newtype;}
   void        SetCorrectionsMaps();
   void        SetEtaBins (int newbins=20);
   void        SetPhiBins (int newbins=20);
   void        SetMCbias (std::vector <double> mcbias) {m_JPsi1SMCbias = mcbias;}
   void        SetMCbias (int);
   void        SetPrintLevel (int newprintlevel=0);
   inline void SetSampleType (int newtype) { m_sampleType = newtype;}
   inline void SetUserIteration (int iter) { m_iterationUserInput = iter;}
   void        writeToFile(int iteration);
   
   enum {ZSAMPLE, JPSISAMPLE, UPSILONSAMPLE, DIMUONSAMPLE, KSHORTSAMPLE};
   enum {ANALYSIS_ALL = 0, ANALYSIS_RADIALDIST = 1, ANALYSIS_SAGITTADIST= 2};


};
#endif

