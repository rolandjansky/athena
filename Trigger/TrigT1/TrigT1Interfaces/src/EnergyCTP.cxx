/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         EnergyCTP.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/

#include "TrigT1Interfaces/EnergyCTP.h"

namespace LVL1 {

  EnergyCTP::EnergyCTP( unsigned int cableword0, unsigned int cableword1 )
    : m_cableWord0( cableword0 ), m_cableWord1( cableword1 ) {

  }

  EnergyCTP::~EnergyCTP() {

  }

  /** return the data
  |P|8b METSig Map|8b EtMiss Hit Map|8b Et Sum Map|*/
  unsigned int EnergyCTP::cableWord0() const {
    return m_cableWord0;
  }

  /** return the data
  |P|8b EtMiss Hit Map|8b Et Sum Map|*/
  unsigned int EnergyCTP::cableWord1() const {
    return m_cableWord1;
  }

} // namespace LVL1
