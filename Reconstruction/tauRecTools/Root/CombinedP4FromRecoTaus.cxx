/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/CombinedP4FromRecoTaus.h"

#include "TFile.h"

#include <cmath>



CombinedP4FromRecoTaus::CombinedP4FromRecoTaus(const std::string& name) : 
  TauRecToolBase(name) {
  declareProperty("addCalibrationResultVariables", m_addCalibrationResultVariables = false);
  declareProperty("WeightFileName", m_calFileName = "");
}



StatusCode CombinedP4FromRecoTaus::initialize() {
  // It would be better to retrieve the function from calibration file 
  m_nSigmaCompatibility=std::make_unique<TF1>("nSigmaCompatibility", "pol1", 0, 500000);
  m_nSigmaCompatibility->SetParameter(0, 3.809); // derived from fit
  m_nSigmaCompatibility->SetParameter(1, -9.58/1000000.); // derived from fit

  std::string calFilePath = find_file(m_calFileName);
  std::unique_ptr<TFile> calFile(TFile::Open(calFilePath.c_str(), "READ"));

  // 1D array with decay mode as index
  TH1F* hist = nullptr;
  std::string histName = "";
  for (size_t decayModeIndex = 0; decayModeIndex < DecayModeBinning; ++ decayModeIndex) {
    histName="CorrelationCoeff_tauRec_" + m_decayModeNames[decayModeIndex];
    hist = dynamic_cast<TH1F*> (calFile->Get(histName.c_str()));
    if(hist){
      hist->SetDirectory(0);
      m_correlationHists[decayModeIndex] = std::unique_ptr<TH1F>(hist);
      ATH_MSG_DEBUG("Adding corr hist: " << histName);
    }
    else {
      ATH_MSG_FATAL("Failed to get an object with name " << histName);
      return StatusCode::FAILURE;
    }
  }
  
  // 2D array with (eta, decay mode) as index
  TGraph* graph = nullptr;
  std::string graphName="";
  for (size_t decayModeIndex = 0; decayModeIndex < DecayModeBinning; ++decayModeIndex) {
    for (size_t etaIndex = 0; etaIndex < EtaBinning; ++etaIndex) {
      // Calo TES: relative bias 
      graphName = "tauRec/Graph_from_MeanEt_tauRec_" + m_decayModeNames[decayModeIndex] + "_" + m_etaBinNames[etaIndex];
      graph = dynamic_cast<TGraph*> (calFile->Get(graphName.c_str()));
      if(graph) {
        m_caloRelBiasMaxEt[decayModeIndex][etaIndex] = TMath::MaxElement(graph->GetN(), graph->GetX());
        m_caloRelBias[decayModeIndex][etaIndex] = std::unique_ptr<TGraph>(graph);
        ATH_MSG_DEBUG("Adding graph: " << graphName);
      } 
      else {
       	ATH_MSG_FATAL("Failed to get an object with name " << graphName);
       	return StatusCode::FAILURE;
      }
      
      // Calo TES: resolution
      graphName = "tauRec/Graph_from_ResolutionEt_tauRec_" + m_decayModeNames[decayModeIndex] + "_" + m_etaBinNames[etaIndex];
      graph = dynamic_cast<TGraph*> (calFile->Get(graphName.c_str()));
      if(graph){
        m_caloResMaxEt[decayModeIndex][etaIndex] = TMath::MaxElement(graph->GetN(), graph->GetX());
        m_caloRes[decayModeIndex][etaIndex] = std::unique_ptr<TGraph>(graph);
        ATH_MSG_DEBUG("Adding graph: " << graphName);
      } 
      else {
       	ATH_MSG_FATAL("Failed to get an object with name " << graphName);
        return StatusCode::FAILURE;
      }
     
      // PanTau: relative bias 
      graphName = "ConstituentEt/Graph_from_MeanEt_ConstituentEt_" + m_decayModeNames[decayModeIndex] + "_" + m_etaBinNames[etaIndex];
      graph = dynamic_cast<TGraph*> (calFile->Get(graphName.c_str()));
      if(graph){
        m_panTauRelBiasMaxEt[decayModeIndex][etaIndex] = TMath::MaxElement(graph->GetN(), graph->GetX());
        m_panTauRelBias[decayModeIndex][etaIndex] = std::unique_ptr<TGraph>(graph);
        ATH_MSG_DEBUG("Adding graph: " << graphName);
      } 
      else {
       	ATH_MSG_FATAL("Failed to get an object with name " << graphName);
       	return StatusCode::FAILURE;
      }

      // PanTau: resolution
      graphName = "ConstituentEt/Graph_from_ResolutionEt_ConstituentEt_" + m_decayModeNames[decayModeIndex] + "_" + m_etaBinNames[etaIndex];
      graph = dynamic_cast<TGraph*> (calFile->Get(graphName.c_str()));
      if(graph){
        m_panTauResMaxEt[decayModeIndex][etaIndex] = TMath::MaxElement(graph->GetN(), graph->GetX());
        m_panTauRes[decayModeIndex][etaIndex] = std::unique_ptr<TGraph>(graph);
        ATH_MSG_DEBUG("Adding graph: " << graphName);
      } 
      else {
        ATH_MSG_FATAL("Failed to get an object with name " << graphName);
       	return StatusCode::FAILURE;
      }
    }
  }
  calFile->Close();

  return StatusCode::SUCCESS;
}



