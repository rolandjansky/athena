///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2Cnv_p1.cxx 
// Implementation file for class TileL2Cnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#define private public
#define protected public
#include "TileEvent/TileL2.h"
#undef private
#undef protected

// TileTPCnv includes
#include "TileTPCnv/TileL2Cnv_p1.h"


void TileL2Cnv_p1::transToPers(const TileL2* transObj, TileL2_p1* persObj, MsgStream &/*log*/) {

  // Drawer ID
  persObj->m_ID = transObj->m_ID;

  // Encoded 32-bit words
  persObj->m_val.push_back(0); // one extra word which is not present in new transient class
  for (std::vector<unsigned int>::const_iterator it = transObj->m_val.begin(); it != transObj->m_val.end(); ++it) {
    persObj->m_val.push_back((unsigned int) *it );
  }

  // Muon eta coordinate
  for (std::vector<float>::const_iterator it = transObj->m_eta.begin(); it != transObj->m_eta.end(); ++it) {
    persObj->m_eta.push_back( *it );
  }

  // Muon phi coordinate
  persObj->m_phi = transObj->m_phi;

  // Energy in the innermost layer
  for (std::vector<float>::const_iterator it = transObj->m_enemu0.begin(); it != transObj->m_enemu0.end(); ++it) {
    persObj->m_enemu0.push_back( *it );
  }

  // Energy in the central layer
  for (std::vector<float>::const_iterator it = transObj->m_enemu1.begin(); it != transObj->m_enemu1.end(); ++it) {
    persObj->m_enemu1.push_back( *it );
  }

  // Energy in the outermost layer
  for (std::vector<float>::const_iterator it = transObj->m_enemu2.begin(); it != transObj->m_enemu2.end(); ++it) {
    persObj->m_enemu2.push_back( *it );
  }

  // Quality factor
  for (std::vector<unsigned int>::const_iterator it = transObj->m_quality_factor.begin(); it != transObj->m_quality_factor.end(); ++it) {
    persObj->m_quality_factor.push_back((unsigned int) *it );
  }

  // Transverse energy
  persObj->m_Et = transObj->m_sumE[0];

}


void TileL2Cnv_p1::persToTrans(const TileL2_p1* persObj, TileL2* transObj, MsgStream &/*log*/) {

  transObj->m_eta.reserve(persObj->m_eta.size());

  // Drawer ID
  transObj->m_ID = persObj->m_ID;

  // Encoded 32-bit words
  std::vector<unsigned int>::const_iterator it = persObj->m_val.begin();
  if (it != persObj->m_val.end()) {
    ++it; // ignore first word, which is not in new transient format
    for (std::vector<unsigned int>::const_iterator it = persObj->m_val.begin(); it != persObj->m_val.end(); ++it) {
      transObj->m_val.push_back((unsigned int) *it );
    }
  }

  // Muon eta coordinate
  for (std::vector<float>::const_iterator it = persObj->m_eta.begin(); it != persObj->m_eta.end(); ++it) {
    transObj->m_eta.push_back( *it );
  }

  // Muon phi coordinate
  transObj->m_phi = persObj->m_phi;

  // Energy in the innermost layer
  for (std::vector<float>::const_iterator it = persObj->m_enemu0.begin(); it != persObj->m_enemu0.end(); ++it) {
    transObj->m_enemu0.push_back( *it );
  }

  // Energy in the central layer
  for (std::vector<float>::const_iterator it = persObj->m_enemu1.begin(); it != persObj->m_enemu1.end(); ++it) {
    transObj->m_enemu1.push_back( *it );
  }

  // Energy in the outermost layer
  for (std::vector<float>::const_iterator it = persObj->m_enemu2.begin(); it != persObj->m_enemu2.end(); ++it) {
    transObj->m_enemu2.push_back( *it );
  }

  // Quality factor
  for (std::vector<unsigned int>::const_iterator it = persObj->m_quality_factor.begin(); it != persObj->m_quality_factor.end(); ++it) {
    transObj->m_quality_factor.push_back((unsigned int) *it );
  }

  // Transverse energy
  transObj->m_sumE.push_back(persObj->m_Et);

}
