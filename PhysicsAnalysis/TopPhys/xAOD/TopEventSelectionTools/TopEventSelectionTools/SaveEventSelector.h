/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAVEEVENTSELECTOR_H_
#define SAVEEVENTSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief Save this event in the output file.
 *
 * This is special.  Not only will it mark the event as passing the event
 * selection, but it will also write out the selected objects to the output
 * file in the CollectionTree.
 */
class SaveEventSelector : public EventSelectorBase {
    /**
     * @brief Accept every event
     *
     * @return Always true, so the event is accepted.
     */
    bool apply(const top::Event&) const override;

    /**
     * @brief The name printed in the cutflow.  The name is extra special for
     * this because it is used to check if we should write-out the event too.
     *
     * @return The word SAVE.
     */
    std::string name() const override;
};

}

#endif
