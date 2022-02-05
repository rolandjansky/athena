/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Component.h"

namespace MuonGM {

    Component::Component()
      : name ("XXXY"),
        dx1 (0),
        dx2 (0),
        dy (0)
    {
    }

    double Component::GetThickness() const { return 0; }

} // namespace MuonGM
