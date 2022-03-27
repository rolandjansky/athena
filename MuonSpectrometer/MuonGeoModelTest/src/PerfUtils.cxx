/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModelTest/PerfUtils.h"

#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

// copied and modified from GEOMODELKERNEL GeoPerfUtils

int PerfUtils::getMem() {
    int memSize = 0;
    pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/status";
    std::ifstream memfile(procstream.str());
    std::string line;
    if (memfile.is_open()) {
        while (std::getline(memfile, line)) {
            std::size_t pos = line.find("VmSize:");
            if (pos != std::string::npos) {
                std::string myStr = line.substr(pos + 7);
                myStr.erase(std::remove(myStr.begin(), myStr.end(), ' '), myStr.end());
                myStr.erase(std::remove(myStr.begin(), myStr.end(), '\t'), myStr.end());
                memSize = std::atoi(myStr.substr(0, myStr.find("kB")).c_str());
            }
        }
        memfile.close();
    }
    return memSize;
}

void PerfUtils::getCpu(int& uTime, int& sTime) {
    // return ms
    pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/stat";
    std::ifstream memfile(procstream.str());
    std::string longString;
    memfile >> longString;

    int nblancks = 0;
    uTime = 0;
    sTime = 0;

    char line[256];
    while ((memfile >> line)) {
        nblancks++;
        if (nblancks == 13) {
            memfile >> line;
            std::istringstream istream(line);
            istream >> uTime;
            uTime = uTime * 10;
            memfile >> line;
            std::istringstream istream1(line);
            istream1 >> sTime;
            sTime = sTime * 10;
            return;
        }
    }
}
