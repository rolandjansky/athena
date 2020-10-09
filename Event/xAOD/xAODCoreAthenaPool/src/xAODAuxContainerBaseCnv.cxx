/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODAuxContainerBaseCnv.h"

xAOD::AuxContainerBase*
xAODAuxContainerBaseCnv::createPersistentWithKey( xAOD::AuxContainerBase* trans,
                                                  const std::string& key )
{
   xAOD::AuxContainerBase* result =
     xAODAuxContainerBaseCnvBase::createPersistentWithKey( trans, key );

   // Return the new object:
   return result;
}

