/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonSystemDescription.h"

#include <string>
#include <utility>
#include <vector>

namespace MuonGM {

    MuonSystemDescription::MuonSystemDescription(std::string n)
        : barrelInnerRadius(0.), innerRadius(0.), outerRadius(0.), endcapFrontFace(0.), length(0.), barreLength(0.), barrelInterRadius(0.), extraZ(0.), extraR(0.), m_name(std::move(n)) {}

} // namespace MuonGM
