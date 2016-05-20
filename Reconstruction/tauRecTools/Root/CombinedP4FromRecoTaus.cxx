/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s)
#include "PathResolver/PathResolver.h"

#include "TauAnalysisTools/HelperFunctions.h"

// local include(s)
#include "tauRecTools/CombinedP4FromRecoTaus.h"

//Root includes(s)
#include "TH1F.h"
#include "TH1.h"
#include "TFile.h"

//C++ includes
#include <math.h>
#include <string.h>

//_____________________________________________________________________________
CombinedP4FromRecoTaus::CombinedP4FromRecoTaus(const std::string& name) : 
  TauRecToolBase(name)
{
  declareProperty( "WeightFileName", m_sWeightFileName = "");
  declareProperty( "addCalibrationResultVariables", m_addCalibrationResultVariables=false);
  declareProperty( "tauRecEt_takenAs_combinedEt", m_tauRecEt_takenAs_combinedEt=false);
}

//_____________________________________________________________________________
StatusCode CombinedP4FromRecoTaus::initialize() {

  m_resHists_tauRec = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );
  m_resHists_CellBased2PanTau = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );   
  m_meanHists_CellBased2PanTau = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );
  m_meanHists_tauRec = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );
  //m_correlationHists = std::vector< std::vector<TH1F*> >(m_etaBinNames.size(), std::vector<TH1F*>(0) );
  m_correlationHists = std::vector<TH1F*>(0);

  
  std::string calibFilePath = find_file(m_sWeightFileName);
  TFile * file = TFile::Open(calibFilePath.c_str(), "READ");

  TH1F* histogram;
  TObject* obj;
  std::string histname="";


  //loop over decay modes
  for(int imode=0;imode < abs(m_modeNames.size());imode++){
    
    ATH_MSG_DEBUG("mode = " << imode);

    //Get m_resHists_tauRec
    //histname="ConstituentEt/CorrelationCoeff_ConstituentEt_" + m_modeNames[imode];
    histname="CorrelationCoeff_tauRec_" + m_modeNames[imode];
    obj = file->Get(histname.c_str());
    if(obj) histogram = dynamic_cast<TH1F*>(obj);
    if(histogram){
      m_correlationHists.push_back(histogram);
      ATH_MSG_DEBUG("Adding corr hist: "); 
      //histogram->Print("all"); 
    }

  }

      
  //loop over eta bins
  for(int ietaBin=0;ietaBin < abs(m_etaBinNames.size()); ietaBin++){
  
    //loop over decay modes
    for(int imode=0;imode < abs(m_modeNames.size());imode++){

      ATH_MSG_DEBUG("eta bin = " << ietaBin << " / mode = " << imode );
      
      //Get m_resHists_tauRec
      histname = "tauRec/ResolutionEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      obj = file->Get(histname.c_str());
      if(obj) histogram = dynamic_cast<TH1F*>(obj);
      if(histogram){
	m_resHists_tauRec[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");
	//histogram->Print("all");
      } else {
       	ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
	return StatusCode::FAILURE;
      }

      //Get m_meanHists_tauRec
      histname = "tauRec/MeanEt_tauRec_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      obj = file->Get(histname.c_str());
      if(obj) histogram = dynamic_cast<TH1F*>(obj);
      if(histogram) {
	m_meanHists_tauRec[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");
	//histogram->Print("all"); 
      } else {
       	ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
       	return StatusCode::FAILURE;
      }
      
      //Get m_resHists_CellBased2PanTau
      histname = "ConstituentEt/ResolutionEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      obj = file->Get(histname.c_str());
      if(obj) histogram = dynamic_cast<TH1F*>(obj);
      if(histogram){
	m_resHists_CellBased2PanTau[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");
	//histogram->Print("all"); 
      } else {
	ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
       	return StatusCode::FAILURE;
      }
      
      //Get m_meanHists_CellBased2PanTau
      histname = "ConstituentEt/MeanEt_ConstituentEt_" + m_modeNames[imode] + "_" + m_etaBinNames[ietaBin];
      obj = file->Get(histname.c_str());
      if(obj) histogram = dynamic_cast<TH1F*>(obj);
      if(histogram){
	m_meanHists_CellBased2PanTau[ietaBin].push_back(histogram);
	ATH_MSG_DEBUG("Adding hist: ");
	//histogram->Print("all"); 
      } else {
       	ATH_MSG_FATAL("Failed to get an object with  histname " << histname);
       	return StatusCode::FAILURE;
      }
      
    }
    
  }

  return StatusCode::SUCCESS;

}


//_____________________________________________________________________________
StatusCode CombinedP4FromRecoTaus::execute(xAOD::TauJet& xTau) {
  xAOD::TauJet* Tau = &xTau;
  int tmpDecayModeProto;
  xTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_DecayModeProto, tmpDecayModeProto);
  if(tmpDecayModeProto>xAOD::TauJetParameters::Mode_3pXn){
    xTau.auxdecor<float>("pt_combined")  = 1.;    
    return StatusCode::SUCCESS;
  }
  

  TLorentzVector substructureP4 = getCombinedP4(Tau);

  // create xAOD variables and fill:
  xTau.auxdecor<float>("pt_combined")  = substructureP4.Pt();
  // xTau.auxdecor<float>("eta_combined") = substructureP4.Eta();
  // xTau.auxdecor<float>("phi_combined") = substructureP4.Phi();
  // xTau.auxdecor<float>("m_combined")   = substructureP4.M();  




  // move these to another file? :
  m_weight = -1111.;
  m_combined_res = -1111.;
  m_sigma_tauRec = -1111.;
  m_sigma_constituent = -1111.;
  m_corrcoeff = -1111.;

  if (m_addCalibrationResultVariables==true){
    substructureP4 = getCalibratedConstituentP4(Tau);
    xTau.auxdecor<float>("pt_constituent")  = substructureP4.Pt(); 
    xTau.auxdecor<float>("eta_constituent") = substructureP4.Eta();
    xTau.auxdecor<float>("phi_constituent") = substructureP4.Phi();
    xTau.auxdecor<float>("m_constituent")   = substructureP4.M();  
    
    substructureP4 = getCalibratedTauRecP4(Tau);
    xTau.auxdecor<float>("pt_tauRecCalibrated")  = substructureP4.Pt(); 
    xTau.auxdecor<float>("eta_tauRecCalibrated") = substructureP4.Eta();
    xTau.auxdecor<float>("phi_tauRecCalibrated") = substructureP4.Phi();
    xTau.auxdecor<float>("m_tauRecCalibrated")   = substructureP4.M();  
  
    substructureP4 = getWeightedP4(Tau);
    xTau.auxdecor<float>("pt_weighted")  = substructureP4.Pt(); 
    xTau.auxdecor<float>("eta_weighted") = substructureP4.Eta();
    xTau.auxdecor<float>("phi_weighted") = substructureP4.Phi();
    xTau.auxdecor<float>("m_weighted")   = substructureP4.M();  
    
    xTau.auxdecor<float>("weight_weighted") = m_weight; 
    xTau.auxdecor<float>("sigma_combined") = m_combined_res;
    xTau.auxdecor<float>("sigma_tauRec") = m_sigma_tauRec;
    xTau.auxdecor<float>("sigma_constituent") = m_sigma_constituent;
    xTau.auxdecor<float>("correlation_coefficient") = m_corrcoeff;
  }

  return StatusCode::SUCCESS;

}


int CombinedP4FromRecoTaus::GetIndex_Eta(float eta){
  //std::cout << "Entering GetIndex_Eta(" << eta << ")" << std::endl;
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
  if( fabs(eta) < 2.5 ) {
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

  /*  
  TString calibrationFile = "CalibLoopResult.root";
  TFile *file = TFile::Open(calibrationFile, "READ" );

  TString histname="";    
  TString decayMode="";

  switch(mode){
  case xAOD::TauJetParameters::Mode_1p0n : decayMode = "1p0n";
    break;
  case xAOD::TauJetParameters::Mode_1p1n : decayMode = "1p1n";
    break;
  case xAOD::TauJetParameters::Mode_1pXn : decayMode = "1pXn";
    break;
  case xAOD::TauJetParameters::Mode_3p0n : decayMode = "3p0n";
    break;
  case xAOD::TauJetParameters::Mode_3pXn : decayMode = "3pXn";
    break;
  default: decayMode = "";
  }

  histname="ConstituentEt/CorrelationCoeff_ConstituentEt_" + decayMode;
  TH1F* histogram = (TH1F*) file->GetObjectChecked(histname,"TH1F");      
  std::cout << "\tReturning Mean of Histogram: " << histname  << " : "  << histogram->GetMean() << std::endl;

  return histogram->GetMean();
  */
  
  // from http://en.wikipedia.org/wiki/Standard_deviation#Identities_and_mathematical_properties
  // values from my talk at Tau WG meeting March 10th 2015
  
  /*if( mode == xAOD::TauJetParameters::DecayMode::Mode_1p0n ) return 0.13;
  if( mode == xAOD::TauJetParameters::DecayMode::Mode_1p1n ) return 0.32;
  if( mode == xAOD::TauJetParameters::DecayMode::Mode_1pXn ) return 0.48;
  if( mode == xAOD::TauJetParameters::DecayMode::Mode_3p0n ) return 0.1;
  if( mode == xAOD::TauJetParameters::DecayMode::Mode_3pXn ) return 0.25;
  return 0.;
  */

} 


/*
double CombinedP4FromRecoTaus::getCorrespondingBinContent(const TH1D* hist, double value) const {
  if (not hist) {
    return 0;
  }
  int bin = hist->FindFixBin(value);
  if (bin > hist->GetNbinsX()) bin = hist->GetNbinsX();
  if (bin < 1) bin = 1;
  return hist->GetBinContent(bin);
  }*/




double CombinedP4FromRecoTaus::GetWeightedEt(double et_tauRec, 
					     double et_cb2PT,
					     int etaIndex,
					     const xAOD::TauJetParameters::DecayMode& mode){
  ATH_MSG_DEBUG("Entering CombinedP4FromRecoTaus::GetWeightedEt!");

  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING( "Warning! etaIndex not defined!" );
    return 0.;
  }

  float res_tauRec = GetResolution_taurec( et_tauRec, etaIndex, mode );  
  float res_substruct = GetResolution_CellBased2PanTau( et_cb2PT, etaIndex, mode );
  
  if( res_tauRec == 0. || res_substruct == 0. ) {
    ATH_MSG_WARNING( "Warning! res_tauRec or res_substruct is 0!" );
    ATH_MSG_WARNING( "bin_taurec = " << et_tauRec );
    ATH_MSG_WARNING( "bin_substruct = " << et_cb2PT );
    //int mode=GetIndex_Mode(mode);                                                                                                                                                                                                                                           
    //    m_ResHists_tauRec[mode].Print("all");                                                                                                                                                                                                                               
    m_resHists_tauRec[etaIndex][mode]->Print("all");
    //    m_ResHists_CellBased2PanTau[mode].Print("all");                                                                                                                                                                                                                     
    m_resHists_CellBased2PanTau[etaIndex][mode]->Print("all");
    return 0.;
  }

  //float invres_tauRec=pow(res_tauRec, -2);
  //float invres_substruct=pow(res_substruct, -2);

  float weight=( pow(res_substruct, 2) - GetCorrelationCoefficient(etaIndex, mode )*res_tauRec*res_substruct )
    / ( pow(res_tauRec, 2) + pow(res_substruct, 2) - 2*GetCorrelationCoefficient(etaIndex, mode )*res_tauRec*res_substruct );
  //float weighted_et = ( et_tauRec*invres_tauRec + GetCellbased2PantauEt( et_cb2PT, mode )*invres_substruct ) / ( invres_tauRec + invres_substruct );
  float weighted_et = weight*GetTauRecEt( et_tauRec, etaIndex, mode) + (1 - weight)*GetCellbased2PantauEt( et_cb2PT, etaIndex, mode );

  m_weight = weight;
  
  return weighted_et;
}


double CombinedP4FromRecoTaus::GetResolution_taurec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
  
  ATH_MSG_DEBUG("Entering GetResolution_tauRec!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING("Warning! etaIndex not defined!");
    return 0.;
  }
  
  int bin_taurec = m_resHists_tauRec[etaIndex][mode]->FindBin( et );
  if( bin_taurec > m_resHists_tauRec[etaIndex][mode]->GetNbinsX() ) bin_taurec=m_resHists_tauRec[etaIndex][mode]->GetNbinsX();
  if( bin_taurec < 1 ) bin_taurec=1; 
  ATH_MSG_DEBUG("GetResolution_taurec: " << m_resHists_tauRec[etaIndex][mode]->GetBinContent( bin_taurec ) );
  return m_resHists_tauRec[etaIndex][mode]->GetBinContent( bin_taurec ) * et;

}
 

