/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MakeTopConfig.h 681944 2015-07-09 21:59:27Z morrisj $
#ifndef ANALYSISTOP_NEXTGEN_MAKETOPCONFIG_H
#define ANALYSISTOP_NEXTGEN_MAKTOPCONFIG_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief MakeTopConfig - it makes a top::TopConfig from the metadata
 *
 * $Revision: 681944 $
 * $Date: 2015-07-09 23:59:27 +0200 (Thu, 09 Jul 2015) $
 *
 *
 **/

#include <memory>

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"

namespace top{
  class TopConfig;
  class MakeTopConfig : public asg::AsgMetadataTool {
    public:
      explicit MakeTopConfig( const std::string& name );
      
      std::shared_ptr<top::TopConfig> makeTopConfig();
    
  };
}
#endif
