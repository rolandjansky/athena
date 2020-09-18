/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Surface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_SURFACE_H
#define TRKSURFACES_SURFACE_H

// Trk
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/SurfaceUniquePtrT.h"
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/Neutral.h"
#include "TrkParametersBase/ParametersBase.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkSurfaces/DistanceSolution.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Identifier
#include "Identifier/Identifier.h"

#include "CxxUtils/CachedUniquePtr.h"
#include "CxxUtils/checker_macros.h"
#include <atomic>

class MsgStream;
class SurfaceCnv_p1;

namespace Trk {

class TrkDetElementBase;
class SurfaceBounds;
class Layer;

enum SurfaceOwner
{
  noOwn = 0,
  TGOwn = 1,
  DetElOwn = 2
};

/**
 @class Surface

 Abstract Base Class for tracking surfaces

 The creation of a Surface by passing a HepGeom::Transform3D* through the
 constructor implies that the ownership of the HepGeom::Transform3D object is
 also passed to the Surface, therfor the memory is freed in the Surface
 destructor.

 For all isOnSurface, or positionOnSurface and insideBounds methods two
 tolerance parameters can be given which correspond to the two local natural
 coordinates of the surface loc1, loc2.

 @author Andreas.Salzburger@cern.ch
 */

class Surface
{

  /** Declare the ILayerBuilder / ITrackingVolumeHelper to be a friend class
   * such it is able to set the layer */
  friend class ILayerBuilder;
  friend class ITrackingVolumeHelper;

public:
  /** @enum SurfaceType

      This enumerator simplifies the persistency & calculations,
      by saving a dynamic_cast to happen.

      Other is reserved for the GeometrySurfaces implementation.

    */
  enum SurfaceType
  {
    Cone = 0,
    Cylinder = 1,
    Disc = 2,
    Perigee = 3,
    Plane = 4,
    Line = 5,
    Curvilinear = 6,
    Other = 7
  };

  /**Default Constructor
   - needed for inherited classes */
  Surface();

  /**Copy constructor - it resets the associated
   detector element to 0 and the identifier to invalid,
   as the copy cannot be owned by the same detector element as the original */
  Surface(const Surface& sf);

  /**Copy constructor with shift */
  Surface(const Surface& sf, const Amg::Transform3D& transf);

  /**Constructor with HepGeom::Transform3D, passing ownership */
  Surface(Amg::Transform3D* htrans);

  /**Constructor with HepGeom::Transform3D, by unique_ptr */
  Surface(std::unique_ptr<Amg::Transform3D> htrans);

  /**Constructor from TrkDetElement*/
  Surface(const TrkDetElementBase& detelement);

  /**Constructor form TrkDetElement and Identifier*/
  Surface(const TrkDetElementBase& detelement, const Identifier& id);

  /**Virtual Destructor*/
  virtual ~Surface();

