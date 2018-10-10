/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         JetCTP.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/


#include "TrigT1Interfaces/JetCTP.h"

namespace LVL1 {

  JetCTP::JetCTP( unsigned int cableword0, unsigned int cableword1 )
    : m_cableWord0( cableword0 ), m_cableWord1( cableword1 ) {

  }

  JetCTP::~JetCTP() {

  }

  unsigned int JetCTP::cableWord0() const {
    return m_cableWord0;
  }

  unsigned int JetCTP::cableWord1() const {
    return m_cableWord1;
  }

} // namespace LVL1
