/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DED_H
#define DED_H

#include "MuonGeoModel/Technology.h"

#include <string>

namespace MuonGM {

    class DED : public Technology {
      public:
        double AlThickness;
        double HoneyCombThickness;

        DED(std::string s);

        bool hasAlHoneyComb() { return false; };
        bool hasPaperHoneyComb() { return true; };
    };
} // namespace MuonGM

#endif
