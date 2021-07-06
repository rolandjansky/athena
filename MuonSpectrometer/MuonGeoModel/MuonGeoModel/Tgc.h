/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Tgc_H
#define Tgc_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/TgcComponent.h"

class GeoFullPhysVol;

namespace MuonGM {

    class Cutout;

    class Tgc : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers
        double irad;
        double orad;
        double dphi;
        int index;

        Tgc(Component *s);
        GeoFullPhysVol *build(int minimalgeo);
        GeoFullPhysVol *build(int minimalgeo, int cutoutson, std::vector<Cutout *>);
        void print();

      private:
        TgcComponent *m_component;
    };

} // namespace MuonGM

#endif
