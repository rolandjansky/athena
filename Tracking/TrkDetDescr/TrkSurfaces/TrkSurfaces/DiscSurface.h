/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DISCSURFACE_H
#define TRKSURFACES_DISCSURFACE_H

// Trk
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// std
#include <cmath> //for cos, sin etc

class MsgStream;
template<class SURFACE, class BOUNDS_CNV>
class BoundSurfaceCnv_p1;

namespace Trk {

class DiscBounds;
class DiscTrapezoidalBounds;
class TrkDetElementBase;
class LocalParameters;

template<int DIM, class T, class S>
class ParametersT;

/**
 @class DiscSurface
 Class for a DiscSurface in the ATLAS detector.
 It inherits from Surface.

 @author Andreas.Salzburger@cern.ch
 */

class DiscSurface : public Surface
{

public:
  /**Default Constructor*/
  DiscSurface();

  /**Constructor for Discs from HepGeom::Transform3D, \f$ r_{min}, r_{max} \f$
   */
  DiscSurface(Amg::Transform3D* htrans, double rmin, double rmax);

  /**Constructor for Discs from HepGeom::Transform3D, \f$ r_{min}, r_{max},
   * \phi_{hsec} \f$ */
  DiscSurface(Amg::Transform3D* htrans,
              double rmin,
              double rmax,
              double hphisec);

  /**Constructor for Discs from HepGeom::Transform3D, \f$ r_{min}, r_{max},
     hx_{min}, hx_{max} \f$ In this case you have DiscTrapezoidalBounds*/
  DiscSurface(Amg::Transform3D* htrans,
              double minhalfx,
              double maxhalfx,
              double maxR,
              double minR,
              double avephi,
              double stereo = 0.);

  /**Constructor for Discs from HepGeom::Transform3D and DiscBounds
     - ownership of bounds is passed */
  DiscSurface(Amg::Transform3D* htrans, DiscBounds* dbounds);

  /**Constructor for Discs from HepGeom::Transform3D and DiscTrapezoidalBounds
     - ownership of bounds is passed */
  DiscSurface(Amg::Transform3D* htrans, DiscTrapezoidalBounds* dtbounds);

  /**Constructor for Discs from HepGeom::Transform3D by unique_ptr
   - bounds is not set */
  DiscSurface(std::unique_ptr<Amg::Transform3D> htrans);

  /**Constructor for DiscSegment from DetectorElement*/
  DiscSurface(const TrkDetElementBase& dmnt);

  /**Copy Constructor*/
  DiscSurface(const DiscSurface& psf);

  /**Copy Constructor with shift*/
  DiscSurface(const DiscSurface& psf, const Amg::Transform3D& transf);

  /**Destructor*/
  virtual ~DiscSurface() = default;

