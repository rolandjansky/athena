/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CollectionBaseNames.h"


std::string
pool::CollectionBaseNames::tokenTypeName()
{
  return "Token";
}


std::string
pool::CollectionBaseNames::defaultEventReferenceColumnName()
{
   //return "EVENT_REFERENCE";
   return "Token";  // MN - changed to be consistent with RootCollections and retain backward compatibility at the same time
}


/// name of the key that holds CollectionID in the metadata
std::string
pool::CollectionBaseNames::CollIDMdataKey()
{
   return  "POOLCollectionID";
}
