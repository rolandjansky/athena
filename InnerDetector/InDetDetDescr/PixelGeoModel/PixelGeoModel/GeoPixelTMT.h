/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELTMT_H
#define GEOPIXELTMT_H

#include "PixelGeoModel/GeoPixelStaveSupport.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AthenaKernel/MsgStreamMember.h"

class GeoShape;

class GeoPixelTMT : public GeoPixelStaveSupport {

public:  
  GeoPixelTMT();
  virtual GeoVPhysVol* Build();
  virtual GeoVPhysVol* getPhysVol() const {return m_physVol;}
  virtual const HepGeom::Transform3D & transform() const {return m_transform;}
  virtual double thicknessP() const {return 0;} // Use ladder thickness from database
  virtual double thicknessN() const {return 0;} // Use ladder thickness from database
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape(double) { return 0;}
  virtual GeoVPhysVol* getEndblockEnvelopShape(int) const {return 0;}
  virtual GeoTransform* getEndblockEnvelopShapeTrf(int) const {return 0;}
  virtual double getEndblockZpos() const { return 0.; }
  virtual double getServiceZpos() const { return 0; }
  virtual double getEndblockLength() const { return 0.; }
  virtual void computeStaveEnvelopTransformAndSize(double ,double, double, double, double, double){};

  virtual int PixelNModule() const {return 0;}
  virtual int PixelNPlanarModule() const {return 0;}
  virtual int PixelN3DModule() const {return 0;}
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl;}

private:
  const GeoShape * addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans);

  GeoVPhysVol* m_physVol;
  HepGeom::Transform3D m_transform;
  mutable Athena::MsgStreamMember m_msg;
};

#endif



