/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  static double s_width;
  static double s_length;
  static double s_extended_length;
  static double s_hole_r;
  static double s_hole_position;
  static double s_holder_length;
  static double s_holder_height;
  static double s_holder_spacing;
  static double s_holder_spacing_length;
  static double s_holder_thickness;
  static double s_clamp_length;
  static double s_clamp_thickness;

};

#endif
