/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSpacerComponent_H
#define MMSpacerComponent_H

#include "MuonGeoModel/StandardComponent.h"

#include <iostream>

namespace MuonGM {

    class MMSpacerComponent : public StandardComponent {

      public:
        MMSpacerComponent();
        friend std::ostream &operator<<(std::ostream &os, const MMSpacerComponent &c);
    };

} // namespace MuonGM

#endif