double CombinedP4FromRecoTaus::GetResolution_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
  ATH_MSG_DEBUG("Entering GetResolution_CellBased2Pantau!");
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING("Warning! decay mode not defined!");
    return 0.;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING( "Warning! etaIndex not defined!");
    return 0.;
  }
  
  int bin_substruct = m_resHists_CellBased2PanTau[etaIndex][mode]->FindBin( et );
  if( bin_substruct > m_resHists_CellBased2PanTau[etaIndex][mode]->GetNbinsX() ) bin_substruct=m_resHists_CellBased2PanTau[etaIndex][mode]->GetNbinsX();
  if( bin_substruct < 1 ) {
    bin_substruct=1;
    ATH_MSG_DEBUG("bin_substruct < 1 . Set to 1!");
  }
  ATH_MSG_DEBUG( "GetResolution_CellBased2PanTau: " << m_resHists_CellBased2PanTau[etaIndex][mode]->GetBinContent( bin_substruct ));
  return m_resHists_CellBased2PanTau[etaIndex][mode]->GetBinContent( bin_substruct ) * et;

}
 
 
double CombinedP4FromRecoTaus::GetMean_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING( "Warning! etaIndex not defined!" );
    return 0.;
  }

  int bin_substruct = m_meanHists_CellBased2PanTau[etaIndex][mode]->FindBin( et );
  if( bin_substruct > m_meanHists_CellBased2PanTau[etaIndex][mode]->GetNbinsX() ) return 0.; // bin_substruct=m_meanHists_CellBased2PanTau[etaIndex][mode]->GetNbinsX();
  if( bin_substruct < 1 ) return 0.; //bin_substruct=1;
  ATH_MSG_DEBUG( "MeanHists_CellBased2PanTau: " << m_meanHists_CellBased2PanTau[etaIndex][mode]->GetBinContent( bin_substruct ));

  // return the uncalibrated ET for the track-only modes:
  if( mode == xAOD::TauJetParameters::Mode_1p0n || mode == xAOD::TauJetParameters::Mode_3p0n ){
    return 0.;
  }

  return m_meanHists_CellBased2PanTau[etaIndex][mode]->GetBinContent( bin_substruct ) * et;

}
 
 
double CombinedP4FromRecoTaus::GetMean_TauRec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING("Warning! etaIndex not defined!");
    return 0.;
  }
    
  int bin_tauRec = m_meanHists_tauRec[etaIndex][mode]->FindBin( et );
  if( bin_tauRec > m_meanHists_tauRec[etaIndex][mode]->GetNbinsX() ) return 0.; //bin_tauRec=m_meanHists_tauRec[etaIndex][mode]->GetNbinsX();
  if( bin_tauRec < 1 ) return 0.; //bin_tauRec=1;
  ATH_MSG_DEBUG("MeanHists_tauRec: " << m_meanHists_tauRec[etaIndex][mode]->GetBinContent( bin_tauRec ) );
  return m_meanHists_tauRec[etaIndex][mode]->GetBinContent( bin_tauRec ) * et;
}
 
 
double CombinedP4FromRecoTaus::GetCombinedResolution( double et_tauRec, double et_cb2PT, int etaIndex, xAOD::TauJetParameters::DecayMode mode){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    ATH_MSG_WARNING( "Warning! decay mode not defined!" );
    return 0.;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING( "Warning! etaIndex not defined!" );
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
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING( "Warning! etaIndex not defined!" );
    return et_cb2PT;
  }
 
  return et_cb2PT - GetMean_CellBased2PanTau(et_cb2PT,etaIndex, mode);
 
}

