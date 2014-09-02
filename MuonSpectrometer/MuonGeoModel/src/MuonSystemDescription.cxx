/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonSystemDescription.h"
#include <string>
#include <vector>

namespace MuonGM {

extern std::vector<std::string> tokenize(std::string);

MuonSystemDescription::MuonSystemDescription(std::string n):  
name(n)
{
}

} // namespace MuonGM


