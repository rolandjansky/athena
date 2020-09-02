/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BLM_Wall.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BLMWALL_H
#define BLMWALL_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

  /** @class BLM_Wall

  Beam Loss Monitor wall builder
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */

class BLM_Wall
{
 public:
  GeoPhysVol* BuildLayerI(double thick, const GeoMaterial* material, bool hole);
  GeoPhysVol* BuildLayerII(double thick, const GeoMaterial* material);
  GeoPhysVol* BuildLayerIII(double thick, const GeoMaterial* material);
  GeoPhysVol* BuildLayerIV(double thick, const GeoMaterial* material);
  GeoPhysVol* BuildLayerV(double thick, const GeoMaterial* material);
  GeoPhysVol* BuildHolder(const GeoMaterial* material);
  GeoPhysVol* BuildScrew(double length, const GeoMaterial* material);
  GeoPhysVol* BuildBlock(double X, double Y, double Z, std::string logname, const GeoMaterial* material);
  GeoPhysVol* BuildClamp(const GeoMaterial* material);

  static constexpr double s_width = 18;
  static constexpr double s_length = 22;
  static constexpr double s_extended_length = 27;//must be more than 'length'
  static constexpr double s_hole_r = 1.85;
  static constexpr double s_hole_position = 2.5;
  static constexpr double s_holder_length = 33;
  static constexpr double s_holder_height = 10;
  static constexpr double s_holder_spacing = 4;
  static constexpr double s_holder_spacing_length = 4;
  static constexpr double s_holder_thickness = 3;
  static constexpr double s_clamp_length = 6;
  static constexpr double s_clamp_thickness = 2;

};

#endif