double CombinedP4FromRecoTaus::GetTauRecEt( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode ){
 
  if( mode < xAOD::TauJetParameters::Mode_1p0n || mode > xAOD::TauJetParameters::Mode_3pXn ){
    return et;
  }
  if( etaIndex < 0 || etaIndex > 4 ){
    ATH_MSG_WARNING( "Warning! etaIndex not defined!" );
    return et;
  }
  
  return et - GetMean_TauRec(et, etaIndex, mode);
 
}

double CombinedP4FromRecoTaus::getCombinedEt(double et_tauRec,
					     double et_substructure,
					     float eta,
					     const xAOD::TauJetParameters::DecayMode& mode) {
  ATH_MSG_DEBUG("Entering CombinedP4FromRecoTaus::getCombinedEt");

  int etaIndex = GetIndex_Eta(eta);
  ATH_MSG_DEBUG("Eta = " << eta << " , eta bin = " << etaIndex );
  if(etaIndex == 99){
    ATH_MSG_WARNING( "Eta = " << eta << " - outside limit! eta bin = " << etaIndex );//is this a warning?
    return et_substructure;
  }

  double et_reco = GetWeightedEt( et_tauRec, et_substructure, etaIndex, mode );
  ATH_MSG_DEBUG( "GetWeightedEt: " << et_reco );
  double combined_res = GetCombinedResolution( et_tauRec, et_substructure, etaIndex, mode );
  ATH_MSG_DEBUG( "Combined_resolution: " << combined_res );
  double et_diff = GetTauRecEt( et_tauRec, etaIndex, mode) - GetCellbased2PantauEt( et_substructure, etaIndex, mode );
  ATH_MSG_DEBUG( "et_diff (GetTauRecEt - GetCellb2PEt): " << et_diff );

  m_combined_res = combined_res;

  if( fabs( et_diff ) > 5*combined_res ) {
    /*    
	  if( mode == RecoTypes::t_3p0n){
	  // *fillOK = false;
	  // return -1;
	  et_reco = et_cb2PT;
	  
	  } else {
    */
    et_reco = et_tauRec;
    m_tauRecEt_takenAs_combinedEt = true;
    ATH_MSG_DEBUG( "(Boolean)m_tauRecEt_takenAs_combinedEt is set to:"  <<  m_tauRecEt_takenAs_combinedEt );
    ATH_MSG_DEBUG( "fabs( et_diff ) > 5*combined_resolution !" );
    //    }
  }
  return et_reco;
}


