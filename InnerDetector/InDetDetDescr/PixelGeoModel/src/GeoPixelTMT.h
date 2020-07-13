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
  GeoPixelTMT();
  virtual ~GeoPixelTMT();
  virtual GeoVPhysVol* Build();
  virtual GeoVPhysVol* getPhysVol ATLAS_NOT_THREAD_SAFE () const {return m_physVol;} // const method returns non-const pointer.
  virtual const GeoTrf::Transform3D & transform() const {return m_transform;}
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
  const GeoShape * addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans);

  GeoVPhysVol* m_physVol;
  GeoTrf::Transform3D m_transform;
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
};

#endif



