/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/CollMetaRegistry.h"
#include "CollectionUtilities/TagMetadataKey.h"

using namespace pool;

std::unique_ptr<CollMetaRegistry> CollMetaRegistry::m_instance = 0;

CollMetaRegistry* CollMetaRegistry::Instance()
{
   if (!m_instance) {
     // Can't use make_unique because ctor is protected.
     m_instance.reset (new CollMetaRegistry);
   }
   return m_instance.get();
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

