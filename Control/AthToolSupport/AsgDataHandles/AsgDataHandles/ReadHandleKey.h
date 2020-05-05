// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/ReadHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Property holding a SG store/key/clid from which a ReadHandle is made.
 */


#ifndef ASG_DATA_HANDLES_READ_HANDLE_KEY_H
#define ASG_DATA_HANDLES_READ_HANDLE_KEY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/ReadHandle.h>
#else

#include "AsgDataHandles/VarHandleKey.h"
#include "AsgDataHandles/exceptions.h"
#include "AsgDataHandles/common.h"


namespace SG {


/**
 * @brief Property holding a SG store/key/clid from which a ReadHandle is made.
 *
 * This class holds the key part of a ReadHandle.  For a reentrant algorithm,
 * you would use this as the algorithm property and construct the actual
 * ReadHandle on the stack from this key object (and optionally the event
 * context).
 *
 * See VarHandleKey for more details.
 */
template <class T>
class ReadHandleKey
  : public VarHandleKey
{
public:
  /**
   * @brief Constructor.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  ReadHandleKey (const std::string& key = "");
                 // const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief Auto-declaring Property constructor.
   * @param owner Owning component.
   * @param name Name of the Property.
   * @param key  Default StoreGate key for the object.
   * @param doc Documentation string.
   *
   * Will associate the named Property with this RHK via declareProperty.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  template <class OWNER, class K>
  ReadHandleKey( OWNER* owner,
                 const std::string& name,
                 const K& key = {},
                 const std::string& doc = "");

  
//   /**
//    * @brief Change the key of the object to which we're referring.
//    * @param sgkey The StoreGate key for the object.
//    * 
//    * The provided key may actually start with the name of the store,
//    * separated by a "+":  "MyStore+Obj".  If no "+" is present,
//    * the store is not changed.
//    */
//   ReadHandleKey& operator= (const std::string& sgkey);


// protected:
//   /**
//    * @brief Constructor with explicit CLID.
//    * @param clid The CLID for the referenced object.
//    * @param key The StoreGate key for the object.
//    * @param storeName Name to use for the store, if it's not encoded in sgkey.
//    *
//    * This is meant to be used by @c ReadDecorHandleKey, to allow fixing the
//    * CLID to a base class to avoid scheduler issues.
//    */
//   ReadHandleKey (CLID clid,
//                  const std::string& key,
//                  const std::string& storeName);


//   /**
//    * @brief Auto-declaring constructor with explicit CLID.
//    * @param clid The CLID for the referenced object.
//    * @param owner Owning component.
//    * @param name name of the Property
//    * @param key The StoreGate key for the object.
//    * @param doc Documentation string.
//    *
//    * This is meant to be used by @c ReadDecorHandleKey, to allow fixing the
//    * CLID to a base class to avoid scheduler issues.
//    */
//   template <class OWNER, class K,
//             typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
//   ReadHandleKey( CLID clid,
//                  OWNER* owner,
//                  const std::string& name,
//                  const K& key,
//                  const std::string& doc);
};


// /**
//  * @brief @c ReadHandleKey that initializes during construction.
//  *
//  * This is a variant of @c ReadHandleKey that calls @c initialize
//  * during construction.  Do _not_ use this for handle keys
//  * that are used as properties.  This is intended for keys
//  * embedded in code and declared const.
//  */
// template <class T>
// class InitializedReadHandleKey
//   : public ReadHandleKey<T>
// {
// public:
//   /**
//    * @brief Constructor.
//    * @param key The StoreGate key for the object.
//    * @param storeName Name to use for the store, if it's not encoded in sgkey.
//    *
//    * The provided key may actually start with the name of the store,
//    * separated by a "+":  "MyStore+Obj".  If no "+" is present
//    * the store named by @c storeName is used.
//    */
//   InitializedReadHandleKey (const std::string& key = "",
//                             const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));
// };


} // namespace SG


#include "AsgDataHandles/ReadHandleKey.icc"

#endif

#endif // not STOREGATE_READHANDLEKEY_H
