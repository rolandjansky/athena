 
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_OverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include <utility>



#include "GeoPrimitives/GeoPrimitives.h"
// HGTD
#include "HGTD_TrackingGeometry/HGTD_OverlapDescriptor.h"
#include "HGTD_Identifier/HGTD_ID.h"
// Trk
#include "TrkSurfaces/Surface.h"
//
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"


HGTD_OverlapDescriptor::HGTD_OverlapDescriptor(const Trk::BinnedArray<const Trk::Surface>* bin_array,
                                               std::vector < float > valuesR,
                                               std::vector < std::vector< float> > valuesPhi,
                                               int nStepsR, int nStepsPhi):
  m_binnedArray(bin_array),
  m_valuesR(std::move(valuesR)),
  m_valuesPhi(std::move(valuesPhi)),
  m_nStepsR(nStepsR),
  m_nStepsPhi(nStepsPhi)
{}

/** get the compatible surfaces */
bool HGTD_OverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                                               const Trk::Surface& tsf,
                                               const Amg::Vector3D& pos,
                                               const Amg::Vector3D&) const
  
{
  surfaces.emplace_back(Trk::Intersection(pos, 0., true),&tsf);
  
  // add the other targets
  // use the center of this surface in (x,y) global coordinates and look for
  // adjactent ones in the binutiliy  
  
  float centerR = tsf.center().perp();
  float centerPhi = tsf.center().phi();
  
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
    surfaces.emplace_back(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),surface);
  
  return false;
}


bool HGTD_OverlapDescriptor::dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const {
  
  if (m_hgtdIdHelper==nullptr) {
    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    
    // get DetectorStore service
    StoreGateSvc* detStore;
    if (svcLocator->service("DetectorStore",detStore).isFailure()) {
      return false;
    }
    
    const HGTD_ID* hgtdIdHelper = nullptr;
    if (detStore->retrieve(hgtdIdHelper, "HGTD_ID").isFailure()) {
      return false;
    }
    m_hgtdIdHelper = hgtdIdHelper;
  }
  
  std::cout << "Dumping Surfaces for HGTD with size = " << surfaces.size() << std::endl;
  for (unsigned int surf = 0; surf < surfaces.size(); surf++) {
    Identifier hitId = ((surfaces.at(surf)).object)->associatedDetectorElementIdentifier(); 
    std::cout <<  "barrel_ec " << m_hgtdIdHelper.load()->endcap(hitId) << ", layer_disk " << m_hgtdIdHelper.load()->layer(hitId) << ", phi_module " << m_hgtdIdHelper.load()->phi_module(hitId) << ", eta_module " << m_hgtdIdHelper.load()->eta_module(hitId) << std::endl;
  }
  return true;
}
