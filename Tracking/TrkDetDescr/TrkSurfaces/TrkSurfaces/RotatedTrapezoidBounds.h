/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RotatedTrapezoidBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_ROTATEDTRAPEZOIDBOUNDS_H
#define TRKSURFACES_ROTATEDTRAPEZOIDBOUNDS_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <math.h>

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

class MsgStream;
class RotatedTrapezoidBoundsCnv_p1;

namespace Trk {

/**
 @class RotatedTrapezoidBounds

 Bounds for a rotated trapezoidal, planar Surface.
 Contrary to the TrapezoidBounds the local x axis builds the
 symmetry axis for a symmetric Trapezoid.

 An arbitrary rotated trapezoidal shape has not been implemented yet,
 the TrapezoidBounds may be taken instead.

 @image html RotatedTrapezoidBounds.gif

 @author Andreas.Salzburger@cern.ch
 */

class RotatedTrapezoidBounds : public SurfaceBounds
{

public:
  /** @enum BoundValues for readibility */
  enum BoundValues
  {
    bv_halfX = 0,
    bv_minHalfY = 1,
    bv_maxHalfY = 2,
    bv_length = 3
  };

  /**Default Constructor, needed for persistency*/
  RotatedTrapezoidBounds();

  /**Constructor for symmetric Trapezoid*/
  RotatedTrapezoidBounds(double halex, double minhalex, double maxhalex);

  /**Copy constructor*/
  RotatedTrapezoidBounds(const RotatedTrapezoidBounds& trabo) = default;

  /**Destructor*/
  virtual ~RotatedTrapezoidBounds() = default;

  /**Virtual constructor*/
  virtual RotatedTrapezoidBounds* clone() const override;

  /** Return the type of the bounds for persistency */
  virtual BoundsType type() const override { return SurfaceBounds::RotatedTrapezoid; }

  /**Assignment operator*/
  RotatedTrapezoidBounds& operator=(const RotatedTrapezoidBounds& sbo) = default;

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& trabo) const override;

  /**This method returns the minimal halflength in X (first coordinate of local surface frame)*/
  double halflengthX() const;

  /**This method returns the maximal halflength in X (first coordinate of local surface frame)*/
  double minHalflengthY() const;

  /**This method returns the halflength in Y (second coordinate of local surface frame)*/
  double maxHalflengthY() const;

  /**This method returns the maximal extension on the local plane*/
  virtual double r() const override;

  /** The orientation of the Trapezoid is according to the figure above,
   in words: the shorter of the two parallel sides of the trapezoid intersects
   with the negative @f$ y @f$ - axis of the local frame.

   <br>
   The cases are:<br>
   (0) @f$ \eta @f$ or @f$ \phi @f$ bounds are 0 || 0<br>
   (1) LocalPosition is outside @f$ x @f$ bounds <br>
   (2) LocalPosition is inside @f$ x @f$ bounds, but outside maximum @f$ y @f$ bounds  <br>
   (3) LocalPosition is inside @f$ x @f$ bounds AND inside minimum @f$ y @f$ bounds <br>
   (4) LocalPosition is inside @f$ x @f$ bounds AND inside maximum @f$ y @f$ bounds, so that
   it depends on the @f$ x @f$ coordinate
   (5) LocalPosition fails test of (4) <br>

   The inside check is done using single equations of straight lines and one has to take care if a point
   lies on the positive @f$ \phi @f$ half area(I) or the negative one(II). Denoting @f$ | y_{min}| @f$ and
   @f$ | y_{max} | @f$ as \c minHalfY respectively \c maxHalfX, such as @f$ | x_{H} | @f$ as \c halfX,
   the equations for the straing lines in (I) and (II) can be written as:<br>
    <br>
   - (I):  @f$ y = \kappa_{I} x + \delta_{I} @f$ <br>
   - (II): @f$ y = \kappa_{II} x + \delta_{II} @f$ ,<br>
    <br>
   where <br>
    @f$ \kappa_{I} = - \kappa_{II} = \frac{y_{max}-y_{min}}{x_{H}} @f$ <br>
    @f$ \delta_{I} = \frac{1}{2}\cdot (y_{max} + y_{min} ) @f$ <br>*/
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  /** This method checks inside bounds in loc1
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !*/
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc2
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !*/
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  friend class ::RotatedTrapezoidBoundsCnv_p1;

  /** isBelow() method for checking whether a point lies above or under a straight line */
  bool isBelow(double locX, double fabsLocY, double tol1, double tol2) const;

  /** Helper function for angle parameter initialization */
  virtual void initCache() override;

