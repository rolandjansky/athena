/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NTAUTIGHTSELECTOR_H_
#define NTAUTIGHTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good taus.
 */
class NTauTightSelector : public SignValueSelector {
public:
    explicit NTauTightSelector(const std::string& params);

    bool apply(const top::Event& event) const override;
};

}

#endif
