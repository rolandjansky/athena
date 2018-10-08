/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPERFORMANCEFUNCTIONS_CXX
#define MUONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "PathResolver/PathResolver.h"
#include "TSystem.h"

namespace Upgrade {

float UpgradePerformanceFunctions::getMuonEfficiency(float ptMeV, float eta, float phi){

  if (!m_muonEff && !setupMuonEffProvider()){
    ATH_MSG_ERROR("FAILED to setup the muon efficiency provider. Returning default values from older tool version.");
    return getMuonEfficiencyFallback(ptMeV, eta);
  }
  return m_muonEff->getEfficiency(ptMeV,eta,phi);
}

float UpgradePerformanceFunctions::getMuonEfficiencyFallback(float ptMeV, float eta) {
  // Returns the muon efficiency for the specified eta region. 
  // For 'tight=true' this corresponds to 0.97 everywhere, except the acceptance gap at eta~0
  // For 'tight=false' (i.e. loose muons) is 0.99 everywhere, except the acceptance gap at eta~0*/
  //M.Corradi: NEW VERSION FROM 30/7/2013
  // using 2012 values from M. Goblirsch
  // NOTE: This has now been replaced by more finely granular efficiency maps provided via a wrapper tool.
  // This method with fixed efficiency values is kept as a fallback for cases where the fine maps are not available.

    if (ptMeV < 4000.|| fabs(eta)>4.0) return 0;

    double max_eta =2.7;
    if  (m_layout == gold || m_layout == LoIVF || m_layout == InclBrl || m_layout == ExtBrl || m_layout ==  Step1p6 ) max_eta=4.0;
    
    // large eta extension
    if (fabs(eta)>2.7){
        if (m_layout == gold || m_layout == LoIVF  || m_layout == InclBrl || m_layout == ExtBrl || m_layout ==  Step1p6)
        {
            if (m_muonCutLevel == looseMuon){ // large-eta extension
                return 0.99;
            } else {
                return 0.95;
            }
        }
        return 0;
    }
    
    if (m_muonCutLevel == looseMuon) {  // loose selection : CB + ST + calo tagged for |eta|<0.1 + SA for 2.5<|eta|<2.7
      return 0.99; 
    } else if (m_muonCutLevel == tightMuon || m_muonCutLevel == highPtMuon) { // CB "tight" selection
        if (fabs(eta)<0.1) {
            return 0.54;
        } else if (fabs(eta)<max_eta) {
            return 0.97;
        } else {
            return 0;
        }
    }
    ATH_MSG_ERROR("getMuonEfficiency: Invalid working point.");
    return 0.;
}
float UpgradePerformanceFunctions::getMuonMSPtResolution(float ptMeV, float eta, float phi) {
  // All of the coefficients expect pT in GeV, so we convert the input and output
  float ptGeV = ptMeV/1000.;

  if (!m_muonRes && !setupMuonResProvider()){
      ATH_MSG_ERROR("FAILED to setup muon pt resolution provider. Returning absurd values");
      return -1000*ptMeV;
  }
  double sigma_ms = ptGeV * m_muonRes->getExpMsResolution(ptGeV,eta,phi);
  return sigma_ms*1000.; // back to MeV
}

float UpgradePerformanceFunctions::getMuonIDPtResolution(float ptMeV, float eta, float phi) {
  // All of the coefficients expect pT in GeV, so we convert the input and output
  float ptGeV = ptMeV/1000.;

  if (!m_muonRes && !setupMuonResProvider()){
      ATH_MSG_ERROR("FAILED to setup muon pt resolution provider. Returning absurd values");
      return -1000*ptMeV;
  }
  double sigma_itk = ptGeV * m_muonRes->getExpItkResolution(ptGeV,eta,phi);
  return sigma_itk*1000.; // back to MeV
}

float UpgradePerformanceFunctions::getMuonPtResolution(float ptMeV, float eta, float phi) {
  
  // resolution components //
  double sigma_ID, sigma_MS, sigma_CB;
    
  ///////////////////////////////////////
  // CALCULATE THE MOMENTUM RESOLUTION //
  ///////////////////////////////////////

  sigma_MS = getMuonMSPtResolution(ptMeV, eta, phi);
  sigma_ID = getMuonIDPtResolution(ptMeV, eta, phi);
  // replaced by custom MCP values for now while waiting for official upgrade tracking numbers
  // sigma_ID = getTrackPtResolution(ptMeV, eta);

  if (fabs(eta)>2.7 && !m_useWarmToroid) { // large eta tagger momentum measurement comes from ITK
      return sigma_ID;
  }
  if (sigma_ID > 0.) {
    sigma_CB = sigma_ID*sigma_MS/sqrt(sigma_ID*sigma_ID+sigma_MS*sigma_MS);
  } else {
    sigma_CB = sigma_MS;
  }
  return sigma_CB; // back to MeV
}

float  UpgradePerformanceFunctions::getMuonQOverPtResolution(float ptMeV, float eta, float phi){
    // This is resolution of q/p_T :
    return getMuonPtResolution(ptMeV, eta, phi)/ptMeV/ptMeV;
}

float  UpgradePerformanceFunctions::getMuonMSQOverPtResolution(float ptMeV, float eta, float phi){
    // This is resolution of q/p_T :
    return getMuonMSPtResolution(ptMeV, eta, phi)/ptMeV/ptMeV;
}
float  UpgradePerformanceFunctions::getMuonIDQOverPtResolution(float ptMeV, float eta, float phi){
    // This is resolution of q/p_T :
    return getMuonIDPtResolution(ptMeV, eta, phi)/ptMeV/ptMeV;
}


bool UpgradePerformanceFunctions::setupMuonResProvider(){
    m_muonRes =  std::unique_ptr<MuonMomentumResProvider>(new MuonMomentumResProvider("MuonMomentumResProvider"));
    if (m_muonCutLevel == highPtMuon){
      m_muonEnergyResolutionFilename = "MuonMomentumResolutions_Step1p9_171002-HighPt.dat";
    }
    if (m_muonCutLevel == highPtMuon && m_useWarmToroid ){
      ATH_MSG_ERROR("Warm toroid smearing for High pT muons missing! ");
      return false;  
    }
    if(m_muonRes->setProperty("UpgradeResolutionRelease", m_muonEnergyResolutionFilename).isFailure()  
       || m_muonRes->initialize()!=StatusCode::SUCCESS){
      ATH_MSG_ERROR("Failed to set up MuonMomentumResProvider! ");
      return false;  
    }
    return true;
}

bool UpgradePerformanceFunctions::setupMuonEffProvider(){
    std::string inputfile = "";
    if (m_muonCutLevel == looseMuon){
      inputfile = "UpgradePerformanceFunctions/CalibArea-00-01/Muon_Eff_Loose.root";
    }
    else if (m_muonCutLevel == tightMuon){
      inputfile = "UpgradePerformanceFunctions/CalibArea-00-01/Muon_Eff_Tight.root";
    }
    else if (m_muonCutLevel == highPtMuon){
      inputfile = "UpgradePerformanceFunctions/CalibArea-00-01/Muon_Eff_HighPt.root";
    }
    else {
      ATH_MSG_WARNING("Unable to identify the configured muon working point. Will provide TIGHT efficiencies");
      inputfile = "UpgradePerformanceFunctions/CalibArea-00-01/Muon_Eff_Tight.root";
    }
    std::string inputPath = PathResolverFindCalibFile( inputfile );
    const std::string inDir = gSystem->DirName(inputPath.c_str());
    const std::string inFile = gSystem->BaseName(inputPath.c_str());
    m_muonEff = std::unique_ptr<MuonEffProvider>(new MuonEffProvider("UpgradeMuonEfficiencyProvider"));
    if (!m_muonEff->initialize(inDir,inFile, m_useMuonHighEta)){
      ATH_MSG_ERROR("Trouble initializing the efficiency provider for our muons ");
      return false;
    }
    return true;
}

}

#endif
