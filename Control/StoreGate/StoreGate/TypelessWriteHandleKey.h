// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/TypelessWriteHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Property holding a write key for which the type can be configured.
 */


#ifndef STOREGATE_TYPELESSWRITEHANDLEKEY_H
#define STOREGATE_TYPELESSWRITEHANDLEKEY_H


#include "StoreGate/VarHandleKey.h"


namespace SG {


/**
 * @brief Property holding a write key for which the type can be configured.
 *
 * This class plays the same role as @c WriteHandleKey, except that the
 * type of the handle may be changed during configuration.  The value
 * assigned to this property may be of the form STORE+CLASS/KEY, where
 * the STORE+ and CLASS/ parts are optional.
 *
 * See VarHandleKey for more details.
 */
class TypelessWriteHandleKey
  : public VarHandleKey
{
public:
  /**
   * @brief Constructor.
   * @param clid Type of this key.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  TypelessWriteHandleKey (CLID clid = CLID_NULL,
                          const std::string& key = "",
                          const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief auto-declaring Property constructor.
   * @param owner Owning component.
   * @param name name of the Property
   * @param clid Type of this key.
   * @param key  Default StoreGate key for the object.
   * @param doc Documentation string.
   *
   * Will associate the named Property with this object via declareProperty.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  template <class OWNER, class K,
            typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
  inline
  TypelessWriteHandleKey( OWNER* owner,
                          const std::string& name,
                          CLID clid = CLID_NULL,
                          const K& key={},
                          const std::string& doc="")
    : TypelessWriteHandleKey(clid, key)
  {
    auto p = owner->declareProperty(name, *this, doc);
    p->template setOwnerType<OWNER>();
  }


  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The key may have the full form STORE+CLASS/NAME, where the STORE+
   * and CLASS/ parts are optional.
   */
  TypelessWriteHandleKey& operator= (const std::string& sgkey);


  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The key may have the full form STORE+CLASS/NAME, where the STORE+
   * and CLASS/ parts are optional.
   *
   * Returns failure the key string format is bad.
   */
  virtual StatusCode assign (const std::string& key) override;
};


} //namespace SG


#endif // not STOREGATE_TYPELESSWRITEHANDLEKEY_H
