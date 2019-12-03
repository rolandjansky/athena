// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/ThinningHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief HandleKey object for adding thinning to an object.
 */


#ifndef STOREGATE_THINNINGHANDLEKEY_H
#define STOREGATE_THINNINGHANDLEKEY_H

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthContainers/ThinningDecision.h"
#include "GaudiKernel/IDataHandleHolder.h"


namespace SG {


/**
 * @brief HandleKey object for adding thinning to an object.
 *
 * This key object goes with @c ThinningHandle to allow adding thinning for an object.
 * See @c ThinningHandle for more details.
 *
 * Using this key will make a read dependency on the object of type @c T given
 * by the requested key, and a write dependency on the corresponding
 * @c ThinningDecision object.
 */
template <class T>
class ThinningHandleKey
  : public ReadHandleKey<T>
{
public:
  typedef ReadHandleKey<T> Base;

  
  /**
   * @brief Constructor.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  ThinningHandleKey (const std::string& key = "",
                     const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


  ThinningHandleKey (const ThinningHandleKey&) = default;


  /**
   * @brief Auto-declaring Property constructor.
   * @param owner Owning component.
   * @param name Name of the Property.
   * @param key  Default StoreGate key for the object.
   * @param doc Documentation string.
   *
   * Will associate the named Property with this key via declareProperty.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  template <class OWNER, class K,
            typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
  ThinningHandleKey( OWNER* owner,
                     const std::string& name,
                     const K& key,
                     const std::string& doc);


  /**
   * @brief Assignment.
   * @param other Key to assign.
   */
  ThinningHandleKey& operator= (const ThinningHandleKey& other);


  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present,
   * the store is not changed.
   */
  ThinningHandleKey& operator= (const std::string& sgkey);

  
  /**
   * @brief Should be called during the initialize phase.  It will fail
   *        if the requested StoreGate service cannot be found or if the key is blank.
   * @param stream The name of the stream for which thinning is being applied.
   * @param qualifier Qualifier to add on to the key for the decision object.
   *                  If there are multiple algorithms thinning the same object,
   *                  this can be used to keep them from interfering with
   *                  each other.  (The thinning decisions will be merged with AND
   *                  at the end.)
   * @param used If false, then this handle is not to be used.
   *             Instead of normal initialization, the key will be cleared.
   */
  StatusCode initialize (const std::string& stream,
                         const std::string& qualifier,
                         bool used = true);


  /**
   * @brief Should be called during the initialize phase.  It will fail
   *        if the requested StoreGate service cannot be found or if the key is blank.
   * @param stream The name of the stream for which thinning is being applied.
   * @param qualifier Qualifier to add on to the key for the decision object.
   *                  If there are multiple algorithms thinning the same object,
   *                  this can be used to keep them from interfering with
   *                  each other.  (The thinning decisions will be merged with AND
   *                  at the end.)
   * @param used If false, then this handle is not to be used.
   *             Instead of normal initialization, the key will be cleared.
   */
  StatusCode initialize (const std::string& stream,
                         const char* qualifier,
                         bool used = true);


  /**
   * @brief Should be called during the initialize phase.  It will fail
   *        if the requested StoreGate service cannot be found or if the key is blank.
   * @param stream The name of the stream for which thinning is being applied.
   * @param used If false, then this handle is not to be used.
   *             Instead of normal initialization, the key will be cleared.
   */
  StatusCode initialize (const std::string& stream,
                         bool used = true);


  /**
   * @brief Return the write key for the thinning decision.
   */
  const WriteHandleKey<ThinningDecision>& decisionHandleKey() const;


private:
  /// Thinning decision write key.
  WriteHandleKey<ThinningDecision> m_decisionKey;
};


} // namespace SG


#include "StoreGate/ThinningHandleKey.icc"


#endif // not STOREGATE_THINNINGHANDLEKEY_H
