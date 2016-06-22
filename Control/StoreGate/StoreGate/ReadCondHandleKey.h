/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_READCONDHANDLEKEY_H
#define STOREGATE_READCONDHANDLEKEY_H

#include "StoreGate/CondHandleKey.h"
#include <string>


namespace SG {

  template <class T>
  class ReadCondHandle;

  template <class T>
  class ReadCondHandleKey
    : public CondHandleKey<T>
  {      
  public:
    
    friend class ReadCondHandle<T>;
        
    ReadCondHandleKey (const std::string& key, const std::string& dbKey="") :
      CondHandleKey<T>(key, dbKey, Gaudi::DataHandle::Reader)
    {}    

};


} // namespace SG

#endif // not STOREGATE_READCONDHANDLEKEY_H
