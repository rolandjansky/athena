/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELTMT_H
#define GEOPIXELTMT_H

#include "GeoPixelStaveSupport.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class GeoShape;

class GeoPixelTMT : public GeoPixelStaveSupport {

public:  
  GeoPixelTMT(InDetDD::PixelDetectorManager* ddmgr, PixelGeometryManager* mgr);
  virtual ~GeoPixelTMT();
  virtual GeoVPhysVol* Build() override;
  virtual GeoVPhysVol* getPhysVol () override {return m_physVol;}
  virtual const GeoTrf::Transform3D & transform() const override {return m_transform;}
  virtual double thicknessP() const override {return 0;} // Use ladder thickness from database
  virtual double thicknessN() const override {return 0;} // Use ladder thickness from database
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape(double) override { return 0;}
  virtual GeoVPhysVol* getEndblockEnvelopShape(int) const override {return 0;}
  virtual GeoTransform* getEndblockEnvelopShapeTrf(int) const override {return 0;}
  virtual double getEndblockZpos() const override { return 0.; }
  virtual double getServiceZpos() const override { return 0; }
  virtual double getEndblockLength() const override { return 0.; }
  virtual void computeStaveEnvelopTransformAndSize(double ,double, double, double, double, double)  override {};

  virtual int PixelNModule() const override {return 0;}
  virtual int PixelNPlanarModule() const override {return 0;}
  virtual int PixelN3DModule() const override {return 0;}
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl;}

private:
  const GeoShape * addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans);

  GeoVPhysVol* m_physVol;
  GeoTrf::Transform3D m_transform;
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
};

#endif



