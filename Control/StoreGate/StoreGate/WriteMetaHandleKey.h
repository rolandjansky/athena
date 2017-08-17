/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITEMETAHANDLEKEY_H
#define STOREGATE_WRITEMETAHANDLEKEY_H


#include "StoreGate/MetaHandleKey.h"


namespace SG {

  template <class T>
  class WriteMetaHandle;

  template <class T>
  class WriteMetaHandleKey 
    : public MetaHandleKey<T>
  {
    
    friend class WriteMetaHandle<T>;
    
  public:
    WriteMetaHandleKey(const std::string& key, const std::string& dbKey) :
      MetaHandleKey<T>(key, dbKey, Gaudi::DataHandle::Writer)
    {}

  };

} // namespace SG

#endif
