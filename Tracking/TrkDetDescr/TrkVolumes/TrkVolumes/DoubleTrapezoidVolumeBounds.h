/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DoubleTrapezoidVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_DOUBLETRAPEZOIDVOLUMESBOUNDS_H
#define TRKVOLUMES_DOUBLETRAPEZOIDVOLUMESBOUNDS_H

// Trk
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkDetDescrUtils/ObjectsAccessor.h"
#include "TrkVolumes/VolumeBounds.h"

class MsgStream;

namespace Trk {

class Surface;
class RectangleBounds;
class TrapezoidBounds;
class DiamondBounds;

/**
 @class DoubleTrapezoidVolumeBounds

 Bounds for a double trapezoidal shaped Volume, the decomposeToSurfaces method
 creates a vector of 8 surfaces:

  BoundarySurfaceFace [index]:

      - negativeFaceXY     [0] : Diamond Trk::PlaneSurface,
                                 parallel to \f$ xy \f$ plane at negative \f$ z
 \f$
      - positiveFaceXY     [1] : Diamond Trk::PlaneSurface,
                                 parallel to \f$ xy \f$ plane at positive \f$ z
 \f$
      - trapezoidFaceAlpha1 [2] : Rectangular  Trk::PlaneSurface,
                                 attached to [0] and [1] at negative \f$ x \f$
 (associated to alpha1)
      - trapezoidFaceBeta1  [3] : Rectangular  Trk::PlaneSurface,
                                 attached to [0] and [1] at positive \f$ x \f$
 (associated to beta1)
      - trapezoidFaceAlpha2 [5] : Rectangular  Trk::PlaneSurface,
                                 attached to [0] and [1] at negative \f$ x \f$
 (associated to alpha2)
      - trapezoidFaceBeta2  [6] : Rectangular  Trk::PlaneSurface,
                                 attached to [0] and [1] at positive \f$ x \f$
 (associated to beta2)
      - negativeFaceZX     [4] : Rectangular  Trk::PlaneSurface,
                                 parallel to \f$ zx \f$ plane at negative \f$ y
 \f$
      - positiveFaceZX     [5] : Rectangular  Trk::PlaneSurface,
                                 parallel to \f$ zx \f$ plane at positive \f$ y
 \f$

  @internal
  @image html DoubleTrapezoidVolumeBounds_decomp.gif
  @endinternal

  @author Andreas.Salzburger@cern.ch , Sarka.Todorova@cern.ch
  */

class DoubleTrapezoidVolumeBounds final: public VolumeBounds {
 public:
  /**Default Constructor*/
  DoubleTrapezoidVolumeBounds();

  /**Constructor - the double trapezoid boundaries (symmetric trapezoid/diamond)
   */
  DoubleTrapezoidVolumeBounds(double minhlenghtx, double medhlengthx,
                              double maxhlengthx, double hlenghty1,
                              double hlenghty2, double hlengthz);

  /**Copy Constructor */
  DoubleTrapezoidVolumeBounds(const DoubleTrapezoidVolumeBounds& bobo);

  /**Destructor */
  virtual ~DoubleTrapezoidVolumeBounds();

  /**Assignment operator*/
  DoubleTrapezoidVolumeBounds& operator=(
      const DoubleTrapezoidVolumeBounds& bobo);

  /**Virtual constructor */
  DoubleTrapezoidVolumeBounds* clone() const override final;

  /**This method checks if position in the 3D volume frame is inside the
   * cylinder*/
  bool inside(const Amg::Vector3D&, double tol = 0.) const override final;

  /** Method to decompose the Bounds into Surfaces */
  const std::vector<const Trk::Surface*>* decomposeToSurfaces
  (const Amg::Transform3D& transform) override final;

  /** Provide accessor for BoundarySurfaces */
  ObjectAccessor boundarySurfaceAccessor(
      const Amg::Vector3D& gp, const Amg::Vector3D& dir,
      bool forceInside = false) const override final;

  /**This method returns the X halflength at minimal Y*/
  double minHalflengthX() const;

  /**This method returns the (maximal) halflength in local x*/
  double medHalflengthX() const;

  /**This method returns the X halflength at maximal Y (local coordinates)*/
  double maxHalflengthX() const;

  /**This method returns the halflength1 in local y*/
  double halflengthY1() const;

  /**This method returns the halflength2 in local y*/
  double halflengthY2() const;

  /**This method returns the halflength in local z*/
  double halflengthZ() const;

  /**This method returns the opening angle in point A (negative local x)*/
  double alpha1() const;

  /**This method returns the opening angle in point A' (negative local x)*/
  double alpha2() const;

  /** Output Method for MsgStream*/
  MsgStream& dump(MsgStream& sl) const override final;
  /** Output Method for std::ostream */
  std::ostream& dump(std::ostream& sl) const override final;

 private:
  /** This method returns the associated DoubleTrapezoidBounds of the face
   * PlaneSurface parallel to local xy plane */
  DiamondBounds* faceXYDiamondBounds() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * attached to alpha (negative local x)*/
  RectangleBounds* faceAlpha1RectangleBounds() const;
  RectangleBounds* faceAlpha2RectangleBounds() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * attached to beta (positive local x)*/
  RectangleBounds* faceBeta1RectangleBounds() const;
  RectangleBounds* faceBeta2RectangleBounds() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * parallel to local zx plane, negative local y */
  RectangleBounds* faceZXRectangleBoundsBottom() const;

  /** This method returns the associated RecantleBounds of the face PlaneSurface
   * parallel to local zx plane, positive local y */
  RectangleBounds* faceZXRectangleBoundsTop() const;

#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif
  double m_minHalfX;  //!< minimal Y halflength in x
  double m_medHalfX;  //!< maximal halflength in x
  double m_maxHalfX;  //!< maximal Y halflength in x
  double m_halfY1;    //!< halflength in y
  double m_halfY2;    //!< halflength in y
  double m_halfZ;     //!< halflength in z

  double m_alpha1;  //!< opening angle alpha (in point A)
  double m_alpha2;  //!< opening angle alpha (in point A')
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif

  /** There's only one single object Acessor for the moment
      has to be implemented if Cuboids are used more widely */
  Trk::EightObjectsAccessor m_objectAccessor;
};

inline DoubleTrapezoidVolumeBounds* DoubleTrapezoidVolumeBounds::clone() const {
  return new DoubleTrapezoidVolumeBounds(*this);
}

inline double DoubleTrapezoidVolumeBounds::minHalflengthX() const {
  return m_minHalfX;
}

inline double DoubleTrapezoidVolumeBounds::medHalflengthX() const {
  return m_medHalfX;
}

inline double DoubleTrapezoidVolumeBounds::maxHalflengthX() const {
  return m_maxHalfX;
}

inline double DoubleTrapezoidVolumeBounds::halflengthY1() const {
  return m_halfY1;
}

inline double DoubleTrapezoidVolumeBounds::halflengthY2() const {
  return m_halfY2;
}

inline double DoubleTrapezoidVolumeBounds::halflengthZ() const {
  return m_halfZ;
}

inline double DoubleTrapezoidVolumeBounds::alpha1() const { return m_alpha1; }

inline double DoubleTrapezoidVolumeBounds::alpha2() const { return m_alpha2; }

inline ObjectAccessor DoubleTrapezoidVolumeBounds::boundarySurfaceAccessor(
    const Amg::Vector3D&, const Amg::Vector3D&, bool) const {
  return Trk::ObjectAccessor(m_objectAccessor);
}

}  // namespace Trk

#endif  // TRKVOLUMES_DOUBLETRAPEZOIDVOLUMESBOUNDS_H