  /**Assignment operator- it sets the associated
   detector element to 0 and the associated identifier to invalid,
   as the copy cannot be owned by the same detector element as the original */
  Surface& operator=(const Surface& sf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const = 0;

  /**Non-equality operator*/
  virtual bool operator!=(const Surface& sf) const;

  /**Implicit constructor - uses the copy constructor */
  virtual Surface* clone() const = 0;

  /** Returns the Surface type to avoid dynamic casts */
  virtual SurfaceType type() const = 0;

  /** Return the cached transformation directly.  Don't try to make
      a new transform if it's not here. */
  const Amg::Transform3D* cachedTransform() const;

  /** Returns HepGeom::Transform3D by reference */
  virtual const Amg::Transform3D& transform() const;

  /** Returns the center position of the Surface */
  virtual const Amg::Vector3D& center() const;

  /** Returns the normal vector of the Surface (i.e. in generall z-axis of
   * rotation) */
  virtual const Amg::Vector3D& normal() const;

  /** Returns a normal vector at a specific local position
   */
  virtual const Amg::Vector3D* normal(const Amg::Vector2D& lp) const;

  /** Returns a global reference point on the surface,
     for PlaneSurface, StraightLineSurface, PerigeeSurface this is equal to
     center(), for CylinderSurface and DiscSurface this is a new member
     */
  virtual const Amg::Vector3D& globalReferencePoint() const;

  /** return associated Detector Element */
  const TrkDetElementBase* associatedDetectorElement() const;

  /** return Identifier of the associated Detector Element */
  Identifier associatedDetectorElementIdentifier() const;

  /** return the associated Layer */
  const Trk::Layer* associatedLayer() const;

  /** return the material Layer */
  const Trk::Layer* materialLayer() const;

  /** return the base surface (simplified for persistification) */
  virtual const Trk::Surface* baseSurface() const;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * charged. The caller assumes ownership of the returned ptr.
   */
  virtual ParametersBase<5, Trk::Charged>* createTrackParameters(
    double,
    double,
    double,
    double,
    double,
    AmgSymMatrix(5) * cov = nullptr) const = 0;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * charged  The caller assumes ownership of the returned ptr
   */
  virtual ParametersBase<5, Trk::Charged>* createTrackParameters(
    const Amg::Vector3D&,
    const Amg::Vector3D&,
    double,
    AmgSymMatrix(5) * cov = nullptr) const = 0;

  /** Use the Surface as a ParametersBase constructor, from local parameters -
   * neutral.  The caller assumes ownership of the returned ptr
   */
  virtual ParametersBase<5, Trk::Neutral>* createNeutralParameters(
    double,
    double,
    double,
    double,
    double,
    AmgSymMatrix(5) * cov = nullptr) const = 0;

  /** Use the Surface as a ParametersBase constructor, from global parameters -
   * neutral. The caller assumes ownership of the returned ptr
   */
  virtual ParametersBase<5, Trk::Neutral>* createNeutralParameters(
    const Amg::Vector3D&,
    const Amg::Vector3D&,
    double charge = 0.,
    AmgSymMatrix(5) * cov = nullptr) const = 0;

  /** positionOnSurface() returns a pointer to a LocalPosition on the
    Surface,<br>
    If BoundaryCheck==false it just returns the value of
    globalToLocal (including nullptr possibility),
    if BoundaryCheck==true
    it checks whether the point is inside bounds or not (returns nullptr
    pointer in this case).
    The caller assumes ownership of the returned ptr.
    */
  Amg::Vector2D* positionOnSurface(const Amg::Vector3D& glopo,
                                   const BoundaryCheck& bchk = true,
                                   double tol1 = 0.,
                                   double tol2 = 0.) const;

  /** The templated Parameters OnSurface method - checks on surface pointer
   * first */
  template<class T>
  bool onSurface(const T& parameters,
                 const BoundaryCheck& bchk = BoundaryCheck(true)) const;

  /** This method returns true if the GlobalPosition is on the Surface for both,
    within or without check of whether the local position is inside boundaries
    or not */
  virtual bool isOnSurface(const Amg::Vector3D& glopo,
                           BoundaryCheck bchk = true,
                           double tol1 = 0.,
                           double tol2 = 0.) const;

  /**  virtual methods to be overwritten by the inherited surfaces */
  virtual bool insideBounds(const Amg::Vector2D& locpos,
                            double tol1 = 0.,
                            double tol2 = 0.) const = 0;

  virtual bool insideBoundsCheck(const Amg::Vector2D& locpos,
                                 const BoundaryCheck& bchk) const = 0;

  /** Specified by each surface type: LocalToGlobal method without dynamic
   * memory allocation */
  virtual void localToGlobal(const Amg::Vector2D& locp,
                             const Amg::Vector3D& mom,
                             Amg::Vector3D& glob) const = 0;

  /** This method returns the GlobalPosition from a LocalPosition
   * uses the per surface localToGlobal and performs dynamic allocations.
   * The caller is responsible for deleting the ptr.
   */
  Amg::Vector3D* localToGlobal(const Amg::Vector2D& locpos) const;

  /** This method returns the GlobalPosition from a LocalPosition
   * uses the per surface localToGlobal. Return by value
   */
  Amg::Vector3D localToGlobalPos(const Amg::Vector2D& locpos) const;

  /** This method returns the GlobalPosition from a LocalPosition
   * The LocalPosition can be outside Surface bounds - for generality with
   * momentum
   * Uses the per surface localToGlobal and performs dynamic allocations.
   * The caller is responsible for deleting the ptr.
   */

  Amg::Vector3D* localToGlobal(const Amg::Vector2D& locpos,
                               const Amg::Vector3D& glomom) const;

  /** This method returns the GlobalPosition from a LocalPosition
   * The LocalPosition can be outside Surface bounds - for generality with
   * momentum
   * Return by value No memeory allocation
   */
  Amg::Vector3D localToGlobalPos(const Amg::Vector2D& locpos,
                                 const Amg::Vector3D& glomom) const;

  /** This method returns the GlobalPosition from LocalParameters
   * The LocalParameters can be outside Surface bounds.
   * Uses the per surface localToGlobal and performs dynamic allocations.
   * The caller is responsible for deleting the ptr.
   */
  Amg::Vector3D* localToGlobal(const LocalParameters& locpars) const;

  /** This method returns the GlobalPosition from LocalParameters
   * The LocalParameters can be outside Surface bounds.
   * Return by value no memory allocation
   */
  Amg::Vector3D localToGlobalPos(const LocalParameters& locpars) const;

  /** This method returns the GlobalPosition from LocalParameters
   * The LocalParameters can be outside Surface bounds - for generality with
   * momentum
   * Uses the per surface localToGlobal and performs dynamic allocations.
   * The caller is responsible for deleting the ptr.
   */
  Amg::Vector3D* localToGlobal(const LocalParameters& locpars,
                               const Amg::Vector3D& glomom) const;

  /** This method returns the GlobalPosition from LocalParameters
   * The LocalParameters can be outside Surface bounds - for generality with
   * momentum
   * Return by value no memory allocation
   */
  Amg::Vector3D localToGlobalPos(const LocalParameters& locpars,
                                 const Amg::Vector3D& glomom) const;

  /** Specified by each surface type: GlobalToLocal method without dynamic
   * memory allocation - boolean checks if on surface */
  virtual bool globalToLocal(const Amg::Vector3D& glob,
                             const Amg::Vector3D& mom,
                             Amg::Vector2D& loc) const = 0;

  /** This method returns the LocalPosition from a provided GlobalPosition.
    If the GlobalPosition is not on the Surface, it returns a NULL pointer.
    This method does not check if the calculated LocalPosition is inside surface
    bounds. If this check is needed, use positionOnSurface - only for planar,
    cylinder surface fully defined*/
  Amg::Vector2D* globalToLocal(const Amg::Vector3D& glopos,
                               double tol = 0.) const;

  /** This method returns the LocalPosition from a provided GlobalPosition.
      If the GlobalPosition is not on the Surface, it returns a NULL pointer.
      This method does not check if the calculated LocalPosition is inside
     surface bounds. If this check is needed, use positionOnSurface - for
     generality with momentum */
  Amg::Vector2D* globalToLocal(const Amg::Vector3D& glopos,
                               const Amg::Vector3D& glomom) const;

  /** Optionally specified by each surface type : LocalParameters to Vector2D */
  virtual Amg::Vector2D localParametersToPosition(
    const LocalParameters& locpars) const;

  /** the pathCorrection for derived classes with thickness - it reflects if the
   * direction projection is positive or negative */
  virtual double pathCorrection(const Amg::Vector3D& pos,
                                const Amg::Vector3D& mom) const;

  /** Return the measurement frame - this is needed for alignment, in particular
     for StraightLine and Perigee Surface
       - the default implementation is the the RotationMatrix3D of the transform
   */
  virtual Amg::RotationMatrix3D measurementFrame(
    const Amg::Vector3D& glopos,
    const Amg::Vector3D& glomom) const;

  /** fst straight line intersection schema - templated for cvharged and neutral
   * parameters */
  template<class T>
  Intersection straightLineIntersection(const T& pars,
                                        bool forceDir = false,
                                        Trk::BoundaryCheck bchk = false) const
  {
    return straightLineIntersection(
      pars.position(), pars.momentum().unit(), forceDir, bchk);
  }

  /** fast straight line intersection schema - standard: provides closest
     intersection and (signed) path length forceFwd is to provide the closest
     forward solution
   */
  virtual Intersection straightLineIntersection(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool forceDir = false,
    Trk::BoundaryCheck bchk = false) const = 0;

  /** fast straight line distance evaluation to Surface */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir) const = 0;

