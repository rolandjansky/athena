/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"

namespace MuonGM {


  MuonClusterReadoutElement::MuonClusterReadoutElement(GeoVFullPhysVol* pv, std::string,
						       int zi, int fi, bool is_mirrored,
						       MuonDetectorManager* mgr)
    : MuonReadoutElement(pv, zi, fi, is_mirrored, mgr), m_surfaceData(0), m_surfaceDataBackup(0)
  {
    
  }
  
  
  MuonClusterReadoutElement::~MuonClusterReadoutElement()
  {
  }
  

  void MuonClusterReadoutElement::shiftSurface(const Identifier&) const{
    if( m_surfaceDataBackup ){
      delete m_surfaceDataBackup;
    }
    m_surfaceDataBackup = m_surfaceData;
    fillCache();
  }

  void MuonClusterReadoutElement::restoreSurfaces() const {
    delete m_surfaceData;
    if( m_surfaceDataBackup ) m_surfaceData = m_surfaceDataBackup;
    else fillCache();
  }


} // namespace MuonGM
