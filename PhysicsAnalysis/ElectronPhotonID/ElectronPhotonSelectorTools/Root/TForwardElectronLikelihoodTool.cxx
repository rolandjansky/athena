#include "TForwardElectronLikelihoodTool.h"
#include <cmath>
#include "TSystem.h"
#include "TROOT.h"

//=============================================================================
// Constructor
//=============================================================================

//----------------------------------------------------------------------------------------
Root::TForwardElectronLikelihoodTool::TForwardElectronLikelihoodTool(const char* name) :
  TCalculatorToolBase(name),
  TSelectorToolBase(name),
  asg::AsgMessaging(std::string(name)),
  doPileupCorrection(false),
  VariableNames(""),
  OperatingPoint(),
  PdfFileName(""),
  m_pdfFile(0),
  m_resultPrefix(""),
  m_resultName("likelihood"),
  m_cutPosition_kinematic(-9),
  m_cutPosition_LH(-9),
  m_resultPosition_LH(-9)
{
  for(unsigned int varIndex = 0; varIndex < fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < IP_FBINS; ip++){
	for(unsigned int et = 0; et < fnEtBinsHist; et++){
	  for(unsigned int eta = 0; eta < fnEtaBins; eta++){
	    fPDFbins[s_or_b][ip][et][eta][varIndex] = 0;
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
  for(unsigned int varIndex = 0; varIndex < fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < IP_FBINS; ip++){
	for(unsigned int et = 0; et < fnEtBinsHist; et++){
	  for(unsigned int eta = 0; eta < fnEtaBins; eta++){
	    if (fPDFbins[s_or_b][ip][et][eta][varIndex]){
	      delete fPDFbins[s_or_b][ip][et][eta][varIndex];
	      fPDFbins[s_or_b][ip][et][eta][varIndex] = 0;
	    }
	  }
	}
      }
    }
  }
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
  if ( sc == 0 )
    {
      ATH_MSG_ERROR("Could NOT initialize! Please fix the errors mentioned above...");
      return sc;
    }
  
  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  
  // Cut position for the kineatic pre-selection
  m_cutPosition_kinematic = m_accept.addCut( "kinematic", "pass kinematic" );
  if ( m_cutPosition_kinematic < 0 ) sc = 0;
 
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

  // ----------------------------------
  // Get the correct bit mask for the current likelihood operating point
  // m_variableBitMask = GetLikelihoodBitmask(VariableNames);
  // ----------------------------------
  
  //----------File/Histo operation------------------------------------
  // Load the ROOT file containing the PDFs
  TString tmpString(PdfFileName);
  gSystem->ExpandPathName(tmpString);
  std::string fname(tmpString.Data());
  m_pdfFile = TFile::Open( fname.c_str(), "READ" );
  // Check that we could load the ROOT file
  if ( !m_pdfFile )
    {
      ATH_MSG_ERROR(" No ROOT file found here: " << PdfFileName);
      return 0;
    }

  //Load the histograms
  for(unsigned int varIndex = 0; varIndex < fnVariables; varIndex++){
    std::string vstr = fVariables[varIndex];
    ATH_MSG_INFO(vstr);
    // Skip the loading of PDFs for variables we don't care about for this operating point.
    // If the string is empty (which is true in the default 2012 case), load all of them.
    if(VariableNames.find(vstr) == std::string::npos && !VariableNames.empty()){
      continue;
    }
    LoadVarHistograms(vstr,varIndex);
  }

  //TFile close does not free the memory
  m_pdfFile->Close();
  //We need the destructor to be called
  delete m_pdfFile;

  return sc;
}

int Root::TForwardElectronLikelihoodTool::LoadVarHistograms(std::string vstr,unsigned int varIndex){
  for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
    for (unsigned int ip = 0; ip < IP_FBINS; ip++){
      for(unsigned int et = 0; et < fnEtBinsHist; et++){
        for(unsigned int eta = 0; eta < fnEtaBins; eta++){

          std::string sig_bkg = (s_or_b==0) ? "sig" : "bkg" ;	  
          // Because eta bins in the root file don't match up exactly with cut menu
          // definitions, the second eta bin is an exact copy of the first,
          // and all subsequent eta bins are pushed back by one.
          unsigned int eta_tmp = (eta > 0) ? eta-1 : eta ;
          // The 7-10 GeV, crack bin uses the 10-15 Gev pdfs. WE DO NOT DO THIS ANYMORE!
          //unsigned int et_tmp = (eta == 5 && et == 1) ? 1 : et; 
	  unsigned int et_tmp = et;
          char binname[200];
          getBinName( binname, et_tmp, eta_tmp );
	  
	  char pdfdir[500];
	  snprintf(pdfdir,500,"%s/%s",vstr.c_str(),sig_bkg.c_str());
	  char pdf[500];
	  snprintf(pdf,500,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
	  
	  if (!m_pdfFile->GetListOfKeys()->Contains(vstr.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
	  }
	  if (!((TDirectory*)m_pdfFile->Get(vstr.c_str()))->GetListOfKeys()->Contains(sig_bkg.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
	  }
	  
          // it shouldn't go to the last bin (which is 7000 GeV)
          if ( et > fnEtBinsHistOrig-1 ) {
            continue;
          }


	  // Use the first Et bin given in the root file for all Et ranges below
          if (et == 0 && !((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            // ATH_MSG_INFO( "using lowest GeV bin in place of all below." );
	    getBinName( binname, et_tmp+1, eta_tmp );
	    snprintf(pdf,500,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
	  }
          if (((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            TH1F* hist = (TH1F*)(((TDirectory*)m_pdfFile->Get(pdfdir))->Get(pdf));
            fPDFbins[s_or_b][ip][et][eta][varIndex] = new TForwardElectronLikelihoodTool::SafeTH1(hist);
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
  bool passKine(true);
  bool passLH(true);

  if (fabs(vars_struct.eta) < 2.5) {
    ATH_MSG_DEBUG("This forward electron has" << vars_struct.eta <<  " ,which is fabs(eta)<2.5 Returning False.");
    passKine = false;
  }

  unsigned int etbin = (getLikelihoodEtHistBin(vars_struct.eT)> 2) ? (getLikelihoodEtHistBin(vars_struct.eT)-2) : getLikelihoodEtHistBin(vars_struct.eT);
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);
  
  if (etbin  >= fnDiscEtBinsOrig) {
    ATH_MSG_WARNING( "Cannot evaluate likelihood for Et " << vars_struct.eT<< ". Returning false..");
    passKine = false;
  }
  
  // Return if the kinematic requirements are not fulfilled
  m_accept.setCutResult( m_cutPosition_kinematic, passKine );
  if ( !passKine ){ return m_accept; }
  
  double cutDiscriminant = -999.;
  unsigned int ibin_combined = etbin*10+etabin-1;

  if(CutLikelihood.size()){
    // To protect against a binning mismatch, which should never happen
    if(ibin_combined > CutLikelihood.size()){
      ATH_MSG_WARNING ( "The desired bin is outside of the range!  Bin : " << ibin_combined << 
                        " available LH bins " << CutLikelihood.size() << " Should never happen!" );
      
    }
    
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

double Root::TForwardElectronLikelihoodTool::EvaluateLikelihood(std::vector<float> varVector,double et,double eta,double ip) const
{
  std::vector<double> vec;
  for(unsigned int var = 0; var < fnVariables; var++){
    vec.push_back(varVector[var]);
  }
  return EvaluateLikelihood(vec,et,eta,ip);//,mask);  
}

double Root::TForwardElectronLikelihoodTool::EvaluateLikelihood(std::vector<double> varVector,double et,double eta,double ip) const
{
  
  // const double GeV = 1000;
  unsigned int etbin = (getLikelihoodEtHistBin(et) > 2) ? (getLikelihoodEtHistBin(et)-2):getLikelihoodEtHistBin(et); // hist binning
  unsigned int etabin = getLikelihoodEtaBin(eta);
  unsigned int ipbin  = getIpBin(ip);

  if (etbin >= fnEtBinsHist) {
    ATH_MSG_WARNING("skipping etbin " << etbin << ", et " << et);
    return -999.;
  }
  if (etabin >= fnEtaBins) {
    ATH_MSG_WARNING("skipping etabin " << etabin << ", eta " << eta);
    return -999.;
  }
  
  if (varVector.size() != fnVariables) 
    ATH_MSG_WARNING("Error! Variable vector size mismatch! Check your vector!" );
  
  double SigmaS = 1.;
  double SigmaB = 1.;

  for(unsigned int var = 0; var < fnVariables; var++){
    
    std::string varstr = fVariables[var];
    
    for (unsigned int s_or_b=0; s_or_b<2;s_or_b++) {
      
      int bin = fPDFbins[s_or_b][ipbin][etbin][etabin][var]->FindBin(varVector[var]);

      double prob = 0;

      double integral = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->Integral());
      if (integral == 0) { // currently, the crack always has integral == 0 
	ATH_MSG_DEBUG("Error! PDF integral == 0!"); // changed it to debug message since we intend to run the tool in the derivations
	return -1.35;
      }
      
      prob = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;
      
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


const double Root::TForwardElectronLikelihoodTool::fIpBounds[IP_FBINS+1] = {0.,500.};

//---------------------------------------------------------------------------------------
// Gets the IP bin
unsigned int Root::TForwardElectronLikelihoodTool::getIpBin(double ip) const{
  for(unsigned int ipBin = 0; ipBin < IP_FBINS; ++ipBin){
    if(ip < fIpBounds[ipBin+1])
      return ipBin;
  }
  return 0;
}


//---------------------------------------------------------------------------------------
// Gets the Eta bin [0-9] given the eta
unsigned int Root::TForwardElectronLikelihoodTool::getLikelihoodEtaBin(double eta) const{
  const unsigned int nEtaBins = 11;
  const double etaBins[nEtaBins] = {2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.16, 3.35, 3.6, 4.9};
  
  for(unsigned int etaBin = 1; etaBin < nEtaBins; ++etaBin){ // start loop at 1 so underflow is applied in case we're below 2.5 (which shouldn't happen since there's a kin cut)
    if(fabs(eta) < etaBins[etaBin])
      return etaBin;
  }
  
  return 10;
}
//---------------------------------------------------------------------------------------
// Gets the histogram Et bin given the et (MeV) -- corrresponds to fnEtBinsHist
unsigned int Root::TForwardElectronLikelihoodTool::getLikelihoodEtHistBin(double eT) const {
  const double GeV = 1000;
  
  const unsigned int nEtBins = fnEtBinsHist;
  const double eTBins[nEtBins] = {15*GeV,20*GeV,30*GeV,40*GeV,50*GeV,7000*GeV};
  
  for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
    if(eT < eTBins[eTBin]){
      //    ATH_MSG_INFO("Get ET " << eT << " etbin " << eTBins[eTBin] );
      return eTBin;
    }
  }
  
  // ATH_MSG_INFO("Get ET " << eT << " etbin " << eTBins[nEtBins-1] );
  return nEtBins-1; // Return the last bin if > the last bin.
  
}

//---------------------------------------------------------------------------------------
// Gets the bin name. Given the HISTOGRAM binning (fnEtBinsHist)
void Root::TForwardElectronLikelihoodTool::getBinName(char* buffer, int etbin,int etabin) const{
  static const double eta_bounds[11] = {2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.16, 3.35, 3.6, 4.9};
  static const int et_bounds[fnEtBinsHist] = {15,20,30,40,50,7000};
  snprintf(buffer, 200,"et%deta%0.2f", et_bounds[etbin], eta_bounds[etabin]);
  return;
}


// These are the variables availalble in the likelihood.
const char* Root::TForwardElectronLikelihoodTool::fVariables[fnVariables] = {"el_secondLambda"
									     ,"el_lateral"
									     ,"el_longitudinal"
									     ,"el_centerLambda"
									     ,"el_fracMax"
									     ,"el_secondR"
									     ,"el_significance"
									     ,"el_secondDensity"							     };

//=============================================================================
// SafeTH1, to allow us to immediately free the ROOT TH1 memory
//=============================================================================

Root::TForwardElectronLikelihoodTool::SafeTH1::SafeTH1(TH1F* roothist){

  int nbins = roothist->GetNbinsX();

  m_binContent.resize(nbins,0); // Note that the PDF over/underflows are unused and thus unrepresented here!

  for(int i = 0; i < nbins; ++i){
    m_binContent[i] = roothist->GetBinContent(i+1);
  }

  m_firstBinLowEdge = roothist->GetBinLowEdge(1);
  m_lastBinLowEdge  = roothist->GetBinLowEdge(nbins);
  m_binWidth        = (m_lastBinLowEdge - m_firstBinLowEdge) / (GetNbinsX() - 1);
  m_integral        = roothist->Integral(1,nbins);

  return;
}

Root::TForwardElectronLikelihoodTool::SafeTH1::~SafeTH1(){
  return;
}

int Root::TForwardElectronLikelihoodTool::SafeTH1::GetNbinsX(){
  int n = m_binContent.size();
  return n;
}

int Root::TForwardElectronLikelihoodTool::SafeTH1::FindBin(double value){

  if(value < m_firstBinLowEdge){
    return 0; // first bin of m_binContent
  }
  if(value > m_lastBinLowEdge){
    return GetNbinsX() - 1; // last bin of m_binContent
  }
   
  // note double rather than double due to incorrect rounding in O(1/10000) cases if double is used
  double bin_double = (value - m_firstBinLowEdge) / m_binWidth; 
  int bin = static_cast<int>(bin_double);
  
  return bin;
}

double Root::TForwardElectronLikelihoodTool::SafeTH1::GetBinContent(int bin){
  return m_binContent[bin];
}

double Root::TForwardElectronLikelihoodTool::SafeTH1::GetBinLowEdge(int bin){
  return m_firstBinLowEdge + m_binWidth*bin;
}

double Root::TForwardElectronLikelihoodTool::SafeTH1::Integral(){
  return m_integral;
}
