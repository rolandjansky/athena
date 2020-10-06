/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H
#define TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H

#include "tauRecTools/TauRecToolBase.h"

#include "xAODTau/TauJet.h"

#include "TH1F.h"
#include "TF1.h"
#include "TGraph.h"

#include <array>
#include <string>

class CombinedP4FromRecoTaus : public TauRecToolBase {
public:
  ASG_TOOL_CLASS2( CombinedP4FromRecoTaus, TauRecToolBase, ITauToolBase )

  CombinedP4FromRecoTaus(const std::string& name="CombinedP4FromRecoTaus");  
    
  virtual StatusCode initialize() override;
      
  virtual StatusCode execute(xAOD::TauJet& xTau) const override;

  /** Whether to use calo pt, invoked by TauSmearing tool */
  bool getUseCaloPtFlag(const xAOD::TauJet& tau) const;
  
  /** Get the resolution of Et at the calo TES, invoked by METSignificance */
  double getCaloResolution(const xAOD::TauJet& tau) const;

private:
  struct Variables
  {
    double pt_constituent{0.0};
    double pt_tauRecCalibrated{0.0};
    double pt_weighted{0.0};
    double weight{-1111.0};
    double sigma_combined{-1111.};
    double sigma_tauRec{-1111.0};
    double sigma_constituent{-1111.0};
    double corrcoeff{-1111.0};
  };

  /** Get the weighted four momentum of calo TES and PanTau */
  TLorentzVector getCombinedP4(const xAOD::TauJet& tau, Variables& variables) const;

  /** Whether the tau candidate is valid for the calculation */
  bool isValid(const xAOD::TauJet& tau) const;

  /** Get the index of eta in the calibration histogram */
  int getEtaIndex(const float& eta) const;

  /** Get the decay mode of the tau candidate */
  xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TauJet& tau) const;
  
  /** Get the index of decay mode in the calibration histogram */
  int getDecayModeIndex(const xAOD::TauJetParameters::DecayMode& decayMode) const;

  /** Get correlation coefficient between the calo TES and PanTau */
  double getCorrelation(const int& decayModeIndex, const int& etaIndex) const;
  
  /** Get the resolution of Et at the calo TES */
  double getCaloResolution(const double& et, const int& decayModeIndex, const int& etaIndex) const;
  
  /** Get the resolution of Et at PanTau */
  double getPanTauResolution(const double& et, const int& decayModeIndex, const int& etaIndex) const;
  
  /** Get the Et at the calo TES after calibration correction */ 
  double getCaloCalEt(const double& et, const int& decayModeIndex, const int& etaIndex) const;

  /** Get the Et at PanTau after calibration correction */ 
  double getPanTauCalEt(const double& panTauEt, const int& decayModeIndex, 
                          const int& etaIndex) const;
 
  /** Get the weight of calo TES */
  double getWeight(const double& caloSigma, const double& panTauSigma, const double& correlatioon) const;

  /** Get the combined sigma of calo TES and PanTau */
  double getCombinedSigma(const double& caloSigma, const double& panTauSigma, const double& correlation) const;

  /** Get the combined Et of calo TES and PanTau */
  double getCombinedEt(const double& caloEt, const double& et_substructure,
		               const xAOD::TauJetParameters::DecayMode& decayMode, const float& eta, 
                       Variables& variables) const;
  
  /** Get the allowed difference between calo TES and PanTau */ 
  double getNsigmaCompatibility(const double& caloEt) const;

  /// Switch of adding the intermediate results
  bool m_addCalibrationResultVariables;
  
  /// Name of the calibration file 
  std::string m_calFileName;
  
  /// Binning in the calibraction graph/hist
  enum Binning {DecayModeBinning = 5, EtaBinning = 5};

  /// Decay mode binning in the calibration graph/hist
  const std::array<std::string, DecayModeBinning> m_decayModeNames = {"1p0n","1p1n","1pXn","3p0n","3pXn"}; //!
  
  /// Eta binning in the calibration graph
  const std::array<std::string, EtaBinning> m_etaBinNames = {"0", "1", "2", "3", "4"}; //!
  
  /// Calibration graph: mean of bias/caloEt as a function of caloEt
  std::array<std::array<std::unique_ptr<TGraph>, DecayModeBinning>, EtaBinning> m_caloRelBias; //!

  /// Maximum Et of m_caloRelBias
  std::array<std::array<double, DecayModeBinning>, EtaBinning> m_caloRelBiasMaxEt; //!

  /// Calibration graph: resolution at Calo TES as a function of caloEt
  std::array<std::array<std::unique_ptr<TGraph>, DecayModeBinning>, EtaBinning> m_caloRes; //!

  /// Maximum Et of m_caloRes
  std::array<std::array<double, DecayModeBinning>, EtaBinning> m_caloResMaxEt; //!

  /// Calibration graph: mean of bias/panTauEt as a funtion of panTauEt
  std::array<std::array<std::unique_ptr<TGraph>, DecayModeBinning>, EtaBinning> m_panTauRelBias; //!
  
  /// Maximum Et of m_panTauRelBias
  std::array<std::array<double, DecayModeBinning>, EtaBinning> m_panTauRelBiasMaxEt; //!

  /// Calibration graph: resolution at PanTau as a function of panTauEt
  std::array<std::array<std::unique_ptr<TGraph>, DecayModeBinning>, EtaBinning> m_panTauRes; //!
  
  /// Maximum Et of m_panTauRes
  std::array<std::array<double, DecayModeBinning>, EtaBinning> m_panTauResMaxEt; //!
  
  /// Calibration histogram: correlation coefficient of calo TES and PanTau
  std::array<std::unique_ptr<TH1F>, DecayModeBinning> m_correlationHists; //!

  /// Calibration histogram: maximum tolerence in unit of combined sigma
  std::unique_ptr<TF1> m_nSigmaCompatibility; //!
};

#endif // TAURECTOOLS_COMBINEDP4FROMRECOTAUS_H
