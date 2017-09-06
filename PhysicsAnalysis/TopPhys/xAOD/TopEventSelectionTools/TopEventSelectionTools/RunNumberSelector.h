/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RUNNUMBERSELECTOR_H_
#define RUNNUMBERSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"


namespace top {
class TopConfig;

/**
 * @brief Apply a cut on the (random) run number.
 */
class RunNumberSelector : public SignValueSelector {
public:
    explicit RunNumberSelector(const std::string& params,std::shared_ptr<top::TopConfig> config);

    bool apply(const top::Event& event) const override;

private:
    std::shared_ptr<top::TopConfig> m_config;
};

}

#endif