/*
TLorentzVector CombinedP4FromRecoTaus::getConstituentsP4(const xAOD::TauJet* tau) {
  TLorentzVector constituentsP4;
  // add all charged 4-vectors
  for (size_t i = 0; i < tau->nTracks(); i++) {
    constituentsP4 += tau->track(i)->p4();
  }
  // get all corrected Pi0s and add them
  std::vector<TLorentzVector> correctedPi0P4s;

  // TauAnalysisTools::createPi0Vectors(tau, correctedPi0P4s);
  // for (auto vPi0 : correctedPi0P4s) {
  //   constituentsP4 += vPi0;
  // }

  size_t iNumPi0PFO = tau->nPi0PFOs();
  for (size_t iPFO = 0; iPFO < iNumPi0PFO; iPFO++){
    constituentsP4 += tau->pi0PFO(iPFO)->p4();
  }
  return constituentsP4;
}
*/


TLorentzVector CombinedP4FromRecoTaus::getCombinedP4(const xAOD::TauJet* tau) {

  ATH_MSG_DEBUG( "In CombinedP4FromRecoTaus::getCombinedP4..." );

  m_tauRecEt_takenAs_combinedEt=false;

  ATH_MSG_DEBUG( "(Boolean)m_tauRecEt_takenAs_combinedEt is initialized to: " << m_tauRecEt_takenAs_combinedEt );

  //const TLorentzVector& tauRecP4 = tau->p4();
  TLorentzVector tauRecP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  //  TLorentzVector substructureP4 = getConstituentsP4(tau);
  TLorentzVector substructureP4;
  substructureP4.SetPtEtaPhiM(tau->ptPanTauCellBased(), tau->etaPanTauCellBased(), tau->phiPanTauCellBased(), tau->mPanTauCellBased());
  ATH_MSG_DEBUG( "ConstituentET: " << substructureP4.Et() );
  ATH_MSG_DEBUG( "TauRecET: " << tauRecP4.Et() );
  xAOD::TauJetParameters::DecayMode decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_DecayMode, tmpDecayMode)) {
    decayMode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }
  ATH_MSG_DEBUG( "Decaymode is: " << decayMode );

  //double combinedEt = getCombinedEt(tauRecP4.Et(), substructureP4.Et(), tau->Eta(), decayMode);
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