  /**Assignement operator*/
  DiscSurface& operator=(const DiscSurface& dsf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override;

  /** Virtual constructor*/
  virtual DiscSurface* clone() const override;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * charged */
  virtual ParametersT<5, Charged, DiscSurface>* createTrackParameters(
    double l1,
    double l2,
    double phi,
    double theta,
    double qop,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * charged*/
  virtual ParametersT<5, Charged, DiscSurface>* createTrackParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * neutral */
  virtual ParametersT<5, Neutral, DiscSurface>* createNeutralParameters(
    double l1,
    double l2,
    double phi,
    double theta,
    double qop,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * neutral */
  virtual ParametersT<5, Neutral, DiscSurface>* createNeutralParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from local parameters */
  template<int DIM, class T>
  ParametersT<DIM, T, DiscSurface>* createParameters(double l1,
                                                     double l2,
                                                     double phi,
                                                     double theta,
                                                     double qop,
                                                     AmgSymMatrix(DIM) *
                                                       cov = 0) const;

  /** Use the Surface as a ParametersBase constructor, from global parameters */
  template<int DIM, class T>
  ParametersT<DIM, T, DiscSurface>* createParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(DIM) * cov = 0) const;

  /** Return the surface type */
  virtual SurfaceType type() const override final;

  /** Return the surface type */
  static constexpr SurfaceType staticType();

  /** Returns a global reference point:
     For the Disc this is @f$ (R*cos(\phi), R*sin(\phi),0)*transform() @f$
     Where  @f$ r,  \phi @f$ denote the r(), averagePhi() of the Bounds.
   */
  virtual const Amg::Vector3D& globalReferencePoint() const override;

  /**This method returns the bounds by reference*/
  const SurfaceBounds& bounds() const override;

  /**This method calls the inside method of the bounds*/
  virtual bool insideBounds(const Amg::Vector2D& locpos,
                            double tol1 = 0.,
                            double tol2 = 0.) const override;
  virtual bool insideBoundsCheck(const Amg::Vector2D& locpos,
                                 const BoundaryCheck& bchk) const override;

  /** This method returns true if the GlobalPosition is on the Surface for both,
    within or without check of whether the local position is inside boundaries
    or not */
  virtual bool isOnSurface(const Amg::Vector3D& glopo,
                           BoundaryCheck bchk = true,
                           double tol1 = 0.,
                           double tol2 = 0.) const override;

  /** Specialized for DiscSurface : LocalParameters to Vector2D */
  virtual Amg::Vector2D localParametersToPosition(
    const LocalParameters& locpars) const override final;

  /** Specialized for DiscSurface: LocalToGlobal method without dynamic memory
   * allocation */
  virtual void localToGlobal(const Amg::Vector2D& locp,
                             const Amg::Vector3D& mom,
                             Amg::Vector3D& glob) const override;

  /** Specialized for DiscSurface: GlobalToLocal method without dynamic memory
   * allocation - boolean checks if on surface
   */
  virtual bool globalToLocal(const Amg::Vector3D& glob,
                             const Amg::Vector3D& mom,
                             Amg::Vector2D& loc) const override;

  /**  Special method for DiscSurface : local<->local transformations polar <->
   * cartesian */
  const Amg::Vector2D* localPolarToCartesian(const Amg::Vector2D& locpol) const;

  /**  Special method for Disc surface : local<->local transformations polar <->
   * cartesian */
  const Amg::Vector2D* localCartesianToPolar(
    const Amg::Vector2D& loccart) const;

  /**  Special method for Disc surface : local<->local transformations polar <->
   * cartesian by value*/
  Amg::Vector2D localCartesianToPolarValue(const Amg::Vector2D& loccart) const;

  /**  Special method for DiscSurface : local<->local transformations polar <->
   * cartesian */
  const Amg::Vector2D* localPolarToLocalCartesian(
    const Amg::Vector2D& locpol) const;

  /** Special method for DiscSurface :  local<->global transformation when
   * provided cartesian coordinates */
  const Amg::Vector3D* localCartesianToGlobal(
    const Amg::Vector2D& locpos) const;

  /** Special method for DiscSurface : global<->local from cartesian coordinates
   */
  const Amg::Vector2D* globalToLocalCartesian(const Amg::Vector3D& glopos,
                                              double tol = 0.) const;

  /** fast straight line intersection schema - standard: provides closest
     intersection and (signed) path length forceDir is to provide the closest
     forward solution

      <b>mathematical motivation:</b>

      the equation of the plane is given by: <br>
      @f$ \vec n \cdot \vec x = \vec n \cdot \vec p,@f$ <br>
      where @f$ \vec n = (n_{x}, n_{y}, n_{z})@f$ denotes the normal vector of
     the plane,
      @f$ \vec p = (p_{x}, p_{y}, p_{z})@f$ one specific point on the plane and
     @f$ \vec x = (x,y,z) @f$ all possible points on the plane.<br> Given a line
     with:<br>
      @f$ \vec l(u) = \vec l_{1} + u \cdot \vec v @f$, <br>
      the solution for @f$ u @f$ can be written:
      @f$ u = \frac{\vec n (\vec p - \vec l_{1})}{\vec n \vec v}@f$ <br>
      If the denominator is 0 then the line lies:
      - either in the plane
      - perpenticular to the normal of the plane
   */
  virtual Intersection straightLineIntersection(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool forceDir = false,
    Trk::BoundaryCheck bchk = false) const override final;

  /** fast straight line distance evaluation to Surface */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir) const override;

  /** fast straight line distance evaluation to Surface - with bound option*/
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool Bound) const override;

  /** Return properly formatted class name for screen output */
  virtual std::string name() const override;

protected: //!< data members
  template<class SURFACE, class BOUNDS_CNV>
  friend class ::BoundSurfaceCnv_p1;
  //!< bounds (shared)
  SharedObject<const SurfaceBounds> m_bounds;
  //!< reference Point on the Surface
  CxxUtils::CachedUniquePtrT<Amg::Vector3D> m_referencePoint;
  //!< static member for boundless approach
  static const NoBounds s_boundless;
};

} // end of namespace
#include "TrkSurfaces/DiscSurface.icc"

#endif // TRKSURFACES_DISCSURFACE_H
