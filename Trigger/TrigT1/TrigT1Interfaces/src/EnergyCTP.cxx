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

  EnergyCTP::EnergyCTP( unsigned int cableword0 )
    : m_cableWord0( cableword0 ) {

  }

  EnergyCTP::~EnergyCTP() {

  }

  /** return the data
  |P|4b Et Sum Map|8b EtMiss Hit Map|0|*/
  unsigned int EnergyCTP::cableWord0() const {
    return m_cableWord0;
  }

} // namespace LVL1
