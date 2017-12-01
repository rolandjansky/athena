/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TElectronLikelihoodTool.h"
#include <cmath>
#include "TSystem.h"
#include "TROOT.h"
#include <stdio.h>                      // for sprintf
#include <algorithm>                    // for min
#include <fstream>                      // for char_traits
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult
#include "TFile.h"                      // for TFile
#include "TH1.h"                        // for TH1F
#include "TString.h"                    // for TString

#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"

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
  HighETBinThreshold(125),
  doPileupTransform(false),
  doCentralityTransform(false),
  DiscMaxForPileupTransform(2.0),
  PileupMaxForPileupTransform(50),
  VariableNames(""),
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
  m_cutPosition_ambiguity(-9),
  m_cutPosition_LH(-9),
  m_cutPositionTrackA0(-9),
  m_cutPositionTrackMatchEta(-9),
  m_cutPositionTrackMatchPhiRes(-9),
  m_cutPositionWstotAtHighET(-9),
  m_cutPositionEoverPAtHighET(-9),
  m_resultPosition_LH(-9)
{
  for(unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < IP_BINS; ip++){
	for(unsigned int et = 0; et < s_fnEtBinsHist; et++){
	  for(unsigned int eta = 0; eta < s_fnEtaBins; eta++){
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
  for(unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++){
    for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
      for (unsigned int ip = 0; ip < IP_BINS; ip++){
	for(unsigned int et = 0; et < s_fnEtBinsHist; et++){
	  for(unsigned int eta = 0; eta < s_fnEtaBins; eta++){
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
  
  unsigned int number_of_expected_bin_combinedLH ;
  if(useHighETLHBinning) number_of_expected_bin_combinedLH =  s_fnDiscEtBins*s_fnEtaBins ;
  else  if(useOneExtraHighETLHBin) number_of_expected_bin_combinedLH =  s_fnDiscEtBinsOneExtra*s_fnEtaBins ;
  else number_of_expected_bin_combinedLH =  s_fnDiscEtBinsOrig*s_fnEtaBins ;
  unsigned int number_of_expected_bin_combinedOther =  s_fnDiscEtBinsOrig*s_fnEtaBins ;


  if( CutLikelihood.size() != number_of_expected_bin_combinedLH){
    ATH_MSG_ERROR("Configuration issue :  CutLikelihood expected size " << number_of_expected_bin_combinedLH << 
		  " input size " << CutLikelihood.size());
    sc = 0;
  } 

  if( DiscHardCutForPileupTransform.size() >0 ) {
    if( DiscHardCutForPileupTransform.size() != number_of_expected_bin_combinedLH){
      ATH_MSG_ERROR("Configuration issue :   DiscHardCutForPileupTransform expected size " << number_of_expected_bin_combinedLH << 
		    " input size " <<  DiscHardCutForPileupTransform.size());
      sc = 0;
    } 
  }
  if(DiscHardCutSlopeForPileupTransform.size() >0 ) {
    if(DiscHardCutSlopeForPileupTransform.size() != number_of_expected_bin_combinedLH){
      ATH_MSG_ERROR("Configuration issue :   DiscHardCutSlopeForPileupTransform expected size " << number_of_expected_bin_combinedLH << 
		    " input size " <<  DiscHardCutSlopeForPileupTransform.size());
      sc = 0;
    } 
  }
  if(DiscLooseForPileupTransform.size() >0) {
    if( DiscLooseForPileupTransform.size() != number_of_expected_bin_combinedLH){
      ATH_MSG_ERROR("Configuration issue :   DiscLooseForPileupTransform expected size " << number_of_expected_bin_combinedLH << 
		    " input size " <<  DiscLooseForPileupTransform.size());
      sc = 0;
    } 
  }

  // d0 cut
  if (CutA0.size() >0){
    if (CutA0.size() != number_of_expected_bin_combinedOther){
      ATH_MSG_ERROR("Configuration issue :   CutA0  expected size " << number_of_expected_bin_combinedOther << 
		    " input size " <<  CutA0.size());
      sc = 0;
    }
  }
    
  // deltaEta cut
  if (CutDeltaEta.size() >0){
    if (CutDeltaEta.size() != number_of_expected_bin_combinedOther){
      ATH_MSG_ERROR("Configuration issue :  CutDeltaEta  expected size " << number_of_expected_bin_combinedOther << 
		    " input size " <<  CutDeltaEta.size());
      sc = 0;
    }
  }
  
  // deltaPhiRes cut
  if (CutDeltaPhiRes.size() >0){
    if (CutDeltaPhiRes.size() != number_of_expected_bin_combinedOther ){
      ATH_MSG_ERROR("Configuration issue :  CutDeltaPhiRes  expected size " << number_of_expected_bin_combinedOther << 
		    " input size " <<  CutDeltaPhiRes.size());
      sc = 0;
    }
  }
  if ( sc == 0 ){
    ATH_MSG_ERROR("Could NOT initialize! Please fix the errors mentioned above...");
    return sc;
  }

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
  // if ( m_cutPosition_nSCTMin < 0 ) sc = 0; // Exceeded the number of allowed cuts (32)
  
  // Cut position for the kineatic pre-selection
  m_cutPosition_kinematic = m_accept.addCut( "kinematic", "pass kinematic" );
  if ( m_cutPosition_kinematic < 0 ) {sc = 0;}

  // NSilicon
  m_cutPosition_NSilicon = m_accept.addCut( "NSilicon", "pass NSilicon" );
  if ( m_cutPosition_NSilicon < 0 ) {sc = 0;}

  // NPixel
  m_cutPosition_NPixel = m_accept.addCut( "NPixel", "pass NPixel" );
  if ( m_cutPosition_NPixel < 0 ) {sc = 0;}

  // NBlayer
  m_cutPosition_NBlayer = m_accept.addCut( "NBlayer", "pass NBlayer" );
  if ( m_cutPosition_NBlayer < 0 ) {sc = 0;}

  // Conversion
  m_cutPosition_conversion = m_accept.addCut( "conversion", "pass conversion" );
  if ( m_cutPosition_conversion < 0 ) {sc = 0;}

 // Ambiguity
  m_cutPosition_ambiguity = m_accept.addCut( "ambiguity", "pass ambiguity" );
  if ( m_cutPosition_ambiguity < 0 ) {sc = 0;}


  // Cut position for the likelihood selection - DO NOT CHANGE ORDER!
  m_cutPosition_LH = m_accept.addCut( "passLH", "pass Likelihood" );
  if ( m_cutPosition_LH < 0 ) {sc = 0;}

  // D0
  m_cutPositionTrackA0 = m_accept.addCut( "TrackA0", "A0 (aka d0) wrt beam spot < Cut" );
  if ( m_cutPositionTrackA0 < 0 ) {sc = 0;}

  // deltaeta
  m_cutPositionTrackMatchEta = m_accept.addCut("TrackMatchEta", "Track match deta in 1st sampling < Cut");
  if ( m_cutPositionTrackMatchEta < 0 ) {sc = 0;}

  // deltaphi
  m_cutPositionTrackMatchPhiRes = m_accept.addCut( "TrackMatchPhiRes", "Track match dphi in 2nd sampling, rescaled < Cut" );
  if ( m_cutPositionTrackMatchPhiRes < 0 ) {sc = 0;}
  
  // Wstot
  m_cutPositionWstotAtHighET = m_accept.addCut( "WstotAtHighET", "Above HighETBinThreshold, Wstot < Cut" );
  if ( m_cutPositionWstotAtHighET < 0 ) {sc = 0;}

  // EoverP
  m_cutPositionEoverPAtHighET = m_accept.addCut( "EoverPAtHighET", "Above HighETBinThreshold, EoverP < Cut" );
  if ( m_cutPositionEoverPAtHighET < 0 ) {sc = 0;}

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
  m_resultPosition_LH = m_result.addResult( (m_resultPrefix+m_resultName).c_str(), "electron likelihood" );
  if ( m_resultPosition_LH < 0 ) sc = 0; // Exceeded the number of allowed results

  // Set the result to a defaul value
  m_result.setResult( m_resultPosition_LH, -9999.0  );

  // Check that we got everything OK
  if ( sc == 0 ){
    ATH_MSG_ERROR("! Something went wrong with the setup of the decision  objects...");
    return 0;
  }

  // ----------------------------------
  // Get the correct bit mask for the current likelihood operating point
  m_variableBitMask = GetLikelihoodBitmask(VariableNames);
  
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
  for(unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++){
    const std::string& vstr = fVariables[varIndex];
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
		<< "\n - Result name                                  : " << (m_resultPrefix+m_resultName).c_str()
		<< "\n - Variable bitmask                             : " << m_variableBitMask);

  ATH_MSG_DEBUG("\n - VariableNames                                : " << VariableNames
		<< "\n - (bool)CutBL (yes/no)                         : " << (CutBL.size() ? "yes" : "no")
		<< "\n - (bool)CutPi (yes/no)                         : " << (CutPi.size() ? "yes" : "no")
		<< "\n - (bool)CutSi (yes/no)                         : " << (CutSi.size() ? "yes" : "no")
		<< "\n - (bool)doCutConversion (yes/no)               : " << (doCutConversion ? "yes" : "no")
		<< "\n - (bool)CutAmbiguity (yes/no)                  : " << (CutAmbiguity.size() ? "yes" : "no")
		<< "\n - (bool)doRemoveF3AtHighEt (yes/no)            : " << (doRemoveF3AtHighEt ? "yes" : "no")
		<< "\n - (bool)doRemoveTRTPIDAtHighEt (yes/no)        : " << (doRemoveTRTPIDAtHighEt ? "yes" : "no")
		<< "\n - (bool)doSmoothBinInterpolation (yes/no)      : " << (doSmoothBinInterpolation ? "yes" : "no")
		<< "\n - (bool)useHighETLHBinning (yes/no)            : " << (useHighETLHBinning ? "yes" : "no")
		<< "\n - (bool)useOneExtraHighETLHBin(yes/no)         : " << (useOneExtraHighETLHBin ? "yes" : "no")
		<< "\n - (double)HighETBinThreshold                   : " << HighETBinThreshold
		<< "\n - (bool)doPileupTransform (yes/no)             : " << (doPileupTransform ? "yes" : "no")
		<< "\n - (bool)doCentralityTransform (yes/no)         : " << (doCentralityTransform ? "yes" : "no")
		<< "\n - (bool)CutLikelihood (yes/no)                 : " << (CutLikelihood.size() ? "yes" : "no")
		<< "\n - (bool)CutLikelihoodPileupCorrection (yes/no) : " << (CutLikelihoodPileupCorrection.size() ? "yes" : "no")
		<< "\n - (bool)CutA0 (yes/no)                         : " << (CutA0.size() ? "yes" : "no")
		<< "\n - (bool)CutDeltaEta (yes/no)                   : " << (CutDeltaEta.size() ? "yes" : "no")
		<< "\n - (bool)CutDeltaPhiRes (yes/no)                : " << (CutDeltaPhiRes.size() ? "yes" : "no")
		<< "\n - (bool)CutWstotAtHighET (yes/no)              : " << (CutWstotAtHighET.size() ? "yes" : "no")
		<< "\n - (bool)CutEoverPAtHighET (yes/no)             : " << (CutEoverPAtHighET.size() ? "yes" : "no")
		);
  return sc;
}

int Root::TElectronLikelihoodTool::LoadVarHistograms(std::string vstr,unsigned int varIndex){
  for(unsigned int s_or_b = 0; s_or_b < 2; s_or_b++){
    for (unsigned int ip = 0; ip < IP_BINS; ip++){
      for(unsigned int et = 0; et < s_fnEtBinsHist; et++){
	for(unsigned int eta = 0; eta < s_fnEtaBins; eta++){
	  
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
	  snprintf(pdfdir,500,"%s/%s",vstr.c_str(),sig_bkg.c_str());
	  char pdf[500];
	  snprintf(pdf,500,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
	  char pdf_newname[500];
	  snprintf(pdf_newname,500,"%s_%s_%s_LHtool_copy_%s",Root::TSelectorToolBase::getName(),vstr.c_str(),sig_bkg.c_str(),binname);

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
          if(!useHighETLHBinning && et > s_fnEtBinsHistOrig-1){
            continue;
          }

          // If the 0th et bin (4-7 GeV) histogram does not exist in the root file,
          // then just use the 7-10 GeV bin histogram. 
          // This should preserve backward compatibility
          if (et == 0 && !((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            //std::cout << "Info: using 7 GeV bin in place of 4 GeV bin." << std::endl;
            getBinName( binname, et_tmp+1, eta_tmp, ip, m_ipBinning );
            snprintf(pdf,500,"%s_%s_smoothed_hist_from_KDE_%s",vstr.c_str(),sig_bkg.c_str(),binname);
            snprintf(pdf_newname,500,"%s_%s_%s_LHtool_copy4GeV_%s",Root::TSelectorToolBase::getName(),vstr.c_str(),sig_bkg.c_str(),binname);
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
                                                            int nSiHitsPlusDeadSensors, int nPixHitsPlusDeadSensors,
                                                            bool passBLayerRequirement,
                                                            int convBit, uint8_t ambiguityBit, double d0, double deltaEta, double deltaphires, 
                                                            double wstot, double EoverP, double ip
                                                            ) const
{
  LikeEnum::LHAcceptVars_t vars;
  
  vars.likelihood              = likelihood;
  vars.eta                     = eta;
  vars.eT                      = eT;
  vars.nSiHitsPlusDeadSensors  = nSiHitsPlusDeadSensors;
  vars.nPixHitsPlusDeadSensors = nPixHitsPlusDeadSensors;
  vars.passBLayerRequirement   = passBLayerRequirement;
  vars.convBit                 = convBit;
  vars.ambiguityBit            = ambiguityBit;
  vars.d0                      = d0;
  vars.deltaEta                = deltaEta;
  vars.deltaphires             = deltaphires;
  vars.wstot                   = wstot;
  vars.EoverP                  = EoverP;
  vars.ip                      = ip;
  
  return accept(vars);
}

// This method calculates if the current electron passes the requested likelihood cut
const Root::TAccept& Root::TElectronLikelihoodTool::accept( LikeEnum::LHAcceptVars_t& vars_struct ) const
{
  // Reset the cut result bits to zero (= fail cut)
  m_accept.clear();

  // Set up the individual cuts
  bool passKine(true);
  bool passNSilicon(true);
  bool passNPixel(true);
  bool passNBlayer(true);
  bool passConversion(true);
  bool passAmbiguity(true);
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
  
  unsigned int etbinLH = getLikelihoodEtDiscBin(vars_struct.eT,true);
  unsigned int etbinOther = getLikelihoodEtDiscBin(vars_struct.eT,false);
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);
  //unsigned int ipbin  = 0;

  // sanity
  if (etbinLH  >= s_fnDiscEtBins) {
    ATH_MSG_WARNING( "Cannot evaluate likelihood for Et " << vars_struct.eT<< ". Returning false..");
    passKine = false;
  }
  // sanity
  if (etbinOther  >= s_fnDiscEtBinsOrig) {
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
  
  // ambiguity bit
  if (CutAmbiguity.size()) {
    if ( !ElectronSelectorHelpers::passAmbiguity((xAOD::AmbiguityTool::AmbiguityType)vars_struct.ambiguityBit,
						CutAmbiguity[etabin])
	 ) {
      ATH_MSG_DEBUG("Likelihood macro: ambiguity Bit Failed." );
      passAmbiguity = false;
    }
  }
  
  // blayer cut
  if (CutBL.size() ) {
    if(CutBL[etabin] == 1 && !vars_struct.passBLayerRequirement) {
      ATH_MSG_DEBUG("Likelihood macro: Blayer cut failed.");
      passNBlayer = false;
    }
  }
  // pixel cut
  if (CutPi.size()){
    if (vars_struct.nPixHitsPlusDeadSensors < CutPi[etabin]){
      ATH_MSG_DEBUG("Likelihood macro: Pixels Failed.");
      passNPixel = false;
    }
  }
  // silicon cut
  if (CutSi.size()){
    if (vars_struct.nSiHitsPlusDeadSensors < CutSi[etabin]){
      ATH_MSG_DEBUG( "Likelihood macro: Silicon Failed.");
      passNSilicon = false;
    }
  }
  
  double cutDiscriminant;
  unsigned int ibin_combinedLH = etbinLH*10+etabin; // Must change if number of eta bins changes!. Also starts from 7-10 GeV bin.
  unsigned int ibin_combinedOther = etbinOther*10+etabin; // Must change if number of eta bins changes!. Also starts from 7-10 GeV bin.

  if(CutLikelihood.size()){
    // To protect against a binning mismatch, which should never happen
    if(ibin_combinedLH>=CutLikelihood.size()){
      ATH_MSG_ERROR("The desired eta/pt bin " << ibin_combinedLH 
		    << " is outside of the range specified by the input" << CutLikelihood.size() << "This should never happen!");
      return m_accept; 
    }

    if (doSmoothBinInterpolation){
      cutDiscriminant = InterpolateCuts(CutLikelihood,CutLikelihood4GeV,vars_struct.eT,vars_struct.eta);
      if (!doPileupTransform && CutLikelihoodPileupCorrection.size() && CutLikelihoodPileupCorrection4GeV.size())
	cutDiscriminant += vars_struct.ip*InterpolateCuts(CutLikelihoodPileupCorrection,CutLikelihoodPileupCorrection4GeV,vars_struct.eT,vars_struct.eta);
    } else {
      if (vars_struct.eT > 7000. || !CutLikelihood4GeV.size()){
	cutDiscriminant = CutLikelihood[ibin_combinedLH];
	// If doPileupTransform, then correct the discriminant itself instead of the cut value
	if (!doPileupTransform && CutLikelihoodPileupCorrection.size()) 
	  cutDiscriminant += vars_struct.ip*CutLikelihoodPileupCorrection[ibin_combinedLH];
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
  }

  // d0 cut
  if (CutA0.size()){
    if (fabs(vars_struct.d0) > CutA0[ibin_combinedOther]){
      ATH_MSG_DEBUG("Likelihood macro: D0 Failed.");
      passTrackA0 = false;
    }
  }

  // deltaEta cut
  if (CutDeltaEta.size()){
    if ( fabs(vars_struct.deltaEta) > CutDeltaEta[ibin_combinedOther]){
      ATH_MSG_DEBUG("Likelihood macro: deltaEta Failed.");
      passDeltaEta = false;
    }
  }
  
  // deltaPhiRes cut
  if (CutDeltaPhiRes.size()){
    if ( fabs(vars_struct.deltaphires) > CutDeltaPhiRes[ibin_combinedOther]){
      ATH_MSG_DEBUG("Likelihood macro: deltaphires Failed.");
      passDeltaPhiRes = false;
    }
  }

  // Only do this above HighETBinThreshold [in GeV]
  if(vars_struct.eT > HighETBinThreshold*1000){
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
  m_accept.setCutResult( m_cutPosition_ambiguity, passAmbiguity );
  m_accept.setCutResult( m_cutPosition_LH, passLH );  
  m_accept.setCutResult( m_cutPositionTrackA0, passTrackA0 );  
  m_accept.setCutResult( m_cutPositionTrackMatchEta, passDeltaEta );  
  m_accept.setCutResult( m_cutPositionTrackMatchPhiRes, passDeltaPhiRes );  
  m_accept.setCutResult( m_cutPositionWstotAtHighET, passWstotAtHighET );  
  m_accept.setCutResult( m_cutPositionEoverPAtHighET, passEoverPAtHighET );  
  return m_accept;
}

const Root::TResult& Root::TElectronLikelihoodTool::calculate( double eta, double eT,double f3, double rHad, double rHad1,
							       double Reta, double w2, double f1, double eratio,
							       double deltaEta, double d0, double d0sigma, double rphi,
							       double deltaPoverP ,double deltaphires, double TRT_PID,
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
		  ,vars_struct.d0,vars_struct.w2
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
  for(unsigned int var = 0; var < s_fnVariables; var++){
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
  
  if (etbin >= s_fnEtBinsHist) {
    ATH_MSG_WARNING("skipping etbin " << etbin << ", et " << et);
    return -999.;
  }
  if (etabin >= s_fnEtaBins) {
    ATH_MSG_WARNING("skipping etabin " << etabin << ", eta " << eta);
    return -999.;
  }
  
  if (varVector.size() != s_fnVariables) 
    ATH_MSG_WARNING("Error! Variable vector size mismatch! Check your vector!" );
  
  double SigmaS = 1.;
  double SigmaB = 1.;

  // define some string constants
  const std::string TRT_string = "TRT";
  const std::string el_f3_string = "el_f3";
  const std::string el_TRT_PID_string = "el_TRT_PID";

  for(unsigned int var = 0; var < s_fnVariables; var++){
    
    const std::string& varstr = fVariables[var];
    
    // Skip variables that are masked off (not used) in the likelihood
    if (!(m_variableBitMask & (0x1 << var))){
      continue;
    }
    // Don't use TRT for outer eta bins (2.01,2.37)
    if (((etabin == 8) || (etabin == 9)) && (varstr.find(TRT_string) != std::string::npos)){
      continue;
    }
    // Don't use f3 for outer eta bin (2.37)
    if ((etabin == 9) && (varstr.find(el_f3_string) != std::string::npos)){
      continue;
    }
    // Don't use f3 for high et (>80 GeV)
    if (doRemoveF3AtHighEt && (et > 80*GeV) && (varstr.find(el_f3_string) != std::string::npos)){
      continue;
    }
    // Don't use TRTPID for high et (>80 GeV)
    if (doRemoveTRTPIDAtHighEt && (et > 80*GeV) && (varstr.find(el_TRT_PID_string) != std::string::npos)){
      continue;
    }
    for (unsigned int s_or_b=0; s_or_b<2;s_or_b++) {
      
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
  // In the HI case, a quadratic centrality transform is applied instead.
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

    if(doCentralityTransform && DiscHardCutQuadForPileupTransform.size() == 0){
      ATH_MSG_WARNING("Vectors needed for centrality-dependent transform not correctly filled! Skipping the transform.");
      return disc;
    }

    unsigned int etabin = getLikelihoodEtaBin(eta);

    double disc_hard_cut_ref       = 0;
    double disc_hard_cut_ref_slope = 0;
    double disc_hard_cut_ref_quad  = 0; // only used for heavy ion implementation of the LH
    double disc_loose_ref          = 0;
    double disc_max                = DiscMaxForPileupTransform;
    double pileup_max              = PileupMaxForPileupTransform;

    if (doSmoothBinInterpolation){
      disc_hard_cut_ref       = InterpolateCuts(DiscHardCutForPileupTransform,DiscHardCutForPileupTransform4GeV,et,eta);
      disc_hard_cut_ref_slope = InterpolateCuts(DiscHardCutSlopeForPileupTransform,DiscHardCutSlopeForPileupTransform4GeV,et,eta);
      if (doCentralityTransform) disc_hard_cut_ref_quad = InterpolateCuts(DiscHardCutQuadForPileupTransform,DiscHardCutQuadForPileupTransform4GeV,et,eta);
      disc_loose_ref          = InterpolateCuts(DiscLooseForPileupTransform,DiscLooseForPileupTransform4GeV,et,eta);
    } else {
      // default situation, in the case where 4-7 GeV bin is not defined
      if (et > 7000. || !DiscHardCutForPileupTransform4GeV.size()){
	unsigned int etfinebinLH = getLikelihoodEtDiscBin(et,true);
	unsigned int ibin_combined = etfinebinLH*10+etabin;
	disc_hard_cut_ref       = DiscHardCutForPileupTransform[ibin_combined];
	disc_hard_cut_ref_slope = DiscHardCutSlopeForPileupTransform[ibin_combined];
	if (doCentralityTransform) disc_hard_cut_ref_quad  = DiscHardCutQuadForPileupTransform[ibin_combined];
	disc_loose_ref          = DiscLooseForPileupTransform[ibin_combined];
      } else {
	if( DiscHardCutForPileupTransform4GeV.size() == 0 || DiscHardCutSlopeForPileupTransform4GeV.size() == 0 || DiscLooseForPileupTransform4GeV.size() == 0){
	  ATH_MSG_WARNING("Vectors needed for pileup-dependent transform not correctly filled for 4-7 GeV bin! Skipping the transform.");
	  return disc;
	}
	if(doCentralityTransform && DiscHardCutQuadForPileupTransform4GeV.size() == 0){
	  ATH_MSG_WARNING("Vectors needed for centrality-dependent transform not correctly filled for 4-7 GeV bin! Skipping the transform.");
	  return disc;
	}
	disc_hard_cut_ref       = DiscHardCutForPileupTransform4GeV[etabin];
	disc_hard_cut_ref_slope = DiscHardCutSlopeForPileupTransform4GeV[etabin];
	if (doCentralityTransform) disc_hard_cut_ref_quad  = DiscHardCutQuadForPileupTransform4GeV[etabin];
	disc_loose_ref          = DiscLooseForPileupTransform4GeV[etabin];
      }
    }

    double ip_for_corr = std::min(ip,pileup_max); // turn off correction for values > pileup_max
    double disc_hard_cut_ref_prime = disc_hard_cut_ref + disc_hard_cut_ref_slope * ip_for_corr + disc_hard_cut_ref_quad * ip_for_corr * ip_for_corr;

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
    const unsigned int nEtBins = s_fnEtBinsHist;
    const double eTBins[nEtBins] = {7*GeV,10*GeV,15*GeV,20*GeV,30*GeV,40*GeV,100*GeV,6000*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
	return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.
  }
  else{
    const unsigned int nEtBins = s_fnEtBinsHistOrig;
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
unsigned int Root::TElectronLikelihoodTool::getLikelihoodEtDiscBin(double eT, const bool isLHbinning) const{
  const double GeV = 1000;

  if(useHighETLHBinning && isLHbinning){
    const unsigned int nEtBins = s_fnDiscEtBins;
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
  else if(useOneExtraHighETLHBin && isLHbinning){
    const unsigned int nEtBins = s_fnDiscEtBinsOneExtra;
    const double eTBins[nEtBins] = {10*GeV,15*GeV,20*GeV,25*GeV,30*GeV,35*GeV,40*GeV,45*GeV,HighETBinThreshold*GeV,6000*GeV};

    for(unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin){
      if(eT < eTBins[eTBin])
	return eTBin;
    }
    
    return nEtBins-1; // Return the last bin if > the last bin.

  }
  else{
    const unsigned int nEtBins = s_fnDiscEtBinsOrig;
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
  int et_bounds[s_fnEtBinsHist] = {4,7,10,15,20,30,40,100};
  if (!iptype.empty()){
    snprintf(buffer, 200,"%s%det%02deta%0.2f", iptype.c_str(), int(fIpBounds[ipbin]), et_bounds[etbin], eta_bounds[etabin]);
  }
  else{ 
    snprintf(buffer, 200,"et%deta%0.2f", et_bounds[etbin], eta_bounds[etabin]);
  }
  return;
}


//----------------------------------------------------------------------------------------
unsigned int Root::TElectronLikelihoodTool::GetLikelihoodBitmask(std::string vars) const{
  unsigned int mask = 0x0;
  ATH_MSG_DEBUG ("Variables to be used: ");
  for(unsigned int var = 0; var < s_fnVariables; var++){
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
  int etbinLH = getLikelihoodEtDiscBin(et,true);
  int etabin = getLikelihoodEtaBin(eta);
  unsigned int ibin_combinedLH = etbinLH*10+etabin;
  double cut = cuts.at(ibin_combinedLH);
  if (cuts_4gev.size() && et < 7000.) {cut = cuts_4gev.at(etabin);}
  if (et > 47500.) {return cut;} // interpolation stops here.
  if (cuts_4gev.size() == 0 && et < 8500.) {return cut;} // stops here
  if (cuts_4gev.size() > 0 && et < 6000.) {return cut;} // stops here
  double bin_width = 5000.;
  if (7000. < et && et < 10000.) {bin_width = 3000.;}
  if (et < 7000.) {bin_width = 2000.;}
  const double GeV = 1000; 
  const double eTBins[9] = {8.5*GeV,12.5*GeV,17.5*GeV,22.5*GeV,27.5*GeV,32.5*GeV,37.5*GeV,42.5*GeV,47.5*GeV};
  double bin_center = eTBins[etbinLH];
  if (et > bin_center) {
    double cut_next = cut;
    if (etbinLH+1<=8) cut_next = cuts.at((etbinLH+1)*10+etabin);
    return cut+(cut_next-cut)*(et-bin_center)/(bin_width);
  }
  // or else if et < bin_center :
  double cut_before = cut;
  if (etbinLH-1>=0) {cut_before = cuts.at((etbinLH-1)*10+etabin);}
  else if (etbinLH == 0 && cuts_4gev.size()){cut_before = cuts_4gev.at(etabin);}

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

  int Nbins      = fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetNbinsX();
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
      // account for potential histogram bin inequalities
      int NbinsPlus  = fPDFbins[s_or_b][ipbin][etbin+1][etabin][var]->GetNbinsX();
      int binplus = bin;
      if (Nbins < NbinsPlus){
	binplus = int(round(bin*(Nbins/NbinsPlus)));
      }
      else if (Nbins > NbinsPlus){
	binplus = int(round(bin*(NbinsPlus/Nbins)));
      }
      // do interpolation
      double integral_next = double(fPDFbins[s_or_b][ipbin][etbin+1][etabin][var]->Integral());
      prob_next = double(fPDFbins[s_or_b][ipbin][etbin+1][etabin][var]->GetBinContent(binplus)) / integral_next;
      return prob+(prob_next-prob)*(et-bin_center)/(bin_width);
    }
  }
  // or else if et < bin_center :
  double prob_before = prob;
  if (etbin-1>=0) {
    // account for potential histogram bin inequalities
    int NbinsMinus = fPDFbins[s_or_b][ipbin][etbin-1][etabin][var]->GetNbinsX();
    int binminus = bin;
    if (Nbins < NbinsMinus){
      binminus = int(round(bin*(Nbins/NbinsMinus)));
    }
    else if (Nbins > NbinsMinus){
      binminus = int(round(bin*(NbinsMinus/Nbins)));
    }
    double integral_before = double(fPDFbins[s_or_b][ipbin][etbin-1][etabin][var]->Integral());
    prob_before = double(fPDFbins[s_or_b][ipbin][etbin-1][etabin][var]->GetBinContent(binminus)) / integral_before;
  }
  return prob-(prob-prob_before)*(bin_center-et)/(bin_width);
}

//----------------------------------------------------------------------------------------

// These are the variables availalble in the likelihood.
const std::string Root::TElectronLikelihoodTool::fVariables[s_fnVariables] = {
  "el_d0significance"
  ,"el_eratio"
  ,"el_deltaeta1"
  ,"el_f1"
  ,"el_f3"
  ,"el_reta"
  ,"el_rhad"
  ,"el_rphi"
  ,"el_trackd0pvunbiased"
  ,"el_weta2"
  ,"el_DeltaPoverP"
  ,"el_deltaphiRescaled"
  ,"el_TRT_PID"
};

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
  int nbins = this->GetNbinsX();
  // since we store the bin content in a vector we need a protection 
  // for cases where we try to access a non-existing bin. In these 
  // cases just go to the last bin
  return (bin>nbins) ? m_binContent[nbins-1] : m_binContent[bin];
}

double Root::TElectronLikelihoodTool::SafeTH1::GetBinLowEdge(int bin){
  return m_firstBinLowEdge + m_binWidth*bin;
}

double Root::TElectronLikelihoodTool::SafeTH1::Integral(){
  return m_integral;
}
