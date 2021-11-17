/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPOBJECTSELECTIONTOOLS_SOFTMUON_H_
#define TOPOBJECTSELECTIONTOOLS_SOFTMUON_H_

#include "TopObjectSelectionTools/SoftMuonSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include "AsgTools/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace top {
/**
 * @brief Select muons based on some early suggestions.
 */
  class SoftMuon: public SoftMuonSelectionBase {
  public:
    /**
     * @brief Construct the tool to select good muons.
     *
     * @param ptcut The minimum pT cut for good muons.
     * @param isolation The isolation the user wants to apply.  Don't want any
     * isolation to be applied?  Then leave this as a nullptr.
     */
    SoftMuon(const double ptcut);

    // Does nothing.
    virtual ~SoftMuon() {}

    /**
     * @brief Implements the logic to select good muons.
     *
     * @param mu The muon that we want to check.
     * @return True if the muon is good, false otherwise.
     */
    virtual bool passSelection(const xAOD::Muon& mu) const override;

    // Print the cuts to the ostream.
    virtual void print(std::ostream& os) const override;
  protected:
    // Lower pT threshold to apply to object selection.
    double m_ptcut;

    // Proper tool to select muons.
    ToolHandle<CP::IMuonSelectionTool> m_softmuonSelectionTool;
  };
}  // namespace top

#endif  // TOPOBJECTSELECTIONTOOLS_SOFTMUON_H_
