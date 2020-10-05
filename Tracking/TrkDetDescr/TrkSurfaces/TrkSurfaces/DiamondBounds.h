/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiamondBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DIAMONDDBOUNDS_H
#define TRKSURFACES_DIAMONDDBOUNDS_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <math.h>

class MsgStream;
class DiamondBoundsCnv_p1;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
 @class DiamondBounds
 Bounds for a double trapezoidal ("diamond"), planar Surface.

 @author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
 */

class DiamondBounds : public SurfaceBounds
{

public:
  /** BoundValues for better readability */
  enum BoundValues
  {
    bv_minHalfX = 0,
    bv_medHalfX = 1,
    bv_maxHalfX = 2,
    bv_halfY1 = 3,
    bv_halfY2 = 4,
    bv_length = 5
  };

  /**Default Constructor, needed for persistency*/
  DiamondBounds();

  /**Constructor for symmetric Diamond*/
  DiamondBounds(double minhalex, double medhalex, double maxhalex, double haley1, double haley2);

  /**Copy constructor*/
  DiamondBounds(const DiamondBounds& diabo) = default;

  /**Destructor*/
  virtual ~DiamondBounds() = default;

  /**Virtual constructor*/
  DiamondBounds* clone() const override;

  /**Assignment operator*/
  DiamondBounds& operator=(const DiamondBounds& sbo) = default;

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& diabo) const override;

  /** Return the bounds type */
  virtual BoundsType type() const override { return SurfaceBounds::Diamond; }

  /**This method returns the halflength in X at minimal Y (first coordinate of local surface frame)*/
  double minHalflengthX() const;

  /**This method returns the (maximal) halflength in X (first coordinate of local surface frame)*/
  double medHalflengthX() const;

  /**This method returns the halflength in X at maximal Y (first coordinate of local surface frame)*/
  double maxHalflengthX() const;

  /**This method returns the halflength in Y of trapezoid at negative/positive Y (second coordinate)*/
  double halflengthY1() const;
  double halflengthY2() const;

  /**This method returns the maximal extension on the local plane*/
  virtual double r() const override;

  /**This method returns the opening angle alpha in point A   */
  double alpha1() const;

  /**This method returns the opening angle alpha in point A'  */
  double alpha2() const;

  /** The orientation of the Diamond is according to the figure */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  /** This method checks inside bounds in loc1
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !
  */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc2
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !
  */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  friend class ::DiamondBoundsCnv_p1;

  /** inside() method for a full symmetric diamond */
  bool insideFull(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const;

  /** initialize the alpha1/2 cache - needed also for object persistency */
  virtual void initCache() override;

  /** Internal parameters stored in the geometry */
  std::vector<TDD_real_t> m_boundValues;
  TDD_real_t m_alpha1;
  TDD_real_t m_alpha2;
};

inline DiamondBounds*
DiamondBounds::clone() const
{
  return new DiamondBounds(*this);
}

inline double
DiamondBounds::minHalflengthX() const
{
  return m_boundValues[DiamondBounds::bv_minHalfX];
}

inline double
DiamondBounds::medHalflengthX() const
{
  return m_boundValues[DiamondBounds::bv_medHalfX];
}

inline double
DiamondBounds::maxHalflengthX() const
{
  return m_boundValues[DiamondBounds::bv_maxHalfX];
}

inline double
DiamondBounds::halflengthY1() const
{
  return m_boundValues[DiamondBounds::bv_halfY1];
}

inline double
DiamondBounds::halflengthY2() const
{
  return m_boundValues[DiamondBounds::bv_halfY2];
}

inline double
DiamondBounds::r() const
{
  return sqrt(m_boundValues[DiamondBounds::bv_medHalfX] * m_boundValues[DiamondBounds::bv_medHalfX] +
              m_boundValues[DiamondBounds::bv_halfY1] * m_boundValues[DiamondBounds::bv_halfY1]);
}

