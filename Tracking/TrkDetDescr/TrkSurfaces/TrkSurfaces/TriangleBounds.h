/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TriangleBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_TRIANGLEBOUNDS_H
#define TRKSURFACES_TRIANGLEBOUNDS_H

#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <utility>

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON 
typedef float TDD_real_t; 
#else 
typedef double TDD_real_t; 
#endif

namespace Trk {

   /**
    @class TriangleBounds 

    Bounds for a triangular, planar surface.

    @image html TriangularBounds.gif
    
    @author sarka.todorova@cern.ch */

  class TriangleBounds : public SurfaceBounds {

      
    public:
      /** @enum BoundValues for readability */  
      enum BoundValues {
          bv_x1     = 0,
          bv_y1     = 1,
          bv_x2     = 2,
          bv_y2     = 3,
          bv_x3     = 4,
          bv_y3     = 5,
          bv_length = 6
      };
      
      /**Default Constructor - needed for persistency*/
      TriangleBounds();
        
      /**Constructor with coordinates of vertices - floats*/
      TriangleBounds( std::vector< std::pair<float,float> >  );
      
      /**Constructor with coordinates of vertices - double*/
      TriangleBounds( std::vector< std::pair<double,double> >  );
      
      /**Constructor from three 2 Vectors */
      TriangleBounds( const Amg::Vector2D& p1, const Amg::Vector2D& p2, const Amg::Vector2D& p3);
      
      /**Copy constructor*/
      TriangleBounds(const TriangleBounds& tribo);
      
      /**Destructor*/
      virtual ~TriangleBounds();
      
      /**Assignment Operator*/
      TriangleBounds& operator=(const TriangleBounds& recbo);
      
      /**Equality operator*/
      bool operator==(const SurfaceBounds& sbo) const;
      
      /**Virtual constructor*/
      virtual TriangleBounds* clone() const;
      
      /** Return the type of the bounds for persistency */
      virtual BoundsType type() const { return SurfaceBounds::Triangle; }
    
      /**This method checks if the provided local coordinates are inside the surface bounds*/
      virtual bool inside(const Amg::Vector2D &locpo, double tol1 = 0., double tol2 = 0.) const;
      
      /** This method checks inside bounds in loc1
        - loc1/loc2 correspond to the natural coordinates of the surface */
      virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1=0.) const;
      
      /** This method checks inside bounds in loc2 
        - loc1/loc2 correspond to the natural coordinates of the surface */
      virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2=0.) const;
      
      /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
      virtual double minDistance(const Amg::Vector2D& pos) const;

      /**This method returns the coordinates of vertices*/
      const std::vector< std::pair< TDD_real_t, TDD_real_t> > vertices() const;

      /**This method returns the maximal extension on the local plane, i.e. @f$s\sqrt{h_{\phi}^2 + h_{\eta}^2}\f$*/
      virtual double r() const;
    
      /** Output Method for MsgStream*/
      virtual MsgStream& dump(MsgStream& sl) const;
      
      /** Output Method for std::ostream */
      virtual std::ostream& dump(std::ostream& sl) const;
  
    private:
      std::vector<TDD_real_t> m_boundValues;
            
  };

  inline TriangleBounds* TriangleBounds::clone() const
    { return new TriangleBounds(*this); }
  
  inline bool TriangleBounds::inside(const Amg::Vector2D &locpo, double tol1, double tol2) const {
    std::pair<double,double> locB(m_boundValues[TriangleBounds::bv_x2]-m_boundValues[TriangleBounds::bv_x1],
				  m_boundValues[TriangleBounds::bv_y2]-m_boundValues[TriangleBounds::bv_y1]);
    std::pair<double,double> locT(m_boundValues[TriangleBounds::bv_x3] -locpo[0],
				  m_boundValues[TriangleBounds::bv_y3]-locpo[1]);
    std::pair<double,double> locV(m_boundValues[TriangleBounds::bv_x1] -locpo[0],
				  m_boundValues[TriangleBounds::bv_y1]-locpo[1]);

    // special case :: third vertex ?
    if (locT.first*locT.first+locT.second*locT.second<tol1*tol1) return true; 

    // special case : lies on base ?
    double db = locB.first*locV.second-locB.second*locV.first;
    if ( fabs(db)<tol1 ) {
      double a = (locB.first!=0) ? -locV.first/locB.first : -locV.second/locB.second;
      if ( a>-tol2 && a-1.<tol2 ) return true;
      return false;
    }
    
    double dn = locB.first*locT.second-locB.second*locT.first;

    if ( fabs(dn) > tol1 ) {
      double t = (locB.first*locV.second-locB.second*locV.first)/dn;
      if ( t > 0.) return false;

      double a = (locB.first!=0.) ? (t*locT.first - locV.first)/locB.first :
                                    (t*locT.second - locV.second)/locB.second ;
      if ( a < -tol2 || a-1.>tol2 ) return false;      
    } else {
      return false;
    }
    return true;
  }

  inline bool TriangleBounds::insideLoc1(const Amg::Vector2D &locpo, double tol1) const
    { return inside(locpo,tol1,tol1); }

  inline bool TriangleBounds::insideLoc2(const Amg::Vector2D &locpo, double tol2) const
    { return inside(locpo,tol2,tol2); }
  
  inline const std::vector<std::pair<TDD_real_t,TDD_real_t> > TriangleBounds::vertices() const 
  { std::vector< std::pair<TDD_real_t,TDD_real_t> > vertices;
    vertices.resize(3);
    for (size_t iv = 0; iv < 3 ; iv++) 
       vertices.push_back(std::pair<TDD_real_t,TDD_real_t>(m_boundValues[2*iv],m_boundValues[2*iv+1]));
   return vertices;  
   }

  inline double TriangleBounds::r() const {
    double rmax = 0.;
    for (size_t iv = 0; iv < 3 ; iv++) 
      rmax = fmax(rmax, m_boundValues[2*iv]*m_boundValues[2*iv] + m_boundValues[2*iv+1]*m_boundValues[2*iv+1]);
    return sqrt(rmax);
  }

} // end of namespace


#endif // TRKSURFACES_RECTANGLEBOUNDS_H


