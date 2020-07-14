/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSIMPLESTAVESUPPORT_H
#define GEOPIXELSIMPLESTAVESUPPORT_H

#include "GeoPixelStaveSupport.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"

class GeoPixelSimpleStaveSupport : public GeoPixelStaveSupport {

public:  
  GeoPixelSimpleStaveSupport();
  virtual ~GeoPixelSimpleStaveSupport();
  virtual GeoVPhysVol* Build();
  virtual GeoVPhysVol* getPhysVol ATLAS_NOT_THREAD_SAFE () const {return m_physVol;}
  virtual const GeoTrf::Transform3D & transform() const {return m_transform;}
  virtual double thicknessP() const {return m_thicknessP;}
  virtual double thicknessN() const {return m_thicknessN;}
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape(double) { return 0;}
  virtual GeoVPhysVol* getEndblockEnvelopShape(int) const { return 0;}
  virtual GeoTransform* getEndblockEnvelopShapeTrf(int) const {return 0;}
  virtual double getEndblockZpos() const { return 0.; }
  virtual double getServiceZpos() const { return 0; }
  virtual double getEndblockLength() const { return 0.; }

  virtual void computeStaveEnvelopTransformAndSize(double ,double, double, double, double, double){};

  virtual int PixelNModule() const {return 0;} 
  virtual int PixelNPlanarModule() const {return 0;}
  virtual int PixelN3DModule() const {return 0;}

private:
  GeoVPhysVol* m_physVol;
  GeoTrf::Transform3D m_transform;
  double m_thicknessP;
  double m_thicknessN;
};

#endif


