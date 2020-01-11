/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H
#define TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H

//STL include(s)
//#include <memory>
//#include <map>

//Root include(s)
#include "TH1F.h"
#include "TFile.h"
#include "TF1.h"
#include "TGraph.h"

//tauRecTools include(s)
#include "tauRecTools/TauRecToolBase.h"

//xAOD include(s)
#include "xAODTau/TauJet.h"

class CombinedP4FromRecoTaus
: public TauRecToolBase
{
 public:
  ASG_TOOL_CLASS2( CombinedP4FromRecoTaus, TauRecToolBase, ITauToolBase )

    //standard constructor
    CombinedP4FromRecoTaus(const std::string& name="CombinedP4FromRecoTaus");  
    
  //function where variables are computed and decorated
  StatusCode initialize();
        
  // Get correlation coefficient for the given decay mode
  double GetCorrelationCoefficient(int etaIndex, const xAOD::TauJetParameters::DecayMode decayMode);

  //Calculates the tau 4-vector
  //TLorentzVector getConstituentsP4(const xAOD::TauJet* tau);
    
  double GetWeightedEt(double et_tauRec, 
		       double et_cb2PT,
		       int etaIndex,
		       const xAOD::TauJetParameters::DecayMode& mode);

  double GetResolution_taurec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode);
  double GetResolution_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode);
  double GetMean_TauRec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode);
  double GetMean_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode);
  double GetCombinedResolution( double et_tauRec, double et_cb2PT, int etaIndex, xAOD::TauJetParameters::DecayMode mode);

  double GetTauRecEt( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode);
  double GetCellbased2PantauEt( double et_cb2PT, int etaIndex, xAOD::TauJetParameters::DecayMode mode);

  //Calculates the optimal tau Et 
  double getCombinedEt(double et_tauRec, 
		       double et_substructure, 
		       float eta,
		       const xAOD::TauJetParameters::DecayMode& mode);


  //Calculates the optimal tau 4-vector
  TLorentzVector getCombinedP4(const xAOD::TauJet* tau);

  // Get the enum-value for eta corresponding to the eta value
  int GetIndex_Eta(float eta);
  float GetNsigma_Compatibility(float et_TauRec);  

  //high pt flag
  double GetCaloResolution(const xAOD::TauJet* tau);
  bool GetUseCaloPtFlag(const xAOD::TauJet* tau);

  StatusCode execute(xAOD::TauJet& xTau); 

 private:
  /*std::vector< std::vector<TH1F*> >  m_resHists_tauRec;
  std::vector< std::vector<TH1F*> >  m_resHists_CellBased2PanTau;
  std::vector< std::vector<TH1F*> >  m_meanHists_CellBased2PanTau;
  std::vector< std::vector<TH1F*> >  m_meanHists_tauRec;*/
  std::vector< std::vector<TGraph*> >  m_resTGraph_tauRec;
  std::vector< std::vector<TGraph*> >  m_resTGraph_CellBased2PanTau;
  std::vector< std::vector<TGraph*> >  m_meanTGraph_CellBased2PanTau;
  std::vector< std::vector<TGraph*> >  m_meanTGraph_tauRec;

  std::vector<TH1F*> m_correlationHists;

  std::vector<TString> m_modeNames = {"1p0n","1p1n","1pXn","3p0n","3pXn"};
  std::vector<TString> m_etaBinNames = {"0", "1", "2", "3", "4"};//("<0.3"), ("<0.8"), ("<1.3"), ("<1.6"), ("<2.5")

  std::string m_calibFilePath;

  bool m_addCalibrationResultVariables;
  bool m_addUseCaloPtFlag;
  bool m_tauRecEt_takenAs_combinedEt;
  double m_weight, m_combined_res, m_sigma_tauRec, m_sigma_constituent, m_corrcoeff;
  double m_et_cb2PT_postcalib, m_et_postcalib, m_et_weighted;
    
  std::string m_sWeightFileName;
  std::unique_ptr<TF1> m_Nsigma_compatibility;
};


#endif // TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H
