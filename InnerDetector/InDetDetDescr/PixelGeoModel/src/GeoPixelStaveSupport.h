/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTAVESUPPORT_H
#define GEOPIXELSTAVESUPPORT_H

#include "GeoVPixelFactory.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

class ATLAS_NOT_THREAD_SAFE GeoPixelStaveSupport :  public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.

public:  
  virtual GeoVPhysVol* getPhysVol ATLAS_NOT_THREAD_SAFE () const = 0; // const method returns non-const pointer.
  virtual const GeoTrf::Transform3D & transform() const = 0;
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


