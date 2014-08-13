/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_ICOLLMETAHANDLER
#define UTILITIES_COLLECTION_ICOLLMETAHANDLER

#include  <string>
#include  <map>

// Would like to use DBDataModel/CollectionMetadata for retrieve, 
//   but the DataVector brings in unwanted dependencies

namespace pool {

   class ICollMetaHandler 
   {
   public:
      // All sorts of types
      typedef std::string ProvNode;
      typedef std::string MetaKey; 	// simple string until more needed
      typedef std::string MetaValue; 	// simple string until more needed
      typedef std::map<MetaKey,MetaValue> CollectionMetadata;  // would be nice if this were avail from CBase
      enum CopyMode { copy, extract, merge };

      ICollMetaHandler() {}
      virtual ~ICollMetaHandler() {}

      virtual std::string name() = 0;

      // This should be what is called by the clients, the real interface
      virtual void process(const CopyMode&  mode, 
                                 MetaValue& value, 
                                 CollectionMetadata& out, 
                           const ProvNode&  prefix, 
                                 MetaKey&   key) = 0;
 
   };

}

#endif
