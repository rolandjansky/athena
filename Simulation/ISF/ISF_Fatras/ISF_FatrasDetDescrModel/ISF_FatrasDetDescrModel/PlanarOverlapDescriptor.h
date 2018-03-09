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
#define addSurfacePO(cur,surfaces) if (cur) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->surface(cur->identify()))))
#define addOtherSidePO(cur, surfaces) if (cur && cur->otherSide()) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->otherSide()->surface(cur->otherSide()->identify()))))
#define addNextInPhiPO(cur, surfaces) addSurfacePO(cur->nextInPhi(), surfaces); addOtherSidePO(cur->nextInPhi(),surfaces)
#define addPrevInPhiPO(cur, surfaces) addSurfacePO(cur->prevInPhi(), surfaces); addOtherSidePO(cur->prevInPhi(),surfaces)
#define addNextInEtaPO(cur, surfaces) addSurfacePO(cur->nextInEta(), surfaces); addOtherSidePO(cur->nextInEta(),surfaces)
#define addPrevInEtaPO(cur, surfaces) addSurfacePO(cur->prevInEta(), surfaces); addOtherSidePO(cur->prevInEta(),surfaces)
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
    PlanarOverlapDescriptor(bool addMoreSurfaces = false, int slices = 3);
         
    /** Destructor */
    virtual ~PlanarOverlapDescriptor(){}
         
    /**Pseudo-Constructor*/
    virtual PlanarOverlapDescriptor* clone() const override;
    
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

    bool m_addMoreSurfaces;
    int  m_slices;
    
  };
  
  
  inline PlanarOverlapDescriptor* PlanarOverlapDescriptor::clone() const { return new PlanarOverlapDescriptor(); }     
  
}

#endif // ISF_DETDESCRMODEL_PLANAROVERLAPDESCRIPTOR_H
