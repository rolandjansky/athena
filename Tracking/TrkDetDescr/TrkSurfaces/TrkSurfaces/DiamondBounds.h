/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiamondBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DIAMONDDBOUNDS_H
#define TRKSURFACES_DIAMONDDBOUNDS_H

#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <math.h>

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON 
typedef float TDD_real_t; 
#else 
typedef double TDD_real_t; 
#endif

namespace Trk {

  /** 
   @class DiamondBounds
   Bounds for a double trapezoidal ("diamond"), planar Surface.
           
   @author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
   */

  class DiamondBounds : public SurfaceBounds {

    public:
      /** BoundValues for better readability */
      enum BoundValues {
            bv_minHalfX  = 0,
            bv_medHalfX  = 1,
            bv_maxHalfX  = 2,
            bv_halfY1    = 3,
            bv_halfY2    = 4,
            bv_length    = 5
      };
      
      /**Default Constructor, needed for persistency*/
      DiamondBounds();
        
      /**Constructor for symmetric Diamond*/
      DiamondBounds(double minhalex, double medhalex, double maxhalex, double haley1, double haley2);
      
      /**Copy constructor*/
      DiamondBounds(const DiamondBounds& diabo);
      
      /**Destructor*/
      virtual ~DiamondBounds();
      
      /**Virtual constructor*/
      DiamondBounds* clone() const;
      
      /**Assignment operator*/
      DiamondBounds& operator=(const DiamondBounds& sbo); 
      
      /**Equality operator*/
      bool operator==(const SurfaceBounds& diabo) const;
    
      /** Return the bounds type */
      virtual BoundsType type() const { return SurfaceBounds::Diamond; }
    
      /**This method returns the halflength in X at minimal Y (first coordinate of local surface frame)*/ 
      double minHalflengthX() const;
      
      /**This method returns the (maximal) halflength in X (first coordinate of local surface frame)*/
      double medHalflengthX() const;
      
      /**This method returns the halflength in X at maximal Y (first coordinate of local surface frame)*/
      double maxHalflengthX() const;
      
      /**This method returns the halflength in Y of trapezoid at negative/positive Y (second coordinate)*/
      double halflengthY1() const;
      double halflengthY2() const;
      
      /**This method returns the maximal extension on the local plane*/
      double r() const;
      
      /**This method returns the opening angle alpha in point A   */
      double alpha1() const;
      
      /**This method returns the opening angle alpha in point A'  */
      double alpha2() const;
            
      /** The orientation of the Diamond is according to the figure */
      virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const;

      /** This method checks inside bounds in loc1
      - loc1/loc2 correspond to the natural coordinates of the surface 
      - As loc1/loc2 are correlated the single check doesn't make sense : 
         -> check is done on enclosing Rectangle !
      */
      virtual bool insideLoc1(const Amg::Vector2D& locpo,   double tol1=0.) const;
      
      /** This method checks inside bounds in loc2 
      - loc1/loc2 correspond to the natural coordinates of the surface
      - As loc1/loc2 are correlated the single check doesn't make sense : 
         -> check is done on enclosing Rectangle !
      */
      virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2=0.) const;

      /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
      double minDistance(const Amg::Vector2D& pos) const;

      /** Output Method for MsgStream*/
      virtual MsgStream& dump(MsgStream& sl) const;
      
      /** Output Method for std::ostream */
      virtual std::ostream& dump(std::ostream& sl) const;
      
   private:
      /** inside() method for a full symmetric diamond */
      bool insideFull(const Amg::Vector2D& locpo, double tol1=0., double tol2=0.) const;

      /** initialize the alpha1/2 cache - needed also for object persistency */
      void initCache();

      /** Internal parameters stored in the geometry */
      std::vector<TDD_real_t>                   m_boundValues;
      TDD_real_t                                m_alpha1;
      TDD_real_t                                m_alpha2;
      
  };

  inline DiamondBounds* DiamondBounds::clone() const { return new DiamondBounds(*this); }
    
  inline double DiamondBounds::minHalflengthX() const { return m_boundValues[DiamondBounds::bv_minHalfX]; }
  
  inline double DiamondBounds::medHalflengthX() const { return m_boundValues[DiamondBounds::bv_medHalfX]; }
  
  inline double DiamondBounds::maxHalflengthX() const { return m_boundValues[DiamondBounds::bv_maxHalfX]; }
  
  inline double DiamondBounds::halflengthY1() const    { return m_boundValues[DiamondBounds::bv_halfY1]; }
  
  inline double DiamondBounds::halflengthY2() const    { return m_boundValues[DiamondBounds::bv_halfY2]; }
  
  inline double DiamondBounds::r() const 
    { return sqrt(m_boundValues[DiamondBounds::bv_medHalfX]*m_boundValues[DiamondBounds::bv_medHalfX] 
                + m_boundValues[DiamondBounds::bv_halfY1]*m_boundValues[DiamondBounds::bv_halfY1]); }
  
  inline bool DiamondBounds::insideLoc1(const Amg::Vector2D &locpo, double tol1) const
    { return (fabs(locpo[locX]) < m_boundValues[DiamondBounds::bv_medHalfX] + tol1); }

  inline bool DiamondBounds::insideLoc2(const Amg::Vector2D &locpo, double tol2) const
    { return ((locpo[locY] > -2.*m_boundValues[DiamondBounds::bv_halfY1] - tol2) && (locpo[locY] < 2.*m_boundValues[DiamondBounds::bv_halfY2] + tol2) ); }
    
  inline void DiamondBounds::initCache() {
      m_alpha1 = atan2(m_boundValues[DiamondBounds::bv_medHalfX]-m_boundValues[DiamondBounds::bv_minHalfX], 2.*m_boundValues[DiamondBounds::bv_halfY1]);
      m_alpha2 = atan2(m_boundValues[DiamondBounds::bv_medHalfX]-m_boundValues[DiamondBounds::bv_maxHalfX], 2.*m_boundValues[DiamondBounds::bv_halfY2]);
  }    
    
} // end of namespace

#endif // TRKSURFACES_DIAMONDBOUNDS_H

