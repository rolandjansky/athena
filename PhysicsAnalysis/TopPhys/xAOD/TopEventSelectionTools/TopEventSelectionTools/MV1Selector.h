/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MV1SELECTOR_H_
#define MV1SELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Should count how many jets have an mv1 weight above some value.
 * mv1 Seem to not be filled yet though.
 */
class MV1Selector : public SignValueSelector {
public:
    explicit MV1Selector(const std::string& params);

    bool apply(const top::Event& event) const override;
};

}

#endif
