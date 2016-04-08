/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRYSURFACES_SUBTRACTEDCYLINDERSURFACE_H
#define TRKGEOMETRYSURFACES_SUBTRACTEDCYLINDERSURFACE_H

//Trk
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrUtils/AreaExcluder.h"
#include "TrkEventPrimitives/ParamDefs.h"
// Geometry & Math
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
class Identifier;

namespace Trk {

  /**
   @class SubtractedCylinderSurface
   Class for a cylinder subtracted/shared surface in the ATLAS detector.
   It owns its surface bounds and subtracted volume. 
    
   @author Sarka.Todorova@cern.ch
   */

  class SubtractedCylinderSurface : public CylinderSurface {
    public:
      /** Default Constructor - needed for persistency*/
      SubtractedCylinderSurface();
      
      /** Copy Constructor*/
      SubtractedCylinderSurface(const SubtractedCylinderSurface& psf);
      
      /** Copy Constructor with shift*/
      SubtractedCylinderSurface(const SubtractedCylinderSurface& psf, const Amg::Transform3D& transf);
      
      /** Constructor */      
      SubtractedCylinderSurface(const CylinderSurface& cs, AreaExcluder* vol, bool shared);
      
      /**Destructor*/
      virtual ~SubtractedCylinderSurface();
      
      /**Assignment operator*/
      SubtractedCylinderSurface& operator=(const SubtractedCylinderSurface& psf);
      
      /**Equality operator*/
      bool operator==(const Surface& sf) const;

      /** This method indicates the subtraction mode */
      bool shared() const;
                      
      /**This method calls the inside() method of the Bounds*/
      bool insideBounds(const Amg::Vector2D& locpos, double tol1=0., double tol2=0.) const;       

      /**This method allows access to the subtracted part*/
      SharedObject<AreaExcluder> subtractedVolume() const;
      
      /** Return properly formatted class name for screen output */
      std::string name() const { return "Trk::SubtractedCylinderSurface"; }
      
    protected:
      SharedObject<AreaExcluder>           m_subtrVol;  
      bool                                 m_shared;  
    };

  inline bool SubtractedCylinderSurface::insideBounds(const Amg::Vector2D& locpos, double tol1, double tol2) const
  {
    // no subtracted volume exists
    if (!m_subtrVol.getPtr()) return (this->bounds().inside(locpos,tol1,tol2));
    // subtracted volume exists, needs to be checked
    double rCyl    = bounds().r();
    double phiPos  = locpos[Trk::locRPhi]/rCyl;
    const Amg::Vector3D gp(rCyl*cos(phiPos),rCyl*sin(phiPos),locpos[Trk::locZ]);

    bool inside_shared(this->bounds().inside(locpos,tol1,tol2) && m_subtrVol.getPtr()->inside(gp,0.) );
    bool inside(this->bounds().inside(locpos,tol1,tol2) && !m_subtrVol.getPtr()->inside(gp,0.) );

    if (m_shared) return inside_shared;
    return inside;
  }

  inline bool SubtractedCylinderSurface::shared() const { return m_shared;}

  inline SharedObject< AreaExcluder> SubtractedCylinderSurface::subtractedVolume() const 
  { 
    return m_subtrVol; 
  }
    
} // end of namespace

#endif // TRKGEOMETRYSURFACES_SUBTRACTEDCYLINDERSURFACE_H


