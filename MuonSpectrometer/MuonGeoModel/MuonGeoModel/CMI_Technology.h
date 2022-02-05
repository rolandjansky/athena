/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMI_H
#define CMI_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {
    class MYSQL;

    class CMI : public Technology {
      public:
        inline CMI(MYSQL& mysql, const std::string& s);
        double largeness;
        double height;
        double excent;
    };

    CMI::CMI(MYSQL& mysql, const std::string& s) : Technology(mysql, s), largeness(0.), height(0.), excent(0.) {}
} // namespace MuonGM

#endif
