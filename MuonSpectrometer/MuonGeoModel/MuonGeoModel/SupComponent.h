/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SupComponent_H
#define SupComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {

    class SupComponent : public StandardComponent {
      public:
        static double xAMDB0(const StandardComponent &c);
        static double yAMDB0(const StandardComponent &c);
        static double zAMDB0(const StandardComponent &c);
        static double bottomsizewrtAMDB0(const StandardComponent &c);
        static double topsizewrtAMDB0(const StandardComponent &c);
    };
} // namespace MuonGM

#endif