inline bool
DiamondBounds::inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const
{
  if (bchk.bcType == 0)
    return DiamondBounds::inside(locpo, bchk.toleranceLoc1, bchk.toleranceLoc2);

  // a fast FALSE
  double max_ell = bchk.lCovariance(0, 0) > bchk.lCovariance(1, 1) ? bchk.lCovariance(0, 0) : bchk.lCovariance(1, 1);
  double limit = bchk.nSigmas * sqrt(max_ell);
  if (locpo[Trk::locY] < -2 * m_boundValues[DiamondBounds::bv_halfY1] - limit)
    return false;
  if (locpo[Trk::locY] > 2 * m_boundValues[DiamondBounds::bv_halfY2] + limit)
    return false;
  // a fast FALSE
  double fabsX = fabs(locpo[Trk::locX]);
  if (fabsX > (m_boundValues[DiamondBounds::bv_medHalfX] + limit))
    return false;
  // a fast TRUE
  double min_ell = bchk.lCovariance(0, 0) < bchk.lCovariance(1, 1) ? bchk.lCovariance(0, 0) : bchk.lCovariance(1, 1);
  limit = bchk.nSigmas * sqrt(min_ell);
  if (fabsX < (fmin(m_boundValues[DiamondBounds::bv_minHalfX], m_boundValues[DiamondBounds::bv_maxHalfX]) - limit))
    return true;
  // a fast TRUE
  if (fabs(locpo[Trk::locY]) <
      (fmin(m_boundValues[DiamondBounds::bv_halfY1], m_boundValues[DiamondBounds::bv_halfY2]) - limit))
    return true;

  // compute KDOP and axes for surface polygon
  std::vector<KDOP> elementKDOP(5);
  std::vector<Amg::Vector2D> elementP(6);
  float theta = (bchk.lCovariance(1, 0) != 0 && (bchk.lCovariance(1, 1) - bchk.lCovariance(0, 0)) != 0)
                  ? .5 * bchk.FastArcTan(2 * bchk.lCovariance(1, 0) / (bchk.lCovariance(1, 1) - bchk.lCovariance(0, 0)))
                  : 0.;
  sincosCache scResult = bchk.FastSinCos(theta);
  AmgMatrix(2, 2) rotMatrix;
  rotMatrix << scResult.cosC, scResult.sinC, -scResult.sinC, scResult.cosC;
  AmgMatrix(2, 2) normal;
  normal << 0, -1, 1, 0;
  // ellipse is always at (0,0), surface is moved to ellipse position and then rotated
  Amg::Vector2D p;
  p << -m_boundValues[DiamondBounds::bv_minHalfX], -2. * m_boundValues[DiamondBounds::bv_halfY1];
  elementP[0] = (rotMatrix * (p - locpo));
  p << -m_boundValues[DiamondBounds::bv_medHalfX], 0.;
  elementP[1] = (rotMatrix * (p - locpo));
  p << -m_boundValues[DiamondBounds::bv_maxHalfX], 2. * m_boundValues[DiamondBounds::bv_halfY2];
  elementP[2] = (rotMatrix * (p - locpo));
  p << m_boundValues[DiamondBounds::bv_maxHalfX], 2. * m_boundValues[DiamondBounds::bv_halfY2];
  elementP[3] = (rotMatrix * (p - locpo));
  p << m_boundValues[DiamondBounds::bv_medHalfX], 0.;
  elementP[4] = (rotMatrix * (p - locpo));
  p << m_boundValues[DiamondBounds::bv_minHalfX], -2. * m_boundValues[DiamondBounds::bv_halfY1];
  elementP[5] = (rotMatrix * (p - locpo));
  std::vector<Amg::Vector2D> axis = { normal * (elementP[1] - elementP[0]),
                                      normal * (elementP[2] - elementP[1]),
                                      normal * (elementP[3] - elementP[2]),
                                      normal * (elementP[4] - elementP[3]),
                                      normal * (elementP[5] - elementP[4]) };
  bchk.ComputeKDOP(elementP, axis, elementKDOP);
  // compute KDOP for error ellipse
  std::vector<KDOP> errelipseKDOP(5);
  bchk.ComputeKDOP(bchk.EllipseToPoly(3), axis, errelipseKDOP);
  // check if KDOPs overlap and return result
  return bchk.TestKDOPKDOP(elementKDOP, errelipseKDOP);
}

inline bool
DiamondBounds::insideLoc1(const Amg::Vector2D& locpo, double tol1) const
{
  return (fabs(locpo[locX]) < m_boundValues[DiamondBounds::bv_medHalfX] + tol1);
}

inline bool
DiamondBounds::insideLoc2(const Amg::Vector2D& locpo, double tol2) const
{
  return ((locpo[locY] > -2. * m_boundValues[DiamondBounds::bv_halfY1] - tol2) &&
          (locpo[locY] < 2. * m_boundValues[DiamondBounds::bv_halfY2] + tol2));
}

inline void
DiamondBounds::initCache()
{
  m_alpha1 = atan2(m_boundValues[DiamondBounds::bv_medHalfX] - m_boundValues[DiamondBounds::bv_minHalfX],
                   2. * m_boundValues[DiamondBounds::bv_halfY1]);
  m_alpha2 = atan2(m_boundValues[DiamondBounds::bv_medHalfX] - m_boundValues[DiamondBounds::bv_maxHalfX],
                   2. * m_boundValues[DiamondBounds::bv_halfY2]);
}

} // end of namespace

#endif // TRKSURFACES_DIAMONDBOUNDS_H
