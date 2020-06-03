///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2Cnv_p2.cxx 
// Implementation file for class TileL2Cnv_p2
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
#include "TileTPCnv/TileL2Cnv_p2.h"

// #define TILEL2_DEBUG true

#ifdef TILEL2_DEBUG
#include "iostream"
#endif

static bool old_format_print=true;

void TileL2Cnv_p2::transToPers(const TileL2* transObj, TileL2_p2* persObj, MsgStream &log) {

  unsigned int l1 = transObj->m_sumE.size();
  unsigned int l2 = transObj->m_eta.size();
  unsigned int l3 = transObj->m_val.size();
  unsigned int l4 = transObj->m_quality_factor.size();
  
  if (l1>15 || l2>15 || l3>15 || l4>15) {
      log << MSG::ERROR << "TOO BIG SIZE IN TileL2Cnv_p2::transToPers !!! " 
          << " l1=" << l1
          << " l2=" << l2
          << " l3=" << l3
          << " l4=" << l4
          << endmsg;
      persObj->m_ID = (transObj->m_ID & 0xFFFF);
      return;
  }

  persObj->m_fval.reserve(l1+4*l2);
  persObj->m_ival.reserve(l3+l4);

  // Transverse energy
  if (l1>0) {
    bool allzeros=true;
    for (std::vector<float>::const_iterator it = transObj->m_sumE.begin(); it != transObj->m_sumE.end(); ++it) {
      persObj->m_fval.push_back( *it );
      if (fabs(*it) > 1.e-6) allzeros=false;
    }
    if (allzeros) {
      l1=0;
      persObj->m_fval.clear();
    }
  }

  // Drawer ID
  persObj->m_ID = (l1<<28) | (l2<<24) | (l3<<20) | (l4<<16) | (transObj->m_ID & 0xFFFF);

  if (l2) {
    // Muon eta coordinate
    for (std::vector<float>::const_iterator it = transObj->m_eta.begin(); it != transObj->m_eta.end(); ++it) {
        persObj->m_fval.push_back( *it );
    }

    // Energy in the innermost layer
    for (std::vector<float>::const_iterator it = transObj->m_enemu0.begin(); it != transObj->m_enemu0.end(); ++it) {
        persObj->m_fval.push_back( *it );
    }

    // Energy in the central layer
    for (std::vector<float>::const_iterator it = transObj->m_enemu1.begin(); it != transObj->m_enemu1.end(); ++it) {
        persObj->m_fval.push_back( *it );
    }

    // Energy in the outermost layer
    for (std::vector<float>::const_iterator it = transObj->m_enemu2.begin(); it != transObj->m_enemu2.end(); ++it) {
        persObj->m_fval.push_back( *it );
    }

    if (l1 + 4*l2 != persObj->m_fval.size()) {
      log << MSG::ERROR << "WRONG SIZE IN TileL2Cnv_p2::transToPers !!! " 
          << " l1=" << l1
          << " l2=" << l2
          << "(" << transObj->m_eta.size()
          << "," << transObj->m_enemu0.size()
          << "," << transObj->m_enemu1.size()
          << "," << transObj->m_enemu2.size()
          << ")  f_size=" << persObj->m_fval.size()
          << endmsg;
    }
  }
  
  // Encoded 32-bit words
  for (std::vector<unsigned int>::const_iterator it = transObj->m_val.begin(); it != transObj->m_val.end(); ++it) {
      persObj->m_ival.push_back( *it );
  }

  // Quality factor
  for (std::vector<unsigned int>::const_iterator it = transObj->m_quality_factor.begin(); it != transObj->m_quality_factor.end(); ++it) {
    persObj->m_ival.push_back( *it );
  }

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


void TileL2Cnv_p2::persToTrans(const TileL2_p2* persObj, TileL2* transObj, MsgStream &log) {

  unsigned int l0 = persObj->m_ID;

  // Drawer ID
  transObj->m_ID = l0 & 0xFFFF;
  transObj->setphi();

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
  
  if (l1>2 || l3 == 2*l2 ) { // new format, usually with 3 sumE values
    transObj->m_sumE.reserve(l1);
    for (unsigned int i=0; i<l1; ++i) {
      transObj->m_sumE.push_back( (*it++) );
    }
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

    transObj->m_sumE.resize(1,0.0);

    if (l3>0) {
      transObj->m_sumE[0] = (*iti++)-9000.; // take Et from integer applying offset
      --l3; // and ignore first integer value - Et is not kept in val array in new format
    }
    
    if (l1>0) { // old format with Et and phi saved as two floats
      transObj->m_sumE[0] = (*it++); // use Et value from float array directly
      if (l1>1) ++it; // ignore phi value
    }
  }

  if (l2) {
    // Muon eta coordinate
    transObj->m_eta.reserve(l2);
    for (unsigned int i=0; i<l2; ++i) {
      transObj->m_eta.push_back( (*it++) );
    }

    // Energy in the innermost layer
    transObj->m_enemu0.reserve(l2);
    for (unsigned int i=0; i<l2; ++i) {
      transObj->m_enemu0.push_back( (*it++) );
    }

    // Energy in the central layer
    transObj->m_enemu1.reserve(l2);
    for (unsigned int i=0; i<l2; ++i) {
      transObj->m_enemu1.push_back( (*it++) );
    }

    // Energy in the outermost layer
    transObj->m_enemu2.reserve(l2);
    for (unsigned int i=0; i<l2; ++i) {
      transObj->m_enemu2.push_back( (*it++) );
    }
  }
  
  // Encoded 32-bit words
  transObj->m_val.reserve(l3);
  for (unsigned int i=0; i<l3; ++i) {
    transObj->m_val.push_back( (*iti++) );
  }

  // Quality factor
  transObj->m_quality_factor.reserve(l4);
  for (unsigned int i=0; i<l4; ++i) {
    transObj->m_quality_factor.push_back( (*iti++) );
  }

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

