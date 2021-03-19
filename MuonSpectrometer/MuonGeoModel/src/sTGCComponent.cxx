/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/sTGCComponent.h"

namespace MuonGM {

    sTGCComponent::sTGCComponent() : StandardComponent(), yCutout(0.), subType("") {}

    std::ostream &operator<<(std::ostream &os, const sTGCComponent &c) {
        os << "Component " << c.name << std::endl;
        return os;
    }

} // namespace MuonGM
