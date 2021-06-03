/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_COLLMETAREGISTRY
#define UTILITIES_COLLECTION_COLLMETAREGISTRY

#include <map>
#include <iostream>
#include "CollectionUtilities/ICollMetaHandler.h"
#include "CxxUtils/checker_macros.h"

namespace pool {

   class ATLAS_NOT_THREAD_SAFE CollMetaRegistry : public std::map<ICollMetaHandler::MetaKey,ICollMetaHandler*>
   {
   public:
      typedef std::map<ICollMetaHandler::MetaKey,ICollMetaHandler*> Container;

      static CollMetaRegistry* Instance();
      virtual ~CollMetaRegistry();

      Container::const_iterator seek(std::string);

   protected:
      CollMetaRegistry() {}
   private:
      static CollMetaRegistry* m_instance;
   };

   inline CollMetaRegistry::~CollMetaRegistry() 
   {
      delete m_instance;
   }

}

#endif
