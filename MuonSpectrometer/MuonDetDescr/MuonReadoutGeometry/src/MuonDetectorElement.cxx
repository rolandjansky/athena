/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ----------------------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/MuonDetectorElement.h"

class GeoVFullPhysVol;

namespace MuonGM {

  MuonDetectorElement::MuonDetectorElement(GeoVFullPhysVol* pv, 
                                           MuonDetectorManager* mgr,
                                           Identifier id,
                                           IdentifierHash idHash) : 
    TrkDetElementBase(pv)
  {
    m_muon_mgr = mgr;
    m_id = id;
    m_idhash = idHash;
    m_nREinDetectorElement = 0;
  }

  MuonDetectorElement::~MuonDetectorElement()=default;
    
}