StatusCode CombinedP4FromRecoTaus::execute(xAOD::TauJet& tau) const {
  TLorentzVector combinedP4(tau.p4(xAOD::TauJetParameters::TauEtaCalib));
  
  // used to store immediate results
  Variables variables;

  // Parameterization is only valid for |eta| < 2.5, and decay modes of 1p0n, 1p1n, 1pXn, 3p0n, 3pXn
  // If these variables of the given tau candidate are outside the range, we just use calo TES
  if(isValid(tau)) {
    combinedP4 = getCombinedP4(tau, variables);
  }
  
  static const SG::AuxElement::Decorator<float> decPtCombined("pt_combined");
  static const SG::AuxElement::Decorator<float> decEtaCombined("eta_combined");
  static const SG::AuxElement::Decorator<float> decPhiCombined("phi_combined");
  static const SG::AuxElement::Decorator<float> decMCombined("m_combined");

  decPtCombined(tau) = combinedP4.Pt();
  decEtaCombined(tau) = combinedP4.Eta();
  decPhiCombined(tau) = combinedP4.Phi();
  decMCombined(tau) = combinedP4.M();  

  if (m_addCalibrationResultVariables){
    static const SG::AuxElement::Decorator<float> decPtConstituent("pt_constituent");
    static const SG::AuxElement::Decorator<float> decPtTauRecCalibrated("pt_tauRecCalibrated");
    static const SG::AuxElement::Decorator<float> decPtWeighted("pt_weighted");
    static const SG::AuxElement::Decorator<float> decWeightWeighted("weight_weighted");
    static const SG::AuxElement::Decorator<float> decSigmaCombined("sigma_combined");
    static const SG::AuxElement::Decorator<float> decSigmaTaurec("sigma_tauRec");
    static const SG::AuxElement::Decorator<float> decSigmaConstituent("sigma_constituent");    
    static const SG::AuxElement::Decorator<float> decCorrelationCoefficient("correlation_coefficient");    
    
    decPtConstituent(tau) = variables.pt_constituent;
    decPtTauRecCalibrated(tau) = variables.pt_tauRecCalibrated;
    decPtWeighted(tau) = variables.pt_weighted;
    decWeightWeighted(tau) = variables.weight; 
    decSigmaCombined(tau) = variables.sigma_combined;
    decSigmaTaurec(tau) = variables.sigma_tauRec;
    decSigmaConstituent(tau) = variables.sigma_constituent;
    decCorrelationCoefficient(tau) = variables.corrcoeff;
  }

  return StatusCode::SUCCESS;
}



