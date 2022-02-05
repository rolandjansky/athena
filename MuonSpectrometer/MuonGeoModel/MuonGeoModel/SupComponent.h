/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SupComponent_H
#define SupComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {
    class MYSQL;

    class SupComponent : public StandardComponent {
      public:
        static double xAMDB0(const MYSQL& mysql, const StandardComponent &c);
        static double yAMDB0(const MYSQL& mysql, const StandardComponent &c);
        static double zAMDB0(const MYSQL& mysql, const StandardComponent &c);
        static double bottomsizewrtAMDB0(const MYSQL& mysql, const StandardComponent &c);
        static double topsizewrtAMDB0(const MYSQL& mysql, const StandardComponent &c);
    };
} // namespace MuonGM

#endif
