/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscBounds.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DISCBOUNDS_H
#define TRKSURFACES_DISCBOUNDS_H

// Trk
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/ParamDefs.h"
// 
#include <math.h>
//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON 
typedef float TDD_real_t; 
#else 
typedef double TDD_real_t; 
#endif

namespace Trk {

  /**
   @class DiscBounds
    
   Class to describe the bounds for a planar DiscSurface.
   By providing an argument for hphisec, the bounds can 
   be restricted to a phirange around the center position.
    
   @image html DiscBounds.gif
    
   @author Andreas.Salzburger@cern.ch    
   */

 class DiscBounds : public SurfaceBounds {

   public:
     /** enumeration for readability */
     enum BoundValues {
         bv_rMin          = 0,
         bv_rMax          = 1,
         bv_averagePhi    = 2,
         bv_halfPhiSector = 3,
         bv_length        = 4 
     };
     /**Default Constructor*/
     DiscBounds(); 
      
     /**Constructor for full disc of symmetric disc around phi=0*/
     DiscBounds(double minrad, double maxrad, double hphisec=M_PI);
     
     /**Constructor for a symmetric disc around phi != 0*/
     DiscBounds(double minrad, double maxrad, double avephi, double hphisec);
     
     /**Copy constructor*/
     DiscBounds(const DiscBounds& discbo);
     
     /**Destructor*/
     virtual ~DiscBounds();
     
     /**Assignment operator*/
     DiscBounds& operator=(const DiscBounds& discbo);
     
     /**Equality operator*/
     bool operator==(const SurfaceBounds& sbo) const;
     
     /**Virtual constructor*/
     virtual DiscBounds* clone() const;
     
     /** Return the type - mainly for persistency */
     virtual SurfaceBounds::BoundsType type() const { return SurfaceBounds::Disc; }
     
     /**This method cheks if the radius given in the LocalPosition is inside [rMin,rMax]
        if only tol1 is given and additional in the phi sector is tol2 is given */
     virtual bool inside(const Amg::Vector2D &locpo, double tol1=0., double tol2=0.) const;
                               
     /** This method checks inside bounds in loc1
       - loc1/loc2 correspond to the natural coordinates of the surface */
     virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1=0.) const;
                           
     /** This method checks inside bounds in loc2 
       - loc1/loc2 correspond to the natural coordinates of the surface */
     virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2=0.) const;
     
     /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
     virtual double minDistance(const Amg::Vector2D& pos) const;
     
     /**This method returns inner radius*/
     double rMin() const;
     
     /**This method returns outer radius*/
     double rMax() const;
     
     /**This method returns the maximum expansion on the plane (=rMax)*/
     virtual double r() const;
     
     /**This method returns the average phi*/
     double averagePhi() const;
     
     /**This method returns the halfPhiSector which is covered by the disc*/
     double halfPhiSector() const;
     
     /** Output Method for MsgStream*/
     virtual MsgStream& dump(MsgStream& sl) const;
     
     /** Output Method for std::ostream */
     virtual std::ostream& dump(std::ostream& sl) const;

   private:  
     /** Internal members of the bounds (float/double)*/
     std::vector<TDD_real_t>                    m_boundValues;
 
  };
  
  inline DiscBounds* DiscBounds::clone() const
  { return new DiscBounds(*this); }
  
  inline bool DiscBounds::inside(const Amg::Vector2D &locpo, double tol1, double tol2) const
  { 
    double alpha = fabs(locpo[locPhi]-m_boundValues[DiscBounds::bv_averagePhi]);
    if ( alpha>M_PI) alpha = 2*M_PI - alpha; 
    bool insidePhi = (alpha <= (m_boundValues[DiscBounds::bv_halfPhiSector]+tol2));
    return ( locpo[locR] > (m_boundValues[DiscBounds::bv_rMin] - tol1) && locpo[locR] < (m_boundValues[DiscBounds::bv_rMax] + tol1)  && insidePhi ); 
  }
  
  inline bool DiscBounds::insideLoc1(const Amg::Vector2D &locpo, double tol1) const
  { return (locpo[locR] > (m_boundValues[DiscBounds::bv_rMin] -tol1) && locpo[locR] < (m_boundValues[DiscBounds::bv_rMax] + tol1)); }
  
  inline bool DiscBounds::insideLoc2(const Amg::Vector2D &locpo, double tol2) const
  { 
    double alpha = fabs(locpo[locPhi]-m_boundValues[DiscBounds::bv_averagePhi]);
    if ( alpha > M_PI ) alpha  = 2.*M_PI-alpha;
    //alpha -= alpha > M_PI ? 2.*M_PI : 0.;
    //alpha += alpha < -M_PI ? 2.*M_PI : 0.;
    bool insidePhi = (alpha <= (m_boundValues[DiscBounds::bv_halfPhiSector]+tol2));
    return insidePhi; 
  }
  
  inline double DiscBounds::rMin() const { return m_boundValues[DiscBounds::bv_rMin]; }
  
  inline double DiscBounds::rMax() const { return m_boundValues[DiscBounds::bv_rMax]; }
  
  inline double DiscBounds::r() const { return m_boundValues[DiscBounds::bv_rMax]; }
  
  inline double DiscBounds::averagePhi() const { return m_boundValues[DiscBounds::bv_averagePhi]; }
  
  inline double DiscBounds::halfPhiSector() const { return m_boundValues[DiscBounds::bv_halfPhiSector]; }

} // end of namespace


#endif // TRKSURFACES_DISCBOUNDS_H


