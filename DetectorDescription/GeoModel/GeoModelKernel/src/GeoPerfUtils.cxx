/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPerfUtils.h"
#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include <fstream>
int GeoPerfUtils::getMem() {
  int memSize = 0;
  {
    pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/status";
    std::ifstream memfile(procstream.str().c_str());
    

    std::string line;
    while ((memfile >> line)) {
      if (line=="VmSize:") {
	memfile >> line;
	std::istringstream istream(line);
	istream >> memSize;
      }
    }
  }
  return memSize;
}

int GeoPerfUtils::getCpu() {

    pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/stat";
    std::ifstream memfile(procstream.str().c_str());

    int nblancks=0;
    int uTime = 0;
    int sTime = 0;

    char line[256];
    while ((memfile >> line)) {
        nblancks++;

        if (nblancks==13) {
            memfile >> line;
            std::istringstream istream(line);
            istream >> uTime;

            memfile >> line;
            std::istringstream istream1(line);
            istream1 >> sTime;

            return uTime+sTime;
        }
    }    
    return 0;
}
