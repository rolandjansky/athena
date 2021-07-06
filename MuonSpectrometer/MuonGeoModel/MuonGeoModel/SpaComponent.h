/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SpaComponent_H
#define SpaComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {

    class SpaComponent : public StandardComponent {
      public:
        double maxwdy; // length from bottom to the max width of the CSL
                       // for CSC it is = dy
    };
} // namespace MuonGM

#endif
