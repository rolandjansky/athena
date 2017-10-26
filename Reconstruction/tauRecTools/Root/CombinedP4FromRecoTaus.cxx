/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s)
#include "PathResolver/PathResolver.h"

//#include "TauAnalysisTools/HelperFunctions.h"

// local include(s)
#include "tauRecTools/CombinedP4FromRecoTaus.h"


//Root includes(s)
#include "TH1F.h"
#include "TH1.h"
#include "TFile.h"
#include "TGraph.h"

//C++ includes
#include <math.h>
#include <string.h>
#include <CxxUtils/make_unique.h>

//_____________________________________________________________________________
CombinedP4FromRecoTaus::CombinedP4FromRecoTaus(const std::string& name) : 
  TauRecToolBase(name),
  // move these to another file? :
  m_weight(-1111.),
  m_combined_res(-1111.),
  m_sigma_tauRec(-1111.),
  m_sigma_constituent(-1111.),
  m_corrcoeff(-1111.),
  m_et_cb2PT_postcalib(0.),
  m_et_postcalib(0.),
  m_et_weighted(0.)
{
  declareProperty( "WeightFileName", m_sWeightFileName = "");
  declareProperty( "addCalibrationResultVariables", m_addCalibrationResultVariables=false);
  declareProperty( "addUseCaloPtFlag", m_addUseCaloPtFlag=false);
  declareProperty( "tauRecEt_takenAs_combinedEt", m_tauRecEt_takenAs_combinedEt=false);
}

//_____________________________________________________________________________
StatusCode CombinedP4FromRecoTaus::initialize() {

  /*m_resHists_tauRec = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );
  m_resHists_CellBased2PanTau = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );   
  m_meanHists_CellBased2PanTau = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );
  m_meanHists_tauRec = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );*/

  m_resTGraph_tauRec = std::vector< std::vector<TGraph*> >(m_etaBinNames.size(), std::vector<TGraph*>(0) );
  m_resTGraph_CellBased2PanTau = std::vector< std::vector<TGraph*> >(m_etaBinNames.size(), std::vector<TGraph*>(0) );   
  m_meanTGraph_CellBased2PanTau = std::vector< std::vector<TGraph*> >(m_etaBinNames.size(), std::vector<TGraph*>(0) );
  m_meanTGraph_tauRec = std::vector< std::vector<TGraph*> >(m_etaBinNames.size(), std::vector<TGraph*>(0) );

  m_correlationHists = std::vector<TH1F*>(0);

    std::string calibFilePath = find_file(m_sWeightFileName);
  TFile * file = TFile::Open(calibFilePath.c_str(), "READ");

  //m_Nsigma_compatibility=5;
  m_Nsigma_compatibility=std::make_unique<TF1>("Nsigma_compatibility", "pol1", 0, 500000); // needs to go beyond ~420 where it crosses y=0
  m_Nsigma_compatibility->SetParameter(0, 3.809); // derived from fit
  m_Nsigma_compatibility->SetParameter(1, -9.58/1000000.); // derived from fit

  TH1F* histogram(0);
  std::string histname="";
  TGraph* Graph(0);
  std::string Graphname="";

  //loop over decay modes
  for(size_t imode=0;imode < m_modeNames.size();imode++){
    
    ATH_MSG_DEBUG("mode = " << imode);

    //Get m_resHists_tauRec
    //histname="ConstituentEt/CorrelationCoeff_ConstituentEt_" + m_modeNames[imode];
    histname="CorrelationCoeff_tauRec_" + m_modeNames[imode];
    histogram = dynamic_cast<TH1F*> (file->Get(histname.c_str()));
    if(histogram){
      m_correlationHists.push_back(histogram);
      ATH_MSG_DEBUG("Adding corr hist: "); 
      //histogram->Print("all"); 
    }
  }


  //loop over eta bins
  for(size_t ietaBin=0;ietaBin < m_etaBinNames.size(); ietaBin++){
  
    //loop over decay modes
    for(size_t imode=0;imode < m_modeNames.size();imode++){

      ATH_MSG_DEBUG("eta bin = " << ietaBin << " / mode = " << imode );
      
      //Get m_resHists_tauRec
      /*histname = "tauRec/ResolutionEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      histogram = dynamic_cast<TH1F*> (file->Get(histname.c_str()));
      if(histogram){
	m_resHists_tauRec[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");*/
      Graphname = "tauRec/Graph_from_ResolutionEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph){
	m_resTGraph_tauRec[ietaBin].push_back(Graph);
	ATH_MSG_DEBUG("Adding graph: ");
	  //histogram->Print("all");
      } else {
       	//ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
       	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
	return StatusCode::FAILURE;
      }

      //Get m_meanHists_tauRec
      /*histname = "tauRec/MeanEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      histogram = dynamic_cast<TH1F*> (file->Get(histname.c_str()));
      if(histogram) {
	m_meanHists_tauRec[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");*/

      Graphname = "tauRec/Graph_from_MeanEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph) {
	m_meanTGraph_tauRec[ietaBin].push_back(Graph);
	ATH_MSG_DEBUG("Adding graph: ");
	  //histogram->Print("all"); 
      } else {
       	//ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
       	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
       	return StatusCode::FAILURE;
      }
      
      //Get m_resHists_CellBased2PanTau
      /*histname = "ConstituentEt/ResolutionEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      histogram = dynamic_cast<TH1F*> (file->Get(histname.c_str()));
      if(histogram){
	m_resHists_CellBased2PanTau[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");*/
      Graphname = "ConstituentEt/Graph_from_ResolutionEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph){
	m_resTGraph_CellBased2PanTau[ietaBin].push_back(Graph);
	ATH_MSG_DEBUG("Adding graph: ");
	//histogram->Print("all"); 
      } else {
	//ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
       	return StatusCode::FAILURE;
      }
      
      //Get m_meanHists_CellBased2PanTau
      /*histname = "ConstituentEt/MeanEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      histogram = dynamic_cast<TH1F*> (file->Get(histname.c_str()));
      if(histogram){
	m_meanHists_CellBased2PanTau[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");*/
      Graphname = "ConstituentEt/Graph_from_MeanEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      Graph = dynamic_cast<TGraph*> (file->Get(Graphname.c_str()));
      if(Graph){
	m_meanTGraph_CellBased2PanTau[ietaBin].push_back(Graph);
	ATH_MSG_DEBUG("Adding graph: ");
	//histogram->Print("all"); 
      } else {
       	//ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
       	ATH_MSG_FATAL("Failed to get an object with name " << Graphname);
       	return StatusCode::FAILURE;
      }
      
    }
    
  }

  return StatusCode::SUCCESS;

}


