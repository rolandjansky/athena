/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_INVALIDBOUNDS_H
#define TRKSURFACES_INVALIDBOUNDS_H
    
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/ParamDefs.h"

//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {
   
  /**
   @class InvalidBounds
   Invalid Bounds object
           
   @author Shaun Roe
   */
   
   class InvalidBounds : public SurfaceBounds {
     public:
       /**Default Constructor*/
       InvalidBounds(){}
       
       /** Destructor */
       ~InvalidBounds(){}
       
       /**Equality operator */
       virtual bool operator==(const SurfaceBounds& ) const override {return false;}
       
      /**Non-Equality operator*/
      virtual bool operator!=(const SurfaceBounds& ) const override {return true;}
      
      /**'address of' will return nullptr **/
      InvalidBounds * operator &() {return nullptr;}
      
       
       /** Return SurfaceBounds for persistency */
       virtual SurfaceBounds::BoundsType type() const override { return SurfaceBounds::Other; }
       
       /** Method inside() returns false for any case */
       virtual bool inside(const Amg::Vector2D& , double =0., double =0.) const override {return false; }
       virtual bool inside(const Amg::Vector2D& , const BoundaryCheck& ) const override {return false; } 
       /** This method checks inside bounds in loc1
         - loc1/loc2 correspond to the natural coordinates of the surface */
       virtual bool insideLoc1(const Amg::Vector2D& , double =0.) const override {return false; }
                        
       /** This method checks inside bounds in loc2 
         - loc1/loc2 correspond to the natural coordinates of the surface */
       virtual bool insideLoc2(const Amg::Vector2D& , double =0.) const override {return false; }

       /** Minimal distance to boundary (=0 if inside) */
       virtual double minDistance(const Amg::Vector2D& ) const override {return std::nan("");}
      
       /** Invalidate cloning of an invalid object */
       virtual InvalidBounds* clone() const override {return nullptr;} 

       /** r() method to complete inherited interface */
       virtual double r() const override {return std::nan(""); }
     
       /** Output Method for MsgStream*/
       virtual MsgStream& dump(MsgStream& sl) const override;

       /** Output Method for std::ostream */
       virtual std::ostream& dump(std::ostream& sl) const override;
      
     private:
  
  };

 
  inline MsgStream& InvalidBounds::dump(MsgStream& sl) const
  {
       sl << "Trk::InvalidBounds ... invalid surface" << endmsg;
       return sl;
  }
   
  inline std::ostream& InvalidBounds::dump(std::ostream& sl) const
  {
      sl << "Trk::InvalidBounds ... invalid surface" << std::endl;
      return sl;   
  }
  
} // end of namespace

#endif // TRKSURFACES_INVALIDBOUNDS_H
