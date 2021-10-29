/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SPA_H
#define SPA_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {
    class MYSQL;

    class SPA : public Technology {
      public:
        inline SPA(MYSQL& mysql, const std::string& s);
    };

    SPA::SPA(MYSQL& mysql, const std::string& s) : Technology(mysql, s) {}
} // namespace MuonGM

#endif
