/***************************************************************************
                         EnergyCTP.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   copyright            : (C) 2001 by moyse
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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
