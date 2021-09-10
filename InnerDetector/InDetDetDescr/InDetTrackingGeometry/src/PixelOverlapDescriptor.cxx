/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// InDet
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetTrackingGeometry/PixelOverlapDescriptor.h"
#include "InDetIdentifier/PixelID.h"
// Trk
#include "TrkSurfaces/Surface.h"
//
#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

InDet::PixelOverlapDescriptor::PixelOverlapDescriptor(bool addMoreSurfaces, int eta_slices, int phi_slices):
  m_robustMode(true),
  m_addMoreSurfaces(addMoreSurfaces), 
  m_etaSlices(eta_slices),
  m_phiSlices(phi_slices) {}

/** get the compatible surfaces */
bool
InDet::PixelOverlapDescriptor::reachableSurfaces(
  std::vector<Trk::SurfaceIntersection>& cSurfaces,
  const Trk::Surface& tsf,
  const Amg::Vector3D& pos,
  const Amg::Vector3D&) const

{
  // first add the target surface - it's always worth
  cSurfaces.emplace_back(Trk::Intersection(pos, 0., true), &tsf);

  // make sure we have the correct associated element
  const auto* tmp = tsf.associatedDetectorElement();
  const InDetDD::SiDetectorElement* sElement =
    tmp->detectorType() == Trk::DetectorElemType::Silicon
      ? static_cast<const InDetDD::SiDetectorElement*>(tmp)
      : nullptr;
  if (sElement) {
    size_t newCapacity = cSurfaces.size();
    if (m_robustMode and m_addMoreSurfaces and sElement->isBarrel()) {
      // sum up the defined slices to evaluate the new capacity
      newCapacity += (2*m_phiSlices+ 2*(m_etaSlices*(2*m_phiSlices+1)));
    } else if (m_robustMode) {
      newCapacity += 8;
    } else {
      newCapacity += 1;
      if (sElement->isBarrel()) {
        newCapacity += 2;
      }
    }
    cSurfaces.reserve(newCapacity);

    // now get the overlap options
    //!< position phi and surface phi - rescale to 0 -> 2PI
    double surfacePhi = tsf.center().phi() + M_PI;
    double positionPhi = pos.phi() + M_PI;
    // 8-cell-connectivity depending on track/surface geometry
    // nPhi - can be jump + or -
    const InDetDD::SiDetectorElement* nElement = nullptr;
    // robust mode --> return 9 surfaces
    if (m_robustMode) {
      addNextInPhi(sElement, cSurfaces);
      addNextInEta(sElement, cSurfaces);

      addPrevInPhi(sElement, cSurfaces);
      addPrevInEta(sElement, cSurfaces);

      nElement = sElement->nextInPhi();
      addNextInEta(nElement, cSurfaces);
      addPrevInEta(nElement, cSurfaces);

      nElement = sElement->prevInPhi();
      addNextInEta(nElement, cSurfaces);
      addPrevInEta(nElement, cSurfaces);
      
      if (m_addMoreSurfaces and sElement->isBarrel()) {
  
        unsigned int next = 1;
        const InDetDD::SiDetectorElement* currentElement = sElement->nextInEta();
        while (currentElement and next<(unsigned int)m_etaSlices) {
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
        while (currentElement and prev<(unsigned int)m_etaSlices) {
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
          while (currentElement_nextPhi and currentElement_prevPhi and next_eta<(unsigned int)m_etaSlices) {
            addNextInEta(currentElement_nextPhi,cSurfaces);
            currentElement_nextPhi = currentElement_nextPhi->nextInEta();
            addNextInEta(currentElement_prevPhi,cSurfaces);
            currentElement_prevPhi = currentElement_prevPhi->nextInEta();
            next_eta++;
          }
          
          unsigned int prev_eta = 0;
          currentElement_nextPhi = currentElementNextPhi;
          currentElement_prevPhi = currentElementPrevPhi;
          while (currentElement_nextPhi and currentElement_prevPhi and prev_eta<(unsigned int)m_etaSlices) {
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
      if (surfacePhi < positionPhi) {
        addNextInPhi(sElement, cSurfaces);
        nElement = sElement->nextInPhi();
      } else {
        addPrevInPhi(sElement, cSurfaces);
        nElement = sElement->prevInPhi();
      }
      if (sElement->isBarrel()) {
        // get the eta information - also possible
        double positionEta = pos.eta();
        double surfaceEta = tsf.center().eta();
        // check the surface / position eta values
        if (surfaceEta < positionEta) {
          // we go next in eta for both, the original and the phi jumped one
          addNextInEta(sElement, cSurfaces);
          addNextInEta(nElement, cSurfaces);
        } else {
          // opposite direction
          addPrevInEta(sElement, cSurfaces);
          addPrevInEta(nElement, cSurfaces);
        }
      }
    }
  }
  return false;
}

bool InDet::PixelOverlapDescriptor::dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const {
  
  if (m_pixIdHelper==nullptr) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    
    // get DetectorStore service
    StoreGateSvc* detStore = nullptr;
    if (svcLocator->service("DetectorStore", detStore).isFailure()) {
      return false;
    }

    const PixelID* pixIdHelper = nullptr;
    if (detStore->retrieve(pixIdHelper, "PixelID").isFailure()) {
      return false;
    }
    m_pixIdHelper = pixIdHelper;
  }
  
  std::cout << "Dumping Surfaces for Pixel with size = " << surfaces.size() << std::endl;
  for (unsigned int surf = 0; surf < surfaces.size(); surf++) {
    Identifier hitId = ((surfaces.at(surf)).object)->associatedDetectorElementIdentifier(); 
    std::cout <<  "barrel_ec " << m_pixIdHelper.load()->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper.load()->layer_disk(hitId) << ", phi_module " << m_pixIdHelper.load()->phi_module(hitId) << ", eta_module " << m_pixIdHelper.load()->eta_module(hitId) << std::endl;
  }  
  return true;
}
