/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRINTEVENTSELECTOR_H_
#define PRINTEVENTSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief Print some information about the event to the screen using cout.
 *
 * Prints the run number, event number, mu, and lists of 4-vectors for good
 * electrons, muons and jets and the event missing ET.
 */
class PrintEventSelector : public EventSelectorBase {
    /**
     * @brief For each event print some information about it to the terminal.
     *
     * @return Also marks the event as passed.
     */
    bool apply(const top::Event& event) const override;

    /**
     * @brief The word PRINT.
     */
    std::string name() const override;
};

}

#endif
