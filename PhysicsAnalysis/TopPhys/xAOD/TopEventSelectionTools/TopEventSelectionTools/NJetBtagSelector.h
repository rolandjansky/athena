/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NJETBTAGSELECTOR_H_
#define NJETBTAGSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {
  class TopConfig;

/**
 * @brief A tool that selects events with a certain number of good jets.
 */
class NJetBtagSelector : public SignValueSelector {
public:
    explicit NJetBtagSelector(const std::string& params,std::shared_ptr<top::TopConfig> config);

    bool apply(const top::Event& event) const override;
};

}

#endif