  /** The internal storage of the bounds can be float/double*/
  std::vector<TDD_real_t> m_boundValues;
  TDD_real_t m_kappa;
  TDD_real_t m_delta;
};

inline RotatedTrapezoidBounds*
RotatedTrapezoidBounds::clone() const
{
  return new RotatedTrapezoidBounds(*this);
}

inline void
RotatedTrapezoidBounds::initCache()
{
  m_kappa = 0.5 *
            (m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] - m_boundValues[RotatedTrapezoidBounds::bv_minHalfY]) /
            m_boundValues[RotatedTrapezoidBounds::bv_halfX];
  m_delta =
    0.5 * (m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] + m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY]);
}

inline double
RotatedTrapezoidBounds::halflengthX() const
{
  return m_boundValues[RotatedTrapezoidBounds::bv_halfX];
}

inline double
RotatedTrapezoidBounds::minHalflengthY() const
{
  return m_boundValues[RotatedTrapezoidBounds::bv_minHalfY];
}

inline double
RotatedTrapezoidBounds::maxHalflengthY() const
{
  return m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY];
}

inline double
RotatedTrapezoidBounds::r() const
{
  return sqrt(m_boundValues[RotatedTrapezoidBounds::bv_halfX] * m_boundValues[RotatedTrapezoidBounds::bv_halfX] +
              m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] * m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY]);
}

inline bool
RotatedTrapezoidBounds::inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const
{
  if (bchk.bcType == 0)
    return RotatedTrapezoidBounds::inside(locpo, bchk.toleranceLoc1, bchk.toleranceLoc2);

  // a fast FALSE
  double fabsX = fabs(locpo[Trk::locX]);
  double max_ell = bchk.lCovariance(0, 0) > bchk.lCovariance(1, 1) ? bchk.lCovariance(0, 0) : bchk.lCovariance(1, 1);
  double limit = bchk.nSigmas * sqrt(max_ell);
  if (fabsX > (m_boundValues[RotatedTrapezoidBounds::bv_halfX] + limit))
    return false;
  // a fast FALSE
  double fabsY = fabs(locpo[Trk::locY]);
  if (fabsY > (m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] + limit))
    return false;
  // a fast TRUE
  double min_ell = bchk.lCovariance(0, 0) < bchk.lCovariance(1, 1) ? bchk.lCovariance(0, 0) : bchk.lCovariance(1, 1);
  limit = bchk.nSigmas * sqrt(min_ell);
  if (fabsY < (m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] + limit) &&
      fabsX < (m_boundValues[RotatedTrapezoidBounds::bv_halfX] + limit))
    return true;

  // compute KDOP and axes for surface polygon
  std::vector<KDOP> elementKDOP(3);
  std::vector<Amg::Vector2D> elementP(4);
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
  p << -m_boundValues[RotatedTrapezoidBounds::bv_halfX], m_boundValues[RotatedTrapezoidBounds::bv_minHalfY];
  elementP[0] = (rotMatrix * (p - locpo));
  p << -m_boundValues[RotatedTrapezoidBounds::bv_halfX], -m_boundValues[RotatedTrapezoidBounds::bv_minHalfY];
  elementP[1] = (rotMatrix * (p - locpo));
  p << m_boundValues[RotatedTrapezoidBounds::bv_halfX], m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY];
  elementP[2] = (rotMatrix * (p - locpo));
  p << m_boundValues[RotatedTrapezoidBounds::bv_halfX], -m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY];
  elementP[3] = (rotMatrix * (p - locpo));
  std::vector<Amg::Vector2D> axis = { normal * (elementP[1] - elementP[0]),
                                      normal * (elementP[3] - elementP[1]),
                                      normal * (elementP[2] - elementP[0]) };
  bchk.ComputeKDOP(elementP, axis, elementKDOP);
  // compute KDOP for error ellipse
  std::vector<KDOP> errelipseKDOP(3);
  bchk.ComputeKDOP(bchk.EllipseToPoly(3), axis, errelipseKDOP);
  // check if KDOPs overlap and return result
  return bchk.TestKDOPKDOP(elementKDOP, errelipseKDOP);
}

inline bool
RotatedTrapezoidBounds::insideLoc1(const Amg::Vector2D& locpo, double tol1) const
{
  return (fabs(locpo[locX]) < m_boundValues[RotatedTrapezoidBounds::bv_halfX] + tol1);
}

inline bool
RotatedTrapezoidBounds::insideLoc2(const Amg::Vector2D& locpo, double tol2) const
{
  return (fabs(locpo[locY]) < m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] + tol2);
}
} // end of namespace

#endif // TRKSURFACES_TRAPEZOIDBOUNDS_H
