///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileL2Cnv_p2.cxx 
// Implementation file for class TileL2Cnv_p2
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#include "TileEvent/TileL2.h"

// TileTPCnv includes
#include "TileTPCnv/TileL2Cnv_p2.h"
#include <atomic>

// #define TILEL2_DEBUG true

#ifdef TILEL2_DEBUG
#include <iostream>
#endif

static std::atomic<bool> old_format_print=true;

void TileL2Cnv_p2::transToPers(const TileL2* transObj, TileL2_p2* persObj, MsgStream &log) const {

  unsigned int l1 = transObj->sumEVec().size();
  unsigned int l2 = transObj->eta().size();
  unsigned int l3 = transObj->val().size();
  unsigned int l4 = transObj->qual().size();
  
  if (l1>15 || l2>15 || l3>15 || l4>15) {
      log << MSG::ERROR << "TOO BIG SIZE IN TileL2Cnv_p2::transToPers !!! " 
          << " l1=" << l1
          << " l2=" << l2
          << " l3=" << l3
          << " l4=" << l4
          << endmsg;
      persObj->m_ID = (transObj->identify() & 0xFFFF);
      return;
  }

  persObj->m_fval.clear();
  persObj->m_ival.clear();
  persObj->m_fval.reserve(l1+4*l2);
  persObj->m_ival.reserve(l3+l4);

  // Transverse energy
  if (l1>0) {
    bool allzeros=true;
    for (float f : transObj->sumEVec()) {
      persObj->m_fval.push_back( f );
      if (fabs(f) > 1.e-6) allzeros=false;
    }
    if (allzeros) {
      l1=0;
      persObj->m_fval.clear();
    }
  }

  // Drawer ID
  persObj->m_ID = (l1<<28) | (l2<<24) | (l3<<20) | (l4<<16) | (transObj->identify() & 0xFFFF);

  if (l2) {
    // Muon eta coordinate
    persObj->m_fval.insert (persObj->m_fval.end(),
                            transObj->eta().begin(),
                            transObj->eta().end());

    // Energy in the innermost layer
    persObj->m_fval.insert (persObj->m_fval.end(),
                            transObj->enemu0().begin(),
                            transObj->enemu0().end());


    // Energy in the central layer
    persObj->m_fval.insert (persObj->m_fval.end(),
                            transObj->enemu1().begin(),
                            transObj->enemu1().end());

    // Energy in the outermost layer
    persObj->m_fval.insert (persObj->m_fval.end(),
                            transObj->enemu2().begin(),
                            transObj->enemu2().end());

    if (l1 + 4*l2 != persObj->m_fval.size()) {
      log << MSG::ERROR << "WRONG SIZE IN TileL2Cnv_p2::transToPers !!! " 
          << " l1=" << l1
          << " l2=" << l2
          << "(" << transObj->eta().size()
          << "," << transObj->enemu0().size()
          << "," << transObj->enemu1().size()
          << "," << transObj->enemu2().size()
          << ")  f_size=" << persObj->m_fval.size()
          << endmsg;
    }
  }
  
  // Encoded 32-bit words
  persObj->m_ival.insert (persObj->m_ival.end(),
                          transObj->val().begin(),
                          transObj->val().end());

  // Quality factor
  persObj->m_ival.insert (persObj->m_ival.end(),
                          transObj->qual().begin(),
                          transObj->qual().end());


#ifdef TILEL2_DEBUG
  transObj->print();
  std::cout << "pers TileL2 0x"<< std::hex << persObj->m_ID << "  " << std::dec;
  for (unsigned int i=0; i<persObj->m_fval.size(); ++i) 
      std::cout << persObj->m_fval[i] << " ";
  for (unsigned int i=0; i<persObj->m_ival.size(); ++i) 
      std::cout << " " << persObj->m_ival[i];
  std::cout << std::endl;
#endif
}


void TileL2Cnv_p2::persToTrans(const TileL2_p2* persObj, TileL2* transObj, MsgStream &log) const {

  unsigned int l0 = persObj->m_ID;

  // Drawer ID
  *transObj = TileL2 (l0 & 0xFFFF);

  unsigned int l1 = (l0 >> 28) & 0xF;
  unsigned int l2 = (l0 >> 24) & 0xF;
  unsigned int l3 = (l0 >> 20) & 0xF;
  unsigned int l4 = (l0 >> 16) & 0xF;

  std::vector<float>::const_iterator it = persObj->m_fval.begin();
  std::vector<unsigned int>::const_iterator iti = persObj->m_ival.begin();

  if (l1 + 4*l2 != persObj->m_fval.size() || l3+l4 != persObj->m_ival.size()) {
      log << MSG::ERROR << "WRONG SIZE IN TileL2Cnv_p2::persToTrans !!! " 
          << " id=0x" << MSG::hex << l0 << MSG::dec 
          << " f_size=" << persObj->m_fval.size()
          << " i_size=" << persObj->m_ival.size()
          << endmsg;
      return;
  }

  std::vector<float> sumE;
  if (l1>2 || l3 == 2*l2 ) { // new format, usually with 3 sumE values
    sumE.assign (it, it+l1);  it += l1;
  } else { // decode all old formats (used between July 2010 and June 2011)
           // sumE vector is not saved, only Et might be present

    if (old_format_print) {
      old_format_print=false;
      log << MSG::INFO << "Old format detected in TileL2Cnv_p2::persToTrans"
          << " l1="<<l1
          << " l2="<<l2
          << " l3="<<l3
          << " l4="<<l4
          << endmsg;
    }

    sumE.resize(1,0.0);

    if (l3>0) {
      sumE[0] = (*iti++)-9000.; // take Et from integer applying offset
      --l3; // and ignore first integer value - Et is not kept in val array in new format
    }
    
    if (l1>0) { // old format with Et and phi saved as two floats
      sumE[0] = (*it++); // use Et value from float array directly
      if (l1>1) ++it; // ignore phi value
    }
  }

  transObj->setEt (std::move(sumE));

  // Muon eta coordinate
  std::vector<float> eta (it, it+l2);  it += l2;
  // Energy in the innermost layer
  std::vector<float> enemu0 (it, it+l2);  it += l2;
  // Energy in the central layer
  std::vector<float> enemu1 (it, it+l2);  it += l2;
  // Energy in the outermost layer
  std::vector<float> enemu2 (it, it+l2);  it += l2;
  
  // Encoded 32-bit words
  std::vector<unsigned int> val (iti, iti+l3);  iti += l3;

  // Quality factor
  std::vector<unsigned int> qual (iti, iti+l4);  iti += l4;

  transObj->setMu (std::move(eta),
                   std::move(enemu0),
                   std::move(enemu1),
                   std::move(enemu2),
                   std::move(qual),
                   std::move(val));

#ifdef TILEL2_DEBUG
  transObj->print();
  std::cout << "pers TileL2 0x"<< std::hex << persObj->m_ID << "  " << std::dec;
  for (unsigned int i=0; i<persObj->m_fval.size(); ++i) 
      std::cout << persObj->m_fval[i] << " ";
  for (unsigned int i=0; i<persObj->m_ival.size(); ++i) 
      std::cout << " " << persObj->m_ival[i];
  std::cout << std::endl;
#endif
}

