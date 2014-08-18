/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTPPMRXID_P_H
#define CALOTRIGGERTOOL_CALOTTPPMRXID_P_H

#include <vector>
#include "CaloTriggerTool/CaloTTPpmRxId.h"

/**
 * @brief This is a database object holding the TT PPM to RX channels map
 *
 * Its persistifiable version is the CaloTTPpmRxId class
 *
 * @author Damien Prieur
 * @author maintained by Damien Prieur
 *
 */

class CaloTTPpmRxId_P
{
public:

 /** Structure of the map */
 struct CaloTTPpmRxId_P_t
 {
 unsigned short ppm_crate;
 unsigned short ppm_module;
 unsigned short ppm_subModule;
 unsigned short ppm_channel;

 unsigned short rx_crate ;
 unsigned short rx_module ;
 unsigned short rx_inputConn;
 unsigned short rx_inputPair;
 unsigned short rx_outputConn;
 unsigned short rx_outputPair;

 };
 typedef CaloTTPpmRxId_P_t __t;

 CaloTTPpmRxId_P (): m_version(0)  { }

 virtual ~CaloTTPpmRxId_P() { }


 std::vector<__t>  m_v;

 int m_version ;
};

#endif
