/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"
#include <cmath>
#include "TSystem.h"
#include "TROOT.h"

/** 
    Author : Kurt Brendlinger <kurb@sas.upenn.edu>
    Please see TElectronLikelihoodTool.h for usage.
*/

//=============================================================================
// Constructor
//=============================================================================

//----------------------------------------------------------------------------------------
Root::TElectronLikelihoodTool::TElectronLikelihoodTool(const char* name) :
  TCalculatorToolBase(name),
  TSelectorToolBase(name),
  asg::AsgMessaging(std::string(name)),
  doCutConversion(false),
  doRemoveF3AtHighEt(false),
  doRemoveTRTPIDAtHighEt(false),
  doSmoothBinInterpolation(false),
  useHighETLHBinning(false),
  useOneExtraHighETLHBin(false),
  doPileupTransform(false),
  DiscMaxForPileupTransform(2.0),
  PileupMaxForPileupTransform(50),
  VariableNames(""),
  OperatingPoint(LikeEnum::VeryLoose),
  PdfFileName(""),
  m_variableBitMask(0x0),
  m_ipBinning(""),
  m_pdfFile(0),
  m_resultPrefix(""),
  m_resultName("likelihood"),
  m_cutPosition_kinematic(-9),
  m_cutPosition_NSilicon(-9),
  m_cutPosition_NPixel(-9),
  m_cutPosition_NBlayer(-9),
  m_cutPosition_conversion(-9),
  m_cutPosition_LH(-9),
  m_cutPositionTrackA0(-9),
  m_cutPositionTrackMatchEta(-9),
  m_cutPositionTrackMatchPhiRes(-9),
  m_cutPositionWstotAtHighET(-9),
  m_cutPositionEoverPAtHighET(-9),
  m_resultPosition_LH(-9)
{
  for(unsigned int varIndex = 0; varIndex < fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < IP_BINS; ip++){
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
Root::TElectronLikelihoodTool::~TElectronLikelihoodTool()
{
  for(unsigned int varIndex = 0; varIndex < fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < IP_BINS; ip++){
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


int Root::TElectronLikelihoodTool::initialize()
{
  ATH_MSG_DEBUG( "TElectronLikelihoodTool initialize.");

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
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
  // if ( m_cutPosition_nSCTMin < 0 ) sc = 0; // Exceeded the number of allowed cuts (32)
  
  // Cut position for the kineatic pre-selection
  m_cutPosition_kinematic = m_accept.addCut( "kinematic", "pass kinematic" );
  if ( m_cutPosition_kinematic < 0 ) sc = 0;

  // NSilicon
  m_cutPosition_NSilicon = m_accept.addCut( "NSilicon", "pass NSilicon" );
  if ( m_cutPosition_NSilicon < 0 ) sc = 0;

  // NPixel
  m_cutPosition_NPixel = m_accept.addCut( "NPixel", "pass NPixel" );
  if ( m_cutPosition_NPixel < 0 ) sc = 0;

  // NBlayer
  m_cutPosition_NBlayer = m_accept.addCut( "NBlayer", "pass NBlayer" );
  if ( m_cutPosition_NBlayer < 0 ) sc = 0;

  // Conversion
  m_cutPosition_conversion = m_accept.addCut( "conversion", "pass conversion" );
  if ( m_cutPosition_conversion < 0 ) sc = 0;

  // Cut position for the likelihood selection - DO NOT CHANGE ORDER!
  m_cutPosition_LH = m_accept.addCut( "passLH", "pass Likelihood" );
  if ( m_cutPosition_LH < 0 ) sc = 0;

  // D0
  m_cutPositionTrackA0 = m_accept.addCut( "TrackA0", "A0 (aka d0) wrt beam spot < Cut" );
  if ( m_cutPositionTrackA0 < 0 ) sc = 0;

  // deltaeta
  m_cutPositionTrackMatchEta = m_accept.addCut("TrackMatchEta", "Track match deta in 1st sampling < Cut");
  if ( m_cutPositionTrackMatchEta < 0 ) sc = 0;

  // deltaphi
  m_cutPositionTrackMatchPhiRes = m_accept.addCut( "TrackMatchPhiRes", "Track match dphi in 2nd sampling, rescaled < Cut" );
  if ( m_cutPositionTrackMatchPhiRes < 0 ) sc = 0;
  
  // Wstot
  m_cutPositionWstotAtHighET = m_accept.addCut( "WstotAtHighET", "Above 125 GeV, Wstot < Cut" );
  if ( m_cutPositionWstotAtHighET < 0 ) sc = 0;

  // EoverP
  m_cutPositionEoverPAtHighET = m_accept.addCut( "EoverPAtHighET", "Above 125 GeV, EoverP < Cut" );
  if ( m_cutPositionEoverPAtHighET < 0 ) sc = 0;

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
  m_resultPosition_LH = m_result.addResult( (m_resultPrefix+m_resultName).c_str(), "electron likelihood" );
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
  m_variableBitMask = GetLikelihoodBitmask(VariableNames);
  setOperatingPoint(OperatingPoint);
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
  //----------End File/Histo operation------------------------------------

    ATH_MSG_DEBUG("Initialization complete for a LH tool with these specs:"
		  << "\n - PdfFileName                                  : " << PdfFileName
		  << "\n - OperatingPoint                               : " << (int)OperatingPoint
		  << "\n - Result name                                  : " << (m_resultPrefix+m_resultName).c_str()
		  << "\n - Variable bitmask                             : " << m_variableBitMask);

    if (OperatingPoint != LikeEnum::CustomOperatingPoint) {
      ATH_MSG_DEBUG("   Taking all values from LikeEnum Operating point!"
		    << "\n   To customize use LikeEnum::CustomOperatingPoint.");
    }
    else {
      ATH_MSG_DEBUG("\n - VariableNames                                : " << VariableNames
		    << "\n - (bool)CutBL (yes/no)                         : " << (CutBL.size() ? "yes" : "no")
		    << "\n - (bool)CutPi (yes/no)                         : " << (CutPi.size() ? "yes" : "no")
		    << "\n - (bool)CutSi (yes/no)                         : " << (CutSi.size() ? "yes" : "no")
		    << "\n - (bool)doCutConversion (yes/no)               : " << (doCutConversion ? "yes" : "no")
		    << "\n - (bool)doRemoveF3AtHighEt (yes/no)            : " << (doRemoveF3AtHighEt ? "yes" : "no")
		    << "\n - (bool)doRemoveTRTPIDAtHighEt (yes/no)        : " << (doRemoveTRTPIDAtHighEt ? "yes" : "no")
		    << "\n - (bool)doSmoothBinInterpolation (yes/no)      : " << (doSmoothBinInterpolation ? "yes" : "no")
		    << "\n - (bool)useHighETLHBinning (yes/no)            : " << (useHighETLHBinning ? "yes" : "no")
		    << "\n - (bool)useOneExtraHighETLHBin(yes/no)         : " << (useOneExtraHighETLHBin ? "yes" : "no")
		    << "\n - (bool)doPileupTransform (yes/no)             : " << (doPileupTransform ? "yes" : "no")
		    << "\n - (bool)CutLikelihood (yes/no)                 : " << (CutLikelihood.size() ? "yes" : "no")
		    << "\n - (bool)CutLikelihoodPileupCorrection (yes/no) : " << (CutLikelihoodPileupCorrection.size() ? "yes" : "no")
		    << "\n - (bool)CutA0 (yes/no)                         : " << (CutA0.size() ? "yes" : "no")
		    << "\n - (bool)CutDeltaEta (yes/no)                   : " << (CutDeltaEta.size() ? "yes" : "no")
		    << "\n - (bool)CutDeltaPhiRes (yes/no)                : " << (CutDeltaPhiRes.size() ? "yes" : "no")
		    << "\n - (bool)CutWstotAtHighET (yes/no)              : " << (CutWstotAtHighET.size() ? "yes" : "no")
		    << "\n - (bool)CutEoverPAtHighET (yes/no)             : " << (CutEoverPAtHighET.size() ? "yes" : "no")
		    );
    }
  return sc;
}

int Root::TElectronLikelihoodTool::LoadVarHistograms(std::string vstr,unsigned int varIndex){
  for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
    for (unsigned int ip = 0; ip < IP_BINS; ip++){
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
          
	  if (((std::string(binname).find("2.37") != std::string::npos)) && (vstr.find("el_f3") != std::string::npos))
	    continue;

	  if (((std::string(binname).find("2.01") != std::string::npos) || (std::string(binname).find("2.37") != std::string::npos))
	      && (vstr.find("TRT") != std::string::npos))
	    continue;
	  
	  char pdfdir[500];
	  sprintf(pdfdir,"%s/%s",vstr.c_str(),sig_bkg.c_str());
	  char pdf[500];
	  sprintf(pdf,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
	  char pdf_newname[500];
	  sprintf(pdf_newname,"%s_%s_%s_LHtool_copy_%s",Root::TSelectorToolBase::getName(),vstr.c_str(),sig_bkg.c_str(),binname);

	  if (!m_pdfFile->GetListOfKeys()->Contains(vstr.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
	  }
	  if (!((TDirectory*)m_pdfFile->Get(vstr.c_str()))->GetListOfKeys()->Contains(sig_bkg.c_str())){
            ATH_MSG_INFO("Warning: skipping variable " << vstr << " because the folder does not exist.");
            return 1;
	  }

          // For backwards compatibility:
          // If we are not using the high ET LH binning, we only need to load PDFs 
          // up to a certain ET value (40 GeV)
          if(!useHighETLHBinning && et > fnEtBinsHistOrig-1){
            continue;
          }

          // If the 0th et bin (4-7 GeV) histogram does not exist in the root file,
          // then just use the 7-10 GeV bin histogram. 
          // This should preserve backward compatibility
          if (et == 0 && !((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            //std::cout << "Info: using 7 GeV bin in place of 4 GeV bin." << std::endl;
            getBinName( binname, et_tmp+1, eta_tmp, ip, m_ipBinning );
            sprintf(pdf,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
            sprintf(pdf_newname,"%s_%s_%s_LHtool_copy4GeV_%s",Root::TSelectorToolBase::getName(),vstr.c_str(),sig_bkg.c_str(),binname);
          }
          if (((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            TH1F* hist = (TH1F*)(((TDirectory*)m_pdfFile->Get(pdfdir))->Get(pdf));
            fPDFbins[s_or_b][ip][et][eta][varIndex] = new TElectronLikelihoodTool::SafeTH1(hist);
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

const Root::TAccept& Root::TElectronLikelihoodTool::accept( double likelihood,
                                                            double eta, double eT,
                                                            int nSi,int nSiDeadSensors, int nPix, int nPixDeadSensors,
                                                            int nBlayer, int nBlayerOutliers, bool expectBlayer,
							    int nNextToInnerMostLayer, int nNextToInnerMostLayerOutliers, bool expectNextToInnerMostLayer,
                                                            int convBit, double d0, double deltaEta, double deltaphires, 
                                                            double wstot, double EoverP, double ip
                                                            ) const
{
  LikeEnum::LHAcceptVars_t vars;
  
  vars.likelihood      = likelihood     ;
  vars.eta             = eta            ;
  vars.eT              = eT             ;
  vars.nSi             = nSi            ;
  vars.nSiDeadSensors  = nSiDeadSensors ;
  vars.nPix            = nPix           ;
  vars.nPixDeadSensors = nPixDeadSensors;
  vars.nBlayer         = nBlayer        ;
  vars.nBlayerOutliers = nBlayerOutliers;
  vars.expectBlayer    = expectBlayer   ;
  vars.nNextToInnerMostLayer        = nNextToInnerMostLayer        ;
  vars.nNextToInnerMostLayerOutliers = nNextToInnerMostLayerOutliers;
  vars.expectNextToInnerMostLayer = expectNextToInnerMostLayer   ;
  vars.convBit         = convBit        ;
  vars.d0              = d0             ;
  vars.deltaEta        = deltaEta       ;
  vars.deltaphires     = deltaphires    ;
  vars.wstot           = wstot          ;
  vars.EoverP          = EoverP         ;
  vars.ip              = ip             ;
  
  return accept(vars);
}

// This method calculates if the current electron passes the requested likelihood cut
const Root::TAccept& Root::TElectronLikelihoodTool::accept( LikeEnum::LHAcceptVars_t& vars_struct ) const
{
  // Reset the cut result bits to zero (= fail cut)
  m_accept.clear();

  ATH_MSG_DEBUG("Likelihood macro: Using operating point " << OperatingPoint);

  // Set up the individual cuts
  bool passKine(true);
  bool passNSilicon(true);
  bool passNPixel(true);
  bool passNBlayer(true);
  bool passConversion(true);
  bool passLH(true);
  bool passTrackA0(true);
  bool passDeltaEta(true);
  bool passDeltaPhiRes(true);
  bool passWstotAtHighET(true);
  bool passEoverPAtHighET(true);
  
  if (fabs(vars_struct.eta) > 2.47) {
    ATH_MSG_DEBUG("This electron is fabs(eta)>2.47 Returning False.");
    passKine = false;
  }
  
  unsigned int etbin = getLikelihoodEtDiscBin(vars_struct.eT);
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);
  //unsigned int ipbin  = 0;

  // sanity
  if (etbin  >= fnDiscEtBins) {
    ATH_MSG_WARNING( "Cannot evaluate likelihood for Et " << vars_struct.eT<< ". Returning false..");
    passKine = false;
  }

  // Return if the kinematic requirements are not fulfilled
  m_accept.setCutResult( m_cutPosition_kinematic, passKine );
  if ( !passKine ){ return m_accept; }

  // conversion bit
  if (doCutConversion && vars_struct.convBit){
      ATH_MSG_DEBUG("Likelihood macro: Conversion Bit Failed." );
      passConversion = false;
  }

  // blayer cut
  if (CutBL.size() ) {
    if(vars_struct.expectBlayer) {
      if(vars_struct.nBlayer + vars_struct.nBlayerOutliers < CutBL[etabin]) {
	ATH_MSG_DEBUG("Likelihood macro: Inner most Blayer Failed." );
	passNBlayer = false;
      }
    }  
    else if(vars_struct.expectNextToInnerMostLayer) { // When we do not expect IBL but next to inner 
      if(vars_struct.nNextToInnerMostLayer + vars_struct.nNextToInnerMostLayerOutliers < CutBL[etabin]) {
	ATH_MSG_DEBUG("Likelihood macro: Next Inner Failed when not expecting." );
	passNBlayer = false;
      } 
    }
  }
  
  // pixel cut
  if (CutPi.size()){
    if (vars_struct.nPix+vars_struct.nPixDeadSensors < CutPi[etabin]){
      ATH_MSG_DEBUG("Likelihood macro: Pixels Failed.");
      passNPixel = false;
    }
  }
  
  // silicon cut
  if (CutSi.size()){
    if (vars_struct.nSi + vars_struct.nSiDeadSensors < CutSi[etabin]){
      ATH_MSG_DEBUG( "Likelihood macro: Silicon Failed.");
      passNSilicon = false;
    }
  }
  
  double cutDiscriminant;
  unsigned int ibin_combined = etbin*10+etabin; // Must change if number of eta bins changes!. Also starts from 7-10 GeV bin.

  // To protect against a binning mismatch, which should never happen
  if(ibin_combined > CutLikelihood.size()){
    ATH_MSG_ERROR("Somehow the desired bin is outside of the range specified by the conf file! This should never happen!");
  }
        
  if (doSmoothBinInterpolation){
    cutDiscriminant = InterpolateCuts(CutLikelihood,CutLikelihood4GeV,vars_struct.eT,vars_struct.eta);
    if (!doPileupTransform)
      cutDiscriminant += vars_struct.ip*InterpolateCuts(CutLikelihoodPileupCorrection,CutLikelihoodPileupCorrection4GeV,vars_struct.eT,vars_struct.eta);
  } else {
    if (vars_struct.eT > 7000. || !CutLikelihood4GeV.size()){
      cutDiscriminant = CutLikelihood[ibin_combined];
      // If doPileupTransform, then correct the discriminant itself instead of the cut value
      if (!doPileupTransform && CutLikelihoodPileupCorrection.size()) 
        cutDiscriminant += vars_struct.ip*CutLikelihoodPileupCorrection[ibin_combined];
    }
    else {
      cutDiscriminant = CutLikelihood4GeV[etabin];
      if (!doPileupTransform && CutLikelihoodPileupCorrection4GeV.size()) 
        cutDiscriminant += vars_struct.ip*CutLikelihoodPileupCorrection4GeV[etabin];
    }
  }

  // Determine if the calculated likelihood value passes the cut
    ATH_MSG_DEBUG("Likelihood macro: Discriminant: ");
  if ( vars_struct.likelihood < cutDiscriminant )
    {
      ATH_MSG_DEBUG("Likelihood macro: Disciminant Cut Failed.");
      passLH = false;
    }

  // d0 cut
  if (CutA0.size()){
    if (fabs(vars_struct.d0) > CutA0[ibin_combined]){
      ATH_MSG_DEBUG("Likelihood macro: D0 Failed.");
      passTrackA0 = false;
    }
  }

  // deltaEta cut
  if (CutDeltaEta.size()){
    if ( fabs(vars_struct.deltaEta) > CutDeltaEta[ibin_combined]){
      ATH_MSG_DEBUG("Likelihood macro: deltaEta Failed.");
      passDeltaEta = false;
    }
  }
  
  // deltaPhiRes cut
  if (CutDeltaPhiRes.size()){
    if ( fabs(vars_struct.deltaphires) > CutDeltaPhiRes[ibin_combined]){
      ATH_MSG_DEBUG("Likelihood macro: deltaphires Failed.");
      passDeltaPhiRes = false;
    }
  }

  // Only do this above 125 GeV
  if(vars_struct.eT > 125000.){
    // wstot cut
    if (CutWstotAtHighET.size()){
      if ( fabs(vars_struct.wstot) > CutWstotAtHighET[etabin]){
        ATH_MSG_DEBUG("Likelihood macro: wstot Failed.");
        passWstotAtHighET = false;
      }
    }

    // EoverP cut
    if (CutEoverPAtHighET.size()){
      if ( fabs(vars_struct.EoverP) > CutEoverPAtHighET[etabin]){
        ATH_MSG_DEBUG("Likelihood macro: EoverP Failed.");
        passEoverPAtHighET = false;
      }
    }
  }
  
  
  // Set the individual cut bits in the return object
  m_accept.setCutResult( m_cutPosition_NSilicon, passNSilicon );
  m_accept.setCutResult( m_cutPosition_NPixel, passNPixel );
  m_accept.setCutResult( m_cutPosition_NBlayer, passNBlayer );
  m_accept.setCutResult( m_cutPosition_conversion, passConversion );
  m_accept.setCutResult( m_cutPosition_LH, passLH );  
  m_accept.setCutResult( m_cutPositionTrackA0, passTrackA0 );  
  m_accept.setCutResult( m_cutPositionTrackMatchEta, passDeltaEta );  
  m_accept.setCutResult( m_cutPositionTrackMatchPhiRes, passDeltaPhiRes );  
  m_accept.setCutResult( m_cutPositionWstotAtHighET, passWstotAtHighET );  
  m_accept.setCutResult( m_cutPositionEoverPAtHighET, passEoverPAtHighET );  
  
  return m_accept;
}

const Root::TResult& Root::TElectronLikelihoodTool::calculate( double eta, double eT,double f3, double rHad, double rHad1,
                                                               double Reta, double w2, double f1, /*double wstot,*/ double eratio,
                                                               double deltaEta, double d0, double TRratio, /*double eOverP,*/
                                                               /*double deltaPhi,*/ double d0sigma, /*double fside,*/
                                                               /*double ptcone20,*/ double rphi, /*double ws3,*/
                                                               double deltaPoverP ,double deltaphires,
							       double TRT_PID,
                                                               double ip )  const
{

  LikeEnum::LHCalcVars_t vars;
 
  vars.eta         = eta        ;
  vars.eT          = eT         ;
  vars.f3          = f3         ;
  vars.rHad        = rHad       ;
  vars.rHad1       = rHad1      ;
  vars.Reta        = Reta       ;
  vars.w2          = w2         ;
  vars.f1          = f1         ;
  vars.eratio      = eratio     ;
  vars.deltaEta    = deltaEta   ;
  vars.d0          = d0         ;
  vars.TRratio     = TRratio    ;
  vars.d0sigma     = d0sigma    ;
  vars.rphi        = rphi       ;
  vars.deltaPoverP = deltaPoverP;
  vars.deltaphires = deltaphires;
  vars.TRT_PID     = TRT_PID    ;
  vars.ip          = ip         ;

  return calculate(vars);
}

// The main public method to actually calculate the likelihood value
const Root::TResult& Root::TElectronLikelihoodTool::calculate(LikeEnum::LHCalcVars_t& vars_struct)  const
{
  // Reset the results to defaul values
  m_result.setResult( m_resultPosition_LH, -999.0  );
  
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);
  double rhad_corr;
  if(etabin == 3 || etabin == 4) rhad_corr = vars_struct.rHad;
  else rhad_corr = vars_struct.rHad1;
  double d0significance = vars_struct.d0sigma == 0 ? 0. : fabs(vars_struct.d0)/vars_struct.d0sigma;
  
  double arr[] = {d0significance,vars_struct.eratio,vars_struct.deltaEta
                  ,vars_struct.f1,vars_struct.f3
                  ,vars_struct.Reta,rhad_corr,vars_struct.rphi
                  ,vars_struct.d0,vars_struct.TRratio,vars_struct.w2
                  ,vars_struct.deltaPoverP,vars_struct.deltaphires
		  ,vars_struct.TRT_PID};
  std::vector<double> vec (arr, arr + sizeof(arr) / sizeof(double) );
  
  // Calculate the actual likelihood value and fill the return object
  double lhVal = this->EvaluateLikelihood(vec,vars_struct.eT,vars_struct.eta,vars_struct.ip);
  m_result.setResult( m_resultPosition_LH, lhVal  );

  return m_result;
}


  

double Root::TElectronLikelihoodTool::EvaluateLikelihood(std::vector<float> varVector,double et,double eta,double ip) const
{
  std::vector<double> vec;
  for(unsigned int var = 0; var < fnVariables; var++){
    vec.push_back(varVector[var]);
  }
  return EvaluateLikelihood(vec,et,eta,ip);//,mask);  
}



double Root::TElectronLikelihoodTool::EvaluateLikelihood(std::vector<double> varVector,double et,double eta,double ip) const
{

  const double GeV = 1000;
  unsigned int etbin = getLikelihoodEtHistBin(et); // hist binning
  unsigned int etabin = getLikelihoodEtaBin(eta);
  unsigned int ipbin  = getIpBin(ip);

  ATH_MSG_DEBUG("et: " << et << " eta: " << eta 
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
    
    // Skip variables that are masked off (not used) in the likelihood
    if (!(m_variableBitMask & (0x1 << var))){
      continue;
    }
    // Don't use TRT for outer eta bins (2.01,2.37)
    if (((etabin == 8) || (etabin == 9)) && (varstr.find("TRT") != std::string::npos)){
      continue;
    }
    // Don't use f3 for outer eta bin (2.37)
    if ((etabin == 9) && (varstr.find("el_f3") != std::string::npos)){
      continue;
    }
    // Don't use f3 for high et (>100 GeV)
    if (doRemoveF3AtHighEt && (et > 100*GeV) && (varstr.find("el_f3") != std::string::npos)){
        continue;
    }
    // Don't use TRTPID for high et (>100 GeV)
    if (doRemoveTRTPIDAtHighEt && (et > 100*GeV) && (varstr.find("el_TRT_PID") != std::string::npos)){
        continue;
    }
    for (unsigned int s_or_b=0; s_or_b<2;s_or_b++) {
      
      std::string sig_bkg = (s_or_b==0) ? "sig" : "bkg" ;

      int bin = fPDFbins[s_or_b][ipbin][etbin][etabin][var]->FindBin(varVector[var]);

      double prob = 0;
      if (doSmoothBinInterpolation) {
        prob = InterpolatePdfs(s_or_b,ipbin,et,eta,bin,var);
      } 
      else {
        double integral = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->Integral());
        if (integral == 0) {
          ATH_MSG_WARNING("Error! PDF integral == 0!");
          return -1.35;
        }
        
        prob = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;
      }

      if   (s_or_b == 0) SigmaS *= prob;
      else if (s_or_b == 1) SigmaB *= prob;
    }
  }
  
  return TransformLikelihoodOutput( SigmaS, SigmaB, ip, et, eta );
}




// --------------------------------------------
double Root::TElectronLikelihoodTool::TransformLikelihoodOutput(double ps,double pb, double ip, double et, double eta) const {
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
    
  // Linearly transform the discriminant as a function of pileup, rather than
  // the old scheme of changing the cut value based on pileup. This is simpler for
  // the tuning, as well as ensuring subsets / making discriminants more transparent.
  if(doPileupTransform){

    // The variables used by the transform:
    //
    // - hard_cut_ref = extremely tight discriminant cut as reference to ensure
    // pileup correction for looser menus is less than for tighter menus.
    // - loose_ref = a loose menu with no pileup correction. Any tighter
    // menu with same inputs will necessarily have pileup dependence built in
    // - disc_max = max disc value for which pileup correction is desired.
    // - pileup_max = max nvtx or mu for calculating the transform. Any larger
    // pileup values will use this maximum value in the transform.

    if( DiscHardCutForPileupTransform.size() == 0 || DiscHardCutSlopeForPileupTransform.size() == 0 || DiscLooseForPileupTransform.size() == 0){
      ATH_MSG_WARNING("Vectors needed for pileup-dependent transform not correctly filled! Skipping the transform.");
      return disc;
    }

    unsigned int etabin = getLikelihoodEtaBin(eta);

    double disc_hard_cut_ref      ;
    double disc_hard_cut_ref_slope;
    double disc_loose_ref         ;
    double disc_max                = DiscMaxForPileupTransform;
    double pileup_max              = PileupMaxForPileupTransform;

    if (doSmoothBinInterpolation){
      disc_hard_cut_ref       = InterpolateCuts(DiscHardCutForPileupTransform,DiscHardCutForPileupTransform4GeV,et,eta);
      disc_hard_cut_ref_slope = InterpolateCuts(DiscHardCutSlopeForPileupTransform,DiscHardCutSlopeForPileupTransform4GeV,et,eta);
      disc_loose_ref          = InterpolateCuts(DiscLooseForPileupTransform,DiscLooseForPileupTransform4GeV,et,eta);
    } else {
      // default situation, in the case where 4-7 GeV bin is not defined
      if (et > 7000. || !DiscHardCutForPileupTransform4GeV.size()){
        unsigned int etfinebin = getLikelihoodEtDiscBin(et);
        unsigned int ibin_combined = etfinebin*10+etabin;
        disc_hard_cut_ref       = DiscHardCutForPileupTransform[ibin_combined];
        disc_hard_cut_ref_slope = DiscHardCutSlopeForPileupTransform[ibin_combined];
        disc_loose_ref          = DiscLooseForPileupTransform[ibin_combined];
      } else {
        if( DiscHardCutForPileupTransform4GeV.size() == 0 || DiscHardCutSlopeForPileupTransform4GeV.size() == 0 || DiscLooseForPileupTransform4GeV.size() == 0){
          ATH_MSG_WARNING("Vectors needed for pileup-dependent transform not correctly filled for 4-7 GeV bin! Skipping the transform.");
          return disc;
        }
        disc_hard_cut_ref       = DiscHardCutForPileupTransform4GeV[etabin];
        disc_hard_cut_ref_slope = DiscHardCutSlopeForPileupTransform4GeV[etabin];
        disc_loose_ref          = DiscLooseForPileupTransform4GeV[etabin];
      }
    }

    double disc_hard_cut_ref_prime = disc_hard_cut_ref + disc_hard_cut_ref_slope * std::min(ip,pileup_max);

    if(disc <= disc_loose_ref){
      // Below threshold for applying pileup correction
      //disc = disc;
    }
    else if(disc <= disc_hard_cut_ref_prime){
      // Between the loose and hard cut reference points for pileup correction
      double denom = double(disc_hard_cut_ref_prime - disc_loose_ref);
      if(denom < 0.001) denom = 0.001;
      disc = disc_loose_ref + (disc - disc_loose_ref) * (disc_hard_cut_ref - disc_loose_ref) / denom;
    }
    else if(disc_hard_cut_ref_prime < disc && disc <= disc_max){
      // Between the hard cut and max reference points for pileup correction
      double denom = double(disc_max - disc_hard_cut_ref_prime);
      if(denom < 0.001) denom = 0.001;
      disc = disc_hard_cut_ref + (disc - disc_hard_cut_ref_prime) * (disc_max - disc_hard_cut_ref) / denom;
    }
    else{
      // Above threshold where pileup correction necessary
      //disc = disc;
    }
  }

  ATH_MSG_DEBUG( "disc is " << disc );
  return disc;
}



const double Root::TElectronLikelihoodTool::fIpBounds[IP_BINS+1] = {0.,500.};

//---------------------------------------------------------------------------------------
// Gets the IP bin
unsigned int Root::TElectronLikelihoodTool::getIpBin(double ip) const{
  for(unsigned int ipBin = 0; ipBin < IP_BINS; ++ipBin){
    if(ip < fIpBounds[ipBin+1])
      return ipBin;
  }
  return 0;
}


//---------------------------------------------------------------------------------------
// Gets the Eta bin [0-9] given the eta
unsigned int Root::TElectronLikelihoodTool::getLikelihoodEtaBin(double eta) const{
  const unsigned int nEtaBins = 10;
  const double etaBins[nEtaBins] = {0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};
  
  for(unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin){
    if(fabs(eta) < etaBins[etaBin])
      return etaBin;
  }
  
  return 9;
}
//---------------------------------------------------------------------------------------
// Gets the histogram Et bin given the et (MeV) -- corrresponds to fnEtBinsHist
unsigned int Root::TElectronLikelihoodTool::getLikelihoodEtHistBin(double eT) const {
  const double GeV = 1000;

  if(useHighETLHBinning){
    const unsigned int nEtBins = fnEtBinsHist;
    const double eTBins[nEtBins] = {7*GeV,10*GeV,15*GeV,20*GeV,30*GeV,40*GeV,100*GeV,6000*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
        return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.
  }
  else{
    const unsigned int nEtBins = fnEtBinsHistOrig;
    const double eTBins[nEtBins] = {7*GeV,10*GeV,15*GeV,20*GeV,30*GeV,40*GeV,50*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
        return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.
  }
}

//---------------------------------------------------------------------------------------
// Gets the Et bin [0-10] given the et (MeV)
unsigned int Root::TElectronLikelihoodTool::getLikelihoodEtDiscBin(double eT) const{
  const double GeV = 1000;

  if(useHighETLHBinning){
    const unsigned int nEtBins = fnDiscEtBins;
    const double eTBins[nEtBins] = {10*GeV,15*GeV,20*GeV,25*GeV,30*GeV,35*GeV,40*GeV,45*GeV
                                   ,100*GeV,150*GeV,200*GeV,250*GeV,300*GeV,350*GeV,400*GeV
                                   ,450*GeV,500*GeV,600*GeV,700*GeV,800*GeV,900*GeV,1000*GeV
                                   ,1200*GeV,1400*GeV,1600*GeV,1800*GeV,2000*GeV,2200*GeV
                                   ,2400*GeV,2600*GeV,2800*GeV,3000*GeV,6000*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
        return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.
  }
  else if(useOneExtraHighETLHBin){
    const unsigned int nEtBins = fnDiscEtBinsOneExtra;
    const double eTBins[nEtBins] = {10*GeV,15*GeV,20*GeV,25*GeV,30*GeV,35*GeV,40*GeV,45*GeV,125*GeV,6000*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
        return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.

  }
  else{
    const unsigned int nEtBins = fnDiscEtBinsOrig;
    const double eTBins[nEtBins] = {10*GeV,15*GeV,20*GeV,25*GeV,30*GeV,35*GeV,40*GeV,45*GeV,50*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
        return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.
  }
}



//---------------------------------------------------------------------------------------
// Gets the bin name. Given the HISTOGRAM binning (fnEtBinsHist)
void Root::TElectronLikelihoodTool::getBinName(char* buffer, int etbin,int etabin, int ipbin, std::string iptype) const{
  double eta_bounds[9] = {0.0,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37};
  int et_bounds[fnEtBinsHist] = {4,7,10,15,20,30,40,100};
  if (!iptype.empty())
    sprintf(buffer, "%s%det%02deta%0.2f", iptype.c_str(), int(fIpBounds[ipbin]), et_bounds[etbin], eta_bounds[etabin]);
  else 
    sprintf(buffer, "et%deta%0.2f", et_bounds[etbin], eta_bounds[etabin]);
  return;
}


//----------------------------------------------------------------------------------------
unsigned int Root::TElectronLikelihoodTool::GetLikelihoodBitmask(std::string vars) const{
  unsigned int mask = 0x0;
  ATH_MSG_DEBUG ("Variables to be used: ");
  for(unsigned int var = 0; var < fnVariables; var++){
    if (vars.find(fVariables[var]) != std::string::npos){
      ATH_MSG_DEBUG( fVariables[var] );
      mask = mask | 0x1 << var;
    }
  }
  ATH_MSG_DEBUG("mask: " << mask);
  return mask;
}

//----------------------------------------------------------------------------------------
// Note that this will only perform the cut interpolation up to ~45 GeV, so 
// no smoothing is done above this for the high ET LH binning yet
double Root::TElectronLikelihoodTool::InterpolateCuts(const std::vector<double>& cuts,const std::vector<double>& cuts_4gev,double et,double eta) const{
  int etbin = getLikelihoodEtDiscBin(et);
  int etabin = getLikelihoodEtaBin(eta);
  unsigned int ibin_combined = etbin*10+etabin;
  double cut = cuts.at(ibin_combined);
  if (cuts_4gev.size() && et < 7000.) cut = cuts_4gev.at(etabin);
  if (et > 47500.) return cut; // interpolation stops here.
  if (cuts_4gev.size() == 0 && et < 8500.) return cut; // stops here
  if (cuts_4gev.size() > 0 && et < 6000.) return cut; // stops here
  double bin_width = 5000.;
  if (7000. < et && et < 10000.) bin_width = 3000.;
  if (et < 7000.) bin_width = 2000.;
  const double GeV = 1000; 
  const double eTBins[9] = {8.5*GeV,12.5*GeV,17.5*GeV,22.5*GeV,27.5*GeV,32.5*GeV,37.5*GeV,42.5*GeV,47.5*GeV};
  double bin_center = eTBins[etbin];
  if (et > bin_center) {
    double cut_next = cut;
    if (etbin+1<=8) cut_next = cuts.at((etbin+1)*10+etabin);
    return cut+(cut_next-cut)*(et-bin_center)/(bin_width);
  }
  // or else if et < bin_center :
  double cut_before = cut;
  if (etbin-1>=0) cut_before = cuts.at((etbin-1)*10+etabin);
  else if (etbin == 0 && cuts_4gev.size()) cut_before = cuts_4gev.at(etabin);
  return cut-(cut-cut_before)*(bin_center-et)/(bin_width);
}

//----------------------------------------------------------------------------------------
// Note that this will only perform the PDF interpolation up to ~45 GeV, so 
// no smoothing is done above this for the high ET LH binning yet
double Root::TElectronLikelihoodTool::InterpolatePdfs(unsigned int s_or_b,unsigned int ipbin,double et,double eta,int bin,unsigned int var) const{
  // histograms exist for the following bins: 4, 7, 10, 15, 20, 30, 40.
  // Interpolation between histograms must follow fairly closely the interpolation
  // scheme between cuts - so be careful!
  int etbin = getLikelihoodEtHistBin(et); // hist binning
  int etabin = getLikelihoodEtaBin(eta);
  double integral = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->Integral());
  double prob = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;
  if (et > 42500.) return prob; // interpolation stops here.
  if (et < 6000.) return prob; // interpolation stops here.
  if (22500. < et && et < 27500.) return prob; // region of non-interpolation for pdfs
  if (32500. < et && et < 37500.) return prob; // region of non-interpolation for pdfs
  double bin_width = 5000.;
  if (7000. < et && et < 10000.) bin_width = 3000.;
  if (et < 7000.) bin_width = 2000.;
  const double GeV = 1000; 
  const double eTHistBins[7] = {6.*GeV,8.5*GeV,12.5*GeV,17.5*GeV,22.5*GeV,32.5*GeV,42.5*GeV};
  double bin_center = eTHistBins[etbin];
  if (etbin == 4 && et >= 27500.) bin_center = 27500.; // special: interpolate starting from 27.5 here
  if (etbin == 5 && et >= 37500.) bin_center = 37500.; // special: interpolate starting from 37.5 here
  if (et > bin_center){
    double prob_next = prob;
    if (etbin+1<=6) {
      double integral_next = double(fPDFbins[s_or_b][ipbin][etbin+1][etabin][var]->Integral());
      prob_next = double(fPDFbins[s_or_b][ipbin][etbin+1][etabin][var]->GetBinContent(bin)) / integral_next;
      return prob+(prob_next-prob)*(et-bin_center)/(bin_width);
    }
  }
  // or else if et < bin_center :
  double prob_before = prob;
  if (etbin-1>=0) {
    double integral_before = double(fPDFbins[s_or_b][ipbin][etbin-1][etabin][var]->Integral());
    prob_before = double(fPDFbins[s_or_b][ipbin][etbin-1][etabin][var]->GetBinContent(bin)) / integral_before;
  }
  return prob-(prob-prob_before)*(bin_center-et)/(bin_width);
}

//----------------------------------------------------------------------------------------

// These are the variables availalble in the likelihood.
const char* Root::TElectronLikelihoodTool::fVariables[fnVariables] = {"el_d0significance"
							     ,"el_eratio"
							     ,"el_deltaeta1"
							     ,"el_f1"
							     ,"el_f3"
							     ,"el_reta"
							     ,"el_rhad"
							     ,"el_rphi"
							     ,"el_trackd0pvunbiased"
							     ,"el_TRTHighTOutliersRatio"
							     ,"el_weta2"
							     ,"el_DeltaPoverP"
							     ,"el_deltaphiRescaled"
							     ,"el_TRT_PID"};
//,"el_ws3"
//,"el_ptcone20pt"
//,"el_deltaphi2"
//,"el_EoverP"
//,"el_fside"
//,"el_wstot"

//
// Cuts definitions
//
const int Root::TElectronLikelihoodTool::TightCutBL[10]     = {1,1,1,1,1,1,1,1,1,1};
const int Root::TElectronLikelihoodTool::VeryLooseCutPi[10] = {1,1,1,1,1,1,1,1,1,1};
const int Root::TElectronLikelihoodTool::TightCutPi[10]     = {2,2,2,2,2,2,2,2,2,2};
const int Root::TElectronLikelihoodTool::TightCutSi[10]     = {7,7,7,7,7,7,7,7,7,7};

// These are the discriminant values to compare against for various operating points.
// Binning : [mu or nvtx][Et][eta]
// Note that the second bin is an exact copy of the first, since they are one bin in the pdf histograms.

const double Root::TElectronLikelihoodTool::Disc_VeryLoose[90] = { -0.034, -0.034,  0.080, -0.250, -0.472, -0.064, -0.112, -0.052,  0.086, -0.094,
								   0.080,  0.080,  0.104, -0.022, -0.196, -0.046, -0.100,  0.062,  0.158, -0.088,
								   0.164,  0.164,  0.242,  0.140,  0.020, -0.022,  0.044,  0.050,  0.146, -0.076,
								   0.158,  0.158,  0.158,  0.158, -0.016, -0.052,  0.068,  0.134,  0.170, -0.004,
								   0.158,  0.158,  0.182,  0.176,  0.014, -0.022,  0.098,  0.152,  0.164,  0.014,
								   0.080,  0.080,  0.158,  0.056,  0.002, -0.106,  0.032,  0.098,  0.128, -0.022,
								   0.080,  0.080,  0.128,  0.098, -0.052, -0.106,  0.026,  0.098,  0.182,  0.002,
								   0.122,  0.122,  0.164,  0.146,  0.020, -0.178, -0.076,  0.146,  0.224,  0.092,
								   0.086,  0.086,  0.218,  0.080, -0.022, -0.178,  0.044,  0.116,  0.206, -0.010};

const double Root::TElectronLikelihoodTool::Disc_LooseRelaxed[90] = { -0.814, -0.814, -0.322, -0.730, -1.126, -0.202, -0.502, -0.610, -0.490, -1.192,
								      -0.562, -0.562, -0.490, -0.436, -0.538, -0.124, -0.490, -0.496, -0.550, -1.084,
								      -0.532, -0.532, -0.442, -0.454, -0.496, -0.172, -0.454, -0.544, -0.634, -0.820,
								      -0.580, -0.580, -0.556, -0.610, -0.646, -0.340, -0.610, -0.640, -0.622, -0.664,
								      -0.514, -0.514, -0.502, -0.532, -0.562, -0.310, -0.562, -0.610, -0.574, -0.664,
								      -0.526, -0.526, -0.526, -0.538, -0.610, -0.436, -0.580, -0.556, -0.586, -0.820,
								      -0.460, -0.460, -0.454, -0.550, -0.556, -0.382, -0.568, -0.580, -0.484, -0.748,
								      -0.514, -0.514, -0.454, -0.508, -0.646, -0.544, -0.586, -0.574, -0.736, -0.712,
								      -0.364, -0.364, -0.358, -0.448, -0.424, -0.394, -0.544, -0.496, -0.496, -0.628};

const double Root::TElectronLikelihoodTool::Disc_Loose[90] = {  0.100,  0.100,  0.150, -0.045, -0.110,  0.050,  0.036,  0.063,  0.140,  0.030,
								0.240,  0.240,  0.255,  0.190,  0.117,  0.114,  0.097,  0.153,  0.165, -0.037,
								0.278,  0.278,  0.273,  0.254,  0.196,  0.134,  0.181,  0.201,  0.209, -0.017,
								0.280,  0.280,  0.295,  0.234,  0.174,  0.116,  0.196,  0.234,  0.244,  0.067,
								0.305,  0.305,  0.326,  0.280,  0.228,  0.128,  0.234,  0.276,  0.286,  0.108,
								0.336,  0.336,  0.350,  0.311,  0.268,  0.098,  0.252,  0.298,  0.334,  0.090,
								0.382,  0.382,  0.401,  0.368,  0.312,  0.114,  0.276,  0.330,  0.380,  0.089,
								0.371,  0.371,  0.393,  0.327,  0.292,  0.048,  0.234,  0.314,  0.362,  0.218,
								0.428,  0.428,  0.464,  0.408,  0.378,  0.076,  0.304,  0.380,  0.422,  0.238};

const double Root::TElectronLikelihoodTool::Disc_Medium[90] = {  0.301,  0.301,  0.345,  0.368,  0.266,  0.148,  0.319,  0.312,  0.267,  0.123,
								 0.401,  0.401,  0.410,  0.407,  0.396,  0.250,  0.395,  0.426,  0.346,  0.102,
								 0.439,  0.439,  0.466,  0.429,  0.422,  0.255,  0.403,  0.439,  0.341,  0.091,
								 0.499,  0.499,  0.480,  0.475,  0.443,  0.258,  0.458,  0.459,  0.448,  0.158,
								 0.514,  0.514,  0.501,  0.498,  0.466,  0.265,  0.463,  0.475,  0.463,  0.265,
								 0.513,  0.513,  0.494,  0.500,  0.457,  0.270,  0.455,  0.471,  0.481,  0.289,
								 0.535,  0.535,  0.528,  0.533,  0.486,  0.290,  0.467,  0.491,  0.505,  0.361,
								 0.514,  0.514,  0.517,  0.520,  0.454,  0.230,  0.387,  0.464,  0.481,  0.372,
								 0.566,  0.566,  0.581,  0.585,  0.526,  0.243,  0.441,  0.529,  0.524,  0.487};

const double Root::TElectronLikelihoodTool::Disc_Tight[90] = {  0.301,  0.301,  0.371,  0.428,  0.390,  0.269,  0.410,  0.488,  0.401,  0.277,
								0.452,  0.452,  0.410,  0.436,  0.443,  0.271,  0.461,  0.537,  0.442,  0.305,
								0.518,  0.518,  0.522,  0.446,  0.503,  0.255,  0.413,  0.526,  0.464,  0.299,
								0.567,  0.567,  0.516,  0.506,  0.474,  0.258,  0.498,  0.573,  0.448,  0.300,
								0.526,  0.526,  0.526,  0.508,  0.501,  0.265,  0.494,  0.544,  0.464,  0.366,
								0.617,  0.617,  0.547,  0.502,  0.500,  0.285,  0.462,  0.526,  0.481,  0.372,
								0.623,  0.623,  0.639,  0.555,  0.554,  0.290,  0.548,  0.557,  0.505,  0.361,
								0.603,  0.603,  0.564,  0.566,  0.511,  0.232,  0.528,  0.533,  0.507,  0.375,
								0.637,  0.637,  0.641,  0.585,  0.561,  0.243,  0.507,  0.586,  0.524,  0.487};

const double Root::TElectronLikelihoodTool::Disc_VeryTight[90] = {  0.344,  0.344,  0.454,  0.468,  0.453,  0.287,  0.453,  0.545,  0.498,  0.335,
								    0.544,  0.544,  0.494,  0.513,  0.519,  0.325,  0.531,  0.590,  0.506,  0.349,
								    0.573,  0.573,  0.572,  0.565,  0.549,  0.323,  0.516,  0.595,  0.505,  0.408,
								    0.623,  0.623,  0.595,  0.587,  0.561,  0.333,  0.563,  0.612,  0.553,  0.393,
								    0.649,  0.649,  0.619,  0.613,  0.597,  0.337,  0.579,  0.655,  0.577,  0.443,
								    0.690,  0.690,  0.643,  0.647,  0.630,  0.360,  0.588,  0.673,  0.613,  0.476,
								    0.719,  0.719,  0.682,  0.690,  0.672,  0.374,  0.625,  0.698,  0.641,  0.522,
								    0.774,  0.774,  0.702,  0.705,  0.672,  0.351,  0.611,  0.695,  0.631,  0.519,
								    0.809,  0.809,  0.759,  0.757,  0.718,  0.363,  0.663,  0.745,  0.657,  0.582};

//
// Vertex-dependent term d = a + bx where x is number of vertices
//
const double Root::TElectronLikelihoodTool::Disc_Medium_b[90] = { -0.00216, -0.00216, -0.00344, -0.00504, -0.00600, -0.00176, -0.00488, -0.00896, -0.00816, -0.00448,
								  -0.00184, -0.00184,  0.00000, -0.00168, -0.00160, -0.00096, -0.00288, -0.00472, -0.00496, -0.00528,
								  -0.00096, -0.00096, -0.00120, -0.00128, -0.00160, -0.00008, -0.00168, -0.00208, -0.00344, -0.00448,
								  -0.00120, -0.00120, -0.00160, -0.00120, -0.00088, -0.00040, -0.00072, -0.00128, -0.00128, -0.00112,
								  -0.00080, -0.00080, -0.00080, -0.00088, -0.00088, -0.00016, -0.00048, -0.00120, -0.00120, -0.00184,
								  -0.00104, -0.00104, -0.00088, -0.00104, -0.00096, -0.00064, -0.00040, -0.00096, -0.00120, -0.00136,
								  -0.00080, -0.00080, -0.00072, -0.00096, -0.00096, -0.00040, -0.00048, -0.00080, -0.00080, -0.00080,
								  -0.00104, -0.00104, -0.00112, -0.00088, -0.00088, -0.00064, -0.00064, -0.00104, -0.00096, -0.00056,
								  -0.00088, -0.00088, -0.00096, -0.00088, -0.00088, -0.00064, -0.00080, -0.00080, -0.00088, -0.00080};

const double Root::TElectronLikelihoodTool::Disc_Tight_b[90] = { -0.00216, -0.00216, -0.01264, -0.01536, -0.00944, -0.00464, -0.00928, -0.01728, -0.02272, -0.00992,
								 -0.00368, -0.00368,  0.00000, -0.00368, -0.00304, -0.00160, -0.00496, -0.00672, -0.00880, -0.00464,
								 -0.00176, -0.00176, -0.00224, -0.00272, -0.00272, -0.00016, -0.00384, -0.00352, -0.00528, -0.00464,
								 -0.00240, -0.00240, -0.00352, -0.00272, -0.00176, -0.00040, -0.00160, -0.00240, -0.00128, -0.00176,
								 -0.00160, -0.00160, -0.00160, -0.00176, -0.00192, -0.00016, -0.00112, -0.00304, -0.00272, -0.00320,
								 -0.00208, -0.00208, -0.00160, -0.00192, -0.00208, -0.00128, -0.00064, -0.00160, -0.00120, -0.00256,
								 -0.00144, -0.00144, -0.00144, -0.00208, -0.00224, -0.00040, -0.00128, -0.00112, -0.00080, -0.00080,
								 -0.00240, -0.00240, -0.00224, -0.00160, -0.00192, -0.00064, -0.00160, -0.00272, -0.00096, -0.00192,
								 -0.00208, -0.00208, -0.00176, -0.00176, -0.00192, -0.00064, -0.00128, -0.00176, -0.00096, -0.00080};

const double Root::TElectronLikelihoodTool::Disc_VeryTight_b[90] = {  0.00000,  0.00000, -0.00792, -0.00682, -0.00814, -0.00374, -0.01078, -0.01584, -0.02794, -0.01364,
								      -0.00484, -0.00484,  0.00000, -0.00396, -0.00308, -0.00198, -0.00550, -0.00748, -0.00946, -0.00704,
								      -0.00242, -0.00242, -0.00264, -0.00308, -0.00308, -0.00044, -0.00308, -0.00396, -0.00638, -0.00440,
								      -0.00264, -0.00264, -0.00330, -0.00308, -0.00220, -0.00110, -0.00198, -0.00242, -0.00330, -0.00176,
								      -0.00198, -0.00198, -0.00198, -0.00220, -0.00220, -0.00044, -0.00154, -0.00308, -0.00330, -0.00220,
								      -0.00264, -0.00264, -0.00220, -0.00220, -0.00198, -0.00176, -0.00110, -0.00220, -0.00308, -0.00154,
								      -0.00198, -0.00198, -0.00198, -0.00264, -0.00264, -0.00110, -0.00176, -0.00154, -0.00220, -0.00198,
								      -0.00264, -0.00264, -0.00242, -0.00242, -0.00264, -0.00132, -0.00198, -0.00286, -0.00330, -0.00198,
								      -0.00198, -0.00198, -0.00220, -0.00220, -0.00242, -0.00132, -0.00198, -0.00220, -0.00220, -0.00154};

//=============================================================================
// SafeTH1, to allow us to immediately free the ROOT TH1 memory
//=============================================================================

Root::TElectronLikelihoodTool::SafeTH1::SafeTH1(TH1F* roothist){

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

Root::TElectronLikelihoodTool::SafeTH1::~SafeTH1(){
  return;
}

int Root::TElectronLikelihoodTool::SafeTH1::GetNbinsX(){
  int n = m_binContent.size();
  return n;
}

int Root::TElectronLikelihoodTool::SafeTH1::FindBin(double value){

  if(value < m_firstBinLowEdge){
    return 0; // first bin of m_binContent
  }
  if(value > m_lastBinLowEdge){
    return GetNbinsX() - 1; // last bin of m_binContent
  }

  // note double rather than float due to incorrect rounding in O(1/10000) cases if float is used
  double bin_double = (value - m_firstBinLowEdge) / m_binWidth; 
  int bin = static_cast<int>(bin_double);

  return bin;
}

double Root::TElectronLikelihoodTool::SafeTH1::GetBinContent(int bin){
  return m_binContent[bin];
}

double Root::TElectronLikelihoodTool::SafeTH1::GetBinLowEdge(int bin){
  return m_firstBinLowEdge + m_binWidth*bin;
}

double Root::TElectronLikelihoodTool::SafeTH1::Integral(){
  return m_integral;
}
