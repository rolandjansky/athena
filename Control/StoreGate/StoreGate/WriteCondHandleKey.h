/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_WRITECONDHANDLEKEY_H
#define STOREGATE_WRITECONDHANDLEKEY_H


#include "StoreGate/CondHandleKey.h"


namespace SG {

  template <class T>
  class WriteCondHandle;

  template <class T>
  class WriteCondHandleKey 
    : public CondHandleKey<T>
  {
    
    friend class WriteCondHandle<T>;
    
  public:
    WriteCondHandleKey(const std::string& key, const std::string& dbKey) :
      CondHandleKey<T>(key, dbKey, Gaudi::DataHandle::Writer)
    {}

  };

} // namespace SG

#endif
