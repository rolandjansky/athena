/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LBI_H
#define LBI_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {

    class LBI : public Technology {
      public:
        inline LBI(std::string s);
        double height;
        float lowerThickness;
        float yShift;
    };

    LBI::LBI(std::string s) : Technology(s), height(0.), lowerThickness(0.), yShift(0.) {}
} // namespace MuonGM

#endif
