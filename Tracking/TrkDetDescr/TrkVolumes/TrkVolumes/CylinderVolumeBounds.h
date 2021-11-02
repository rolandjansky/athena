/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_CYLINDERVOLUMESBOUNDS_H
#define TRKVOLUMES_CYLINDERVOLUMESBOUNDS_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkVolumes/CylinderVolumeBoundaryAccessors.h"
#include "TrkVolumes/VolumeBounds.h"

class MsgStream;

namespace Trk {

class Surface;
class DiscBounds;
class CylinderBounds;
class RectangleBounds;

/**
 @class CylinderVolumeBounds

 Bounds for a cylindrical Volume, the decomposeToSurfaces method creates a
 vector of up to 6 surfaces:

 case A) 3 Surfaces (full cylindrical tube):
  BoundarySurfaceFace [index]:
      - negativeFaceXY [0] : Trk::DiscSurface with \f$ r_{inner}=0 \f$,
                             parallel to \f$ xy \f$ plane at negative \f$ z \f$
      - positiveFaceXY [1] : Trk::DiscSurface with \f$ r_{inner}=0 \f$,
                             parallel to \f$ xy \f$ plane at positive \f$ z \f$
      - cylinderCover  [2] : Trk::CylinderSurface confining the Trk::Volume

 case B) 4 Surfaces (tube with inner and outer radius):
  BoundarySurfaceFace [index]:
      - negativeFaceXY [0] : Trk::DiscSurface with \f$ r_{inner}>0 \f$,
                             parallel to \f$ xy \f$ plane at negative \f$ z \f$
      - positiveFaceXY [1] : Trk::DiscSurface with \f$ r_{inner}>0 \f$,
                             parallel to \f$ xy \f$ plane at positive \f$ z \f$
      - tubeOuterCover [2] : Trk::CylinderSurface with \f$ r = r_{outer} \f$
      - tubeInnerCover [3] : Trk::CylinderSurface with \f$ r = r_{inner} \f$

 case C) 6 Surfaces (sectoral tube with inner and outer radius):
  BoundarySurfaceFace [index]:
      - negativeFaceXY        [0] : Trk::DiscSurface with \f$ r_{inner}>0  \f$
 and \f$ \phi < \pi \f$, parallel to \f$ xy \f$ plane at negative \f$ z \f$
      - positiveFaceXY        [1] : Trk::DiscSurface with \f$ r_{inner}>0 \f$
 and \f$ \phi < \pi \f$, parallel to \f$ xy \f$ plane at positive \f$ z \f$
      - tubeSectorOuterCover  [2] : Trk::CylinderSurface with \f$ r = r_{outer}
 \f$
      - tubeSectorInnerCover  [3] : Trk::CylinderSurface with \f$ r = r_{inner}
 \f$
      - tubeSectorNegativePhi [4] : Rectangular Trk::PlaneSurface attached to
 [0] and [1] at negative \f$ \phi \f$
      - tubeSectorNegativePhi [5] : Rectangular Trk::PlaneSurface attached to
 [0] and [1] at positive \f$ \phi \f$

  @image html CylinderVolumeBounds_decomp.gif

  @author Andreas.Salzburger@cern.ch
  @author Christos Anastopoulos (Athena  MT modifications)
  */

class CylinderVolumeBounds final: public VolumeBounds {
 public:
  /**Default Constructor*/
  CylinderVolumeBounds();

  /**Constructor - full cylinder */
  CylinderVolumeBounds(double radius, double halez);

  /**Constructor - extruded cylinder */
  CylinderVolumeBounds(double rinner, double router, double halez);

  /**Constructor - extruded cylinder segment */
  CylinderVolumeBounds(double rinner, double router, double halfPhiSector,
                       double halez);

  /**Copy Constructor */
  CylinderVolumeBounds(const CylinderVolumeBounds& cylbo);

  /**Destructor */
  virtual ~CylinderVolumeBounds();

  /**Assignment operator*/
  CylinderVolumeBounds& operator=(const CylinderVolumeBounds& cylbo);

