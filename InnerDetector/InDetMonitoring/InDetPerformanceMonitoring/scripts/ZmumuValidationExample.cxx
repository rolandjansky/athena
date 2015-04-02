/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define ZMUMUVALIDATIONEXAMPLE_C

#include <iostream>
#include <list>
#include "TMath.h"
#include "TLorentzVector.h"
#include "TF1.h"

#include "ZmumuValidationExample.h"

ZmumuValidationExample::ZmumuValidationExample( std::list<std::string> const & s_fileNames, string s_treeName
                                          , std::string const & s_outFileName
					  , bool isMC
						) : m_fileNames( s_fileNames )
						  , m_outfilename( s_outFileName )
						  , m_file( s_outFileName.c_str(), "RECREATE" )
						  , m_eventChain( s_treeName.c_str() )
						  , m_truthChain( "TruthParams" )
						  , m_PrintLevel(0)
						  , m_EtaBins(1)
						  , m_PhiBins(1)
						  , m_isMC(isMC)
						  , n_iteration(0)

{
   loadChains();
   setBranchAddresses();
   // bookHistograms();
   
   m_factor = 0.5; //factor to assign lambda corrections - nominal is 0.5
   m_shift = 0; //shift in Z mass in MeV - nominal is 0
   TempCanvasIterGaussFit = NULL;
}


ZmumuValidationExample::~ZmumuValidationExample()
{
   m_file.Close();
   
}


/////////////////////////////////////////////////////////
// load TChains using the user inputed list of strings
/////////////////////////////////////////////////////////
void ZmumuValidationExample::loadChains()
{
   std::cout << "Input Files:" << std::endl;

   std::list<std::string>::const_iterator theEnd = m_fileNames.end();
   for( std::list<std::string>::const_iterator itr = m_fileNames.begin()
      ; itr != theEnd
      ; ++itr
      )
   {
      std::cout <<"  "<< *itr << std::endl;
      
      m_eventChain.Add( itr->c_str() );
      if (m_isMC) m_truthChain.Add( itr->c_str() );
   }
   
   return;
}    


//////////////////////////////////////////////////////
// sets the branch address for each leaf to be used
//////////////////////////////////////////////////////
void ZmumuValidationExample::setBranchAddresses()
{
   /////////////////////////////////////////
   // set branch addresses for event tree
   /////////////////////////////////////////
   m_eventChain.SetBranchAddress( "Positive_Px",  &m_px_pos,  &b_px_pos); 
   m_eventChain.SetBranchAddress( "Positive_Py",  &m_py_pos,  &b_py_pos);	
   m_eventChain.SetBranchAddress( "Positive_Pz",  &m_pz_pos,  &b_pz_pos);

   m_eventChain.SetBranchAddress( "Negative_Px",  &m_px_neg,  &b_px_neg); 
   m_eventChain.SetBranchAddress( "Negative_Py",  &m_py_neg,  &b_py_neg);	
   m_eventChain.SetBranchAddress( "Negative_Pz",  &m_pz_neg,  &b_pz_neg);

   m_eventChain.SetBranchAddress( "Positive_z0",  &m_z0_pos,  &b_z0_pos);
   m_eventChain.SetBranchAddress( "Positive_d0",  &m_d0_pos,  &b_d0_pos);
   
   m_eventChain.SetBranchAddress( "Negative_z0",  &m_z0_neg,  &b_z0_neg);
   m_eventChain.SetBranchAddress( "Negative_d0",  &m_d0_neg,  &b_d0_neg);

   if (m_isMC) {
   
     m_truthChain.SetBranchAddress("Positive_Px",  &m_truth_px_pos,  &b_truth_px_pos);
     m_truthChain.SetBranchAddress("Positive_Py",  &m_truth_py_pos,  &b_truth_py_pos);
     m_truthChain.SetBranchAddress("Positive_Pz",  &m_truth_pz_pos,  &b_truth_pz_pos);
						     	   	             
     m_truthChain.SetBranchAddress("Negative_Px",  &m_truth_px_neg,  &b_truth_px_neg);
     m_truthChain.SetBranchAddress("Negative_Py",  &m_truth_py_neg,  &b_truth_py_neg);
     m_truthChain.SetBranchAddress("Negative_Pz",  &m_truth_pz_neg,  &b_truth_pz_neg);
   }

   return;   
}


