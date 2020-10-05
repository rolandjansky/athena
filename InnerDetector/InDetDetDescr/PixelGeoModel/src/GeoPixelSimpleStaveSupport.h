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
  GeoPixelSimpleStaveSupport(InDetDD::PixelDetectorManager* ddmgr,
                             PixelGeometryManager* mgr);
  virtual ~GeoPixelSimpleStaveSupport();
  virtual GeoVPhysVol* Build() override;
  virtual GeoVPhysVol* getPhysVol() override {return m_physVol;}
  virtual const GeoTrf::Transform3D & transform() const override {return m_transform;}
  virtual double thicknessP() const override {return m_thicknessP;}
  virtual double thicknessN() const override {return m_thicknessN;}
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape(double) override { return 0;}
  virtual GeoVPhysVol* getEndblockEnvelopShape(int) const override { return 0;}
  virtual GeoTransform* getEndblockEnvelopShapeTrf(int) const override {return 0;}
  virtual double getEndblockZpos() const override { return 0.; }
  virtual double getServiceZpos() const override { return 0; }
  virtual double getEndblockLength() const override { return 0.; }

  virtual void computeStaveEnvelopTransformAndSize(double ,double, double, double, double, double) override {};

  virtual int PixelNModule() const override {return 0;} 
  virtual int PixelNPlanarModule() const override {return 0;}
  virtual int PixelN3DModule() const override {return 0;}

private:
  GeoVPhysVol* m_physVol;
  GeoTrf::Transform3D m_transform;
  double m_thicknessP;
  double m_thicknessN;
};

#endif


