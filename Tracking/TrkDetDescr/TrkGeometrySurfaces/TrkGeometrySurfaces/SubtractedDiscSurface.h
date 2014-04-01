/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedDiscSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRYSURFACES_SUBTRACTEDDISCSURFACE_H
#define TRKGEOMETRYSURFACES_SUBTRACTEDDISCSURFACE_H

//Trk
#include "TrkSurfaces/DiscSurface.h"
#include "TrkDetDescrUtils/AreaExcluder.h"
#include "TrkEventPrimitives/ParamDefs.h"

// Geometry & Maths
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {
  
  /**
   @class SubtractedDiscSurface
   Class for a planar subtracted/shared surface in the ATLAS detector.
   It owns its surface bounds and subtracted volume. 
    
   @author Sarka.Todorova@cern.ch
   */

  class SubtractedDiscSurface : public DiscSurface {
    public:
      /** Default Constructor - needed for persistency*/
      SubtractedDiscSurface();  
      
      /** Copy Constructor*/
      SubtractedDiscSurface(const SubtractedDiscSurface& psf);
      
      /** Copy Constructor*/
      SubtractedDiscSurface(const SubtractedDiscSurface& psf, const Amg::Transform3D& shift);
      
      /** Constructor */      
      SubtractedDiscSurface(const DiscSurface& ps , AreaExcluder* vol, bool shared);
      
      /**Destructor*/
      virtual ~SubtractedDiscSurface();  
      
      /**Assignment operator*/
      SubtractedDiscSurface& operator=(const SubtractedDiscSurface& psf);
      
      /**Equality operator*/
      bool operator==(const Surface& sf) const;

      /** This method indicates the subtraction mode */
      bool shared() const;

      /**This method calls the inside() method of the Bounds*/
      bool insideBounds(const Amg::Vector2D& locpos, double tol1=0., double tol2=0.) const;       

      /**This method allows access to the subtracted part*/
      SharedObject<AreaExcluder> subtractedVolume() const; 
            
      /** Return properly formatted class name for screen output */
      std::string name() const { return "Trk::SubtractedDiscSurface"; }
      
    protected:
      SharedObject<AreaExcluder>    m_subtrVol; 
      bool                          m_shared;  
    };

  inline bool SubtractedDiscSurface::insideBounds(const Amg::Vector2D& locpos, double tol1, double tol2) const
  {
    // no subtracted Volume exists
    if (!m_subtrVol.getPtr()) return (this->bounds().inside(locpos,tol1,tol2)); 
    // subtracted Volume exists, needs to be checked
    double rPos   = locpos[Trk::locR];
    double phiPos = locpos[Trk::locPhi];
    Amg::Vector3D gp(rPos*cos(phiPos),rPos*sin(phiPos),0.);
    if (m_shared) return (this->bounds().inside(locpos,tol1,tol2) && m_subtrVol.getPtr()->inside(gp,0.) );
    bool in(this->bounds().inside(locpos,tol1,tol2) && !m_subtrVol.getPtr()->inside(gp,0.)) ;

    return in;
  }

  inline bool SubtractedDiscSurface::shared() const { return m_shared;} 

  inline SharedObject<AreaExcluder> SubtractedDiscSurface::subtractedVolume() const 
  {
    return m_subtrVol;
  }
    
} // end of namespace

#endif // TRKGEOMETRYSURFACES_SUBTRACTEDDISCSURFACE_H


