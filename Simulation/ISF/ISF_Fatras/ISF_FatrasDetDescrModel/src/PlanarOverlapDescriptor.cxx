/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// iFatras
#include "ISF_FatrasDetDescrModel/PlanarOverlapDescriptor.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"

iFatras::PlanarOverlapDescriptor::PlanarOverlapDescriptor() {}


/** get the compatible surfaces 
    - return vector : surfaces
    - primary bin surface : sf
    - position & direction : pos, dir
*/
bool iFatras::PlanarOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                                                           const Trk::Surface& tsf,
                                                           const Amg::Vector3D& pos,
                                                           const Amg::Vector3D&) const 
{

    // get the Planar element
   const iFatras::PlanarDetElement* pElement = dynamic_cast<const iFatras::PlanarDetElement*>(tsf.associatedDetectorElement());

   // first add the target surface
   surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
   
   if (pElement){
     addNextInPhi(pElement,surfaces);
     addPrevInPhi(pElement,surfaces);
     addNextInEta(pElement,surfaces);
     addPrevInEta(pElement,surfaces);
     const iFatras::PlanarDetElement* nextPhi =  pElement->nextInPhi();
     addNextInEta(nextPhi,surfaces);
     addPrevInEta(nextPhi,surfaces);
     const iFatras::PlanarDetElement* prevPhi =  pElement->prevInPhi();
     addNextInEta(prevPhi,surfaces);
     addPrevInEta(prevPhi,surfaces);
   }

   return false;

}