//_____________________________________________________________________________
StatusCode CombinedP4FromRecoTaus::execute(xAOD::TauJet& xTau) {
  xAOD::TauJet* Tau = &xTau;

  static SG::AuxElement::Decorator<float> decPtCombined("pt_combined");
  static SG::AuxElement::Decorator<float> decEtaCombined("eta_combined");
  static SG::AuxElement::Decorator<float> decPhiCombined("phi_combined");
  static SG::AuxElement::Decorator<float> decMCombined("m_combined");

  decPtCombined(xTau) = 0;
  decEtaCombined(xTau) = 0;
  decPhiCombined(xTau) = 0;
  decMCombined(xTau) = 0;
  
  TLorentzVector CombinedP4(xTau.p4());
  int tmpDecayMode;
  //Needs to be proto, by default PanTau_DecyMode==0 for invalid
  //(pan)taus which is "valid"
  //maybe we should initialize PanTau_DecyMode to NotSet
  //Do we want to apply to Mode_Other? 2,4,5 prongs, I thnk yes
  xTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayModeProto, tmpDecayMode);
  if(tmpDecayMode>=xAOD::TauJetParameters::Mode_1p0n &&
     tmpDecayMode<=xAOD::TauJetParameters::Mode_Other) CombinedP4=getCombinedP4(Tau);

  // create xAOD variables and fill:
  decPtCombined(xTau) = CombinedP4.Pt();
  decEtaCombined(xTau) = CombinedP4.Eta();
  decPhiCombined(xTau) = CombinedP4.Phi();
  decMCombined(xTau) = CombinedP4.M();  


  // move these to another file? :

  /*  m_weight = -1111.;
  m_combined_res = -1111.;
  m_sigma_tauRec = -1111.;
  m_sigma_constituent = -1111.;
  m_corrcoeff = -1111.;
  */

  if (m_addUseCaloPtFlag){
    static SG::AuxElement::Decorator<char> decUseCaloPtFlag("UseCaloPtFlag");
    decUseCaloPtFlag(xTau)  = GetUseCaloPtFlag(Tau);
  }

  if (m_addCalibrationResultVariables){

    static SG::AuxElement::Decorator<float> decPtConstituent("pt_constituent");
    static SG::AuxElement::Decorator<float> decPtTauRecCalibrated("pt_tauRecCalibrated");
    static SG::AuxElement::Decorator<float> decPtWeighted("pt_weighted");
    decPtConstituent(xTau) = m_et_cb2PT_postcalib;
    decPtTauRecCalibrated(xTau) = m_et_postcalib;
    decPtWeighted(xTau) = m_et_weighted;

    static SG::AuxElement::Decorator<float> decWeightWeighted("weight_weighted");
    static SG::AuxElement::Decorator<float> decSigmaCombined("sigma_combined");
    static SG::AuxElement::Decorator<float> decSigmaTaurec("sigma_tauRec");
    static SG::AuxElement::Decorator<float> decSigmaConstituent("sigma_constituent");    
    static SG::AuxElement::Decorator<float> decCorrelationCoefficient("correlation_coefficient");    
    decWeightWeighted(xTau)         = m_weight; 
    decSigmaCombined(xTau)          = m_combined_res;
    decSigmaTaurec(xTau)            = m_sigma_tauRec;
    decSigmaConstituent(xTau)       = m_sigma_constituent;
    decCorrelationCoefficient(xTau) = m_corrcoeff;
  }

  return StatusCode::SUCCESS;

}


