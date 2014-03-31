///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuCnv_p1.cxx 
// Implementation file for class TileMuCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#define private public
#define protected public
#include "TileEvent/TileMu.h"
#undef private
#undef protected

// TileTPCnv includes
#include "TileTPCnv/TileMuCnv_p1.h"


void TileMuCnv_p1::transToPers(const TileMu* transObj, TileMu_p1* persObj, MsgStream &/*log*/) {

  // Muon eta coordinate
  persObj->m_eta = transObj->m_eta;

  // Muon phi coordinate
  persObj->m_phi = transObj->m_phi;

  // Muon quality factor
  persObj->m_quality_factor = transObj->m_quality_factor;

  // Energy deposited by the muon in TileCal
  for (std::vector<float>::const_iterator it = transObj->m_energy_deposited.begin(); it != transObj->m_energy_deposited.end(); ++it) {
    persObj->m_energy_deposited.push_back( *it );
  }

}


void TileMuCnv_p1::persToTrans(const TileMu_p1* persObj, TileMu* transObj, MsgStream &/*log*/) {

  // Muon eta coordinate
  transObj->m_eta = persObj->m_eta;

  // Muon phi coordinate
  transObj->m_phi = persObj->m_phi;

  // Muon quality factor
  transObj->m_quality_factor = persObj->m_quality_factor;

  // Energy deposited by the muon in TileCal
  transObj->m_energy_deposited.reserve(persObj->m_energy_deposited.size());
  for (std::vector<float>::const_iterator it = persObj->m_energy_deposited.begin(); it != persObj->m_energy_deposited.end(); ++it) {
    transObj->m_energy_deposited.push_back( *it );
  }

}
