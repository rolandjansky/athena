/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GlobalUniqueKey.h"

#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include <iostream>

using namespace std;
using namespace pool;

void GlobalUniqueKey::Merge (ICollMetaHandler::MetaValue& value,
                             ICollMetaHandler::MetaKey&   key,
                             ICollMetaHandler::CollectionMetadata& md)
{
   // First check what is already there (funky increment due to using erase)
   for (ICollMetaHandler::CollectionMetadata::iterator it = md.begin();
        it != md.end();) {
      std::string mdkey = it->first;
      // Check if there are any keys which contain the new key but are not the new key
      // --> eliminate them to ensure global unique key
      if ( mdkey.find(key) != std::string::npos && mdkey != key) {
         if (it->second != value) 
            std::cerr << "Found inconsistent values (" 
                      << value << "," << it->second << ") for key " << key;
         md.erase(it++);
      }
      else it++;
   }

   // Now deal with case already make unique

   // if not there, put it in
   if (md.find(key) == md.end()) {
      md.insert(std::make_pair(key,value));
   }
   // if already there, then check that it's the same value
   else if (md[key] != value) {
      std::cerr << "Found inconsistent values ("
                << value << "," << md[key] << ") for key " << key;
   }

}
