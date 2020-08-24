/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_CONESURFACE_H
#define TRKSURFACES_CONESURFACE_H

// Trk
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkSurfaces/ConeBounds.h"
#include "TrkSurfaces/Surface.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

class LocalParameters;
template<int DIM, class T, class S>
class ParametersT;

/**
 @class ConeSurface

 Class for a conical surface in the ATLAS detector.
 It inherits from Surface.

 The ConeSurface is special since no corresponding
 TrackParameters exist since they're numerical instable
 at the tip of the cone.
 Propagations to a cone surface will be returned in
 curvilinear coordinates.

 @author Ian.Watson@cern.ch, Andreas.Salzburger@cern.ch
 */

class ConeSurface : public Surface
{

public:
  /**Default Constructor*/
  ConeSurface();

  /**Constructor form HepTransform and an opening angle */
  ConeSurface(Amg::Transform3D* htrans, double alpha, bool symmetric = false);

  /**Constructor form HepTransform, radius halfphi, and halflenght*/
  ConeSurface(Amg::Transform3D* htrans,
              double alpha,
              double locZmin,
              double locZmax,
              double halfPhi = M_PI);

  /**Constructor from HepTransform and CylinderBounds
    - ownership of the bounds is passed
    */
  ConeSurface(Amg::Transform3D* htrans, ConeBounds* cbounds);

  /**Constructor from HepTransform by unique_ptr.
     - bounds is not set. */
  ConeSurface(std::unique_ptr<Amg::Transform3D> htrans);

  /**Copy constructor */
  ConeSurface(const ConeSurface& csf);

  /**Copy constructor with shift */
  ConeSurface(const ConeSurface& csf, const Amg::Transform3D& transf);

  /**Destructor*/
  virtual ~ConeSurface();

