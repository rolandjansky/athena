/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LBI_H
#define LBI_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {
    class MYSQL;

    class LBI : public Technology {
      public:
        inline LBI(MYSQL& mysql, const std::string& s);
        double height;
        float lowerThickness;
        float yShift;
    };

    LBI::LBI(MYSQL& mysql, const std::string& s) : Technology(mysql, s), height(0.), lowerThickness(0.), yShift(0.) {}
} // namespace MuonGM

#endif
