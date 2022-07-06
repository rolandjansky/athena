/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_TrigConfMD5
#define TrigConfigSvc_TrigConfMD5

#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTMenu.h"


namespace TrigConf {

/**
  * @brief Function to compute a truncated MD5 hash for a JSON file.
  * Trigger keys must be uint32_t whereas a MD5 is 128 bit.
  * Used to generate a unique key for each MC JSON file to preserve
  * JSON distinguishability upon merging of AOD files between MC campaigns.
  */
  uint32_t truncatedHash(const DataStructure& dataStructure);

/**
  * @brief Function to compute a truncated MD5 hash for a JSON file.
  * Trigger keys must be uint32_t whereas a MD5 is 128 bit.
  * Used to generate a unique key for both a L1 and HLT menu, mimicking a SMK
  */
  uint32_t truncatedHash(const TrigConf::L1Menu& L1DataStructure, const TrigConf::HLTMenu& HLTDataStructure);

}

#endif