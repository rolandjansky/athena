/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_OverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// InDet
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetTrackingGeometry/SCT_OverlapDescriptor.h"
// Trk
#include "TrkSurfaces/Surface.h"

InDet::SCT_OverlapDescriptor::SCT_OverlapDescriptor()
  : m_robustMode(true)
{}

/** get the compatible surfaces */
bool
InDet::SCT_OverlapDescriptor::reachableSurfaces(
  std::vector<Trk::SurfaceIntersection>& cSurfaces,
  const Trk::Surface& tsf,
  const Amg::Vector3D& pos,
  const Amg::Vector3D&) const

{
  size_t newCapacity = cSurfaces.size() + 2;
  if (m_robustMode) {
    newCapacity += 16;
  } else {
    newCapacity += 6;
  }
  cSurfaces.reserve(newCapacity);

  // first add the target surface and the backside surface (in the if statement)
  cSurfaces.emplace_back(Trk::Intersection(pos, 0., true), &tsf);

  // make sure we have the correct associated element
  const auto* tmp = tsf.associatedDetectorElement();
  const InDetDD::SiDetectorElement* sElement =
    tmp->detectorType() == Trk::DetectorElemType::Silicon
      ? static_cast<const InDetDD::SiDetectorElement*>(tmp)
      : nullptr;
 
  //!< position phi and surface phi - rescale to 0 -> 2PI
  double surfacePhi = tsf.center().phi() + M_PI;
  double positionPhi = pos.phi() + M_PI;

  // now get the overlap options
  if (sElement) {
    addOtherSide(sElement, cSurfaces);

    // 8-cell-connectivity depending on track/surface geometry
    // nPhi - can be jump + or -
    const InDetDD::SiDetectorElement* nElement = nullptr;

    // robust mode --> return 9 (*2) surfaces
    if (m_robustMode) {
      addNextInPhiOS(sElement, cSurfaces);
      addNextInEtaOS(sElement, cSurfaces);

      addPrevInPhiOS(sElement, cSurfaces);
      addPrevInEtaOS(sElement, cSurfaces);

      nElement = sElement->nextInPhi();
      addNextInEtaOS(nElement, cSurfaces);
      addPrevInEtaOS(nElement, cSurfaces);

      nElement = sElement->prevInPhi();
      addNextInEtaOS(nElement, cSurfaces);
      addPrevInEtaOS(nElement, cSurfaces);
    } else {
      // get the phi information
      if (surfacePhi < positionPhi) {
        addNextInPhiOS(sElement, cSurfaces);
        nElement = sElement->nextInPhi();
      } else {
        addPrevInPhiOS(sElement, cSurfaces);
        nElement = sElement->prevInPhi();
      }
      // get the eta information - also possible
      double positionEta = pos.eta();
      double surfaceEta = tsf.center().eta();
      int side = (sElement->isBarrel() || surfaceEta > 0.) ? 1 : -1;
      // check the surface / position eta values
      if (side * surfaceEta < side * positionEta) {
        // we go next in eta for both, the original and the phi jumped one
        addNextInEtaOS(sElement, cSurfaces);
        addNextInEtaOS(nElement, cSurfaces);
      } else {
        // opposite direction
        addPrevInEtaOS(sElement, cSurfaces);
        addPrevInEtaOS(nElement, cSurfaces);
      }
    }
  }

  return false;
}

