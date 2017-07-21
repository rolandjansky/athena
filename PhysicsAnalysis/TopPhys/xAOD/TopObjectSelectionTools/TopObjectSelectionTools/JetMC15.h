/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMC15_H_
#define JETMC15_H_

#include "TopObjectSelectionTools/JetSelectionBase.h"
#include "JetJvtEfficiency/IJetJvtEfficiency.h"

#include "AsgTools/ToolHandle.h"

# include <string>

namespace top {

/**
 * @brief A class that performs object cuts on Jets (of any size!).  At the
 * moment it includes a cut on pT, eta and JVT.
 */
class JetMC15 : public JetSelectionBase {
 public:
  /**
   * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
   *
   * @param ptcut The minimum pT to accept good jets.
   * @param etamax The maximum eta cut.
   * @param doJVTCut To perform JVT cut - should be false for small-R jets
   * @param fwdJetSel Scenario for treating forward jets.
   */
  JetMC15(const double ptcut, const double etamax, const bool doJVTCut, const std::string fwdJetSel="default");

  /**
   * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
   *
   * This version of the constructor always perform JVT cut
   *
   * @param ptcut The minimum pT to accept good jets.
   * @param etamax The maximum eta cut.
   * @param fwdJetSel Scenario for treating forward jets.
   */
  JetMC15(const double ptcut, const double etamax, const std::string fwdJetSel="default");

  /**
   * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
   *
   * DEPRECATED - only kept for backward compatibility - no need to specify any jvt cut, please use the above function instead.
   *
   * @param ptcut The minimum pT to accept good jets.
   * @param etamax The maximum eta cut.
   * @param jvtmin The minimum cut on the jet vertex fraction (no longer used)
   */
  JetMC15(const double ptcut, const double etamax, const double);

  // Does nothing.
  virtual ~JetMC15() {}

  /**
   * @brief Implements the logic to select jets based on pT and eta.
   *
   * It cuts on pt, eta, jvt.
   *
   * @param jet The jet to check.
   * @return True if the jet passes the cuts, false otherwise.
   */
  virtual bool passSelection(const xAOD::Jet& jet) override;

  // Print the cuts to the ostream.
  virtual void print(std::ostream& os) const override;

 protected:
  // The lower pT cut threshold to apply to the object.
  double m_ptcut;

  // The upper eta cut.
  double m_etamax;

  // Scenario for treating forward jets
  std::string m_fwdJetSel;

  // To do JVT cut - should be false for large-R jets
  bool m_applyJVTCut;

  ToolHandle<CP::IJetJvtEfficiency> m_jvt_tool;
};
}  // namespace top
#endif
