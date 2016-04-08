/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_LUMIMETAHANDLER
#define UTILITIES_COLLECTION_LUMIMETAHANDLER

#include "CollectionUtilities/ICollMetaHandler.h"

////////////////////////////////////////////////
//
// LumiMetaHandler
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
//     BUT is specialized to handle GRL's
//
////////////////////////////////////////////////

class LumiMetaHandler : public pool::ICollMetaHandler
{
public:

  LumiMetaHandler(std::string name) : m_name(name) {}
  virtual ~LumiMetaHandler() {}

  std::string name() {return m_name;}

  void process(const pool::ICollMetaHandler::CopyMode&  mode, 
                     pool::ICollMetaHandler::MetaValue& value, 
                     pool::ICollMetaHandler::CollectionMetadata& out, 
               const pool::ICollMetaHandler::ProvNode&  srcName, 
                     pool::ICollMetaHandler::MetaKey&   key);
  void Merge  (pool::ICollMetaHandler::MetaValue& value, 
               pool::ICollMetaHandler::MetaValue& out);
  void Extract(pool::ICollMetaHandler::MetaValue& value, 
               pool::ICollMetaHandler::MetaValue& out);
  void Copy   (pool::ICollMetaHandler::MetaValue& value, 
               pool::ICollMetaHandler::MetaValue& out);

private:
  std::string m_name;
};



#endif
