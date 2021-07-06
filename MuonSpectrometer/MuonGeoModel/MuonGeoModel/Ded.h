/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Ded_H
#define Ded_H

#include "MuonGeoModel/DedComponent.h"
#include "MuonGeoModel/DetectorElement.h"

class GeoVPhysVol;

namespace MuonGM {

    class Cutout;

    class Ded : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers

        Ded(Component *s);
        GeoVPhysVol *build();
        GeoVPhysVol *build(int cutoutson, std::vector<Cutout *>);
        void print();

      private:
        DedComponent *m_component;
    };
} // namespace MuonGM

#endif
