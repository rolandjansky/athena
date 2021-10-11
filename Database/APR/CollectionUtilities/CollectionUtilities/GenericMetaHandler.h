/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_LUMIMETAHANDLER
#define UTILITIES_COLLECTION_LUMIMETAHANDLER

#include "CollectionUtilities/ICollMetaHandler.h"

namespace pool {
   
   class GenericMetaHandler : public pool::ICollMetaHandler
   {
  public:

     GenericMetaHandler(const std::string& name="Generic") : m_name(name) {}
     virtual ~GenericMetaHandler() {}

     std::string name() {return m_name;}
     bool exactMatch()  {return true;}

     void process(const CopyMode&  mode,
                  MetaValue& value,
                  pool::ICollMetaHandler::CollectionMetadata& out,
                  const ProvNode&  prefix,
                  MetaKey&   key);
     void Merge  (MetaKey& key, MetaValue& value, MetaValue& out, const ProvNode& srcName);
     void Extract(MetaKey& key, MetaValue& value, MetaValue& out, const ProvNode& srcName);
     void Copy   (MetaValue& value, MetaValue& out);

  private:
     std::string m_name;
   };

}
#endif
