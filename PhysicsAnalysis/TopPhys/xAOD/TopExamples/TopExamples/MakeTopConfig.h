/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_NEXTGEN_MAKETOPCONFIG_H
#define ANALYSISTOP_NEXTGEN_MAKETOPCONFIG_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief MakeTopConfig - it makes a top::TopConfig from the metadata
 *
 *
 **/

#include <memory>

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"

namespace top {
  class TopConfig;
  class MakeTopConfig: public asg::AsgMetadataTool {
  public:
    explicit MakeTopConfig(const std::string& name);

    std::shared_ptr<top::TopConfig> makeTopConfig();
  };
}
#endif
