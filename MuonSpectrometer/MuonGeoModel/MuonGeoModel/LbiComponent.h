/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LbiComponent_H
#define LbiComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {

    class LbiComponent : public StandardComponent {
      public:
        std::string associated_CMIsubtype;
    };
} // namespace MuonGM

#endif
