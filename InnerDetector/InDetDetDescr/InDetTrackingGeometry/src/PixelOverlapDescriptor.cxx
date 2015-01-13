/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// InDet
#include "InDetTrackingGeometry/PixelOverlapDescriptor.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
// Trk
#include "TrkSurfaces/Surface.h"

InDet::PixelOverlapDescriptor::PixelOverlapDescriptor()
{}


/** get the compatible surfaces */
bool InDet::PixelOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                                                      const Trk::Surface& tsf,
                                                      const Amg::Vector3D& pos,
                                                      const Amg::Vector3D&) const
                                                            
{
    // first add the target surface - it's always worth 
    cSurfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
    
    
    // make sure the return vector is cleared
    const InDetDD::SiDetectorElement* sElement = dynamic_cast<const InDetDD::SiDetectorElement*>(tsf.associatedDetectorElement());
    //!< @TODO this is full 8-cell connectivity --- way too robust: optimise
    if (sElement){
        if (sElement->isBarrel()){
            addNextInPhi(sElement,cSurfaces);
            addPrevInPhi(sElement,cSurfaces);
            addNextInEta(sElement,cSurfaces);
            addPrevInEta(sElement,cSurfaces);
            const InDetDD::SiDetectorElement* sNextInPhi = sElement->nextInPhi();
            addNextInEta(sNextInPhi,cSurfaces);
            addPrevInEta(sNextInPhi,cSurfaces);
            const InDetDD::SiDetectorElement* sPrevInPhi = sElement->prevInPhi();
            addNextInEta(sPrevInPhi,cSurfaces);
            addPrevInEta(sPrevInPhi,cSurfaces);
        } else if (sElement->isEndcap()){ // since inclusion of DBM isBarrel & isEndcap are not mutually exclusive anymore
            addNextInPhi(sElement,cSurfaces);
            addPrevInPhi(sElement,cSurfaces);
        }
    }
    return false;
}                                                            


