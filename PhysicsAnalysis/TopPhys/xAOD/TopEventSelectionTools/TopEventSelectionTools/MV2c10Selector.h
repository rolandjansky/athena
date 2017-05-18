/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MV2C10SELECTOR_H_
#define MV2C10SELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Should count how many jets have an mv2c10 weight above some value.
 */
class MV2c10Selector : public SignValueSelector {
public:
    explicit MV2c10Selector(const std::string& params);

    /**
     * @brief Look at the event and check if the required number of tagged jets
     * are there.
     *
     * If MV2C10 is not in the input file (it is for MC15) then you need to
     * return false, since no events pass the cut.  If it is there we retrieve
     * it and check the value against the user specified cut.
     *
     * @param event The event to analyse
     * @return True if the event passes the cut, false otherwise.
     */
    bool apply(const top::Event& event) const override;
};

}

#endif
