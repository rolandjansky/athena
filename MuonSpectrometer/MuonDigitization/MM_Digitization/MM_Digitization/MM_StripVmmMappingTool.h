/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//MMStripVmmMappingTool.h

#ifndef MMSTRIPVMMMAPPINGTOOL_H
#define MMSTRIPVMMMAPPINGTOOL_H
#include <string>
#include <tuple>
using namespace std;

class electronics{
public:
int elec(int stripNumber, string tech, int off_stationEta, int chMax);
};







#endif