  /** fast straight line distance evaluation to Surface - with bound option*/
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool Bound) const = 0;

  /** Surface Bounds method */
  virtual const SurfaceBounds& bounds() const = 0;

  /** Returns 'true' if this surface is 'free', i.e. it does not belong to a
   * detector element (and returns false otherwise*/
  bool isFree() const;

  /** Return 'true' if this surface is own by the detector element */
  bool isActive() const;

  /** Set ownership for const*/
  void setOwner ATLAS_NOT_CONST_THREAD_SAFE(SurfaceOwner x) const;

  /** set Ownership */
  void setOwner(SurfaceOwner x);

  /** return ownership */
  SurfaceOwner owner() const;

  /** set material layer */
  void setMaterialLayer(const Layer& mlay);

  void setMaterialLayer(const Layer* mlay);

  /** set material layer const */
  void setMaterialLayer ATLAS_NOT_CONST_THREAD_SAFE(const Layer& mlay) const;
  void setMaterialLayer ATLAS_NOT_CONST_THREAD_SAFE(const Layer* mlay) const;

  /** Output Method for MsgStream, to be overloaded by child classes */
  virtual MsgStream& dump(MsgStream& sl) const;

  /** Output Method for std::ostream, to be overloaded by child classes */
  virtual std::ostream& dump(std::ostream& sl) const;

