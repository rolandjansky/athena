/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FWDELECTRONMC15_H_
#define FWDELECTRONMC15_H_

#include "TopObjectSelectionTools/FwdElectronSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronLikelihoodTool.h"
#include "TrigBunchCrossingTool/WebBunchCrossingTool.h"
#include "TopConfiguration/TopConfig.h"

#include <memory>

namespace top {

/**
 * @brief Electron selection based on the cut-based thingy.
 */
class FwdElectronMC15 : public top::FwdElectronSelectionBase {
public:
    /**
     * @brief Class to help select cut-based good electrons.
     *
     * @param ptcut The minimum pT cut to apply to the electrons.
     * *
     * @param minEtacut The minimum |eta| cut to apply to the electrons.
     * *
     * @param maxEtacut The maximum |eta| cut to apply to the electrons.
     */
    FwdElectronMC15(double ptcut, double minEtacut, double maxEtacut, const std::shared_ptr<top::TopConfig> config);
    virtual ~FwdElectronMC15(){}
    /**
     * @brief The cuts to select good electrons for your analysis should be
     * implemented in here.
     *
     * @param el The electron to cut on (all electrons in the event are passed
     * to the tool)
     * @return True if this is a good electron, false otherwise.
     */
    virtual bool passSelection(const xAOD::Electron& el, int bcid, int runNumber) const override;

    /**
     * @brief The loose selection needed by some background estimates.
     *
     * @param el
     * @return
     */
    virtual bool passSelectionLoose(const xAOD::Electron& el, int bcid, int runNumber) const override;

    /**
     * @brief Print some useful information about the electron selection.
     *
     * Usually this goes to the log file, so you know what you ran with.
     *
     * @param Where the print-out should go, e.g. cout.
     */
    virtual void print(std::ostream&) const override;

protected:
    /**
     * @brief Since both selections are fairly similar, we can perform
     * the common part in one function.
     *
     * Do all the cuts except for the isolation.
     *
     * @param el The electron in question
     * @return True if the electron passes
     */
    bool passSelectionCommon(const xAOD::Electron& el, int bcid, int runNumber) const;

    ///minimum pT cut to apply
    double m_ptcut;
    double m_minEtacut;
    double m_maxEtacut;
    bool m_isMC;
    int m_fwdElectronBCIDCleaningMinRun;
    int m_fwdElectronBCIDCleaningMaxRun;
    
    /// Proper tool to select electrons.
    ToolHandle<AsgForwardElectronLikelihoodTool> m_fwdElectronSelectionTool;
    ToolHandle<AsgForwardElectronLikelihoodTool> m_fwdElectronLooseSelectionTool;
    ToolHandle<Trig::WebBunchCrossingTool> m_webBunchCrossingTool;
    
    /**
     * @brief Pointer to the configuration object so we can check which objects
     * were requested in the config file.
     */
    const std::shared_ptr<top::TopConfig> m_config;

};

}

#endif
