/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SPA_H
#define SPA_H

#include "MuonGeoModel/Technology.h"

namespace MuonGM {

    class SPA : public Technology {
      public:
        inline SPA(std::string s);
    };

    SPA::SPA(std::string s) : Technology(s) {}
} // namespace MuonGM

#endif