bool CombinedP4FromRecoTaus::getUseCaloPtFlag(const xAOD::TauJet& tau) const {
  if (! isValid(tau)) return true;
  
  xAOD::TauJetParameters::DecayMode decayMode = getDecayMode(tau);  
  int decayModeIndex = getDecayModeIndex(decayMode);

  int etaIndex = getEtaIndex(tau.etaTauEtaCalib());
  
  double caloSigma = tau.ptTauEtaCalib() * getCaloResolution(tau.ptTauEtaCalib(), decayModeIndex, etaIndex);
  double deltaEt = tau.ptFinalCalib() - tau.ptTauEtaCalib();
  
  bool useCaloPt = false;
  
  // FIXME: should we use combinedSigma here ??
  if (std::abs(deltaEt) > 5 * caloSigma) {
    useCaloPt = true;
  }
  
  return useCaloPt;
}



int CombinedP4FromRecoTaus::getEtaIndex(const float& eta) const {
  // It would be better to retrieve eta bins from the calibration file, e.g. for upgrade studies!
  if (std::abs(eta) < 0.3) {
    return 0;
  }
  if (std::abs(eta) < 0.8) {
    return 1;
  }
  if (std::abs(eta) < 1.3) {
    return 2;
  }
  if (std::abs(eta) < 1.6) {
    return 3;
  }
  if (std::abs(eta) < 2.5) {
    return 4;
  }

  return 99;
}



xAOD::TauJetParameters::DecayMode CombinedP4FromRecoTaus::getDecayMode(const xAOD::TauJet& tau) const{
  int decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  
  // When PanTau fails, the decay mode will be set to 1p0n !
  int isPanTauCandidate;
  tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_isPanTauCandidate, isPanTauCandidate);
  if (isPanTauCandidate) {
    tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, decayMode);
  }
 
  return static_cast<xAOD::TauJetParameters::DecayMode>(decayMode);
} 



int CombinedP4FromRecoTaus::getDecayModeIndex(const xAOD::TauJetParameters::DecayMode& decayMode) const {
  return static_cast<int>(decayMode);
}



bool CombinedP4FromRecoTaus::isValid(const xAOD::TauJet& tau) const {
  xAOD::TauJetParameters::DecayMode decayMode = getDecayMode(tau); 
  if (decayMode < xAOD::TauJetParameters::Mode_1p0n || decayMode > xAOD::TauJetParameters::Mode_3pXn) {
    ATH_MSG_DEBUG("Decay mode is not supported !");
    return false;
  }

  // FIXME: At which calibration state ???
  int etaIndex = getEtaIndex(tau.eta()); 
  if (etaIndex > 4) {
    ATH_MSG_DEBUG("Eta is out of the supported range !");
    return false;
  }

  return true;
}



double CombinedP4FromRecoTaus::getCorrelation(const int& decayModeIndex, const int& etaIndex) const {
  return m_correlationHists[decayModeIndex]->GetBinContent(etaIndex);
} 



double CombinedP4FromRecoTaus::getCaloCalEt(const double& caloEt,
                                            const int& decayModeIndex,
                                            const int& etaIndex) const {
  // ratio stored in the calibration graph equals (caloEt-truthEt)/caloEt
  double ratio = 0.0;
  
  // FIXME: If caloEt is larger then max et, could we use the ratio at 
  // max et, instead of setting it to zero
  if (caloEt <= m_caloRelBiasMaxEt[decayModeIndex][etaIndex]) {
    ratio = m_caloRelBias[decayModeIndex][etaIndex]->Eval(caloEt);
  }
  
  double caloCalEt = caloEt - ratio * caloEt;

  return caloCalEt;
}