TLorentzVector CombinedP4FromRecoTaus::getCalibratedConstituentP4(const xAOD::TauJet* tau) {

  ATH_MSG_DEBUG( "In CombinedP4FromRecoTaus::getCalibratedConstituentP4..." );

  //const TLorentzVector& tauRecP4 = tau->p4();
  TLorentzVector tauRecP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  //  TLorentzVector substructureP4 = getConstituentsP4(tau);
  TLorentzVector substructureP4;
  substructureP4.SetPtEtaPhiM(tau->ptPanTauCellBased(), tau->etaPanTauCellBased(), tau->phiPanTauCellBased(), tau->mPanTauCellBased());

  ATH_MSG_DEBUG( "ConstituentET: " << substructureP4.Et() );
  ATH_MSG_DEBUG( "TauRecET: " << tauRecP4.Et() );

  xAOD::TauJetParameters::DecayMode decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_DecayMode, tmpDecayMode)) {
    decayMode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }
  ATH_MSG_DEBUG( "Decaymode is: " << decayMode );

  float eta = tauRecP4.Eta();

  int etaIndex = GetIndex_Eta(eta);
  ATH_MSG_DEBUG( "Eta = " << eta << " , eta bin = " << etaIndex );
  if(etaIndex == 99){
    ATH_MSG_WARNING( "Eta = " << eta << " - outside limit! eta bin = " << etaIndex );//warning?
    return substructureP4;
  }

  double et = GetCellbased2PantauEt( substructureP4.Et(), etaIndex, decayMode );
  ATH_MSG_DEBUG( "et: " << et );
  ATH_MSG_DEBUG( "SubstructureP4.M(): " << substructureP4.M() );

  TLorentzVector p4;
  double pt = sqrt(pow(et,2) - substructureP4.M2());
  ATH_MSG_DEBUG( "pt: " << pt );

  p4.SetPtEtaPhiM(pt, substructureP4.Eta(), substructureP4.Phi(), substructureP4.M());

  return p4;

}

