/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H
#define TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H

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
  struct Variables
  {
    double weight{-1111.0};
    double combined_res{-1111.};
    double sigma_tauRec{-1111.0};
    double sigma_constituent{-1111.0};
    double corrcoeff{-1111.0};
    double et_weighted{0.0};
    double et_cb2PT_postcalib{0.0};
    double et_postcalib{0.0};
  };

 public:
  ASG_TOOL_CLASS2( CombinedP4FromRecoTaus, TauRecToolBase, ITauToolBase )

    //standard constructor
    CombinedP4FromRecoTaus(const std::string& name="CombinedP4FromRecoTaus");  
    
  //function where variables are computed and decorated
  StatusCode initialize() override;
        
  // Get correlation coefficient for the given decay mode
  double GetCorrelationCoefficient(int etaIndex, const xAOD::TauJetParameters::DecayMode decayMode) const;

  //Calculates the tau 4-vector
  //TLorentzVector getConstituentsP4(const xAOD::TauJet* tau);
    
  double GetWeightedEt(double et_tauRec, 
		       double et_cb2PT,
		       int etaIndex,
		       const xAOD::TauJetParameters::DecayMode& mode,
                       Variables& variables) const;

  double GetResolution_taurec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode) const;

  double GetResolution_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode) const;

  double GetMean_TauRec( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode) const;

  double GetMean_CellBased2PanTau( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode) const;

  double GetCombinedResolution(double et_tauRec,
                               double et_cb2PT,
                               int etaIndex,
                               xAOD::TauJetParameters::DecayMode mode,
                               Variables& variables) const;

  double GetTauRecEt( double et, int etaIndex, xAOD::TauJetParameters::DecayMode mode, double& et_postcalib) const;

  double GetCellbased2PantauEt(double et_cb2PT,
                               int etaIndex,
                               xAOD::TauJetParameters::DecayMode mode,
                               double& et_cb2PT_postcalib) const;

  //Calculates the optimal tau Et 
  double getCombinedEt(double et_tauRec, 
		       double et_substructure, 
		       float eta,
		       const xAOD::TauJetParameters::DecayMode& mode,
                       Variables& variables) const;


  //Calculates the optimal tau 4-vector
  TLorentzVector getCombinedP4(const xAOD::TauJet* tau,
                               Variables& variables) const;

  // Get the enum-value for eta corresponding to the eta value
  int GetIndex_Eta(float eta) const;

  float GetNsigma_Compatibility(float et_TauRec) const;

  //high pt flag
  double GetCaloResolution(const xAOD::TauJet* tau) const;

  bool GetUseCaloPtFlag(const xAOD::TauJet* tau) const;

  StatusCode execute(xAOD::TauJet& xTau) override
  {
    return execute_const(xTau);
  } 
  StatusCode execute_const(xAOD::TauJet& xTau) const;

 private:
  const std::vector<TString> m_modeNames = {"1p0n","1p1n","1pXn","3p0n","3pXn"};
  const std::vector<TString> m_etaBinNames = {"0", "1", "2", "3", "4"};//("<0.3"), ("<0.8"), ("<1.3"), ("<1.6"), ("<2.5")
  
  /// row: size of m_etaBinNames, column: size of m_modeNames
  std::vector<std::vector<std::unique_ptr<TGraph>>> m_meanTGraph_CellBased2PanTau; 
  std::vector<std::vector<std::unique_ptr<TGraph>>> m_resTGraph_CellBased2PanTau;
  
  std::vector<std::vector<std::unique_ptr<TGraph>>> m_meanTGraph_tauRec;
  std::vector<std::vector<std::unique_ptr<TGraph>>> m_resTGraph_tauRec;

  /// size of m_modeNames
  std::vector<std::unique_ptr<TH1F>> m_correlationHists;

  std::string m_calibFilePath;

  bool m_addCalibrationResultVariables;
  bool m_addUseCaloPtFlag;
    
  std::string m_sWeightFileName;
  std::unique_ptr<TF1> m_Nsigma_compatibility;
};


#endif // TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H