  /**Virtual constructor */
  CylinderVolumeBounds* clone() const override;

  /**This method checks if position in the 3D volume frame is inside the
   * cylinder*/
  bool inside(const Amg::Vector3D&, double tol = 0.) const override final;

  /** Method to decompose the Bounds into boundarySurfaces */
  const std::vector<const Trk::Surface*>* decomposeToSurfaces
  (const Amg::Transform3D& transform) override final;

  /** Provide accessor for BoundarySurfaces */
  ObjectAccessor boundarySurfaceAccessor(
      const Amg::Vector3D& gp, const Amg::Vector3D& dir,
      bool forceInside = false) const override final;

  /**This method returns the inner radius*/
  double innerRadius() const;

  /**This method returns the outer radius*/
  double outerRadius() const;

  /**This method returns the medium radius*/
  double mediumRadius() const;

  /**This method returns the delta radius*/
  double deltaRadius() const;

  /**This method returns the halfPhiSector angle*/
  double halfPhiSector() const;

  /**This method returns the halflengthZ*/
  double halflengthZ() const;

  /** Output Method for MsgStream*/
  MsgStream& dump(MsgStream& sl) const override final;
  /** Output Method for std::ostream */
  std::ostream& dump(std::ostream& sl) const override final;

 private:
  /** This method returns the associated CylinderBounds of the inner
   * CylinderSurfaces. */
  CylinderBounds* innerCylinderBounds() const;

  /** This method returns the associated CylinderBounds of the outer
   * CylinderSurfaces. */
  CylinderBounds* outerCylinderBounds() const;

  /** This method returns the associated DiscBounds for the bottom/top
   * DiscSurface. */
  DiscBounds* bottomDiscBounds() const;

  /** This method returns the associated DiscBounds for the bottom/top
   * DiscSurface. */
  DiscBounds* topDiscBounds() const;

  /** This method returns the associated PlaneBounds limiting a sectoral
   * CylinderVolume. */
  RectangleBounds* sectorPlaneBounds() const;

  /** Private method to construct the accessors */
  void createBoundarySurfaceAccessors();

  double m_innerRadius;
  double m_outerRadius;
  double m_halfPhiSector;
  double m_halfZ;

  /** Accessors for Boundary surface access - static is not possible due to
     mismatched delete() / free () with TrkMagFieldUtils
    */
  CylinderVolumeBoundaryAccessors m_boundaryAccessors;

  /** numerical stability */
  static const double s_numericalStable;
};

inline CylinderVolumeBounds* CylinderVolumeBounds::clone() const {
  return new CylinderVolumeBounds(*this);
}

inline bool CylinderVolumeBounds::inside(const Amg::Vector3D& pos,
                                         double tol) const {
  double ros = pos.perp();
  // bool insidePhi =  fabs(pos.phi()) <= m_halfPhiSector + tol;
  bool insidePhi = cos(pos.phi()) >= cos(m_halfPhiSector) - tol;
  bool insideR =
      insidePhi ? ((ros >= m_innerRadius - tol) && (ros <= m_outerRadius + tol))
                : false;
  bool insideZ = insideR ? (fabs(pos.z()) <= m_halfZ + tol) : false;
  return (insideZ && insideR && insidePhi);
}

inline double CylinderVolumeBounds::innerRadius() const {
  return m_innerRadius;
}

inline double CylinderVolumeBounds::outerRadius() const {
  return m_outerRadius;
}

inline double CylinderVolumeBounds::mediumRadius() const {
  return 0.5 * (m_innerRadius + m_outerRadius);
}

inline double CylinderVolumeBounds::deltaRadius() const {
  return (m_outerRadius - m_innerRadius);
}

inline double CylinderVolumeBounds::halfPhiSector() const {
  return m_halfPhiSector;
}

inline double CylinderVolumeBounds::halflengthZ() const { return m_halfZ; }

inline DiscBounds* CylinderVolumeBounds::topDiscBounds() const {
  return this->bottomDiscBounds();
}

}  // namespace Trk

#endif  // TRKVOLUMES_CYLINDERVOLUMESBOUNDS_H

