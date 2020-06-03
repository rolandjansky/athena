/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         NimCTP.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : eleanor.dobson@cern.ch
***************************************************************************/


#include "TrigT1Interfaces/NimCTP.h"

namespace LVL1 {

  NimCTP::NimCTP( unsigned int cableword0, unsigned int cableword1, unsigned int cableword2 )
    : m_cableWord0( cableword0 ), m_cableWord1( cableword1 ), m_cableWord2( cableword2 ) {

  }

  NimCTP::~NimCTP() {

  }

  unsigned int NimCTP::cableWord0() const {
    return m_cableWord0;
  }

  unsigned int NimCTP::cableWord1() const {
    return m_cableWord1;
  }

  unsigned int NimCTP::cableWord2() const {
    return m_cableWord2;
  }

  void NimCTP::SetCableWord0(unsigned int  cableWord0){
    m_cableWord0=cableWord0;
  }

  void NimCTP::SetCableWord1(unsigned int  cableWord1){
    m_cableWord1=cableWord1;
  }

  void NimCTP::SetCableWord2(unsigned int  cableWord2){
    m_cableWord2=cableWord2;
  }


} // namespace LVL1
