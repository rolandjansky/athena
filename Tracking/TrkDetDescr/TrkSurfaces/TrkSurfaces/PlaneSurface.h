/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlaneSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_PLANESURFACE_H
#define TRKSURFACES_PLANESURFACE_H

//Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkParametersBase/ParametersT.h"
// Amg
#include "EventPrimitives/EventPrimitives.h" 
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
class Identifier;
template< class SURFACE, class BOUNDS_CNV > class BoundSurfaceCnv_p1;

namespace Trk {

  class LocalDirection;
  class LocalParameters;
  class TrkDetElementBase;
  class RectangleBounds;
  class TriangleBounds;
  class AnnulusBounds;
  class TrapezoidBounds;
  class RotatedTrapezoidBounds;
  class DiamondBounds;
  class EllipseBounds;
  class CurvilinearUVT;
  template<int DIM, class T, class S> class ParametersT;
  
  /**
   @class PlaneSurface
   Class for a planaer rectangular or trapezoidal surface in the ATLAS detector.
   It inherits from Surface.
   
   The Trk::PlaneSurface extends the Surface class with the possibility to convert 
   in addition to local to global positions, also local to global direction (vice versa).
   The definition with of a local direciton with respect to a plane can be found in
   the dedicated Trk::LocalDirection class of the TrkEventPrimitives package.
 
   @image html PlaneSurface.gif
    
   @author Andreas.Salzburger@cern.ch
   */

  class PlaneSurface : public Surface {
    public:
	 
      /** Default Constructor - needed for persistency*/
      PlaneSurface();
        
      /** Copy Constructor*/
      PlaneSurface(const PlaneSurface& psf);
      
      /** Copy Constructor with shift*/
      PlaneSurface(const PlaneSurface& psf, const Amg::Transform3D& transf);
      
      /** Dedicated Constructor with CurvilinearUVT class */
      PlaneSurface(const Amg::Vector3D& position, const CurvilinearUVT& curvUVT);
      
      /** Constructor from TrkDetElementBase*/
      PlaneSurface(const TrkDetElementBase& detelement,Amg::Transform3D* transf=nullptr);
      
      /** Constructor from TrkDetElementBase and Identifier in case one element holds more surfaces*/
      PlaneSurface(const TrkDetElementBase& detelement, const Identifier& id,Amg::Transform3D* transf=nullptr); 
      
      /** Constructor for planar Surface without Bounds */
      PlaneSurface(Amg::Transform3D* htrans);
      
      /** Constructor for planar Surface from unique_ptr without Bounds */
      PlaneSurface(std::unique_ptr<Amg::Transform3D> htrans);
      
      /** Constructor for Rectangular Planes*/      
      PlaneSurface(Amg::Transform3D* htrans, double halephi, double haleta);
      
      /** Constructor for Trapezoidal Planes*/
      PlaneSurface(Amg::Transform3D* htrans, double minhalephi, double maxhalephi, double haleta);
      
      /** Constructor for Planes with provided RectangleBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, RectangleBounds* rbounds);
      
      /** Constructor for Planes with provided TriangleBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, TriangleBounds* rbounds);
      
      /** Constructor for Planes with provided AnnulusBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, AnnulusBounds* rbounds);   
   
      /** Constructor for Planes with provided TrapezoidBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, TrapezoidBounds* rbounds);
      
      /** Constructor for Planes with provided RotatedTrapezoidBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, RotatedTrapezoidBounds* rbounds);
      
      /** Constructor for Planes with provided DiamondBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, DiamondBounds* rbounds);
      
      /** Constructor for Planes with provided EllipseBounds - ownership of bounds is passed*/
      PlaneSurface(Amg::Transform3D* htrans, EllipseBounds* rbounds);
      
      /** Constructor for Planes with shared object*/
      PlaneSurface(Amg::Transform3D* htrans, Trk::SharedObject<const Trk::SurfaceBounds>& sbounds);
      
      /**Destructor*/
      virtual ~PlaneSurface();
      
      /**Assignment operator*/
      PlaneSurface& operator=(const PlaneSurface& psf);
      
      /**Equality operator*/
      virtual bool operator==(const Surface& sf) const override;
      
      /**Virtual constructor*/
      virtual PlaneSurface* clone() const override;
      
