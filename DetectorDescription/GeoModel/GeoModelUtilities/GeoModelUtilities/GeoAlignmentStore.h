/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELUTILITIES_GEOALIGNMENTSTORE_H
#define GEOMODELUTILITIES_GEOALIGNMENTSTORE_H

#include "GeoModelKernel/GeoVAlignmentStore.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include "CLHEP/Geometry/Transform3D.h"
#include <unordered_map>
#include <stdexcept>

class GeoAlignmentStore final : public GeoVAlignmentStore
{
 public:
  GeoAlignmentStore() {}
  virtual ~GeoAlignmentStore() override {}

  virtual void setDelta(const GeoAlignableTransform*, const HepGeom::Transform3D&) override;
  virtual const HepGeom::Transform3D* getDelta(const GeoAlignableTransform*) const override;

  virtual void setAbsPosition(const GeoVFullPhysVol*, const HepGeom::Transform3D&) override;
  virtual const HepGeom::Transform3D* getAbsPosition(const GeoVFullPhysVol*) const override;

  virtual void setDefAbsPosition(const GeoVFullPhysVol*, const HepGeom::Transform3D&) override;
  virtual const HepGeom::Transform3D* getDefAbsPosition(const GeoVFullPhysVol*) const override;


 private:
  std::unordered_map<const GeoAlignableTransform*, HepGeom::Transform3D> m_deltas;
  std::unordered_map<const GeoVFullPhysVol*, HepGeom::Transform3D> m_absPositions;
  std::unordered_map<const GeoVFullPhysVol*, HepGeom::Transform3D> m_defAbsPositions;
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

#endif 



