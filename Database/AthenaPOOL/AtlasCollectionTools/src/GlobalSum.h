/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_GLOBALSUM
#define UTILITIES_COLLECTION_GLOBALSUM

#include "CollectionUtilities/ICollMetaHandler.h"

////////////////////////////////////////////////
//
// GlobalSum
//   - created to handle luminosity, of the three possible metadata handlings
//     (1) Provenance tree (all metadata retained)
//     (2) Provenance stripped
//         (a) new data for same key+qualifier omitted
//         (b) new data for same key+qualifier merged into existing
//         (c) new data for same key+qualifier has new qualifier with version
//     (3) Provenance and qualifier stripped
//         (a) new data for same key omitted
//         (b) new data for same key merged into existing
//         (c) new data for same key has new qualifier with version
//     this corresponds to (3)(b)
//
////////////////////////////////////////////////

class GlobalSum : public pool::ICollMetaHandler
{
public:

  GlobalSum(std::string name) : m_name(name) {}
  virtual ~GlobalSum() {}

  std::string name() {return m_name;}

  void process(const pool::ICollMetaHandler::CopyMode& , 
                     pool::ICollMetaHandler::MetaValue& value, 
                     pool::ICollMetaHandler::CollectionMetadata& out, 
               const pool::ICollMetaHandler::ProvNode&  srcName, 
                     pool::ICollMetaHandler::MetaKey&   key);
  void Merge  (pool::ICollMetaHandler::MetaValue& value, 
               pool::ICollMetaHandler::MetaKey&   key,
               pool::ICollMetaHandler::CollectionMetadata& out);

private:
  std::string m_name;
};

inline
void GlobalSum::process(const pool::ICollMetaHandler::CopyMode&,
                        pool::ICollMetaHandler::MetaValue& value,
                        pool::ICollMetaHandler::CollectionMetadata& out,
                        const ProvNode& /*prefix*/,
                        MetaKey& key)
{
   //std::cerr << "GlobalSum: called for " <<prefix <<"/"<<key << std::endl;
   this->Merge  (value,key,out);
}

#endif
