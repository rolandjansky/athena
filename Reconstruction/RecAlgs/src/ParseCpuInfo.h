/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RecAlgs_ParseCpuInfo_h
#define RecAlgs_ParseCpuInfo_h
#include <string>
#include <optional>

struct CpuInfo{
  std::string msg;
  unsigned int cpuId = 0;
  unsigned int cpuId_a = 0;
  unsigned int cpuId_b = 0;
};


std::optional<CpuInfo>
ParseCpuInfoFromFile(const std::string & filename);


#endif

