/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IP3DSV1SELECTOR_H_
#define IP3DSV1SELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief Should count how many jets have an IP3DSV1 weight above some value.
 * The user is expected to input something a bit like IP3DSV1 1.8 >= 2, to get
 * two jets tagged with 1.8.  Maybe in the future we can make this easier to use
 * by the person just saying 70% or something for the 0.7 WP?
 */
class IP3DSV1Selector : public SignValueSelector {
public:
    explicit IP3DSV1Selector(const std::string& params);

    virtual bool apply(const top::Event& event) const override;
};

}

#endif
