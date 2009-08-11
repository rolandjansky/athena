/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCHAMBERNAMECONVERTER_H
#define MUONCHAMBERNAMECONVERTER_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

std::string convertChamberName(int, int, int, std::string); 

//std::vector<int> convertChamberName(std::string HarwareChamberName , std::string HarwareChamberType , std::string ChamberType);

#endif
