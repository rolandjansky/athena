/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELKERNEL_GEOVALIGNMENTSTORE_H
#define GEOMODELKERNEL_GEOVALIGNMENTSTORE_H

/**
 * @class GeoVAlignmentStore
 *
 * @brief Abstract base class for GeoModel alignment stores
 * The interface allows to store/retrieve transformations that serve as:
 *   1. Delta-s of Alignable Transforms
 *   2. Abs positions of Full Physical Volumes
 *   3. Def Abs positions of Full Physical Volumes
 */

class GeoAlignableTransform;
class GeoVFullPhysVol;
namespace HepGeom {
  class Transform3D;
}

class GeoVAlignmentStore
{
 public:
  GeoVAlignmentStore() {}
  virtual ~GeoVAlignmentStore() {}
  
  virtual void setDelta(const GeoAlignableTransform*, const HepGeom::Transform3D&) = 0;
  virtual const HepGeom::Transform3D* getDelta(const GeoAlignableTransform*) const = 0;

  virtual void setAbsPosition(const GeoVFullPhysVol*, const HepGeom::Transform3D&) = 0;
  virtual const HepGeom::Transform3D* getAbsPosition(const GeoVFullPhysVol*) const = 0;

  virtual void setDefAbsPosition(const GeoVFullPhysVol*, const HepGeom::Transform3D&) = 0;
  virtual const HepGeom::Transform3D* getDefAbsPosition(const GeoVFullPhysVol*) const = 0;

};


#endif