      /** Return the surface type */
      virtual SurfaceType type() const override { return Surface::Plane; }
      
      /** Use the Surface as a ParametersBase constructor, from local parameters - charged */
      virtual const ParametersT<5, Charged, PlaneSurface>* createTrackParameters(double l1,
                                                                                 double l2,
                                                                                 double phi,
                                                                                 double theta,
                                                                                 double qop,
                                                                                 AmgSymMatrix(5)* cov = 0) const override
       { return new ParametersT<5, Charged, PlaneSurface>(l1, l2, phi, theta, qop, *this, cov); }


      /** Use the Surface as a ParametersBase constructor, from global parameters - charged*/
      virtual const ParametersT<5, Charged, PlaneSurface>* createTrackParameters(const Amg::Vector3D& position,
                                                                                 const Amg::Vector3D& momentum,
                                                                                 double charge,
                                                                                 AmgSymMatrix(5)* cov = 0) const override
       { return new ParametersT<5, Charged, PlaneSurface>(position, momentum, charge, *this, cov); }   
      
      /** Use the Surface as a ParametersBase constructor, from local parameters - neutral */
      virtual const ParametersT<5, Neutral, PlaneSurface>* createNeutralParameters(double l1,
                                                                                   double l2,
                                                                                   double phi,
                                                                                   double theta,
                                                                                   double oop,
                                                                                   AmgSymMatrix(5)* cov = 0) const override
        { return new ParametersT<5, Neutral, PlaneSurface>(l1, l2, phi, theta, oop, *this, cov); }
      
      /** Use the Surface as a ParametersBase constructor, from global parameters - neutral */
      virtual const ParametersT<5, Neutral, PlaneSurface>* createNeutralParameters(const Amg::Vector3D& position,
                                                                                   const Amg::Vector3D& momentum,
                                                                                   double charge = 0.,
                                                                                   AmgSymMatrix(5)* cov = 0) const override
       { return new ParametersT<5, Neutral, PlaneSurface>(position, momentum, charge, *this, cov); }
      
      /** Use the Surface as a ParametersBase constructor, from local parameters */
      template <int DIM, class T> const ParametersT<DIM, T, PlaneSurface>* createParameters(double l1,
                                                                                            double l2,
                                                                                            double phi,
                                                                                            double theta,
                                                                                            double qop,
                                                                                            AmgSymMatrix(DIM)* cov = 0) const
        { return new ParametersT<DIM, T, PlaneSurface>(l1, l2, phi, theta, qop, *this, cov); }

      /** Use the Surface as a ParametersBase constructor, from global parameters */
      template <int DIM, class T> const ParametersT<DIM, T, PlaneSurface>* createParameters(const Amg::Vector3D& position,
                                                                                            const Amg::Vector3D& momentum,
                                                                                            double charge,
                                                                                            AmgSymMatrix(DIM)* cov = 0) const
        { return new ParametersT<DIM, T, PlaneSurface>(position, momentum, charge, *this, cov); }
      
      /**This method returns the bounds by reference, static NoBounds in case of no boundaries*/
      virtual const SurfaceBounds& bounds() const override;
           
      /**This method calls the inside() method of the Bounds*/
      virtual bool insideBounds(const Amg::Vector2D& locpos, double tol1=0., double tol2=0.) const override;
      
      virtual bool insideBoundsCheck(const Amg::Vector2D& locpos, const BoundaryCheck& bchk) const override;                        
	  
      /** This method returns true if the GlobalPosition is on the Surface for both, within
        or without check of whether the local position is inside boundaries or not */
      virtual bool isOnSurface(const Amg::Vector3D& glopo, BoundaryCheck bchk=true, double tol1=0., double tol2=0.) const override;
      
      /** Specified for PlaneSurface: LocalToGlobal method without dynamic memory allocation */
      virtual void localToGlobal(const Amg::Vector2D& locp, const Amg::Vector3D& mom, Amg::Vector3D& glob) const override;
      
      /** Specified for PlaneSurface: GlobalToLocal method without dynamic memory allocation - boolean checks if on surface */
      virtual bool globalToLocal(const Amg::Vector3D& glob, const Amg::Vector3D& mom, Amg::Vector2D& loc) const override;
      
      /** This method transforms a local direction wrt the plane to a global direction */
      void localToGlobalDirection(const Trk::LocalDirection& locdir, Amg::Vector3D& globdir) const;
            
