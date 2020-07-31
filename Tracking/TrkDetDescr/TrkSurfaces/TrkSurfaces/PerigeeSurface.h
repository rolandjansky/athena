/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PerigeeSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_PERIGEESURFACE_H
#define TRKSURFACES_PERIGEESURFACE_H

// Trk
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkSurfaces/NoBounds.h"
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
 @class PerigeeSurface

 Class describing the Line to which the Perigee refers to.
 The Surface axis is fixed to be the z-axis of the Tracking frame.
 It inherits from Surface.

 @author Andreas.Salzburger@cern.ch
 */

class PerigeeSurface : public Surface
{

public:
  /**Default Constructor - needed for persistency*/
  PerigeeSurface();

  /**Constructor from GlobalPosition*/
  PerigeeSurface(const Amg::Vector3D& gp);

  /**Constructor with a Transform - needed for tilt */
  PerigeeSurface(Amg::Transform3D* tTransform);

  /**Constructor with a Transform by unique_ptr - needed for tilt */
  PerigeeSurface(std::unique_ptr<Amg::Transform3D> tTransform);

  /**Copy constructor*/
  PerigeeSurface(const PerigeeSurface& pesf);

  /**Copy constructor with shift*/
  PerigeeSurface(const PerigeeSurface& pesf, const Amg::Transform3D& transf);

  /**Destructor*/
  virtual ~PerigeeSurface() = default;

  /**Virtual constructor*/
  virtual PerigeeSurface* clone() const override;

  /**Assignment operator*/
  PerigeeSurface& operator=(const PerigeeSurface& slsf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * charged */
  virtual ParametersT<5, Charged, PerigeeSurface>* createTrackParameters(
    double l1,
    double l2,
    double phi,
    double theta,
    double qop,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * charged*/
  virtual ParametersT<5, Charged, PerigeeSurface>* createTrackParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * neutral */
  virtual ParametersT<5, Neutral, PerigeeSurface>* createNeutralParameters(
    double l1,
    double l2,
    double phi,
    double theta,
    double qop,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * neutral */
  virtual ParametersT<5, Neutral, PerigeeSurface>* createNeutralParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(5) * cov = nullptr) const override final;

  /** Use the Surface as a ParametersBase constructor, from local parameters */
  template<int DIM, class T>
  ParametersT<DIM, T, PerigeeSurface>* createParameters(double l1,
                                                        double l2,
                                                        double phi,
                                                        double theta,
                                                        double qop,
                                                        AmgSymMatrix(DIM) *
                                                          cov = 0) const;

  /** Use the Surface as a ParametersBase constructor, from global parameters
   */
  template<int DIM, class T>
  ParametersT<DIM, T, PerigeeSurface>* createParameters(
    const Amg::Vector3D& position,
    const Amg::Vector3D& momentum,
    double charge,
    AmgSymMatrix(DIM) * cov = 0) const;

  /** Return the surface type */
  virtual SurfaceType type() const override final;

  /** Return the surface type static constexpr */
  static constexpr SurfaceType staticType() ;


  /**Return method for transfromation, overwrites the transform() form base
   * class*/
  virtual const Amg::Transform3D& transform() const override;

  /**Return method for surface center infromation, overwrites the center() form
   * base class*/
  virtual const Amg::Vector3D& center() const override;

  /**Return method for surface center infromation, overwrites the center() form
   * base class*/
  virtual const Amg::Vector3D& normal() const override;

  /**Returns a normal vector at a specific localPosition*/
  virtual const Amg::Vector3D* normal(const Amg::Vector2D& lp) const override;

  /** Return the measurement frame - this is needed for alignment, in particular
     for StraightLine and Perigee Surface
      - the default implementation is the the RotationMatrix3D of the transform
   */
  virtual Amg::RotationMatrix3D measurementFrame(
    const Amg::Vector3D& glopos,
    const Amg::Vector3D& glomom) const override;

  /** Local to global method:
      Take care that by just providing locR and locZ the global position cannot
     be calculated. Therefor only the locZ-coordinate is taken and transformed
     into the global frame. for calculating the global position, a momentum
     direction has to be provided as well, use the appropriate function!
       */
  const Amg::Vector3D* localToGlobal(
    const LocalParameters& locpos) const;

  /** This method is the true local->global transformation.<br>
      by providing a locR and locZ coordinate such as a GlobalMomentum
      the global position can be calculated.
      The choice between the two possible canditates is done by the sign of the
     radius
      */
  const Amg::Vector3D* localToGlobal(const LocalParameters& locpos,
                                     const Amg::Vector3D& glomom) const;

  /** LocalToGlobal method without dynamic memory allocation */
  virtual void localToGlobal(const Amg::Vector2D& locp,
                             const Amg::Vector3D& mom,
                             Amg::Vector3D& glob) const override final;

  /** GlobalToLocal method without dynamic memory allocation - boolean checks if
     on surface \image html SignOfDriftCircleD0.gif
  */
  virtual bool globalToLocal(const Amg::Vector3D& glob,
                             const Amg::Vector3D& mom,
                             Amg::Vector2D& loc) const override final;

  /** fast straight line intersection schema - standard: provides closest
     intersection and (signed) path length forceDir is to provide the closest
     forward solution

      b>mathematical motivation:</b>
      Given two lines in parameteric form:<br>
      - @f$ \vec l_{a}(\lambda) = \vec m_a + \lambda \cdot \vec e_{a} @f$ <br>
      - @f$ \vec l_{b}(\mu) = \vec m_b + \mu \cdot \vec e_{b} @f$ <br>
      the vector between any two points on the two lines is given by:
      - @f$ \vec s(\lambda, \mu) = \vec l_{b} - l_{a} = \vec m_{ab} + \mu \cdot
     \vec e_{b} - \lambda \cdot \vec e_{a}
     @f$, <br> when @f$ \vec m_{ab} = \vec m_{b} - \vec m_{a} @f$.<br>
      @f$ \vec s(\lambda_0, \mu_0) @f$  denotes the vector between the two
     closest points <br>
      @f$ \vec l_{a,0} = l_{a}(\lambda_0) @f$ and @f$ \vec l_{b,0} =
     l_{b}(\mu_0) @f$ <br> and is perpenticular to both, @f$ \vec e_{a} @f$ and
     @f$ \vec e_{b} @f$.

      This results in a system of two linear equations:<br>
      - (i) @f$ 0 = \vec s(\lambda_0, \mu_0) \cdot \vec e_a = \vec m_ab \cdot
     \vec e_a + \mu_0 \vec e_a \cdot \vec e_b - \lambda_0 @f$ <br>
      - (ii) @f$ 0 = \vec s(\lambda_0, \mu_0) \cdot \vec e_b = \vec m_ab \cdot
     \vec e_b + \mu_0  - \lambda_0 \vec e_b \cdot \vec e_a @f$ <br>

      Solving (i), (ii) for @f$ \lambda_0 @f$ and @f$ \mu_0 @f$ yields:
      - @f$ \lambda_0 = \frac{(\vec m_ab \cdot \vec e_a)-(\vec m_ab \cdot \vec
     e_b)(\vec e_a \cdot \vec e_b)}{1-(\vec e_a \cdot \vec e_b)^2} @f$ <br>
      - @f$ \mu_0 = - \frac{(\vec m_ab \cdot \vec e_b)-(\vec m_ab \cdot \vec
     e_a)(\vec e_a \cdot \vec e_b)}{1-(\vec e_a \cdot \vec e_b)^2} @f$ <br>
   */
  virtual Intersection straightLineIntersection(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool forceDir = false,
    Trk::BoundaryCheck bchk = false) const override final;

  /** fast straight line distance evaluation to Surface */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir) const override final;

  /** fast straight line distance evaluation to Surface - with bound option*/
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool Bound) const override final;

