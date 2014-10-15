/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarOverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_DETDESCRMODEL_PLANAROVERLAPDESCRIPTOR_H
#define ISF_DETDESCRMODEL_PLANAROVERLAPDESCRIPTOR_H

// Trk
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetDescrUtils/Intersection.h"

#ifndef ISF_DETDESCRMODEL_PLANARDETADDNEXTPHIETA
#define ISF_DETDESCRMODEL_PLANARDETADDNEXTPHIETA
#define addSurface(cur,surfaces) if (cur) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->surface(cur->identify()))))
#define addOtherSide(cur, surfaces) if (cur && cur->otherSide()) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->otherSide()->surface(cur->otherSide()->identify()))))
#define addNextInPhi(cur, surfaces) addSurface(cur->nextInPhi(), surfaces); addOtherSide(cur->nextInPhi(),surfaces)
#define addPrevInPhi(cur, surfaces) addSurface(cur->prevInPhi(), surfaces); addOtherSide(cur->prevInPhi(),surfaces)
#define addNextInEta(cur, surfaces) addSurface(cur->nextInEta(), surfaces); addOtherSide(cur->nextInEta(),surfaces)
#define addPrevInEta(cur, surfaces) addSurface(cur->prevInEta(), surfaces); addOtherSide(cur->prevInEta(),surfaces)
#endif // ISF_DETDESCRMODEL_PLANARDETADDNEXTPHIETA

namespace Trk {
  class Surface;
}

namespace iFatras {
    
  class PlanarDetElement;
  
  /** @class PlanarOverlapDescriptor
         
      Class to describe overlaps in the custom detector,
      it extends the Trk::OverlapDescriptor base class.
        
      There are two interface methods, one provides the most probably overlapcell,
      the second provides a list of overlap cells, based on an restricted area
        
      @author Noemi.Calace@cern.ch
        
  */
  class PlanarOverlapDescriptor : public Trk::OverlapDescriptor {
  public:
         
    /** Constructor (area restriction, LC check) */
    PlanarOverlapDescriptor();
         
    /** Destructor */
    virtual ~PlanarOverlapDescriptor(){}
         
    /**Pseudo-Constructor*/
    PlanarOverlapDescriptor* clone() const;
    
    /** get the compatible surfaces 
        - return vector : surfaces
        - primary bin surface : sf
        - position & direction : pos, dir
    */
    bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                           const Trk::Surface& sf,
                           const Amg::Vector3D& pos,
                           const Amg::Vector3D& dir) const override;
                                      
             
  private:
    
  };
  
  
  inline PlanarOverlapDescriptor* PlanarOverlapDescriptor::clone() const { return new PlanarOverlapDescriptor(); }     
  
}

#endif // ISF_DETDESCRMODEL_PLANAROVERLAPDESCRIPTOR_H
