/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/RunNumberSelector.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

namespace top {

RunNumberSelector::RunNumberSelector(const std::string& params,std::shared_ptr<top::TopConfig> config) :
            SignValueSelector("RUN_NUMBER", params) {
  m_config = config;
}

bool RunNumberSelector::apply(const top::Event& event) const {
    unsigned int number = 0;
    if (!m_config->isMC())
        number = event.m_info -> runNumber();
    else {
        if (event.m_info->isAvailable<unsigned int>("RandomRunNumber"))
            number = event.m_info->auxdataConst<unsigned int>("RandomRunNumber");
        else {
          std::cout << "RunNumberSelector: can't find random Run Number - did you setup the pile-up reweighting tool?" << std::endl;
          exit(1);
        }
    }

    return checkInt(number, value());
}


}
