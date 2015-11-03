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
     addOtherSidePO(pElement,surfaces);
     addNextInPhiPO(pElement,surfaces);
     addPrevInPhiPO(pElement,surfaces);
     addNextInEtaPO(pElement,surfaces);
     addPrevInEtaPO(pElement,surfaces);
     const iFatras::PlanarDetElement* nextPhi =  pElement->nextInPhi();
     addNextInEtaPO(nextPhi,surfaces);
     addPrevInEtaPO(nextPhi,surfaces);
     const iFatras::PlanarDetElement* prevPhi =  pElement->prevInPhi();
     addNextInEtaPO(prevPhi,surfaces);
     addPrevInEtaPO(prevPhi,surfaces);
     
     if (m_addMoreSurfaces) {
       const iFatras::PlanarDetElement* currentNextPhi = nextPhi;
       const iFatras::PlanarDetElement* currentPrevPhi = prevPhi;
       const iFatras::PlanarDetElement* currentNextEta = pElement->nextInEta();
       const iFatras::PlanarDetElement* currentPrevEta = pElement->prevInEta();

       for (unsigned int slice = 0; slice < (unsigned int)m_slices; slice++) {

	 // Adding the current next in phi and its next and previous in eta
	 // then change the pointer to the next in phi
	 addNextInPhiPO(currentNextPhi,surfaces);
	 if (currentNextPhi) {
	   const iFatras::PlanarDetElement* nextCurrentNextPhi =  currentNextPhi->nextInPhi();
	   addNextInEtaPO(nextCurrentNextPhi,surfaces);
	   addPrevInEtaPO(nextCurrentNextPhi,surfaces);
	   currentNextPhi = nextCurrentNextPhi;
	 }
	 
	 // Adding the current previous in phi and its next and previous in eta
	 // then change the pointer to the previous in phi
	 addPrevInPhiPO(currentPrevPhi,surfaces);
	 if (currentPrevPhi) {
	   const iFatras::PlanarDetElement* prevCurrentPrevPhi =  currentPrevPhi->prevInPhi();
	   addNextInEtaPO(prevCurrentPrevPhi,surfaces);
	   addPrevInEtaPO(prevCurrentPrevPhi,surfaces);
	   currentPrevPhi = prevCurrentPrevPhi;
	 }
	 
	 // Adding the current next in eta and its next and previous in phi
	 // then change the pointer to the next in eta
	 if (currentNextEta && currentNextEta->nextInEta()) {
	   addNextInEtaPO(currentNextEta,surfaces);
	   const iFatras::PlanarDetElement* nextCurrentNextEta =  currentNextEta->nextInEta();
	   addNextInPhiPO(nextCurrentNextEta,surfaces);
	   addPrevInPhiPO(nextCurrentNextEta,surfaces);
	   currentNextEta = nextCurrentNextEta;
	 }
	 
	 // Adding the current previous in eta and its next and previous in phi
	 // then change the pointer to the previous in eta
	 if (currentPrevEta && currentPrevEta->prevInEta()) {
	   addPrevInEtaPO(currentPrevEta,surfaces);
	   const iFatras::PlanarDetElement* prevCurrentPrevEta =  currentPrevEta->prevInEta();
	   addNextInPhiPO(prevCurrentPrevEta,surfaces);
	   addPrevInPhiPO(prevCurrentPrevEta,surfaces);
	   currentPrevEta = prevCurrentPrevEta;
	 }
       }
     }
   }
     
   return false;
}
