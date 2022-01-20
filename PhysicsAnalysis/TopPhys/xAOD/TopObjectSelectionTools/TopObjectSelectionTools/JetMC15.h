/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef JETMC15_H_
#define JETMC15_H_

#include "TopObjectSelectionTools/JetSelectionBase.h"
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"

#include "AsgTools/ToolHandle.h"

# include <string>

namespace top {
/**
 * @brief A class that performs object cuts on Jets (of any size!).  At the
 * moment it includes a cut on pT, eta and JVT.
 */
  class JetMC15: public JetSelectionBase {
  public:
    /**
     * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
     *
     * @param ptcut The minimum pT to accept good jets.
     * @param etamax The maximum eta cut.
     * @param minmass The minimum mass to accept good jets.
     * @param doJVTCut To perform JVT cut - should be false for small-R jets
     */
    JetMC15(const double ptcut, const double etamax, const double minmass, const bool doJVTCut);

    /**
     * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
     *
     * This version of the constructor is for using the jet-electron model
     * This would add a flag to each jet to tell if the jet passes the jet-electron cuts
     *
     * @param ptcut The minimum pT to accept good jets.
     * @param etamax The maximum eta cut.
     * @param minmass The minimum mass to accept good jets.
     * @param doJVTCut To perform JVT cut - should be false for small-R jets
     * @param jetElectronEMFractionMin Minimal EM frac value for jet-electrons
     * @param jetElectronEMFractionMax Maximal EM frac value for jet-electrons
     * @param jetElectronEtaMax Maximal absolute eta value for jet-electrons
     */
    JetMC15(const double ptcut, const double etamax, const bool doJVTCut,
            const float jetElectronEMFractionMin, const float jetElectronEMFractionMax, const float jetElectronEtaMax);

    /**
     * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
     *
     * This version of the constructor always perform JVT cut
     *
     * @param ptcut The minimum pT to accept good jets.
     * @param etamax The maximum eta cut.
     */
    JetMC15(const double ptcut, const double etamax);

    /**
     * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
     *
     * DEPRECATED - Only kept for backwards compatibility, to stop external extensions from crashing, please use above forms
     *
     * @param ptcut The minimum pT to accept good jets.
     * @param etamax The maximum eta cut.
     * @param fwdJetSel Scenario for treating forward jets.
     */
    JetMC15(const double ptcut, const double etamax, const std::string);

    /**
     * @brief A tool to select some jets for MC15 based on the pT and eta cuts.
     *
     * DEPRECATED - only kept for backward compatibility - no need to specify any jvt cut, please use the above function
     *instead.
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

    /// jet-electron parameters
    // jet-electrons not used by default
    bool m_useJetElectrons=false;
    // minimal jet-electron EM frac cut
    float m_jetElectronEMFractionMin;
    // maximal jet-electron EM frac cut
    float m_jetElectronEMFractionMax;
    // maximal jet-electron eta cut
    float m_jetElectronEtaMax;

    // The lower mass cut threshold to apply on the object.
    // if negative, do not apply mass cut
    double m_masscut;

    // To do JVT cut - should be false for large-R jets
    bool m_applyJVTCut;

    // Whether minimum mass cut should be applied
    bool m_appyMassCut;

    ToolHandle<CP::IJetJvtEfficiency> m_jvt_tool;
  };
}  // namespace top
#endif