int CombinedP4FromRecoTaus::GetIndex_Eta(float eta){
  if( fabs(eta) < 0.3 ) {
    return 0;
  }
  if( fabs(eta) < 0.8 ) {
    return 1;
  }
  if( fabs(eta) < 1.3 ) {
    return 2;
  }
  if( fabs(eta) < 1.6 ) {
    return 3;
  }
  if( fabs(eta) < 2.7 ) {
    return 4;
  }
  
  return 99;
}


double CombinedP4FromRecoTaus::GetCorrelationCoefficient(int etaIndex, xAOD::TauJetParameters::DecayMode mode ){
  
  ATH_MSG_DEBUG("Entering GetCorrelationCoefficient!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }

  return m_correlationHists[mode]->GetBinContent(etaIndex);

} 


double CombinedP4FromRecoTaus::GetWeightedEt(double et_tauRec, 
					     double et_cb2PT,
					     int etaIndex,
					     const xAOD::TauJetParameters::DecayMode& mode){
  ATH_MSG_DEBUG("Entering CombinedP4FromRecoTaus::GetWeightedEt!");

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return et_tauRec;
  }

  float res_tauRec = GetResolution_taurec( et_tauRec, etaIndex, mode );  
  float res_substruct = GetResolution_CellBased2PanTau( et_cb2PT, etaIndex, mode );
  
  if( res_tauRec == 0. || res_substruct == 0. ) {
    ATH_MSG_WARNING( "Warning! res_tauRec or res_substruct is 0!" );
    ATH_MSG_WARNING( "bin_taurec = " << et_tauRec );
    ATH_MSG_WARNING( "bin_substruct = " << et_cb2PT );
    //int mode=GetIndex_Mode(mode);                                                                                                                                                                                                                                           
    //m_resHists_tauRec[etaIndex][mode]->Print("all");
    m_resTGraph_tauRec[etaIndex][mode]->Print("all");
    //m_resHists_CellBased2PanTau[etaIndex][mode]->Print("all");
    m_resTGraph_CellBased2PanTau[etaIndex][mode]->Print("all");
    return 0.;
  }

  //float invres_tauRec=pow(res_tauRec, -2);
  //float invres_substruct=pow(res_substruct, -2);

  float weight=( pow(res_substruct, 2) - GetCorrelationCoefficient(etaIndex, mode )*res_tauRec*res_substruct )
    / ( pow(res_tauRec, 2) + pow(res_substruct, 2) - 2*GetCorrelationCoefficient(etaIndex, mode )*res_tauRec*res_substruct );
  //float weighted_et = ( et_tauRec*invres_tauRec + GetCellbased2PantauEt( et_cb2PT, mode )*invres_substruct ) / ( invres_tauRec + invres_substruct );
  double weighted_et = weight*GetTauRecEt( et_tauRec, etaIndex, mode) + (1 - weight)*GetCellbased2PantauEt( et_cb2PT, etaIndex, mode );

  m_weight = weight;
  m_et_weighted = weighted_et;
  
  return weighted_et;
}


