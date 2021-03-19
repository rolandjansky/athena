/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MicromegasComponent.h"

namespace MuonGM {

    MicromegasComponent::MicromegasComponent() : StandardComponent(), subType("") {}

    std::ostream &operator<<(std::ostream &os, const MicromegasComponent &c) {
        os << "Component " << c.name << std::endl;
        return os;
    }

} // namespace MuonGM
