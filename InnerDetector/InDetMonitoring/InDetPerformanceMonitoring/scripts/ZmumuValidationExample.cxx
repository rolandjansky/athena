#define ZMUMUVALIDATIONEXAMPLE_C

#include <iostream>
#include <list>
#include "TMath.h"
#include "TLorentzVector.h"
#include "TF1.h"
#include "JPsiMassFit.C"
#include "deltaSagittaFitter.C"
#include "IPResolStudies.C"
//#include "RooEggePdf.cxx"

#include "ZmumuValidationExample.h"

ZmumuValidationExample::ZmumuValidationExample( std::list<std::string> const & s_fileNames, string s_treeName
                                          , std::string const & s_outFileName
					  , bool isMC
						) : m_fileNames( s_fileNames )
						  , m_outfilename( s_outFileName )
						  , m_file( s_outFileName.c_str(), "RECREATE" )
						  , m_eventChain( s_treeName.c_str() )
						  , m_truthChain( "TruthParams" )
						  , m_PrintLevel(1)
						  , m_EtaBins(1)
						  , m_PhiBins(1)
						  , m_isMC(isMC)
                                                  //, m_isMC_IPResol(false)
						  , m_ApplyBTest(false)
                                                  , m_ApplySagittaHistogramTest(false)
                                                  , m_ApplyRadialDistortion(false)
						  , m_ApplyTelescopeDistortion (false)
						  , m_ApplyEndcapExpansion (false)
                                                  , m_FitBothMuons(false)
                                                  , m_RunIPResolStudies(false)
						    //						  , m_JPsi1SMCbias({0.})
						  , n_iteration(0)
						  , m_sampleType(ZmumuValidationExample::ZSAMPLE)
						  , m_analysisType(ZmumuValidationExample::ANALYSIS_ALL)
                                                  , m_iterationUserInput(1)
						  ,  h_deltaSagittaCorrMaps(NULL)

{
   loadChains();
   setBranchAddresses();

   m_JPsiMassFit = new JPsiMassFit();
   m_deltaSagittaFitter = new deltaSagittaFitter ();
   m_IPResolStudies = new IPResolStudies ();

   this->bookHistograms();
   this->defineInfoStrings();
   this->SetCorrectionsMaps();


   m_factor = 0.5; //factor to assign lambda corrections - nominal is 0.5
   m_shift = 0; //shift in Z mass in MeV - nominal is 0
   TempCanvasIterGaussFit = NULL;
  
   vec_pos = new TLorentzVector();
   vec_neg = new TLorentzVector();
   vec_dimuon = new TLorentzVector();
   vec_truth_pos = new TLorentzVector();
   vec_truth_neg = new TLorentzVector();

   // default values of several tests
   m_ApplyBTest = false;
   m_ApplySagittaHistogramTest = false;
   m_ApplyRadialDistortion = false;
   m_FitBothMuons = false;
   m_RunDeltaSagittaFitter = false;
   m_RunIPResolStudies = false;

   //m_isMC_IPResol = false;

   this->WelcomeBanner();
}

///////////////////////////////////////////////////
ZmumuValidationExample::~ZmumuValidationExample()
{
   m_file.Close();
}

