/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHV_H
#define CHV_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {

    class CHV : public Technology {
      public:
        inline CHV(std::string s);
        double largeness;
        double height;
        double excent;
    };

    CHV::CHV(std::string s) : Technology(s), largeness(0.), height(0.), excent(0.) {}
} // namespace MuonGM

#endif
