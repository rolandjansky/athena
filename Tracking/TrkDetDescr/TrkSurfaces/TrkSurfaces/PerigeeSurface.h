/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PerigeeSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_PERIGEESURFACE_H
#define TRKSURFACES_PERIGEESURFACE_H

//Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
// Amg
#include "EventPrimitives/EventPrimitives.h" 
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

  class LocalParameters;
  template<int DIM, class T, class S> class ParametersT;
  
  /**
   @class PerigeeSurface
   
   Class describing the Line to which the Perigee refers to.
   The Surface axis is fixed to be the z-axis of the Tracking frame.
   It inherits from Surface.
   
   @author Andreas.Salzburger@cern.ch
   */

  class PerigeeSurface : public Surface {

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
      virtual ~PerigeeSurface();
      
      /**Virtual constructor*/
      virtual PerigeeSurface* clone() const override;
      
      /**Assignment operator*/
      PerigeeSurface& operator=(const PerigeeSurface& slsf );
      
      /**Equality operator*/
      virtual bool operator==(const Surface& sf) const override;
      
      /** Use the Surface as a ParametersBase constructor, from local parameters - charged */
      virtual const ParametersT<5, Charged, PerigeeSurface>* createTrackParameters(double l1,
                                                                                   double l2,
                                                                                   double phi,
                                                                                   double theta,
                                                                                   double qop,
                                                                                   AmgSymMatrix(5)* cov = 0) const override
       { return new ParametersT<5, Charged, PerigeeSurface>(l1, l2, phi, theta, qop, *this, cov); }


      /** Use the Surface as a ParametersBase constructor, from global parameters - charged*/
      virtual const ParametersT<5, Charged, PerigeeSurface>* createTrackParameters(const Amg::Vector3D& position,
                                                                                   const Amg::Vector3D& momentum,
                                                                                   double charge,
                                                                                   AmgSymMatrix(5)* cov = 0) const override
       { return new ParametersT<5, Charged, PerigeeSurface>(position, momentum, charge, *this, cov); }   
      
      /** Use the Surface as a ParametersBase constructor, from local parameters - neutral */
      virtual const ParametersT<5, Neutral, PerigeeSurface>* createNeutralParameters(double l1,
                                                                                     double l2,
                                                                                     double phi,
                                                                                     double theta,
                                                                                     double qop,
                                                                                     AmgSymMatrix(5)* cov = 0) const override
        { return new ParametersT<5, Neutral, PerigeeSurface>(l1, l2, phi, theta, qop, *this, cov); }
      
      /** Use the Surface as a ParametersBase constructor, from global parameters - neutral */
      virtual const ParametersT<5, Neutral, PerigeeSurface>* createNeutralParameters(const Amg::Vector3D& position,
                                                                                     const Amg::Vector3D& momentum,
                                                                                     double charge,
                                                                                     AmgSymMatrix(5)* cov = 0) const override
       { return new ParametersT<5, Neutral, PerigeeSurface>(position, momentum, charge, *this, cov); }
      
      
      /** Use the Surface as a ParametersBase constructor, from local parameters */
      template <int DIM, class T> const ParametersT<DIM, T, PerigeeSurface>* createParameters(double l1,
                                                                                              double l2,
                                                                                              double phi,
                                                                                              double theta,
                                                                                              double qop,
                                                                                              AmgSymMatrix(DIM)* cov = 0) const
        { return new ParametersT<DIM, T, PerigeeSurface>(l1, l2, phi, theta, qop, *this, cov); }

      /** Use the Surface as a ParametersBase constructor, from global parameters */
      template <int DIM, class T> const ParametersT<DIM, T, PerigeeSurface>* createParameters(const Amg::Vector3D& position,
                                                                                              const Amg::Vector3D& momentum,
                                                                                              double charge,
                                                                                              AmgSymMatrix(DIM)* cov = 0) const
        { return new ParametersT<DIM, T, PerigeeSurface>(position, momentum, charge, *this, cov); }

      
      /** Return the surface type */
      virtual SurfaceType type() const override { return Surface::Perigee; }
      
      /**Return method for transfromation, overwrites the transform() form base class*/
      virtual const Amg::Transform3D& transform() const override;
      
      /**Return method for surface center infromation, overwrites the center() form base class*/
      virtual const Amg::Vector3D& center() const override;
      
      /**Return method for surface center infromation, overwrites the center() form base class*/
      virtual const Amg::Vector3D& normal() const override;
      
      /**Returns a normal vector at a specific localPosition*/
      virtual const Amg::Vector3D* normal(const Amg::Vector2D& lp) const override;

      /** Return the measurement frame - this is needed for alignment, in particular for StraightLine and Perigee Surface
          - the default implementation is the the RotationMatrix3D of the transform */
      virtual const Amg::RotationMatrix3D measurementFrame(const Amg::Vector3D& glopos, const Amg::Vector3D& glomom) const override;

      /** Local to global method:
          Take care that by just providing locR and locZ the global position cannot be calculated.
          Therefor only the locZ-coordinate is taken and transformed into the global frame.
          for calculating the global position, a momentum direction has to be provided as well, use the
          appropriate function!
           */          
      virtual const Amg::Vector3D* localToGlobal(const LocalParameters& locpos) const;

      /** This method is the true local->global transformation.<br>
          by providing a locR and locZ coordinate such as a GlobalMomentum
          the global position can be calculated.
          The choice between the two possible canditates is done by the sign of the radius
          */
      virtual const Amg::Vector3D* localToGlobal(const LocalParameters& locpos, const Amg::Vector3D& glomom) const;                                          

      /** LocalToGlobal method without dynamic memory allocation */
      virtual void localToGlobal(const Amg::Vector2D& locp, const Amg::Vector3D& mom, Amg::Vector3D& glob) const override;
      
      /** GlobalToLocal method without dynamic memory allocation - boolean checks if on surface 
         \image html SignOfDriftCircleD0.gif
      */
      virtual bool globalToLocal(const Amg::Vector3D& glob, const Amg::Vector3D& mom, Amg::Vector2D& loc) const override;
            
      /** fast straight line intersection schema - standard: provides closest intersection and (signed) path length
          forceDir is to provide the closest forward solution
          
          b>mathematical motivation:</b>
          Given two lines in parameteric form:<br>
          - @f$ \vec l_{a}(\lambda) = \vec m_a + \lambda \cdot \vec e_{a} @f$ <br>
          - @f$ \vec l_{b}(\mu) = \vec m_b + \mu \cdot \vec e_{b} @f$ <br>
          the vector between any two points on the two lines is given by:
          - @f$ \vec s(\lambda, \mu) = \vec l_{b} - l_{a} = \vec m_{ab} + \mu \cdot \vec e_{b} - \lambda \cdot \vec e_{a} @f$, <br>
          when @f$ \vec m_{ab} = \vec m_{b} - \vec m_{a} @f$.<br>
          @f$ \vec s(\lambda_0, \mu_0) @f$  denotes the vector between the two closest points <br>
          @f$ \vec l_{a,0} = l_{a}(\lambda_0) @f$ and @f$ \vec l_{b,0} = l_{b}(\mu_0) @f$ <br>
          and is perpenticular to both, @f$ \vec e_{a} @f$ and @f$ \vec e_{b} @f$.
          
          This results in a system of two linear equations:<br>
          - (i) @f$ 0 = \vec s(\lambda_0, \mu_0) \cdot \vec e_a = \vec m_ab \cdot \vec e_a + \mu_0 \vec e_a \cdot \vec e_b - \lambda_0 @f$ <br>     
          - (ii) @f$ 0 = \vec s(\lambda_0, \mu_0) \cdot \vec e_b = \vec m_ab \cdot \vec e_b + \mu_0  - \lambda_0 \vec e_b \cdot \vec e_a @f$ <br>
          
          Solving (i), (ii) for @f$ \lambda_0 @f$ and @f$ \mu_0 @f$ yields:
          - @f$ \lambda_0 = \frac{(\vec m_ab \cdot \vec e_a)-(\vec m_ab \cdot \vec e_b)(\vec e_a \cdot \vec e_b)}{1-(\vec e_a \cdot \vec e_b)^2} @f$ <br>
          - @f$ \mu_0 = - \frac{(\vec m_ab \cdot \vec e_b)-(\vec m_ab \cdot \vec e_a)(\vec e_a \cdot \vec e_b)}{1-(\vec e_a \cdot \vec e_b)^2} @f$ <br>          
       */
      virtual Intersection straightLineIntersection(const Amg::Vector3D& pos, 
                                                   const Amg::Vector3D& dir, 
                                                   bool forceDir = false,
                                                   Trk::BoundaryCheck bchk = false) const override;
      
      /** fast straight line distance evaluation to Surface */
      virtual DistanceSolution straightLineDistanceEstimate(const Amg::Vector3D& pos,const Amg::Vector3D& dir) const override;

      /** fast straight line distance evaluation to Surface - with bound option*/
      virtual DistanceSolution straightLineDistanceEstimate(const Amg::Vector3D& pos,const Amg::Vector3D& dir, bool Bound) const override;


      /** the pathCorrection for derived classes with thickness */
      virtual double pathCorrection(const Amg::Vector3D&, const Amg::Vector3D&) const override { return 1.; }

        /**This method checks if a globalPosition in on the Surface or not*/
      virtual bool isOnSurface(const Amg::Vector3D& glopo, BoundaryCheck bchk=true, double tol1=0., double tol2=0.) const override;

      /**This surface calls the iside method of the bounds*/
      virtual bool insideBounds(const Amg::Vector2D& locpos, double tol1=0., double tol2=0.) const override;
      virtual bool insideBoundsCheck(const Amg::Vector2D& locpos, const BoundaryCheck& bchk) const override;   

      /** Special method for StraightLineSurface - provides the Line direction from cache: speedup */
      const Amg::Vector3D& lineDirection() const;
      
      /** Return bounds() method */
      virtual const NoBounds& bounds() const override;
        
      /** Return properly formatted class name for screen output */
      virtual std::string name() const override { return "Trk::PerigeeSurface"; }
              
      /** Output Method for MsgStream*/
      virtual MsgStream& dump(MsgStream& sl) const override;
      /** Output Method for std::ostream*/
      virtual std::ostream& dump(std::ostream& sl) const override;

    protected: //!< data members
    
      mutable Amg::Vector3D*                      m_lineDirection;  //!< cache of the line direction (speeds up)
      static NoBounds                             s_perigeeBounds;
      
  };


  inline PerigeeSurface* PerigeeSurface::clone() const {return new PerigeeSurface(*this);}
         
  inline const Amg::Transform3D& PerigeeSurface::transform() const
  {
    if (!Surface::m_transform) return(s_idTransform);
    return(*Trk::Surface::m_transform);
  }

  inline const Amg::Vector3D& PerigeeSurface::center() const
  {
    if (!Surface::m_center && !Surface::m_transform) return(s_origin);
    else if (!Surface::m_center) m_center = new Amg::Vector3D(m_transform->translation());
    return(*Surface::m_center);
  }
  
  inline const Amg::Vector3D& PerigeeSurface::normal() const { return(s_xAxis); }

  inline const Amg::Vector3D* PerigeeSurface::normal(const Amg::Vector2D&) const { return new Amg::Vector3D(this->normal());  }
  
  inline bool PerigeeSurface::insideBounds(const Amg::Vector2D&, double, double) const { return true;}
  
  inline bool PerigeeSurface::insideBoundsCheck(const Amg::Vector2D&, const BoundaryCheck& ) const { return true;}
  
  inline bool PerigeeSurface::isOnSurface(const Amg::Vector3D&, BoundaryCheck, double, double) const {return true;}
  
  inline const NoBounds& PerigeeSurface::bounds() const { return s_perigeeBounds; } 
  
  inline Intersection PerigeeSurface::straightLineIntersection(const Amg::Vector3D& pos, 
                                                               const Amg::Vector3D& dir, 
                                                               bool forceDir,
                                                               Trk::BoundaryCheck) const
  {
       // following nominclature found in header file and doxygen documentation
       // line one is the straight track
       const Amg::Vector3D   ma  = pos;
       const Amg::Vector3D&  ea  = dir;
       // line two is the line surface
       const Amg::Vector3D& mb = center();
       const Amg::Vector3D& eb = lineDirection();
       // now go ahead
       Amg::Vector3D  mab(mb - ma);
       double eaTeb = ea.dot(eb);
       double denom = 1 - eaTeb*eaTeb;
       if (fabs(denom)>10e-7){
          double lambda0 = (mab.dot(ea) - mab.dot(eb)*eaTeb)/denom;
          // evaluate the direction, bounds are always true for Perigee
          bool isValid = forceDir ? (lambda0 > 0.) : true;
          return Trk::Intersection( (ma+lambda0*ea),lambda0, isValid );
       }
      return Trk::Intersection(pos,0.,false);
  }
  
  inline const Amg::Vector3D& PerigeeSurface::lineDirection() const {
       if (m_lineDirection)
           return (*m_lineDirection);
       if (!m_lineDirection && Surface::m_transform) {
           m_lineDirection = new Amg::Vector3D(transform().rotation().col(2));
           return (*m_lineDirection);
       }
       return Trk::s_zAxis;

   }
  
} // end of namespace

#endif // TRKSURFACES_PERIGEESURFACE_H