////////////////////////////////////////////////////////
void ZmumuValidationExample::SetCorrectionsMaps()
{  
  TString fileName("DeltaSagittaMap.root");
  TFile* inputFileWithMaps = new TFile(fileName.Data());

  if (inputFileWithMaps->IsOpen()) {
    h_deltaSagittaCorrMaps = (TH2F*) inputFileWithMaps->Get("LambdaCorrectionVsEtaPhi");
    h_deltaSagittaCorrMaps->SetDirectory(0); // detach from file
    if (h_deltaSagittaCorrMaps) {
      std::cout << "  good... " << h_deltaSagittaCorrMaps << " " << h_deltaSagittaCorrMaps->GetName() 
		<< "  bins: " << h_deltaSagittaCorrMaps->GetNbinsX() << "x" <<  h_deltaSagittaCorrMaps->GetNbinsY() << std::endl;
      float delta_sagitta = h_deltaSagittaCorrMaps->GetBinContent(1,1);
      std::cout << " delta_sagitta: " << delta_sagitta << std::endl;
    }
    else {
      std::cout << " bad ... " << std::endl;
    }
    inputFileWithMaps->Close();
  }
  else {
    std::cout << " ** SetCorrectionsMap ** WARNING ** " << fileName.Data() << " does not exists " << std::endl;
  }

  return;
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
   m_eventChain.SetBranchAddress( "runNumber",   &m_runNumber); 
   m_eventChain.SetBranchAddress( "eventNumber",   &m_eventNumber); 

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

   m_eventChain.SetBranchAddress( "Positive_d0_err",   &m_d0_err_pos); 
   m_eventChain.SetBranchAddress( "Negative_d0_err",   &m_d0_err_neg); 
   m_eventChain.SetBranchAddress( "Positive_z0_err",   &m_z0_err_pos); 
   m_eventChain.SetBranchAddress( "Negative_z0_err",   &m_z0_err_neg); 
   m_eventChain.SetBranchAddress( "preScale",   &m_prescale);
   
   
   // for IP resolution studies
   m_eventChain.SetBranchAddress( "Positive_z0_PVerr",  &m_z0_pos_PVerr);
   m_eventChain.SetBranchAddress( "Positive_d0_PVerr",  &m_d0_pos_PVerr);
   m_eventChain.SetBranchAddress( "Negative_z0_PVerr",  &m_z0_neg_PVerr);
   m_eventChain.SetBranchAddress( "Negative_d0_PVerr",  &m_d0_neg_PVerr);

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

  const double eta_bound = ZmumuValidationExample::InnerDetectorEtaRange;
  const double phi_bound = ZmumuValidationExample::NumberPI;
  const double p_bound = 0.5;
  const double pt_bound = 15; 

  int pt_nbins = 80;
  //int pt_nbins = 200;
  //int pt_nbins = 20;
  double pt_low = 0.;
  double pt_high = 120.; 
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) pt_high = 50.;
  if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) pt_high = 50.;
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) pt_high = 50.;

  double z_bound = 1.0; //mm
  double d_bound = 0.18; //mm
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE ||
      m_sampleType == ZmumuValidationExample::UPSILONSAMPLE ||
      m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) {
    z_bound = 1.0;
    d_bound =  0.200;
  }

  int z0_nbins = 100;
  double z0_range = 150.; 
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) z0_range = 200;
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) z0_range = 200;

  int d0_nbins = 100;
  double d0_range = 0.08; 
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) d0_range = 0.200;
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) d0_range = 0.200;

  int invMass_nbins = 80; //OSCAR was 80 
  invMass_low = 70.; //was 65.; // defined in .h
  invMass_high = 110.; //was 115.; // defined in .h
  double referenceMass = ZmumuValidationExample::ZPDGMass;

  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) {
    referenceMass = m_JPsiMassFit->GetReferenceMass(); 
    invMass_nbins = 75; // was 50
    invMass_low = 2.6;  // was 2.6 
    invMass_high = 4.8; // was 4.4
    pt_high = 50.; // was 15
    pt_nbins = 50;
    if (m_isMC) invMass_high = 3.6;
    //    invMass_high = 3.6;
  }
  if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) {
    referenceMass = ZmumuValidationExample::Upsilon1SPDGMass;
    invMass_nbins = 60;
    invMass_low = 7.;
    invMass_high = 13.;
    if (m_isMC) {
     invMass_nbins = 30;
     invMass_low = 8.;
     invMass_high = 11.;
    }
  }
  if (m_sampleType == ZmumuValidationExample::DIMUONSAMPLE) {
    invMass_nbins = 220;
    invMass_low = 0.;
    invMass_high = 110.;
  }
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) {
    referenceMass = ZmumuValidationExample::KShortPDGMass;
    invMass_nbins = 50;
    invMass_low = 0.45;
    invMass_high = 0.55;
  }

  // variable size pt bins (for some histos)
  const int  nptbins_variable = 21;
  float ptbins_variable_limits[nptbins_variable+1] = {0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.,
                                                      12., 14., 16., 18., 20., 22., 
						      26., 30.,
						      35., 40.,
						      50.};

  ////// histogram booking

  h_DELTA = new TH1F("h_DELTA",";#delta [GeV]",200,-50,50);

  h_pt = 	new TH1F("Pt","p_{T} of both #mu; p_{T} [GeV]",    pt_nbins, pt_low, pt_high);
  h_pt_pos = 	new TH1F("Pt_Pos","p_{T} of #mu^{+}; p_{T} [GeV]", pt_nbins, pt_low, pt_high);
  h_pt_neg = 	new TH1F("Pt_Neg","p_{T} of #mu^{-}; p_{T} [GeV]", pt_nbins, pt_low, pt_high);
  h_pt_pos_vs_neg = 	new TH2F("Pt_Pos_vs_Pt_Neg","p_{T} of #mu^{-} and #mu^{+}; p_{T} [GeV]", pt_nbins, pt_low, pt_high, pt_nbins, pt_low, pt_high);


  h_z0     =	new TH1F("h_z0","z_{0}: longitudinal impact param.; z_{0} [mm]", z0_nbins, -z0_range, z0_range);
  h_z0_pos =	new TH1F("h_z0_Pos","z_{0} of #mu^{+};z_{0} [mm]", z0_nbins, -z0_range, z0_range);
  h_z0_neg =	new TH1F("h_z0_Neg","z_{0} of #mu^{-};z_{0} [mm]", z0_nbins, -z0_range, z0_range);
  h_d0     =	new TH1F("h_d0","d_{0}: transvers. impact param.; d_{0} [mm]", d0_nbins, -d0_range, d0_range);
  h_d0_pos =	new TH1F("h_d0_Pos","d_{0} of #mu^{+};d_{0} [mm]", d0_nbins, -d0_range, d0_range);
  h_d0_neg =	new TH1F("h_d0_Neg","d_{0} of #mu^{-};d_{0} [mm]", d0_nbins, -d0_range, d0_range);


  // invariant mass histograms
  h_mass =	 new TH1F("ZMass",";Mass [GeV]", invMass_nbins, invMass_low, invMass_high);
  h_dimuon_mass_vs_etaphi =  new TH3F( "h_dimuon_mass_vs_etaphi","dimuon invariant mass;#eta; #phi [rad]; m_{#mu#mu} [GeV]", 
				       fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, invMass_nbins, invMass_low, invMass_high);
  h_dimuon_mass_vs_etaphiPos =  new TH3F( "h_dimuon_mass_vs_etaphiPos","dimuon invariant mass vs #mu^{+} (#eta,#phi);#eta; #phi [rad]; m_{#mu#mu} [GeV]", 
				       fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, invMass_nbins, invMass_low, invMass_high);
  h_dimuon_mass_vs_etaphiNeg =  new TH3F( "h_dimuon_mass_vs_etaphiNeg","dimuon invariant mass vs #mu^{-} (#eta,#phi) ;#eta; #phi [rad]; m_{#mu#mu} [GeV]", 
				       fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, invMass_nbins, invMass_low, invMass_high);

  // pt of positive and negative muon vs eta-phi

  h_ptpos_vs_etaphi = new TH3F( "h_ptpos_vs_etaphi","p_{T} of #mu^{+};#eta; #phi [rad]; p_{T} [GeV]", 
				fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound,  pt_nbins, pt_low, pt_high);
  h_ptneg_vs_etaphi = new TH3F( "h_ptneg_vs_etaphi","p_{T} of #mu^{-};#eta; #phi [rad]; p_{T} [GeV]", 
				fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound,  pt_nbins, pt_low, pt_high);



  h_dimuon_mass_vs_etaphi_fitresult = NULL;
  h_dimuon_mass_vs_etaphi_fiterror = NULL;
  h_dimuon_mass_vs_etaphi_fitresultVsPDG = NULL;
  h_dimuon_mass_vs_etaphi_fitresultVsPDGpc = NULL;
  h_dimuon_mass_vs_etaphi_fitsigma = NULL;
  h_dimuon_mass_vs_etaphi_chi2overndf = NULL;
  h_dimuon_mass_shfit_significance_vs_etaphi = NULL;

  h_dimuon_mass_vs_etaphiPos_fitresult = NULL;
  h_dimuon_mass_vs_etaphiPos_fiterror = NULL;
  h_dimuon_mass_vs_etaphiPos_fitresultVsPDG = NULL;
  h_dimuon_mass_vs_etaphiPos_fitresultVsPDGpc = NULL;
  h_dimuon_mass_vs_etaphiPos_fitsigma = NULL;
  h_dimuon_mass_shfit_significance_vs_etaphiPos = NULL;

  h_dimuon_mass_vs_etaphiNeg_fitresult = NULL;
  h_dimuon_mass_vs_etaphiNeg_fiterror = NULL;
  h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG = NULL;
  h_dimuon_mass_vs_etaphiNeg_fitresultVsPDGpc = NULL;
  h_dimuon_mass_vs_etaphiNeg_fitsigma = NULL;
  h_dimuon_mass_shfit_significance_vs_etaphiNeg = NULL;

  h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues = NULL;
  h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues = NULL;

  h_deltaSagittaFitterMaps = NULL; 
  h_RadialDistortion_input = NULL;
  h_radialdistortion_pos_neg = NULL;

  // entries
  h_entries_vs_etaphi =  new TH2F( "h_entries_vs_etaphi",";#eta; #phi [rad]", 
				   fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  // dimuon histograms
  h_dimuon_mass =      new TH1F("h_dimuon_mass","Dimuon invariant mass;Mass [GeV]", invMass_nbins, invMass_low, invMass_high);
  h_dimuon_pt =        new TH1F("h_dimuon_pt","p_{T} of dimuon; p_{T} [GeV]", pt_nbins, pt_low, pt_high);
  h_dimuon_pz =        new TH1F("h_dimuon_pz","p_{Z} of dimuon; p_{Z} [GeV]", pt_nbins, -2*pt_high, 2*pt_high); 
  h_dimuon_eta =       new TH1F("h_dimuon_eta","#eta of dimuon;#eta", fnEtaBins, -eta_bound, eta_bound); h_dimuon_eta->SetMinimum(0.);
  h_dimuon_phi =       new TH1F("h_dimuon_phi","#phi of dimuon;#phi [rad]", fnPhiBins, -phi_bound, phi_bound); h_dimuon_phi->SetMinimum(0.);
  h_dimuon_pt_vs_eta = new TH2F("h_dimuon_pt_vs_eta" ,"dimuon: p_{T} vs #eta;#eta; p_{T} [GeV]", fnEtaBins,-eta_bound, eta_bound, pt_nbins, pt_low, 2*pt_high); 
  h_dimuon_pt_vs_mass= new TH2F("h_dimuon_pt_vs_mass","dimuon: p_{T} vs mass;m_{#mu#mu} [GeV]; p_{T} [GeV]", invMass_nbins, invMass_low, invMass_high, pt_nbins, pt_low, pt_high); 

  int deltaPhiBins = 30*fnPhiBins; if (deltaPhiBins > 120) deltaPhiBins = 120;
  h_dimuon_deltaphi =  new TH1F("h_dimuon_deltaphi","dimuon: #Delta #phi = #phi_{leading} - #phi_{second}; #Delta #phi [rad]", deltaPhiBins, 0, phi_bound);
  h_dimuon_deltaphi_vs_mass =  new TH2F("h_dimuon_deltaphi_vs_mass","dimuon: #Delta #phi = #phi_{leading} - #phi_{second} vs invariant mass; m_{#mu#mu} [GeV]; #Delta #phi [rad]", 
					invMass_nbins, invMass_low, invMass_high, deltaPhiBins, 0, phi_bound);
  h_dimuon_cosalpha = new TH1F("h_dimuon_cosalpha","dimuon: cos(opening angle); cos(#alpha)", deltaPhiBins, -1., 1.);
  h_dimuon_cosalpha_vs_mass = new TH2F("h_dimuon_cosalpha_vs_mass","dimuon: cos(opening angle) vs invariant mass; m_{#mu#mu} [GeV]; cos(#alpha)", 
				       invMass_nbins, invMass_low, invMass_high, deltaPhiBins, -1., 1.);

  int deltaetabins = 2*m_EtaBins; 
  if (deltaetabins < 20) deltaetabins = 20;
  if (deltaetabins > 80) deltaetabins = 80;
  h_dimuon_deltaeta = new TH1F("h_dimuon_deltaeta","dimuon: #Delta #eta = #eta_{leading} - #eta_{second}; #Delta #eta", deltaetabins, -1., 1.);
  h_dimuon_deltaeta->SetMinimum(0.);

  h_dimuon_deltaz0_vs_mass =  new TH2F("h_dimuon_deltaz0_vs_mass","dimuon: #Delta z_{0} vs invariant mass; m_{#mu#mu} [GeV]; #Delta z_{0} [mm]",
                                       invMass_nbins, invMass_low, invMass_high, z0_nbins, -z_bound, z_bound);

  h_dimuon_deltad0_vs_mass =  new TH2F("h_dimuon_deltad0_vs_mass","dimuon: #Delta d_{0} vs invariant mass; m_{#mu#mu} [GeV]; #Delta d_{0} [mm]",
				       invMass_nbins, invMass_low, invMass_high, d0_nbins, -d_bound, d_bound);
  h_dimuon_deltaz0_vs_eta  =  new TH2F("h_dimuon_deltaz0_vs_eta","dimuon: #Delta z_{0} vs #eta; dimuon #eta; #Delta z_{0} [mm]",
				       fnEtaBins,-eta_bound,eta_bound, z0_nbins, -z_bound, z_bound);
  h_dimuon_deltad0_vs_eta  =  new TH2F("h_dimuon_deltad0_vs_eta","dimuon: #Delta d_{0} vs #eta; dimuon #eta; #Delta d_{0} [mm]",
				       fnEtaBins,-eta_bound,eta_bound, d0_nbins, -d_bound, d_bound);

  h_d0Significance_vs_mass = new TH2F("h_d0Significance_vs_mass","d0/#sigma_{d0} vs invariant mass; m_{#mu#mu} [GeV]; d_{0}/#sigma_{d0}",
				       invMass_nbins, invMass_low, invMass_high, d0_nbins, -10., 10.);

  h_dimuon_ptlead_vs_mass = new TH2F("h_dimuon_ptlead_vs_mass","dimuon: p_{T} leading vs invariant mass; m_{#mu#mu} [GeV]; Leading p_{T} [GeV]",
				     invMass_nbins, invMass_low, invMass_high,  pt_nbins, pt_low, 2*pt_high); 

  h_dimuon_DeltaM_vs_ptlead = new TH2F("h_dimuon_DeltaM_vs_ptlead", "(m_{reco} - M_{MC})/M_{PDG} Vs leading p_{T}; Leading p_{T} [GeV]; #DeltaM/M_{PDG}",
				       pt_nbins, pt_low, pt_high, 50, -0.02, 0.02);
  p_dimuon_DeltaM_vs_ptlead = new TProfile("p_dimuon_DeltaM_vs_ptlead", "(m_{reco} - M_{MC})/M_{PDG} Vs leading p_{T}; Leading p_{T} [GeV];  #DeltaM/M_{PDG}",
					   nptbins_variable, ptbins_variable_limits);

  h_prescale_vs_ptlead = new TH2F("h_prescale_vs_ptlead", "Trigger prescale vs p_{T} leading; p_{T} [GeV]; Trigger prescale",
				  pt_nbins, pt_low, pt_high, 256, 0.5, 256.5);

  // radial distortion
  h_radialdistortion = new TH1F ("h_radialdistortion","Radial distortion #epsilon = #delta R / R; #epsilon", 51, -0.025, 0.025); 
  h_radialdistortion_vs_etaphi =  new TProfile2D( "h_radialdistortion_vs_etaphi",";#eta; #phi [rad]", 
						  fnEtaBins, -eta_bound, eta_bound, fnPhiBins, -phi_bound, phi_bound);
  h_radialdistortion_vs_etaphi_Integrated =  new TH2F( "h_radialdistortion_vs_etaphi_Integrated","Integrated radial distortion;#eta; #phi [rad]", 
							     fnEtaBins, -eta_bound, eta_bound, fnPhiBins, -phi_bound, phi_bound);
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE || ZmumuValidationExample::UPSILONSAMPLE || ZmumuValidationExample::KSHORTSAMPLE){
    h_radialdistortion_vs_mass = new TH2F ("h_radialdistortion_vs_mass", "Radial distortion #epsilon = #delta R / R vs reconstructed mass; m_{#mu#mu} [GeV]; #epsilon",
					 80, referenceMass*1000 - 20, referenceMass*1000 + 20, 
					 3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());

    h_radialdistortion_vs_pt = new TH2F ("h_radialdistortion_vs_pt", "Radial distortion #epsilon = #delta R / R vs pt; p_{T} [GeV]; #epsilon",
					 pt_nbins, pt_low, pt_high,
					 3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
    h_radialdistortion_vs_pt_prof =  new TProfile( "h_radialdistortion_vs_pt_prof",";p_{T} [GeV]; #epsilon = #delta R /R", 
						   nptbins_variable, ptbins_variable_limits);

    h_radialdistortion_vs_pt_eta = new TH3F ("h_radialdistortion_vs_pt_eta", "Radial distortion #epsilon = #delta R / R vs pt and #eta; p_{T} [GeV]; #eta; #epsilon",
					     pt_nbins, pt_low, pt_high,
					     fnEtaBins, -eta_bound, eta_bound,
					 3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
    h_radialdistortion_vs_eta_prof =  new TProfile( "h_radialdistortion_vs_eta_prof",";#eta; #epsilon = #delta R /R", 
						     fnEtaBins, -eta_bound, eta_bound);

    h_radialdistortion_pos_neg = new TH2F ("h_radialdistortion_pos_neg", "Radial distortion: #epsilon^{-} vs #epsilon^{+}; #epsilon^{-}; #epsilon^{+}",
					   h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax(),
					   h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
					   
  }
  else if (m_sampleType == ZmumuValidationExample::ZSAMPLE){
     h_radialdistortion_vs_mass = new TH2F ("h_radialdistortion_vs_mass", "Radial distortion #epsilon = #delta R / R vs reconstructed mass; m_{#mu#mu} [GeV]; #epsilon",
					 300, referenceMass*1000 - 100, referenceMass*1000 + 100, 
					 3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
  }
  v_radialdistortion_vs_massEta.clear();
  v_radialdistortion_vs_ptEta.clear();

  for (int bineta=0; bineta < fnEtaBins; bineta++) {
    TString hname("h_radialdistortion_vs_massEta"); hname += bineta;
    TString htitle("Radial distortion #epsilon = #delta R / R vs reconstructed mass (eta bin:"); htitle += bineta;
    htitle.Append("); m_{#mu#mu} [GeV]; #epsilon");
    TString hnamept("h_radialdistortion_vs_ptEta"); hnamept += bineta;
    TString htitlept("Radial distortion #epsilon = #delta R / R vs reconstructed pt (eta bin:"); htitlept += bineta;
    htitlept.Append("); p_{T} [GeV]; #epsilon");

    if (m_sampleType == ZmumuValidationExample::JPSISAMPLE || ZmumuValidationExample::UPSILONSAMPLE || ZmumuValidationExample::KSHORTSAMPLE){
    TH2F* h_radialdistortion_vs_massEta = new TH2F (hname.Data(), htitle.Data(),
						    80, referenceMass*1000 - 20, referenceMass*1000 + 20, 
						    3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
    v_radialdistortion_vs_massEta.push_back(h_radialdistortion_vs_massEta);
    if (m_PrintLevel>= 10) std::cout << " v_radialdistortion_vs_massEta for " << bineta << " built " << std::endl;
    TH2F* h_radialdistortion_vs_ptEta = new TH2F (hnamept.Data(), htitlept.Data(),
					 pt_nbins, pt_low , pt_high, 
					 3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
    v_radialdistortion_vs_ptEta.push_back(h_radialdistortion_vs_ptEta);
    std::cout << " v_radialdistortion_vs_ptEta for " << bineta << " built " << std::endl;

    }
    else if (m_sampleType == ZmumuValidationExample::ZSAMPLE){
    TH2F* h_radialdistortion_vs_massEta = new TH2F (hname.Data(), htitle.Data(),
						    300, referenceMass*1000 - 100, referenceMass*1000 + 100, 
						    3*h_radialdistortion->GetNbinsX(),  h_radialdistortion->GetXaxis()->GetXmin(), h_radialdistortion->GetXaxis()->GetXmax());
    v_radialdistortion_vs_massEta.push_back(h_radialdistortion_vs_massEta);
    std::cout << " v_radialdistortion_vs_massEta for " << bineta << " built " << std::endl;
    }
  }

//  corrected_mass =	 new TH1F("PCorrectedZMass",";Mass [GeV]",100,70,110);
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


  z0delta_vs_eta = 	new TH2F("Delta_z0_VsEta",    ";#eta;#delta_{z_{0}}", fnEtaBins,-eta_bound,eta_bound, 50,-z_bound,z_bound);

  z0deltacorrections_vs_eta  = 			new TH1D("z0CorrectionVsEta",";#eta; mean #delta_{z_{0}}", fnEtaBins,-eta_bound,eta_bound);
  z0deltacorrections_vs_eta_RMS =		new TH1D("z0CorrectionVsEta_RMS",";#eta; RMS #delta_{z_{0}}", fnEtaBins,-eta_bound,eta_bound);

  z0delta =	new TH1D("Delta_z0", ";#delta_{z_{0}} [mm]", 100, -z_bound, z_bound);

  z0delta_pos =	new TH1D("Delta_z0_Pos",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_neg =	new TH1D("Delta_z0_Neg",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_etaphi =	new TH1D("Delta_z0_etaphi",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_etaphi_pos =	new TH1D("Delta_z0_etaphi_Pos",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  z0delta_etaphi_neg =	new TH1D("Delta_z0_etaphi_Neg",";#delta_{z_{0}} [mm]", 100,-z_bound,z_bound);

  // sanity check
  float bins_z0_pT[] = {20,30,50,100};
  int  binn_z0_pT = sizeof(bins_z0_pT)/sizeof(float) - 1;

  float bins_z0_eta[] = {-2.5,-2.0,-1.5,-1.0,-0.5,0.,0.5,1.0,1.5,2.0,2.5};
  int  binn_z0_eta = sizeof(bins_z0_eta)/sizeof(float) - 1;
  
  float bins_z0_z0[] = {-150, -149, -148, -147, -146, -145, -144, -143, -142, -141, -140, -139, -138, -137, -136, -135, -134, -133, -132, -131, -130, -129, -128, -127, -126, -125, -124, -123, -122, -121, -120, -119, -118, -117, -116, -115, -114, -113, -112, -111, -110, -109, -108, -107, -106, -105, -104, -103, -102, -101, -100, -99, -98, -97, -96, -95, -94, -93, -92, -91, -90, -89, -88, -87, -86, -85, -84, -83, -82, -81, -80, -79, -78, -77, -76, -75, -74, -73, -72, -71, -70, -69, -68, -67, -66, -65, -64, -63, -62, -61, -60, -59, -58, -57, -56, -55, -54, -53, -52, -51, -50, -49, -48, -47, -46, -45, -44, -43, -42, -41, -40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,150 };
 int  binn_z0_z0 = sizeof(bins_z0_z0)/sizeof(float) - 1;

  z0_vs_etapT =	new TH3F("z0_vsEtaPt",";#eta;p_{T};z_{0}", binn_z0_eta, bins_z0_eta, binn_z0_pT,bins_z0_pT, binn_z0_z0,bins_z0_z0);

  z0_vs_eta =	new TH2D("z0_vsEta",";#eta;z_{0}", binn_z0_eta, bins_z0_eta,binn_z0_z0,bins_z0_z0);
  z0_vs_eta_mean = new TH1D("z0_vsEta_mean",";#eta;#mu(z_{0})",binn_z0_eta, bins_z0_eta);
  z0_vs_eta_RMS = new TH1D("z0_vsEta_RMS",";#eta;#sigma(z_{0})",binn_z0_eta, bins_z0_eta);
  z0_vs_eta_chi2 = new TH1D("z0_vsEta_chi2",";#eta;#chi^{2}",binn_z0_eta, bins_z0_eta);

  z0_vs_pT =	new TH2D("z0_vspT",";p_{T};z_{0}", binn_z0_pT,bins_z0_pT, binn_z0_z0,bins_z0_z0);
  z0_vs_pT_mean = new TH1D("z0_vspT_mean",";p_{T};#mu(z_{0})",binn_z0_pT,bins_z0_pT);
  z0_vs_pT_RMS =	new TH1D("z0_vspT_RMS",";p_{T};#sigma(z_{0})",binn_z0_pT,bins_z0_pT);
  z0_vs_pT_chi2 = new TH1D("z0_vspT_chi2",";#p_{T};#chi^{2}",binn_z0_pT,bins_z0_pT);


  //d0 Histograms------------------------------------------------------------------

  d0delta_vs_etaphi =	new TH3F("Delta_d0_VsEtaPhi",";#eta;#phi;#delta_{d_{0}} [mm]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 50,-d_bound, d_bound);

  d0deltacorrections_vs_etaphi =	new TH2D("d0CorrectionVsEtaPhi",";#eta;#phi;#delta_{d_{0}} [mm]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  d0deltacorrections_vs_etaphi_err = new TH2D("d0CorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{d_{0}}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  d0delta_vs_eta = 	new TH2F("Delta_d0_VsEta",    ";#eta;#Delta(d_{0})", fnEtaBins,-eta_bound,eta_bound,50,-d_bound,d_bound);

  d0deltacorrections_vs_eta  = 			new TH1D("d0CorrectionVsEta",";#eta; mean #delta_{d_{0}}", fnEtaBins,-eta_bound,eta_bound);
  d0deltacorrections_vs_eta_RMS =		new TH1D("d0CorrectionVsEta_RMS",";#eta; RMS #delta_{d_{0}}", fnEtaBins,-eta_bound,eta_bound);


  d0delta =	new TH1D("Delta_d0", ";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_pos =	new TH1D("Delta_d0_Pos",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_neg =	new TH1D("Delta_d0_Neg",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_etaphi =	new TH1D("Delta_d0_etaphi",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_etaphi_pos =	new TH1D("Delta_d0_etaphi_Pos",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);

  d0delta_etaphi_neg =	new TH1D("Delta_d0_etaphi_Neg",";#delta_{d_{0}} [mm]", 100,-d_bound,d_bound);



  // sanity check
  float bins_d0_eta[] = {-2.5,-1.5,-0.5,0.5,1.5,2.5};
  int  binn_d0_eta = sizeof(bins_d0_eta)/sizeof(float) - 1;

  float bins_d0_pT[] = {20,25,30,40,50,100};
  int  binn_d0_pT = sizeof(bins_d0_pT)/sizeof(float) - 1;

  float bins_d0_d0[] = {-0.1, -0.098, -0.096, -0.094, -0.092, -0.09, -0.088, -0.086, -0.084, -0.082, -0.08, -0.078, -0.076, -0.074, -0.072, -0.07, -0.068, -0.066, -0.064, -0.062, -0.06, -0.058, -0.056, -0.054, -0.052, -0.05, -0.048, -0.046, -0.044, -0.042, -0.04, -0.038, -0.036, -0.034, -0.032, -0.03, -0.028, -0.026, -0.024, -0.022, -0.02, -0.018, -0.016, -0.014, -0.012, -0.01, -0.008, -0.006, -0.004, -0.002, 6.94E-17, 0.002, 0.004, 0.006, 0.008, 0.01, 0.012, 0.014, 0.016, 0.018, 0.02, 0.022, 0.024, 0.026, 0.028, 0.03, 0.032, 0.034, 0.036, 0.038, 0.04, 0.042, 0.044, 0.046, 0.048, 0.05, 0.052, 0.054, 0.056, 0.058, 0.06, 0.062, 0.064, 0.066, 0.068, 0.07, 0.072, 0.074, 0.076, 0.078, 0.08, 0.082, 0.084, 0.086, 0.088, 0.09, 0.092, 0.094, 0.096, 0.098, 0.1};
  int  binn_d0_d0 = sizeof(bins_d0_d0)/sizeof(float) - 1;

  d0_vs_etapT =	new TH3F("d0_vsEtaPt",";#eta;p_{T};d_{0}", binn_d0_eta, bins_d0_eta, binn_d0_pT,bins_d0_pT, binn_d0_d0,bins_d0_d0);

  d0_vs_eta =	new TH2D("d0_vsEta",";#eta;d_{0}", binn_d0_eta, bins_d0_eta,binn_d0_d0,bins_d0_d0);
  d0_vs_eta_mean = new TH1D("d0_vsEta_mean",";#eta;#mu(d_{0})",binn_d0_eta, bins_d0_eta);
  d0_vs_eta_RMS = new TH1D("d0_vsEta_RMS",";#eta;#sigma(d_{0})",binn_d0_eta, bins_d0_eta);
  d0_vs_eta_chi2 = new TH1D("d0_vsEta_chi2",";#eta;#chi^{2}",binn_d0_eta, bins_d0_eta);

  d0_vs_pT =	new TH2D("d0_vspT",";p_{T};d_{0}", binn_d0_pT,bins_d0_pT, binn_d0_d0,bins_d0_d0);
  d0_vs_pT_mean = new TH1D("d0_vspT_mean",";p_{T};#mu(d_{0})",binn_d0_pT,bins_d0_pT);
  d0_vs_pT_RMS =	new TH1D("d0_vspT_RMS",";p_{T};#sigma(d_{0})",binn_d0_pT,bins_d0_pT);
  d0_vs_pT_chi2 = new TH1D("d0_vspT_chi2",";#p_{T};#chi^{2}",binn_d0_pT,bins_d0_pT);



  //delta - charge symmetric -----------------
  delta_vs_etaphi = 				new TH3F("DeltaPVsEtaPhi",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-p_bound,p_bound);

  deltacorrections_vs_etaphi = 			new TH2D("DeltaPCorrectionVsEtaPhi",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  deltacorrections_vs_etaphi_err = 		new TH2D("DeltaPCorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  delta_vs_eta = 	new TH2F("DeltaPVsEta",    ";#eta;#delta_{r}", fnEtaBins,-eta_bound,eta_bound,100,-p_bound,p_bound);

  deltacorrections_vs_eta  = 			new TH1D("DeltaPCorrectionVsEta",";#eta;mean #delta_{r}", fnEtaBins,-eta_bound,eta_bound);
  deltacorrections_vs_eta_RMS =		new TH1D("DeltaPCorrectionVsEta_RMS",";#eta; RMS #delta_{r}", fnEtaBins,-eta_bound,eta_bound);



  //lambda * pT - charge anti-symmetric -----------------
  lambda_vs_etaphi = 			 	new TH3F("LambdaVsEtaPhi",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-pt_bound,pt_bound);

  lambdacorrections_vs_etaphi =	new TH2D("LambdaCorrectionVsEtaPhi",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  lambdacorrections_vs_etaphi_err = 		new TH2D("LambdaCorrectionVsEtaPhi_Err",";#eta;#phi;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
  lambdacorrections_vs_etaphi_RMS =             new TH2D("LambdaCorrectionVsEtaPhi_RMS",";#eta;#phi;RMS #delta_{sagitta} [GeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

  //lambda * pT - charge anti-symmetric -----------------
  lambda_vs_eta = 			 	new TH2F("LambdaVsEta",";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
  lambda_vs_eta_pos = 			 	new TH2F("LambdaVsEta_Pos", ";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);
  lambda_vs_eta_neg = 			 	new TH2F("LambdaVsEta_Neg", ";#eta;#delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, 100,-pt_bound,pt_bound);

  lambdacorrections_vs_eta  = 			new TH1D("LambdaCorrectionVsEta",";#eta;mean #delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound);
  lambdacorrections_vs_eta_RMS =		new TH1D("LambdaCorrectionVsEta_RMS",";#eta; RMS #delta_{sagitta} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound);

  lambda =				     	new TH1D("Lambda",    ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_pos =				      	new TH1D("Lambda_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_neg =				      	new TH1D("Lambda_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

  lambda_eta =  			        new TH1D("Lambda_eta",	 ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_eta_pos =			        new TH1D("Lambda_eta_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_eta_neg =			        new TH1D("Lambda_eta_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

  lambda_etaphi =			        new TH1D("Lambda_etaphi",    ";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_etaphi_pos =			        new TH1D("Lambda_etaphi_Pos",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);
  lambda_etaphi_neg =			        new TH1D("Lambda_etaphi_Neg",";#delta_{sagitta} [TeV^{-1}]", 100,-pt_bound,pt_bound);

  delta =				     	new TH1D("DeltaP",    ";#delta_{r}", 100,-p_bound,p_bound);


  if (m_isMC) {

    h_pt_truth = 			     new TH1D("Pt_Truth",     ";p_{T} [GeV]",500,0,500);
    h_pt_pos_truth =			     new TH1D("Pt_Pos_Truth",";p_{T} [GeV]",500,0,500);
    h_pt_neg_truth =			     new TH1D("Pt_Neg_Truth",";p_{T} [GeV]",500,0,500);

    delta_vs_etaphi_truth = 		     new TH3F("DeltaPVsEtaPhi_Truth",";#eta;#phi;#delta_{r}", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound, 100,-p_bound,p_bound);
    deltacorrections_vs_etaphi_truth = 	     new TH2D("DeltaPCorrectionVsEtaPhi_Truth",";#eta;#phi;#delta_{r} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);
    deltacorrections_vs_etaphi_truth_err =   new TH2D("DeltaPCorrectionVsEtaPhi_Truth_Err",";#eta;#phi;#delta_{r} [TeV^{-1}]", fnEtaBins,-eta_bound,eta_bound, fnPhiBins,-phi_bound,phi_bound);

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


  // Histograms of IP Resolution Studies
  m_IPResolStudies->BookHistograms();
  m_IPResolStudies->SetTFile (&m_file);

  return;
}


/////////////////////////////////////////
// called by user to loop over TChains
/////////////////////////////////////////
void ZmumuValidationExample::loop( unsigned int maxEvents )
{
   std::cout << " ** ZmumuValidation::loop() ** START ** " << std::endl;
   n_iteration++;


   //clear histograms to be fit
   this->ResetHistograms();
   
  if (m_RunDeltaSagittaFitter && n_iteration <= 2) {
    // this method only needs to be called just twice
    if (m_deltaSagittaFitter != NULL) {
      if (m_deltaSagittaFitter->Status()) {
	std::cout << " ** ZmumuValidation::loop ** -- calling here deltaSagittaFitter Initialization -- " << std::endl;
	m_deltaSagittaFitter->Initialization(h_ptpos_vs_etaphi);
      }
    }
  }

   unsigned int const nEvents = m_eventChain.GetEntries();
   unsigned int const maxItr = ( maxEvents > 0 && maxEvents < nEvents ) ? maxEvents : nEvents;

   //loop through events and fill histograms
   if (m_PrintLevel >= 0) cout << " ** ZmumuValidationExample::loop ** start loop on events ... " << endl;
   this->loopThroughEvents( maxItr );

   if (m_PrintLevel >= 0) cout<< " ** ZmumuValidationExample::loop ** Fitting histograms" << endl;
   
   //Fit corrections and add to previous corrections
   profileZwithIterativeGaussFit(lambda_vs_etaphi,lambdacorrections_vs_etaphi, lambdacorrections_vs_etaphi_RMS, 1, lambdacorrections_vs_etaphi_err);
   profileZwithIterativeGaussFit(delta_vs_etaphi,deltacorrections_vs_etaphi, 0, 1,deltacorrections_vs_etaphi_err);
   profileYwithIterativeGaussFit(lambda_vs_eta,lambdacorrections_vs_eta,lambdacorrections_vs_eta_RMS);
   profileYwithIterativeGaussFit(delta_vs_eta,deltacorrections_vs_eta,deltacorrections_vs_eta_RMS);
   profileYwithIterativeGaussFit(d0delta_vs_eta,d0deltacorrections_vs_eta,d0deltacorrections_vs_eta_RMS);
   profileYwithIterativeGaussFit(z0delta_vs_eta,z0deltacorrections_vs_eta,z0deltacorrections_vs_eta_RMS);
   profileZwithIterativeGaussFit(z0delta_vs_etaphi,z0deltacorrections_vs_etaphi, 0, 1, z0deltacorrections_vs_etaphi_err);
   profileZwithIterativeGaussFit(d0delta_vs_etaphi,d0deltacorrections_vs_etaphi, 0, 1, d0deltacorrections_vs_etaphi_err);

   
   if (m_isMC && n_iteration == 1) {
     profileZwithIterativeGaussFit(lambda_vs_etaphi_truth,  lambdacorrections_vs_etaphi_truth, 0, 1, lambdacorrections_vs_etaphi_truth_err);
     profileZwithIterativeGaussFit(delta_vs_etaphi_truth,  deltacorrections_vs_etaphi_truth, 0, 1, deltacorrections_vs_etaphi_truth_err);

     profileYwithIterativeGaussFit(lambda_vs_eta_truth, lambdacorrections_vs_eta_truth);
     profileYwithIterativeGaussFit(truth_mom_bias_vs_eta, truth_mom_biascorrections_vs_eta);
     profileYwithIterativeGaussFit(truth_mass_bias_vs_eta, truth_mass_biascorrections_vs_eta);
   }



  if (n_iteration == 1) {
    std::cout << " ** ZmumuValidationExample ** loop ** start fitting slices d0 & z0 slices " << endl;
    FitSlices(d0_vs_eta);
    FitSlices(d0_vs_pT);
    FitSlices(z0_vs_eta);
    FitSlices(z0_vs_pT);
  }

  if ((m_sampleType == ZmumuValidationExample::JPSISAMPLE || m_sampleType == ZmumuValidationExample::ZSAMPLE || m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) 
      && m_analysisType == ZmumuValidationExample::ANALYSIS_RADIALDIST) {
    std::cout << " ** ZmumuValidationExample ** loop ** going to fit dimuon mass distributions for iteration " << n_iteration << std::endl; 
    m_JPsiMassFit->SetMC(m_isMC);
    m_JPsiMassFit->SetAnalysisMode (m_sampleType);
    m_JPsiMassFit->SetInputHistogram (h_dimuon_mass_vs_etaphi);
    m_JPsiMassFit->SetHistogramTitle ("Dimuon invariant mass");
    m_JPsiMassFit->Execute();
    h_dimuon_mass_vs_etaphi_fitresult = m_JPsiMassFit->GetHistoInvMassFitResult();
    h_dimuon_mass_vs_etaphi_fiterror = m_JPsiMassFit->GetHistoInvMassFitError();
    h_dimuon_mass_vs_etaphi_fitresultVsPDG = m_JPsiMassFit->GetHistoInvMassFitResultVsPDG();
    h_dimuon_mass_vs_etaphi_fitresultVsPDGpc = m_JPsiMassFit->GetHistoInvMassFitResultVsPDGpc();
    h_dimuon_mass_vs_etaphi_fitsigma = m_JPsiMassFit->GetHistoInvMassFitSigma();
    h_dimuon_mass_vs_etaphi_chi2overndf = m_JPsiMassFit->GetHistoInvMassChi2overNdf();
    h_dimuon_mass_shfit_significance_vs_etaphi =  m_JPsiMassFit->GetHistoMassShiftSignificance();

    if (m_FitBothMuons == true) {
      // positive muon eta-phi
      m_JPsiMassFit->SetInputHistogram (h_dimuon_mass_vs_etaphiPos);
      // m_JPsiMassFit->SetMuonCharge(1);
      m_JPsiMassFit->Execute();
      h_dimuon_mass_vs_etaphiPos_fitresult = m_JPsiMassFit->GetHistoInvMassFitResult();
      h_dimuon_mass_vs_etaphiPos_fiterror = m_JPsiMassFit->GetHistoInvMassFitError();
      h_dimuon_mass_vs_etaphiPos_fitresultVsPDG = m_JPsiMassFit->GetHistoInvMassFitResultVsPDG();
      h_dimuon_mass_vs_etaphiPos_fitresultVsPDGpc = m_JPsiMassFit->GetHistoInvMassFitResultVsPDGpc();
      h_dimuon_mass_vs_etaphiPos_fitsigma = m_JPsiMassFit->GetHistoInvMassFitSigma();
      h_dimuon_mass_shfit_significance_vs_etaphiPos =  m_JPsiMassFit->GetHistoMassShiftSignificance();
      
      // negative muon eta-phi
      m_JPsiMassFit->SetInputHistogram (h_dimuon_mass_vs_etaphiNeg);
      // m_JPsiMassFit->SetMuonCharge(-1);
      m_JPsiMassFit->Execute();
      h_dimuon_mass_vs_etaphiNeg_fitresult = m_JPsiMassFit->GetHistoInvMassFitResult();
      h_dimuon_mass_vs_etaphiNeg_fiterror = m_JPsiMassFit->GetHistoInvMassFitError();
      h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG = m_JPsiMassFit->GetHistoInvMassFitResultVsPDG();
      h_dimuon_mass_vs_etaphiNeg_fitresultVsPDGpc = m_JPsiMassFit->GetHistoInvMassFitResultVsPDGpc();
      h_dimuon_mass_vs_etaphiNeg_fitsigma = m_JPsiMassFit->GetHistoInvMassFitSigma();
      h_dimuon_mass_shfit_significance_vs_etaphiNeg =  m_JPsiMassFit->GetHistoMassShiftSignificance();

      // homogenize ranges:
      float theMax = h_dimuon_mass_vs_etaphiPos_fitresultVsPDG->GetMaximum();
      if (h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->GetMaximum() > theMax) theMax = h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->GetMaximum();
      float theMin = h_dimuon_mass_vs_etaphiPos_fitresultVsPDG->GetMinimum();
      if (h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->GetMinimum() > theMin) theMin = h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->GetMinimum();

      if (fabs(theMin) > theMax) {
	theMax = fabs(theMin);
      }
      else {
	theMin = - theMax;
      }
      h_dimuon_mass_vs_etaphiPos_fitresultVsPDG->SetMaximum(theMax);
      h_dimuon_mass_vs_etaphiPos_fitresultVsPDG->SetMinimum(theMin);
      h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->SetMaximum(theMax);
      h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->SetMinimum(theMin);
    }
    // radial distortions
    if (m_analysisType == ZmumuValidationExample::ANALYSIS_ALL 
	|| m_analysisType == ZmumuValidationExample::ANALYSIS_RADIALDIST) {
      this->IntegratedRadialDistortions();
    }
  }


  if (m_RunDeltaSagittaFitter && n_iteration <= 2) {
    // this method only needs to be called just twice
    if (m_deltaSagittaFitter != NULL) {
      if (m_deltaSagittaFitter->Status()) {
	std::cout << " ** ZmumuValidation ** -- calling here deltaSagittaFitter -- " << std::endl;
	m_deltaSagittaFitter->SetInputHistograms (h_ptpos_vs_etaphi, h_ptneg_vs_etaphi);
	m_deltaSagittaFitter->Execute();
	// Retrieve histogram with fitted deltaSagitta map
	h_deltaSagittaFitterMaps = m_deltaSagittaFitter->GetDeltaSagittaMap();
      }
      else {
	std::cout << " ** ZmumuValidation ** -- deltaSagittaFitter status = ERROR -- " << std::endl;
      }
    }
  }

  if (m_RunIPResolStudies) {
    if (m_IPResolStudies != NULL) {
      m_IPResolStudies->SetIsMC(m_isMC);
      m_IPResolStudies->Execute();
    }
  }

  writeToFile(n_iteration);
  std::cout << " ** ZmumuValidation::loop() ** COMPLETED ** " << std::endl;

  return;
}


//////////////////////////////////////////////////////////////////////////
// loops over each event and calls fillHistograms() if there are tracks
//////////////////////////////////////////////////////////////////////////
void ZmumuValidationExample::loopThroughEvents( unsigned int maxItr )
{
   std::cout << "  Looping over " << maxItr << " events"<< std::endl;

   for( unsigned int eventItr = 0 ; eventItr != maxItr; ++eventItr) {
      if( eventItr % 10000 == 0 )
	std::cout << "  -- Processing event " << eventItr << " of " << maxItr << "  --> " << (100. * eventItr) /maxItr << " %" << std::endl;
      
      if( !m_eventChain.LoadTree(eventItr) || (m_isMC && !m_truthChain.LoadTree(eventItr)) )
	continue;
      
      if( !m_eventChain.GetEntry(eventItr) || (m_isMC && !m_truthChain.GetEntry(eventItr)) )
	continue;
      
      fillHistograms(eventItr);
      if( eventItr == maxItr-1 ) 
	std::cout << "  -- Processing event " << eventItr << " of " << maxItr << "  --> " << (100. * eventItr) /(maxItr-1) << " %" << std::endl;
   }

   return;
}

//////////////////////////////////////////////////////////////////////////
void ZmumuValidationExample::fillHistograms(int theEvent=0)
{

  //std::unique_ptr< TLorentzVector> vec_pos(new  TLorentzVector());
  //create lorentz vectors for both muons in this event

  //corrected z0 for both muons
  double corrected_z0_pos;
  double corrected_z0_neg;
  double corrected_d0_pos;
  double corrected_d0_neg;

  double sec_mass = muon_mass;

  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) sec_mass = pion_mass;

  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, sec_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, sec_mass);

  // Apply user defined momentum distortions (if requested)
  if (m_ApplyBTest || 
      m_ApplyEndcapExpansion || 
      m_ApplyRadialDistortion ||
      m_ApplyTelescopeDistortion || 
      m_ApplySagittaHistogramTest ) this->SetDistortedMomenta(vec_pos, vec_neg); 

  // form the dimuon (dimuon = mu+ + mu-)
  vec_dimuon->SetXYZM(vec_pos->Px()+vec_neg->Px(), 
		      vec_pos->Py()+vec_neg->Py(),
		      vec_pos->Pz()+vec_neg->Pz(), 
		      ((*vec_pos)+(*vec_neg)).M());

  double deltaphi = this->ComputeDeltaPhi(vec_pos, vec_neg);
  double deltaeta = this->ComputeDeltaEta(vec_pos, vec_neg);

  // event weight according to trigger prescale;
  double event_weight = m_prescale;
  if (event_weight > 512) event_weight = 512;
  event_weight = 1;

  // Perform event selection
  bool goodEvent = this->EventSelection (theEvent, vec_pos, vec_neg, vec_dimuon);


  if (!goodEvent) return;

  // pass TLorentzVectors to deltaSagittaFitter 
  if (m_RunDeltaSagittaFitter) {
    if (m_deltaSagittaFitter != NULL) {
      if (m_deltaSagittaFitter->Status()) {
	if (m_PrintLevel >= 10) {
	  std::cout << " - ZmumuValidationExample - feed data as follows: " << std::endl
		    << "   mu+ (Pxyz,M) " << vec_pos->Px() << " " << vec_pos->Py() << " " << vec_pos->Pz() << " " << vec_pos->M() 
		    << "  Pt: " << vec_pos->Pt() << " Eta: " << vec_pos->Eta() << " Phi: " << vec_pos->Phi() << std::endl 
		    << "   mu- (Pxyz,M) " << vec_neg->Px() << " " << vec_neg->Py() << " " << vec_neg->Pz() << " " << vec_neg->M() 
		    << "  Pt: " << vec_neg->Pt() << " Eta: " << vec_neg->Eta() << " Phi: " << vec_neg->Phi() << std::endl; 
	}
	m_deltaSagittaFitter->Fill(vec_pos->Px(), vec_pos->Py(), vec_pos->Pz(), vec_pos->M(), vec_neg->Px(), vec_neg->Py(), vec_neg->Pz(), vec_neg->M());
      }
    }
  }

  // pass track parameters to IPResolStudies
  if (m_RunIPResolStudies) {
    if (m_IPResolStudies != NULL) {
     m_IPResolStudies->SetMuPosTrackparams (vec_pos, m_d0_pos, m_z0_pos, m_d0_err_pos, m_z0_err_pos);
     m_IPResolStudies->SetMuNegTrackparams (vec_neg, m_d0_neg, m_z0_neg, m_d0_err_neg, m_z0_err_neg);
     m_IPResolStudies->SetPVParams (m_d0_pos_PVerr, m_z0_pos_PVerr, m_d0_neg_PVerr, m_z0_neg_PVerr);
     m_IPResolStudies->SetRunNumber (m_runNumber);
     m_IPResolStudies->SetInputIteration (n_iteration);
     m_IPResolStudies->FillHistograms ();
    }
  }


  // Estimate of radial distortion 
  if (m_analysisType == ZmumuValidationExample::ANALYSIS_ALL 
      || m_analysisType == ZmumuValidationExample::ANALYSIS_RADIALDIST) {
    RadialDistortionCorrection(vec_pos, vec_neg, vec_dimuon);
    if (m_PrintLevel>= 10) std::cout << " ** fillhistograms ** - returned from RadialDistortionCorrection - " << std::endl;
  }

  
  // dimuon histograms
  //std::cout << " ** fillhistograms ** - filling dimuon histograms - " << std::endl;
  h_dimuon_mass->Fill(vec_dimuon->M()/1000.0);
  h_dimuon_pt->Fill(vec_dimuon->Pt()/1000.0); 
  h_dimuon_pz->Fill(vec_dimuon->Pz()/1000.0);
  h_dimuon_eta->Fill(vec_dimuon->Eta());
  h_dimuon_phi->Fill(vec_dimuon->Phi());
  h_dimuon_pt_vs_eta->Fill( vec_dimuon->Eta(),    vec_dimuon->Pt()/1000.0);
  h_dimuon_pt_vs_mass->Fill(vec_dimuon->M()/1000, vec_dimuon->Pt()/1000.0); 
  h_dimuon_deltaphi->Fill(deltaphi);
  h_dimuon_deltaphi_vs_mass->Fill(vec_dimuon->M()/1000.0, deltaphi);
  h_dimuon_deltaeta->Fill(deltaeta);
  h_dimuon_cosalpha->Fill(cos(vec_pos->Angle(vec_neg->Vect())));
  h_dimuon_cosalpha_vs_mass->Fill(vec_dimuon->M()/1000.0, cos(vec_pos->Angle(vec_neg->Vect())));
  h_dimuon_deltaz0_vs_mass->Fill(vec_dimuon->M()/1000.0, m_z0_pos - m_z0_neg);
  h_dimuon_deltaz0_vs_eta->Fill(vec_dimuon->Eta(), m_z0_pos - m_z0_neg);
  h_dimuon_deltad0_vs_mass->Fill(vec_dimuon->M()/1000.0, m_d0_pos - m_d0_neg);
  h_dimuon_deltad0_vs_eta->Fill(vec_dimuon->Eta(), m_d0_pos - m_d0_neg);
  
  // d0 significance
  //std::cout << " ** fillhistograms ** - filling d0 significance - " << std::endl;
  if (m_d0_err_pos > 0.0001 && m_d0_err_neg > 0.0001) { // this is to make sure the d0 error was filled
    h_d0Significance_vs_mass->Fill(vec_dimuon->M()/1000.0, m_d0_pos/m_d0_err_pos);
    h_d0Significance_vs_mass->Fill(vec_dimuon->M()/1000.0, m_d0_neg/m_d0_err_neg);
  }

  // dimuon invariant mass:
  //std::cout << " ** fillhistograms ** - filling dimuon invariant mass significance - " << std::endl;
  h_dimuon_mass_vs_etaphi->Fill(vec_dimuon->Eta(), vec_dimuon->Phi(), vec_dimuon->M()/1000.0, event_weight);
  h_dimuon_mass_vs_etaphiPos->Fill(vec_pos->Eta(), vec_pos->Phi(), vec_dimuon->M()/1000.0, event_weight);
  h_dimuon_mass_vs_etaphiNeg->Fill(vec_neg->Eta(), vec_neg->Phi(), vec_dimuon->M()/1000.0, event_weight);

  h_dimuon_ptlead_vs_mass->Fill(vec_dimuon->M()/1000.0, max(vec_pos->Pt(), vec_neg->Pt())/1000.0, event_weight );

  // entries:
  h_entries_vs_etaphi->Fill(vec_dimuon->Eta(), vec_dimuon->Phi());

  // trigger prescale
  h_prescale_vs_ptlead->Fill(max(vec_pos->Pt(), vec_neg->Pt())/1000.0, m_prescale);

  
  h_ptpos_vs_etaphi->Fill(vec_pos->Eta(), vec_pos->Phi(), vec_pos->Pt()/1000.0, event_weight);
  h_ptneg_vs_etaphi->Fill(vec_neg->Eta(), vec_neg->Phi(), vec_neg->Pt()/1000.0, event_weight);

  /////////
  entries->Fill(vec_pos->Eta(),vec_pos->Phi(),1);
  entries->Fill(vec_neg->Eta(),vec_neg->Phi(),1);

  // 
  //std::cout << " ** fillhistograms ** - going to call fillHistogram for lambda - " << std::endl;
  fillHistogram( lambda,      vec_pos,  vec_neg,  1);
  fillHistogram( lambda_pos,  vec_pos,  vec_neg,  1, +1);
  fillHistogram( lambda_neg,  vec_pos,  vec_neg,  1, -1);

  fillHistogram( delta,      vec_pos,  vec_neg,  0);


  // //z0 etaphi -- fill z0 maps before any correction is applied
  //std::cout << " ** fillhistograms ** - going to fill z0 maps - " << std::endl;
  if ( m_z0_neg != 0  &&  m_z0_pos != 0 && n_iteration == 1 ){
    fillEtaPtHistogram(z0_vs_etapT,z0_vs_eta, z0_vs_pT,vec_pos, vec_neg, m_z0_pos, m_z0_neg);
    //    cout <<"z0 - : "<< m_z0_neg << " z0 + : " << m_z0_pos << endl;
  }

  // //d0 etaphi -- fill d0 maps before any correction is applied
  if ( m_d0_neg != 0  &&  m_d0_pos != 0 && n_iteration == 1){
    fillEtaPtHistogram(d0_vs_etapT,d0_vs_eta, d0_vs_pT,vec_pos, vec_neg, m_d0_pos, m_d0_neg);
    //    cout <<"d0 - : "<< m_d0_neg << " d0 + : " << m_d0_pos << endl;
  }



  //z0 etaphi
  if ( m_z0_neg != 0  &&  m_z0_pos != 0){
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

     // setting original z0values
     corrected_z0_pos = m_z0_pos;
     corrected_z0_neg = m_z0_neg;

     correctZd0(z0deltacorrections_vs_eta, vec_pos, vec_neg, corrected_z0_pos, corrected_z0_neg);
     fillZd0EtaHistogram(z0delta_vs_eta, vec_pos, vec_neg, corrected_z0_pos, corrected_z0_neg);


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

     // setting original d0values
     corrected_d0_pos = m_d0_pos;
     corrected_d0_neg = m_d0_neg;
     correctZd0(d0deltacorrections_vs_eta, vec_pos, vec_neg, corrected_d0_pos, corrected_d0_neg);
     fillZd0EtaHistogram(d0delta_vs_eta, vec_pos, vec_neg, corrected_d0_pos, corrected_d0_neg);
  }

  //fill truth histograms on first iteration without correcting momentum
  if (m_isMC && n_iteration == 1) {

    vec_truth_pos->SetXYZM(m_truth_px_pos, m_truth_py_pos, m_truth_pz_pos, sec_mass);
    vec_truth_neg->SetXYZM(m_truth_px_neg, m_truth_py_neg, m_truth_pz_neg, sec_mass);

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
  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, sec_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, sec_mass);

  //apply delta momentum corrections in eta/phi
  correctMomentum( deltacorrections_vs_etaphi,  vec_pos, vec_neg, 0);
  //fill histograms
  fillEtaPhiHistogram( delta_vs_etaphi, vec_pos, vec_neg, 0);

  //-----------------------------------------------------------------

  //reset muon vectors and use original values
  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, sec_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, sec_mass);

  //apply lambda momentum corrections in eta
  correctMomentum( lambdacorrections_vs_eta,  vec_pos, vec_neg, 1);

  fillHistogram( lambda_eta,	  vec_pos,  vec_neg,  1);
  fillHistogram( lambda_eta_pos,  vec_pos,  vec_neg,  1, +1);
  fillHistogram( lambda_eta_neg,  vec_pos,  vec_neg,  1, -1);

  //fill histograms
  fillEtaHistogram( lambda_vs_eta,      vec_pos,  vec_neg,  1);
  fillEtaHistogram( lambda_vs_eta_pos,  vec_pos,  vec_neg,  1, +1);
  fillEtaHistogram( lambda_vs_eta_neg,  vec_pos,  vec_neg,  1, -1);


  //reset muon vectors and use original values
  vec_pos->SetXYZM(m_px_pos, m_py_pos, m_pz_pos, sec_mass);
  vec_neg->SetXYZM(m_px_neg, m_py_neg, m_pz_neg, sec_mass);
  //apply lambda momentum corrections in eta
  correctMomentum( deltacorrections_vs_eta,  vec_pos, vec_neg, 0);
  fillEtaHistogram( delta_vs_eta,  vec_pos,  vec_neg, 0);


  //check corrected pt/mass distributions at each iteration
  h_pt->Fill(vec_pos->Pt()/1000.0, event_weight);
  h_pt->Fill(vec_neg->Pt()/1000.0, event_weight);
  h_pt_pos->Fill(vec_pos->Pt()/1000.0, event_weight);
  h_pt_neg->Fill(vec_neg->Pt()/1000.0); // PRESCALE MISSING JUST FOR CROSSCHECK
  h_pt_pos_vs_neg->Fill(vec_pos->Pt()/1000.0, vec_neg->Pt()/1000.0);  


  h_mass->Fill(((*vec_pos)+(*vec_neg)).M()/1000.0, event_weight);

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


  //delete vec_pos;
  //delete vec_neg;
  //delete vec_dimuon;

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

void ZmumuValidationExample::fillZd0EtaHistogram(TH2* hist, TLorentzVector* v_pos, TLorentzVector* v_neg, double zd0_muon_p, double zd0_muon_n)
{
   double deltazd0 = -(zd0_muon_p - zd0_muon_n);
   hist->Fill(v_pos->Eta(), deltazd0);
   hist->Fill(v_neg->Eta(), -1.*deltazd0);
}



void ZmumuValidationExample::fillEtaPtHistogram(TH3* hist,TH2* vsEta, TH2* vspT ,TLorentzVector* V_pos, TLorentzVector* V_neg, double deltaTrackPar_pos, double deltaTrackPar_neg)
{
  hist->Fill(V_pos->Eta(),V_pos->Pt()/1000.0, deltaTrackPar_pos);
  hist->Fill(V_neg->Eta(),V_neg->Pt()/1000.0, deltaTrackPar_neg);

  vsEta->Fill(V_pos->Eta(), deltaTrackPar_pos);
  vsEta->Fill(V_neg->Eta(), deltaTrackPar_neg);
  vspT->Fill(V_pos->Pt()/1000.0, deltaTrackPar_pos);
  vspT->Fill(V_neg->Pt()/1000.0, deltaTrackPar_neg);

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


void ZmumuValidationExample::FitSlices(TH2 * hist){//, TH1 * h_mean, TH1 * h_rms, TH1 * h_chi2){

  hist->FitSlicesY();

  std::string mean = hist->GetName(); mean += "_1";
  std::string rms = hist->GetName(); rms += "_2";
  std::string chi2 = hist->GetName(); chi2 += "_chi2";

  TH1D *h2_1_mean = (TH1D*)gDirectory->Get(mean.c_str());
  TH1D *h2_2_rms = (TH1D*)gDirectory->Get(rms.c_str());
  TH1D *h2_3_chi2 = (TH1D*)gDirectory->Get(chi2.c_str());


  std::string hist_mean = hist->GetName(); hist_mean += "_mean";
  std::string hist_rms = hist->GetName(); hist_rms += "_RMS";
  std::string hist_chi2 = hist->GetName(); hist_chi2 += "_chi2";

  if(mean.find("d0_vsEta") != string::npos && !(mean.find("d0_vsEtaPt") != string::npos)){
    h2_1_mean->GetYaxis()->SetTitle("#mu(d_{0})");
    h2_2_rms->GetYaxis()->SetTitle("#sigma(d_{0})");
    h2_3_chi2->GetYaxis()->SetTitle("#chi^{2}");
    d0_vs_eta_mean = (TH1D*)h2_1_mean->Clone(hist_mean.c_str());
    d0_vs_eta_RMS = (TH1D*)h2_2_rms->Clone(hist_rms.c_str());
    d0_vs_eta_chi2 = (TH1D*)h2_3_chi2->Clone(hist_chi2.c_str());

  }
  if(mean.find("d0_vspT") != string::npos ){
    h2_1_mean->GetYaxis()->SetTitle("#mu(d_{0})");
    h2_2_rms->GetYaxis()->SetTitle("#sigma(d_{0})");
    h2_3_chi2->GetYaxis()->SetTitle("#chi^{2}");
    d0_vs_pT_mean = (TH1D*)h2_1_mean->Clone(hist_mean.c_str());
    d0_vs_pT_RMS = (TH1D*)h2_2_rms->Clone(hist_rms.c_str());
    d0_vs_pT_chi2 = (TH1D*)h2_3_chi2->Clone(hist_chi2.c_str());
  }
  if(mean.find("z0_vsEta") != string::npos  && !(mean.find("z0_vsEtaPt") != string::npos)){
    h2_1_mean->GetYaxis()->SetTitle("#mu(z_{0})");
    h2_2_rms->GetYaxis()->SetTitle("#sigma(z_{0})");
    h2_3_chi2->GetYaxis()->SetTitle("#chi^{2}");
    z0_vs_eta_mean = (TH1D*)h2_1_mean->Clone(hist_mean.c_str());
    z0_vs_eta_RMS = (TH1D*)h2_2_rms->Clone(hist_rms.c_str());
    z0_vs_eta_chi2 = (TH1D*)h2_3_chi2->Clone(hist_chi2.c_str());

  }

  if(mean.find("z0_vspT") != string::npos ){
    h2_1_mean->GetYaxis()->SetTitle("#mu(z_{0})");
    h2_2_rms->GetYaxis()->SetTitle("#sigma(z_{0})");
    h2_3_chi2->GetYaxis()->SetTitle("#chi^{2}");
    z0_vs_pT_mean = (TH1D*)h2_1_mean->Clone(hist_mean.c_str());
    z0_vs_pT_RMS = (TH1D*)h2_2_rms->Clone(hist_rms.c_str());
    z0_vs_pT_chi2 = (TH1D*)h2_3_chi2->Clone(hist_chi2.c_str());
  }


}










// impact parameter biases methods------------------------------------------------------------------

///////////////////////////////////
// saves root objects to a TFile
///////////////////////////////////
void ZmumuValidationExample::writeToFile(int iteration)
{

  std::cout << " ** ZmumuValidationExample::writeToFile ** iteration = " << iteration << std::endl;
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

  if(iteration == 1){
    d0_vs_etapT->Write();

    d0_vs_eta->Write();
    d0_vs_pT->Write();

    d0_vs_eta_mean->Write();
    d0_vs_eta_RMS->Write();
    d0_vs_eta_chi2->Write();

    d0_vs_pT_mean->Write();
    d0_vs_pT_RMS->Write();
    d0_vs_pT_chi2->Write();

    z0_vs_etapT->Write();

    z0_vs_eta->Write();
    z0_vs_pT->Write();

    z0_vs_eta_mean->Write();
    z0_vs_eta_RMS->Write();
    z0_vs_eta_chi2->Write();

    z0_vs_pT_mean->Write();
    z0_vs_pT_RMS->Write();
    z0_vs_pT_chi2->Write();
  }

  if (iteration > 0) {
    m_file.mkdir(Form("Iteration%i",iteration));
    m_file.cd(Form("Iteration%i",iteration));
  }

  h_pt->Write();
  h_pt_pos->Write();
  h_pt_neg->Write();
  h_pt_pos_vs_neg->Write();
  h_mass->Write();

  h_z0->Write();
  h_z0_pos->Write();
  h_z0_neg->Write();
  h_d0->Write();
  h_d0_pos->Write();
  h_d0_neg->Write();

  h_dimuon_mass->Write();
  h_dimuon_pt->Write();
  h_dimuon_pz->Write();
  h_dimuon_eta->Write();
  h_dimuon_phi->Write();
  h_dimuon_pt_vs_eta->Write();
  h_dimuon_pt_vs_mass->Write();
  h_dimuon_deltaphi->Write();
  h_dimuon_deltaphi_vs_mass->Write();
  h_dimuon_deltaeta->Write();
  h_dimuon_cosalpha->Write();
  h_dimuon_cosalpha_vs_mass->Write();
  h_dimuon_deltaz0_vs_mass->Write();
  h_dimuon_deltaz0_vs_eta->Write();
  h_dimuon_ptlead_vs_mass->Write();
  h_dimuon_deltad0_vs_mass->Write();
  h_dimuon_deltad0_vs_eta->Write();
  h_d0Significance_vs_mass->Write();

  h_dimuon_mass_vs_etaphi->Write();
  if (h_dimuon_mass_vs_etaphi_fitresult != NULL) h_dimuon_mass_vs_etaphi_fitresult->Write();
  if (h_dimuon_mass_vs_etaphi_fitresultVsPDG != NULL) h_dimuon_mass_vs_etaphi_fitresultVsPDG->Write();
  if (h_dimuon_mass_vs_etaphi_fitresultVsPDGpc != NULL) h_dimuon_mass_vs_etaphi_fitresultVsPDGpc->Write();
  if (h_dimuon_mass_vs_etaphi_fiterror != NULL) h_dimuon_mass_vs_etaphi_fiterror->Write();
  if (h_dimuon_mass_vs_etaphi_fitsigma != NULL) h_dimuon_mass_vs_etaphi_fitsigma->Write();
  if (h_dimuon_mass_vs_etaphi_chi2overndf != NULL) h_dimuon_mass_vs_etaphi_chi2overndf->Write();
  if (h_dimuon_mass_shfit_significance_vs_etaphi != NULL) h_dimuon_mass_shfit_significance_vs_etaphi->Write();
  h_entries_vs_etaphi->Write();
  h_prescale_vs_ptlead->Write();

  h_dimuon_mass_vs_etaphiPos->Write();
  if (h_dimuon_mass_vs_etaphiPos_fitresult != NULL) h_dimuon_mass_vs_etaphiPos_fitresult->Write();
  if (h_dimuon_mass_vs_etaphiPos_fitresultVsPDG != NULL) h_dimuon_mass_vs_etaphiPos_fitresultVsPDG->Write();
  if (h_dimuon_mass_vs_etaphiPos_fitresultVsPDGpc != NULL) h_dimuon_mass_vs_etaphiPos_fitresultVsPDGpc->Write();
  if (h_dimuon_mass_vs_etaphiPos_fiterror != NULL) h_dimuon_mass_vs_etaphiPos_fiterror->Write();
  if (h_dimuon_mass_vs_etaphiPos_fitsigma != NULL) h_dimuon_mass_vs_etaphiPos_fitsigma->Write();
  if (h_dimuon_mass_shfit_significance_vs_etaphiPos != NULL) h_dimuon_mass_shfit_significance_vs_etaphiPos->Write();

  h_dimuon_mass_vs_etaphiNeg->Write();
  if (h_dimuon_mass_vs_etaphiNeg_fitresult != NULL) h_dimuon_mass_vs_etaphiNeg_fitresult->Write();
  if (h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG != NULL) h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG->Write();
  if (h_dimuon_mass_vs_etaphiNeg_fitresultVsPDGpc != NULL) h_dimuon_mass_vs_etaphiNeg_fitresultVsPDGpc->Write();
  if (h_dimuon_mass_vs_etaphiNeg_fiterror != NULL) h_dimuon_mass_vs_etaphiNeg_fiterror->Write();
  if (h_dimuon_mass_vs_etaphiNeg_fitsigma != NULL) h_dimuon_mass_vs_etaphiNeg_fitsigma->Write();
  if (h_dimuon_mass_shfit_significance_vs_etaphiNeg != NULL) h_dimuon_mass_shfit_significance_vs_etaphiNeg->Write();

  // deltaM vs pt
  h_dimuon_DeltaM_vs_ptlead->Write();
  p_dimuon_DeltaM_vs_ptlead->Write();
  h_mcmassbiasmap->Write();

  // pt of pos and neg muon
  if (h_ptpos_vs_etaphi != NULL) h_ptpos_vs_etaphi->Write();
  if (h_ptneg_vs_etaphi != NULL) h_ptneg_vs_etaphi->Write();

  // radial distortion
  h_radialdistortion->Write();
  h_radialdistortion_vs_mass->Write();
  h_radialdistortion_vs_pt->Write();
  h_radialdistortion_vs_pt_prof->Write();
  h_radialdistortion_vs_pt_eta->Write();
  h_radialdistortion_vs_eta_prof->Write();
  h_radialdistortion_vs_etaphi->Write();
  h_radialdistortion_vs_etaphi_Integrated->Write();
  for (int i=0; i < (int) v_radialdistortion_vs_massEta.size(); i++){
    v_radialdistortion_vs_massEta.at(i)->Write();
    v_radialdistortion_vs_ptEta.at(i)->Write();
  }
  if (h_RadialDistortion_input != NULL) h_RadialDistortion_input->Write();
  if (h_radialdistortion_pos_neg != NULL) h_radialdistortion_pos_neg->Write();

  //
  entries->Write();

  lambdacorrections_vs_etaphi->Write();
  lambdacorrections_vs_etaphi_err->Write();
  lambdacorrections_vs_etaphi_RMS->Write();
  lambdacorrections_vs_eta->Write();
  lambdacorrections_vs_eta_RMS->Write();

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

  z0delta_vs_eta->Write();
  z0deltacorrections_vs_eta->Write();
  z0deltacorrections_vs_eta_RMS->Write();

  z0delta->Write();
  z0delta_pos->Write();
  z0delta_neg->Write();


  z0delta_etaphi->Write();
  z0delta_etaphi_pos->Write();
  z0delta_etaphi_neg->Write();

  d0delta_vs_etaphi->Write();
  d0deltacorrections_vs_etaphi->Write();
  d0deltacorrections_vs_etaphi_err->Write();

  d0delta_vs_eta->Write();
  d0deltacorrections_vs_eta->Write();
  d0deltacorrections_vs_eta_RMS->Write();

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

  delta->Write();
  delta_vs_etaphi->Write();
  deltacorrections_vs_etaphi->Write();
  deltacorrections_vs_etaphi_err->Write();

  delta_vs_eta->Write();
  deltacorrections_vs_eta->Write();
  deltacorrections_vs_eta_RMS->Write();
  
  // deltaSagitta maps from deltaSagittaFitter
  if (h_deltaSagittaFitterMaps != NULL) h_deltaSagittaFitterMaps->Write();


  // IP resolution studies
  if (m_RunIPResolStudies) {
    if (m_IPResolStudies != NULL) {
      m_IPResolStudies->GetHistoKTrkd0Factors()->Write();
      m_IPResolStudies->GetHistoKTrkz0Factors()->Write();
      m_IPResolStudies->GetHistoPtEtaEntries()->Write();
      m_IPResolStudies->GetHistoRecod0Sigma()->Write();
      TH2F *d0unf_Pt_Eta_Tight = (TH2F*) m_IPResolStudies->GetHistoRecod0SigmaUnfold()->Clone();
      m_IPResolStudies->GetHistoRecod0SigmaUnfold()->Write();
      m_IPResolStudies->GetHistoRecod0SigmaMuPos()->Write();
      m_IPResolStudies->GetHistoRecod0SigmaMuNeg()->Write();
      m_IPResolStudies->GetHistoRecod0SigmaMuPosUnfold()->Write();
      m_IPResolStudies->GetHistoRecod0SigmaMuNegUnfold()->Write();
      m_IPResolStudies->GetHistoRecoz0Sigma()->Write();
      TH2F *z0unf_Pt_Eta_Tight = (TH2F*) m_IPResolStudies->GetHistoRecoz0SigmaUnfold()->Clone();
      m_IPResolStudies->GetHistoRecoz0SigmaUnfold()->Write();
      m_IPResolStudies->GetHistoRecoz0SigmaMuPos()->Write();
      m_IPResolStudies->GetHistoRecoz0SigmaMuNeg()->Write();
      m_IPResolStudies->GetHistoRecoz0SigmaMuPosUnfold()->Write();
      m_IPResolStudies->GetHistoRecoz0SigmaMuNegUnfold()->Write();
      //mean histos
      m_IPResolStudies->GetHistoRecod0Mean()->Write(); 
      m_IPResolStudies->GetHistoRecoz0Mean()->Write();
      m_IPResolStudies->GetHistoRecod0MeanUnfold()->Write();
      m_IPResolStudies->GetHistoRecoz0MeanUnfold()->Write();
      //chi2 histos
      m_IPResolStudies->GetHistoRecod0Chi2()->Write(); 
      m_IPResolStudies->GetHistoRecoz0Chi2()->Write();
      m_IPResolStudies->GetHistoRecod0Chi2Unfold()->Write();
      m_IPResolStudies->GetHistoRecoz0Chi2Unfold()->Write();
      // K_PV up and down
      TH2F *d0unf_Pt_Eta_kPVErUp_Tight = (TH2F*)m_IPResolStudies->GetHistoRecod0SigmaUnfoldKPVup()->Clone();
      m_IPResolStudies->GetHistoRecod0SigmaUnfoldKPVup()->Write();
      TH2F *d0unf_Pt_Eta_kPVErDw_Tight = (TH2F*)m_IPResolStudies->GetHistoRecod0SigmaUnfoldKPVdown()->Clone();
      m_IPResolStudies->GetHistoRecod0SigmaUnfoldKPVdown()->Write();
      TH2F *z0unf_Pt_Eta_kPVErUp_Tight = (TH2F*)m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKPVup()->Clone();
      m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKPVup()->Write();
      TH2F *z0unf_Pt_Eta_kPVErDw_Tight = (TH2F*)m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKPVdown()->Clone();
      m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKPVdown()->Write();
      // K_trk up and down
      TH2F *d0unf_Pt_Eta_kIterErUp_Tight = (TH2F*)m_IPResolStudies->GetHistoRecod0SigmaUnfoldKtrkup()->Clone();
      m_IPResolStudies->GetHistoRecod0SigmaUnfoldKtrkup()->Write();
      TH2F *d0unf_Pt_Eta_kIterErDw_Tight = (TH2F*)m_IPResolStudies->GetHistoRecod0SigmaUnfoldKtrkdown()->Clone();
      m_IPResolStudies->GetHistoRecod0SigmaUnfoldKtrkdown()->Write();
      TH2F *z0unf_Pt_Eta_kIterErUp_Tight = (TH2F*)m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKtrkup()->Clone();
      m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKtrkup()->Write();
      TH2F *z0unf_Pt_Eta_kIterErDw_Tight = (TH2F*)m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKtrkdown()->Clone();
      m_IPResolStudies->GetHistoRecoz0SigmaUnfoldKtrkdown()->Write();
 
      // fit of impact parameter using wider units of sigma
      TH2F *d0unf_Pt_Eta_widerFitErr_Tight = (TH2F*)m_IPResolStudies->GetHistoRecod0SigmaUnfoldWideFit()->Clone();
      m_IPResolStudies->GetHistoRecod0SigmaUnfoldWideFit()->Write();
      TH2F *z0unf_Pt_Eta_widerFitErr_Tight = (TH2F*)m_IPResolStudies->GetHistoRecoz0SigmaUnfoldWideFit()->Clone();
      m_IPResolStudies->GetHistoRecoz0SigmaUnfoldWideFit()->Write();
      
      // Cloned histos
      if (iteration == m_iterationUserInput) {      
      m_file.mkdir("IPresolutions_d0");
      m_file.mkdir("IPresolutions_z0");      
      m_file.cd("IPresolutions_d0");
      d0unf_Pt_Eta_Tight->SetName("d0unf_Pt_Eta_Tight");
      d0unf_Pt_Eta_Tight->Write();
      d0unf_Pt_Eta_kPVErUp_Tight->SetName("d0unf_Pt_Eta_kPVErUp_Tight");
      d0unf_Pt_Eta_kPVErUp_Tight->Write();
      d0unf_Pt_Eta_kPVErDw_Tight->SetName("d0unf_Pt_Eta_kPVErDw_Tight");
      d0unf_Pt_Eta_kPVErDw_Tight->Write();
      d0unf_Pt_Eta_kIterErUp_Tight->SetName("d0unf_Pt_Eta_kIterErUp_Tight");
      d0unf_Pt_Eta_kIterErUp_Tight->Write();
      d0unf_Pt_Eta_kIterErDw_Tight->SetName("d0unf_Pt_Eta_kIterErDw_Tight");
      d0unf_Pt_Eta_kIterErDw_Tight->Write();
      d0unf_Pt_Eta_widerFitErr_Tight->SetName("d0unf_Pt_Eta_widerFitErr_Tight");
      d0unf_Pt_Eta_widerFitErr_Tight->Write();
      m_file.cd("IPresolutions_z0");
      z0unf_Pt_Eta_Tight->SetName("z0unf_Pt_Eta_Tight");
      z0unf_Pt_Eta_Tight->Write();
      z0unf_Pt_Eta_kPVErUp_Tight->SetName("z0unf_Pt_Eta_kPVErUp_Tight");
      z0unf_Pt_Eta_kPVErUp_Tight->Write();
      z0unf_Pt_Eta_kPVErDw_Tight->SetName("z0unf_Pt_Eta_kPVErDw_Tight");
      z0unf_Pt_Eta_kPVErDw_Tight->Write();
      z0unf_Pt_Eta_kIterErUp_Tight->SetName("z0unf_Pt_Eta_kIterErUp_Tight");
      z0unf_Pt_Eta_kIterErUp_Tight->Write();
      z0unf_Pt_Eta_kIterErDw_Tight->SetName("z0unf_Pt_Eta_kIterErDw_Tight");
      z0unf_Pt_Eta_kIterErDw_Tight->Write();
      z0unf_Pt_Eta_widerFitErr_Tight->SetName("z0unf_Pt_Eta_widerFitErr_Tight");
      z0unf_Pt_Eta_widerFitErr_Tight->Write();

      }
    }
  }

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
	if (m_PrintLevel >= 3) cout << " ** profileZwithIterativeGaussFit ** fitting " << hist->GetName() << "  bin (" << index << ", " << index_y << ") "
				    << " Not enough entries " << current_proj->GetEntries() << " < " << minEntries << endl;
	//current_mu = -999;
	current_mu = 0;
	current_sigma = 0;
	current_err_mu = 1;
	current_err_sigma = 1;
	
	if (fDebug) std::cout<<"WARNING: Only "<<current_proj->GetEntries()<<" entries in bin "<<index<<","<<index_y<< " in histogram " <<hist->GetName()<< std::endl;
	num_skipped++;

      } 
      else {
	if (m_PrintLevel >= 3) cout << " ** profileZwithIterativeGaussFit ** fitting " << hist->GetName() 
				    << "  bin (" << index << ", " << index_y << ") "
				    << "  entries: "<< current_proj->GetEntries()
				    << endl;
	
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
    //sigma_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    //sigma_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    sigma_graph->GetYaxis()->SetTitle(sigma_graph->GetYaxis()->GetTitle());
    sigma_graph->GetXaxis()->SetTitle(sigma_graph->GetXaxis()->GetTitle());
    sigma_graph->SetTitle("");
  }

  if (mu_graph) {
    mu_graph->SetError(errs_mu);
    //mu_graph->SetMaximum(max_mu+0.15*(max_mu - min_mu));
    //mu_graph->SetMinimum(min_mu-0.15*(max_mu - min_mu));
    mu_graph->GetYaxis()->SetTitleOffset(1.5);
    //    mu_graph->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    // mu_graph->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    mu_graph->GetYaxis()->SetTitle(mu_graph->GetYaxis()->GetTitle());
    mu_graph->GetXaxis()->SetTitle(mu_graph->GetXaxis()->GetTitle());
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
  double current_mu = 0;
  double current_sigma = 0;
  double mu_percent_diff;
  double sigma_percent_diff;

  int minEntries = 10;

  if (!hist) {
    if (fDebug) std::cout<< "Error in  Anp::IterativeGaussFit(): Histogram to be fit is missing" <<std::endl;
    return 1;
  }
  if (hist->GetEntries() < minEntries) {
    if (fDebug) std::cout<< "Error in  Anp::IterativeGaussFit(): Histogram has too few entries " << hist->GetEntries() << std::endl;
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

  TCanvas* can_lambdamap = new TCanvas("can_lambdamap","lambda corrections map", 800, 800);
  can_lambdamap->cd(1);
  lambdacorrections_vs_etaphi->DrawCopy("colz");

   return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::ResetHistograms ()
{
  if (m_PrintLevel >= 3) std::cout << " ** ZmumuValidationExample::ResetHistograms ** STARTED ** " << std::endl;

  delta->Reset();
  delta_vs_etaphi->Reset();
  //deltacorrections_vs_eta->Reset();
  //deltacorrections_vs_eta_RMS->Reset();

   delta_vs_eta->Reset();
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
   h_pt_pos_vs_neg->Reset();
   h_mass->Reset();

   delta_vs_etaphi->Reset();

   //deltacorrections_vs_etaphi->Reset();
   //deltacorrections_vs_etaphi_err->Reset();

   lambda_vs_etaphi->Reset();
   //lambdacorrections_vs_etaphi->Reset();
   //lambdacorrections_vs_etaphi_err->Reset();

   lambda_vs_eta->Reset();
   lambda_vs_eta_pos->Reset();
   lambda_vs_eta_neg->Reset();

   //lambdacorrections_vs_eta->Reset();
   //lambdacorrections_vs_eta_RMS->Reset();

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


   d0_vs_etapT->Reset();
   
   d0_vs_eta->Reset();
   d0_vs_pT->Reset();
   
   d0_vs_eta_mean->Reset();
   d0_vs_eta_RMS->Reset();
   d0_vs_eta_chi2->Reset();
   
   d0_vs_pT_mean->Reset();
   d0_vs_pT_RMS->Reset();
   d0_vs_pT_chi2->Reset();
   
   z0_vs_etapT->Reset();
   
   z0_vs_eta->Reset();
   z0_vs_pT->Reset();
   
   z0_vs_eta_mean->Reset();
   z0_vs_eta_RMS->Reset();
   z0_vs_eta_chi2->Reset();
   
   z0_vs_pT_mean->Reset();
   z0_vs_pT_RMS->Reset();
   z0_vs_pT_chi2->Reset();
   
   
   // store the previous iteration values of the radial correction
   // h_radialdistortion_vs_etaphi_PreviousIterValues = (TProfile2D*) h_radialdistortion_vs_etaphi->Clone(); 
   if (h_dimuon_mass_vs_etaphi_fitsigma !=NULL) 
     h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues = (TH2*) h_dimuon_mass_vs_etaphi_fitsigma->Clone(); 
   if (h_dimuon_mass_vs_etaphi_fitresult !=NULL) 
     h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues = (TH2*) h_dimuon_mass_vs_etaphi_fitresult->Clone(); 

   // now reset
   h_radialdistortion->Reset();
   h_radialdistortion_pos_neg->Reset();
   h_radialdistortion_vs_etaphi->Reset();
   h_radialdistortion_vs_mass->Reset();
   h_radialdistortion_vs_pt->Reset();
   h_radialdistortion_vs_pt_prof->Reset();
   h_radialdistortion_vs_pt_eta->Reset();
   h_radialdistortion_vs_eta_prof->Reset();

   h_dimuon_DeltaM_vs_ptlead->Reset();
   p_dimuon_DeltaM_vs_ptlead->Reset();

   // 
   if(h_dimuon_mass_vs_etaphi)    h_dimuon_mass_vs_etaphi->Reset();
   if(h_dimuon_mass_vs_etaphiPos) h_dimuon_mass_vs_etaphiPos->Reset();
   if(h_dimuon_mass_vs_etaphiNeg) h_dimuon_mass_vs_etaphiNeg->Reset();

   h_entries_vs_etaphi->Reset();

   h_dimuon_mass->Reset();
   h_dimuon_pt->Reset();
   h_dimuon_pz->Reset();
   h_dimuon_eta->Reset();
   h_dimuon_phi->Reset();
   h_dimuon_pt_vs_eta->Reset();
   h_dimuon_pt_vs_mass->Reset();
   h_dimuon_deltaphi->Reset();
   h_dimuon_deltaphi_vs_mass->Reset();
   h_dimuon_cosalpha->Reset();
   h_dimuon_cosalpha_vs_mass->Reset();
   h_dimuon_deltaz0_vs_mass->Reset();
   h_dimuon_deltad0_vs_mass->Reset();
   h_dimuon_deltaz0_vs_eta->Reset();
   h_dimuon_ptlead_vs_mass->Reset();
   h_dimuon_deltad0_vs_eta->Reset();
   h_d0Significance_vs_mass->Reset();

   h_prescale_vs_ptlead->Reset();

  if (m_PrintLevel >= 3) std::cout << " ** ZmumuValidationExample::ResetHistograms ** COMPLETED ** " << std::endl;

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

//-----------------------------------------------------------------------------
double ZmumuValidationExample::ComputeDeltaPhi (TLorentzVector* muon_pos, TLorentzVector* muon_neg)
{
  double deltaphi = muon_pos->Phi() - muon_neg->Phi();
  if (muon_neg->Pt() > muon_pos->Pt()) deltaphi = muon_neg->Phi() - muon_pos->Phi();

  if (deltaphi > 3.141593) {
    double newdeltaphi = 2*3.141593 - deltaphi; 
    //    std::cout << " deltaphi was: " << deltaphi << " new deltaphi = " << 2*3.141593 << " - " << deltaphi << " = " << newdeltaphi << std::endl;  
    deltaphi = newdeltaphi;
  }
  if (deltaphi < - 3.141593) {
    double newdeltaphi = 2*3.141593 + deltaphi; 
    //std::cout << " deltaphi was: " << deltaphi << " new deltaphi = " << 2*3.141593 << " + " << deltaphi << " = " << newdeltaphi << std::endl;  
    deltaphi = newdeltaphi;
  }
  if (deltaphi < 0) deltaphi = -deltaphi;

  return deltaphi;
}

//-----------------------------------------------------------------------------
double ZmumuValidationExample::ComputeDeltaPhi (TLorentzVector muon_pos, TLorentzVector muon_neg)
{
  double deltaphi = muon_pos.Phi() - muon_neg.Phi();
  if (muon_neg.Pt() > muon_pos.Pt()) deltaphi = muon_neg.Phi() - muon_pos.Phi();

  if (deltaphi > 3.141593) {
    double newdeltaphi = 2*3.141593 - deltaphi; 
    //    std::cout << " deltaphi was: " << deltaphi << " new deltaphi = " << 2*3.141593 << " - " << deltaphi << " = " << newdeltaphi << std::endl;  
    deltaphi = newdeltaphi;
  }
  if (deltaphi < - 3.141593) {
    double newdeltaphi = 2*3.141593 + deltaphi; 
    //std::cout << " deltaphi was: " << deltaphi << " new deltaphi = " << 2*3.141593 << " + " << deltaphi << " = " << newdeltaphi << std::endl;  
    deltaphi = newdeltaphi;
  }
  if (deltaphi < 0) deltaphi = -deltaphi;

  return deltaphi;
}

//-----------------------------------------------------------------------------
double ZmumuValidationExample::ComputeDeltaEta (TLorentzVector* muon_pos, TLorentzVector* muon_neg)
{
  double theDelta = muon_pos->Eta() - muon_neg->Eta();
  if (muon_neg->Pt() > muon_pos->Pt()) theDelta = muon_neg->Eta() - muon_pos->Eta();

  return theDelta;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::RadialDistortionCorrection (TLorentzVector* v_pos, TLorentzVector* v_neg, TLorentzVector* v_dimuon)
{
  double sec_mass = ZmumuValidationExample::muon_mass;
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) sec_mass = ZmumuValidationExample::pion_mass;

  // there are two models to compute epsilon
  // first: same radial correction to both particles
  // second: each particle has a different radial correction
  enum {SAMEEPSILON, DIFFERENTEPSILON};
  int epsilonCalcModel = SAMEEPSILON;
  epsilonCalcModel = DIFFERENTEPSILON;

  bool dodebug = false;
  static int myIter = -1;
  if (n_iteration != myIter) {dodebug=true; myIter=n_iteration;}

  if (dodebug) std::cout << std::endl << " ** RadialDistortionCorrection ** STARTED ** processing event: " << m_eventNumber 
			 << " at iteration: " << n_iteration 
			 << "  sampleType: "<< m_infoSampleType.at(m_sampleType)  
			 << "  (" << m_sampleType << ")"
			 << std::endl;

  double currentEpsilon = 0.000;
  double currentEpsilonPos = 0.000;
  double currentEpsilonNeg = 0.000;
  double newPx, newPy, newPz;
  int bineta=1, binphi=1;
  double deltaEpsilon = 0.;
  double deltaEpsilonPos = 0.;
  double deltaEpsilonNeg = 0.;

  
  // Find out in which eta-phi bin the dimuon is flying
  bineta = h_radialdistortion_vs_etaphi->GetXaxis()->FindBin(v_dimuon->Eta());
  binphi = h_radialdistortion_vs_etaphi->GetYaxis()->FindBin(v_dimuon->Phi());

  // especial case for eta out of range of the histogram
  if (v_dimuon->Eta() > h_radialdistortion_vs_etaphi->GetXaxis()->GetXmax()) {
    if (dodebug) std::cout << " ** RadialDistortionCorrection ** eta: " << v_dimuon->Eta() << " !!! --> out of bounds" << std::endl;
    bineta = h_radialdistortion_vs_etaphi->GetXaxis()->GetNbins();
  } 
  if (v_dimuon->Eta() < h_radialdistortion_vs_etaphi->GetXaxis()->GetXmin()) {
    if (dodebug) std::cout << " ** RadialDistortionCorrection ** eta: " << v_dimuon->Eta() << " !!! --> out of bounds" << std::endl;
    bineta = 1;
  }
  if (dodebug) std::cout << " ** RadialDistortionCorrection ** This event has eta: " << v_dimuon->Eta() << "  phi: " << v_dimuon->Phi()
			 << "  --> use (eta,phi) bins: ( " << bineta <<", " << binphi <<") of the epsilon table " << std::endl;

  // retrieve epsilon. This is used to correct the four-momentum
  if (n_iteration <= 1) {
    currentEpsilon = 0.000;
    currentEpsilonPos = 0.000;
    currentEpsilonNeg = 0.000;
  }
  else {
    // dimuon (or assume both muons have the same correction)
    currentEpsilon = h_radialdistortion_vs_etaphi_Integrated->GetBinContent(bineta, binphi);
    currentEpsilonPos = currentEpsilon;
    currentEpsilonNeg = currentEpsilon;
    if (epsilonCalcModel == DIFFERENTEPSILON) {
      // mu+
      bineta = h_radialdistortion_vs_etaphi->GetXaxis()->FindBin(v_pos->Eta());
      binphi = h_radialdistortion_vs_etaphi->GetYaxis()->FindBin(v_pos->Phi());
      currentEpsilonPos = h_radialdistortion_vs_etaphi_Integrated->GetBinContent(bineta, binphi);
      // mu-
      bineta = h_radialdistortion_vs_etaphi->GetXaxis()->FindBin(v_pos->Eta());
      binphi = h_radialdistortion_vs_etaphi->GetYaxis()->FindBin(v_pos->Phi());
      currentEpsilonNeg = h_radialdistortion_vs_etaphi_Integrated->GetBinContent(bineta, binphi);
    }
  }

  if (dodebug) std::cout << " ** RadialDistortionCorrection ** ----> input epsilon for this event (from previous iter " << n_iteration-1 <<" map)= " 
			 << currentEpsilon <<  " <---- " << std::endl;

  if(dodebug) {
    std::cout << " ** RadialDistortionCorrection **" << std::endl << "  -- positive muon at input -- " << std::endl;
    // v_pos->Print();
    std::cout << "    muon + (as given at input):  M: " << v_pos->M() << "   Pt: " << v_pos->Pt() << "   Pz: " << v_pos->Pz() << "    E: " <<  v_pos->E() << std::endl;
  }

  // correcting the positive muon
  newPx = v_pos->Px() * (1-2*currentEpsilonPos);
  newPy = v_pos->Py() * (1-2*currentEpsilonPos);
  newPz = v_pos->Pz() * (1-currentEpsilonPos);
  v_pos->SetXYZM(newPx, newPy, newPz, sec_mass);
 
  if (dodebug) {
    std::cout << "    muon + (epsilon:" << std::setw(9) << currentEpsilonPos 
	      << "):  M: " << v_pos->M() 
	      << "   Pt: " << v_pos->Pt() 
	      << "   Pz: " << v_pos->Pz() 
	      << "    E: " << v_pos->E() << std::endl;
  }
  
  // correcting the negative muon
  newPx = v_neg->Px() * (1-2*currentEpsilonNeg);
  newPy = v_neg->Py() * (1-2*currentEpsilonNeg);
  newPz = v_neg->Pz() * (1-currentEpsilonNeg);
  v_neg->SetXYZM(newPx, newPy, newPz, sec_mass);

  if (dodebug) {
    std::cout << "    muon - (epsilon:" << std::setw(9) << currentEpsilonNeg 
	      << "):  M: " << v_neg->M() 
	      << "   Pt: " << v_neg->Pt() 
	      << "   Pz: " << v_neg->Pz() 
	      << "    E: " << v_neg->E() << std::endl;
  }

  // set the new dimuon;
  v_dimuon->SetXYZT(v_pos->Px()+v_neg->Px(), v_pos->Py()+v_neg->Py(), v_pos->Pz()+v_neg->Pz(), v_pos->E()+v_neg->E());
  if (dodebug) {
    std::cout << " ** RadialDistortionCorrection ** " << std::endl 
	      << "  -- dimuon pair after radial correction with epsilon= " << currentEpsilon 
	      << " -- epsilonPos= " << currentEpsilonPos 
	      << "  epsilonNeg= " << currentEpsilonNeg
	      << std::endl;
    std::cout << "     dimuon:  M: " << v_dimuon->M() << "   Pt: " << v_dimuon->Pt() << "   Pz: " << v_dimuon->Pz() << "    E: " <<  v_dimuon->E() << std::endl;
  }

  // before computing the epsilon, one needs to know whith respect to which mass 
  double referenceMass = JPsi1SPDGMass*1000; //in MeV
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) {
    //referenceMass =  1000*m_JPsiMassFit->GetReferenceMass(); // in MeV   
    referenceMass =  1000*m_JPsiMassFit->GetTargetMass(bineta, binphi); // in MeV   
  }
  if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) {
    referenceMass = Upsilon1SPDGMass * 1000; // in MeV
  }
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) {
    referenceMass = KShortPDGMass * 1000; // in MeV
  }
  if (m_sampleType == ZmumuValidationExample::ZSAMPLE) {
    referenceMass =  ZPDGMass*1000; // in MeV     
  }
  if (dodebug) std::cout << " ** RadialDistortionCorrection ** reference Mass for (eta,phi) bin (" 
			 << bineta << ", " << binphi << ") --> " 
			 << referenceMass << " MeV" << std::endl;


  // As for iter==1, there is no mass fit yet --> no epsilon calculation, but fill the histograms
  // if (n_iteration <= 1) {
  if (n_iteration <= 0) {
    deltaEpsilon = 0.;
    h_radialdistortion->Fill(deltaEpsilon); 
    h_radialdistortion_vs_etaphi->Fill(v_dimuon->Eta(), v_dimuon->Phi(), deltaEpsilon);
    h_radialdistortion_vs_mass->Fill(v_dimuon->M(), deltaEpsilon);
    h_radialdistortion_vs_pt->Fill(v_dimuon->Pt()/1000., deltaEpsilon);//in GeV
    h_radialdistortion_vs_pt_prof->Fill(v_dimuon->Pt()/1000., deltaEpsilon);//in GeV
    h_radialdistortion_vs_pt_eta->Fill(v_dimuon->Pt()/1000., v_dimuon->Eta(), deltaEpsilon);
    h_radialdistortion_vs_eta_prof->Fill(v_dimuon->Eta(), deltaEpsilon);
    v_radialdistortion_vs_massEta.at(bineta-1)->Fill(v_dimuon->M(), deltaEpsilon);
    v_radialdistortion_vs_ptEta.at(bineta-1)->Fill(v_dimuon->Pt()/1000., deltaEpsilon);
  }
  else {
    if (epsilonCalcModel == SAMEEPSILON) {
      // compute the epsilon for this muon pair
      double term_num1 = (v_dimuon->M2() - pow(referenceMass,2));  // calculation in MeV
      double term_den1 = v_dimuon->M2();
      double term_box1 = v_pos->E() * v_neg->Perp2() /v_neg->E();
      double term_box2 = v_neg->E() * v_pos->Perp2() /v_pos->E();
      double term_box3 = pow(v_pos->E()+v_neg->E(),2) * pow (sec_mass, 2) / (v_pos->E()*v_neg->E());
      double term_den2 = term_box1 + term_box2 - term_box3;
      double term_den3 = 2* (v_pos->Px() * v_neg->Px() + v_pos->Py() * v_neg->Py());
      
      double numerator = term_num1;
      double denominator = term_den1 + term_den2 - term_den3;
      
      deltaEpsilon = 0.5 * numerator / denominator;
    
      if (dodebug) {
	std::cout << " ** RadialDistortionCorrection ** terms used in epsillon calculation ** NEW ** " << std::endl;
	std::cout << "    (Muud2 - Muu02)     = " << term_num1 << std::endl;
	std::cout << "    Muud2               = " << term_den1 << std::endl;
	std::cout << "    [big term ]         = " << term_den2 << std::endl;
	std::cout << "    2(vec(Pt+)*vec(Pt-))= " << term_den3 << std::endl;
	std::cout << "    ---> New Epsilon = 0.5 * " << numerator << " / " << denominator << " = " << deltaEpsilon <<  std::endl;
	std::cout << "         comparing to reference mass: " << referenceMass << std::endl;
	std::cout << "    ---> New dimuon mass: " << v_dimuon->M() << " MeV   DeltaM " << v_dimuon->M() - referenceMass << " MeV   epsilon= " << deltaEpsilon << std::endl;
      }

      double thisMass = v_dimuon->M(); // Mass is returned in MeV
      double sigmaCutMin =  20.; // sigma cut in MeV
      double sigmaCutMax = 100.;
      double thisSigmaCut = sigmaCutMin;
      if (dodebug) std::cout << "    default sigma = " << thisSigmaCut << std::endl;
      
      if (h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues != NULL) {
	if (dodebug) std::cout << "  - retrieving previous sigma... ";
	thisSigmaCut = 1.0*h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues->GetBinContent(bineta, binphi);
	if (thisSigmaCut < sigmaCutMin) thisSigmaCut = sigmaCutMin;
	if (thisSigmaCut > sigmaCutMax) thisSigmaCut = sigmaCutMax;
	if (dodebug) std::cout << " sigma cut to be used: " << thisSigmaCut << std::endl;
      }
      
      double previousMass = referenceMass;
      if (h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues != NULL) {
	if (dodebug) std::cout << "  - retrieving previous mass... ";
	previousMass = h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues->GetBinContent(bineta, binphi) * 1000; // GeV --> MeV
	if (dodebug) {
	  std::cout << previousMass << "  DeltaM(prev-ref) =  " << previousMass-referenceMass << " MeV ";
	  std::cout << std::endl;
	}
      }
      
      // check if this event dimuon-mass is near the peak
      if (previousMass-thisSigmaCut < thisMass && thisMass < previousMass+thisSigmaCut && fabs(deltaEpsilon)<0.025) {
	if (dodebug) std::cout << "  ======== Mass within sigma cut ========   " << previousMass-thisSigmaCut << " < " << thisMass
			       << " < " << previousMass+thisSigmaCut << "  ==>   epsilon = " << deltaEpsilon << " ======== " << std::endl;
	h_radialdistortion->Fill(deltaEpsilon); 
	h_radialdistortion_vs_etaphi->Fill(v_dimuon->Eta(), v_dimuon->Phi(), deltaEpsilon);
	h_radialdistortion_vs_mass->Fill(v_dimuon->M(), currentEpsilon + deltaEpsilon);
	h_radialdistortion_vs_pt->Fill(v_dimuon->Pt()/1000., currentEpsilon + deltaEpsilon); // quin pt gastar ? el del dimuo o el de cada muo ?
	h_radialdistortion_vs_pt_prof->Fill(v_dimuon->Pt()/1000, currentEpsilon + deltaEpsilon);//in GeV quin pt gastar ? el del dimuo o el de cada muo ?
	h_radialdistortion_vs_pt_eta->Fill(v_dimuon->Pt()/1000., v_dimuon->Eta(), currentEpsilon + deltaEpsilon);
	h_radialdistortion_vs_eta_prof->Fill(v_dimuon->Eta(), currentEpsilon + deltaEpsilon);
	v_radialdistortion_vs_massEta.at(bineta-1)->Fill(v_dimuon->M(), currentEpsilon + deltaEpsilon);
	v_radialdistortion_vs_ptEta.at(bineta-1)->Fill(v_dimuon->Pt()/1000., currentEpsilon + deltaEpsilon);

	// DeltaM vs pt
	float pdgMass = ZmumuValidationExample::ZPDGMass;
	if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) pdgMass = ZmumuValidationExample::JPsi1SPDGMass;
	if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) pdgMass = ZmumuValidationExample::Upsilon1SPDGMass;
	
	float mcMass = this->GetMCbiasMass(v_pos, v_neg);	
	float DeltaM = (v_dimuon->M()/1000 - mcMass) / pdgMass;
	h_dimuon_DeltaM_vs_ptlead->Fill(max(v_pos->Pt(), v_neg->Pt())/1000., DeltaM);
	p_dimuon_DeltaM_vs_ptlead->Fill(max(v_pos->Pt(), v_neg->Pt())/1000., DeltaM);
	if (dodebug) std::cout << "  mcMass= " << mcMass << "  DeltaM= " << DeltaM << std::endl; 

      }
      else {
	if (dodebug) std::cout << "  ======== Mass out of sigma cut ========   " << thisMass 
			       << " is not in [" <<referenceMass-thisSigmaCut 
			       << ", " << referenceMass+thisSigmaCut
			       << "]  epsilon= " << deltaEpsilon
			       << std::endl;
      }
    } // same epsilon

    if (epsilonCalcModel == DIFFERENTEPSILON) {
      double DeltaMSquared = pow(referenceMass,2)-v_dimuon->M2();  // calculation in MeV
      double Vecpd1Vecpd2  = v_pos->Px() * v_neg->Px() + v_pos->Py() * v_neg->Py() + v_pos->Pz() * v_neg->Pz();
      double Vecptd1Vecptd2= v_pos->Px() * v_neg->Px() + v_pos->Py() * v_neg->Py(); 
      double extratermPos = v_neg->E() * (pow(sec_mass,2) - pow(v_pos->Pt(),2) - pow(v_pos->E(),2))/v_pos->E();
      double extratermNeg = v_pos->E() * (pow(sec_mass,2) - pow(v_neg->Pt(),2) - pow(v_neg->E(),2))/v_neg->E();
      double boxPos = Vecpd1Vecpd2 + Vecptd1Vecptd2 + extratermPos;
      double boxNeg = Vecpd1Vecpd2 + Vecptd1Vecptd2 + extratermNeg;

      deltaEpsilonPos = (1./4.) * DeltaMSquared / boxPos;
      deltaEpsilonNeg = (1./4.) * DeltaMSquared / boxNeg;
      if (dodebug) {
	std::cout << " ** RadialDistortionCorrection ** terms used in epsillon calculation ** DIFFERENT EPSILON ** " << std::endl;
	std::cout << "    Muud                = " << v_dimuon->M() << std::endl;
	std::cout << "    Muu0                = " << referenceMass << std::endl;
	std::cout << "    v(pd+)·v(pd-)       = " << Vecpd1Vecpd2 << std::endl;
	std::cout << "    v(ptd+)·v(ptd-)     = " << Vecptd1Vecptd2 << std::endl;
	std::cout << "    box+                = " << boxPos << std::endl;
	std::cout << "    box-                = " << boxNeg << std::endl;
	std::cout << "    epsilon+            = " << deltaEpsilonPos << std::endl;
	std::cout << "    epsilon-            = " << deltaEpsilonNeg << std::endl;
      }
      double thisMass = v_dimuon->M(); // Mass is returned in MeV
      double sigmaCutMin =  20.; // sigma cut in MeV
      double sigmaCutMax = 100.;
      double thisSigmaCut = sigmaCutMin;
      if (dodebug) std::cout << "    default sigma = " << thisSigmaCut << std::endl;

      if (h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues != NULL) {
	if (dodebug) std::cout << "  - retrieving previous sigma... ";
	thisSigmaCut = 1.0*h_dimuon_mass_vs_etaphi_fitsigma_PreviousIterValues->GetBinContent(bineta, binphi);
	if (thisSigmaCut < sigmaCutMin) thisSigmaCut = sigmaCutMin;
	if (thisSigmaCut > sigmaCutMax) thisSigmaCut = sigmaCutMax;
	if (dodebug) std::cout << " sigma cut to be used: " << thisSigmaCut << std::endl;
      }
      double previousMass = referenceMass;
      if (h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues != NULL) {
	if (dodebug) std::cout << "  - retrieving previous mass... ";
	previousMass = h_dimuon_mass_vs_etaphi_fitresult_PreviousIterValues->GetBinContent(bineta, binphi) * 1000; // GeV --> MeV
	if (dodebug) {
	  std::cout << previousMass << "  DeltaM(prev-ref) =  " << previousMass-referenceMass << " MeV ";
	  std::cout << std::endl;
	}
      }
      
      // check if this event dimuon-mass is near the peak
      if (previousMass-thisSigmaCut < thisMass && thisMass < previousMass+thisSigmaCut 
	  && fabs(deltaEpsilonPos)<0.025
	  && fabs(deltaEpsilonNeg)<0.025
	  && fabs(deltaEpsilonPos-deltaEpsilonPos) < 0.0025) {
	
	if ((thisMass - referenceMass) < 0 && (deltaEpsilonPos+deltaEpsilonNeg) > 0.000) return;
	if ((thisMass - referenceMass) > 0 && (deltaEpsilonPos+deltaEpsilonNeg) < 0.000) return;


	if (dodebug) std::cout << "  ======== Mass within sigma cut ========   " << referenceMass-thisSigmaCut << " < " << thisMass
			       << " < " << referenceMass+thisSigmaCut << std::endl
			       << "                                            epsilon+ = " << deltaEpsilonPos << std::endl
			       << "                                            epsilon- = " << deltaEpsilonNeg << std::endl;
	h_radialdistortion->Fill(deltaEpsilonPos); 
	h_radialdistortion->Fill(deltaEpsilonNeg); 

	h_radialdistortion_pos_neg->Fill(deltaEpsilonPos, deltaEpsilonNeg);

	h_radialdistortion_vs_etaphi->Fill(v_pos->Eta(), v_pos->Phi(), deltaEpsilonPos); 
	h_radialdistortion_vs_etaphi->Fill(v_neg->Eta(), v_neg->Phi(), deltaEpsilonNeg); 
	h_radialdistortion_vs_mass->Fill(v_dimuon->M(), currentEpsilonPos + deltaEpsilonPos); 
	h_radialdistortion_vs_mass->Fill(v_dimuon->M(), currentEpsilonNeg + deltaEpsilonNeg);
	
	h_radialdistortion_vs_pt->Fill(v_pos->Pt()/1000., currentEpsilonPos + deltaEpsilonPos); 
	h_radialdistortion_vs_pt->Fill(v_neg->Pt()/1000., currentEpsilonNeg + deltaEpsilonNeg);

	h_radialdistortion_vs_pt_prof->Fill(v_pos->Pt()/1000, currentEpsilon + deltaEpsilonPos);//pt in GeV
	h_radialdistortion_vs_pt_prof->Fill(v_neg->Pt()/1000, currentEpsilon + deltaEpsilonNeg);
	h_radialdistortion_vs_pt_eta->Fill(v_pos->Pt()/1000., v_pos->Eta(), currentEpsilonPos + deltaEpsilonPos);
	h_radialdistortion_vs_pt_eta->Fill(v_neg->Pt()/1000., v_neg->Eta(), currentEpsilonNeg + deltaEpsilonNeg);

	h_radialdistortion_vs_eta_prof->Fill(v_pos->Eta(), currentEpsilonPos + deltaEpsilonPos);
	h_radialdistortion_vs_eta_prof->Fill(v_neg->Eta(), currentEpsilonNeg + deltaEpsilonNeg);

	// be ware: 
	// if mass is below target --> epsilon should be negative
	// if mass is above the target --> epsilon should be positive
	// the contrary means the assumption that half correction goes to each is really wrong. Not even approximate
	v_radialdistortion_vs_massEta.at(bineta-1)->Fill(v_dimuon->M(), currentEpsilonPos + deltaEpsilonPos);
	v_radialdistortion_vs_massEta.at(bineta-1)->Fill(v_dimuon->M(), currentEpsilonNeg + deltaEpsilonNeg);
	v_radialdistortion_vs_ptEta.at(bineta-1)->Fill(v_pos->Pt()/1000., currentEpsilonPos + deltaEpsilonPos);
	v_radialdistortion_vs_ptEta.at(bineta-1)->Fill(v_neg->Pt()/1000., currentEpsilonNeg + deltaEpsilonNeg);

	// DeltaM vs pt
	float pdgMass = ZmumuValidationExample::ZPDGMass;
	if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) pdgMass = ZmumuValidationExample::JPsi1SPDGMass;
	if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) pdgMass = ZmumuValidationExample::Upsilon1SPDGMass;
	
	float mcMass = this->GetMCbiasMass(v_pos, v_neg);	
	float DeltaM = (v_dimuon->M()/1000 - mcMass) / pdgMass;
	h_dimuon_DeltaM_vs_ptlead->Fill(max(v_pos->Pt(), v_neg->Pt())/1000., DeltaM);
	p_dimuon_DeltaM_vs_ptlead->Fill(max(v_pos->Pt(), v_neg->Pt())/1000., DeltaM);
	if (dodebug) std::cout << "  mcMass= " << mcMass << "  DeltaM= " << DeltaM << std::endl; 
      }
      else {
	if (dodebug) std::cout << "  ======== Mass out of sigma cut ========   " << thisMass 
			       << " is not in [" <<referenceMass-thisSigmaCut 
			       << ", " << referenceMass+thisSigmaCut
			       << "]  epsilon+= " << deltaEpsilonPos 
			       << "  epsilon-=" << deltaEpsilonNeg
			       << std::endl;
      }

    } // different epsilon (epsilon+ and epsilon-)

  }

  if (dodebug) {
    std::cout << " ** RadialDistortionCorrection ** COMPLETED ** processing of event " << m_eventNumber 
	      << " at iteration: " << n_iteration << std::endl
	      << "              --> epsilon: " << deltaEpsilon
	      << "  |||  epsilon+: " << deltaEpsilonPos 
	      << "  epsilon-: " << deltaEpsilonNeg
	      << std::endl;
  }
  return;
}
//-----------------------------------------------------------------------------
void ZmumuValidationExample::IntegratedRadialDistortions()
{
  if (m_PrintLevel >= 0) std::cout << " ** ZmumuValidationExample::IntegratedRadialDistortions ** START ** " << std::endl;

  double current_value, thisiter_value, new_value=0, new_error=0;
  enum {HISTOGRAMMEAN, GAUSSIANMU, LINEARFIT};
  int computingMethod = LINEARFIT;
  
  if (m_PrintLevel >= 5) std::cout << " ** ZmumuValidationExample::IntegratedRadialDistortions ** computingMethod " << computingMethod << std::endl;

  if (h_radialdistortion_vs_etaphi != NULL) { // check if histogram exists
    if (h_radialdistortion_vs_etaphi->GetEntries() > 0 ) { // check the radial distortion calculation was activated 

      // loop over this iter radial distortions eta-phi map and fill the integrated one;
      for (int etabin=1; etabin <= h_radialdistortion_vs_etaphi->GetNbinsX(); etabin++) {
	for (int phibin=1; phibin <= h_radialdistortion_vs_etaphi->GetNbinsY(); phibin++) {
	  if (m_PrintLevel >=5) std::cout << std::endl << " ** IntegratedRadialDistortions ** dealing with (eta,phi)= (" << etabin <<", " << phibin <<")" << std::endl; 

	  if (computingMethod == HISTOGRAMMEAN) {
	    current_value = h_radialdistortion_vs_etaphi_Integrated->GetBinContent(etabin,phibin);
	    thisiter_value = h_radialdistortion_vs_etaphi->GetBinContent(etabin,phibin);
	    new_value = current_value + thisiter_value;
	    new_error = h_radialdistortion_vs_etaphi->GetBinError(etabin,phibin);
	    if (m_PrintLevel >= 5) std::cout << " ** MEAN ** IntegratedRadialDistortions ** etabin: " << etabin 
					     << " phibin: " << phibin
					     << "  current value: " << current_value
					     << "  this iter: " << thisiter_value
					     << "  new: " << new_value
					     << " +- " << new_error
					     << std::endl;  
	  }      
	  
	  if (computingMethod == GAUSSIANMU) {
	    current_value = h_radialdistortion_vs_etaphi_Integrated->GetBinContent(etabin,phibin);
	    h_radialdistortion->Fit("gaus");
	    thisiter_value = h_radialdistortion->GetFunction("gaus")->GetParameter(1);
	    new_value = current_value + thisiter_value;
	    new_error = h_radialdistortion_vs_etaphi->GetBinError(etabin,phibin);
	  
	    if (m_PrintLevel >= 5) std::cout << " **  FIT ** IntegratedRadialDistortions ** etabin: " << etabin 
					     << " phibin: " << phibin
					     << "  current value: " << current_value
					     << "  this iter: " << thisiter_value
					     << "  new: " << new_value
					     << " +- " << new_error
					     << std::endl;
	  }
	
	  if (computingMethod == LINEARFIT) {
	    float lowerRange = 0.;
	    float upperRange = 100000.;
	    if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) { 
	      lowerRange = 9400.;
	      upperRange = 9480.;
	    }
	    if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) { 
	      lowerRange = 3080.;
	      upperRange = 3120.;
	    }
	    if (m_sampleType == ZmumuValidationExample::ZSAMPLE) {
	      lowerRange = 90000.;
	      upperRange = 92000.;
	    }

	    if (h_radialdistortion_vs_mass == NULL ||
		h_dimuon_mass_vs_etaphi_fitresult == NULL) {
	      if (m_PrintLevel >= 0) std::cout << " ** IntegratedRadialDistortions ** Early exit due to non existing histo ** " << std::endl;
	      return;
	    }
	    else {
	      if (m_PrintLevel >= 5) std::cout << " ** IntegratedRadialDistortions ** continue ... range:" << lowerRange
					       << " --> " << upperRange
					       << std::endl;
	    }
	      
	    h_radialdistortion_vs_mass->Fit("pol1","RQ","", lowerRange, upperRange);
	    v_radialdistortion_vs_massEta.at(etabin-1)->Fit("pol1","RQ","", lowerRange, upperRange);

	    double slope = v_radialdistortion_vs_massEta.at(etabin-1)->GetFunction("pol1")->GetParameter(1);
	    double slope_err = v_radialdistortion_vs_massEta.at(etabin-1)->GetFunction("pol1")->GetParError(1);
	    double intercept = v_radialdistortion_vs_massEta.at(etabin-1)->GetFunction("pol1")->GetParameter(0);
	    double thismass = h_dimuon_mass_vs_etaphi_fitresult->GetBinContent(etabin, phibin) * 1000.; // GeV to MeV
	    double thismass_err = h_dimuon_mass_vs_etaphi_fiterror->GetBinContent(etabin,phibin); // in MeV
	    double targetmass =  1000*m_JPsiMassFit->GetTargetMass(etabin, phibin); // in MeV   
	  
	    thisiter_value = slope * (thismass-targetmass);
	    
	    // discard silly values:
	    if (fabs(thisiter_value)>0.025) thisiter_value = 0.;

	    current_value = h_radialdistortion_vs_etaphi_Integrated->GetBinContent(etabin,phibin);
	    new_value = current_value + thisiter_value;

	    new_error = sqrt( pow((thismass-targetmass)*slope_err,2) + pow(slope*thismass_err,2) ); //propagate thisiter_value

	    if (m_PrintLevel >= 5) std::cout << " ** LINEAR FIT ** current mass: " << thismass 
					     << "   target mass: " << targetmass 
					     << "   delta mass: " << thismass-targetmass
					     << std::endl
					     << "                  current epsilon: " << current_value
					     << "   delta epsilon= " << thisiter_value
					     << "   new epsilon= " << new_value
					     << std::endl;

	    if (m_PrintLevel >= 5) std::cout << " ** LINEAR FIT ** from line: " << slope * thismass + intercept 
					     << " wrt to target : " << slope * (thismass-targetmass) 
					     << "   Delta: " << slope * thismass + intercept - slope * (thismass-targetmass) 
					     << std::endl;
	    if (m_PrintLevel >= 5) std::cout << " ** LINEAR FIT ** IntegratedRadialDistortions ** etabin: " << etabin 
					     << " phibin: " << phibin
					     << " " << slope << " * " << thismass << " + " << intercept << " = "
					     << slope * thismass + intercept
					     << "  current value: " << current_value
					     << "  new: " << current_value + slope * thismass + intercept
					     << " +- " << new_error
					     << std::endl;
	    if (m_PrintLevel >= 5) std::cout << " ** TARGET MASS** IntegratedRadialDistortions ** etabin: " << etabin 
					     << " phibin: " << phibin
					     << " " << slope << " * " << targetmass << " + " << intercept << " = "
					     << slope * targetmass + intercept
					     << "  current value: " << current_value
					     << "  new: " << current_value + slope * targetmass + intercept
					     << " +- " << new_error
					     << std::endl << std::endl;
	  }
	
	  h_radialdistortion_vs_etaphi_Integrated->SetBinContent(etabin, phibin, new_value);
	  h_radialdistortion_vs_etaphi_Integrated->SetBinError(etabin, phibin, new_error);
	}
      }
    } // histogram is filled
  } // histogram exists

  if (m_PrintLevel >=0) {
    std::cout << " ** IntegratedRadialDistortions ** FINAL RESULT **" << std::endl;
    for (int etabin=1; etabin <= h_radialdistortion_vs_etaphi->GetNbinsX(); etabin++) {
      for (int phibin=1; phibin <= h_radialdistortion_vs_etaphi->GetNbinsY(); phibin++) {
	std::cout << "  Integrated epsilon (" << etabin << ", " << phibin 
		  << ") = " << setw(6) << h_radialdistortion_vs_etaphi_Integrated->GetBinContent(etabin, phibin)
		  << "  +-  " << h_radialdistortion_vs_etaphi_Integrated->GetBinError(etabin, phibin)
		  << std::endl;
      }
    }
  }


  if (m_PrintLevel >= 5) std::cout << " ** ZmumuValidationExample::IntegratedRadialDistortions ** COMPLETED ** " << std::endl;

  return;
}

//-----------------------------------------------------------------------------
void ZmumuValidationExample::SetMCbias(int sampletype)
{
  bool methodknown = false;
  if (sampletype == ZmumuValidationExample::JPSISAMPLE) {
    methodknown = true;
    this->SetMCbiasJPsi();
  }
  
  if (sampletype == ZmumuValidationExample::UPSILONSAMPLE) {
    methodknown = true;
    this->SetMCbiasUpsilon();
  }

  if (sampletype == ZmumuValidationExample::ZSAMPLE) {
    methodknown = true;
    this->SetMCbiasZ();
  }

  if (!methodknown) {
    std::cout << " ** ZmumuValidationExample::SetMCbias ** unknown method " << std::endl;
  }
  return;
}

//----------------------------------------------------------------------------
void ZmumuValidationExample::SetMCbiasJPsi()
{
  // here one defines a map in eta and phi of possible MC biases
  // The map depends on the number of eta and phi histogram used
 
  // WARNING Set values in GeV
  bool mapset = false;
  h_mcmassbiasmap = new TH2F("h_mcmassbiasmap","J/#psi MC mass bias map",
			     m_EtaBins, 
			     h_dimuon_mass_vs_etaphi->GetXaxis()->GetXmin(),
			     h_dimuon_mass_vs_etaphi->GetXaxis()->GetXmax(),
			     m_PhiBins, 
			     h_dimuon_mass_vs_etaphi->GetYaxis()->GetXmin(),
			     h_dimuon_mass_vs_etaphi->GetYaxis()->GetXmax());
  
  if (m_EtaBins == 1 && m_PhiBins == 1) {
    mapset = true;
    //h_mcmassbiasmap->SetBinContent(1,1, 0.00308); // in GeV
    h_mcmassbiasmap->SetBinContent(1,1, 0.0000); // in GeV
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 3 && m_PhiBins == 1) {
    mapset = true;
    h_mcmassbiasmap->SetBinContent(1,1, 0.000657); // in GeV
    h_mcmassbiasmap->SetBinContent(2,1,-0.001921);
    h_mcmassbiasmap->SetBinContent(3,1,-0.000267);
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 3 && m_PhiBins == 3) {
    mapset = true;
    h_mcmassbiasmap->SetBinContent(1,1, 0.0006253); // in GeV
    h_mcmassbiasmap->SetBinContent(2,1,-0.0018587);
    h_mcmassbiasmap->SetBinContent(3,1,-0.0002319);
    h_mcmassbiasmap->SetBinContent(1,2, 0.0005993); // in GeV
    h_mcmassbiasmap->SetBinContent(2,2,-0.0021253);
    h_mcmassbiasmap->SetBinContent(3,2,-0.0003251);
    h_mcmassbiasmap->SetBinContent(1,3, 0.0007452); // in GeV
    h_mcmassbiasmap->SetBinContent(2,3,-0.0019254);
    h_mcmassbiasmap->SetBinContent(3,3,-0.0002481);

    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }

  if (m_EtaBins == 4 && m_PhiBins == 4) {
    mapset = true;
    h_mcmassbiasmap->SetBinContent(1,1, 0.0014866); // in GeV
    h_mcmassbiasmap->SetBinContent(2,1,-0.0014940);
    h_mcmassbiasmap->SetBinContent(3,1,-0.0018041);
    h_mcmassbiasmap->SetBinContent(4,1, 0.0006929);
    h_mcmassbiasmap->SetBinContent(1,2, 0.0015791); // in GeV
    h_mcmassbiasmap->SetBinContent(2,2,-0.0014463);
    h_mcmassbiasmap->SetBinContent(3,2,-0.0018142);
    h_mcmassbiasmap->SetBinContent(4,2, 0.0006500);
    h_mcmassbiasmap->SetBinContent(1,3, 0.0015591); // in GeV
    h_mcmassbiasmap->SetBinContent(2,3,-0.0014709);
    h_mcmassbiasmap->SetBinContent(3,3,-0.0018138);
    h_mcmassbiasmap->SetBinContent(4,3, 0.0006696);
    h_mcmassbiasmap->SetBinContent(1,4, 0.0017242); // in GeV
    h_mcmassbiasmap->SetBinContent(2,4,-0.0013788);
    h_mcmassbiasmap->SetBinContent(3,4,-0.0019454);
    h_mcmassbiasmap->SetBinContent(4,4, 0.0007193);

    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 3 && m_PhiBins == 6) {
    mapset = true;
    for( int phibin = 1; phibin <= m_PhiBins; phibin++) {
      h_mcmassbiasmap->SetBinContent(1,phibin, 0.);//0.00188); // in GeV
      h_mcmassbiasmap->SetBinContent(2,phibin, 0.);//-0.00095);
      h_mcmassbiasmap->SetBinContent(3,phibin, 0.);//-0.000429);
    }
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }

  if (m_EtaBins == 11 && m_PhiBins > 0) { // 11 eta bins
    mapset = true;
    for( int phibin = 1; phibin <= m_PhiBins; phibin++) {
      h_mcmassbiasmap->SetBinContent( 1, phibin, 0.005791); // in GeV
      h_mcmassbiasmap->SetBinContent( 2, phibin, 0.001654);
      h_mcmassbiasmap->SetBinContent( 3, phibin, 0.000047);
      h_mcmassbiasmap->SetBinContent( 4, phibin,-0.000804);
      h_mcmassbiasmap->SetBinContent( 5, phibin,-0.002059);
      h_mcmassbiasmap->SetBinContent( 6, phibin,-0.002133);
      h_mcmassbiasmap->SetBinContent( 7, phibin,-0.001930);
      h_mcmassbiasmap->SetBinContent( 8, phibin,-0.001664);
      h_mcmassbiasmap->SetBinContent( 9, phibin,-0.001057);
      h_mcmassbiasmap->SetBinContent(10, phibin, 0.000771);
      h_mcmassbiasmap->SetBinContent(11, phibin, 0.005234);
    }
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 12 && m_PhiBins == 8) {
    mapset = true;
    h_mcmassbiasmap->Reset();
    h_mcmassbiasmap->SetBinContent( 1, 1,  0.0063018);
    h_mcmassbiasmap->SetBinContent( 2, 1,  0.0018091);
    h_mcmassbiasmap->SetBinContent( 3, 1, -0.0000232);
    h_mcmassbiasmap->SetBinContent( 4, 1, -0.0005568);
    h_mcmassbiasmap->SetBinContent( 5, 1, -0.0018146);
    h_mcmassbiasmap->SetBinContent( 6, 1, -0.0019255);
    h_mcmassbiasmap->SetBinContent( 7, 1, -0.0021025);
    h_mcmassbiasmap->SetBinContent( 8, 1, -0.0013904);
    h_mcmassbiasmap->SetBinContent( 9, 1, -0.0017327);
    h_mcmassbiasmap->SetBinContent(10, 1, -0.0006371);
    h_mcmassbiasmap->SetBinContent(11, 1,  0.0013042);
    h_mcmassbiasmap->SetBinContent(12, 1,  0.0054972);
    
    h_mcmassbiasmap->SetBinContent( 1, 2,  0.0062759);
    h_mcmassbiasmap->SetBinContent( 2, 2,  0.0021575);
    h_mcmassbiasmap->SetBinContent( 3, 2,  0.0002282);
    h_mcmassbiasmap->SetBinContent( 4, 2, -0.0004410);
    h_mcmassbiasmap->SetBinContent( 5, 2, -0.0016962);
    h_mcmassbiasmap->SetBinContent( 6, 2, -0.0020002);
    h_mcmassbiasmap->SetBinContent( 7, 2, -0.0017157);
    h_mcmassbiasmap->SetBinContent( 8, 2, -0.0017411);
    h_mcmassbiasmap->SetBinContent( 9, 2, -0.0014398);
    h_mcmassbiasmap->SetBinContent(10, 2, -0.0009174);
    h_mcmassbiasmap->SetBinContent(11, 2,  0.0008868);
    h_mcmassbiasmap->SetBinContent(12, 2,  0.0061736);
    
    h_mcmassbiasmap->SetBinContent( 1, 3,  0.0059533);
    h_mcmassbiasmap->SetBinContent( 2, 3,  0.0024405);
    h_mcmassbiasmap->SetBinContent( 3, 3,  0.0004483);
    h_mcmassbiasmap->SetBinContent( 4, 3, -0.0006800);
    h_mcmassbiasmap->SetBinContent( 5, 3, -0.0015104);
    h_mcmassbiasmap->SetBinContent( 6, 3, -0.0022564);
    h_mcmassbiasmap->SetBinContent( 7, 3, -0.0021583);
    h_mcmassbiasmap->SetBinContent( 8, 3, -0.0017984);
    h_mcmassbiasmap->SetBinContent( 9, 3, -0.0014943);
    h_mcmassbiasmap->SetBinContent(10, 3, -0.0007742);
    h_mcmassbiasmap->SetBinContent(11, 3,  0.0013927);
    h_mcmassbiasmap->SetBinContent(12, 3,  0.0060892);

    h_mcmassbiasmap->SetBinContent( 1, 4,  0.0052705);
    h_mcmassbiasmap->SetBinContent( 2, 4,  0.0015885);
    h_mcmassbiasmap->SetBinContent( 3, 4,  0.0005767);
    h_mcmassbiasmap->SetBinContent( 4, 4, -0.0003923);
    h_mcmassbiasmap->SetBinContent( 5, 4, -0.0016645);
    h_mcmassbiasmap->SetBinContent( 6, 4, -0.0020897);
    h_mcmassbiasmap->SetBinContent( 7, 4, -0.0019022);
    h_mcmassbiasmap->SetBinContent( 8, 4, -0.0017663);
    h_mcmassbiasmap->SetBinContent( 9, 4, -0.0013222);
    h_mcmassbiasmap->SetBinContent(10, 4, -0.0011173);
    h_mcmassbiasmap->SetBinContent(11, 4,  0.0010643);
    h_mcmassbiasmap->SetBinContent(12, 4,  0.0049215);

    h_mcmassbiasmap->SetBinContent( 1, 5,  0.0062427);
    h_mcmassbiasmap->SetBinContent( 2, 5,  0.0022182);
    h_mcmassbiasmap->SetBinContent( 3, 5,  0.0003031);
    h_mcmassbiasmap->SetBinContent( 4, 5, -0.0005872);
    h_mcmassbiasmap->SetBinContent( 5, 5, -0.0016638);
    h_mcmassbiasmap->SetBinContent( 6, 5, -0.0021005);
    h_mcmassbiasmap->SetBinContent( 7, 5, -0.0018820);
    h_mcmassbiasmap->SetBinContent( 8, 5, -0.0019586);
    h_mcmassbiasmap->SetBinContent( 9, 5, -0.0014348);
    h_mcmassbiasmap->SetBinContent(10, 5, -0.0007710);
    h_mcmassbiasmap->SetBinContent(11, 5,  0.0012458);
    h_mcmassbiasmap->SetBinContent(12, 5,  0.0060923);

    h_mcmassbiasmap->SetBinContent( 1, 6,  0.0054178);
    h_mcmassbiasmap->SetBinContent( 2, 6,  0.0020473);
    h_mcmassbiasmap->SetBinContent( 3, 6,  0.0000681);
    h_mcmassbiasmap->SetBinContent( 4, 6, -0.0006145);
    h_mcmassbiasmap->SetBinContent( 5, 6, -0.0014947);
    h_mcmassbiasmap->SetBinContent( 6, 6, -0.0021058);
    h_mcmassbiasmap->SetBinContent( 7, 6, -0.0018207);
    h_mcmassbiasmap->SetBinContent( 8, 6, -0.0019624);
    h_mcmassbiasmap->SetBinContent( 9, 6, -0.0012605);
    h_mcmassbiasmap->SetBinContent(10, 6, -0.0009436);
    h_mcmassbiasmap->SetBinContent(11, 6,  0.0012189);
    h_mcmassbiasmap->SetBinContent(12, 6,  0.0051504);

    h_mcmassbiasmap->SetBinContent( 1, 7,  0.0062879);
    h_mcmassbiasmap->SetBinContent( 2, 7,  0.0022693);
    h_mcmassbiasmap->SetBinContent( 3, 7,  0.0002587);
    h_mcmassbiasmap->SetBinContent( 4, 7, -0.0001312);
    h_mcmassbiasmap->SetBinContent( 5, 7, -0.0016649);
    h_mcmassbiasmap->SetBinContent( 6, 7, -0.0021791);
    h_mcmassbiasmap->SetBinContent( 7, 7, -0.0019351);
    h_mcmassbiasmap->SetBinContent( 8, 7, -0.0017767);
    h_mcmassbiasmap->SetBinContent( 9, 7, -0.0015619);
    h_mcmassbiasmap->SetBinContent(10, 7, -0.0009596);
    h_mcmassbiasmap->SetBinContent(11, 7,  0.0013991);
    h_mcmassbiasmap->SetBinContent(12, 7,  0.0055196);

    h_mcmassbiasmap->SetBinContent( 1, 8,  0.0069479);
    h_mcmassbiasmap->SetBinContent( 2, 8,  0.0023946);
    h_mcmassbiasmap->SetBinContent( 3, 8,  0.0000428);
    h_mcmassbiasmap->SetBinContent( 4, 8, -0.0004667);
    h_mcmassbiasmap->SetBinContent( 5, 8, -0.0015263);
    h_mcmassbiasmap->SetBinContent( 6, 8, -0.0022327);
    h_mcmassbiasmap->SetBinContent( 7, 8, -0.0022915);
    h_mcmassbiasmap->SetBinContent( 8, 8, -0.0020015);
    h_mcmassbiasmap->SetBinContent( 9, 8, -0.0015784);
    h_mcmassbiasmap->SetBinContent(10, 8, -0.0008595);
    h_mcmassbiasmap->SetBinContent(11, 8,  0.0009392);
    h_mcmassbiasmap->SetBinContent(12, 8,  0.0063127);
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }

  if (m_EtaBins == 14 && m_PhiBins == 14) {
    mapset = true;
    for( int phibin = 1; phibin <= m_PhiBins; phibin++) {
      h_mcmassbiasmap->SetBinContent(1,phibin, 0.0002743);//0.074);//0.072);// in GeV
      h_mcmassbiasmap->SetBinContent(2,phibin, -0.00193957);//0.077);//0.061);
      h_mcmassbiasmap->SetBinContent(3,phibin, -0.00235117);//0.063);//0.096);
      h_mcmassbiasmap->SetBinContent(4,phibin, -0.0020198);//0.063);//0.071);
      h_mcmassbiasmap->SetBinContent(5,phibin, -0.0019043);//0.101);//0.074);
      h_mcmassbiasmap->SetBinContent(6,phibin, -0.00234);//0.060);//0.085);
      h_mcmassbiasmap->SetBinContent(7,phibin, -0.002791);//0.092);
      h_mcmassbiasmap->SetBinContent(8,phibin, -0.002172);//0.073);
      h_mcmassbiasmap->SetBinContent(9,phibin, -0.002329);//0.072);
      h_mcmassbiasmap->SetBinContent(10,phibin, -0.002739);//0.088);
      h_mcmassbiasmap->SetBinContent(11,phibin, -0.0030986);//0.088);
      h_mcmassbiasmap->SetBinContent(12,phibin, -0.003196);//0.088);
      h_mcmassbiasmap->SetBinContent(13,phibin, -0.002950);//0.088);
      h_mcmassbiasmap->SetBinContent(14,phibin, -0.0000656);//0.088);
    }
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (!mapset) {
    std::cout << " ** ZmumuValidationExample::SetMCbiasJPsi ** WARNING ** no MC bias map  " << std::endl;
  }
  return;
}

//----------------------------------------------------------------------------
void ZmumuValidationExample::SetMCbiasUpsilon()
{
  // here one defines a map in eta and phi of possible MC biases
  // The map depends on the number of eta and phi histogram used
 
  // WARNING Set values in GeV
  bool mapset = false;
  h_mcmassbiasmap = new TH2F("h_mcmassbiasmap","Upsilon MC mass bias map",
			     m_EtaBins, 
			     h_dimuon_mass_vs_etaphi->GetXaxis()->GetXmin(),
			     h_dimuon_mass_vs_etaphi->GetXaxis()->GetXmax(),
			     m_PhiBins, 
			     h_dimuon_mass_vs_etaphi->GetYaxis()->GetXmin(),
			     h_dimuon_mass_vs_etaphi->GetYaxis()->GetXmax());
  
  if (m_EtaBins == 1 && m_PhiBins == 1) {
    mapset = true;
    //h_mcmassbiasmap->SetBinContent(1,1, 0.00308); // in GeV
    h_mcmassbiasmap->SetBinContent(1,1, 0.0000); // in GeV
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 3 && m_PhiBins == 1) {
    mapset = true;
    h_mcmassbiasmap->SetBinContent(1,1, 0.);//-0.022); // in GeV
    h_mcmassbiasmap->SetBinContent(2,1, 0.);//-0.021);
    h_mcmassbiasmap->SetBinContent(3,1, 0.);//0.009);
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 3 && m_PhiBins == 6) {
    mapset = true;
    for( int phibin = 1; phibin <= m_PhiBins; phibin++) {
      h_mcmassbiasmap->SetBinContent(1,phibin, 0.);//-0.022);// in GeV
      h_mcmassbiasmap->SetBinContent(2,phibin, 0.);//-0.021);
      h_mcmassbiasmap->SetBinContent(3,phibin, 0.);//0.009);
    }
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  
  if (!mapset) {
    std::cout << " ** ZmumuValidationExample::SetMCbiasUpsilon ** WARNING ** no MC bias map  " << std::endl;
  }
  return;
}
//----------------------------------------------------------------------------
void ZmumuValidationExample::SetMCbiasZ()
{
  // here one defines a map in eta and phi of possible MC biases
  // The map depends on the number of eta and phi histogram used
 
  // WARNING Set values in GeV
  bool mapset = false;
  h_mcmassbiasmap = new TH2F("h_mcmassbiasmap","Upsilon MC mass bias map",
			     m_EtaBins, 
			     h_dimuon_mass_vs_etaphi->GetXaxis()->GetXmin(),
			     h_dimuon_mass_vs_etaphi->GetXaxis()->GetXmax(),
			     m_PhiBins, 
			     h_dimuon_mass_vs_etaphi->GetYaxis()->GetXmin(),
			     h_dimuon_mass_vs_etaphi->GetYaxis()->GetXmax());
  
  if (m_EtaBins == 1 && m_PhiBins == 1) {
    mapset = true;
    //h_mcmassbiasmap->SetBinContent(1,1, 0.00308); // in GeV
    h_mcmassbiasmap->SetBinContent(1,1, 0.0000); // in GeV
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 3 && m_PhiBins == 1) {
    mapset = true;
    h_mcmassbiasmap->SetBinContent(1,1, 0.);// 0.091);//0.053); // in GeV
    h_mcmassbiasmap->SetBinContent(2,1, 0.);//0.090);//0.07497);
    h_mcmassbiasmap->SetBinContent(3,1, 0.);//0.100);//0.068);
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (m_EtaBins == 10 && m_PhiBins == 10) {
    mapset = true;
    for( int phibin = 1; phibin <= m_PhiBins; phibin++) {
      h_mcmassbiasmap->SetBinContent(1,phibin, 0.);//0.074);//0.072);// in GeV
      h_mcmassbiasmap->SetBinContent(2,phibin, 0.);//0.077);//0.061);
      h_mcmassbiasmap->SetBinContent(3,phibin, 0.);//0.063);//0.096);
      h_mcmassbiasmap->SetBinContent(4,phibin, 0.);//0.063);//0.071);
      h_mcmassbiasmap->SetBinContent(5,phibin, 0.);//0.101);//0.074);
      h_mcmassbiasmap->SetBinContent(6,phibin, 0.);//0.060);//0.085);
      h_mcmassbiasmap->SetBinContent(7,phibin, 0.);//0.092);
      h_mcmassbiasmap->SetBinContent(8,phibin, 0.);//0.073);
      h_mcmassbiasmap->SetBinContent(9,phibin, 0.);//0.072);
      h_mcmassbiasmap->SetBinContent(10,phibin, 0.);//0.088); 
    }
    if (m_JPsiMassFit != NULL) m_JPsiMassFit->SetMCbias(h_mcmassbiasmap);
  }
  
  if (!mapset) {
    std::cout << " ** ZmumuValidationExample::SetMCbiasZ ** WARNING ** no MC bias map  " << std::endl;
  }
  return;
}
//----------------------------------------------------------------------------
void ZmumuValidationExample::WelcomeBanner()
{
  std::cout << std::endl << std::endl;
  std::cout << "                  +++++++++++++++++++++++++++++++++++++ " << std::endl
	    << "                  +        ZmumuValidationExample     + " << std::endl
	    << "                  +-----------------------------------+ " << std::endl
	    << "                  + Sample Type:   " << m_infoSampleType.at(m_sampleType)     << "  (" << m_sampleType << ")" << std::endl
	    << "                  + Analysis type: " << m_infoAnalysisType.at(m_analysisType) << "  (" << m_analysisType << ")" << std::endl
	    << "                  +++++++++++++++++++++++++++++++++++++ " << std::endl
	    << std::endl;

  return;
}

//----------------------------------------------------------------------------
void ZmumuValidationExample::configurationReport()
{
  std::cout << std::endl << std::endl;
  std::cout << "                  +++++++++++++++++++++++++++++++++++++ " << std::endl
	    << "                  +        ZmumuValidationExample     + " << std::endl
	    << "                  +-----------------------------------+ " << std::endl
	    << "                  + Sample Type:   " << m_infoSampleType.at(m_sampleType)     << "  (" << m_sampleType << ")" << std::endl
	    << "                  + Analysis type: " << m_infoAnalysisType.at(m_analysisType) << "  (" << m_analysisType << ")" << std::endl
	    << "                  + # eta bins:    " << m_EtaBins << std::endl
	    << "                  + # phi bins:    " << m_PhiBins << std::endl
	    << "                  +++++++++++++++++++++++++++++++++++++ " << std::endl
	    << std::endl;

  return;
}

//----------------------------------------------------------------------------
void ZmumuValidationExample::defineInfoStrings()
{
  //   enum {ZSAMPLE, JPSISAMPLE, UPSILONSAMPLE, DIMUONSAMPLE, KSHORTSAMPLE};
  m_infoSampleType.push_back("ZSample");
  m_infoSampleType.push_back("JPSiSample");
  m_infoSampleType.push_back("UpsilonSample");
  m_infoSampleType.push_back("DimuonSample");
  m_infoSampleType.push_back("KshortSample");

  // enum {ANALYSIS_ALL = 0, ANALYSIS_RADIALDIST = 1, ANALYSIS_SAGITTADIST= 2};
  m_infoAnalysisType.push_back("ALL");
  m_infoAnalysisType.push_back("RADIALDIST");
  m_infoAnalysisType.push_back("SAGITTADIST");

  return;
}

//----------------------------------------------------------------------------
bool ZmumuValidationExample::EventSelection (int theEvent, TLorentzVector* vec_pos, TLorentzVector* vec_neg, TLorentzVector* vec_dimuon)
{
  bool goodEvent = true;
  
  //

  // initial calculations
  double deltaphi = this->ComputeDeltaPhi(vec_pos, vec_neg);
  
  // 
  // -- Apply selection depending on the sample being analyzed
  //

  // K SHORT
  if (m_sampleType == ZmumuValidationExample::KSHORTSAMPLE) {
    goodEvent = true; // default true
  }


  // JPsi 
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) {
    goodEvent = true; // default true
    if (fabs(m_z0_pos - m_z0_neg) > 0.500) {
      goodEvent = false; // OSCAR was 0.700
      if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << " failed Delta_z0 cut" << std::endl;
    }

    if (fabs(m_d0_pos - m_d0_neg) > 0.120) {
      goodEvent = false;
      if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << "  failed Delta_d0 cut" << std::endl;
    }
    if (m_d0_err_pos > 0.0001 && m_d0_err_neg > 0.0001) { // this is to make sure the d0 error was filled
      if (fabs( m_d0_pos/m_d0_err_pos) > 4.0 ) {
	goodEvent = false;
	if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << "  failed d0 significance cut" << std::endl;
      }
      if (fabs( m_d0_neg/m_d0_err_neg) > 4.0 ) {
	goodEvent = false;
	if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << "  failed z0 significance cut" << std::endl;
      }
    }
    
    if (std::max(vec_pos->Pt(), vec_neg->Pt())/1000. < 4.) {
      goodEvent = false;
      if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << "  failed leading pt cut" << std::endl;
    }
    if (std::min(vec_pos->Pt(), vec_neg->Pt())/1000. < 4.) {
      goodEvent = false; //0
      if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << "  failed 2nd pt cut" << std::endl;
    }
    if (TMath::Cos(vec_pos->Angle(vec_neg->Vect())) < 0.85 ) {
      goodEvent = false; // was 0.2
      if (m_PrintLevel >= 5) std::cout << " ** EventSelection ** JPSISAMPLE ** event: " << theEvent << "  failed opening angle cut" << std::endl;
    }
  }


  // Z 
  if (m_sampleType == ZmumuValidationExample::ZSAMPLE) {
    goodEvent = true; // default true
    if (fabs(m_z0_pos - m_z0_neg) >0.500) goodEvent = false; // OSCAR was 0.700
    if (fabs(m_d0_pos - m_d0_neg)> 0.120) goodEvent = false;
    if (m_d0_err_pos > 0.0001 && m_d0_err_neg > 0.0001) { // this is to make sure the d0 error was filled
      if (fabs( m_d0_pos/m_d0_err_pos) > 7.0 ) goodEvent = false;
      if (fabs( m_d0_neg/m_d0_err_neg) > 7.0 ) goodEvent = false;
    }
    if (std::max(vec_pos->Pt(), vec_neg->Pt())/1000. < 20.) goodEvent = false;
    if (std::min(vec_pos->Pt(), vec_neg->Pt())/1000. < 15.) goodEvent = false;
    if (vec_pos->Angle(vec_neg->Vect()) < 1.0 ) goodEvent = false; // was 0.2
  }


  //  
  // finally, be sure that event is in the desired mass range
  //
  if (vec_dimuon->M()/1000.0 < invMass_low)  goodEvent = false;
  if (vec_dimuon->M()/1000.0 > invMass_high) goodEvent = false;


  return goodEvent;
}

/////////////////////////////////////////////////////////////////
void  ZmumuValidationExample::SetDistortedMomenta (TLorentzVector* vec_pos, TLorentzVector* vec_neg)
{
  double sec_mass = ZmumuValidationExample::muon_mass;

  TLorentzVector* new_pos = new TLorentzVector(*vec_pos); // default values as input
  TLorentzVector* new_neg = new TLorentzVector(*vec_neg); // default values as input

  if (m_ApplyRadialDistortion) {
    if (h_RadialDistortion_input == NULL) {
      h_RadialDistortion_input = new TH2F("h_RadialDistortion_input","User input values for radial distortion #epsilon = #delta R/R; #eta; #phi [rad]; #epsilon = #delta R/R",
					  12, -ZmumuValidationExample::InnerDetectorEtaRange, ZmumuValidationExample::InnerDetectorEtaRange,
					  8, -ZmumuValidationExample::NumberPI, ZmumuValidationExample::NumberPI);
      // "Yes" pattern
      h_RadialDistortion_input->SetBinContent( 1,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,2,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,3,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,4,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,5,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,6,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 1,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 2,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 2,2,  0.000);
      h_RadialDistortion_input->SetBinContent( 2,3,  0.000);
      h_RadialDistortion_input->SetBinContent( 2,4,  0.000);
      h_RadialDistortion_input->SetBinContent( 2,5,  0.00051);
      h_RadialDistortion_input->SetBinContent( 2,6,  0.00051);
      h_RadialDistortion_input->SetBinContent( 2,7,  0.00051);
      h_RadialDistortion_input->SetBinContent( 2,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 3,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 3,2,  0.00051);
      h_RadialDistortion_input->SetBinContent( 3,3,  0.00051);
      h_RadialDistortion_input->SetBinContent( 3,4,  0.00051);
      h_RadialDistortion_input->SetBinContent( 3,5,  0.00051);
      h_RadialDistortion_input->SetBinContent( 3,6,  0.000);
      h_RadialDistortion_input->SetBinContent( 3,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 3,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 4,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 4,2,  0.000);
      h_RadialDistortion_input->SetBinContent( 4,3,  0.000);
      h_RadialDistortion_input->SetBinContent( 4,4,  0.000);
      h_RadialDistortion_input->SetBinContent( 4,5,  0.00051);
      h_RadialDistortion_input->SetBinContent( 4,6,  0.00051);
      h_RadialDistortion_input->SetBinContent( 4,7,  0.00051);
      h_RadialDistortion_input->SetBinContent( 4,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 5,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,2,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,3,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,4,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,5,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,6,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 5,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 6,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 6,2,  0.00051);
      h_RadialDistortion_input->SetBinContent( 6,3,  0.00051);
      h_RadialDistortion_input->SetBinContent( 6,4,  0.00051);
      h_RadialDistortion_input->SetBinContent( 6,5,  0.00051);
      h_RadialDistortion_input->SetBinContent( 6,6,  0.00051);
      h_RadialDistortion_input->SetBinContent( 6,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 6,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 7,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 7,2,  0.00051);
      h_RadialDistortion_input->SetBinContent( 7,3,  0.00000);
      h_RadialDistortion_input->SetBinContent( 7,4,  0.00051);
      h_RadialDistortion_input->SetBinContent( 7,5,  0.0000);
      h_RadialDistortion_input->SetBinContent( 7,6,  0.00051);
      h_RadialDistortion_input->SetBinContent( 7,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 7,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 8,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 8,2,  0.00051);
      h_RadialDistortion_input->SetBinContent( 8,3,  0.000);
      h_RadialDistortion_input->SetBinContent( 8,4,  0.00051);
      h_RadialDistortion_input->SetBinContent( 8,5,  0.00051);
      h_RadialDistortion_input->SetBinContent( 8,6,  0.00051);
      h_RadialDistortion_input->SetBinContent( 8,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 8,8,  0.000);

      h_RadialDistortion_input->SetBinContent( 9,1,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,2,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,3,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,4,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,5,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,6,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,7,  0.000);
      h_RadialDistortion_input->SetBinContent( 9,8,  0.000);

      h_RadialDistortion_input->SetBinContent(10,1,  0.000);
      h_RadialDistortion_input->SetBinContent(10,2,  0.00051);
      h_RadialDistortion_input->SetBinContent(10,3,  0.000);
      h_RadialDistortion_input->SetBinContent(10,4,  0.00051);
      h_RadialDistortion_input->SetBinContent(10,5,  0.00051);
      h_RadialDistortion_input->SetBinContent(10,6,  0.00051);
      h_RadialDistortion_input->SetBinContent(10,7,  0.000);
      h_RadialDistortion_input->SetBinContent(10,8,  0.000);

      h_RadialDistortion_input->SetBinContent(11,1,  0.000);
      h_RadialDistortion_input->SetBinContent(11,2,  0.00051);
      h_RadialDistortion_input->SetBinContent(11,3,  0.00051);
      h_RadialDistortion_input->SetBinContent(11,4,  0.00051);
      h_RadialDistortion_input->SetBinContent(11,5,  0.000);
      h_RadialDistortion_input->SetBinContent(11,6,  0.00051);
      h_RadialDistortion_input->SetBinContent(11,7,  0.000);
      h_RadialDistortion_input->SetBinContent(11,8,  0.000);

      h_RadialDistortion_input->SetBinContent(12,1,  0.000);
      h_RadialDistortion_input->SetBinContent(12,2,  0.000);
      h_RadialDistortion_input->SetBinContent(12,3,  0.000);
      h_RadialDistortion_input->SetBinContent(12,4,  0.000);
      h_RadialDistortion_input->SetBinContent(12,5,  0.000);
      h_RadialDistortion_input->SetBinContent(12,6,  0.000);
      h_RadialDistortion_input->SetBinContent(12,7,  0.000);
      h_RadialDistortion_input->SetBinContent(12,8,  0.000);

      // Y wave pattern
      h_RadialDistortion_input->Reset();
      for (int etabin=1; etabin <= h_RadialDistortion_input->GetNbinsX(); etabin++) {
	h_RadialDistortion_input->SetBinContent(etabin,1,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin,2,  0.0005);
	h_RadialDistortion_input->SetBinContent(etabin,3,  0.0000);
	h_RadialDistortion_input->SetBinContent(etabin,4, -0.0005);
	h_RadialDistortion_input->SetBinContent(etabin,5, -0.0010);
	h_RadialDistortion_input->SetBinContent(etabin,6, -0.0005);
	h_RadialDistortion_input->SetBinContent(etabin,7,  0.0000);
	h_RadialDistortion_input->SetBinContent(etabin,8,  0.0005);
      }

      // barrel expansion
      h_RadialDistortion_input->Reset();
      for (int etabin=4; etabin <= 9; etabin++) {
	h_RadialDistortion_input->SetBinContent(etabin, 1,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 2,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 3,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 4,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 5,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 6,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 7,  0.0010);
	h_RadialDistortion_input->SetBinContent(etabin, 8,  0.0010);
      }

      std::cout << " ** SetDistortedMomenta **  h_RadialDistortion_input histogram defined " << std::endl;
    }

    // -- TEST of radial distortion corrections
    double epsilon = 0.000;

    if (h_RadialDistortion_input != NULL) {
      int etabin = h_RadialDistortion_input->GetXaxis()->FindBin(vec_pos->Eta());
      int phibin = h_RadialDistortion_input->GetYaxis()->FindBin(vec_pos->Phi());
      epsilon = h_RadialDistortion_input->GetBinContent(etabin, phibin);
      if (m_PrintLevel >= 10) std::cout << " ** SetDistortedMomenta ** epsilon("<< etabin <<", " << phibin <<")= " << epsilon << std::endl;
    }

    double newPx, newPy, newPz, newE;
    newPx = vec_pos->Px() * (1-2*epsilon);
    newPy = vec_pos->Py() * (1-2*epsilon);
    newPz = vec_pos->Pz() * (1-epsilon);
    newE  = vec_pos->E() - epsilon * ( vec_pos->E() + pow(vec_pos->Pt(),2)/vec_pos->E());
    new_pos->SetXYZM(newPx, newPy, newPz, sec_mass);

    if (m_PrintLevel >= 10) {
      printf(" ** SetDistortedMomenta ** mu+ \n");
      printf("    as input         Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", vec_pos->Px(), vec_pos->Py(), vec_pos->Pz(), vec_pos->E(), vec_pos->Pt());  
      printf("    epsilon=%6.4f   Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", epsilon, new_pos->Px(), new_pos->Py(), new_pos->Pz(), new_pos->E(), new_pos->Pt());
    }

    if (h_RadialDistortion_input != NULL) {
      int etabin = h_RadialDistortion_input->GetXaxis()->FindBin(vec_neg->Eta());
      int phibin = h_RadialDistortion_input->GetYaxis()->FindBin(vec_neg->Phi());
      epsilon = h_RadialDistortion_input->GetBinContent(etabin, phibin);
    }
    newPx = vec_neg->Px() * (1-2*epsilon);
    newPy = vec_neg->Py() * (1-2*epsilon);
    newPz = vec_neg->Pz() * (1-epsilon);
    newE  = vec_neg->E() - epsilon * ( vec_neg->E() + pow(vec_neg->Pt(),2)/vec_neg->E());
    new_neg->SetXYZM(newPx, newPy, newPz, sec_mass);

    //-- 
    vec_pos->SetXYZM(new_pos->Px(), new_pos->Py(), new_pos->Pz(), sec_mass);
    vec_neg->SetXYZM(new_neg->Px(), new_neg->Py(), new_neg->Pz(), sec_mass);
  }

  if (m_ApplyTelescopeDistortion) {
    double zeta = 0.005;
    double newPx, newPy, newPz, newE;
    newPx = vec_pos->Px();
    newPy = vec_pos->Py();
    newPz = vec_pos->Pz() - zeta * vec_pos->Pt();
    new_pos->SetXYZM(newPx, newPy, newPz, sec_mass);

    if (m_PrintLevel >= 10) {
      printf(" ** SetDistortedMomenta ** Telescope ** mu+ \n");
      printf("    as input         Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", vec_pos->Px(), vec_pos->Py(), vec_pos->Pz(), vec_pos->E(), vec_pos->Pt());  
      printf("    zeta=%6.4f      Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", zeta, new_pos->Px(), new_pos->Py(), new_pos->Pz(), new_pos->E(), new_pos->Pt());
    }

    newPx = vec_neg->Px();
    newPy = vec_neg->Py();
    newPz = vec_neg->Pz() - zeta * vec_pos->Pt();
    new_neg->SetXYZM(newPx, newPy, newPz, sec_mass);

    vec_pos->SetXYZM(new_pos->Px(), new_pos->Py(), new_pos->Pz(), sec_mass);
    vec_neg->SetXYZM(new_neg->Px(), new_neg->Py(), new_neg->Pz(), sec_mass);
  }

  if (m_ApplyEndcapExpansion) {
    double zeta = 0.01;
    if (fabs(vec_pos->Eta()<1)) zeta = 0.000;
   
    double newPx, newPy, newPz, newE;
    newPx = vec_pos->Px();
    newPy = vec_pos->Py();
    newPz = vec_pos->Pz() * (1 - zeta);
    new_pos->SetXYZM(newPx, newPy, newPz, sec_mass);
    
    zeta = 0.01;
    if (fabs(vec_neg->Eta()<1)) zeta = 0.000;
    newPx = vec_neg->Px();
    newPy = vec_neg->Py();
    newPz = vec_neg->Pz() * (1 - zeta);
    new_neg->SetXYZM(newPx, newPy, newPz, sec_mass);
    
    if (m_PrintLevel >= 10) {
      printf(" ** SetDistortedMomenta ** Endcap expansion ** mu+ \n");
      printf("    as input         Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", vec_pos->Px(), vec_pos->Py(), vec_pos->Pz(), vec_pos->E(), vec_pos->Pt());  
      printf("    zeta=%6.4f      Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", zeta, new_pos->Px(), new_pos->Py(), new_pos->Pz(), new_pos->E(), new_pos->Pt());
      printf(" ** SetDistortedMomenta ** Endcap expansion ** mu- \n");
      printf("    as input         Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", vec_neg->Px(), vec_neg->Py(), vec_neg->Pz(), vec_neg->E(), vec_neg->Pt());  
      printf("    zeta=%6.4f      Pxyz:( %7.2f, %7.2f, %7.2f)  E= %7.2f   Pt= %7.2f \n", zeta, new_neg->Px(), new_neg->Py(), new_neg->Pz(), new_neg->E(), new_neg->Pt());
      std::cout << std::endl;
    }
    
    vec_pos->SetXYZM(new_pos->Px(), new_pos->Py(), new_pos->Pz(), sec_mass);
    vec_neg->SetXYZM(new_neg->Px(), new_neg->Py(), new_neg->Pz(), sec_mass);
  }
  
  if (m_ApplyBTest == true) {
    // -- TEST of relative B field-detector rotation
    double alpha_rot_ecc = 0.00050; double alpha_rot_pos = 0.000;
    double alpha_rot_eca = 0.00044; double alpha_rot_neg = 0.000;

    double phase_rot_ecc =  0.967; double phase_rot_pos = 0.000;
    double phase_rot_eca = -2.52; double phase_rot_neg = 0.000; 

    double cot_theta_pos = 1./tan(vec_pos->Theta());
    double cot_theta_neg = 1./tan(vec_neg->Theta());
    double scalefactor_pos = 1.;
    double scalefactor_neg = 1.;
    double newPx, newPy, newPz, newE;

    if (vec_pos->Eta() < -1.7) {alpha_rot_pos = alpha_rot_ecc; phase_rot_pos = phase_rot_ecc;}
    if (vec_pos->Eta() > 1.7)  {alpha_rot_pos = alpha_rot_eca; phase_rot_pos = phase_rot_eca;}
    scalefactor_pos = 1. / (1- cot_theta_pos * sin(vec_pos->Phi()+phase_rot_ecc) * alpha_rot_pos);
    newPx = vec_pos->Px() * scalefactor_pos;
    newPy = vec_pos->Py() * scalefactor_pos;
    newPz = vec_pos->Pz() * scalefactor_pos;
    vec_pos->SetXYZM(newPx, newPy, newPz, sec_mass);

    // 
    if (vec_neg->Eta() < -1.7) {alpha_rot_neg = alpha_rot_ecc; phase_rot_neg = phase_rot_ecc;}
    if (vec_neg->Eta() > 1.7)  {alpha_rot_neg = alpha_rot_eca; phase_rot_neg = phase_rot_eca;}
    scalefactor_neg = 1. / (1- cot_theta_neg * sin(vec_neg->Phi()+phase_rot_neg) * alpha_rot_neg);
    newPx = vec_neg->Px() * scalefactor_neg;
    newPy = vec_neg->Py() * scalefactor_neg;
    newPz = vec_neg->Pz() * scalefactor_neg;
    vec_neg->SetXYZM(newPx, newPy, newPz, sec_mass);

    //std::cout << " -- scale factor -- pos: " << scalefactor_pos << "   neg: " << scalefactor_neg << std::endl;
  }

  if (m_ApplySagittaHistogramTest) {
     // -- Test: apply delta_sagitta corrections from map (histogram)
    float delta_sagitta = 0;
    float new_pt = 0.;
    float new_pz = 0.;
    // 
 
    if (h_deltaSagittaCorrMaps != NULL) {
      // delta_sagitta for positive muon:
      delta_sagitta = h_deltaSagittaCorrMaps->GetBinContent(h_deltaSagittaCorrMaps->FindBin(vec_pos->Eta(), vec_pos->Phi()));
      new_pt = vec_pos->Pt() / (1+delta_sagitta/1000000 * vec_pos->Pt());
      new_pz = vec_pos->Pz() / (1+delta_sagitta/1000000 * vec_pos->Pt());
      vec_pos->SetXYZM(new_pt * cos(vec_pos->Phi()), new_pt * sin(vec_pos->Phi()), new_pz, sec_mass);

      // delta_sagitta for negative muon
      delta_sagitta = h_deltaSagittaCorrMaps->GetBinContent(h_deltaSagittaCorrMaps->FindBin(vec_neg->Eta(), vec_neg->Phi()));
      new_pt = vec_neg->Pt() / (1-delta_sagitta/1000000 * vec_neg->Pt());
      new_pz = vec_neg->Pz() / (1-delta_sagitta/1000000 * vec_neg->Pt());
      vec_neg->SetXYZM(new_pt * cos(vec_neg->Phi()), new_pt * sin(vec_neg->Phi()), new_pz, sec_mass);
    }
    else{
      std::cout << " problem with maps " << std::endl;  
    }
  }

  return;
}

////////////////////////////////////////////////////////////////////
float ZmumuValidationExample::GetMCbiasMass (TLorentzVector* vec_pos, TLorentzVector* vec_neg)
{
  float pdgMass = ZmumuValidationExample::ZPDGMass;
  if (m_sampleType == ZmumuValidationExample::JPSISAMPLE) pdgMass = ZmumuValidationExample::JPsi1SPDGMass;
  if (m_sampleType == ZmumuValidationExample::UPSILONSAMPLE) pdgMass = ZmumuValidationExample::Upsilon1SPDGMass;

  // take the eta and phi of the leading muon:
  float thisEta = vec_pos->Eta();
  float thisPhi = vec_pos->Phi();
  if (vec_neg->Pt() > vec_pos->Pt()) {
    thisEta = vec_neg->Eta();
    thisPhi = vec_neg->Phi();
  }
  int etabin = h_mcmassbiasmap->GetXaxis()->FindBin(thisEta);
  int phibin = h_mcmassbiasmap->GetYaxis()->FindBin(thisPhi);
  
  float theMass = pdgMass + h_mcmassbiasmap->GetBinContent(etabin, phibin);
  
  if (m_PrintLevel >= 10) {
    std::cout << " ** GetMCbiasMass ** M(eta,phi)= (" << etabin << ", " << phibin << ") = " << theMass << std::endl;
  }
  return theMass;
}