////////////////////////////
// initializes histograms
////////////////////////////
void ZmumuValidationExample::bookHistograms()
{
   ////////////////////////////////
   // initialize histograms here
   ////////////////////////////////
  
  const int fnEtaBins = m_EtaBins;
  const int fnPhiBins = m_PhiBins;
  const int fnQoverPtBins = 100;
  
  const double eta_bound = 2.5;
  const double phi_bound = 3.1415;
  const double p_bound = 0.5;
  const double pt_bound = 15;
  
  const double z_bound = 0.4; //mm
  const double d_bound = 0.15; //mm
  
  h_DELTA = new TH1F("h_DELTA",";#delta [GeV]",200,-50,50);
  
  h_pt = 	new TH1F("Pt","p_{T} of both #mu; p_{T} [GeV]",    100, 0., 100.);
  h_pt_pos = 	new TH1F("Pt_Pos","p_{T} of #mu^{+}; p_{T} [GeV]", 100, 0., 100.);
  h_pt_neg = 	new TH1F("Pt_Neg","p_{T} of #mu^{-}; p_{T} [GeV]", 100, 0., 100.);

  h_z0     =	new TH1F("h_z0","z_{0}: longitudinal impact param.; z_{0} [mm]",    100, -150., 150.);
  h_z0_pos =	new TH1F("h_z0_Pos","z_{0} of #mu^{+};z_{0} [mm]",100, -150., 150.);
  h_z0_neg =	new TH1F("h_z0_Neg","z_{0} of #mu^{-};z_{0} [mm]",100, -150., 150.);
  h_d0     =	new TH1F("h_d0","d_{0}: transvers. impact param.; d_{0} [mm]",    100, -0.08, 0.08);
  h_d0_pos =	new TH1F("h_d0_Pos","d_{0} of #mu^{+};d_{0} [mm]",100, -0.08, 0.08);
  h_d0_neg =	new TH1F("h_d0_Neg","d_{0} of #mu^{-};d_{0} [mm]",100, -0.08, 0.08);

  h_mass =	 new TH1F("ZMass",";Mass [GeV]",100,70,110);

//  pcorrected_mass =	 new TH1F("PCorrectedZMass",";Mass [GeV]",100,70,110);
//  lambdacorrected_mass = new TH1F("LambdaCorrectedZMass",";Mass [GeV]",100,70,110);

//  pcorrected_mass_vs_etaphi =      new TH3F("ZMassVsEtaPhi_P",";#eta;#phi;Mass [GeV]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,70,110);
//  lambdacorrected_mass_vs_etaphi = new TH3F("ZMassVsEtaPhi_Lambda",";#eta;#phi;Mass [GeV]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,70,110);  
  
//  etaphi_pos = 		new TH2F("MuonEtaPhi_Positive",";#eta;#phi", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
//  etaphi_neg = 		new TH2F("MuonEtaPhi_Negative",";#eta;#phi", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

//  prof_pt_vs_etaphi = 	new TProfile2D("AvgPtVsEtaPhi",";#eta;#phi;<p_{T}>", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  //z0 histograms ----------------------------

  entries	= new TH2F("entries","Entries per #eta-#phi bin;#eta;#phi;entries", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  
  z0delta_vs_etaphi =	new TH3F("Delta_z0_VsEtaPhi",";#eta;#phi;#delta_{z_{0}} [mm]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 50,-z_bound,z_bound);    

  z0deltacorrections_vs_etaphi =	new TH2D("z0CorrectionVsEtaPhi",";#eta;#phi;#delta_{z_{0}} [mm]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  z0deltacorrections_vs_etaphi_err = new TH2D("z0CorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{z_{0}}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);


  z0delta =	new TH1D("Delta_z0", ";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_pos =	new TH1D("Delta_z0_Pos",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_neg =	new TH1D("Delta_z0_Neg",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_etaphi =	new TH1D("Delta_z0_etaphi",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_etaphi_pos =	new TH1D("Delta_z0_etaphi_Pos",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_etaphi_neg =	new TH1D("Delta_z0_etaphi_Neg",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  //d0 Histograms------------------------------------------------------------------

  d0delta_vs_etaphi =	new TH3F("Delta_d0_VsEtaPhi",";#eta;#phi;#delta_{d_{0}} [mm]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 50,-d_bound, d_bound);

  d0deltacorrections_vs_etaphi =	new TH2D("d0CorrectionVsEtaPhi",";#eta;#phi;#delta_{d_{0}} [mm]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  d0deltacorrections_vs_etaphi_err = new TH2D("d0CorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{d_{0}}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  d0delta =	new TH1D("Delta_d0", ";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_pos =	new TH1D("Delta_d0_Pos",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_neg =	new TH1D("Delta_d0_Neg",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_etaphi =	new TH1D("Delta_d0_etaphi",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_etaphi_pos =	new TH1D("Delta_d0_etaphi_Pos",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_etaphi_neg =	new TH1D("Delta_d0_etaphi_Neg",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);
    
  //delta - charge symmetric -----------------
  delta_vs_etaphi = 				new TH3F("DeltaPVsEtaPhi",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-p_bound,p_bound);

  deltacorrections_vs_etaphi = 			new TH2D("PCorrectionVsEtaPhi",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  
  deltacorrections_vs_etaphi_err = 		new TH2D("PCorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  
  //lambda * pT - charge anti-symmetric -----------------
  lambda_vs_etaphi = 			 	new TH3F("LambdaVsEtaPhi",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-pt_bound,pt_bound);

  lambdacorrections_vs_etaphi = 		new TH2D("LambdaCorrectionVsEtaPhi",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  lambdacorrections_vs_etaphi_err = 		new TH2D("LambdaCorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  lambdacorrections_vs_etaphi_RMS =             new TH2D("LambdaCorrectionVsEtaPhi_RMS",";#eta;#phi;RMS #delta_{sagitta} [GeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  //lambda * pT - charge anti-symmetric -----------------
  lambda_vs_eta = 			 	new TH2F("LambdaVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
  lambda_vs_eta_pos = 			 	new TH2F("LambdaVsEta_Pos", ";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
  lambda_vs_eta_neg = 			 	new TH2F("LambdaVsEta_Neg", ";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);

  lambdacorrections_vs_eta  = 			new TH1D("LambdaCorrectionVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound);

  lambda =				     	new TH1D("Lambda",    ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_pos =				      	new TH1D("Lambda_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_neg =				      	new TH1D("Lambda_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

  lambda_eta =  			        new TH1D("Lambda_eta",	 ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_eta_pos =			        new TH1D("Lambda_eta_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_eta_neg =			        new TH1D("Lambda_eta_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

  lambda_etaphi =			        new TH1D("Lambda_etaphi",    ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_etaphi_pos =			        new TH1D("Lambda_etaphi_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_etaphi_neg =			        new TH1D("Lambda_etaphi_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

  if (m_isMC) {

    h_pt_truth = 			     new TH1D("Pt_Truth",     ";p_{T} [GeV]",500,0,500);
    h_pt_pos_truth =			     new TH1D("Pt_Pos_Truth",";p_{T} [GeV]",500,0,500);
    h_pt_neg_truth =			     new TH1D("Pt_Neg_Truth",";p_{T} [GeV]",500,0,500);

    delta_vs_etaphi_truth = 		     new TH3F("DeltaPVsEtaPhi_Truth",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-p_bound,p_bound);
    deltacorrections_vs_etaphi_truth = 	     new TH2D("PCorrectionVsEtaPhi_Truth",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);    
    deltacorrections_vs_etaphi_truth_err =   new TH2D("PCorrectionVsEtaPhi_Truth_Err",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);    

    lambda_vs_etaphi_truth = 		     new TH3F("LambdaVsEtaPhi_Truth",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-pt_bound,pt_bound);
    lambdacorrections_vs_etaphi_truth =      new TH2D("LambdaCorrectionVsEtaPhi_Truth",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
    lambdacorrections_vs_etaphi_truth_err =  new TH2D("LambdaCorrectionVsEtaPhi_Truth_Err",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

    lambda_vs_eta_truth =    		     new TH2F("LambdaVsEta_Truth",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
    lambdacorrections_vs_eta_truth = 	     new TH1D("LambdaCorrectionVsEta_Truth",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound);

    truth_mom_bias_vs_eta =    		     new TH2F("TruthMomentumBiasVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
    truth_mom_biascorrections_vs_eta = 	     new TH1D("TruthMomentumBiasCorrectionVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound);

    truth_mass_bias_vs_eta =		     new TH2F("TruthMassBiasVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
    truth_mass_biascorrections_vs_eta =      new TH1D("TruthMassBiasCorrectionVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound);

    lambda_truth = 			     new TH1D("Lambda_Truth",	 ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
    lambda_truth_pos = 			     new TH1D("Lambda_Truth_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
    lambda_truth_neg =			     new TH1D("Lambda_Truth_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

    delta_phi_truth = 			     new TH1D("DeltaPhi_Truth",";#phi_{Truth} - #phi_{Rec}", 100,-0.01,0.01);	       
    
    delta_eta_truth =			     new TH1D("DeltaEta_Truth",";#eta_{Truth} - #eta_{Rec}", 100,-0.01,0.01);

    delta_M2_vs_zpt_truth =		     new TH2D("delta_M2_vs_zpt_truth",";Z p_{T} [GeV]; #Delta (M^2)", 100,0,100, 100, -1.0, 1.0 );
    delta_M2_vs_zpt =		     	     new TH2D("delta_M2_vs_zpt",";Z p_{T} [GeV]; #Delta (M^2)", 100,0,100, 100, -1.0, 1.0 );

    delta_M2_vs_etaphi_pos =		     new TProfile2D("delta_M2_vs_etaphi_pos",";#eta;#phi; #Delta (M^2)", 20,-2.5,2.5, 20, -3.14, 3.14 );
    delta_M2_vs_etaphi_neg =		     new TProfile2D("delta_M2_vs_etaphi_neg",";#eta;#phi; #Delta (M^2)", 20,-2.5,2.5, 20, -3.14, 3.14 );

  }
   
  // Histograms of q/pt
  h_QoverPt = new TH2F("h_QoverPt","q/p_{T} whole ID", 2*fnPhiBins, -phi_bound, phi_bound, fnQoverPtBins, -1/pt_bound, 1/pt_bound);
  h_QoverPt->GetXaxis()->SetTitle("#phi [rad]");
  h_QoverPt->GetYaxis()->SetTitle("q/p_{T} [GeV^{-1}]");
  
  h_QoverPt3D = new TH3F("h_QoverPt3D", "q/p_{T} whole ID", 
			 fnEtaBins,      -eta_bound,  eta_bound,
			 2*fnPhiBins,    -phi_bound,  phi_bound, 
			 fnQoverPtBins, -1/pt_bound, 1/pt_bound);
  h_QoverPt3D->GetXaxis()->SetTitle("#eta");
  h_QoverPt3D->GetYaxis()->SetTitle("#phi [rad]");
  h_QoverPt3D->GetZaxis()->SetTitle("q/p_{T} [GeV^{-1}]");
}


/////////////////////////////////////////
// called by user to loop over TChains
/////////////////////////////////////////
void ZmumuValidationExample::loop( unsigned int maxEvents )
{
   std::cout << "  TrkValidation::loop()" << std::endl;
   n_iteration++;
   
   //clear histograms to be fit
   this->ResetHistograms();

   unsigned int const nEvents = m_eventChain.GetEntries();
   unsigned int const maxItr = ( maxEvents > 0 && maxEvents < nEvents ) ? maxEvents : nEvents;

   //loop through events and fill histograms
   if (m_PrintLevel >= 1) cout << " ** ZmumuValidationExample::loop ** start loop on events ... " << endl;
   loopThroughEvents( maxItr );
   
   if (m_PrintLevel >= 1) cout<< " ** ZmumuValidationExample::loop ** Fitting histograms" << endl;
   
   //Fit corrections and add to previous corrections
   profileZwithIterativeGaussFit(lambda_vs_etaphi,lambdacorrections_vs_etaphi, lambdacorrections_vs_etaphi_RMS, 1, lambdacorrections_vs_etaphi_err);
   profileZwithIterativeGaussFit(delta_vs_etaphi,deltacorrections_vs_etaphi, 0, 1,deltacorrections_vs_etaphi_err);
   profileYwithIterativeGaussFit(lambda_vs_eta,lambdacorrections_vs_eta);
   
   profileZwithIterativeGaussFit(z0delta_vs_etaphi,z0deltacorrections_vs_etaphi, 0, 1, z0deltacorrections_vs_etaphi_err);
   profileZwithIterativeGaussFit(d0delta_vs_etaphi,d0deltacorrections_vs_etaphi, 0, 1, d0deltacorrections_vs_etaphi_err);
   
   if (m_isMC && n_iteration == 1) {  
     profileZwithIterativeGaussFit(lambda_vs_etaphi_truth,  lambdacorrections_vs_etaphi_truth, 0, 1, lambdacorrections_vs_etaphi_truth_err);
     profileZwithIterativeGaussFit(delta_vs_etaphi_truth,  deltacorrections_vs_etaphi_truth, 0, 1, deltacorrections_vs_etaphi_truth_err);
     
     profileYwithIterativeGaussFit(lambda_vs_eta_truth, lambdacorrections_vs_eta_truth);     
     profileYwithIterativeGaussFit(truth_mom_bias_vs_eta, truth_mom_biascorrections_vs_eta);     
     profileYwithIterativeGaussFit(truth_mass_bias_vs_eta, truth_mass_biascorrections_vs_eta);     
   }

  writeToFile(n_iteration);
  
  return;
}


//////////////////////////////////////////////////////////////////////////
// loops over each event and calls fillHistograms() if there are tracks 
//////////////////////////////////////////////////////////////////////////
void ZmumuValidationExample::loopThroughEvents( unsigned int maxItr )
{
   std::cout << "  Looping over " << maxItr << " events"<< std::endl;
       
   for( unsigned int eventItr = 0
      ; eventItr != maxItr
      ; ++eventItr
      )
   {
      if( eventItr % 10000 == 0 )
      {
	 std::cout << "  Processing event " << eventItr << " of " << maxItr << std::endl;
      }
        
      if( !m_eventChain.LoadTree(eventItr) || (m_isMC && !m_truthChain.LoadTree(eventItr)) ) 
	 continue;
         
      if( !m_eventChain.GetEntry(eventItr) || (m_isMC && !m_truthChain.GetEntry(eventItr)) )
	 continue;
      	 
      fillHistograms();
   }
   
   return;
}

//////////////////////////////////////////////////////////////////////////
void ZmumuValidationExample::fillHistograms()
{
  
  const double muon_mass = 105.658;  //MeV
  
  //create lorentz vectors for both muons
  TLorentzVector* vec_pos = new TLorentzVector(); 
  TLorentzVector* vec_neg = new TLorentzVector();

  //corrected z0 for both muons
  double corrected_z0_pos;
  double corrected_z0_neg;
  double corrected_d0_pos;
  double corrected_d0_neg;

  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, muon_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, muon_mass);
  
  entries->Fill(vec_pos->Eta(),vec_pos->Phi(),1);
  entries->Fill(vec_neg->Eta(),vec_neg->Phi(),1);
  
  fillHistogram( lambda,      vec_pos,  vec_neg,  1);
  fillHistogram( lambda_pos,  vec_pos,  vec_neg,  1, +1);
  fillHistogram( lambda_neg,  vec_pos,  vec_neg,  1, -1);
  
  //z0 etaphi
  if ( m_d0_neg != 0  &&  m_d0_pos != 0){
     // setting original z0values
     corrected_z0_pos = m_z0_pos;
     corrected_z0_neg = m_z0_neg;
     // filling test histograms before correction
     fillZd0Histogram( z0delta, corrected_z0_pos, corrected_z0_neg, 0);
     fillZd0Histogram( z0delta_pos, corrected_z0_pos, corrected_z0_neg, 1);  
     fillZd0Histogram( z0delta_neg, corrected_z0_pos, corrected_z0_neg, -1);
     // adding correction to z0_pos/neg or d0_pos/neg
     correctZd0(z0deltacorrections_vs_etaphi, vec_pos, vec_neg, corrected_z0_pos, corrected_z0_neg);
	 // filling the main histogram to fit in the next step
     fillZd0EtaPhiHistogram(z0delta_vs_etaphi, vec_pos, vec_neg, corrected_z0_pos, corrected_z0_neg);
     // filling test histograms after correction
     fillZd0Histogram( z0delta_etaphi, corrected_z0_pos, corrected_z0_neg, 0);
     fillZd0Histogram( z0delta_etaphi_pos, corrected_z0_pos, corrected_z0_neg, 1);
     fillZd0Histogram( z0delta_etaphi_neg, corrected_z0_pos, corrected_z0_neg, -1);
  }

  //d0 etaphi
  if ( m_d0_neg != 0  &&  m_d0_pos != 0){
     // setting original d0values
     corrected_d0_pos = m_d0_pos;
     corrected_d0_neg = m_d0_neg;
     // filling test histograms before correction
     fillZd0Histogram( d0delta, corrected_d0_pos, corrected_d0_neg, 0);
     fillZd0Histogram( d0delta_pos, corrected_d0_pos, corrected_d0_neg, 1);
     fillZd0Histogram( d0delta_neg, corrected_d0_pos, corrected_d0_neg, -1);
     // adding correction to z0_pos/neg or d0_pos/neg
     correctZd0(d0deltacorrections_vs_etaphi, vec_pos, vec_neg, corrected_d0_pos, corrected_d0_neg);
	 // filling the main histogram to fit in the next step
     fillZd0EtaPhiHistogram(d0delta_vs_etaphi, vec_pos, vec_neg, corrected_d0_pos, corrected_d0_neg);
     // filling test histograms after correction
     fillZd0Histogram( d0delta_etaphi, corrected_d0_pos, corrected_d0_neg, 0);
     fillZd0Histogram( d0delta_etaphi_pos, corrected_d0_pos, corrected_d0_neg, 1);
     fillZd0Histogram( d0delta_etaphi_neg, corrected_d0_pos, corrected_d0_neg, -1);
  }
      
  //fill truth histograms on first iteration without correcting momentum
  if (m_isMC && n_iteration == 1) {  

    TLorentzVector* vec_truth_pos = new TLorentzVector();
    TLorentzVector* vec_truth_neg = new TLorentzVector();

    vec_truth_pos->SetXYZM(m_truth_px_pos, m_truth_py_pos, m_truth_pz_pos, muon_mass);
    vec_truth_neg->SetXYZM(m_truth_px_neg, m_truth_py_neg, m_truth_pz_neg, muon_mass);

    double z_mass = 91187.6;   //MeV
    double mass = ((*vec_pos) + (*vec_neg)).M();
    double mass_truth = ((*vec_truth_pos) + (*vec_truth_neg)).M();

    double delta_M2 = (mass*mass - z_mass*z_mass)/(z_mass*z_mass);
    double delta_M2_truth = (mass*mass - mass_truth*mass_truth)/(mass_truth*mass_truth);
    double delta_M2_truth_bias = (mass_truth*mass_truth - z_mass*z_mass)/(z_mass*z_mass);
 
    h_pt_truth->Fill(vec_truth_pos->Pt()/1000.0);
    h_pt_truth->Fill(vec_truth_neg->Pt()/1000.0);
    h_pt_pos_truth->Fill(vec_truth_pos->Pt()/1000.0);
    h_pt_neg_truth->Fill(vec_truth_neg->Pt()/1000.0);
    
    //truth momentum bias
    double d_bias_truth_pos = +1/vec_truth_pos->Pt()*(1 - vec_truth_pos->P()/vec_pos->P())*1000000.0;
    double d_bias_truth_neg = -1/vec_truth_neg->Pt()*(1 - vec_truth_neg->P()/vec_neg->P())*1000000.0;

    truth_mom_bias_vs_eta->Fill(vec_truth_pos->Eta(), d_bias_truth_pos);
    truth_mom_bias_vs_eta->Fill(vec_truth_neg->Eta(), d_bias_truth_neg);

    //Z mass/method bias
    double d_mass_bias_truth_pos = +1*delta_M2_truth_bias/vec_truth_pos->Pt()*1000000.0;
    double d_mass_bias_truth_neg = -1*delta_M2_truth_bias/vec_truth_neg->Pt()*1000000.0;

    truth_mass_bias_vs_eta->Fill(vec_truth_pos->Eta(), d_mass_bias_truth_pos);
    truth_mass_bias_vs_eta->Fill(vec_truth_neg->Eta(), d_mass_bias_truth_neg);

    //lambda corrections
    double d_lambda_truth_pos = +1*delta_M2_truth/vec_truth_pos->Pt()*1000000.0;
    double d_lambda_truth_neg = -1*delta_M2_truth/vec_truth_neg->Pt()*1000000.0;
      
    lambda_vs_etaphi_truth->Fill(vec_truth_pos->Eta(), vec_truth_pos->Phi(), d_lambda_truth_pos);
    lambda_vs_etaphi_truth->Fill(vec_truth_neg->Eta(), vec_truth_neg->Phi(), d_lambda_truth_neg);

    lambda_vs_eta_truth->Fill(vec_truth_pos->Eta(), d_lambda_truth_pos);
    lambda_vs_eta_truth->Fill(vec_truth_neg->Eta(), d_lambda_truth_neg);

    lambda_truth->Fill( d_lambda_truth_pos );
    lambda_truth->Fill( d_lambda_truth_neg );
    
    lambda_truth_pos->Fill( d_lambda_truth_pos );
    lambda_truth_neg->Fill( d_lambda_truth_neg );
    
    //delta corrections
    double delta_truth_pos =  (vec_truth_pos->P() - vec_pos->P())/vec_truth_pos->P();
    double delta_truth_neg =  (vec_truth_neg->P() - vec_neg->P())/vec_truth_neg->P();
    
    delta_vs_etaphi_truth->Fill(vec_truth_pos->Eta(), vec_truth_pos->Phi(), delta_truth_pos);
    delta_vs_etaphi_truth->Fill(vec_truth_neg->Eta(), vec_truth_neg->Phi(), delta_truth_neg);

    //check eta/phi biases
    delta_phi_truth->Fill(vec_truth_pos->Phi() - vec_pos->Phi());
    delta_phi_truth->Fill(vec_truth_neg->Phi() - vec_neg->Phi());

    delta_eta_truth->Fill(vec_truth_pos->Eta() - vec_pos->Eta());
    delta_eta_truth->Fill(vec_truth_neg->Eta() - vec_neg->Eta());

    //other checks
    double zpt_truth = ((*vec_truth_pos) + (*vec_truth_neg)).Pt();
    
    h_DELTA->Fill(delta_M2/(vec_pos->Pt() - vec_neg->Pt())*1000000);
    //h_DELTA->Fill(delta_M2/(vec_truth_pos->Pt() - vec_truth_neg->Pt())*1000000);
            
    delta_M2_vs_zpt_truth->Fill(zpt_truth/1000.0,delta_M2_truth); 
    delta_M2_vs_zpt->Fill(zpt_truth/1000.0,delta_M2); 
  
    delta_M2_vs_etaphi_pos->Fill(vec_pos->Eta(), vec_pos->Phi(), delta_M2);
    delta_M2_vs_etaphi_neg->Fill(vec_neg->Eta(), vec_neg->Phi(), delta_M2);

    delete vec_truth_pos;
    delete vec_truth_neg;

  } // end if of MC and truth info

  //-----------------------------------------------------------------
  //apply lambda momentum corrections in eta/phi
  correctMomentum( lambdacorrections_vs_etaphi, vec_pos, vec_neg, 1);

  fillHistogram( lambda_etaphi,      vec_pos,  vec_neg,  1);
  fillHistogram( lambda_etaphi_pos,  vec_pos,  vec_neg,  1, +1);
  fillHistogram( lambda_etaphi_neg,  vec_pos,  vec_neg,  1, -1);

  //fill histograms
  fillEtaPhiHistogram( lambda_vs_etaphi, vec_pos, vec_neg, 1);

  // fill q/pt histograms
  fillQoverPtHistograms (vec_pos, vec_neg);

  //-----------------------------------------------------------------

  //reset muon vectors and use original values
  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, muon_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, muon_mass);
  
  //apply delta momentum corrections in eta/phi
  correctMomentum( deltacorrections_vs_etaphi,  vec_pos, vec_neg, 0);
  //fill histograms
  fillEtaPhiHistogram( delta_vs_etaphi, vec_pos, vec_neg, 0);

  //-----------------------------------------------------------------

  //reset muon vectors and use original values
  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, muon_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, muon_mass);

  //apply lambda momentum corrections in eta
  correctMomentum( lambdacorrections_vs_eta,  vec_pos, vec_neg, 1);

  fillHistogram( lambda_eta,	  vec_pos,  vec_neg,  1);
  fillHistogram( lambda_eta_pos,  vec_pos,  vec_neg,  1, +1);
  fillHistogram( lambda_eta_neg,  vec_pos,  vec_neg,  1, -1);
   
  //fill histograms
  fillEtaHistogram( lambda_vs_eta,      vec_pos,  vec_neg,  1);  
  fillEtaHistogram( lambda_vs_eta_pos,  vec_pos,  vec_neg,  1, +1);	  
  fillEtaHistogram( lambda_vs_eta_neg,  vec_pos,  vec_neg,  1, -1);	  
  
  //check corrected pt/mass distributions at each iteration
  h_pt->Fill(vec_pos->Pt()/1000.0);
  h_pt->Fill(vec_neg->Pt()/1000.0);
  h_pt_pos->Fill(vec_pos->Pt()/1000.0);
  h_pt_neg->Fill(vec_neg->Pt()/1000.0);
   
  h_mass->Fill(((*vec_pos)+(*vec_neg)).M()/1000.0);
  
  //check z0 distributions
  h_z0->Fill(m_z0_pos);
  h_z0->Fill(m_z0_neg);
  h_z0_pos->Fill(m_z0_pos);
  h_z0_neg->Fill(m_z0_neg);
  //check d0 distributions
  h_d0->Fill(m_d0_pos);
  h_d0->Fill(m_d0_neg);
  h_d0_pos->Fill(m_d0_pos);
  h_d0_neg->Fill(m_d0_neg);
  //-----------------------------------------------------------------

  delete vec_pos;
  delete vec_neg;
   
  return;
}

//--------------------------------------------------------------------------------------------------
void ZmumuValidationExample::correctMomentum( TH1* h_corrections, TLorentzVector* muon_pos, TLorentzVector* muon_neg, int use_lambda)
{   
  double temp_pt_pos = muon_pos->Pt();
  double temp_pt_neg = muon_neg->Pt();

  // positive muon
  double correction = h_corrections->GetBinContent(h_corrections->FindBin(muon_pos->Eta(), muon_pos->Phi()));
  if (use_lambda) {
    
    double pt_true = muon_pos->Pt();
    //double pt_true = muon_pos->Pt()/(1+(+1)*correction*muon_pos->Pt()/1000000.0);

    muon_pos->SetXYZM(muon_pos->Px()/(1+correction*pt_true/1000000.0),
  		      muon_pos->Py()/(1+correction*pt_true/1000000.0),
		      muon_pos->Pz()/(1+correction*pt_true/1000000.0),
		      muon_pos->M());    
    if (m_PrintLevel >= 3) {
      cout << " ** correctMomentum ** mu+ (eta,phi) (" << muon_pos->Eta() << ", " << muon_pos->Phi() << ") " 
	   << " bin = " << h_corrections->FindBin(muon_pos->Eta(), muon_pos->Phi())
	   << " correction = " << correction 
	   << endl;
    }
  } 
  else {
    // mainly for delta corrections
    muon_pos->SetXYZM(muon_pos->Px()*(1-correction),
  		      muon_pos->Py()*(1-correction),
		      muon_pos->Pz()*(1-correction),
		      muon_pos->M());    
  }
  
  // negative muon
  correction = h_corrections->GetBinContent(h_corrections->FindBin(muon_neg->Eta(), muon_neg->Phi()));
  if (use_lambda) {

    double pt_true = muon_neg->Pt();
    //double pt_true = muon_neg->Pt()/(1+(+1)*correction*muon_neg->Pt()/1000000.0);

    muon_neg->SetXYZM(muon_neg->Px()/(1-correction*pt_true/1000000.0),
  		      muon_neg->Py()/(1-correction*pt_true/1000000.0),
		      muon_neg->Pz()/(1-correction*pt_true/1000000.0),
		      muon_neg->M());    
    if (m_PrintLevel >= 3) {
      cout << " ** correctMomentum ** mu- (eta,phi) (" << muon_neg->Eta() << ", " << muon_neg->Phi() << ") " 
	   << " bin = " << h_corrections->FindBin(muon_neg->Eta(), muon_neg->Phi())
	   << " correction = " << correction 
	   << endl;
    }
  } 
  else {
    // mainly for delta corrections
    muon_neg->SetXYZM(muon_neg->Px()*(1-correction),
  		      muon_neg->Py()*(1-correction),
		      muon_neg->Pz()*(1-correction),
		      muon_neg->M());    
  }

  if (m_PrintLevel >= 3) {
    cout << " ** correctMomentum ** mu+ Pt: " << temp_pt_pos << " --> " << muon_pos->Pt() << " Delta = " << 100*(muon_pos->Pt()-temp_pt_pos)/temp_pt_pos << " %"<< endl
	 << "                       mu- Pt: " << temp_pt_neg << " --> " << muon_neg->Pt() << " Delta = " << 100*(muon_neg->Pt()-temp_pt_neg)/temp_pt_neg << " %"<< endl;
  }


  return;
}


//-------------------------------------------------------------------------------
void ZmumuValidationExample::fillEtaPhiHistogram(TH3* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int use_lambda)
{
   double z_mass = 91187.6 + m_shift;   //MeV
   double mass = ((*v_pos) + (*v_neg)).M();
   double delta_M2 = (mass*mass - z_mass*z_mass)/(z_mass*z_mass);

   if (use_lambda) { 
     // to estimate the correction factor from: pt -> pt /(1+q pt delta_sagitta): q DeltaM2/pt
     hist->Fill(v_pos->Eta(), v_pos->Phi(), +1*m_factor*delta_M2/v_pos->Pt()*1000000.0);     
     hist->Fill(v_neg->Eta(), v_neg->Phi(), -1*m_factor*delta_M2/v_neg->Pt()*1000000.0);
     
   } else { 
     hist->Fill(v_pos->Eta(), v_pos->Phi(), +1*delta_M2/2);
     hist->Fill(v_neg->Eta(), v_neg->Phi(), +1*delta_M2/2);   
   }
}


//-------------------------------------------------------------------------------
void ZmumuValidationExample::fillEtaHistogram(TH2* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int use_lambda, int charge)
{
   double z_mass = 91187.6 + m_shift;   //MeV
   double mass = ((*v_pos) + (*v_neg)).M();
   double delta_M2 = (mass*mass - z_mass*z_mass)/(z_mass*z_mass);
   
   if (use_lambda) {
     if (charge == 0 || charge == +1) hist->Fill(v_pos->Eta(), +1*m_factor*delta_M2/v_pos->Pt()*1000000.0 );
     if (charge == 0 || charge == -1) hist->Fill(v_neg->Eta(), -1*m_factor*delta_M2/v_neg->Pt()*1000000.0 );
     
   } else { 
     hist->Fill(v_pos->Eta(), +1*delta_M2/2);
     hist->Fill(v_neg->Eta(), +1*delta_M2/2);   
   }
}

//-----------------------------------------------------------------------------------------------
void ZmumuValidationExample::fillHistogram(TH1* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, int fill_lambda, int charge)
{
   double z_mass = 91187.6 + m_shift;   //MeV
   double mass = ((*v_pos) + (*v_neg)).M();
   double delta_M2 = (mass*mass - z_mass*z_mass)/(z_mass*z_mass);
   
   if (fill_lambda) {
     if (charge == 0 || charge == +1) hist->Fill( +1*m_factor*delta_M2/v_pos->Pt()*1000000.0 );
     if (charge == 0 || charge == -1) hist->Fill( -1*m_factor*delta_M2/v_neg->Pt()*1000000.0 );
     
   } else { 
     hist->Fill(+1*delta_M2/2);
     hist->Fill(+1*delta_M2/2);   
   }
}

//-----------------------------------------------------------------------------------------------
void ZmumuValidationExample::fillQoverPtHistograms(TLorentzVector* v_pos, TLorentzVector* v_neg)
{
  h_QoverPt->Fill((*v_pos).Phi(),  1000/(*v_pos).Pt());  // 1000 is for MeV to GeV
  h_QoverPt->Fill((*v_neg).Phi(), -1000/(*v_neg).Pt());

  h_QoverPt3D->Fill((*v_pos).Eta(), (*v_pos).Phi(),  1000/(*v_pos).Pt());  // 1000 is for MeV to GeV
  h_QoverPt3D->Fill((*v_neg).Eta(), (*v_neg).Phi(), -1000/(*v_neg).Pt());
  return;
}
// impact parameter biases methods------------------------------------------------------------------
void ZmumuValidationExample::correctZd0( TH1* h_corrections, TLorentzVector* muon_pos, TLorentzVector* muon_neg, double& zd0_muon_p, double& zd0_muon_n)
{   
  double correction = h_corrections->GetBinContent(h_corrections->FindBin(muon_pos->Eta(), muon_pos->Phi()));
//  std::cout << "Pos Cor" << correction << std::endl; 

	zd0_muon_p = zd0_muon_p + correction ;

  correction = h_corrections->GetBinContent(h_corrections->FindBin(muon_neg->Eta(), muon_neg->Phi()));
	zd0_muon_n = zd0_muon_n + correction ;
//  std::cout << "Neg Cor" << correction << std::endl; 
    
}

void ZmumuValidationExample::fillZd0EtaPhiHistogram(TH3* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, double zd0_muon_p, double zd0_muon_n)
{
   double deltazd0 = -(zd0_muon_p - zd0_muon_n);
   hist->Fill(v_pos->Eta(),v_pos->Phi(), deltazd0);
   hist->Fill(v_neg->Eta(),v_neg->Phi(), -1.*deltazd0);
}

void ZmumuValidationExample::fillZd0Histogram(TH1* hist, double zd0_muon_p, double zd0_muon_n, int pn)
{
    double deltazd0 = (zd0_muon_p - zd0_muon_n);
	if (pn == +1) hist->Fill(deltazd0);
	if (pn == -1) hist->Fill(-1.*deltazd0);
	if (pn == 0) {
		hist->Fill(-1.*deltazd0);
		hist->Fill(deltazd0);
	}
}
// impact parameter biases methods------------------------------------------------------------------

///////////////////////////////////
// saves root objects to a TFile
///////////////////////////////////
void ZmumuValidationExample::writeToFile(int iteration)
{

  m_file.cd(); 

  if (m_isMC && iteration == 1) {

    h_pt_truth->Write(); 
    h_pt_pos_truth->Write(); 
    h_pt_neg_truth->Write(); 
  
    deltacorrections_vs_etaphi_truth->Write();
    deltacorrections_vs_etaphi_truth_err->Write();

    lambdacorrections_vs_etaphi_truth->Write();
    lambdacorrections_vs_etaphi_truth_err->Write();
    
    lambdacorrections_vs_eta_truth->Write();
    lambda_vs_eta_truth->Write();
      
    truth_mom_biascorrections_vs_eta->Write();
    truth_mom_bias_vs_eta->Write();  

    truth_mass_biascorrections_vs_eta->Write();
    truth_mass_bias_vs_eta->Write();   

    lambda_truth->Write();
    lambda_truth_pos->Write();
    lambda_truth_neg->Write();
    
    delta_phi_truth->Write();
    delta_eta_truth->Write();
    
    delta_M2_vs_zpt_truth->Write();
    delta_M2_vs_zpt->Write();
    
    delta_M2_vs_etaphi_pos->Write();
    delta_M2_vs_etaphi_neg->Write();

  }
  
  if (iteration > 0) {
    m_file.mkdir(Form("Iteration%i",iteration));
    m_file.cd(Form("Iteration%i",iteration));
  }

  h_pt->Write(); 
  h_pt_pos->Write(); 
  h_pt_neg->Write(); 
  h_mass->Write();
  
  h_z0->Write();
  h_z0_pos->Write();
  h_z0_neg->Write();
  h_d0->Write();
  h_d0_pos->Write();
  h_d0_neg->Write();

  deltacorrections_vs_etaphi->Write(); 
  deltacorrections_vs_etaphi_err->Write(); 

  entries->Write();
  
  lambdacorrections_vs_etaphi->Write();
  lambdacorrections_vs_etaphi_err->Write();
  lambdacorrections_vs_etaphi_RMS->Write();
  lambdacorrections_vs_eta->Write();
  
  lambda_vs_eta->Write();
  lambda_vs_etaphi->Write();
  
  lambda_vs_eta_pos->Write();
  lambda_vs_eta_neg->Write();
  
  lambda->Write();   
  lambda_pos->Write();
  lambda_neg->Write();
  
  lambda_eta->Write();
  lambda_eta_pos->Write();
  lambda_eta_neg->Write();

  lambda_etaphi->Write();
  lambda_etaphi_pos->Write();
  lambda_etaphi_neg->Write();
  
  z0delta_vs_etaphi->Write();
  z0deltacorrections_vs_etaphi->Write();
  z0deltacorrections_vs_etaphi_err->Write();

  z0delta->Write();
  z0delta_pos->Write();
  z0delta_neg->Write();


  z0delta_etaphi->Write();
  z0delta_etaphi_pos->Write();
  z0delta_etaphi_neg->Write();

  d0delta_vs_etaphi->Write();
  d0deltacorrections_vs_etaphi->Write();
  d0deltacorrections_vs_etaphi_err->Write();

  d0delta->Write();
  d0delta_pos->Write();
  d0delta_neg->Write();

  d0delta_etaphi->Write();
  d0delta_etaphi_pos->Write();
  d0delta_etaphi_neg->Write();
  
  //cout<< "MEAN: " <<h_DELTA->GetMean()<<endl;
  h_DELTA->Write();

  //
  h_QoverPt->Write();
  h_QoverPt3D->Write();
  // delta_vs_etaphi->Write();
}


//-------------------------------------------------------------
void ZmumuValidationExample::profileZwithIterativeGaussFit(TH3* hist, TH2* mu_graph, TH2* sigma_graph, int num_bins, TH2* mu_err_graph, TH2* sigma_err_graph)
{
  if (!hist) {
    cout<< "ProfileZwithIterativeGaussFit(): No histogram supplied!"<<endl;
    return;
  }

  int minEntries = 50;
  int fDebug = 0;

  int num_bins_x = hist->GetXaxis()->GetNbins();
  int num_bins_y = hist->GetYaxis()->GetNbins();

  double num_not_converged = 0;
  int num_skipped = 0;

  double max_sigma = 0;
  double min_sigma = 0;

  double max_mu = 0;
  double min_mu = 0;

  TH1D* current_proj;

  for (int i = 1; i < num_bins_x+(num_bins==1); i+=num_bins) {

  for (int j = 1; j < num_bins_y+(num_bins==1); j+=num_bins) {

    int index = i/num_bins;
    int index_y = j/num_bins;

    current_proj = hist->ProjectionZ(Form("%s_GaussProjection_%i_%i",hist->GetName(),index, index_y),i,i+num_bins-1,j,j+num_bins-1);
    current_proj->SetTitle(Form("%s - Bin %i x %i",hist->GetName(), index,index_y));

    double current_mu,current_err_mu, current_sigma, current_err_sigma;

    if(current_proj->GetEntries() < minEntries) {
      if (m_PrintLevel >= 1) cout << " ** profileZwithIterativeGaussFit ** fitting " << hist->GetName() << "  bin (" << index << ", " << index_y << ") "
				  << " Not enough entries " << current_proj->GetEntries() << " < " << minEntries << endl;
       //current_mu = -999;
       current_mu = 0;
       current_sigma = 0;
       current_err_mu = 1;
       current_err_sigma = 1;
       
       if (fDebug) std::cout<<"WARNING: Only "<<current_proj->GetEntries()<<" entries in bin "<<index<<","<<index_y<< " in histogram " <<hist->GetName()<< std::endl;
       num_skipped++;

    } else {
      if (m_PrintLevel >= 1) cout << " ** profileZwithIterativeGaussFit ** fitting " << hist->GetName() << "  bin (" << index << ", " << index_y << ") " << endl;
      
      IterativeGaussFit(current_proj, current_mu, current_err_mu, current_sigma, current_err_sigma);

      if (current_sigma > max_sigma || max_sigma == 0) max_sigma = current_sigma;
      if (current_sigma < min_sigma || min_sigma == 0) min_sigma = current_sigma;
      if (current_mu > max_mu || max_mu == 0) max_mu = current_mu;
      if (current_mu < min_mu || min_mu == 0) min_mu = current_mu;

    }//end if entries < minEntries

    float x_coord = (hist->GetXaxis()->GetBinLowEdge(i) + hist->GetXaxis()->GetBinUpEdge(i+num_bins-1))/2;
    float y_coord = (hist->GetYaxis()->GetBinLowEdge(j) + hist->GetYaxis()->GetBinUpEdge(j+num_bins-1))/2;

    if (sigma_graph) sigma_graph->Fill(x_coord,y_coord,current_sigma);
    if (mu_graph) mu_graph->Fill(x_coord,y_coord,current_mu);
        
    //should probably be replace bin content, not fill?
    if (sigma_err_graph) sigma_err_graph->Fill(x_coord,y_coord,current_err_sigma);
    if (mu_err_graph) mu_err_graph->Fill(x_coord,y_coord,current_err_mu);

    delete current_proj;
    
  } //end loop on j (y)
  } //end loop on i (x)
  
  if (mu_graph) {
    mu_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    mu_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    mu_graph->GetYaxis()->SetTitleOffset(1);
    mu_graph->GetZaxis()->SetTitle(hist->GetZaxis()->GetTitle());
    mu_graph->GetZaxis()->SetTitleOffset(1.2);
    mu_graph->SetTitle( "" );
    //mu_graph->SetMaximum(max_mu + 0.1* (max_mu-min_mu));
    //mu_graph->SetMinimum(min_mu - 0.1* (max_mu-min_mu));
  }
  
  if (sigma_graph) {
    sigma_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    sigma_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    sigma_graph->GetYaxis()->SetTitleOffset(1);
    sigma_graph->GetZaxis()->SetTitle(hist->GetZaxis()->GetTitle());
    sigma_graph->GetZaxis()->SetTitleOffset(1.2);
    sigma_graph->SetTitle( "" );
    //sigma_graph->SetMaximum(max_sigma + 0.1* (max_sigma-min_sigma));
    //sigma_graph->SetMinimum(min_sigma - 0.1* (max_sigma-min_sigma));
  }

  if (mu_err_graph) {
    mu_err_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    mu_err_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    mu_err_graph->GetYaxis()->SetTitleOffset(1);
    mu_err_graph->GetZaxis()->SetTitle(Form("Error of fit #mu: %s",hist->GetZaxis()->GetTitle()));
    mu_err_graph->GetZaxis()->SetTitleOffset(1.2);
    mu_err_graph->SetTitle(hist->GetTitle());
    //mu_err_graph->SetMaximum(max_mu + 0.1* (max_mu-min_mu));
    //mu_err_graph->SetMinimum(min_mu - 0.1* (max_mu-min_mu));
  }

  if (sigma_err_graph) {
    sigma_err_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    sigma_err_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    sigma_err_graph->GetYaxis()->SetTitleOffset(1);
    sigma_err_graph->GetZaxis()->SetTitle(Form("Error of fit #sigma: %s",hist->GetZaxis()->GetTitle()));
    sigma_err_graph->GetZaxis()->SetTitleOffset(1.2);
    sigma_err_graph->SetTitle(hist->GetTitle());
    //sigma_err_graph->SetMaximum(max_mu + 0.1* (max_mu-min_mu));
    //sigma_err_graph->SetMinimum(min_mu - 0.1* (max_mu-min_mu));
  }
  

  if (num_not_converged || num_skipped) std::cout<<"Fit Results for histogram: "<< hist->GetName()<<std::endl;
  if (num_not_converged) std::cout<<"Non Convergent Bin Fraction: "<<num_not_converged<< " / " <<num_bins_x*num_bins_y - num_skipped<<std::endl;
  if (num_skipped) std::cout<<"Number skipped bins: "<<num_skipped<< " / " <<num_bins_x*num_bins_y<<std::endl;

  return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::profileYwithIterativeGaussFit(TH2* hist, TH1* mu_graph, TH1* sigma_graph, int num_bins)
{

  if (!hist) {
    std::cout << "Error in ProfileYwithIterativeGaussFit(): Histogram not found" <<std::endl;
    return;
  }

  if (num_bins < 1 ) {
    std::cout << "Error in ProfileYwithIterativeGaussFit(): Invalid number of bins to integrate over." <<std::endl;
    return;
  }

  const int minEntries = 50;
  const int fDebug = 0; 

  int num_bins_x = hist->GetXaxis()->GetNbins();
  
  if (mu_graph) mu_graph->Rebin(num_bins);
  if (sigma_graph) sigma_graph->Rebin(num_bins);
  
  double* errs_mu = new double[num_bins_x/num_bins + 2]; // +2 for overflow!! 
  double* errs_sigma = new double[num_bins_x/num_bins + 2];

  errs_mu[0] = 0;
  errs_mu[num_bins_x/num_bins + 1] = 0;

  errs_sigma[0] = 0;
  errs_sigma[num_bins_x/num_bins + 1] = 0;

  double min_sigma = 0;
  double max_sigma = 0; 
  double min_mu = 0;
  double max_mu = 0;

  int num_skipped = 0;

  TH1D* current_proj;

  for (int i = 1; i < (num_bins_x + (num_bins == 1)); i+=num_bins) {

    int index = i/num_bins;
    if (num_bins == 1) index--;

    current_proj = hist->ProjectionY(Form("%s_projection_%i",hist->GetName(),index),i,i+num_bins-1);

    double mu, mu_err, sigma, sigma_err;

    if(current_proj->GetEntries() < minEntries) {
      mu = 0;
      mu_err = 0;
      sigma = 0;
      sigma_err = 0;
      num_skipped++;
      if ( fDebug ) std::cout<<"WARNING: Not enough entries in bin "<<index<<std::endl;
    } else {
    
      IterativeGaussFit(current_proj, mu, mu_err, sigma, sigma_err);

      if (sigma > max_sigma || max_sigma == 0) max_sigma = sigma;
      if (sigma < min_sigma || min_sigma == 0) min_sigma = sigma;
      if (mu > max_mu || max_mu == 0) max_mu = mu;
      if (mu < min_mu || min_mu == 0) min_mu = mu;

    }
    
    double value_x = (hist->GetXaxis()->GetBinLowEdge(i) + hist->GetXaxis()->GetBinUpEdge(i+num_bins-1))/2;

    //Important!! Use Fill to increment the graph with each iteration, or SetBinContent to replace contents...

    //if (sigma_graph) sigma_graph->SetBinContent(i, sigma);
    //if (mu_graph) mu_graph->SetBinContent(i, mu);

    if (sigma_graph) sigma_graph->Fill(value_x, sigma);
    if (mu_graph) mu_graph->Fill(value_x, mu);
   
    errs_mu[index + 1] = mu_err;
    errs_sigma[index + 1] = sigma_err;
    
    delete current_proj;
  }
  
  if (sigma_graph) {
    sigma_graph->SetError(errs_sigma);
    //sigma_graph->SetMaximum(max_sigma+0.15*(max_sigma - min_sigma));
    //sigma_graph->SetMinimum(min_sigma-0.15*(max_sigma - min_sigma));
    sigma_graph->GetYaxis()->SetTitleOffset(1.5);
    sigma_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    sigma_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    sigma_graph->SetTitle("");
  }
  
  if (mu_graph) {
    mu_graph->SetError(errs_mu);
    //mu_graph->SetMaximum(max_mu+0.15*(max_mu - min_mu));
    //mu_graph->SetMinimum(min_mu-0.15*(max_mu - min_mu));
    mu_graph->GetYaxis()->SetTitleOffset(1.5);
    mu_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    mu_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    mu_graph->SetTitle("");
  }
  
  if (fDebug && num_skipped)  std::cout<<" Number of skipped bins: "<<num_skipped<<std::endl;
  
  return;

}

//-----------------------------------------------------------------------------
int ZmumuValidationExample::IterativeGaussFit(TH1* hist, double &mu, double &mu_err, double &sigma, double &sigma_err)
{
  
  //constants for fitting algorithm
  const int iteration_limit = 20;
  const float percent_limit = 0.01;
  const float fit_range_multiplier = 1.5;
  const int fDebug = 0;
  
  double last_mu;
  double last_sigma;
  double current_mu;
  double current_sigma;
  double mu_percent_diff;
  double sigma_percent_diff;
  
  if (!hist) {
    if (fDebug) std::cout<< "Error in  Anp::IterativeGaussFit(): Histogram to be fit is missing" <<std::endl;
    return 1;
  }  
  
  this->HistogramConditioning(hist);

  TF1* fit_func = new TF1("fit_func","gaus");

  int bad_fit = hist->Fit(fit_func,"QN");

  if (fDebug && bad_fit) std::cout <<"BAD INITIAL FIT: "<< hist->GetTitle() << std::endl;

  last_mu = fit_func->GetParameter(1); 
  last_sigma = fit_func->GetParameter(2);

  if (bad_fit) last_mu = hist->GetMean();

  // check as well that the last_mu is reasonable
  if (fabs(last_mu - hist->GetMean()) > 5*hist->GetBinWidth(1)) last_mu =  hist->GetMean();  

  fit_func->SetRange(last_mu-fit_range_multiplier*last_sigma,last_mu+fit_range_multiplier*last_sigma);

  int iteration = 0;

  while ( iteration < iteration_limit ) {

    iteration++;

    double FitRangeLower = last_mu-fit_range_multiplier*last_sigma;
    double FitRangeUpper = last_mu+fit_range_multiplier*last_sigma;

    // if range is to narrow --> broaden it
    if ((FitRangeUpper-FitRangeLower)/hist->GetBinWidth(1) < 4) {
      FitRangeLower -= hist->GetBinWidth(1);
      FitRangeUpper += hist->GetBinWidth(1);
    }

    fit_func->SetRange(FitRangeLower, FitRangeUpper);
    if (m_PrintLevel >= 3) cout << " ** IterativeGaussFit ** fit iter # " << iteration 
				<< "   new fit range: " << FitRangeLower << " --> " << FitRangeUpper << endl; 



    bad_fit = hist->Fit(fit_func, "RQN");

    if (fDebug && bad_fit) std::cout<<" ** BAD FIT ** : bin "<< hist->GetTitle() <<"  iteration "<<iteration<<std::endl;

    current_mu = fit_func->GetParameter(1); 
    current_sigma = fit_func->GetParameter(2);

    //std::cout<<"Iteration: "<<iteration<<" Current: "<<current_mu<<" "<<current_sigma<<" Last: "<<last_mu<<" "<<last_sigma<<std::endl;

    float average_mu = (last_mu+current_mu)/2;
    float average_sigma = (last_sigma+current_sigma)/2;

    if (average_mu == 0) {
      if ( fDebug ) std::cout<<" Average mu = 0 in bin "<< hist->GetTitle() <<std::endl;
      average_mu = current_mu;
    } 

    if (average_sigma == 0) {
      if ( fDebug ) std::cout<<"Average sigma = 0; Fit Problem in "<< hist->GetTitle() <<". "<<last_sigma<<" "<<current_sigma<<std::endl;
      average_sigma = current_sigma;
    }

    mu_percent_diff = fabs((last_mu-current_mu)/average_mu);
    sigma_percent_diff = fabs((last_sigma-current_sigma)/average_sigma);

    if ( mu_percent_diff < percent_limit && sigma_percent_diff < percent_limit ) break;

    if (iteration != iteration_limit) { //necessary?
      last_mu = current_mu;
      last_sigma = current_sigma;
    }    
    // check as well that the last_mu is reasonable
    if (fabs(last_mu - hist->GetMean()) > 5*hist->GetBinWidth(1)) {
      if (m_PrintLevel >= 3) cout << " ** IterativeGaussFit ** fit iter # " << iteration
				  << " ** WARNING ** last_mu looks bad: " << last_mu
				  << "    this iter mu: " << fit_func->GetParameter(1)
				  << "    proposed mu: " << hist->GetMean() 
				  << endl;
      last_mu =  hist->GetMean();  
    } 
  }

  if (iteration == iteration_limit) {
    if (fDebug ) std::cout<<"WARNING: Fit did not converge to < "<<percent_limit*100<<"% in "<< hist->GetTitle() <<" in "<<iteration_limit<<" iterations. Percent Diffs: "<<mu_percent_diff*100<<"% (Mu),"<<" "<<sigma_percent_diff*100<<"% (Sigma)"<<std::endl;
    //possibly return a value other than 0 to indicate not converged?
  }

  mu = current_mu;
  mu_err = fit_func->GetParError(1);
  sigma = current_sigma;
  sigma_err = fit_func->GetParError(2);
  
  hist->GetListOfFunctions()->Add(fit_func);

  if (m_PrintLevel >= 1 ) {
    cout << " ** IterativeGaussFit ** fit result: histo name " << hist->GetName() << "    title: " << hist->GetTitle()  << endl
	 << "    mu = " << mu << " +- " << mu_err << endl
	 << " sigma = " << sigma << " +- " << sigma_err 
	 << endl;
    if (TempCanvasIterGaussFit == NULL) {
      TempCanvasIterGaussFit = new TCanvas ("TempCanvasIterGaussFit","Iter Gauss fit", 400, 400);
    }
    hist->DrawCopy();
    TempCanvasIterGaussFit->Update();
    hist->Print();
    string input = "";
    cout << " ** IterativeGaussFit ** Please type RETURN to continue:\n>";
    getline(cin, input);
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::SetPrintLevel (int newprintlevel) 
{
  m_PrintLevel = newprintlevel;
  if (m_PrintLevel < 0) m_PrintLevel = 0;
  return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::SetEtaBins (int newbins) 
{
  m_EtaBins = newbins;
  if (m_EtaBins < 1) m_EtaBins = 1;
  if (m_EtaBins > 100) m_EtaBins = 100;
  return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::SetPhiBins (int newbins) 
{
  m_PhiBins = newbins;
  if (m_PhiBins < 1) m_PhiBins = 1;
  if (m_PhiBins > 100) m_PhiBins = 100;
  return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::DrawMap ()
{
  this->SymmetrizeHisto(lambdacorrections_vs_etaphi);

  lambdacorrections_vs_etaphi->DrawCopy("colz");

   return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::ResetHistograms ()
{
   delta_vs_etaphi->Reset();
   
   //
   entries->Reset();
   z0delta_vs_etaphi->Reset();

   z0delta->Reset();
   z0delta_pos->Reset();
   z0delta_neg->Reset();
   z0delta_etaphi->Reset();
   z0delta_etaphi_pos->Reset();
   z0delta_etaphi_neg->Reset();

   d0delta_vs_etaphi->Reset();

   d0delta->Reset();
   d0delta_pos->Reset();
   d0delta_neg->Reset();
   d0delta_etaphi->Reset();
   d0delta_etaphi_pos->Reset();
   d0delta_etaphi_neg->Reset();
   //
   
   h_DELTA->Reset();
   h_pt->Reset();
   h_pt_pos->Reset();
   h_pt_neg->Reset();
   h_mass->Reset();

   delta_vs_etaphi->Reset();

   deltacorrections_vs_etaphi->Reset();
   deltacorrections_vs_etaphi_err->Reset();
  
   lambda_vs_etaphi->Reset();
   lambdacorrections_vs_etaphi->Reset();
   lambdacorrections_vs_etaphi_err->Reset();

   lambda_vs_eta->Reset();
   lambda_vs_eta_pos->Reset();
   lambda_vs_eta_neg->Reset();

   lambdacorrections_vs_eta->Reset();

   lambda->Reset();
   lambda_pos->Reset();
   lambda_neg->Reset();

   lambda_eta->Reset();
   lambda_eta_pos->Reset();
   lambda_eta_neg->Reset();

   lambda_etaphi->Reset();
   lambda_etaphi_pos->Reset();
   lambda_etaphi_neg->Reset();

   // impact param histos
   h_z0->Reset();
   h_z0_pos->Reset();
   h_z0_neg->Reset();
   h_d0->Reset();
   h_d0_pos->Reset();
   h_d0_neg->Reset();

   return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::SymmetrizeHisto (TH2* hist)
{
  double RangeUpper = hist->GetBinContent(hist->GetMaximumBin());
  double RangeLower = hist->GetBinContent(hist->GetMinimumBin());

  double NewRangeUpper = RangeUpper;
  double NewRangeLower = -RangeUpper;

  if (RangeUpper < - RangeLower) {
    NewRangeUpper = -RangeLower;
    NewRangeLower =  RangeLower;
  }

  NewRangeUpper *= 1.01; // increase a bit the scale just to make sure everything fits in
  NewRangeLower *= 1.01;

  if (m_PrintLevel >= 3) {
    cout <<  " ** SymmetrizeHisto ** old range: " << RangeLower << " --> " << RangeUpper << endl;
    cout <<  "                       new range: " << NewRangeLower << " --> " << NewRangeUpper << endl;
  }

  hist->SetMaximum(NewRangeUpper);
  hist->SetMinimum(NewRangeLower);
  
  return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::HistogramConditioning (TH1* hist)
{
  if (m_PrintLevel>=3) cout << " ** HistogramConditioning ** START ** hist = " << hist->GetName() << endl; 

  double MinEntriesMPB = 15;
  Int_t NGroupBins = 2;

  // goal:
  // make sure that the most populated bin has a minimum number of entries
  Int_t  MostPopulatedBin = (hist->GetMaximumBin());
  double EntriesMPB = hist->GetBinContent(MostPopulatedBin);
  if (EntriesMPB < MinEntriesMPB) {
    // check the entries of the neighbour channels
    if ((EntriesMPB + hist->GetBinContent(MostPopulatedBin+1) + hist->GetBinContent(MostPopulatedBin-1)) > MinEntriesMPB) {
      NGroupBins = 2;
    }
    else {
      NGroupBins = 3;
    }

    // now find the first divisor (factor of ) the original number of bins
    Bool_t DivisorFound = false;
    while (!DivisorFound) {
      if ( hist->GetNbinsX() % NGroupBins == 0) {
  	DivisorFound = true;
      }
      else {
  	DivisorFound = false;
  	NGroupBins++;
      }
    }
    Int_t NBinsWas = hist->GetNbinsX();
    hist = hist->Rebin(NGroupBins);
    if (m_PrintLevel>=1) cout << " ** HistogramConditioning ** histogram had to be rebinned by: " << NGroupBins 
			      << " NBins was: " << NBinsWas << " and now is: " << hist->GetNbinsX() << endl;

  }
    

  return;
}
