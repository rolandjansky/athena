/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Micromegas_H
#define Micromegas_H

#include "MuonGeoModel/DetectorElement.h"

#include <vector>

class GeoFullPhysVol;

namespace MuonGM {

    class Cutout;
    class Component;
    class MicromegasComponent;

    class Micromegas : public DetectorElement {

      public:
        double width;
        double length;
        double thickness = 0.0;
        double longWidth; // for trapezoidal layers
        int index;

        Micromegas(Component *s);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              int minimalgeo);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              int minimalgeo, int cutoutson,
                              const std::vector<Cutout *>&);
        virtual void print() override;

      private:
        MicromegasComponent *m_component;
    };

} // namespace MuonGM

#endif
