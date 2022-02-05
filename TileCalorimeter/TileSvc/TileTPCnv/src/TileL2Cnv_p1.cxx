///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileL2Cnv_p1.cxx 
// Implementation file for class TileL2Cnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#include "TileEvent/TileL2.h"

// TileTPCnv includes
#include "TileTPCnv/TileL2Cnv_p1.h"


void TileL2Cnv_p1::transToPers(const TileL2* transObj, TileL2_p1* persObj, MsgStream &/*log*/) const {

  // Drawer ID
  persObj->m_ID = transObj->identify();

  // Encoded 32-bit words
  persObj->m_val.push_back(0); // one extra word which is not present in new transient class
  persObj->m_val.insert (persObj->m_val.end(),
                         transObj->val().begin(),
                         transObj->val().end());

  // Muon eta coordinate
  persObj->m_eta = transObj->eta();

  // Muon phi coordinate
  persObj->m_phi = transObj->phi(0);

  // Energy in the innermost layer
  persObj->m_enemu0 = transObj->enemu0();

  // Energy in the central layer
  persObj->m_enemu1 = transObj->enemu1();

  // Energy in the outermost layer
  persObj->m_enemu2 = transObj->enemu2();

  // Quality factor
  persObj->m_quality_factor = transObj->qual();

  // Transverse energy
  persObj->m_Et = transObj->sumE(0);

}


void TileL2Cnv_p1::persToTrans(const TileL2_p1* persObj, TileL2* transObj, MsgStream &/*log*/) const
{
  // Drawer ID
  *transObj = TileL2 (persObj->m_ID);

  // Muon phi coordinate
  // ??? This emulates the previous behavior of this converter,
  //     which was probably not right.
  transObj->m_phi = persObj->m_phi;
  transObj->m_cosphi = 0;
  transObj->m_sinphi = 0;

  // Encoded 32-bit words
  // ignore first word, which is not in new transient format
  // ???? Although the comments said that, the previous version
  // of this converter was not, in fact, ignoring the first word here.
  // That was probably a bug (a local variable was shadowed),
  // but for now, reproduce the previous behavior.
  //std::vector<unsigned int> val (persObj->m_val.begin()+1,
  //                               persObj->m_val.end());

  transObj->setMu (persObj->m_eta,
                   persObj->m_enemu0,
                   persObj->m_enemu1,
                   persObj->m_enemu2,
                   persObj->m_quality_factor,
                   persObj->m_val);
                   //val);

  // Transverse energy
  std::vector<float> sumE { persObj->m_Et };
  transObj->setEt (std::move(sumE));
}