double CombinedP4FromRecoTaus::GetResolution_taurec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
  ATH_MSG_DEBUG("Entering GetResolution_tauRec!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }
  
  //Load file
  double MaxEt = TMath::MaxElement(m_resTGraph_tauRec[etaIndex][mode]->GetN(),m_resTGraph_tauRec[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return m_resTGraph_tauRec[etaIndex][mode]->Eval(MaxEt) * et;
  }
  return m_resTGraph_tauRec[etaIndex][mode]->Eval(et) * et;
}
 

double CombinedP4FromRecoTaus::GetResolution_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
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
 
 
double CombinedP4FromRecoTaus::GetMean_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }

  double MaxEt = TMath::MaxElement(m_meanTGraph_CellBased2PanTau[etaIndex][mode]->GetN(),m_meanTGraph_CellBased2PanTau[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return 0;
  }
  return m_meanTGraph_CellBased2PanTau[etaIndex][mode]->Eval(et) * et;
}
 
 
double CombinedP4FromRecoTaus::GetMean_TauRec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
 
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
 
 
double CombinedP4FromRecoTaus::GetCombinedResolution( double et_tauRec, double et_cb2PT, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }
 
  double sigma_tauRec = GetResolution_taurec( et_tauRec, etaIndex, mode );
  double sigma_cb2PT = GetResolution_CellBased2PanTau( et_cb2PT, etaIndex, mode );
 

  m_sigma_tauRec = sigma_tauRec;
  m_sigma_constituent = sigma_cb2PT;
  m_corrcoeff = GetCorrelationCoefficient(etaIndex, mode );

  double combined_res=sqrt( pow( sigma_tauRec, 2) + pow( sigma_cb2PT, 2) - 2 * GetCorrelationCoefficient(etaIndex, mode ) * sigma_tauRec * sigma_cb2PT );
 
  return combined_res;
 
}


double CombinedP4FromRecoTaus::GetCellbased2PantauEt( double et_cb2PT, int etaIndex, xAOD::TauJetParameters::DecayMode mode ){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    return et_cb2PT;
  }

  m_et_cb2PT_postcalib = et_cb2PT - GetMean_CellBased2PanTau(et_cb2PT,etaIndex, mode);
  return m_et_cb2PT_postcalib;
 
}

double CombinedP4FromRecoTaus::GetTauRecEt( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode ){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    return et;
  }
  
  m_et_postcalib = et - GetMean_TauRec(et, etaIndex, mode);
  return m_et_postcalib;
 
}

double CombinedP4FromRecoTaus::getCombinedEt(double et_tauRec,
					     double et_substructure,
					     float eta,
					     const xAOD::TauJetParameters::DecayMode& mode) {
  ATH_MSG_DEBUG("Entering CombinedP4FromRecoTaus::getCombinedEt");

  int etaIndex = GetIndex_Eta(eta);
  ATH_MSG_DEBUG("Eta = " << eta << " , eta bin = " << etaIndex );

  double et_reco = GetWeightedEt( et_tauRec, et_substructure, etaIndex, mode );
  ATH_MSG_DEBUG( "GetWeightedEt: " << et_reco );
  double combined_res = GetCombinedResolution( et_tauRec, et_substructure, etaIndex, mode );
  ATH_MSG_DEBUG( "Combined_resolution: " << combined_res );
  ATH_MSG_DEBUG( GetNsigma_Compatibility(et_tauRec) << "*Combined_resolution: " << GetNsigma_Compatibility(et_tauRec)*combined_res);
  double et_diff = GetTauRecEt( et_tauRec, etaIndex, mode) - GetCellbased2PantauEt( et_substructure, etaIndex, mode );
  ATH_MSG_DEBUG( "et_diff (GetTauRecEt - GetCellb2PEt): " << et_diff );

  m_combined_res = combined_res;

  if( fabs( et_diff ) > GetNsigma_Compatibility(et_tauRec)*combined_res) {
    et_reco = et_tauRec;
    m_tauRecEt_takenAs_combinedEt = true;
    ATH_MSG_DEBUG( "(Boolean)m_tauRecEt_takenAs_combinedEt is set to:"  <<  m_tauRecEt_takenAs_combinedEt );
  }
  return et_reco;
}


