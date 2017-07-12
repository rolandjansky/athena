/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKESMMCONFIG_H_
#define FAKESMMCONFIG_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

class TFile;

namespace top {

/**
 * @brief An example of how to quickly make some plots at a certain point in
 * the cutflow.
 */
class FakesMMConfigs : public EventSelectorBase {
public:
    /**
     * @brief Setup the matrix-method configurations.
     *
     * @param configs The MM weights configurations separated by spaces.
     */
    FakesMMConfigs(const std::string& configs);

    /**
     * @return True because it doesn't select any events.
     */
    virtual bool apply(const top::Event& event) const override;

    /**
     * @brief Return the name for the cutflow table.
     *
     * @return The word FAKESMMCONFIGS.
     */
    std::string name() const override;

    /**
     * @return The MM weight configurations.
     */
    std::vector<std::string> configurations() const {return m_configurations;};

private:
    // Nominal hash value
    std::vector<std::string> m_configurations;
};

}

#endif
