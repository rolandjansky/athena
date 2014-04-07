/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModelTest/PerfUtils.h"
#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include <fstream>
#include <iostream>

// copied and modified from GEOMODELKERNEL GeoPerfUtils

int PerfUtils::getMem() {
  int memSize = 0;
  {
    pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/status";
    std::ifstream memfile(procstream.str().c_str());
    
    
    char line[256];
    while ((memfile >> line)) {
      if (std::string(line)=="VmSize:") {
	memfile >> line;
	std::istringstream istream(line);
	istream >> memSize;
      }
    }
  }
  return memSize;
}

void PerfUtils::getCpu(int& uTime, int& sTime) {
    // return ms 
    pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/stat";
    //procstream << "/proc/" << pid << "/cpu";
    std::ifstream memfile(procstream.str().c_str());
    //std::cout<<memfile<<std::endl;
    std::string longString;
    memfile >> longString;
    
    //std::cout<<"File /proc/<pid>/stat content \n "<<longString<<std::endl;

    int nblancks=0;
    uTime = 0;
    sTime = 0;

    char line[256];
    while ((memfile >> line)) {
        //std::cout<<"while time = <"<<line<<">"<<std::endl;
        nblancks++;
        //std::cout<<"nBlancks = "<<nblancks<<std::endl;
        if (nblancks==13) {
            memfile >> line;
            std::istringstream istream(line);
            istream >> uTime;
            uTime = uTime*10;
            //std::cout<<"uTime = "<<uTime<<std::endl;
            memfile >> line;
            std::istringstream istream1(line);
            istream1 >> sTime;
            sTime = sTime*10;
            //std::cout<<"sTime = "<<sTime<<std::endl;
            return;
        }
    }

    
}
