/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CRO_H
#define CRO_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {
    class MYSQL;

    class CRO : public Technology {
      public:
        inline CRO(MYSQL& mysql, const std::string& s);
        double largeness;
        double height;
        double excent;
    };

    CRO::CRO(MYSQL& mysql, const std::string& s) : Technology(mysql, s), largeness(0.), height(0.), excent(0.) {}
} // namespace MuonGM

#endif