      /**This method transforms the global direction to a local direction wrt the plane */
      void globalToLocalDirection(const Amg::Vector3D& glodir, Trk::LocalDirection& locdir ) const;
      
      /** fast straight line intersection schema - standard: provides closest intersection and (signed) path length
          forceDir is to provide the closest forward solution
          
          <b>mathematical motivation:</b>
                 
          the equation of the plane is given by: <br>
          @f$ \vec n \cdot \vec x = \vec n \cdot \vec p,@f$ <br>
          where @f$ \vec n = (n_{x}, n_{y}, n_{z})@f$ denotes the normal vector of the plane,
          @f$ \vec p = (p_{x}, p_{y}, p_{z})@f$ one specific point on the plane and @f$ \vec x = (x,y,z) @f$ all possible points
          on the plane.<br>
          Given a line with:<br>
          @f$ \vec l(u) = \vec l_{1} + u \cdot \vec v @f$, <br>
          the solution for @f$ u @f$ can be written:
          @f$ u = \frac{\vec n (\vec p - \vec l_{1})}{\vec n \vec v}@f$ <br>
          If the denominator is 0 then the line lies:
          - either in the plane
          - perpenticular to the normal of the plane          
          
       */
      virtual Intersection straightLineIntersection(const Amg::Vector3D& pos, 
                                                           const Amg::Vector3D& dir, 
                                                           bool forceDir,
                                                           Trk::BoundaryCheck bchk) const override;
      
      /** fast straight line distance evaluation to Surface */
      virtual DistanceSolution straightLineDistanceEstimate(const Amg::Vector3D& pos,const Amg::Vector3D& dir) const override;

      /** fast straight line distance evaluation to Surface - with bound option*/
      virtual DistanceSolution straightLineDistanceEstimate(const Amg::Vector3D& pos,const Amg::Vector3D& dir, bool Bound) const override;
      
      /** Return properly formatted class name for screen output */
      virtual std::string name() const  override { return "Trk::PlaneSurface"; }
      
    protected: //!< data members
      template< class SURFACE, class BOUNDS_CNV > friend class ::BoundSurfaceCnv_p1;

      mutable SharedObject<const SurfaceBounds>  m_bounds;      //!< bounds (shared)        
      static NoBounds                            s_boundless;   //!< NoBounds as return object when no bounds are declared

    };

  
  inline PlaneSurface* PlaneSurface::clone() const { return new PlaneSurface(*this);}
 
  inline bool PlaneSurface::insideBounds(const Amg::Vector2D& locpos,
                                         double tol1,
                                         double tol2) const
  { return (bounds().inside(locpos,tol1,tol2));}
  
  inline bool PlaneSurface::insideBoundsCheck(const Amg::Vector2D& locpos, const BoundaryCheck& bchk) const
  { return (bounds().inside(locpos,bchk));}

  inline const SurfaceBounds& PlaneSurface::bounds() const
  {
    if (m_bounds.getPtr()) return (m_bounds.getRef());
    if (Surface::m_associatedDetElement && Surface::m_associatedDetElementId.is_valid()){ 
     return m_associatedDetElement->bounds(Surface::m_associatedDetElementId);
    }
    if (Surface::m_associatedDetElement) return m_associatedDetElement->bounds();  
    return s_boundless;   
  }


  inline Intersection PlaneSurface::straightLineIntersection(const Amg::Vector3D& pos, 
                                                                    const Amg::Vector3D& dir, 
                                                                    bool forceDir,
                                                                    Trk::BoundaryCheck bchk) const
  {
      double denom = dir.dot(normal());
      if (denom){
        double u = (normal().dot((center()-pos)))/(denom);
        Amg::Vector3D intersectPoint(pos + u * dir);
        // evaluate the intersection in terms of direction
        bool isValid = forceDir ?  ( u > 0.) : true;  
        // evaluate (if necessary in terms of boundaries)
        isValid = bchk ? (isValid && isOnSurface(intersectPoint) ) : isValid;
        // return the result
        return Trk::Intersection(intersectPoint,u,isValid);
      }
      return Trk::Intersection(pos,0.,false);
  }
    
} // end of namespace

#endif // TRKSURFACES_PLANESURFACE_H


