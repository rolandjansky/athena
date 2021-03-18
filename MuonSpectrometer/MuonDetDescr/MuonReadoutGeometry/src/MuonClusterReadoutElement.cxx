/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"

class GeoVFullPhysVol;

namespace MuonGM {


  MuonClusterReadoutElement::MuonClusterReadoutElement(GeoVFullPhysVol* pv, std::string,
						       int zi, int fi, bool is_mirrored,
						       MuonDetectorManager* mgr)
    : MuonReadoutElement(pv, zi, fi, is_mirrored, mgr), m_surfaceData(0)
  {
    
  }
  
  
  MuonClusterReadoutElement::~MuonClusterReadoutElement()
  {
  }
  

  void MuonClusterReadoutElement::shiftSurface(const Identifier&){
    fillCache();
  }

  void MuonClusterReadoutElement::restoreSurfaces() {
    delete m_surfaceData;
    fillCache();
  }


} // namespace MuonGM
