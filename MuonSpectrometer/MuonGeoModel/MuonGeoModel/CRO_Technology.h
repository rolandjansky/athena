/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CRO_H
#define CRO_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {

    class CRO : public Technology {
      public:
        inline CRO(std::string s);
        double largeness;
        double height;
        double excent;
    };

    CRO::CRO(std::string s) : Technology(s), largeness(0.), height(0.), excent(0.) {}
} // namespace MuonGM

#endif
