/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         EmTauCTP.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/


#include "TrigT1Interfaces/EmTauCTP.h"

namespace LVL1 {

  EmTauCTP::EmTauCTP( unsigned int cableword0, unsigned int cableword1,
                      unsigned int cableword2, unsigned int cableword3)
    : m_cableWord0( cableword0 ), m_cableWord1( cableword1 ),
      m_cableWord2( cableword2 ), m_cableWord3( cableword3 )
  {

  }

  EmTauCTP::~EmTauCTP() {

  }

  unsigned int EmTauCTP::cableWord0() const {
    return m_cableWord0;
  }

  unsigned int EmTauCTP::cableWord1() const {
    return m_cableWord1;
  }
  
  unsigned int EmTauCTP::cableWord2() const {
    return m_cableWord2;
  }

  unsigned int EmTauCTP::cableWord3() const {
    return m_cableWord3;
  }

} // namespace LVL1
