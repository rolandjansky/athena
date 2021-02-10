/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_OverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// iFatras
#include "HGTD_TrackingGeometry/HGTD_OverlapDescriptor.h"
#include "HGTD_Identifier/HGTD_ID.h"

// Trk
#include "TrkSurfaces/Surface.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"

HGTDet::HGTD_OverlapDescriptor::HGTD_OverlapDescriptor(const Trk::BinnedArray<Trk::Surface>* bin_array,
                                                       std::vector < float > valuesR,
                                                       std::vector < std::vector< float> > valuesPhi,
                                                       int nStepsR, int nStepsPhi):
  m_binnedArray(bin_array),
  m_valuesR(valuesR),
  m_valuesPhi(valuesPhi),
  m_nStepsR(nStepsR),
  m_nStepsPhi(nStepsPhi)
{}

/** get the compatible surfaces */
bool HGTDet::HGTD_OverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                                                       const Trk::Surface& tsf,
                                                       const Amg::Vector3D& pos,
                                                       const Amg::Vector3D&) const
  
{
  surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
  
  // add the other targets
  // use the center of this surface in (x,y) global coordinates and look for
  // adjactent ones in the binutiliy  
  return getSurfaces(surfaces, tsf.center());
}

bool HGTDet::HGTD_OverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                                                       const Amg::Vector3D& pos,
                                                       const Amg::Vector3D&) const
  
{
  // add the other targets if any starting from the position you have
  // in global coordinates and look for adjactent ones in the binutiliy  
  return getSurfaces(surfaces, pos);
}

bool HGTDet::HGTD_OverlapDescriptor::getSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces,
                                                 const Amg::Vector3D& pos) const 
{
  float centerR = pos.perp();
  float centerPhi = pos.phi();
  
  std::vector<const Trk::Surface*> allSurfaces = {};
  
  const auto rBoundVal = std::lower_bound(m_valuesR.begin(), m_valuesR.end(), centerR);
  int rIndex = std::distance(m_valuesR.begin(), rBoundVal);
  
  for (int stepR=-m_nStepsR; stepR<=m_nStepsR; stepR++) {
    int index_r = rIndex+stepR;
    if (index_r>=0 and index_r<int(m_valuesR.size()-1)) {
      const auto phiBoundVal = std::lower_bound(m_valuesPhi.at(index_r).begin(), m_valuesPhi.at(index_r).end(), centerPhi);
      int phiIndex = std::distance(m_valuesPhi.at(index_r).begin(), phiBoundVal);
      for (int stepPhi=-m_nStepsPhi; stepPhi<=m_nStepsPhi; stepPhi++) {
        int index_phi = phiIndex+stepPhi;
        if (index_phi<0)
          index_phi = int(m_valuesPhi.at(index_r).size())+index_phi;
        else if (index_phi>int(m_valuesPhi.at(index_r).size()-1))
          index_phi = index_phi-int(m_valuesPhi.at(index_r).size());
        float pos_r = 0.5*(m_valuesR.at(index_r)+m_valuesR.at(index_r+1));
        float pos_phi = 0.;
        if (index_phi == int(m_valuesPhi.at(index_r).size()-1))
          pos_phi = 0.5*(m_valuesPhi.at(index_r).at(index_phi)+M_PI);
        else
          pos_phi = 0.5*(m_valuesPhi.at(index_r).at(index_phi)+m_valuesPhi.at(index_r).at(index_phi+1));          
        const Trk::Surface* surf = m_binnedArray->object(Amg::Vector2D(pos_r, pos_phi));
        if (surf and std::find(allSurfaces.begin(), allSurfaces.end(), surf) == allSurfaces.end()) {
          allSurfaces.push_back(surf);
        }
      }
    }
  }
  
  for (auto& surface : allSurfaces)
    surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),surface));
  
//   dumpSurfaces(surfaces);
   
  return false;
}


void HGTDet::HGTD_OverlapDescriptor::dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const {
  
  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();
    
  // get DetectorStore service
  StoreGateSvc* detStore;
  StatusCode sc = svcLocator->service("DetectorStore",detStore);
  if (sc.isFailure()) {
    return;
  }
  
  const HGTD_ID* hgtdId = 0;
  if (detStore->retrieve(hgtdId, "HGTD_ID").isFailure()) {
    return;
  }
  
  std::cout << "Dumping Surfaces for HGTD with size = " << surfaces.size() << std::endl;
  for (unsigned int surf = 0; surf < surfaces.size(); surf++) {
    Identifier hitId = ((surfaces.at(surf)).object)->associatedDetectorElementIdentifier(); 
    std::cout <<  "barrel_ec " << hgtdId->endcap(hitId) << ", layer_disk " << hgtdId->layer(hitId) << ", phi_module " << hgtdId->phi_module(hitId) << ", eta_module " << hgtdId->eta_module(hitId) << std::endl;
  }
}