double CombinedP4FromRecoTaus::getPanTauCalEt(const double& panTauEt,
                                                  const int& decayModeIndex,
                                                  const int& etaIndex) const {
  // ratio stored in the calibration graph equals (panTauEt-truthEt)/panTauEt
  double ratio = 0.0;
 
  // Substructure is bad determined at high pt, as track momentum is pooryly measured 
  if (panTauEt <= m_panTauRelBiasMaxEt[decayModeIndex][etaIndex]) {
    ratio = m_panTauRelBias[decayModeIndex][etaIndex]->Eval(panTauEt);
  }

  double panTauCalEt = panTauEt - ratio * panTauEt;

  return panTauCalEt;
}



double CombinedP4FromRecoTaus::getCaloResolution(const xAOD::TauJet& tau) const {
  // Assume the resolution to be 100% when no calibraction is available
  if (! isValid(tau)) return 1.0;
  
  xAOD::TauJetParameters::DecayMode decayMode = getDecayMode(tau);  
  int decayModeIndex = getDecayModeIndex(decayMode);

  // FIXME: At which calibration state ???
  int etaIndex = getEtaIndex(tau.eta());
  
  // FIXME: At which calibration state ???
  return getCaloResolution(tau.pt(), etaIndex, decayModeIndex);
}



double CombinedP4FromRecoTaus::getCaloResolution(const double& et, const int& decayModeIndex, const int& etaIndex) const {
  double x = std::min(et, m_caloResMaxEt[decayModeIndex][etaIndex]);
  double resolution = m_caloRes[decayModeIndex][etaIndex]->Eval(x);
 
  return resolution;
}



double CombinedP4FromRecoTaus::getPanTauResolution(const double& et, const int& decayModeIndex, const int& etaIndex) const {
  double x = std::min(et, m_panTauResMaxEt[decayModeIndex][etaIndex]);
  double resolution = m_panTauRes[decayModeIndex][etaIndex]->Eval(x);
 
  return resolution;
}



double CombinedP4FromRecoTaus::getWeight(const double& caloSigma,
                                         const double& panTauSigma,
                                         const double& correlation) const {
  double cov = correlation * caloSigma * panTauSigma;
  double caloWeight = std::pow(panTauSigma, 2) - cov;
  double panTauWeight = std::pow(caloSigma, 2) - cov;

  return caloWeight/(caloWeight + panTauWeight);
}



double CombinedP4FromRecoTaus::getCombinedSigma(const double& caloSigma,
					                            const double& panTauSigma,
                                                const double& correlation) const {
  // FIXME: the calculation seems not right
  double combinedSigma2 = std::pow(caloSigma, 2) + std::pow(panTauSigma, 2) 
                          - 2 * correlation * caloSigma * panTauSigma;
  
  return std::sqrt(combinedSigma2);
}



double CombinedP4FromRecoTaus::getNsigmaCompatibility(const double& et) const {
  double nsigma = m_nSigmaCompatibility->Eval(et);
  
  if (nsigma < 0.) return 0.;

  return nsigma;
}



