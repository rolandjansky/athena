/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANTIMUONMC15_H_
#define ANTIMUONMC15_H_

#include "TopObjectSelectionTools/MuonSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include "AsgTools/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace top {

/**
 * @brief Select muons based on some early MC15 suggestions.
 */
class AntiMuonMC15 : public MuonSelectionBase {
public:
    /**
     * @brief Construct the tool to select good muons.
     *
     * @param ptcut The minimum pT cut for good muons.
     * @param isolation The isolation the user wants to apply.  Don't want any
     * isolation to be applied?  Then leave this as a nullptr.
     */
    AntiMuonMC15(const double ptcut, IsolationBase* isolation);

    ///Does nothing.
    virtual ~AntiMuonMC15(){}

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

    ///Print the cuts to the ostream.
    virtual void print(std::ostream& os) const override;


protected:
    ///Lower pT threshold to apply to object selection.
    double m_ptcut;

    ///Proper tool to select muons.
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
//    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolLoose;

    ///Isolation tool, can be nullptr meaning "no isolation requirement"
//    std::unique_ptr<top::IsolationBase> m_isolation;
};

}

#endif
