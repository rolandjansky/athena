/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONCUTBASEDMC15_H_
#define ELECTRONCUTBASEDMC15_H_

#include "TopObjectSelectionTools/ElectronSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include <memory>

namespace top {

/**
 * @brief Electron selection based on the cut-based thingy.
 */
class ElectronCutBasedMC15 : public top::ElectronSelectionBase {
public:
    /**
     * @brief Class to help select cut-based good electrons.
     *
     * @param ptcut The minimum pT cut to apply to the electrons.
     * @param vetoCrack Do you want to veto 1.37 < |cluster_eta| < 1.52?
     * @param quality The definition for good electrons, e.g. "Tight"
     * @param qualityLoose The loose defeinition, for fake-lepton estimates with
     * the matrix method?
     * @param isolation nullptr for un-isolated, or a new "isolation object" to
     * apply isolation cuts
     */
    ElectronCutBasedMC15(double ptcut, bool vetoCrack, const std::string& quality, const std::string& qualityLoose, IsolationBase* isolation);
    virtual ~ElectronCutBasedMC15(){}
    /**
     * @brief The cuts to select good electrons for your analysis should be
     * implemented in here.
     *
     * @param el The electron to cut on (all electrons in the event are passed
     * to the tool)
     * @return True if this is a good electron, false otherwise.
     */
    virtual bool passSelection(const xAOD::Electron& el) const override;

    /**
     * @brief The loose selection needed by some background estimates.
     *
     * @param el
     * @return
     */
    virtual bool passSelectionLoose(const xAOD::Electron& el) const override;

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
     * the un-isolated part in one function.
     *
     * Do all the cuts except for the isolation.
     *
     * @param el The electron in question
     * @param quality The string "Tight" or whatever to say which quality cuts
     * should be applied.
     * @return True if the electron passes
     */
    bool passSelectionNoIsolation(const xAOD::Electron& el, const std::string& quality) const;

    ///minimum pT cut to apply
    double m_ptcut;

    ///Should we veto the LAr crack region?
    bool m_vetoCrack;

    ///Quality requirement for the tight (analysis) definition
    std::string m_quality;

    ///Quality requirement for the loose (background estimate?) definition
    std::string m_qualityLoose;

    ///The isolation tool, or nullptr if we don't want isolation
    std::unique_ptr<top::IsolationBase> m_isolation;
};

}

#endif
