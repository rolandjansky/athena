/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPassBitsContainerCnv.cxx 755768 2016-06-17 13:43:10Z krasznaa $

// System include(s):
#include <memory>

// Local include(s):
#include "xAODTrigPassBitsContainerCnv.h"

xAODTrigPassBitsContainerCnv::
xAODTrigPassBitsContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigPassBitsContainerCnvBase( svcLoc ) {

}

xAOD::TrigPassBitsContainer* xAODTrigPassBitsContainerCnv::createTransient() {

   // Get the object using the base class:
   std::unique_ptr< xAOD::TrigPassBitsContainer >
         ondisk( xAODTrigPassBitsContainerCnvBase::createTransient() );

   // Create a copy of it:
   xAOD::TrigPassBitsContainer* result = new xAOD::TrigPassBitsContainer();
   for( size_t i = 0; i < ondisk->size(); ++i ) {
      result->push_back( new xAOD::TrigPassBits() );
   }
   result->setStore( ondisk->getConstStoreLink() );

   // Now return it:
   return result;
}
