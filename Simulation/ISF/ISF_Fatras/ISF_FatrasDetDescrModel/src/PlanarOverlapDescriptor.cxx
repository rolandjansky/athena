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

iFatras::PlanarOverlapDescriptor::PlanarOverlapDescriptor(bool addMoreSurfaces, int slices) : m_addMoreSurfaces(addMoreSurfaces), m_slices(slices) {}


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
   ////std::cout << "--> Surfaces = " << surfaces.size() << std::endl;
   // first add the target surface
   surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
   
   if (pElement){
     addOtherSide(pElement,surfaces);
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
     
     if (m_addMoreSurfaces) {
       const iFatras::PlanarDetElement* currentNextPhi = nextPhi;
       const iFatras::PlanarDetElement* currentPrevPhi = prevPhi;
       const iFatras::PlanarDetElement* currentNextEta = pElement->nextInEta();
       const iFatras::PlanarDetElement* currentPrevEta = pElement->prevInEta();

       for (unsigned int slice = 0; slice < (unsigned int)m_slices; slice++) {

	 // Adding the current next in phi and its next and previous in eta
	 // then change the pointer to the next in phi
	 addNextInPhi(currentNextPhi,surfaces);
	 if (currentNextPhi) {
	   const iFatras::PlanarDetElement* nextCurrentNextPhi =  currentNextPhi->nextInPhi();
	   addNextInEta(nextCurrentNextPhi,surfaces);
	   addPrevInEta(nextCurrentNextPhi,surfaces);
	   currentNextPhi = nextCurrentNextPhi;
	 }
	 
	 // Adding the current previous in phi and its next and previous in eta
	 // then change the pointer to the previous in phi
	 addPrevInPhi(currentPrevPhi,surfaces);
	 if (currentPrevPhi) {
	   const iFatras::PlanarDetElement* prevCurrentPrevPhi =  currentPrevPhi->prevInPhi();
	   addNextInEta(prevCurrentPrevPhi,surfaces);
	   addPrevInEta(prevCurrentPrevPhi,surfaces);
	   currentPrevPhi = prevCurrentPrevPhi;
	 }
	 
	 // Adding the current next in eta and its next and previous in phi
	 // then change the pointer to the next in eta
	 if (currentNextEta && currentNextEta->nextInEta()) {
	   addNextInEta(currentNextEta,surfaces);
	   const iFatras::PlanarDetElement* nextCurrentNextEta =  currentNextEta->nextInEta();
	   addNextInPhi(nextCurrentNextEta,surfaces);
	   addPrevInPhi(nextCurrentNextEta,surfaces);
	   currentNextEta = nextCurrentNextEta;
	 }
	 
	 // Adding the current previous in eta and its next and previous in phi
	 // then change the pointer to the previous in eta
	 if (currentPrevEta && currentPrevEta->prevInEta()) {
	   addPrevInEta(currentPrevEta,surfaces);
	   const iFatras::PlanarDetElement* prevCurrentPrevEta =  currentPrevEta->prevInEta();
	   addNextInPhi(prevCurrentPrevEta,surfaces);
	   addPrevInPhi(prevCurrentPrevEta,surfaces);
	   currentPrevEta = prevCurrentPrevEta;
	 }
       }
     }
   }
     
   return false;
}
