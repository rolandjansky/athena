/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_ReadCalibDataDefs_h
#define SCT_ReadCalibDataDefs_h

// Include STL stuff
#include <array>
#include <string>

#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

namespace SCT_ReadCalibChipDefs {
  const std::array<std::string, SCT_ConditionsData::N_NPTGAIN> nPtGainDbParameterNames{
    {"gainByChip", "gainRMSByChip", "offsetByChip", "offsetRMSByChip", "noiseByChip", "noiseRMSByChip"}
  };
  const std::array<std::string, SCT_ConditionsData::N_NPTGAIN> nPtGainParameterNames{
    {"GainByChip", "GainRMSByChip", "OffsetByChip", "OffsetRMSByChip", "NoiseByChip", "NoiseRMSByChip"}
  };
  const std::array<std::string, SCT_ConditionsData::N_NOISEOCC> noiseOccDbParameterNames{
    {"occupancyByChip", "occupancyRMSByChip", "offsetByChip", "noiseByChip"}
  };
  const std::array<std::string, SCT_ConditionsData::N_NOISEOCC> noiseOccParameterNames{
    {"OccupancyByChip", "OccupancyRMSByChip", "OffsetByChip", "NoiseByChip"}
  };
} // end of namespace

#endif // SCT_ReadCalibDataDefs_h