TLorentzVector CombinedP4FromRecoTaus::getCalibratedTauRecP4(const xAOD::TauJet* tau) {

  ATH_MSG_DEBUG( "In CombinedP4FromRecoTaus::getCalibratedTauRecP4..." );

  //const TLorentzVector& tauRecP4 = tau->p4();
  TLorentzVector tauRecP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  //  TLorentzVector substructureP4 = getConstituentsP4(tau);

  ATH_MSG_DEBUG( "TauRecET: " << tauRecP4.Et() );
  xAOD::TauJetParameters::DecayMode decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_DecayMode, tmpDecayMode)) {
    decayMode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }
  ATH_MSG_DEBUG( "Decaymode is: " << decayMode );

  float eta = tauRecP4.Eta();

  int etaIndex = GetIndex_Eta(eta);
  ATH_MSG_DEBUG( "Eta = " << eta << " , eta bin = " << etaIndex );

  if(etaIndex == 99){
    ATH_MSG_WARNING( "Eta = " << eta << " - outside limit! eta bin = " << etaIndex );
    return tauRecP4;
  }

  double et = GetTauRecEt( tauRecP4.Et(), etaIndex, decayMode);
  ATH_MSG_DEBUG( "et: " << et );
  ATH_MSG_DEBUG( "tauRecP4.M(): " << tauRecP4.M2() );

  TLorentzVector p4;
  double pt = sqrt(pow(et,2) - tauRecP4.M2());
  p4.SetPtEtaPhiM(pt, tauRecP4.Eta(), tauRecP4.Phi(), tauRecP4.M());

  return p4;

}

