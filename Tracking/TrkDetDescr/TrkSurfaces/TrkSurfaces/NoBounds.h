/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_NOBOUNDS_H
#define TRKSURFACES_NOBOUNDS_H
    
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/ParamDefs.h"

//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {
   
  /**
   @class NoBounds
   Bounds object for a boundless surface (...)
           
   @author Andreas.Salzburger@cern.ch
   */
   
   class NoBounds : public SurfaceBounds {
     public:
       /**Default Constructor*/
       NoBounds(){}
       
       /** Destructor */
       ~NoBounds(){}
       
       /**Equality operator */
       virtual bool operator==(const SurfaceBounds& sbo) const override;
       
       /** Return SurfaceBounds for persistency */
       virtual SurfaceBounds::BoundsType type() const override { return SurfaceBounds::Other; }
       
       /** Method inside() returns true for any case */
       virtual bool inside(const Amg::Vector2D& locpo, double tol1=0., double tol2=0.) const override;
       virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;
       /** This method checks inside bounds in loc1
         - loc1/loc2 correspond to the natural coordinates of the surface */
       virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1=0.) const override;
                        
       /** This method checks inside bounds in loc2 
         - loc1/loc2 correspond to the natural coordinates of the surface */
       virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2=0.) const override;

       /** Minimal distance to boundary (=0 if inside) */
       virtual double minDistance(const Amg::Vector2D& pos) const override;
      
       /** Clone method to complete inherited interface */
       virtual NoBounds* clone() const override;

       /** r() method to complete inherited interface */
       virtual double r() const override;
     
       /** Output Method for MsgStream*/
       virtual MsgStream& dump(MsgStream& sl) const override;

       /** Output Method for std::ostream */
       virtual std::ostream& dump(std::ostream& sl) const override;
      
     private:
  
  };

  inline bool NoBounds::operator==(const SurfaceBounds&) const 
    { return true; } 
   
  inline bool NoBounds::inside(const Amg::Vector2D&, double, double) const
    { return true; }
	
  inline bool NoBounds::inside(const Amg::Vector2D&, const BoundaryCheck&) const
  {  return true;  }

  inline bool NoBounds::insideLoc1(const Amg::Vector2D&, double) const
    { return true; }

  inline bool NoBounds::insideLoc2(const Amg::Vector2D&, double) const
    { return true; }

  inline double NoBounds::minDistance(const Amg::Vector2D&) const
    { return 0.; }

  inline NoBounds* NoBounds::clone() const 
    { return new NoBounds(); }

  inline double NoBounds::r() const 
    { return 0.; }
   
  inline MsgStream& NoBounds::dump(MsgStream& sl) const
  {
       sl << "Trk::NoBounds ... boundless surface" << endmsg;
       return sl;
  }
   
  inline std::ostream& NoBounds::dump(std::ostream& sl) const
  {
      sl << "Trk::NoBounds ... boundless surface" << std::endl;
      return sl;   
  }
  
} // end of namespace

#endif // TRKSURFACES_NOBOUNDS_H
