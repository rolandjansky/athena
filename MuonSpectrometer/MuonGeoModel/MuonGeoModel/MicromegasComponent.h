/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MicromegasComponent_H
#define MicromegasComponent_H

#include "MuonGeoModel/StandardComponent.h"

#include <iostream>
#include <string>

namespace MuonGM {

    class MicromegasComponent : public StandardComponent {

      public:
        MicromegasComponent();
        friend std::ostream &operator<<(std::ostream &os, const MicromegasComponent &c);

        std::string subType;
    };

} // namespace MuonGM

#endif
