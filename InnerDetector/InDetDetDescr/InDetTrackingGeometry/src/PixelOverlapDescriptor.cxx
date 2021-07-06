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
// Trk
#include "TrkSurfaces/Surface.h"

InDet::PixelOverlapDescriptor::PixelOverlapDescriptor()
  : m_robustMode(true)
{}

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
  // now get the overlap options
  if (sElement) {
    size_t newCapacity = cSurfaces.size();
    if (m_robustMode) {
      newCapacity += 8;
    } else {
      newCapacity += 1;
      if (sElement->isBarrel()) {
        newCapacity += 2;
      }
    }
    cSurfaces.reserve(newCapacity);

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

