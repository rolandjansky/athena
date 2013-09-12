/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTopoEventTPCnv
 * @Class  : ElectronMuonTopoInfo_p1
 *
 * @brief persistent partner for ElectronMuonTopoInfo
 *
 * @author Olga Igonkina
 *
 **********************************************************************************/
#ifndef TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFO_P1_H
#define TRIGTOPOEVENTTPCNV_ELECTRONMUONTOPOINFO_P1_H


#include "TrigTopoEvent/ElectronMuonTopoInfo.h"


class ElectronMuonTopoInfo_p1 {
  friend class ElectronMuonTopoInfoCnv_p1;
    
 public:
    
  // default constructor
  ElectronMuonTopoInfo_p1():
    m_roiWord(0),
    m_DeltaPhi(0),
    m_DeltaR(0),
    m_InvMass(0),
    m_electronValid(0),
    m_oppositeCharge(0),
    m_vertexState(0)  
  {}

  // default destructor
  ~ElectronMuonTopoInfo_p1() { }

 private:

  int   m_roiWord;
  float m_DeltaPhi;
  float m_DeltaR;
  float m_InvMass;
  bool  m_electronValid;
  bool  m_oppositeCharge;
  unsigned short m_vertexState;
  

};

#endif
