/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTONOFFID_P_H
#define CALOTRIGGERTOOL_CALOTTONOFFID_P_H

#include <vector>
#include "CaloTriggerTool/CaloTTOnOffId.h"

/**
 * @brief This is a database object holding the TT online-offline map
 *
 * Its persistifiable version is the CaloTTOnOffId class
 *
 * @author Damien Prieur
 * @author maintained by Damien Prieur
 *
 */

class CaloTTOnOffId_P
{
public:

 /** Structure of the map */
 struct CaloTTOnOffId_P_t
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

 };
 typedef CaloTTOnOffId_P_t __t;

 CaloTTOnOffId_P (): m_version(0)  { }

 virtual ~CaloTTOnOffId_P() { }


 std::vector<__t>  m_v;

 int m_version ;
};

#endif
