/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedPlaneSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRYSURFACES_SUBTRACTEDPLANESURFACE_H
#define TRKGEOMETRYSURFACES_SUBTRACTEDPLANESURFACE_H

//Trk
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkDetDescrUtils/AreaExcluder.h"
// Geometry & Maths
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
class Identifier;

namespace Trk {
  
  /**
   @class SubtractedPlaneSurface
   Class for a planar subtracted/shared surface in the ATLAS detector.
   It owns its surface bounds and subtracted volume. 
    
   @author Sarka.Todorova@cern.ch
   */

  class SubtractedPlaneSurface : public PlaneSurface {
    public:
      /** Default Constructor - needed for persistency*/
      SubtractedPlaneSurface();
        
      /** Copy Constructor*/
      SubtractedPlaneSurface(const SubtractedPlaneSurface& psf);
      
      /** Copy Constructor with shift*/
      SubtractedPlaneSurface(const SubtractedPlaneSurface& psf, const Amg::Transform3D& transf);
      
      /** Constructor */      
      SubtractedPlaneSurface(const PlaneSurface& ps , AreaExcluder* vol, bool shared);
            
      /**Destructor*/
      virtual ~SubtractedPlaneSurface();  
      
      /**Assignment operator*/
      SubtractedPlaneSurface& operator=(const SubtractedPlaneSurface& psf);
      
      /**Equality operator*/
      bool operator==(const Surface& sf) const;

      /** This method indicates the subtraction mode */
      bool shared() const;

      /**This method calls the inside() method of the Bounds*/
      bool insideBounds(const Amg::Vector2D& locpos,  double tol1=0., double tol2=0.) const;       

      /**This method allows access to the subtracted part*/
      SharedObject<AreaExcluder>  subtractedVolume() const; 
            
      /** Return properly formatted class name for screen output */
      std::string name() const { return "Trk::SubtractedPlaneSurface"; }
      
    protected:
      SharedObject<AreaExcluder>          m_subtrVol; 
      bool                                m_shared;  
    };
 
  inline bool SubtractedPlaneSurface::insideBounds(const Amg::Vector2D& locpos, double tol1, double tol2) const
  {
    // no subtracted volume exists
    if (!m_subtrVol.getPtr()) return (this->bounds().inside(locpos,tol1,tol2)); 
    // subtracted volume exists, needs to be checked
    Amg::Vector3D gp(locpos.x(),locpos.y(),0.);
    if (m_shared) return (this->bounds().inside(locpos,tol1,tol2) && m_subtrVol.getPtr()->inside(gp,0.) );
    bool in(this->bounds().inside(locpos,tol1,tol2) && !m_subtrVol.getPtr()->inside(gp,0.)) ;

    return in;
  }

  inline bool SubtractedPlaneSurface::shared() const { return m_shared;} 

  inline SharedObject<AreaExcluder> SubtractedPlaneSurface::subtractedVolume() const 
  {
    return m_subtrVol;
  }
    
} // end of namespace

#endif // TRKGEOMETRYSURFACES_SUBTRACTEDPLANESURFACE_H


