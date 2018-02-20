/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NOBADMUONSELECTOR_H_
#define NOBADMUONSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

#include "AsgTools/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace top {
  class TopConfig;

/**
 * @brief A tool that selects events containing no bad muons
 * https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool#is_BadMuon_Flag_Event_Veto
 *
 */
  class NoBadMuonSelector : public EventSelectorBase {
public:

    /**
     * @brief Event selection if bad muon detected using the official muon selector tool.
     *
     * Call the muon selector tool and reject the event if any of the muons that
     * passed the event selection are marked as bad.
     */
    explicit NoBadMuonSelector(std::shared_ptr<top::TopConfig> m_config);

    /**
     * @brief Events with a muon flagged bad are rejected
     *
     * @param event The event in question
     * @return False if a bad muon is found
     */
    bool apply(const top::Event& event) const override;

    /**
     * @brief The name of the tool.
     *
     * @return The name of the tool for humans to see.
     */
    std::string name() const override;

private:
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;

    std::shared_ptr<top::TopConfig> m_config;
};

}

#endif