TLorentzVector CombinedP4FromRecoTaus::getWeightedP4(const xAOD::TauJet* tau) {

  ATH_MSG_DEBUG( "In CombinedP4FromRecoTaus::WeightedP4..." );

  //const TLorentzVector& tauRecP4 = tau->p4();
  TLorentzVector tauRecP4;
  tauRecP4.SetPtEtaPhiM(tau->pt(), tau->eta(), tau->phi(), tau->m());
  //  TLorentzVector substructureP4 = getConstituentsP4(tau);
  TLorentzVector substructureP4;
  substructureP4.SetPtEtaPhiM(tau->ptPanTauCellBased(), tau->etaPanTauCellBased(), tau->phiPanTauCellBased(), tau->mPanTauCellBased());
  ATH_MSG_DEBUG( "ConstituentET: " << substructureP4.Et() );
  ATH_MSG_DEBUG( "TauRecET: " << tauRecP4.Et() );
  xAOD::TauJetParameters::DecayMode decayMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
  int tmpDecayMode;
  if (tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::pantau_CellBasedInput_DecayMode, tmpDecayMode)) {
    decayMode = static_cast< xAOD::TauJetParameters::DecayMode>(tmpDecayMode);
  }
  ATH_MSG_DEBUG( "Decaymode is: " << decayMode );

  float eta = tauRecP4.Eta();

  int etaIndex = GetIndex_Eta(eta);
  ATH_MSG_DEBUG( "Eta = " << eta << " , eta bin = " << etaIndex );

  if(etaIndex == 99){
    ATH_MSG_WARNING( "Eta = " << eta << " - outside limit! eta bin = " << etaIndex );
    return substructureP4;
  }

  double et = GetWeightedEt( tauRecP4.Et(), substructureP4.Et(), etaIndex, decayMode );
  ATH_MSG_DEBUG( "et: " << et );

  TLorentzVector p4;
  double pt = sqrt(pow(et,2) - substructureP4.M2());
  p4.SetPtEtaPhiM(pt, substructureP4.Eta(), substructureP4.Phi(), substructureP4.M());

  return p4;

}

