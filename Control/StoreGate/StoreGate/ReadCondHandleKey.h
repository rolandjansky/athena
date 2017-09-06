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


  /**
   * @brief auto-declaring Property Constructor.
   * @param name name of the Property
   * @param key The StoreGate key for the object
   * @param doc documentation string
   *
   * will associate the named Property with this RHK via declareProperty
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.   
   */
  template <class OWNER, class K,
            typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
  inline ReadCondHandleKey( OWNER* owner,
                            std::string name,
                            const K& key={},
                            std::string doc="") :
    ReadCondHandleKey<T>( key ) {
    auto p = owner->declareProperty(std::move(name), *this, std::move(doc));
    p->template setOwnerType<OWNER>();
  }

};


} // namespace SG

#endif // not STOREGATE_READCONDHANDLEKEY_H
