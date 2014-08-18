/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTPPMRXID_H
#define CALOTRIGGERTOOL_CALOTTPPMRXID_H

#include <vector>

/**
 *  @brief Structure definition of the CaloTTPpmRxIdMap
 */

struct CaloTTPpmRxId_t
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

} ;

typedef std::vector<CaloTTPpmRxId_t> CaloTTPpmRxId;

#endif
