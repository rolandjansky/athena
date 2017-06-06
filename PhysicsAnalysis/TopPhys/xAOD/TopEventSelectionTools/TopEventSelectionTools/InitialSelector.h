/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INITIALSELECTOR_H_
#define INITIALSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief A cut that accepts all events, more useful than you might think.
 *
 * Very simple tool that accepts all events so you can see the starting
 * number of events in the cutflow.
 */
class InitialSelector : public EventSelectorBase {
    /**
     * @brief Accept every event
     *
     * @return Always true, so the event is accepted.
     */
    virtual bool apply(const top::Event&) const override;

    /**
     * @brief The name printed in the cutflow.
     *
     * @return The word INITIAL.
     */
    virtual std::string name() const override;
};

}

#endif
