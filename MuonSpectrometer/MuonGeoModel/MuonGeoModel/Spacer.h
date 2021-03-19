/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Spacer_H
#define Spacer_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/SpaComponent.h"

class GeoVPhysVol;

namespace MuonGM {

    class Spacer : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers

        Spacer(Component *s);
        GeoVPhysVol *build();
        GeoVPhysVol *build(int cutoutson);
        void print();

      private:
        SpaComponent m_component;
    };

} // namespace MuonGM

#endif