  /**Assignment operator*/
  ConeSurface& operator=(const ConeSurface& csf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override;

  /**Implicit Constructor*/
  virtual ConeSurface* clone() const override;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * charged */
  virtual ParametersT<5, Charged, ConeSurface>* createTrackParameters(
    double l1,
    double l2,
    double phi,
    double theta,
    double qop,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * charged*/
  virtual ParametersT<5, Charged, ConeSurface>* createTrackParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * neutral */
  virtual ParametersT<5, Neutral, ConeSurface>* createNeutralParameters(
    double l1,
    double l2,
    double phi,
    double theta,
    double qop,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * neutral */
  virtual ParametersT<5, Neutral, ConeSurface>* createNeutralParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from local parameters */
  template<int DIM, class T>
  ParametersT<DIM, T, ConeSurface>* createParameters(double l1,
                                                     double l2,
                                                     double phi,
                                                     double theta,
                                                     double qop,
                                                     AmgSymMatrix(DIM) * cov = 0) const;

  /** Use the Surface as a ParametersBase constructor, from global parameters */
  template<int DIM, class T>
  ParametersT<DIM, T, ConeSurface>* createParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(DIM) * cov = 0) const;

  /** Return the surface type */
  virtual SurfaceType type() const override final;

  /** Return the surface type static constexpr */
  static constexpr SurfaceType staticType() ;

  /** Return the measurement frame - this is needed for alignment, in particular
    for StraightLine and Perigee Surface
    - the default implementation is the the RotationMatrix3D of the transform */
  virtual Amg::RotationMatrix3D measurementFrame(
    const Amg::Vector3D& glopos,
    const Amg::Vector3D& glomom) const override;

  /** Returns a global reference point:
     For the Cylinder this is @f$ (R*cos(\phi), R*sin(\phi),0)*transform() @f$
     Where  @f$ \phi @f$ denotes the averagePhi() of the cylinderBounds.
    */
  virtual const Amg::Vector3D& globalReferencePoint() const override;

  /**Return method for surface normal information
     at a given local point, overwrites the normal() from base class.*/
  virtual const Amg::Vector3D& normal() const override;

  /**Return method for surface normal information
     at a given local point, overwrites the normal() from base class.*/
  virtual const Amg::Vector3D* normal(
    const Amg::Vector2D& locpo) const override;

  /**Return method for the rotational symmetry axis - the z-Axis of the
   * HepTransform */
  virtual const Amg::Vector3D& rotSymmetryAxis() const;

  /**This method returns the ConeBounds by reference
   (NoBounds is not possible for cone)*/
  virtual const ConeBounds& bounds() const override;

  /**This method calls the inside method of ConeBounds*/
  virtual bool insideBounds(const Amg::Vector2D& locpos,
                            double tol1 = 0.,
                            double tol2 = 0.) const override;
  virtual bool insideBoundsCheck(const Amg::Vector2D& locpos,
                                 const BoundaryCheck& bchk) const override;

  /** Specialized for ConeSurface : LocalParameters to Vector2D */
  virtual Amg::Vector2D localParametersToPosition(
    const LocalParameters& locpars) const override final;

  /** Specialized for ConeSurface : LocalToGlobal method without dynamic memory
   * allocation */
  virtual void localToGlobal(const Amg::Vector2D& locp,
                             const Amg::Vector3D& mom,
                             Amg::Vector3D& glob) const override final;

  /** Specialized for ConeSurface : GlobalToLocal method without dynamic memory
   * allocation - boolean checks if on surface */
  virtual bool globalToLocal(const Amg::Vector3D& glob,
                             const Amg::Vector3D& mom,
                             Amg::Vector2D& loc) const override final; 

  /** fast straight line intersection schema - provides closest intersection and
  (signed) path length

  <b>mathematical motivation:</b>

    The calculation will be done in the 3-dim frame of the cone,
    i.e. the symmetry axis of the cone is the z-axis, x- and y-axis are
  perpenticular to the the z-axis. In this frame the cone is centered around the
  origin. Therefore the two points describing the line have to be first
  recalculated into the new frame. Suppose, this is done, the points of
  intersection can be obtained as follows:<br>

    The cone is described by the implicit equation
    @f$x^2 + y^2 = z^2 \tan \alpha@f$
    where @f$\alpha@f$ is opening half-angle of the cone  the and
    the line by the parameter equation (with @f$t@f$ the
    parameter and @f$x_1@f$ and @f$x_2@f$ are points on the line)
    @f$(x,y,z) = \vec x_1 + (\vec x_2 - \vec x_2) t @f$.
    The intersection is the given to the value of @f$t@f$ where
    the @f$(x,y,z)@f$ coordinates of the line satisfy the implicit
    equation of the cone. Inserting the expression for the points
    on the line into the equation of the cone and rearranging to
    the form of a  gives (letting @f$ \vec x_d = \frac{\vec x_2 - \vec
    x_1}{\abs{\vec x_2 - \vec x_1}} @f$):
    @f$t^2 (x_d^2 + y_d^2 - z_d^2 \tan^2 \alpha) + 2 t (x_1 x_d +
    y_1 y_d - z_1 z_d \tan^2 \alpha) + (x_1^2 + y_1^2 - z_1^2
    \tan^2 \alpha) = 0 @f$
    Solving the above for @f$t@f$ and putting the values into the
    equation of the line gives the points of intersection. @f$t@f$
    is also the length of the path, since we normalized @f$x_d@f$
    to be unit length.
  */
  virtual Intersection straightLineIntersection(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool forceDir = false,
    BoundaryCheck bchk = false) const override final;

  /** fast straight line distance to Surface */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir) const override final;

  /** fast straight line distance to Surface - with bounds options */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool bound) const override final;

  /** the pathCorrection for derived classes with thickness */
  virtual double pathCorrection(const Amg::Vector3D&,
                                const Amg::Vector3D&) const override;

  /** Return properly formatted class name for screen output */
  virtual std::string name() const override;

protected:
  //!< bounds (shared)
  SharedObject<const ConeBounds> m_bounds;
  //!< The global reference point (== a point on thesurface)
  CxxUtils::CachedUniquePtrT<Amg::Vector3D> m_referencePoint;
  //!< The rotational symmetry axis
  CxxUtils::CachedUniquePtrT<Amg::Vector3D> m_rotSymmetryAxis;
};

} // end of namespace
#include "TrkSurfaces/ConeSurface.icc"

#endif // TRKSURFACES_CONESURFACE_H
