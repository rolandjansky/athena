// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAURECTOOLS_DITAUIDVARCALCULATOR_H
#define  TAURECTOOLS_DITAUIDVARCALCULATOR_H

/**
 * @brief Tool to calculate and decorate ID variables for boosted di-tau candidates.
 * 
 * @author David Kirchmeier (david.kirchmeier@cern.ch)
 *                                                                              
 */


// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// Core include(s):
// #include "TruthUtils/PIDHelpers.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequenceArea.hh"

// EDM include(s):
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODEgamma/ElectronContainer.h"

#include "tauRecTools/TauEventData.h"

// Selection Tools include:


// Local include(s):
#include "tauRecTools/IDiTauToolBase.h"
#include "tauRecTools/ITauToolBase.h"

namespace CP{
  class IIsolationSelectionTool;
  class IMuonSelectionTool;
  class IMuonCalibrationAndSmearingTool;
}
class AsgElectronLikelihoodTool;

namespace tauRecTools{

class DiTauIDVarCalculator
  : public tauRecTools::IDiTauToolBase
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( DiTauIDVarCalculator,
                  tauRecTools::IDiTauToolBase )

public:

  DiTauIDVarCalculator( const std::string& name );

  virtual ~DiTauIDVarCalculator();

  // initialize the tool
  virtual StatusCode initialize();

  // calculate ID variables depricated
  virtual StatusCode calculateIDVariables(const xAOD::DiTauJet& xDiTau);

  // calculate ID variables
  virtual StatusCode execute(const xAOD::DiTauJet& xDiTau);
  
private:
  virtual StatusCode calculateHadHadIDVariables(const xAOD::DiTauJet& xDiTau);
  virtual StatusCode calculateHadElIDVariables (const xAOD::DiTauJet& xDiTau);
  virtual StatusCode calculateHadMuIDVariables (const xAOD::DiTauJet& xDiTau);
  asg::AnaToolHandle<CP::IMuonSelectionTool>* m_muSelTool_loose;    
  asg::AnaToolHandle<CP::IMuonSelectionTool>* m_muSelTool_medium;   
  asg::AnaToolHandle<CP::IMuonSelectionTool>* m_muSelTool_tight;    
  asg::AnaToolHandle<CP::IMuonSelectionTool>* m_muSelTool_veryloose;
  asg::AnaToolHandle<CP::IMuonSelectionTool>* m_muSelTool_highpt;   

  asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool>* m_muonCalibrationTool;
  asg::AnaToolHandle<CP::IIsolationSelectionTool>* m_isoSelTool;
  asg::AnaToolHandle<AsgElectronLikelihoodTool>* m_electronLikeliHoodTool_medium;
  asg::AnaToolHandle<AsgElectronLikelihoodTool>* m_electronLikeliHoodTool_loose;
  asg::AnaToolHandle<AsgElectronLikelihoodTool>* m_electronLikeliHoodTool_loose_CutBL;
  asg::AnaToolHandle<AsgElectronLikelihoodTool>* m_electronLikeliHoodTool_tight;
  asg::AnaToolHandle<AsgElectronLikelihoodTool>* m_electronLikeliHoodTool_veryloose;

  asg::AnaToolHandle<ITauToolBase>* m_muonTrackRemoval;
  asg::AnaToolHandle<ITauToolBase>* m_tauSubstructureVariables;
  asg::AnaToolHandle<ITauToolBase>* m_tauCommonCalcVars;
  
  float getElectronInfo(const xAOD::Electron* el, const xAOD::EgammaParameters::ShowerShapeType information);

  double n_subjets(const xAOD::DiTauJet& xDiTau) const;
  double ditau_pt(const xAOD::DiTauJet& xDiTau) const;
  double f_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double f_subjet(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double f_subjets(const xAOD::DiTauJet& xDiTau) const;
  double f_track(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double R_max(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  int n_track(const xAOD::DiTauJet& xDiTau) const;
  int n_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  int n_isotrack(const xAOD::DiTauJet& xDiTau) const;
  int n_othertrack(const xAOD::DiTauJet& xDiTau) const;
  double R_track(const xAOD::DiTauJet& xDiTau) const;
  double R_track_all(const xAOD::DiTauJet& xDiTau) const;
  double R_track_core(const xAOD::DiTauJet& xDiTau) const;
  double R_isotrack(const xAOD::DiTauJet& xDiTau) const;
  double R_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double R_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double m_track(const xAOD::DiTauJet& xDiTau) const;
  double m_track_core(const xAOD::DiTauJet& xDiTau) const;
  double m_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double m_track_all(const xAOD::DiTauJet& xDiTau) const;
  double m_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double E_frac(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double R_subjets(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double d0_leadtrack(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double f_isotracks(const xAOD::DiTauJet& xDiTau) const;
  int n_iso_ellipse(const xAOD::DiTauJet& xDiTau) const;
  std::vector<fastjet::PseudoJet> getClusters(const xAOD::DiTauJet& xDiTau) const;
  int n_antikt_subjets(std::vector<fastjet::PseudoJet> vClusters);
  int n_ca_subjets(std::vector<fastjet::PseudoJet> vClusters);
  void mass_drop(const xAOD::DiTauJet& xDiTau, std::vector<fastjet::PseudoJet> vClusters) const;
  double f_clusters(const xAOD::DiTauJet& xDiTau, std::vector<fastjet::PseudoJet> vClusters) const;

  // steering variables
  double m_dMaxDeltaR;
  std::string m_sDiTauContainerName;
  std::string m_DiTauContainerNameAux;
  bool m_bCalcCluserVariables;
  bool m_bMuonTrackRemoval;
  std::string m_sTruthTauContainerName;
  std::string m_sDecayChannel;
  enum DecayChannel{ HadHad, HadEl, HadMu, Default };
  DecayChannel m_eDecayChannel;
  double m_dDefault;
  
  StatusCode decorNtracks (const xAOD::DiTauJet& xDiTau);
  TauEventData m_data;
}; // class DiTauIDVarCalculator

}

#endif // TAURECTOOLS_DITAUIDVARCALCULATOR_H
