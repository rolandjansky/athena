/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/CombinedP4FromRecoTaus.h"

//Root includes(s)
#include "TFile.h"

//C++ includes
#include <cmath>
#include <string.h>

//_____________________________________________________________________________
CombinedP4FromRecoTaus::CombinedP4FromRecoTaus(const std::string& name) : 
  TauRecToolBase(name) {

  declareProperty("addCalibrationResultVariables", m_addCalibrationResultVariables = false);
  declareProperty("addUseCaloPtFlag", m_addUseCaloPtFlag = false);
  declareProperty("WeightFileName", m_sWeightFileName = "");
}

//_____________________________________________________________________________
StatusCode CombinedP4FromRecoTaus::initialize() {
  
  m_correlationHists.resize(m_modeNames.size());  
  m_meanTGraph_CellBased2PanTau.resize(m_etaBinNames.size());
  m_resTGraph_CellBased2PanTau.resize(m_etaBinNames.size());
  m_meanTGraph_tauRec.resize(m_etaBinNames.size());
  m_resTGraph_tauRec.resize(m_etaBinNames.size());

  for (size_t i=0; i<m_etaBinNames.size(); ++i) {
    m_meanTGraph_CellBased2PanTau[i].resize(m_modeNames.size());
    m_resTGraph_CellBased2PanTau[i].resize(m_modeNames.size());
    m_meanTGraph_tauRec[i].resize(m_modeNames.size());
    m_resTGraph_tauRec[i].resize(m_modeNames.size());
  }

  std::string calibFilePath = find_file(m_sWeightFileName);
  std::unique_ptr<TFile> file(TFile::Open(calibFilePath.c_str(), "READ"));

  m_Nsigma_compatibility=std::make_unique<TF1>("Nsigma_compatibility", "pol1", 0, 500000); // needs to go beyond ~420 where it crosses y=0
  m_Nsigma_compatibility->SetParameter(0, 3.809); // derived from fit
  m_Nsigma_compatibility->SetParameter(1, -9.58/1000000.); // derived from fit

  //retrieve correlation histgram
  TH1F* histogram(nullptr);
  std::string histname="";
  
  //loop over decay modes
  for(size_t imode=0; imode < m_modeNames.size(); ++imode){
    
    ATH_MSG_DEBUG("mode = " << imode);

    histname="CorrelationCoeff_tauRec_" + m_modeNames[imode];
    histogram = dynamic_cast<TH1F*> (file->Get(histname.c_str()));
    if(histogram){
      histogram->SetDirectory(0);
      m_correlationHists[imode] = std::unique_ptr<TH1F>(histogram);
      ATH_MSG_DEBUG("Adding corr hist: " << histname);
    }
    else {
      ATH_MSG_FATAL("Failed to get an object with name " << histname);
      return StatusCode::FAILURE;
    }
  }

  //retrieve mean and resolution graph
  TGraph* Graph(nullptr);
  std::string Graphname="";

  //loop over eta bins
  for(size_t ietaBin=0; ietaBin < m_etaBinNames.size(); ++ietaBin){
  
    //loop over decay modes
    for(size_t imode=0; imode < m_modeNames.size(); ++imode){

      ATH_MSG_DEBUG("eta bin = " << ietaBin << " / mode = " << imode );
      
      // retrieve resolution graph 
      Graphname = "tauRec/Graph_from_ResolutionEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph){
        m_resTGraph_tauRec[ietaBin][imode] = std::unique_ptr<TGraph>(Graph);
        ATH_MSG_DEBUG("Adding graph: " << Graphname);
      } 
      else {
       	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
        return StatusCode::FAILURE;
      }

      // retrieve mean graph
      Graphname = "tauRec/Graph_from_MeanEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph) {
        m_meanTGraph_tauRec[ietaBin][imode] = std::unique_ptr<TGraph>(Graph);
        ATH_MSG_DEBUG("Adding graph: " << Graphname);
      } 
      else {
       	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
       	return StatusCode::FAILURE;
      }
      
      Graphname = "ConstituentEt/Graph_from_ResolutionEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph){
        m_resTGraph_CellBased2PanTau[ietaBin][imode] = std::unique_ptr<TGraph>(Graph);
        ATH_MSG_DEBUG("Adding graph: " << Graphname);
      } 
      else {
        ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
       	return StatusCode::FAILURE;
      }
      
      Graphname = "ConstituentEt/Graph_from_MeanEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph){
        m_meanTGraph_CellBased2PanTau[ietaBin][imode] = std::unique_ptr<TGraph>(Graph);
        ATH_MSG_DEBUG("Adding graph: " << Graphname);
      } 
      else {
       	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
       	return StatusCode::FAILURE;
      }
    }
  }
  file->Close();

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode CombinedP4FromRecoTaus::execute(xAOD::TauJet& xTau) const {

  Variables variables;

  static const SG::AuxElement::Decorator<float> decPtCombined("pt_combined");
  static const SG::AuxElement::Decorator<float> decEtaCombined("eta_combined");
  static const SG::AuxElement::Decorator<float> decPhiCombined("phi_combined");
  static const SG::AuxElement::Decorator<float> decMCombined("m_combined");

  decPtCombined(xTau) = 0.;
  decEtaCombined(xTau) = 0.;
  decPhiCombined(xTau) = 0.;
  decMCombined(xTau) = 0.;
  
  TLorentzVector CombinedP4(xTau.p4());
  int tmpDecayMode;
  //Needs to be proto, by default PanTau_DecyMode==0 for invalid
  //(pan)taus which is "valid"
  //maybe we should initialize PanTau_DecyMode to NotSet
  //Do we want to apply to Mode_Other? 2,4,5 prongs, I think yes
  xTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayModeProto, tmpDecayMode);
  if(tmpDecayMode>=xAOD::TauJetParameters::Mode_1p0n && tmpDecayMode<=xAOD::TauJetParameters::Mode_Other) CombinedP4=getCombinedP4(xTau, variables);

  // create xAOD variables and fill:
  decPtCombined(xTau) = CombinedP4.Pt();
  decEtaCombined(xTau) = CombinedP4.Eta();
  decPhiCombined(xTau) = CombinedP4.Phi();
  decMCombined(xTau) = CombinedP4.M();  

  if (m_addUseCaloPtFlag){
    static const SG::AuxElement::Decorator<char> decUseCaloPtFlag("UseCaloPtFlag");
    decUseCaloPtFlag(xTau) = getUseCaloPtFlag(xTau);
  }

  if (m_addCalibrationResultVariables){

    static const SG::AuxElement::Decorator<float> decPtConstituent("pt_constituent");
    static const SG::AuxElement::Decorator<float> decPtTauRecCalibrated("pt_tauRecCalibrated");
    static const SG::AuxElement::Decorator<float> decPtWeighted("pt_weighted");
    decPtConstituent(xTau) = variables.et_cb2PT_postcalib;
    decPtTauRecCalibrated(xTau) = variables.et_postcalib;
    decPtWeighted(xTau) = variables.et_weighted;

    static const SG::AuxElement::Decorator<float> decWeightWeighted("weight_weighted");
    static const SG::AuxElement::Decorator<float> decSigmaCombined("sigma_combined");
    static const SG::AuxElement::Decorator<float> decSigmaTaurec("sigma_tauRec");
    static const SG::AuxElement::Decorator<float> decSigmaConstituent("sigma_constituent");    
    static const SG::AuxElement::Decorator<float> decCorrelationCoefficient("correlation_coefficient");    
    decWeightWeighted(xTau)         = variables.weight; 
    decSigmaCombined(xTau)          = variables.combined_res;
    decSigmaTaurec(xTau)            = variables.sigma_tauRec;
    decSigmaConstituent(xTau)       = variables.sigma_constituent;
    decCorrelationCoefficient(xTau) = variables.corrcoeff;
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
// it would be better to retrieve eta bins from the calibration file, e.g. for upgrade studies!
int CombinedP4FromRecoTaus::getIndexEta(const float& eta) const {

  if( std::abs(eta) < 0.3 ) {
    return 0;
  }
  if( std::abs(eta) < 0.8 ) {
    return 1;
  }
  if( std::abs(eta) < 1.3 ) {
    return 2;
  }
  if( std::abs(eta) < 1.6 ) {
    return 3;
  }
  if( std::abs(eta) < 2.7 ) {
    return 4;
  }

  return 99;
}

//_____________________________________________________________________________
double CombinedP4FromRecoTaus::getCorrelationCoefficient(const int& etaIndex, const xAOD::TauJetParameters::DecayMode& mode) const {
  
  ATH_MSG_DEBUG("Entering getCorrelationCoefficient!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }

  return m_correlationHists[mode]->GetBinContent(etaIndex);
} 

//_____________________________________________________________________________
double CombinedP4FromRecoTaus::getWeightedEt(const double& et_tauRec, 
					     const double& et_cb2PT,
					     const int& etaIndex,
					     const xAOD::TauJetParameters::DecayMode& mode,
                                             Variables& variables) const {
  ATH_MSG_DEBUG("Entering CombinedP4FromRecoTaus::getWeightedEt!");

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return et_tauRec;
  }

  float res_tauRec = getResolutionTaurec( et_tauRec, etaIndex, mode );  
  float res_substruct = getResolutionCellBased2PanTau( et_cb2PT, etaIndex, mode );
  
  if( res_tauRec == 0. || res_substruct == 0. ) {
    ATH_MSG_WARNING( "Warning! res_tauRec or res_substruct is 0!" );
    ATH_MSG_WARNING( "bin_taurec = " << et_tauRec );
    ATH_MSG_WARNING( "bin_substruct = " << et_cb2PT );
    m_resTGraph_tauRec[etaIndex][mode]->Print("all");
    m_resTGraph_CellBased2PanTau[etaIndex][mode]->Print("all");
    return 0.;
  }

  variables.weight=( std::pow(res_substruct, 2) - getCorrelationCoefficient(etaIndex, mode )*res_tauRec*res_substruct )
    / ( std::pow(res_tauRec, 2) + std::pow(res_substruct, 2) - 2*getCorrelationCoefficient(etaIndex, mode )*res_tauRec*res_substruct );
  variables.et_weighted = variables.weight*getTauRecEt( et_tauRec, etaIndex, mode, variables.et_postcalib ) + (1 - variables.weight)*getCellbased2PantauEt( et_cb2PT, etaIndex, mode, variables.et_cb2PT_postcalib );

  return variables.et_weighted;
}

//_____________________________________________________________________________
double CombinedP4FromRecoTaus::getResolutionTaurec(const double& et, const int& etaIndex, const xAOD::TauJetParameters::DecayMode& mode) const {
  ATH_MSG_DEBUG("Entering GetResolution_tauRec!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }
  
  double MaxEt = TMath::MaxElement(m_resTGraph_tauRec[etaIndex][mode]->GetN(),m_resTGraph_tauRec[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return m_resTGraph_tauRec[etaIndex][mode]->Eval(MaxEt) * et;
  }

  return m_resTGraph_tauRec[etaIndex][mode]->Eval(et) * et;
}

//_____________________________________________________________________________
double CombinedP4FromRecoTaus::getResolutionCellBased2PanTau(const double& et, const int& etaIndex, const xAOD::TauJetParameters::DecayMode& mode) const {
  ATH_MSG_DEBUG("Entering GetResolution_CellBased2Pantau!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }
  
  double MaxEt = TMath::MaxElement(m_resTGraph_CellBased2PanTau[etaIndex][mode]->GetN(),m_resTGraph_CellBased2PanTau[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return m_resTGraph_CellBased2PanTau[etaIndex][mode]->Eval(MaxEt) * et;
  }

  return m_resTGraph_CellBased2PanTau[etaIndex][mode]->Eval(et) * et;
}
 
//_____________________________________________________________________________ 
double CombinedP4FromRecoTaus::getMeanCellBased2PanTau(const double& et, const int& etaIndex, const xAOD::TauJetParameters::DecayMode& mode) const {
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }

  double MaxEt = TMath::MaxElement(m_meanTGraph_CellBased2PanTau[etaIndex][mode]->GetN(),m_meanTGraph_CellBased2PanTau[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return 0.;
  }

  return m_meanTGraph_CellBased2PanTau[etaIndex][mode]->Eval(et) * et;
}
 
//_____________________________________________________________________________ 
double CombinedP4FromRecoTaus::getMeanTauRec(const double& et, const int& etaIndex, const xAOD::TauJetParameters::DecayMode& mode) const {
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }
    
  double MaxEt = TMath::MaxElement(m_meanTGraph_tauRec[etaIndex][mode]->GetN(),m_meanTGraph_tauRec[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return 0;
  }

  return m_meanTGraph_tauRec[etaIndex][mode]->Eval(et) * et;
}

//_____________________________________________________________________________ 
double CombinedP4FromRecoTaus::getCombinedResolution(const double& et_tauRec,
                                                     const double& et_cb2PT,
                                                     const int& etaIndex,
                                                     const xAOD::TauJetParameters::DecayMode& mode,
                                                     Variables& variables) const {
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }
 
  double sigma_tauRec = getResolutionTaurec( et_tauRec, etaIndex, mode );
  double sigma_cb2PT = getResolutionCellBased2PanTau( et_cb2PT, etaIndex, mode ); 

  variables.sigma_tauRec = sigma_tauRec;
  variables.sigma_constituent = sigma_cb2PT;
  variables.corrcoeff = getCorrelationCoefficient(etaIndex, mode );

  double combined_res = std::sqrt( std::pow( variables.sigma_tauRec, 2) 
				   + std::pow( sigma_cb2PT, 2) 
				   - 2 * getCorrelationCoefficient(etaIndex, mode ) * variables.sigma_tauRec * sigma_cb2PT );
 
  return combined_res;
}

//_____________________________________________________________________________
// WARNING! likely unexpected behaviour for et_cb2PT_postcalib in case of non-supported DecayMode, FIXME
double CombinedP4FromRecoTaus::getCellbased2PantauEt(const double& et_cb2PT,
                                                     const int& etaIndex,
                                                     const xAOD::TauJetParameters::DecayMode& mode,
                                                     double& et_cb2PT_postcalib) const {
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    return et_cb2PT;
  }

  et_cb2PT_postcalib = et_cb2PT - getMeanCellBased2PanTau(et_cb2PT,etaIndex, mode);

  return et_cb2PT_postcalib; 
}

//_____________________________________________________________________________
// WARNING! likely unexpected behaviour for et_cb2PT_postcalib in case of non-supported DecayMode, FIXME
double CombinedP4FromRecoTaus::getTauRecEt(const double& et,
                                           const int& etaIndex,
                                           const xAOD::TauJetParameters::DecayMode& mode, 
                                           double& et_postcalib) const {
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    return et;
  }
  
  et_postcalib = et - getMeanTauRec(et, etaIndex, mode);

  return et_postcalib; 
}

//_____________________________________________________________________________
double CombinedP4FromRecoTaus::getCombinedEt(const double& et_tauRec,
					     const double& et_substructure,
					     const float& eta,
					     const xAOD::TauJetParameters::DecayMode& mode,
                                             Variables& variables) const {
  ATH_MSG_DEBUG("Entering CombinedP4FromRecoTaus::getCombinedEt");

  int etaIndex = getIndexEta(eta);
  ATH_MSG_DEBUG("Eta = " << eta << " , eta bin = " << etaIndex );

  double et_reco = getWeightedEt( et_tauRec, et_substructure, etaIndex, mode, variables);
  ATH_MSG_DEBUG( "getWeightedEt: " << et_reco );
  variables.combined_res = getCombinedResolution( et_tauRec, et_substructure, etaIndex, mode, variables );
  ATH_MSG_DEBUG( "Combined_resolution: " << variables.combined_res );
  ATH_MSG_DEBUG( getNsigmaCompatibility(et_tauRec) << "*Combined_resolution: " << getNsigmaCompatibility(et_tauRec)*variables.combined_res);
  double et_diff = getTauRecEt( et_tauRec, etaIndex, mode, variables.et_postcalib ) - getCellbased2PantauEt( et_substructure, etaIndex, mode, variables.et_cb2PT_postcalib );
  ATH_MSG_DEBUG( "et_diff (getTauRecEt - GetCellb2PEt): " << et_diff );

  if( std::abs( et_diff ) > getNsigmaCompatibility(et_tauRec)*variables.combined_res) {
    et_reco = et_tauRec;
  }

  return et_reco;
}

//_____________________________________________________________________________
TLorentzVector CombinedP4FromRecoTaus::getCombinedP4(const xAOD::TauJet& tau, Variables& variables) const {
  ATH_MSG_DEBUG( "In CombinedP4FromRecoTaus::getCombinedP4..." );

  const TLorentzVector& tauRecP4 = tau.p4();
  const TLorentzVector& substructureP4 = tau.p4(xAOD::TauJetParameters::PanTauCellBased);
    
  ATH_MSG_DEBUG( "TauRecET: " << tauRecP4.Et() );
  ATH_MSG_DEBUG( "ConstituentET: " << substructureP4.Et() );

  xAOD::TauJetParameters::DecayMode decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, tmpDecayMode)) {
    decayMode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }

  //Return tauRec P4 if tau is no pantau candidate or pantau decay mode is unequal 1P or 3P
  int isPanTauCandidate;  
  tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_isPanTauCandidate, isPanTauCandidate);  
  int DecayMode;
  tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, DecayMode);

  ATH_MSG_DEBUG( "tau IsPanTauCandidate = " << isPanTauCandidate );

  if (isPanTauCandidate == 0 || DecayMode>xAOD::TauJetParameters::Mode_3pXn || std::abs(tauRecP4.Eta()) > 2.5) {
    variables.et_cb2PT_postcalib = substructureP4.Et();
    variables.et_postcalib = tauRecP4.Et();
    variables.et_weighted = tauRecP4.Et();
    return tauRecP4;
  }

  double combinedEt = getCombinedEt(tauRecP4.Et(), substructureP4.Et(), tauRecP4.Eta(), decayMode, variables);
  ATH_MSG_DEBUG( "combinedET: " << combinedEt );

  TLorentzVector combinedP4;
  
  //double combinedM = 0;
  //double combinedPt = std::sqrt(std::pow(combinedEt,2) - std::pow(combinedM,2));
  //combinedP4.SetPtEtaPhiM(combinedPt, substructureP4.Eta(), substructureP4.Phi(), combinedM);

  combinedP4.SetPtEtaPhiM(combinedEt, substructureP4.Eta(), substructureP4.Phi(), 0.);

  return combinedP4;
}

