/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetCollectionHelper.cxx 588111 2014-03-18 09:02:18Z krasznaa $

// EDM include(s):
#include "JetEvent/JetCollection.h"

// Local include(s):
#include "JetCollectionHelper.h"

StatusCode JetCollectionHelper::swapMomentMap( JetCollection* jets ) {

   if( jets->m_ownMap && jets->m_momentMapLink.isValid() ) {
      delete jets->m_momentMapLink.cptr();
   }

   jets->recordedInSG( jets->author() );
 
   return StatusCode::SUCCESS;
}

void JetCollectionHelper::resetJetId( JetCollection* jets, std::size_t dist ) {

   jets->resetJetId( dist );
   return;
}
