/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonSystemDescription.h"

#include <string>
#include <vector>

namespace MuonGM {

    extern std::vector<std::string> tokenize(std::string);

    MuonSystemDescription::MuonSystemDescription(std::string n)
        : barrelInnerRadius(0.), innerRadius(0.), outerRadius(0.), endcapFrontFace(0.), length(0.), barreLength(0.), barrelInterRadius(0.), extraZ(0.), extraR(0.), m_name(n) {}

} // namespace MuonGM
