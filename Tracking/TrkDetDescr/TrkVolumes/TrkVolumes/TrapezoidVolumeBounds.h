/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrapezoidVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_TRAPEZOIDVOLUMESBOUNDS_H
#define TRKVOLUMES_TRAPEZOIDVOLUMESBOUNDS_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/ObjectsAccessor.h"
#include "TrkVolumes/VolumeBounds.h"

class MsgStream;

namespace Trk {

class Surface;
class RectangleBounds;
class TrapezoidBounds;

/**
 @class TrapezoidVolumeBounds

 Bounds for a trapezoidal shaped Volume, the decomposeToSurfaces method creates
 a vector of 6 surfaces:

  BoundarySurfaceFace [index]:

      - negativeFaceXY     [0] : Trazpezoidal Trk::PlaneSurface,
                                 parallel to \f$ xy \f$ plane at negative \f$ z
 \f$
      - positiveFaceXY     [1] : Trazpezoidal Trk::PlaneSurface,
                                 parallel to \f$ xy \f$ plane at positive \f$ z
 \f$
      - trapezoidFaceAlpha [2] : Rectangular  Trk::PlaneSurface,
                                 attached to [0] and [1] at negative \f$ x \f$
 (associated to alpha)
      - trapezoidFaceBeta  [3] : Rectangular  Trk::PlaneSurface,
                                 attached to [0] and [1] at positive \f$ x \f$
 (associated to beta)
      - negativeFaceZX     [4] : Rectangular  Trk::PlaneSurface,
                                 parallel to \f$ zx \f$ plane at negative \f$ y
 \f$
      - positiveFaceZX     [5] : Rectangular  Trk::PlaneSurface,
                                 parallel to \f$ zx \f$ plane at positive \f$ y
 \f$

  @image html TrapezoidVolumeBounds_decomp.gif

  @author Andreas.Salzburger@cern.ch
  */

class TrapezoidVolumeBounds final: public VolumeBounds {
 public:
  /**Default Constructor*/
  TrapezoidVolumeBounds();

  /**Constructor - the trapezoid boundaries (symmetric trapezoid) */
  TrapezoidVolumeBounds(double minhlenghtx, double maxhlengthx, double hlenghty,
                        double hlengthz);

  /**Constructor - the trapezoid boundaries (arbitrary trapezoid) */
  TrapezoidVolumeBounds(double minhlenghtx, double hlenghty, double hlengthz,
                        double alpha, double beta);

  /**Copy Constructor */
  TrapezoidVolumeBounds(const TrapezoidVolumeBounds& bobo);

  /**Destructor */
  virtual ~TrapezoidVolumeBounds();

  /**Assignment operator*/
  TrapezoidVolumeBounds& operator=(const TrapezoidVolumeBounds& bobo);

  /**Virtual constructor */
  TrapezoidVolumeBounds* clone() const override final;

  /**This method checks if position in the 3D volume frame is inside the
   * cylinder*/
  bool inside(const Amg::Vector3D&, double tol = 0.) const override final;

  /** Method to decompose the Bounds into Surfaces */
  const std::vector<const Trk::Surface*>* decomposeToSurfaces
  (const Amg::Transform3D& transform) override final;

  /** Provide accessor for BoundarySurfaces */
  ObjectAccessor boundarySurfaceAccessor(
      const Amg::Vector3D& gp, const Amg::Vector3D& dir,
      bool forceInside = false) const override;

  /**This method returns the minimal halflength in local x*/
  double minHalflengthX() const;

  /**This method returns the maximal halflength in local x*/
  double maxHalflengthX() const;

  /**This method returns the halflength in local y*/
  double halflengthY() const;

  /**This method returns the halflength in local z*/
  double halflengthZ() const;

  /**This method returns the opening angle in point A (negative local x)*/
  double alpha() const;

  /**This method returns the opening angle in point B (negative local x)*/
  double beta() const;

  /** Output Method for MsgStream*/
  MsgStream& dump(MsgStream& sl) const override final;

  /** Output Method for std::ostream */
  std::ostream& dump(std::ostream& sl) const override final;

 private:
  /** This method returns the associated TrapezoidBounds of the face
   * PlaneSurface parallel to local xy plane */
  TrapezoidBounds* faceXYTrapezoidBounds() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * attached to alpha (negative local x)*/
  RectangleBounds* faceAlphaRectangleBounds() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * attached to beta (positive local x)*/
  RectangleBounds* faceBetaRectangleBounds() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * parallel to local zx plane, negative local y */
  RectangleBounds* faceZXRectangleBoundsBottom() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * parallel to local zx plane, positive local y */
  RectangleBounds* faceZXRectangleBoundsTop() const;

  double m_minHalfX;  //!< minimal halflength in x
  double m_maxHalfX;  //!< maximal halflength in x
  double m_halfY;     //!< halflength in y
  double m_halfZ;     //!< halflength in z

  double m_alpha;  //!< opening angle alpha (in point A)
  double m_beta;   //!< opening angle beta  (in point B)

  /** There's only one single object Acessor for the moment
      has to be implemented if Cuboids are used more widely */
  SixObjectsAccessor m_objectAccessor;
};

inline TrapezoidVolumeBounds* TrapezoidVolumeBounds::clone() const {
  return new TrapezoidVolumeBounds(*this);
}

inline double TrapezoidVolumeBounds::minHalflengthX() const {
  return m_minHalfX;
}
inline double TrapezoidVolumeBounds::maxHalflengthX() const {
  return m_maxHalfX;
}
inline double TrapezoidVolumeBounds::halflengthY() const { return m_halfY; }
inline double TrapezoidVolumeBounds::halflengthZ() const { return m_halfZ; }
inline double TrapezoidVolumeBounds::alpha() const { return m_alpha; }
inline double TrapezoidVolumeBounds::beta() const { return m_beta; }

inline ObjectAccessor TrapezoidVolumeBounds::boundarySurfaceAccessor(
    const Amg::Vector3D&, const Amg::Vector3D&, bool) const {
  return Trk::ObjectAccessor(m_objectAccessor);
}

}  // namespace Trk

#endif  // TRKVOLUMES_TRAPEZOIDVOLUMESBOUNDS_H

