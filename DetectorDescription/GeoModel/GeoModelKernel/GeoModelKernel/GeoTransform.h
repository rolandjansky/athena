/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELKERNEL_GEOTRANSFORM_H
#define GEOMODELKERNEL_GEOTRANSFORM_H 

/**
 * @class GeoTransform
 *
 * @brief A basic geometrical (Euclidean) transform.  Can return a HepGeom::Transform3D.
 * Reference counted.  There is no (mis)alignment present in this class.  For a transform
 * that can be  (mis)aligned, see GeoAlignableTransform
 * 
 */

#include "GeoModelKernel/GeoGraphNode.h"
#include "CLHEP/Geometry/Transform3D.h"

class GeoVAlignmentStore;

class GeoTransform : public GeoGraphNode
{
 public:
  GeoTransform(const HepGeom::Transform3D& transform);

  GeoTransform(const GeoTransform &right) = delete;
  GeoTransform & operator=(const GeoTransform &right) = delete;

  /// Gets the total transformation.
  virtual HepGeom::Transform3D getTransform(const GeoVAlignmentStore* store=nullptr) const;

  /// Gets the default transformation (no alignment correction)
  HepGeom::Transform3D getDefTransform(const GeoVAlignmentStore* store=nullptr) const;

  ///	Executes a GeoNodeAction.
  virtual void exec(GeoNodeAction *action) const override final;
  
 protected:
  virtual ~GeoTransform() override;

 private:
  // The Euclidean (Rigid Body) transform.
  HepGeom::Transform3D m_transform;
};

#endif
