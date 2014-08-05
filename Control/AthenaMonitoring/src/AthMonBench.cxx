/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthMonBench.h"
#include <cstring>
#include <stdio.h>

AthMonBench::TMem AthMonBench::currentVMem_ExcludingLWPools()
{
  long result = -1;
  FILE* file = fopen("/proc/self/status", "r");
  if (!file)
    return result;
  char line[128];
  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmSize:", 7) == 0) {
      std::stringstream s(&(line[7]));
      s >> result;
      result *= 1024;//NB: ~1K uncertainty
      result -= LWHistStats::getTotalPoolMemAllocated();
      break;
    }
  }
  fclose(file);
  return result;
}

std::ostream& operator << ( std::ostream& os, const AthMonBench& br) {
  if (br.valid())
    os << "deltaMemLW: "<<br.deltaMemLW_mb()<<" mb, "<<"deltaMemOther: "<<br.deltaMemOther_mb()<<" mb, deltaCPU: "<<br.deltaCPU_ms()<<" ms";
  else
    os <<" [no data]";
  return os;
}
