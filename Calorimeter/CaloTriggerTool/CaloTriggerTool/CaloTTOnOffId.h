/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTONOFFID_H
#define CALOTRIGGERTOOL_CALOTTONOFFID_H

#include <vector>

/**
 *  @brief Structure definition of the LArTTOnOffIdMap
 */

struct CaloTTOnOffId_t
{
          short pn;
 unsigned short sampling;
 unsigned short region;
 unsigned short eta;
 unsigned short phi;
 unsigned short layer;

 unsigned short crate ;
 unsigned short module ;
 unsigned short submodule ;
 unsigned short channel ;

} ;

typedef std::vector<CaloTTOnOffId_t> CaloTTOnOffId;

#endif
