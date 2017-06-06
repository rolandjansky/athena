/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NTAUSELECTOR_H_
#define NTAUSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good taus.
 */
class NTauSelector : public SignValueSelector {
public:
    explicit NTauSelector(const std::string& params);

    bool apply(const top::Event& event) const override;
};

}

#endif
