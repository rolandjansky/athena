///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceiverObjCnv_p1.cxx 
// Implementation file for class TileMuonReceiverObjCnv_p1
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#define private public
#define protected public
#include "TileEvent/TileMuonReceiverObj.h"
#undef private
#undef protected

// TileTPCnv includes
#include "TileTPCnv/TileMuonReceiverObjCnv_p1.h"

//#define DEBUG_OUTPUT 1

void TileMuonReceiverObjCnv_p1::transToPers(const TileMuonReceiverObj* transObj, TileMuonReceiverObj_p1* persObj, MsgStream &/*log*/) {

  // module id
  persObj->m_id = std::max(std::min(transObj->m_id,0xFFF),0);

  if (persObj->m_id > 0 && persObj->m_id < 0xFFF) { // standard object with decision

    size_t maxsize=8; // keep only 8 decision bits maximum

    // decision 
    if (transObj->m_TileMuRcvDecision.size()>0) {
      persObj->m_id |= std::min(transObj->m_TileMuRcvDecision.size(),maxsize) << 20;
      unsigned int bit=0x1000;
      for (std::vector<bool>::const_iterator it = transObj->m_TileMuRcvDecision.begin(); it != transObj->m_TileMuRcvDecision.end(); ++it) {
        if (*it) {
          persObj->m_id |= bit;
        }
        bit <<= 1;
        if (bit >= 0x100000) break; // keep only 8 bits maximum
      }
    }

    maxsize=15; // keep only 15 vords from every vector maximum

    // energy 
    if (transObj->m_TileMuRcvEne.size()>0) {
        persObj->m_id |= std::min(transObj->m_TileMuRcvEne.size(),maxsize) << 24;
      size_t cnt=0;
      for (std::vector<float>::const_iterator it = transObj->m_TileMuRcvEne.begin(); it != transObj->m_TileMuRcvEne.end(); ++it) {
        persObj->m_data.push_back(*it);
        if ((++cnt)>=maxsize) break;
      }
    }

    // time 
    if (transObj->m_TileMuRcvTime.size()>0) {
      persObj->m_id |= std::min(transObj->m_TileMuRcvTime.size(),maxsize) << 28;
      size_t cnt=0;
      for (std::vector<float>::const_iterator it = transObj->m_TileMuRcvTime.begin(); it != transObj->m_TileMuRcvTime.end(); ++it) {
        persObj->m_data.push_back(*it);
        if ((++cnt)>=maxsize) break;
      }
    }

  } else { // non-standard object with thresholds

    size_t maxsize=255; // upper limit for word counter (but keep all threshold words anyway)

    // thresholds
    if ( transObj->m_id==0 && transObj->m_TileMuRcvThresholds.size()>0) {
      persObj->m_id |= std::min(transObj->m_TileMuRcvThresholds.size(),maxsize) << 24;
      for (std::vector<float>::const_iterator it = transObj->m_TileMuRcvThresholds.begin(); it != transObj->m_TileMuRcvThresholds.end(); ++it) {
        persObj->m_data.push_back(*it);
      }
    }

  }

#ifdef DEBUG_OUTPUT
  transObj->print();
  std::cout << "Persistent_TMRO " << transObj->m_id << " id " << std::hex << persObj->m_id  
            << " size " << std::dec << ((persObj->m_id>>20)&0xF) << " " << persObj->m_data.size() << "  val ";
  for (unsigned int i=0; i<((persObj->m_id>>20)&0xF); ++i) {
    std::cout << " " << (persObj->m_id>>(12+i) & 1);
  }
  std::cout << " ";
  for (std::vector<float>::const_iterator it = persObj->m_data.begin(); it != persObj->m_data.end(); ++it) {
    std::cout << " " << (*it);
  }
  std::cout << std::endl;
#endif
}


void TileMuonReceiverObjCnv_p1::persToTrans(const TileMuonReceiverObj_p1* persObj, TileMuonReceiverObj* transObj, MsgStream &/*log*/) {

  unsigned int id=persObj->m_id;

  // module id
  transObj->m_id = id & 0xFFF;

  size_t n_elements = persObj->m_data.size();
  std::vector<float>::const_iterator it = persObj->m_data.begin(); 

  if (transObj->m_id > 0 && transObj->m_id < 0xFFF) { // standard object with decision
      
    size_t n0 = (id >> 20) & 0xF;
    size_t n1 = (id >> 24) & 0xF;
    size_t n2 = (id >> 28) & 0xF;
  
    if (n0) {
      id >>= 12;
      transObj->m_TileMuRcvDecision.reserve(n0); 
      for (size_t i=0; i<n0; ++i) {
        transObj->m_TileMuRcvDecision.push_back((bool) (id & 1) );
        id >>= 1;
      }
    }

    // energy
    if (n1) {
      if (n1>n_elements) n1=n_elements;
      transObj->m_TileMuRcvEne.reserve(n1); 
      for (size_t i=0; i<n1; ++i) {
        transObj->m_TileMuRcvEne.push_back(*it);
        ++it;
      }
      n_elements -= n1;
    }
  
    // time
    if (n2) {
      if (n2>n_elements) n2=n_elements;
      transObj->m_TileMuRcvTime.reserve(n2);
      for (size_t i=0; i<n2; ++i) {
        transObj->m_TileMuRcvTime.push_back(*it);
        ++it;
      }
      n_elements -= n2;
    }
    
  }

  // remaining elements go to thresholds
  if (n_elements) {
    transObj->m_TileMuRcvThresholds.reserve(n_elements); 
    for ( ; it != persObj->m_data.end(); ++it) {
      transObj->m_TileMuRcvThresholds.push_back(*it);
    }
  }

#ifdef DEBUG_OUTPUT
  transObj->print();
  std::cout << "Persistent_TMRO " << transObj->m_id << " id " << std::hex << persObj->m_id  
            << " size " << std::dec << ((persObj->m_id>>20)&0xF) << " " << persObj->m_data.size() << "  val ";
  for (unsigned int i=0; i<((persObj->m_id>>20)&0xF); ++i) {
    std::cout << " " << (persObj->m_id>>(12+i) & 1);
  }
  std::cout << " ";
  for (std::vector<float>::const_iterator it = persObj->m_data.begin(); it != persObj->m_data.end(); ++it) {
    std::cout << " " << (*it);
  }
  std::cout << std::endl;
#endif
}
