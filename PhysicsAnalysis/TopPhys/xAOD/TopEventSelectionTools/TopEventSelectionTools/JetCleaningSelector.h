/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCLEANINGSELECTOR_H_
#define JETCLEANINGSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "JetInterface/IJetSelector.h"
#include "JetSelectorTools/IEventCleaningTool.h"

namespace top {
  class TopConfig;

/**
 * @brief Event selection using the official jet cleaning tool.
 *
 * Hurray! The jet cleaning tool is one one of the few that exists in xAOD
 * format already)
 *
 * Call the jet cleaning tool and reject the event if any of the jets that
 * passed the event selection are marked as bad.  Probably should check that
 * this is okay after overlap removal.
 */
class JetCleaningSelector : public EventSelectorBase {
public:
    /**
     * @brief Setup the jet group's cleaning tool.
     *
     * The cutlevel is specified in the constructor as a little bit of text.
     *
     * @param level How harsh you want the cleaning to be.  For example
     * MediumBad.
     */
    explicit JetCleaningSelector(const std::string& level, std::shared_ptr<top::TopConfig> config);

    /**
     * @brief Only accept the event if the tool says all the jets are okay.
     *
     * @return True to accept the event, false otherwise.
     */
    bool apply(const top::Event& event) const override;

    /**
     * @brief Print the name of the tool and level.
     *
     * @return The name of the tool and the level that it has been configured
     * at.
     */
    std::string name() const override;

private:
    ///The jet cleaning tools
    ToolHandle<IJetSelector> m_jetCleaningToolLooseBad;
    ToolHandle<IJetSelector> m_jetCleaningToolTightBad;    

    ///The jet event cleaning tools
    ToolHandle<ECUtils::IEventCleaningTool> m_jetEventCleaningToolLooseBad;
    ToolHandle<ECUtils::IEventCleaningTool> m_jetEventCleaningToolTightBad;

    ///The level to configure to, so we can print it in the name.
    std::string m_level;
    bool m_useLooseBad;

    std::shared_ptr<top::TopConfig> m_config;
};

}

#endif
