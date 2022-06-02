/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ParseCpuInfo.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdexcept>


std::optional<CpuInfo>
ParseCpuInfoFromFile(const std::string & filename){
  CpuInfo result;
  FILE* pFile = fopen(filename.c_str(), "r");
  if (!pFile) {
    const std::string errmsg="File "+filename+" could not be found";
    throw std::runtime_error(errmsg);
  }
  
  std::string vendor("none");
  std::string stepping("none");
  std::string cpuFamily("none");
  std::string model("none");
      
  unsigned int numProcessors(0);
  char line[1024];
  while(fgets(line, 1024, pFile) != NULL){
    if (strncmp(line, "processor",   9) == 0) numProcessors++;
    if (strncmp(line, "vendor_id",   9) == 0) vendor=line;
    if (strncmp(line, "stepping",    8) == 0) stepping=line;
    if (strncmp(line, "cpu family", 10) == 0) cpuFamily=line;
    if (strncmp(line, "model\t"   ,  6) == 0) model=line;
  }
  fclose(pFile);
  //
  size_t found=vendor.find(':');
  if (found!=std::string::npos)
  vendor.erase(0,found+2);
  
  vendor.erase(vendor.size()-1);
  stepping.erase(stepping.size()-1);
  cpuFamily.erase(cpuFamily.size()-1);
  model.erase(model.size()-1);
  result.msg= "number of CPUs found: " +std::to_string(numProcessors) + " of type " + vendor + " "
		    + cpuFamily + " " + model + " " + stepping;
  //compress all strings/numbers into 2 uints
  result.cpuId_a = 0xFFFFFFFF;
  if ( vendor.length() == 12 ){
	  result.cpuId_a =
	    0x01000000u * vendor[0] +
	    0x00010000u * vendor[1] +
	    0x00000100u * vendor[10] +
	    0x00000001u * vendor[11]; 
		}
      
  result.cpuId_b = 0;
  unsigned int s(0);
  s=0xFFu;
  if (stepping.compare("none") != 0) sscanf(stepping.c_str(),"stepping\t: %80u",&s);
  if ( s > 255 ) s = 255;
  result.cpuId_b += 0x00000100u * s;

  s=0xFFu;
  if (cpuFamily.compare("none") != 0) sscanf(cpuFamily.c_str(),"cpu family\t: %80u",&s);
  if ( s > 255u ) s = 255u;
  result.cpuId_b += 0x01000000u * s;

  s=0xFFu;
  if (model.compare("none") != 0) sscanf(model.c_str(),"model\t: %80u",&s);
  if ( s > 255u ) s = 255u;
  result.cpuId_b += 0x00010000u * s;

  s = numProcessors;
  if ( s > 255u ) s = 255u;
  result.cpuId_b += s;

  result.cpuId=1u;
  return result;
}