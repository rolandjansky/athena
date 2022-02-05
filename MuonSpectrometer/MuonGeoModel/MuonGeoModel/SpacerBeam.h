/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SpacerBeam_H
#define SpacerBeam_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/StandardComponent.h"

class GeoVPhysVol;

namespace MuonGM {
    class MYSQL;

    class SpacerBeam : public DetectorElement {
      public:
        SpacerBeam(const MYSQL& mysql, Component *s);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           bool);
        GeoVPhysVol *build(const StoredMaterialManager& matManager,
                           int cutoutson, bool);
        virtual void print() override;

        double width;
        double length;
        double thickness;
        double lowerThickness;
        double longWidth; // for trapezoidal layers
        double height;    // web of I-beam (or height of box)
        double largeness; // flange width of I-beam (CHV, CRO, CMI)
        double excent;    // angle of beams in trapezoidal chambers

      private:
        StandardComponent m_component;
        double m_cy;           // y coordinate (parallel to tube length) of component
        double m_hole_pos1;    // Location of 1st LB hole in cross beam
        double m_hole_pos2;    // Location of 2nd LB hole in cross beam
        double m_lb_height;    // Height of LB hole
        double m_lb_width;     // Width of LB hole
        double m_cross_excent; // angle of cross beams needed by LB
    };

} // namespace MuonGM

#endif
