/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1TGC/TGCRPhiCoincidenceOut.h"
#include <iostream>
#include <assert.h>

namespace LVL1TGCTrigger {

TGCRPhiCoincidenceOut::TGCRPhiCoincidenceOut():
  m_idSSC(-1),
  m_pT(0),
  m_phi(-1),
  m_r(-1),
  m_dR(0),
  m_dPhi(0),
  m_innerVeto(false),
  m_charge(0),
  m_coincidenceTypeFlag(false),
  m_goodMFFlag(false),
  m_innerCoincidenceFlag(false)
{}

void TGCRPhiCoincidenceOut::clear()
{
  m_idSSC = -1;
  m_phi = -1;
  m_r = -1;
  m_innerVeto = false;
}

bool TGCRPhiCoincidenceOut::isSuperior(const TGCRPhiCoincidenceOut* right) const {
  // selection rule  1. higher pT   2. smaller eta  3. smaller m_phi
  if (this->getpT() > right->getpT()) {
    return true;
  } else if (this->getpT() < right->getpT()) {
    return false;
  } else {
    if (this->getR() < right->getR()) {
      return true;
    } else if (this->getR() > right->getR()) {
      return false;
    } else {
      if (this->getPhi() <= right->getPhi()) {
        return true;
      } else {
        return false;
      }
    }
  }

  return true;
}

void TGCRPhiCoincidenceOut::print() const
{
#ifdef TGCCOUT
  std::cout <<"TGCRPhiCoincidenceOut::print()" << std::endl;
  std::cout <<" phi= " << m_phi << " r= " << m_r << std::endl;
  std::cout <<" Pt= " << m_pT << std::endl;

  std::cout <<" Veto= ";
  if (m_innerVeto) std::cout<<" 1";
  else           std::cout<<" 0";
  std::cout<<std::endl;

#endif
}


} //end of namespace bracket