  /** the pathCorrection for derived classes with thickness */
  virtual double pathCorrection(const Amg::Vector3D&,
                                const Amg::Vector3D&) const override;

  /**This method checks if a globalPosition in on the Surface or not*/
  virtual bool isOnSurface(const Amg::Vector3D& glopo,
                           BoundaryCheck bchk = true,
                           double tol1 = 0.,
                           double tol2 = 0.) const override;

  /**This surface calls the iside method of the bounds*/
  virtual bool insideBounds(const Amg::Vector2D& locpos,
                            double tol1 = 0.,
                            double tol2 = 0.) const override;

  virtual bool insideBoundsCheck(const Amg::Vector2D& locpos,
                                 const BoundaryCheck& bchk) const override;

  /** Special method for StraightLineSurface - provides the Line direction from
   * cache: speedup */
  const Amg::Vector3D& lineDirection() const;

  /** Return bounds() method */
  virtual const NoBounds& bounds() const override;

  /** Return properly formatted class name for screen output */
  virtual std::string name() const override;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;
  /** Output Method for std::ostream*/
  virtual std::ostream& dump(std::ostream& sl) const override;

protected:
  //!< data members
  //!< cache of the line direction (speeds up)
  CxxUtils::CachedUniquePtrT<Amg::Vector3D> m_lineDirection;
  static const NoBounds s_perigeeBounds;
};

} // end of namespace
#include "TrkSurfaces/PerigeeSurface.icc"

#endif // TRKSURFACES_PERIGEESURFACE_H
