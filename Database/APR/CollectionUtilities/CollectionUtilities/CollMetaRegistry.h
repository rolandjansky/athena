/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_COLLMETAREGISTRY
#define UTILITIES_COLLECTION_COLLMETAREGISTRY

#include <map>
#include <iostream>
#include <memory>
#include "CollectionUtilities/ICollMetaHandler.h"
#include "CxxUtils/checker_macros.h"

namespace pool {

   class ATLAS_NOT_THREAD_SAFE CollMetaRegistry : public std::map<ICollMetaHandler::MetaKey,ICollMetaHandler*>
   {
   public:
      typedef std::map<ICollMetaHandler::MetaKey,ICollMetaHandler*> Container;

      static CollMetaRegistry* Instance();

      Container::const_iterator seek(std::string);

   protected:
      CollMetaRegistry() {}
   private:
      static std::unique_ptr<CollMetaRegistry> m_instance;
   };
}

#endif
