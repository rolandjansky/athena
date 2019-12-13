///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceiverObjCnv_p1.cxx 
// Implementation file for class TileMuonReceiverObjCnv_p1
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 

// TileEvent includes
#include "TileEvent/TileMuonReceiverObj.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuonReceiverObjCnv_p1.h"

//#define DEBUG_OUTPUT 1

void TileMuonReceiverObjCnv_p1::transToPers(const TileMuonReceiverObj* transObj, TileMuonReceiverObj_p1* persObj, MsgStream &/*log*/) const {

  // module id
  persObj->m_id = std::max(std::min(transObj->identify(),0xFFF),0);

  if (persObj->m_id > 0 && persObj->m_id < 0xFFF) { // standard object with decision

    size_t maxsize=8; // keep only 8 decision bits maximum

    // decision 
    if (transObj->GetDecision().size()>0) {
      persObj->m_id |= std::min(transObj->GetDecision().size(),maxsize) << 20;
      unsigned int bit=0x1000;
      for (std::vector<bool>::const_iterator it = transObj->GetDecision().begin(); it != transObj->GetDecision().end(); ++it) {
        if (*it) {
          persObj->m_id |= bit;
        }
        bit <<= 1;
        if (bit >= 0x100000) break; // keep only 8 bits maximum
      }
    }

    maxsize=15; // keep only 15 vords from every vector maximum

    // energy 
    if (!transObj->GetEne().empty()) {
      persObj->m_id |= std::min(transObj->GetEne().size(),maxsize) << 24;
      size_t cnt=0;
      for (float f : transObj->GetEne()) {
        persObj->m_data.push_back(f);
        if ((++cnt)>=maxsize) break;
      }
    }

    // time 
    if (!transObj->GetTime().empty()) {
      persObj->m_id |= std::min(transObj->GetTime().size(),maxsize) << 28;
      size_t cnt=0;
      for (float f : transObj->GetTime()) {
        persObj->m_data.push_back(f);
        if ((++cnt)>=maxsize) break;
      }
    }

  } else { // non-standard object with thresholds

    size_t maxsize=255; // upper limit for word counter (but keep all threshold words anyway)

    // thresholds
    if ( transObj->identify()==0 && !transObj->GetThresholds().empty()) {
      persObj->m_id |= std::min(transObj->GetThresholds().size(),maxsize) << 24;
      persObj->m_data.insert (persObj->m_data.end(),
                              transObj->GetThresholds().begin(),
                              transObj->GetThresholds().end());
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


void TileMuonReceiverObjCnv_p1::persToTrans(const TileMuonReceiverObj_p1* persObj, TileMuonReceiverObj* transObj, MsgStream &/*log*/) const {

  unsigned int id=persObj->m_id;
  int trans_id = id & 0xFFF;

  size_t n_elements = persObj->m_data.size();
  std::vector<float>::const_iterator it = persObj->m_data.begin(); 

  std::vector<bool> decision;
  std::vector<float> ene;
  std::vector<float> time;
  if (trans_id > 0 && trans_id < 0xFFF) { // standard object with decision
      
    size_t n0 = (id >> 20) & 0xF;
    size_t n1 = (id >> 24) & 0xF;
    size_t n2 = (id >> 28) & 0xF;
  
    if (n0) {
      id >>= 12;
      decision.reserve(n0); 
      for (size_t i=0; i<n0; ++i) {
        decision.push_back((bool) (id & 1) );
        id >>= 1;
      }
    }

    // energy
    if (n1>n_elements) n1=n_elements;
    ene.assign (it, it+n1);
    it += n1;
    n_elements -= n1;
  
    // time
    if (n2>n_elements) n2=n_elements;
    time.assign (it, it+n2);
    it += n2;
    n_elements -= n2;
  }

  // remaining elements go to thresholds
  std::vector<float> thresholds (it, persObj->m_data.end());

  *transObj = TileMuonReceiverObj (trans_id,
                                   std::move(ene),
                                   std::move(time),
                                   std::move(decision),
                                   std::move(thresholds));

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
