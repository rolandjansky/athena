/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHV_H
#define CHV_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {
    class MYSQL;

    class CHV : public Technology {
      public:
        inline CHV(MYSQL& mysql, const std::string& s);
        double largeness;
        double height;
        double excent;
    };

    CHV::CHV(MYSQL& mysql, const std::string& s) : Technology(mysql, s), largeness(0.), height(0.), excent(0.) {}
} // namespace MuonGM

#endif
