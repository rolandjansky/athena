/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTAVESUPPORT_H
#define GEOPIXELSTAVESUPPORT_H

#include "PixelGeoModel/GeoVPixelFactory.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

class GeoPixelStaveSupport :  public GeoVPixelFactory {

public:  
  virtual GeoVPhysVol* getPhysVol() const = 0;
  virtual const HepGeom::Transform3D & transform() const = 0;
  virtual double thicknessP() const = 0;
  virtual double thicknessN() const = 0;
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape(double) = 0;
  virtual GeoVPhysVol* getEndblockEnvelopShape(int) const = 0;
  virtual GeoTransform* getEndblockEnvelopShapeTrf(int) const =0;
  virtual double getEndblockZpos() const = 0;
  virtual double getServiceZpos() const =0;
  virtual double getEndblockLength() const = 0;

  virtual void computeStaveEnvelopTransformAndSize(double moduleThickN,double moduleThickP, double moduleWidth,
						   double moduleThickN3D,double moduleThickP3D, double moduleWidth3D) =0;
  virtual int PixelNModule() const = 0;
  virtual int PixelNPlanarModule() const = 0;
  virtual int PixelN3DModule() const = 0;

};

#endif


