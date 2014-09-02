/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_H
#define MDT_H

#include <string>
#include "MuonGeoModel/Technology.h"

namespace MuonGM {

class MDT: public Technology {
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

    MDT(std::string s):Technology(s) {}
};
} // namespace MuonGM


#endif
