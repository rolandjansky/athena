/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_H
#define MDT_H

#include "MuonGeoModel/Technology.h"

#include <string>

namespace MuonGM {

    class MDT : public Technology {
      public:
        int numOfLayers;
        double pitch;
        double innerRadius;
        double totalThickness;
        double tubeDeadLength;
        double tubeEndPlugLength;
        double tubeWallThickness;

        double y[4];
        double x[4];

        MDT(std::string s) : Technology(s), numOfLayers(0), pitch(0.), innerRadius(0.), totalThickness(0.), tubeDeadLength(0.), tubeEndPlugLength(0.), tubeWallThickness(0.) {}
    };
} // namespace MuonGM

#endif
