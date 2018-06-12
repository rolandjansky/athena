/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// $Id: RecJetEtRoI.cxx 187728 2009-05-27 16:18:06Z krasznaa $
/***************************************************************************
                         RecJetEtRoI.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
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
