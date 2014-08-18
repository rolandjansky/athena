/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTONATTRID_H
#define CALOTRIGGERTOOL_CALOTTONATTRID_H

#include <vector>

/**
 *  @brief Structure definition of the LArTTOnAttrIdMap
 */

struct CaloTTOnAttrId_t
{
    unsigned short crate ;
    unsigned short module ;
    unsigned short submodule ;
    unsigned short channel ;

    unsigned int em_had;
    unsigned int pos_neg;
    unsigned int barrel_endcap_fcal;
    unsigned int module_type;
} ;

typedef std::vector<CaloTTOnAttrId_t> CaloTTOnAttrId;

#endif
