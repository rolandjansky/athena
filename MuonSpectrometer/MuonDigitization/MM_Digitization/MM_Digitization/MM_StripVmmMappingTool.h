/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MMStripVmmMappingTool.h

#ifndef MMSTRIPVMMMAPPINGTOOL_H
#define MMSTRIPVMMMAPPINGTOOL_H
#include <string>
#include <tuple>

class electronics {
public:
    int elec(int stripNumber, std::string tech, int off_stationEta, int chMax);
};

#endif
