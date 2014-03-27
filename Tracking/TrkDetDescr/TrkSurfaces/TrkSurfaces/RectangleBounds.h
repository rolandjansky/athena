/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RectangleBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_RECTANGLEBOUNDS_H
#define TRKSURFACES_RECTANGLEBOUNDS_H

#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON 
typedef float TDD_real_t; 
#else 
typedef double TDD_real_t; 
#endif

namespace Trk {

   /**
    @class RectangleBounds 

    Bounds for a rectangular, planar surface.
    The two local coordinates locX, locY are for legacy reasons
    also called @f$ phi @f$ respectively @f$ eta @f$. The orientation 
    with respect to the local surface framce can be seen in the attached
    illustration.

    @image html RectangularBounds.gif
    
    @author Andreas.Salzburger@cern.ch */

  class RectangleBounds : public SurfaceBounds {

    public:
        
      /** @enum BoundValues for readability */
      enum BoundValues {
          bv_halfX  = 0,
          bv_halfY  = 1,
          bv_length = 2
      };
        
      /**Default Constructor - needed for persistency*/
      RectangleBounds();  

      /**Constructor with halflength in phi and halflength in eta*/
      RectangleBounds(double halephi, double haleta);
      
      /**Copy constructor*/
      RectangleBounds(const RectangleBounds& recbo);
      
      /**Destructor*/
      virtual ~RectangleBounds();
      
      /**Assignment Operator*/
      RectangleBounds& operator=(const RectangleBounds& recbo);
      
      /**Equality operator*/
      bool operator==(const SurfaceBounds& sbo) const;
      
      /**Virtual constructor*/
      virtual RectangleBounds* clone() const;
    
      /** Return the type of the bounds for persistency */
      virtual BoundsType type() const { return SurfaceBounds::Rectangle; }
    
      /**This method checks if the provided local coordinates are inside the surface bounds*/
      virtual bool inside(const Amg::Vector2D &locpo, double tol1=0., double tol2=0.) const;

      /** This method checks inside bounds in loc1
        - loc1/loc2 correspond to the natural coordinates of the surface */
      virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1=0.) const;

      /** This method checks inside bounds in loc2 
        - loc1/loc2 correspond to the natural coordinates of the surface */
      virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2=0.) const;

      /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
      virtual double minDistance(const Amg::Vector2D& pos) const;

      /**This method returns the halflength in phi (first coordinate of local surface frame)*/
      double halflengthPhi() const;
      
      /**This method returns the halflength in Eta (second coordinate of local surface frame)*/
      double halflengthEta() const;
      
      /**for consistant naming*/
      double halflengthX() const;
      
      /**for consitant naming*/
      double halflengthY() const;    
      
      /**This method returns the maximal extension on the local plane, i.e. @f$s\sqrt{h_{\phi}^2 + h_{\eta}^2}\f$*/
      virtual double r() const;
    
      /** Output Method for MsgStream*/
      virtual MsgStream& dump(MsgStream& sl) const;
      
      /** Output Method for std::ostream */
      virtual std::ostream& dump(std::ostream& sl) const;
  
    private:
      /** The internal version of the bounds can be float/double*/
      std::vector<TDD_real_t>   m_boundValues;

  };

  inline RectangleBounds* RectangleBounds::clone() const
    { return new RectangleBounds(*this); }

  inline bool RectangleBounds::inside(const Amg::Vector2D &locpo, double tol1, double tol2) const
    { return ((fabs(locpo[locX]) < m_boundValues[RectangleBounds::bv_halfX] + tol1) && (fabs(locpo[locY]) < m_boundValues[RectangleBounds::bv_halfY] + tol2)  ); }

  inline bool RectangleBounds::insideLoc1(const Amg::Vector2D &locpo, double tol1) const
    { return (fabs(locpo[locX]) < m_boundValues[RectangleBounds::bv_halfX] + tol1); }

  inline bool RectangleBounds::insideLoc2(const Amg::Vector2D &locpo, double tol2) const
    { return (fabs(locpo[locY]) < m_boundValues[RectangleBounds::bv_halfY] + tol2); }

  inline double RectangleBounds::halflengthPhi() const { return this->halflengthX(); }
  
  inline double RectangleBounds::halflengthEta() const { return this->halflengthY(); }

  inline double RectangleBounds::halflengthX() const { return m_boundValues[RectangleBounds::bv_halfX]; }
  
  inline double RectangleBounds::halflengthY() const { return m_boundValues[RectangleBounds::bv_halfY]; }
 
  inline double RectangleBounds::r() const { 
        return sqrt(m_boundValues[RectangleBounds::bv_halfX]*m_boundValues[RectangleBounds::bv_halfX] 
                  + m_boundValues[RectangleBounds::bv_halfY]*m_boundValues[RectangleBounds::bv_halfY]); 
  }

} // end of namespace


#endif // TRKSURFACES_RECTANGLEBOUNDS_H


