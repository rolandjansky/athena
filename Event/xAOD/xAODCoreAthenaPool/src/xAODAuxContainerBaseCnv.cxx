/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODAuxContainerBaseCnv.h"

xAOD::AuxContainerBase*
xAODAuxContainerBaseCnv::createPersistentWithKey( xAOD::AuxContainerBase* trans,
                                                  const std::string& key )
{
   xAOD::AuxContainerBase* result =
     xAODAuxContainerBaseCnvBase::createPersistentWithKey( trans, key );

   // Make sure that the variable selection gets copied:
   result->m_selection = trans->m_selection;

   // Return the new object:
   return result;
}