//_____________________________________________________________________________
float CombinedP4FromRecoTaus::getNsigmaCompatibility(const float& et_TauRec) const {

  float nsigma = m_Nsigma_compatibility->Eval(et_TauRec);

  if(nsigma<0) return 0.;

  return nsigma;
}

//_____________________________________________________________________________
double CombinedP4FromRecoTaus::getCaloResolution(const xAOD::TauJet& tau) const {
  ATH_MSG_DEBUG("Entering getCaloResolution!");

  double et = tau.pt();

  xAOD::TauJetParameters::DecayMode mode = xAOD::TauJetParameters::DecayMode::Mode_Error;

  int tmpDecayMode;
  if (tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, tmpDecayMode)) {
    mode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_DEBUG("PanTau decay mode > 3PXN! Return false");
    // need to derive 2p parametrisation, undefined behaviour, for now return Et
    return et;
  }

  int etaIndex = getIndexEta(tau.eta());
  if( etaIndex > 4 ){
    ATH_MSG_DEBUG("eta > 2.7. Return false");
    // not upgrade-friendly, undefined behaviour, for now return Et
    return et;
  }

  double MaxEt = TMath::MaxElement(m_resTGraph_tauRec[etaIndex][mode]->GetN(),m_resTGraph_tauRec[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return m_resTGraph_tauRec[etaIndex][mode]->Eval(MaxEt) * et;
  }

  return m_resTGraph_tauRec[etaIndex][mode]->Eval(et) * et;
}

//_____________________________________________________________________________
bool CombinedP4FromRecoTaus::getUseCaloPtFlag(const xAOD::TauJet& tau) const {
  ATH_MSG_DEBUG("Entering getUseCaloPtFlag!");
  
  xAOD::TauJetParameters::DecayMode mode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, tmpDecayMode)) {
    mode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_DEBUG("PanTau decay mode > 3PXN! Return false");
    return false;
  }

  int etaIndex = getIndexEta(tau.eta()); 
  if( etaIndex > 4 ){
    ATH_MSG_DEBUG("eta > 2.7. Return false");
    return false;
  }
  
  double tauRec_res = getCaloResolution(tau);
  double et_diff = tau.ptFinalCalib() - tau.pt();
  
  bool UseCaloPt = false;
  if( et_diff > 5*tauRec_res) {
    UseCaloPt = true;
  }
  
  return UseCaloPt;
}
