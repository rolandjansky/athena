/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// InDet
#include "InDetTrackingGeometryUtils/PixelOverlapDescriptor.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"
// Trk
#include "TrkSurfaces/Surface.h"

#include "StoreGate/StoreGateSvc.h"

InDet::PixelOverlapDescriptor::PixelOverlapDescriptor(bool addMoreSurfaces, int eta_slices, int phi_slices, bool addStaves):
  m_robustMode(true),
  m_addMoreSurfaces(addMoreSurfaces), 
  m_etaSlices(eta_slices),
  m_phiSlices(phi_slices),
  m_addEntireStave(addStaves) {}

/** get the compatible surfaces */
bool InDet::PixelOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces, 
                                                      const Trk::Surface& tsf,
                                                      const Amg::Vector3D& pos,
                                                      const Amg::Vector3D& ) const
                                                            
{
  // first add the target surface - it's always worth 
  cSurfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
  
  // make sure the return vector is cleared
  const InDetDD::SiDetectorElement* sElement = dynamic_cast<const InDetDD::SiDetectorElement*>(tsf.associatedDetectorElement());
  // now get the overlap options
  if (sElement){
    //!< position phi and surface phi - rescale to 0 -> 2PI
    double surfacePhi   = tsf.center().phi() + M_PI;
    double positionPhi  = pos.phi() + M_PI;
    // 8-cell-connectivity depending on track/surface geometry
    // nPhi - can be jump + or -
    const InDetDD::SiDetectorElement* nElement = 0;
    // robust mode --> return 9 surfaces
    if (m_robustMode) {
      
      addNextInPhi(sElement,cSurfaces);
      addPrevInPhi(sElement,cSurfaces);

      addNextInEta(sElement,cSurfaces);
      addPrevInEta(sElement,cSurfaces);
      
      nElement = sElement->nextInPhi();
      addNextInEta(nElement,cSurfaces);
      addPrevInEta(nElement,cSurfaces);
      
      nElement = sElement->prevInPhi();
      addNextInEta(nElement,cSurfaces);
      addPrevInEta(nElement,cSurfaces);
      
      
      if (m_addMoreSurfaces and sElement->isBarrel()) {
	
	unsigned int eta_slices = m_addEntireStave ? 9999 : m_etaSlices;
	
	unsigned int next = 1;
	const InDetDD::SiDetectorElement* currentElement = sElement->nextInEta();
	while (currentElement and next<eta_slices) {
	  addNextInEta(currentElement,cSurfaces);
	  currentElement = currentElement->nextInEta();
	  if (currentElement) {
	    addNextInPhi(currentElement,cSurfaces);
	    addPrevInPhi(currentElement,cSurfaces);
	  }
	  next++;
	}
	
	unsigned int prev = 1;
	currentElement = sElement->prevInEta();
	while (currentElement and prev<eta_slices) {
	  addPrevInEta(currentElement,cSurfaces);
	  currentElement = currentElement->prevInEta();
	  if (currentElement) {
	    addNextInPhi(currentElement,cSurfaces);
	    addPrevInPhi(currentElement,cSurfaces);
	  }
	  prev++;
	}
	
	unsigned int next_phi = 0;
	const InDetDD::SiDetectorElement* currentElementNextPhi = sElement->nextInPhi()->nextInPhi();
	const InDetDD::SiDetectorElement* currentElementPrevPhi = sElement->prevInPhi()->prevInPhi();
	
	while (next_phi<(unsigned int)m_phiSlices) {
	  addSurface(currentElementNextPhi,cSurfaces);
	  addSurface(currentElementPrevPhi,cSurfaces);
	  
	  unsigned int next_eta = 0;
	  const InDetDD::SiDetectorElement* currentElement_nextPhi = currentElementNextPhi;
	  const InDetDD::SiDetectorElement* currentElement_prevPhi = currentElementPrevPhi;
	  while (currentElement_nextPhi and currentElement_prevPhi and next_eta<eta_slices) {
	    addNextInEta(currentElement_nextPhi,cSurfaces);
	    currentElement_nextPhi = currentElement_nextPhi->nextInEta();
	    addNextInEta(currentElement_prevPhi,cSurfaces);
	    currentElement_prevPhi = currentElement_prevPhi->nextInEta();
	    next_eta++;
	  }
	  
	  unsigned int prev_eta = 0;
	  currentElement_nextPhi = currentElementNextPhi;
	  currentElement_prevPhi = currentElementPrevPhi;
	  while (currentElement_nextPhi and currentElement_prevPhi and prev_eta<eta_slices) {
	    addPrevInEta(currentElement_nextPhi,cSurfaces);
	    currentElement_nextPhi = currentElement_nextPhi->prevInEta();
	    addPrevInEta(currentElement_prevPhi,cSurfaces);
	    currentElement_prevPhi = currentElement_prevPhi->prevInEta();
	    prev_eta++;
	  }
	  
	  currentElementNextPhi = currentElementNextPhi->nextInPhi();
	  currentElementPrevPhi = currentElementPrevPhi->prevInPhi();
	  
	  next_phi++;
	}
      }
    } else {
      // we go next in phi
      if (surfacePhi < positionPhi){
	addNextInPhi(sElement,cSurfaces);
	nElement = sElement->nextInPhi();
      } else {
	addPrevInPhi(sElement,cSurfaces);
	nElement = sElement->prevInPhi();
      }
      if (sElement->isBarrel()){
	// get the eta information - also possible
	double positionEta  = pos.eta(); 
	double surfaceEta   = tsf.center().eta();
	// check the surface / position eta values
	if (surfaceEta < positionEta){
	  // we go next in eta for both, the original and the phi jumped one
	  addNextInEta(sElement,cSurfaces);
	  addNextInEta(nElement,cSurfaces);
	} else {
	  // opposite direction
	  addPrevInEta(sElement,cSurfaces);
	  addPrevInEta(nElement,cSurfaces);
	}
      }
    }
  }  

  //dumpSurfaces(cSurfaces);
  return false;
}                                                            

void InDet::PixelOverlapDescriptor::dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const {
  
  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();
 
  // get DetectorStore service
  StoreGateSvc* detStore;
  StatusCode sc = svcLocator->service("DetectorStore",detStore);
  if (sc.isFailure()) {
    return;
  }
  
  const PixelID* m_pixIdHelper = 0;
  if (detStore->retrieve(m_pixIdHelper, "PixelID").isFailure()) return;
  
  std::cout << "Dumping Surfaces for Pixel with size = " << surfaces.size() << std::endl;
  for (unsigned int surf = 0; surf < surfaces.size(); surf++) {
    Identifier hitId = ((surfaces.at(surf)).object)->associatedDetectorElementIdentifier(); 
    std::cout <<  "barrel_ec " << m_pixIdHelper->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper->layer_disk(hitId) << ", phi_module " << m_pixIdHelper->phi_module(hitId) << ", eta_module " << m_pixIdHelper->eta_module(hitId) << std::endl;
  }  
}
