#include "TForwardElectronLikelihoodTool.h"
#include <cmath>
#include "TSystem.h"
#include "TROOT.h"

const double Root::TForwardElectronLikelihoodTool::s_fIpBounds[s_IP_FBINS+1] = {0.,500.};
// These are the variables availalble in the likelihood.
const char* Root::TForwardElectronLikelihoodTool::s_fVariables[s_fnVariables] = {"el_secondLambda"
										 ,"el_lateral"
										 ,"el_longitudinal"
										 ,"el_centerLambda"
										 ,"el_fracMax"
										 ,"el_secondR"
										 ,"el_significance"
										 ,"el_secondDensity"
};


Root::TForwardElectronLikelihoodTool::TForwardElectronLikelihoodTool(const char* name) :
  TCalculatorToolBase(name),
  TSelectorToolBase(name),
  asg::AsgMessaging(std::string(name)),
  doPileupCorrection(false),
  VariableNames(""),
  PdfFileName(""),
  m_variableBitMask(0x0),
  m_cutPosition_kinematicEta(-9),
  m_cutPosition_kinematicEt (-9),
  m_cutPosition_LH(-9),
  m_resultPosition_LH(-9)
{
  for(unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < s_IP_FBINS; ip++){
        for(unsigned int et = 0; et < s_fnEtBinsHist; et++){
          for(unsigned int eta = 0; eta < s_fnEtaBins; eta++){
            m_fPDFbins[s_or_b][ip][et][eta][varIndex] = nullptr;
          }
        }
      }
    }
  }
}

//=============================================================================
// Destructor
//=============================================================================
Root::TForwardElectronLikelihoodTool::~TForwardElectronLikelihoodTool()
{
}


int Root::TForwardElectronLikelihoodTool::initialize()
{
  ATH_MSG_DEBUG( "TForwardElectronLikelihoodTool initialize.");

  // use an int as a StatusCode
  int sc(1);

  // Check that all needed variables are setup
  if ( PdfFileName.empty() )
    {
      ATH_MSG_WARNING("You need to specify the input PDF file name before you call initialize() with setPDFFileName('your/file/name.root') ");
      sc = 0;
    }

  unsigned int number_of_expected_bin    =  s_fnDiscEtBins*s_fnEtaBins ;
  if( CutLikelihood.size() != number_of_expected_bin) {
    ATH_MSG_ERROR("Configuration issue :  CutLikelihood expected size " << number_of_expected_bin <<
                  " input size " << CutLikelihood.size());
    ATH_MSG_ERROR("Could NOT initialize! Please fix the errors mentioned above...");
    sc = 0;
    return sc;
  }

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:

  // Cut position for the kineatic pre-selection, separately for eta/Et
  m_cutPosition_kinematicEta = m_accept.addCut( "kinematicEta", "pass kinematic eta" );
  if ( m_cutPosition_kinematicEta < 0 ) sc = 0;
  m_cutPosition_kinematicEt  = m_accept.addCut( "kinematicEt" , "pass kinematic Et " );
  if ( m_cutPosition_kinematicEt  < 0 ) sc = 0;

  // Cut position for the likelihood selection - DO NOT CHANGE ORDER!
  m_cutPosition_LH = m_accept.addCut( "passLH", "pass Likelihood" );
  m_resultPosition_LH = m_result.addResult("passLH", "pass likelihood" );
  if ( m_resultPosition_LH < 0 ) sc = 0; // Exceeded the number of allowed results

  // Set the result to a defaul value
  m_result.setResult( m_resultPosition_LH, -9999.0  );


  // Check that we got everything OK
  if ( sc == 0 )
  {
    ATH_MSG_ERROR("! Something went wrong with the setup of the return objects...");
    return 0;
  }

  // Get the correct bit mask for the current likelihood operating point
  m_variableBitMask = GetLikelihoodBitmask(VariableNames);

  //----------File/Histo operation------------------------------------
  // Load the ROOT file containing the PDFs
  TString tmpString(PdfFileName);
  gSystem->ExpandPathName(tmpString);
  std::string fname(tmpString.Data());
  TFile* pdfFile = TFile::Open( fname.c_str(), "READ" );
  // Check that we could load the ROOT file
  if ( !pdfFile )
  {
    ATH_MSG_ERROR(" No ROOT file found here: " << PdfFileName);
    return 0;
  }

  //Load the histograms
  for(unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++){
    std::string vstr = s_fVariables[varIndex];
    // Skip the loading of PDFs for variables we don't care about for this operating point.
    // If the string is empty (which is true in the default 2012 case), load all of them.
    if(VariableNames.find(vstr) == std::string::npos && !VariableNames.empty()){
      continue;
    }
    LoadVarHistograms(pdfFile,vstr,varIndex);
  }

  //TFile close does not free the memory
  pdfFile->Close();
  //We need the destructor to be called
  delete pdfFile;
  //----------End File/Histo operation------------------------------------

  ATH_MSG_DEBUG("Initialization complete for a LH tool with these specs:"
		<< "\n - PdfFileName                                  : " << PdfFileName
		<< "\n - Variable bitmask                             : " << m_variableBitMask
		<< "\n - VariableNames                                : " << VariableNames
		);
  return sc;
}

