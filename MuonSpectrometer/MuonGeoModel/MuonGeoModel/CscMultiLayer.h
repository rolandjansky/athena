/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscMultiLayer_H
#define CscMultiLayer_H

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"

namespace MuonGM {

    class Cutout;

    class CscMultiLayer : public DetectorElement {

      public: // data members
        int nrOfLayers;

        double width;
        double longWidth;
        double upWidth;

        double excent;
        double length;
        double physicalLength;
        double maxwLength;

        double thickness;
        double cscthickness;
        double dim[8];

      public: // methods
        CscMultiLayer(std::string n);
        GeoVPhysVol *build();
        GeoVPhysVol *build(int cutoutson, std::vector<Cutout *> vcutdef);
        void print();
    };

} // namespace MuonGM

#endif
