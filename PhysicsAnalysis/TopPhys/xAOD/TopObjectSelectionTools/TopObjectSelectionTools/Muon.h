/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPOBJECTSELECTIONTOOLSMUON_H_
#define TOPOBJECTSELECTIONTOOLSMUON_H_

#include "TopObjectSelectionTools/MuonSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include "AsgTools/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace top {
/**
 * @brief Select muons based on some early suggestions.
 */
  class Muon: public MuonSelectionBase {
  public:
    /**
     * @brief Construct the tool to select good muons.
     *
     * @param ptcut The minimum pT cut for good muons.
     * @param isolation The isolation the user wants to apply.  Don't want any
     *        isolation to be applied?  Then leave this as a nullptr.
     * @param d0SigCut The maximum d0 significance cut
     * @param delta_z0 The maximum |delta z0 sin(theta)| cut
     * @param applyTTVACut Whether to apply cuts on d0 and z0
     */
    Muon(const double ptcut,
         IsolationBase* isolation);

    Muon(const double ptcut,
         IsolationBase* isolation,
         const bool applyTTVACut);

    Muon(const double ptcut,
         IsolationBase* isolation,
         const double d0SigCut,
         const double delta_z0,
         const bool applyTTVACut = true);

    // Does nothing.
    virtual ~Muon() {}

    /**
     * @brief Implements the logic to select good muons.
     *
     * @param mu The muon that we want to check.
     * @return True if the muon is good, false otherwise.
     */
    virtual bool passSelection(const xAOD::Muon& mu) const override;

    /**
     * @brief The loose selection needed by some background estimates.
     *
     * @param mu
     * @return
     */
    virtual bool passSelectionLoose(const xAOD::Muon& mu) const override;

    /**
     * @brief The track-to-vertex association (TTVA) cuts.
     *
     * @param mu
     * @return True if passes, false otherwise
     */
    virtual bool passTTVACuts(const xAOD::Muon& mu) const;

    // Print the cuts to the ostream.
    virtual void print(std::ostream& os) const override;
  protected:
    // Lower pT threshold to apply to object selection.
    double m_ptcut;

    // TTVA cuts
    double m_d0SigCut;
    double m_delta_z0;

    // Proper tool to select muons.
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolLoose;

    // Isolation tool, can be nullptr meaning "no isolation requirement"
    std::unique_ptr<top::IsolationBase> m_isolation;

    //  decide if TTVA cut should be applied at all
    bool m_applyTTVACut;
  };
}  // namespace top

#endif  // TOPOBJECTSELECTIONTOOLSMUON_H_
