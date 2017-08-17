/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READMETAHANDLEKEY_H
#define STOREGATE_READMETAHANDLEKEY_H

#include "StoreGate/MetaHandleKey.h"
#include <string>


namespace SG {

  template <class T>
  class ReadMetaHandle;

  template <class T>
  class ReadMetaHandleKey
    : public MetaHandleKey<T>
  {      
  public:
    
    friend class ReadMetaHandle<T>;
        
    ReadMetaHandleKey (const std::string& key, const std::string& dbKey="") :
      MetaHandleKey<T>(key, dbKey, Gaudi::DataHandle::Reader)
    {}    

};


} // namespace SG

#endif // not STOREGATE_READMETAHANDLEKEY_H
