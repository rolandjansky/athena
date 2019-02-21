/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SurfaceBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_SURFACEBOUNDS_H
#define TRKSURFACES_SURFACEBOUNDS_H

//STD
#include <iostream>
#include <iomanip>

//GaudiKernel
#include "GaudiKernel/MsgStream.h"

// Geo & Math library
#include "GeoPrimitives/GeoPrimitives.h"

// Trk included
#include "TrkSurfaces/BoundaryCheck.h"

#ifdef TRKDETDESCR_USEFLOATPRECISON 
typedef float TDD_real_t; 
#else 
typedef double TDD_real_t;
#endif


namespace Trk {

  /**
   @class SurfaceBounds
   
   Abstract base class for surface bounds to be specified.

   Surface bounds provide:
   - inside() checks 
   - the BoundsType return type
   - an initCache() method
    
   @author Andreas.Salzburger@cern.ch
   */
        
      
  class SurfaceBounds {

    public:

    /** @enum BoundsType 
      
        This enumerator simplifies the persistency,
        by saving a dynamic_cast to happen.
        
        Other is reserved for the GeometrySurfaces implementation.
        
      */
      enum BoundsType {
        Cone             = 0,
        Cylinder         = 1,  
        Diamond          = 2,
        Disc             = 3,
        Ellipse          = 5,
        Rectangle        = 6,
        RotatedTrapezoid = 7,
        Trapezoid        = 8,
        Triangle         = 9,
	DiscTrapezoidal  = 10,
	Annulus		 = 11,
        Other            = 12 
        
      };
        
      /**Default Constructor*/
      SurfaceBounds() = default;

      /**Destructor*/
      virtual ~SurfaceBounds() = default;
      
      /** clone() method to make deep copy in Surface copy constructor and for assigment operator
        of the Surface class.*/
      virtual SurfaceBounds* clone() const = 0;
      
      /**Equality operator*/
      virtual bool operator==(const SurfaceBounds& sb) const = 0;
      
      /**Non-Equality operator*/
      virtual bool operator!=(const SurfaceBounds& sb) const;
      
      /** Return the bounds type - for persistency optimization */
      virtual BoundsType type() const = 0;
              
      /** Each Bounds has a method inside, which checks if a LocalPosition is inside the bounds.
          Inside can be called without/with tolerances. */                 
      virtual bool inside(const Amg::Vector2D& locpo, double tol1=0., double tol2=0.) const = 0;
      virtual bool inside(const Amg::Vector2D& locpo,
                                 const BoundaryCheck& bchk) const = 0;
      /** Extend the interface to for single inside Loc 1 / Loc2 tests 
         - loc1/loc2 correspond to the natural coordinates of the surface
        */
      virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1=0.) const = 0;
      
      /** Extend the interface to for single inside Loc 1 / Loc2 tests 
         - loc1/loc2 correspond to the natural coordinates of the surface
        */
      virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2=0.) const = 0;

      /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
      virtual double minDistance(const Amg::Vector2D& pos) const = 0;

      /** Interface method for the maximal extension or the radius*/
      virtual double r() const = 0;
      
      /** Output Method for MsgStream, to be overloaded by child classes */
      virtual MsgStream& dump(MsgStream& sl) const = 0;
      
      /** Output Method for std::ostream, to be overloaded by child classes */
      virtual std::ostream& dump(std::ostream& sl) const = 0;
                  
    protected:
      /** Swap method to be called from DiscBounds or TrapezoidalBounds */
      void swap(double& b1, double& b2);
      
      /** virtual initCache method for object persistency */
      virtual void initCache() {}
  };

  inline void SurfaceBounds::swap(double& b1, double& b2) { 
    double tmp=b1;
    b1 = b2;
    b2 = tmp;
  }

  inline bool SurfaceBounds::operator!=(const SurfaceBounds& sb) const { return !((*this)==sb);} 
 
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const SurfaceBounds& sb);
std::ostream& operator << ( std::ostream& sl, const SurfaceBounds& sb);      
                              
} // end of namespace

#endif // TRKSURFACES_SURFACEBOUNDS_H