int Root::TForwardElectronLikelihoodTool::LoadVarHistograms(TFile* pdfFile, std::string vstr,unsigned int varIndex){
  for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
    for (unsigned int ip = 0; ip < s_IP_FBINS; ip++){
      for(unsigned int et = 0; et < s_fnEtBinsHist; et++){
        for(unsigned int eta = 0; eta < s_fnEtaBins; eta++){
	  
          std::string sig_bkg = (s_or_b==0) ? "sig" : "bkg" ;	  
          // For the fwd LH PDFs and optimisation have matching eta bin boundaries
          // unsigned int eta_tmp = (eta > 0) ? eta-1 : eta ;
          unsigned int eta_tmp = eta;
          unsigned int et_tmp  = et;
          char binname[200];
          getBinName( binname, et_tmp, eta_tmp );

          char pdfdir[500];
          snprintf(pdfdir,500,"%s/%s",vstr.c_str(),sig_bkg.c_str());
          char pdf[500];
          snprintf(pdf,500,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
	  
          if (!pdfFile->GetListOfKeys()->Contains(vstr.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
          }
          if (!((TDirectory*)pdfFile->Get(vstr.c_str()))->GetListOfKeys()->Contains(sig_bkg.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
          }
          // it shouldn't go to the last bin (which is 7000 GeV)
          if ( et > s_fnEtBinsHist-1 ) {
            continue;
          }
          // Use the first Et bin given in the root file for all Et ranges below
          if (et == 0 && !((TDirectory*)pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            ATH_MSG_INFO( "using lowest GeV bin in place of all below." );
            getBinName( binname, et_tmp+1, eta_tmp );
            snprintf(pdf,500,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
          }
          if (((TDirectory*)pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            TH1F* hist = (TH1F*)(((TDirectory*)pdfFile->Get(pdfdir))->Get(pdf));
            m_fPDFbins[s_or_b][ip][et][eta][varIndex] =std::make_unique<EGSelectors::SafeTH1>(hist);
            delete hist;
          }
          else {
            ATH_MSG_INFO("Warning: Object " << pdf << " does not exist.");
            ATH_MSG_INFO("Skipping all other histograms with this variable.");
            return 1;
          }
        }
      }
    }  
  }
  return 1;
}

const Root::TAccept& Root::TForwardElectronLikelihoodTool::accept( double likelihood,
                                                                   double eta, 
                                                                   double eT,
                                                                   double ip
								   ) const
{
  LikeEnumForward::LHAcceptVars_t vars;
  
  vars.likelihood      = likelihood     ;
  vars.eta             = eta            ;
  vars.eT              = eT             ;
  vars.ip              = ip             ;
  
  return accept(vars);
}

// This method calculates if the current electron passes the requested likelihood cut
const Root::TAccept& Root::TForwardElectronLikelihoodTool::accept( LikeEnumForward::LHAcceptVars_t& vars_struct ) const
{
  // Reset the cut result bits to zero (= fail cut)
  m_accept.clear();
  
  // Set up the individual cuts
  bool passKineEta(true);
  bool passKineEt (true);
  bool passLH(true);
  
  if (fabs(vars_struct.eta) < 2.5) {
    ATH_MSG_DEBUG("This forward electron has" << vars_struct.eta <<  ", which is fabs(eta)<2.5 Returning False.");
    passKineEta = false;
  }
  // Return if the kinematic requirements are not fulfilled
  m_accept.setCutResult( m_cutPosition_kinematicEta, passKineEta );
  if ( !passKineEta ){ return m_accept; }
  
  unsigned int etbin  = getLikelihoodEtBin (vars_struct.eT );
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);

  if (etbin  >= s_fnDiscEtBins) {
    ATH_MSG_WARNING( "Cannot evaluate likelihood for Et " << vars_struct.eT<< ". Returning false..");
    passKineEt = false;
  }

  // Return if the kinematic requirements are not fulfilled
  m_accept.setCutResult( m_cutPosition_kinematicEt, passKineEt );
  if ( !passKineEt ){ return m_accept; }

  double cutDiscriminant = -999.;
  unsigned int ibin_combined = etbin*s_fnEtaBins+etabin;

  if(CutLikelihood.size()){

    cutDiscriminant = CutLikelihood[ibin_combined];
    // If doPileupCorrection, then correct the discriminant itself instead of the cut value
    if (doPileupCorrection) {
      cutDiscriminant += vars_struct.ip*CutLikelihoodPileupCorrectionA[ibin_combined]+CutLikelihoodPileupCorrectionB[ibin_combined];
    }
    
  }
  
  // Determine if the calculated likelihood value passes the cut
  ATH_MSG_DEBUG("Likelihood macro: Discriminant: ");
  if ( vars_struct.likelihood < cutDiscriminant )
  {
    ATH_MSG_DEBUG("Likelihood macro: Disciminant Cut Failed.");
    passLH = false;
  }
  m_accept.setCutResult( m_cutPosition_LH, passLH );
  return m_accept;
}

const Root::TResult& Root::TForwardElectronLikelihoodTool::calculate( 
                                                                     double eta, 
                                                                     double eT,
                                                                     double secondLambda, 
                                                                     double lateral, 
                                                                     double longitudinal, 
                                                                     double centerLambda,
                                                                     double fracMax,
                                                                     double secondR,
                                                                     double significance,
                                                                     double secondDensity,
                                                                     double ip ) const
{
  LikeEnumForward::LHCalcVars_t vars;
  vars.eta             = eta            ;
  vars.eT              = eT             ;
  vars.secondLambda    = secondLambda   ;
  vars.lateral         = lateral        ;
  vars.longitudinal    = longitudinal   ;
  vars.centerLambda    = centerLambda   ;
  vars.fracMax         = fracMax        ;
  vars.secondR         = secondR        ;
  vars.significance    = significance   ;  
  vars.secondDensity   = secondDensity  ;
  vars.ip              = ip             ;
  return calculate(vars);
}

// The main public method to actually calculate the likelihood value
const Root::TResult& Root::TForwardElectronLikelihoodTool::calculate(LikeEnumForward::LHCalcVars_t& vars_struct)  const
{
  // Reset the results to defaul values
  m_result.setResult( m_resultPosition_LH, -999.0  );
  double arr[] = {vars_struct.secondLambda
		  ,vars_struct.lateral
		  ,vars_struct.longitudinal
		  ,vars_struct.centerLambda
		  ,vars_struct.fracMax
		  ,vars_struct.secondR
		  ,vars_struct.significance
		  ,vars_struct.secondDensity
  };
  std::vector<double> vec (arr, arr + sizeof(arr) / sizeof(double) );

  // Calculate the actual likelihood value and fill the return object
  double lhVal = this->EvaluateLikelihood(vec,vars_struct.eT,vars_struct.eta,vars_struct.ip);
  m_result.setResult( m_resultPosition_LH, lhVal  );
  return m_result;
}

double Root::TForwardElectronLikelihoodTool::EvaluateLikelihood(std::vector<double> varVector,double et,double eta,double ip) const
{
  // const double GeV = 1000;
  unsigned int etbin  = getLikelihoodEtBin (et );
  unsigned int etabin = getLikelihoodEtaBin(eta);
  unsigned int ipbin  = getIpBin(ip);

  if (etbin >= s_fnEtBinsHist) {
    ATH_MSG_WARNING("skipping etbin  " << etbin  << ", et  " << et);
    return -999.;
  }
  if (etabin >= s_fnEtaBins) {
    ATH_MSG_WARNING("skipping etabin " << etabin << ", eta " << eta);
    return -999.;
  }
  
  if (varVector.size() != s_fnVariables) {
    ATH_MSG_WARNING("Error! Variable vector size mismatch! Check your vector!" );
    return -999.;
  }  
  
  double SigmaS = 1.;
  double SigmaB = 1.;
  
  for(unsigned int var = 0; var < s_fnVariables; var++){
    
    std::string varstr = s_fVariables[var];
    
    // Skip variables that are masked off (not used) in the likelihood
    if (!(m_variableBitMask & (0x1 << var))){
      continue;
    }

    for (unsigned int s_or_b=0; s_or_b<2;s_or_b++) {

      int bin = m_fPDFbins[s_or_b][ipbin][etbin][etabin][var]->FindBin(varVector[var]);
      double prob = 0;

      double integral = double(m_fPDFbins[s_or_b][ipbin][etbin][etabin][var]->Integral());
      if (integral == 0) { // currently, the crack always has integral == 0 
        ATH_MSG_DEBUG("Error! PDF integral == 0!"); // changed it to debug message since we intend to run the tool in the derivations
        return -1.35;
      }

      prob = double(m_fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;

      if   (s_or_b == 0) SigmaS *= prob;
      else if (s_or_b == 1) SigmaB *= prob;
    }
  }

  return TransformLikelihoodOutput( SigmaS, SigmaB );
}

// --------------------------------------------
double Root::TForwardElectronLikelihoodTool::TransformLikelihoodOutput(double ps,double pb) const {
  // returns transformed or non-transformed output
  // (Taken mostly from TMVA likelihood code)
  double fEpsilon = 1e-99;
  // If both signal and bkg are 0, we want it to fail.
  if (ps < fEpsilon) ps = 0;
  if (pb < fEpsilon) pb = fEpsilon;
  double disc = ps/double(ps + pb);

  if (disc >= 1.0) disc = 1. - 1.e-15;
  else if (disc <= 0.0) disc = fEpsilon;

  double tau = 15.0;
  disc = - log(1.0/disc - 1.0)*(1./double(tau));

  ATH_MSG_DEBUG( "disc is " << disc );
  return disc;
}

// Gets the IP bin
unsigned int Root::TForwardElectronLikelihoodTool::getIpBin(double ip) const{
  for(unsigned int ipBin = 0; ipBin < s_IP_FBINS; ++ipBin){
    if(ip < s_fIpBounds[ipBin+1])
      return ipBin;
  }
  return 0;
}

// Gets the Eta bin  given the eta . Binning uses uper bound
unsigned int Root::TForwardElectronLikelihoodTool::getLikelihoodEtaBin(double eta) const{
  const unsigned int nEtaBins = s_fnEtaBins;
  const double etaBins[nEtaBins] = {2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.16, 3.35, 3.6, 4.9};
  for(unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin){
    if(fabs(eta) < etaBins[etaBin])
      return etaBin;
  }
  return (nEtaBins-1);
}
// Gets the histogram Et bin given the et (MeV). Binning uses upper bound 
unsigned int Root::TForwardElectronLikelihoodTool::getLikelihoodEtBin(double eT) const {
  const double GeV = 1000;  
  const unsigned int nEtBins = s_fnDiscEtBins;
  const double eTBins[nEtBins] = {30*GeV,40*GeV,50*GeV,7000*GeV}; // removed 20 GeV bin since we're upper bound oriented
  for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
    if(eT < eTBins[eTBin]){
      return eTBin;
    }
  }
  return (nEtBins-1); // Return the last bin if > the last bin. 
}

// Gets the bin name. Given the HISTOGRAM bin  naming used in the input file
// which seems to be lower bound oriented ...
void Root::TForwardElectronLikelihoodTool::getBinName(char* buffer, int etbin,int etabin) const{
  static const double eta_bounds[s_fnEtaBins] = {2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.16, 3.35, 3.6};
  static const int et_bounds[s_fnEtBinsHist] = {20,30,40,50};
  snprintf(buffer, 200,"et%deta%0.2f", et_bounds[etbin], eta_bounds[etabin]);
}

unsigned int Root::TForwardElectronLikelihoodTool::GetLikelihoodBitmask(std::string vars) const{
  unsigned int mask = 0x0;
  ATH_MSG_DEBUG ("Variables to be used: ");
  for(unsigned int var = 0; var < s_fnVariables; var++){
    if (vars.find(s_fVariables[var]) != std::string::npos){
      ATH_MSG_DEBUG(s_fVariables[var] );
      mask = mask | 0x1 << var;
    }
  }
  ATH_MSG_DEBUG("mask: " << mask);
  return mask;
}
