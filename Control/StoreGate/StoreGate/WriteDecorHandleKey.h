// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file StoreGate/WriteDecorHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        WriteDecorHandle is made.
 */


#ifndef STOREGATE_WRITEDECORHANDLEKEY_H
#define STOREGATE_WRITEDECORHANDLEKEY_H


#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


class AthAlgorithm;
class AthReentrantAlgorithm;
class AthAlgTool;


namespace SG {


/*
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        WriteDecorHandle is made.
 *
 * This class holds the key part of a WriteDecorHandle.
 *
 * Use this to add a decoration to an object such that the scheduler
 * is aware of it.  This is used just like a @c WriteHandleKey (which see)
 * except that the key string is of the form CCC.DDD, where CCC is the
 * name of the container in StoreGate and DDD is the name of the decoration.
 *
 * Example:
 *
 *@code
 *  class MyAlgorithm : public AthReentrantAlgorithm {
 *    ...
 *    SG::WriteDecorHandleKey<MyCont> m_key;
 *  };
 *  ...
 *  MyAlgorithm::MyAlgorithm (const std::string& name, ISvcLocator* svcloc)
 *    : AthReentrantAlgorithm (name, svcloc), ...
 *  {
 *    declareProperty ("Key", m_key = "container.decor");
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::initialize()
 *  {
 *    ATH_CHECK( m_key.initialize() );
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::execute_r (const EventContext& ctx) const
 *  {
 *    SG::WriteDecorHandle<MyCont, float> h (m_key, ctx);
 *    for (const MyObj& o : *h) {  // Access the container.
 *      h (o) = calculate (o);  // Add the decoration.
 *      ...
 @endcode
 *
 * Implementation note: We want to make two dependencies from this key: a read
 * dependency on the container itself, and a write dependency on the decoration.
 * This class derives from @c WriteHandleKey, which provides the output dependency
 * on the decoration.  We also hold as a member a @c ReadHandleKey for the
 * container.  This extra depedency is added at initialize time via
 * registerWriteDecorHandleKey(), which see.
 */
template <class T>
class WriteDecorHandleKey
  : public WriteHandleKey<T>
{
public:
  typedef WriteHandleKey<T> Base;

  
  /**
   * @brief Constructor.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  WriteDecorHandleKey (const std::string& key = "",
                       const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present,
   * the store is not changed.
   */
  WriteDecorHandleKey& operator= (const std::string& sgkey);


  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store is not changed.  A key name that starts with a slash
   * is interpreted as a hierarchical key name, not an empty store name.
   *
   * Returns failure the key string format is bad.
   */
  virtual StatusCode assign (const std::string& sgkey) override;

  
  /**
   * @brief If this object is used as a property, then this should be called
   *        during the initialize phase.  It will fail if the requested
   *        StoreGate service cannot be found or if the key is blank.
   * @param used If false, then this handle is not to be used.
   *             Instead of normal initialization, the key will be cleared.
   */
  StatusCode initialize (bool used = true);


  /**
   * @brief Return the handle key for the container.
   */
  const ReadHandleKey<T>& contHandleKey() const;


private:
  friend class ::AthAlgorithm;
  friend class ::AthReentrantAlgorithm;
  friend class ::AthAlgTool;

  /**
   * @brief Return the handle key for the container.
   *
   * Should be used only by AthAlgorithm/AthAlgTool.
   */
  ReadHandleKey<T>& contHandleKey_nc();


  /// The container handle.
  ReadHandleKey<T> m_contHandleKey;
};


} // namespace SG


#include "StoreGate/WriteDecorHandleKey.icc"


#endif // not STOREGATE_WRITEDECORHANDLEKEY_H
