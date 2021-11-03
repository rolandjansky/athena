/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Csc_H
#define Csc_H

#include "MuonGeoModel/DetectorElement.h"

#include <vector>

class GeoFullPhysVol;

namespace MuonGM {

    class Component;
    class CscComponent;
    class CscMultiLayer;
    class Cutout;
    class MYSQL;

    class Csc : public DetectorElement {

      public:
        double width;
        double length;
        double thickness;
        double longWidth; // for trapezoidal layers
        double excent;    // for csc layers
        double physicalLength;
        double maxwLength;
        double upWidth;

        int index;

        Csc(const MYSQL& mysql, Component *s);
        ~Csc();
        CscMultiLayer *layer;
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              const MYSQL& mysql,
                              int minimalgeo);
        GeoFullPhysVol *build(const StoredMaterialManager& matManager,
                              const MYSQL& mysql,
                              int minimalgeo, int cutoutson,
                              const std::vector<Cutout *>&);
        virtual void print() override;

      private:
        CscComponent *m_component;
        Csc &operator=(const Csc &right);
        Csc(const Csc &);
    };

} // namespace MuonGM

#endif
