/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CTP/CTPConstants.h"

namespace LVL1CTP {

  const int CTPConstants::MAXIMUM_ITEM_COUNT      = 256;  // This is limited by the CTP design
  const int CTPConstants::MAXIMUM_THRESHOLD_COUNT = 54;   // This is just the current number...

  const std::string CTPConstants::DEFAULT_MuonConfigLocation = "/Run/MuonTrigConfig";
  const std::string CTPConstants::DEFAULT_CaloConfigLocation = "/Run/CaloTrigConfig";
  const std::string CTPConstants::DEFAULT_JEConfigLocation   = "/Run/JetEnergyTrigConfig";

} // namespace LVL1CTP
