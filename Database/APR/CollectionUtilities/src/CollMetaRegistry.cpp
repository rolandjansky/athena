/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/CollMetaRegistry.h"
#include "CollectionUtilities/TagMetadataKey.h"

using namespace pool;

CollMetaRegistry* CollMetaRegistry::m_instance = 0;

CollMetaRegistry* CollMetaRegistry::Instance()
{
   if (m_instance == 0) {
      m_instance = new CollMetaRegistry();
   }
   return m_instance;
}

CollMetaRegistry::Container::const_iterator CollMetaRegistry::seek(std::string input)
{
   TagMetadataKey tkey;
   if( tkey.fromString(input) ) {
      return this->find(tkey.key());
   } else {
      return this->find(input);
   }
}

