// $Id: RecJetEtRoI.cxx 187728 2009-05-27 16:18:06Z krasznaa $
/***************************************************************************
                         RecJetEtRoI.cxx  -  description
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

// Local include(s):
#include "TrigT1Interfaces/RecJetEtRoI.h"
#include "TrigT1Interfaces/JetEtRoIDecoder.h"

namespace LVL1 {

   RecJetEtRoI::RecJetEtRoI( unsigned int RoIWord ) {

      this->construct( RoIWord );

   }

   RecJetEtRoI::~RecJetEtRoI() {

   }

   void RecJetEtRoI::construct( unsigned int RoIWord ) {

      m_roiWord = RoIWord;
      m_energy = 0;

      JetEtRoIDecoder get;
      m_energy = get.energy( m_roiWord );

      return;

   }

   unsigned int RecJetEtRoI::roiWord() const {

      return m_roiWord;

   }

} // namespace LVL1
