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
  doPileupTransform(false),
  VariableNames(""),
  PdfFileName(""),
  m_ipBinning(""),
  m_pdfFile(0),
  m_resultPrefix(""),
  m_resultName("likelihood"),
  m_cutPosition_kinematic(-9),
  m_cutPosition_LH(-9),
  m_resultPosition_LH(-9),
  OperatingPoint()
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
          getBinName( binname, et_tmp, eta_tmp, ip, m_ipBinning );
	  
	  char pdfdir[500];
	  sprintf(pdfdir,"%s/%s",vstr.c_str(),sig_bkg.c_str());
	  char pdf[500];
	  sprintf(pdf,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
	  
	  if (!m_pdfFile->GetListOfKeys()->Contains(vstr.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
	  }
	  if (!((TDirectory*)m_pdfFile->Get(vstr.c_str()))->GetListOfKeys()->Contains(sig_bkg.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
	  }
//  std::cout << pdf << "      "  <<binname << std::endl;
	  
          // PS: it shouldn't go to the last bin (which is 7000 GeV)
          if ( et > fnEtBinsHistOrig-1 ) {
            continue;
          }


	  // Use the first Et bin given in the root file for all Et ranges below
          if (et == 0 && !((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            // ATH_MSG_INFO( "using lowest GeV bin in place of all below." );
	    getBinName( binname, et_tmp+1, eta_tmp, ip, m_ipBinning );
	    sprintf(pdf,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
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
    ATH_MSG_DEBUG("This electron is fabs(eta)<2.5 Returning False.");
    ATH_MSG_INFO(" and the eta is ." <<vars_struct.eta );
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
  
  double cutDiscriminant;
  unsigned int ibin_combined = etbin*10+etabin-1;
//  std::cout<<"ibin_combined    "<<ibin_combined<<std::endl; 
  if(CutLikelihood.size()){
    // To protect against a binning mismatch, which should never happen
    if(ibin_combined > CutLikelihood.size()){
      ATH_MSG_ERROR("Somehow the desired bin is outside of the range! This should never happen!");
      ATH_MSG_INFO ( " the comb bin is " << ibin_combined << " lh size " << CutLikelihood.size() );
      
    }
    
    cutDiscriminant = CutLikelihood[ibin_combined];

    // If doPileupTransform, then correct the discriminant itself instead of the cut value
    if (doPileupTransform) { // PS: but this is opposite of what PU transform means in the central ele tool, you're doing the so-called LikelihoodPileupCorrection
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
								     double ip,
								     int set ) const
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
  vars.set             = set            ;
  
  return calculate(vars);
}

// The main public method to actually calculate the likelihood value
const Root::TResult& Root::TForwardElectronLikelihoodTool::calculate(LikeEnumForward::LHCalcVars_t& vars_struct)  const
{
  // Reset the results to defaul values
  m_result.setResult( m_resultPosition_LH, -999.0  );
  
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);
    
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
  double lhVal = this->EvaluateLikelihood(vec,vars_struct.eT,vars_struct.eta,vars_struct.ip,vars_struct.set);
  m_result.setResult( m_resultPosition_LH, lhVal  );
  
  return m_result;
}


  

double Root::TForwardElectronLikelihoodTool::EvaluateLikelihood(std::vector<float> varVector,double et,double eta,double ip,int set) const
{
  std::vector<double> vec;
  for(unsigned int var = 0; var < fnVariables; var++){
    vec.push_back(varVector[var]);
  }
  return EvaluateLikelihood(vec,et,eta,ip,set);//,mask);  
}



double Root::TForwardElectronLikelihoodTool::EvaluateLikelihood(std::vector<double> varVector,double et,double eta,double ip,int set) const
{

  // PS: I have no clue what the set is supposed to be, Hanlin please comment
  if( (set>8) || (set<1) ) {
    set=4;
  }
  
  const double GeV = 1000;
  unsigned int etbin = (getLikelihoodEtHistBin(et) > 2) ? (getLikelihoodEtHistBin(et)-2):getLikelihoodEtHistBin(et); // hist binning
  unsigned int etabin = getLikelihoodEtaBin(eta);
  if( (etbin==5) && (etabin==10) ) { // PS: what is this doing? Can't this be fixed in the configs?
    etabin = 9;
  }
  unsigned int ipbin  = getIpBin(ip);

  ATH_MSG_INFO("et: " << et << " eta: " << eta 
	       << " etbin: " << etbin << " etabin: " << etabin);
  
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
      if (integral == 0) {
	ATH_MSG_WARNING("Error! PDF integral == 0!");
	return -1.35;
      }
      
      prob = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;
      
      if   (s_or_b == 0) SigmaS *= prob;
      else if (s_or_b == 1) SigmaB *= prob;
    }
  }
  
  return TransformLikelihoodOutput( SigmaS, SigmaB, ip, et, eta );
}





















// --------------------------------------------
double Root::TForwardElectronLikelihoodTool::TransformLikelihoodOutput(double ps,double pb, double ip, double et, double eta) const {
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

  // here is where you should do the pileup transform

  
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
  
  for(unsigned int etaBin = 1; etaBin < nEtaBins; ++etaBin){ // PS: start loop at 1 so underflow is applied in case we're below 2.5 (which shouldn't happen since there's a kin cut)
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
void Root::TForwardElectronLikelihoodTool::getBinName(char* buffer, int etbin,int etabin, int ipbin, std::string iptype) const{
  double eta_bounds[11] = {2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.16, 3.35, 3.6, 4.9};//ATTENTION PS: what attnetion?
  int et_bounds[fnEtBinsHist] = {15,20,30,40,50,7000};
  if (!iptype.empty())
    sprintf(buffer, "%s%det%02deta%0.2f", iptype.c_str(), int(fIpBounds[ipbin]), et_bounds[etbin], eta_bounds[etabin]);
  else 
    sprintf(buffer, "et%deta%0.2f", et_bounds[etbin], eta_bounds[etabin]);
  return;
}


// PS: the bitmask is not even used
// //----------------------------------------------------------------------------------------
// unsigned int Root::TForwardElectronLikelihoodTool::GetLikelihoodBitmask(std::string vars) const{
//   unsigned int mask = 0x0;
//   ATH_MSG_DEBUG ("Variables to be used: ");
//   for(unsigned int var = 0; var < fnVariables; var++){
//     if (vars.find(fVariables[var]) != std::string::npos){
//       ATH_MSG_DEBUG( fVariables[var] );
//       mask = mask | 0x1 << var;
//     }
//   }
//   ATH_MSG_DEBUG("mask: " << mask);
//   return mask;
// }
//----------------------------------------------------------------------------------------
// Note that this will only perform the cut interpolation up to ~45 GeV, so 
// no smoothing is done above this for the high ET LH binning yet


//----------------------------------------------------------------------------------------
// Note that this will only perform the PDF interpolation up to ~45 GeV, so 
// no smoothing is done above this for the high ET LH binning yet

//----------------------------------------------------------------------------------------

// These are the variables availalble in the likelihood.
const char* Root::TForwardElectronLikelihoodTool::fVariables[fnVariables] = {"el_secondLambda"
									     ,"el_lateral"
									     ,"el_longitudinal"
									     ,"el_centerLambda"
									     ,"el_fracMax"
									     ,"el_secondR"
									     ,"el_significance"
									     ,"el_secondDensity"							     };
//
// Cuts definitions
//

// PS: this stuff is not needed, we work with config files
// // These are the discriminant values to compare against for various operating points.
// // Binning : [mu or nvtx][Et][eta]
// // Note that the second bin is an exact copy of the first, since they are one bin in the pdf histograms.


// const double Root::TForwardElectronLikelihoodTool::Disc_Loose[40] = { 0.054,	0.185,	0.328,	-0.324,	-0.516,	-15.197,-15.197,0,	-0.859,	0.104,0.027,	0.197,	0.315,	0.024,	-0.181,	-0.458,	-0.569,	0,	-0.151,	0.079,-0.015,	0.181,	0.335,	0.07,	-0.09,	0.334,	-0.524,	0,	-0.011,	0.105,0.017,	0.07,	0.126,	0.064,	-0.073,	-0.154,	-0.278,	0,	0.038,	0.188};

// const double Root::TForwardElectronLikelihoodTool::Disc_Medium[40] = {  0.275,	0.616,	0.698,	0.438,	0.405,	-0.125,	-0.232,	0,	0.268,	0.897,0.262,	0.634,	0.673,	0.495,	0.433,	0.271,	0.03,	0,	0.322,	0.341,0.219,	0.595,	0.678,	0.504,	0.451,	0.304,	0.042,	0,	0.385,	0.319,0.177,	0.268,	0.358,	0.315,	0.207,	0.232,	0.044,	0,	0.285,	0.349};

// const double Root::TForwardElectronLikelihoodTool::Disc_Tight[40] = { 0.387,	0.797,	0.869,	0.761,	0.756,0,	0.63,	0.309,0.388,	0.813,	0.838,	0.756,	0.752,0	,0.701,	0.358,0.346,	0.76	,0.812,	0.741,	0.735,0,	0.647,	0.328,0.264,	0.362,	0.459	,0.445,	0.375,0,	0.416,	0.186};


// //
// // Vertex-dependent term d = a + bx where x is number of vertices
// //
// const double Root::TForwardElectronLikelihoodTool::Disc_Loose_b[40] = { 0.514,	0.605,	0.818,	-0.104,	-0.626,	-15.197,-15.197,0,	-0.389,	0.594,0.327,	0.687,	0.805,	0.514,	0.309,	0.032,	-0.569,	0,	0.279,	0.569,0.195,	0.481,	0.765,	0.54,	0.4,	0.156,	-0.524,	0,	0.399,	0.455,0.127,	0.35,	0.426,	0.394,	0.307,	0.166,	-0.128,	0,	0.368,	0.188};
// const double Root::TForwardElectronLikelihoodTool::Disc_Medium_b[40] = { 0.705,	1.106,	1.148,	0.778,	0.895,	0.365,	0.128,	0,	0.758,	0.407,0.432,	1.044,	1.013,	0.965,	0.903,	0.721,	0.21,	0,	0.812,	0.801,0.399,	0.825,	0.888,	0.754,	0.791,	0.564,	0.202,	0,	0.765,	0.729,0.267,	0.478,	0.528,	0.565,	0.367,	0.462,	0.234,	0,	0.685,	0.349
// };

// const double Root::TForwardElectronLikelihoodTool::Disc_Tight_b[40] = {0.687,	1.257,	1.299,	1.251,	1.226,0,	1.12,	0.739,0.628,	1.143,	1.048,1.096,	1.092,0,	0.991,	0.478,0.496,	0.97	,0.972,	0.941,	0.935,0,	0.847,	0.438,0.444,	0.532,	0.569,	0.585,	0.545,0,	0.556,	0.296};

// const double Root::TForwardElectronLikelihoodTool::Disc_Medium_a[40] = { -0.035,	-0.039,	-0.036,	-0.027,	-0.036,	-0.037,	-0.026,	0,	-0.039,	-0.039,-0.015,-0.033,	-0.028,	-0.038,	-0.038,	-0.036,	-0.015,	0,	-0.039,	-0.037,-0.015,	-0.019,	-0.018,	-0.021,	-0.028,	-0.02,	-0.014,	0,	-0.031,	-0.03,-0.007,	-0.017,	-0.014,	-0.021,	-0.014,	-0.018,	-0.016,	0,	-0.032,	0};

// const double Root::TForwardElectronLikelihoodTool::Disc_Tight_a[40] = { -0.025,	-0.037,	-0.035,	-0.039,	-0.038,	0,-0.036,	-0.035,-0.02,	-0.026,	-0.018,	-0.028,	-0.028,	0,-0.024,	-0.011,-0.012,	-0.018,	-0.014,	-0.017,	-0.017,	0,-0.017,	-0.009,-0.014,	-0.015,	-0.009,	-0.012,	-0.015,	0,-0.012,	-0.009};
// const double Root::TForwardElectronLikelihoodTool::Disc_Loose_a[40] = {-0.037,	-0.034,	-0.039,	-0.018,	-0.01,	0,	    0,	    0,	-0.038,	-0.039,-0.025,	-0.039, -0.039,	-0.039,	-0.037,	-0.039,	0,   	0,	-0.035,	-0.039,-0.017,	-0.025,	-0.033,	-0.038,	-0.037,	-0.039,	0,	    0,	-0.033,	-0.028,-0.009,	-0.023,	-0.025,	-0.026, -0.031,	-0.026,	-0.013,	0,	-0.026,	0};



//=============================================================================
// SafeTH1, to allow us to immediately free the ROOT TH1 memory
//=============================================================================

Root::TForwardElectronLikelihoodTool::SafeTH1::SafeTH1(TH1F* roothist){

  int nbins = roothist->GetNbinsX();
//  std::cout<<"hehe "<<nbins<<std::endl;
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
//std::cout<<"test FindBin"<<std::endl;
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
