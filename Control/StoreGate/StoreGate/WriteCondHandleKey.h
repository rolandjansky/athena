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

    /**
     * @brief Constructor for WriteCondHandle
     * @key   StoreGate key
     * @dbKey key in the database
     */
    WriteCondHandleKey(const std::string& key, const std::string& dbKey) :
      CondHandleKey<T>(key, dbKey, Gaudi::DataHandle::Writer)
    {}

    /**
     * @brief Constructor for WriteCondHandle that takes just a SG key
     * @key   StoreGate key
     *
     * the value of the dbKey is the same as the StoreGate key
     */
    WriteCondHandleKey(const std::string& key) :
      CondHandleKey<T>(key, key, Gaudi::DataHandle::Writer)
    {}


  /**
   * @brief auto-declaring Property Constructor.
   * @param name name of the Property
   * @param key  default StoreGate key for the object.
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
    inline WriteCondHandleKey( OWNER* owner,
                               std::string name,
                               const K& key={},
                               std::string doc="") :
      WriteCondHandleKey<T>(key) {
      auto p = owner->declareProperty(std::move(name), *this, std::move(doc));
      p->template setOwnerType<OWNER>();
    }
    
  };

} // namespace SG

#endif
