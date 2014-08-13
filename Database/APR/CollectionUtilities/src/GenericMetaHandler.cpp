/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/GenericMetaHandler.h"
#include <iostream>

using namespace std;

void pool::GenericMetaHandler::process(const pool::ICollMetaHandler::CopyMode& mode, 
                                       MetaValue& value,
                                       pool::ICollMetaHandler::CollectionMetadata& out, 
                                 const ProvNode& prefix, 
                                       MetaKey& key)
{
   std::string outval;
   pool::ICollMetaHandler::CollectionMetadata::const_iterator mit = out.find(key);
   if (mit!=out.end()) outval=mit->second;
   if      (mode==ICollMetaHandler::merge)   this->Merge  (key,value,outval,prefix);
   else if (mode==ICollMetaHandler::extract) this->Extract(key,value,outval,prefix);
   else if (mode==ICollMetaHandler::copy)    this->Copy   (value,outval);
   else {
      std::cerr << "ERROR, INVALID COPY MODE" << mode << std::endl;
   }
   if (mit!=out.end()) out[key] = outval;
   else out.insert(std::make_pair(key,outval));
}


void pool::GenericMetaHandler::Merge  (MetaKey& key, MetaValue& value, MetaValue& out, const ProvNode& prefix)
{
   // for merge always prepend provnode to key even for self
   key = prefix + "|" + key;
   out = value;
}

void pool::GenericMetaHandler::Extract(MetaKey& key, MetaValue& value, MetaValue& out, const ProvNode& prefix)
{
   // for extract prepend provnode to key if not SELF
   if (prefix != "SELF") key = prefix + "|" + key;
   out = value;
}

void pool::GenericMetaHandler::Copy   (MetaValue& value, MetaValue& out)
{
   // for copy leave values unchanged
   out = value;
}
