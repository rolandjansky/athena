///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileMuCnv_p1.cxx 
// Implementation file for class TileMuCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#include "TileEvent/TileMu.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuCnv_p1.h"


void TileMuCnv_p1::transToPers(const TileMu* transObj, TileMu_p1* persObj, MsgStream &/*log*/) const {

  // Muon eta coordinate
  persObj->m_eta = transObj->eta();

  // Muon phi coordinate
  persObj->m_phi = transObj->phi();

  // Muon quality factor
  persObj->m_quality_factor = transObj->quality();

  // Energy deposited by the muon in TileCal
  persObj->m_energy_deposited = transObj->enedep();
}


void TileMuCnv_p1::persToTrans(const TileMu_p1* persObj, TileMu* transObj, MsgStream &/*log*/) const {

  *transObj = TileMu (persObj->m_eta,
                      persObj->m_phi,
                      persObj->m_energy_deposited,
                      persObj->m_quality_factor);
}
