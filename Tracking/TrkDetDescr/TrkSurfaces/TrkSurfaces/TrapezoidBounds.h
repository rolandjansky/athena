/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrapezoidBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_TRAPEZOIDBOUNDS_H
#define TRKSURFACES_TRAPEZOIDBOUNDS_H

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include <math.h>

#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
class TrapezoidBoundsCnv_p1;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
 @class TrapezoidBounds
 Bounds for a trapezoidal, planar Surface.

 @image html TrapezoidalBounds.gif
    <br>
 @image html SCT_Trapezoid.gif

 @todo can be speed optimized by calculating kappa/delta and caching it

 @author Andreas.Salzburger@cern.ch
 */

class TrapezoidBounds final: public SurfaceBounds
{

public:
  /** @enum BoundValues - for readability */
  enum BoundValues
  {
    bv_minHalfX = 0,
    bv_maxHalfX = 1,
    bv_halfY = 2,
    bv_length = 3
  };

  /**Default Constructor, needed for persistency*/
  TrapezoidBounds();

  /**Copy constructor*/
  TrapezoidBounds(const TrapezoidBounds& trabo) = default;

  /**Assignment operator*/
  TrapezoidBounds& operator=(const TrapezoidBounds& sbo) = default;

  /**Move constructor*/
  TrapezoidBounds(TrapezoidBounds&& trabo) noexcept = default;

  /**Move Assignment operator*/
  TrapezoidBounds& operator=(TrapezoidBounds&& sbo) noexcept = default;

  /**Destructor*/
  virtual ~TrapezoidBounds() = default;


  /**Constructor for symmetric Trapezoid*/
  TrapezoidBounds(double minhalex, double maxhalex, double haley);

  /**Constructor for arbitrary Trapezoid*/
  TrapezoidBounds(double minhalex, double haley, double alpha, double beta);

  /**Virtual constructor*/
  virtual TrapezoidBounds* clone() const override;

  /** Return the type of the bounds for persistency */
  virtual BoundsType type() const override { return SurfaceBounds::Trapezoid; }

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& trabo) const override;

  /**This method returns the minimal halflength in phi (first coordinate of local surface frame)*/
  double minHalflengthPhi() const;

  /**This method returns the maximal halflength in phi (first coordinate of local surface frame)*/
  double maxHalflengthPhi() const;

  /**This method returns the halflength in eta (second coordinate of local surface frame)*/
  double halflengthEta() const;

  /**This method returns the minimal halflength in X (first coordinate of local surface frame)*/
  double minHalflengthX() const;

  /**This method returns the maximal halflength in X (first coordinate of local surface frame)*/
  double maxHalflengthX() const;

  /**This method returns the halflength in Y (second coordinate of local surface frame)*/
  double halflengthY() const;

  /**This method returns the maximal extension on the local plane*/
  virtual double r() const override;

  /**This method returns the opening angle alpha in point A (negative local phi) */
  double alpha() const;

  /**This method returns the opening angle beta in point B (positive local phi) */
  double beta() const;

  /** The orientation of the Trapezoid is according to the figure above,
   in words: the shorter of the two parallel sides of the trapezoid intersects
   with the negative @f$ y @f$ - axis of the local frame.

   <br>
   The cases are:<br>
   (0) @f$ y @f$ or @f$ x @f$ bounds are 0 || 0<br>
   (1) LocalPosition is outside @f$ y @f$ bounds <br>
   (2) LocalPosition is inside @f$ y @f$ bounds, but outside maximum @f$ x @f$ bounds  <br>
   (3) LocalPosition is inside @f$ y @f$ bounds AND inside minimum @f$ x @f$ bounds <br>
   (4) LocalPosition is inside @f$ y @f$ bounds AND inside maximum @f$ x @f$ bounds, so that
   it depends on the @f$ eta @f$ coordinate
   (5) LocalPosition fails test of (4) <br>

   The inside check is done using single equations of straight lines and one has to take care if a point
   lies on the positive @f$ x @f$ half area(I) or the negative one(II). Denoting @f$ |x_{min}| @f$ and
   @f$ | x_{max} | @f$ as \c minHalfX respectively \c maxHalfX, such as @f$ | y_{H} | @f$ as \c halfY,
   the equations for the straing lines in (I) and (II) can be written as:<br>
    <br>
   - (I):  @f$ y = \kappa_{I} x + \delta_{I} @f$ <br>
   - (II): @f$ y = \kappa_{II} x + \delta_{II} @f$ ,<br>
    <br>
   where @f$  \kappa_{I} = - \kappa_{II} = 2 \frac{y_{H}}{x_{max} - x_{min}} @f$ <br>
   and   @f$  \delta_{I} = \delta_{II} = - \frac{1}{2}\kappa_{I}(x_{max} + x_{min}) @f$  */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  /** This method checks inside bounds in loc1
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle ! */

  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc2
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !  */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  friend class ::TrapezoidBoundsCnv_p1;

  /** inside() method for a full symmetric trapezoid */
  bool insideFull(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const;

  /** inside() method for the triangular exclude area for an arbitrary trapezoid */
  bool insideExclude(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const;

  /** isAbove() method for checking whether a point lies above or under a straight line */
  static bool isAbove(const Amg::Vector2D& locpo, double tol1, double tol2, double k, double d) ;

  std::vector<TDD_real_t> m_boundValues;
  TDD_real_t m_alpha;
  TDD_real_t m_beta;
};

} // end of namespace
#include "TrkSurfaces/TrapezoidBounds.icc"
#endif // TRKSURFACES_TRAPEZOIDBOUNDS_H