TLorentzVector CombinedP4FromRecoTaus::getCombinedP4(const xAOD::TauJet* tau) {
  ATH_MSG_DEBUG( "In CombinedP4FromRecoTaus::getCombinedP4..." );

  m_tauRecEt_takenAs_combinedEt=false;

  ATH_MSG_DEBUG( "(Boolean)m_tauRecEt_takenAs_combinedEt is initialized to: " << m_tauRecEt_takenAs_combinedEt );

  TLorentzVector tauRecP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  TLorentzVector substructureP4;

  substructureP4.SetPtEtaPhiM(tau->ptPanTauCellBased(), tau->etaPanTauCellBased(), tau->phiPanTauCellBased(), tau->mPanTauCellBased());

  ATH_MSG_DEBUG( "TauRecET: " << tauRecP4.Et() );
  ATH_MSG_DEBUG( "ConstituentET: " << substructureP4.Et() );

  xAOD::TauJetParameters::DecayMode decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, tmpDecayMode)) {
    decayMode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }

  //Return tauRec P4 if tau is no pantau candidate or pantau decay mode is unequal 1P or 3P
  int isPanTauCandidate;  
  tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_isPanTauCandidate, isPanTauCandidate);  
  int DecayMode;
  tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, DecayMode);

  ATH_MSG_DEBUG( "tau IsPanTauCandidate = " << isPanTauCandidate );

  if (isPanTauCandidate == 0 || DecayMode>xAOD::TauJetParameters::Mode_3pXn || fabs(tauRecP4.Eta()) > 2.5) {
    m_et_cb2PT_postcalib = substructureP4.Et();
    m_et_postcalib = tauRecP4.Et();
    m_et_weighted = tauRecP4.Et();
    return tauRecP4;
  }

  double combinedEt = getCombinedEt(tauRecP4.Et(), substructureP4.Et(), tauRecP4.Eta(), decayMode);
  ATH_MSG_DEBUG( "combinedET: " << combinedEt );


  TLorentzVector combinedP4;
  // double combinedM = tauRecP4.M();
  // //  if(tauRec momentum is NOT taken as combined){
  // if(m_tauRecEt_takenAs_combinedEt == false){
  //   combinedM = substructureP4.M();    
  // }
  double combinedM = 0;

  ATH_MSG_DEBUG( "combinedM: " << combinedM );

  double combinedPt = sqrt(pow(combinedEt,2) - pow(combinedM,2));
  ATH_MSG_DEBUG( "combinedPt: " << combinedPt );

  combinedP4.SetPtEtaPhiM(combinedPt, substructureP4.Eta(), substructureP4.Phi(), combinedM);

  return combinedP4;
}


//_____________________________________________________________________________
float CombinedP4FromRecoTaus::GetNsigma_Compatibility(float et_TauRec){

    float nsigma=m_Nsigma_compatibility->Eval(et_TauRec);

    if(nsigma<0) return 0.;
    return nsigma;

  }


//_____________________________________________________________________________
double CombinedP4FromRecoTaus::GetCaloResolution(const xAOD::TauJet* tau){
  ATH_MSG_DEBUG("Entering GetCaloResolution!");

  TLorentzVector tauRecP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  double et = tauRecP4.Et();
  float eta = tauRecP4.Eta();
  int etaIndex = GetIndex_Eta(eta);

  xAOD::TauJetParameters::DecayMode mode = xAOD::TauJetParameters::DecayMode::Mode_Error;

  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, tmpDecayMode)) {
    mode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_DEBUG("PanTau decay mode > 3PXN! Return false");
    return false;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_DEBUG("eta > 2.7. Return false");
    return false;
  }

  double MaxEt = TMath::MaxElement(m_resTGraph_tauRec[etaIndex][mode]->GetN(),m_resTGraph_tauRec[etaIndex][mode]->GetX()); 
  if (et > MaxEt){
    return m_resTGraph_tauRec[etaIndex][mode]->Eval(MaxEt) * et;
  }
  return m_resTGraph_tauRec[etaIndex][mode]->Eval(et) * et;
}

//_____________________________________________________________________________
bool CombinedP4FromRecoTaus::GetUseCaloPtFlag(const xAOD::TauJet* tau){
  ATH_MSG_DEBUG("Entering GetUseCaloPtFlag!");
  
  TLorentzVector tauRecP4;
  TLorentzVector tauMVATESP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  tauMVATESP4.SetPtEtaPhiM(tau->ptFinalCalib(), tau->etaFinalCalib(), tau->phiFinalCalib(), tau->mFinalCalib());
  double et_tauRec = tauRecP4.Et();
  double et_MVATES = tauMVATESP4.Et();

  float eta = tauRecP4.Eta();
  int etaIndex = GetIndex_Eta(eta);

  xAOD::TauJetParameters::DecayMode mode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, tmpDecayMode)) {
    mode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_DEBUG("PanTau decay mode > 3PXN! Return false");
    return false;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_DEBUG("eta > 2.7. Return false");
    return false;
  }
  
  double tauRec_res = GetCaloResolution(tau);
  double et_diff = et_MVATES - et_tauRec;
  
  bool UseCaloPt = false;
  if( et_diff > 5*tauRec_res) {
    UseCaloPt = true;
  }
  
  return UseCaloPt;
}
