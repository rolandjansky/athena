/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELUTILITIES_GEOALIGNMENTSTORE_H
#define GEOMODELUTILITIES_GEOALIGNMENTSTORE_H

/// Ensure that the extensions for the Vector3D are properly loaded
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVAlignmentStore.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelUtilities/TransformMap.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include <stdexcept>

class GeoAlignmentStore : public GeoVAlignmentStore
{
 public:
  GeoAlignmentStore() {}
  virtual ~GeoAlignmentStore() override {}

  virtual void setDelta(const GeoAlignableTransform*, const GeoTrf::Transform3D&) override;
  virtual const GeoTrf::Transform3D* getDelta(const GeoAlignableTransform*) const override;

  virtual void setAbsPosition(const GeoVFullPhysVol*, const GeoTrf::Transform3D&) override;
  virtual const GeoTrf::Transform3D* getAbsPosition(const GeoVFullPhysVol*) const override;

  virtual void setDefAbsPosition(const GeoVFullPhysVol*, const GeoTrf::Transform3D&) override;
  virtual const GeoTrf::Transform3D* getDefAbsPosition(const GeoVFullPhysVol*) const override;


 protected:
  TransformMap<GeoAlignableTransform,GeoTrf::Transform3D> m_deltas;
  TransformMap<GeoVFullPhysVol,GeoTrf::Transform3D> m_absPositions;
  TransformMap<GeoVFullPhysVol,GeoTrf::Transform3D> m_defAbsPositions;

 public:
  const TransformMap<GeoAlignableTransform,GeoTrf::Transform3D>&
  getDeltas() const;

  const TransformMap<GeoVFullPhysVol,GeoTrf::Transform3D>&
  getAbsPositions() const;

  const TransformMap<GeoVFullPhysVol,GeoTrf::Transform3D>&
  getDefAbsPositions() const;
};

CLASS_DEF(GeoAlignmentStore, 135648236, 1)
CONDCONT_DEF( GeoAlignmentStore , 33985726 );

class ExcAlignmentStore : public std::runtime_error {
 public:
  ExcAlignmentStore (const std::string& what) 
    : std::runtime_error (what)
    {}
  ExcAlignmentStore (const char* what) 
    : std::runtime_error (what)
    {}  
};

inline
const TransformMap<GeoAlignableTransform,GeoTrf::Transform3D>&
GeoAlignmentStore::getDeltas() const { return m_deltas; }

inline
const TransformMap<GeoVFullPhysVol,GeoTrf::Transform3D>&
GeoAlignmentStore::getAbsPositions() const { return m_absPositions; }

inline
const TransformMap<GeoVFullPhysVol,GeoTrf::Transform3D>&
GeoAlignmentStore::getDefAbsPositions() const { return m_defAbsPositions; }

#endif 



