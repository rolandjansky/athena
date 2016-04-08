/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_GLOBALUNIQUEKEY
#define UTILITIES_COLLECTION_GLOBALUNIQUEKEY

#include "CollectionUtilities/ICollMetaHandler.h"

////////////////////////////////////////////////
//
// GlobalUniqueKey
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
//     this corresponds to (2)(a)
//
////////////////////////////////////////////////

class GlobalUniqueKey : public pool::ICollMetaHandler
{
public:

  GlobalUniqueKey(std::string name) : m_name(name) {}
  virtual ~GlobalUniqueKey() {}

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
void GlobalUniqueKey::process(const pool::ICollMetaHandler::CopyMode&,
                              pool::ICollMetaHandler::MetaValue& value,
                              pool::ICollMetaHandler::CollectionMetadata& out,
                              const ProvNode& /*prefix*/,
                              MetaKey& key)
{
   //std::cerr << "GlobalUniqueKey: called for " <<prefix <<"/"<<key << std::endl;
   this->Merge  (value,key,out);
/*
   if      (mode==ICollMetaHandler::merge)   this->Merge  (value,out);
   else if (mode==ICollMetaHandler::extract) this->Extract(value,out);
   else if (mode==ICollMetaHandler::copy)    this->Copy   (value,out);
   else {
      std::cerr << "ERROR, INVALID COPY MODE" << mode << std::endl;
   }
*/
}

#endif