double CombinedP4FromRecoTaus::getCombinedEt(const double& caloEt,
					                         const double& panTauEt,
					                         const xAOD::TauJetParameters::DecayMode& decayMode,
					                         const float& eta,
                                             Variables& variables) const {
  // Obtain the index of calibration graph
  int decayModeIndex = getDecayModeIndex(decayMode);
  int etaIndex = getEtaIndex(eta);
  
  // Obtain the calibration parameter based on the index
  // -- Correlation between calo TES and PanTau
  double correlation = getCorrelation(decayModeIndex, etaIndex);

  // -- Sigma of the difference between reconstruted et and truth et at calo TES
  double caloSigma = caloEt * getCaloResolution(caloEt, decayModeIndex, etaIndex);
  if (0. == caloSigma) {
    ATH_MSG_WARNING("Calo TES: Et resolution at " << caloEt << " is 0");
    m_caloRes[decayModeIndex][etaIndex]->Print("all");
    return 0.;
  }
  
  // -- Sigma of the difference between reconstruted et and truth et at PanTau
  double panTauSigma = panTauEt * getPanTauResolution(panTauEt, decayModeIndex, etaIndex);
  if (0. == panTauSigma) {
    ATH_MSG_WARNING("PanTau: Et resolution at " << panTauEt << " is 0");
    m_panTauRes[decayModeIndex][etaIndex]->Print("all");
    return 0.;
  }
 
  // -- Et at calo TES with bias corrected
  double caloCalEt = getCaloCalEt(caloEt, decayModeIndex, etaIndex); 
  
  // -- Et at PanTau with bias corrected
  double panTauCalEt = getPanTauCalEt(panTauEt, decayModeIndex, etaIndex);
 
  // Combination of calo TES and PanTau
  // FIXME: A more consistent way would be calculating the weight use bias corrected Et as input
  double weight = getWeight(caloSigma, panTauSigma, correlation);
  double weightedEt = weight * caloCalEt + (1 - weight) * panTauCalEt;
  double combinedSigma = getCombinedSigma(caloSigma, panTauSigma, correlation);
  
  // FIXME: weighteEt will be updated in case the difference of calo TES and PanTau is too large
  variables.pt_weighted = weightedEt;
  
  // If the difference of calo TES and PanTau is too large, the combined result
  // may not be reliable 
  // FIXME: A more consistent way would be calculating the NsigmaCompatibility use caloCalEt
  double deltaEt = caloCalEt - panTauCalEt;
  if (std::abs(deltaEt) > getNsigmaCompatibility(caloEt) * combinedSigma) {
    // FIXME: Why not use caloCalEt here ?
    weightedEt = caloEt;
  }

  // Store the results
  variables.corrcoeff = correlation;
  variables.sigma_tauRec = caloSigma;
  variables.sigma_constituent = panTauSigma;
  variables.pt_tauRecCalibrated = caloCalEt;
  variables.pt_constituent = panTauCalEt;
  variables.weight = weight;
  variables.sigma_combined = combinedSigma;

  ATH_MSG_DEBUG("Intermediate results\n" << 
                "coff: " << correlation << " sigma(calo): " << caloSigma << " sigma(constituent): " << panTauSigma << 
                "\ncalibrated et(calo): " << caloCalEt << " calibrated et(constituent): " << panTauCalEt << 
                "\nweight:" << weight << " combined et: " << weightedEt << " combined sigma: " << combinedSigma);
  
  return weightedEt;
}



TLorentzVector CombinedP4FromRecoTaus::getCombinedP4(const xAOD::TauJet& tau, Variables& variables) const {
  const TLorentzVector& caloP4 = tau.p4(xAOD::TauJetParameters::TauEtaCalib);
  const TLorentzVector& panTauP4 = tau.p4(xAOD::TauJetParameters::PanTauCellBased);
    
  ATH_MSG_DEBUG("Four momentum at calo TES, pt: " << caloP4.Pt() << " eta: " << caloP4.Eta() << 
                " phi: " << caloP4.Phi() << " mass: " << caloP4.M());
  ATH_MSG_DEBUG("Four momentum at PanTau, pt: " << panTauP4.Pt() << " eta: " << panTauP4.Eta() << 
                " phi: " << panTauP4.Phi() << " mass: " << panTauP4.M());

  xAOD::TauJetParameters::DecayMode decayMode = getDecayMode(tau);
  
  double combinedEt = getCombinedEt(caloP4.Et(), panTauP4.Et(), decayMode, caloP4.Eta(), variables);

  // Et is the combination of calo TES and PanTau, but eta and phi is from PanTau 
  TLorentzVector combinedP4;
  combinedP4.SetPtEtaPhiM(combinedEt, panTauP4.Eta(), panTauP4.Phi(), 0.);
  
  ATH_MSG_DEBUG("Combined four momentum, pt: " << combinedP4.Pt() << " eta: " << combinedP4.Eta() << 
                " phi: " << combinedP4.Phi() << " mass: " << combinedP4.M());

  return combinedP4;
}
