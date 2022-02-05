/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DED_H
#define DED_H

#include "MuonGeoModel/Technology.h"

#include <string>

namespace MuonGM {

    class MYSQL;

    class DED : public Technology {
      public:
        double AlThickness;
        double HoneyCombThickness;

        DED(MYSQL& mysql, std::string s);

        bool hasAlHoneyComb() { return false; };
        bool hasPaperHoneyComb() { return true; };
    };
} // namespace MuonGM

#endif
