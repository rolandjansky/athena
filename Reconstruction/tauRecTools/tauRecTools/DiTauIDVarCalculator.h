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

// Core include(s):
// #include "TruthUtils/PIDHelpers.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequenceArea.hh"

// EDM include(s):
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/DiTauJetContainer.h"

// Local include(s):
#include "tauRecTools/IDiTauIDVarCalculator.h"


namespace tauRecTools
{


class DiTauIDVarCalculator
  : public tauRecTools::IDiTauIDVarCalculator
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( DiTauIDVarCalculator,
                  tauRecTools::IDiTauIDVarCalculator )

public:

  DiTauIDVarCalculator( const std::string& name );

  virtual ~DiTauIDVarCalculator();

  // initialize the tool
  virtual StatusCode initialize();

  // set pointer to event
  virtual StatusCode initializeEvent();

  // calculate ID variables
  virtual StatusCode calculateIDVariables(const xAOD::DiTauJet& xDiTau);

private:
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
  double mass_track(const xAOD::DiTauJet& xDiTau) const;
  double mass_track_core(const xAOD::DiTauJet& xDiTau) const;
  double mass_core(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
  double mass_track_all(const xAOD::DiTauJet& xDiTau) const;
  double mass_tracks(const xAOD::DiTauJet& xDiTau, int iSubjet) const;
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
  std::string m_sDiTauContainerName;
  std::string m_DiTauContainerNameAux;
  bool m_bCalcCluserVariables;
  std::string m_sTruthTauContainerName;

  double m_dDefault;

  StatusCode decorNtracks (const xAOD::DiTauJet& xDiTau);

}; // class DiTauIDVarCalculator

}
#endif // TAURECTOOLS_DITAUIDVARCALCULATOR_H
