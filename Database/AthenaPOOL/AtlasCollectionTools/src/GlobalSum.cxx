/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GlobalSum.h"

#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;
using namespace pool;

void GlobalSum::Merge (ICollMetaHandler::MetaValue& value,
                             ICollMetaHandler::MetaKey&   key,
                             ICollMetaHandler::CollectionMetadata& md)
{
   // if not there, put it in
   if (md.find(key) == md.end()) {
      //std::cout << "Adding " << key << " with " << value << std::endl;
      md.insert(std::make_pair(key,value));
      //std::cout << "Added " << key << " with " << md[key] << std::endl;
   }
   // if already there, then add value
   else {
      //std::cout << "Found " << key << " with " << md[key] << std::endl;
      int sum = atoi(md[key].c_str()) + atoi(value.c_str());
      std::stringstream sumstring; sumstring << sum;
      md[key] = sumstring.str();
      //std::cout << "Now has " << key << " with " << md[key] << std::endl;
   }

}