  /** Return properly formatted class name */
  virtual std::string name() const = 0;

  /**return number of surfaces currently created - needed for EDM monitor */
  static unsigned int numberOfInstantiations();
  /**return number of free surfaces currently created (i.e. those not belonging
   * to a DE) - needed for EDM monitor */
  static unsigned int numberOfFreeInstantiations();

  /** method to associate the associated Trk::Layer which is alreay owned
     - only allowed by LayerBuilder
     - only done if no Layer is set already  */
  void associateLayer(const Layer& lay);

  /** const method to associate the associated Trk::Layer which is alreay owned
   - only allowed by LayerBuilder
   - only done if no Layer is set already  */
  void associateLayer ATLAS_NOT_CONST_THREAD_SAFE(const Layer& lay) const;

protected:
  friend class ::SurfaceCnv_p1;

  /** Private members are in principle implemented as pointers to
   * objects for easy checks if they are already declared or not */
  std::unique_ptr<Amg::Transform3D>
    m_transform; //!< Transform3D to orient surface w.r.t to global frame
  CxxUtils::CachedUniquePtr<Amg::Vector3D>
    m_center; //!< center position of the surface
  CxxUtils::CachedUniquePtr<Amg::Vector3D>
    m_normal; //!< normal vector of the surface

  /** Pointers to the a TrkDetElementBase */
  const TrkDetElementBase* m_associatedDetElement;
  Identifier m_associatedDetElementId;

  /**The associated layer Trk::Layer
   - layer in which the Surface is be embedded
   */
  const Layer* m_associatedLayer;

  /** Possibility to attach a material descrption
  - potentially given as the associated material layer
    don't delete, it's the TrackingGeometry's job to do so
  */
  const Layer* m_materialLayer;

  /** enum for surface owner : 0  free surface */
  SurfaceOwner m_owner;

  /**Tolerance for being on Surface */
  const static double s_onSurfaceTolerance;
#ifndef NDEBUG
  /** number of objects of this type in memory - needed for EDM monitor*/
  static std::atomic<unsigned int> s_numberOfInstantiations;

  /** number of objects of this type in memory which do not belong to a detector
   * element - needed for EDM monitor*/
  static std::atomic<unsigned int> s_numberOfFreeInstantiations;
#endif
};
/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
operator<<(MsgStream& sl, const Surface& sf);
std::ostream&
operator<<(std::ostream& sl, const Surface& sf);

typedef SurfaceUniquePtrT<Trk::Surface> SurfaceUniquePtr;
typedef SurfaceUniquePtrT<const Trk::Surface> ConstSurfaceUniquePtr;
} //
#include "Surface.icc"
#endif // TRKSURFACES_SURFACE_H
