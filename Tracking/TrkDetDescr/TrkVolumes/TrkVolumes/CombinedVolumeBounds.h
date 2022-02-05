/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinedVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_COMBINEDVOLUMEBOUNDS_H
#define TRKVOLUMES_COMBINEDVOLUMEBOUNDS_H

// Trk
#include "TrkDetDescrUtils/ObjectsAccessor.h"
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/VolumeBounds.h"

// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

class SurfaceBounds;
class Volume;
class Surface;

/**
 @class CombinedVolumeBounds

 Bounds for a generic combined volume, the decomposeToSurfaces method creates a
 vector of n surfaces (n1+n2-nshared):

 BoundarySurfaceFace [index]: [n1+n2-nshared] combined surfaces

 designed to allow transcript of GeoShapeUnion and GeoShapeIntersection

 @author Sarka.Todorova@cern.ch
 @author Christos Anastopoulos (Athena  MT modifications)
*/

class CombinedVolumeBounds final : public VolumeBounds {
 public:
  /**Default Constructor*/
  CombinedVolumeBounds();

  /**Constructor - the box boundaries */
  CombinedVolumeBounds(Volume* first, Volume* second, bool intersection);

  /**Copy Constructor */
  CombinedVolumeBounds(const CombinedVolumeBounds& bobo);

  /**Destructor */
  virtual ~CombinedVolumeBounds();

  /**Assignment operator*/
  CombinedVolumeBounds& operator=(const CombinedVolumeBounds& bobo);

  /**Virtual constructor */
  virtual CombinedVolumeBounds* clone() const override final;

  /**This method checks if position in the 3D volume frame is inside the
   * volume*/
  virtual bool inside(const Amg::Vector3D&, double tol = 0.) const override final;

  /** Method to decompose the Bounds into boundarySurfaces */
  virtual const std::vector<const Trk::Surface*>* decomposeToSurfaces
  (const Amg::Transform3D& transform) override final;

  /** Provide accessor for BoundarySurfaces */
  virtual ObjectAccessor boundarySurfaceAccessor(
      const Amg::Vector3D& gp, const Amg::Vector3D& dir,
      bool forceInside = false) const override final;

  /**This method returns the first VolumeBounds*/
  const Volume* first() const;

  /**This method returns the second VolumeBounds*/
  const Volume* second() const;

  /**This method distinguishes between Union(0) and Intersection(1)*/
  bool intersection() const;

  /**This method returns bounds orientation*/
  std::vector<bool> boundsOrientation() const;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override final;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override final;

 private:
  static Trk::Volume* createSubtractedVolume(const Amg::Transform3D& transf,
                                      Trk::Volume* subtrVol) ;

  Volume* m_first;
  Volume* m_second;
  bool m_intersection;
  EightObjectsAccessor m_objectAccessor;
  mutable std::vector<bool> m_boundsOrientation ATLAS_THREAD_SAFE;
};

inline CombinedVolumeBounds* CombinedVolumeBounds::clone() const {
  return new CombinedVolumeBounds(*this);
}

inline bool CombinedVolumeBounds::inside(const Amg::Vector3D& pos,
                                         double tol) const {
  if (m_intersection)
    return (m_first->inside(pos, tol) && m_second->inside(pos, tol));
  return (m_first->inside(pos, tol) || m_second->inside(pos, tol));
}

inline const Volume* CombinedVolumeBounds::first() const { return m_first; }

inline const Volume* CombinedVolumeBounds::second() const { return m_second; }

inline bool CombinedVolumeBounds::intersection() const {
  return m_intersection;
}

inline ObjectAccessor CombinedVolumeBounds::boundarySurfaceAccessor(
    const Amg::Vector3D&, const Amg::Vector3D&, bool) const {
  return Trk::ObjectAccessor(m_objectAccessor);
}

inline std::vector<bool> CombinedVolumeBounds::boundsOrientation() const {
  return (m_boundsOrientation);
}

}  // namespace Trk

#endif  // TRKVOLUMES_COMBINEDVOLUMEBOUNDS_H

