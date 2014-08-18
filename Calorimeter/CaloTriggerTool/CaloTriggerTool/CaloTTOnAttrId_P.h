/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTONATTRID_P_H
#define CALOTRIGGERTOOL_CALOTTONATTRID_P_H

#include <vector>
#include "CaloTriggerTool/CaloTTOnAttrId.h"

/**
 * @brief This is a database object holding the TT online-attribute map
 *
 * Its persistifiable version is the CaloTTOnAttrId class
 *
 * @author Damien Prieur
 * @author maintained by Damien Prieur
 *
 */

class CaloTTOnAttrId_P
{
public:

 /** Structure of the map */
 struct CaloTTOnAttrId_P_t
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
 typedef CaloTTOnAttrId_P_t __t;

 CaloTTOnAttrId_P (): m_version(0)  { }

 virtual ~CaloTTOnAttrId_P() { }


 std::vector<__t>  m_v;

 int m_version ;
};

#endif
