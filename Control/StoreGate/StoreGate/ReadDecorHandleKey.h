// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/ReadDecorHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        ReadDecorHandle is made.
 */


#ifndef STOREGATE_READDECORHANDLEKEY_H
#define STOREGATE_READDECORHANDLEKEY_H


#include "StoreGate/ReadHandleKey.h"
#include "AthenaKernel/TopBase.h"


namespace SG {


/*
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        ReadDecorHandle is made.
 *
 * This class holds the key part of a ReadDecorHandle.
 *
 * Use this to read a decoration on an object such that the scheduler
 * is aware of it.  This is used just like a @c ReadHandleKey (which see)
 * except that the key string is of the form CCC.DDD, where CCC is the
 * name of the container in StoreGate and DDD is the name of the decoration.
 *
 * Example:
 *
 *@code
 *  class MyAlgorithm : public AthReentrantAlgorithm {
 *    ...
 *    SG::ReadDecorHandleKey<MyCont> m_key;
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
 *  StatusCode MyAlgorithm::execute (const EventContext& ctx) const
 *  {
 *    SG::ReadDecorHandle<MyCont, float> h (m_key, ctx);
 *    for (const MyObj& o : *h) {  // Access the container.
 *      doSomething (h (o));  // Access the decoration.
 *      ...
 @endcode
 *
 * One can run into issues with the scheduler if this is used in conjunction
 * with inheritance.  For example, if @c D derives from @c B, and one uses
 * WriteDecorHandleKey<B>, then the scheduler will know only about the
 * B.DECOR output; it won't have a dependency for D.DECOR.  That means that
 * trying to use D.DECOR as a dependency downstream won't work.  To solve this,
 * we arrange that the CLID in the DataObjID for ReadDecorHandleKey<D>
 * is actually that for B; in that case, the scheduler will be happy.
 */
template <class T>
class ReadDecorHandleKey
  : public ReadHandleKey<T>
{
public:
  /// Base class.
  typedef ReadHandleKey<T> Base;

  /// Class for which we set the CLID.  See above.
  typedef typename SG::TopBase<T>::type topbase_t;


  /**
   * @brief Constructor.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  ReadDecorHandleKey (const std::string& key = "",
                      const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief auto-declaring Property Constructor.
   * @param owner Owning component.
   * @param name name of the Property
   * @param key  default StoreGate key for the object.
   * @param doc Documentation string.
   *
   * will associate the named Property with this RHK via declareProperty
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  template <class OWNER, class K,
            typename = typename std::enable_if<std::is_base_of<IProperty, OWNER>::value>::type>
  ReadDecorHandleKey( OWNER* owner,
                      const std::string& name,
                      const K& key = {},
                      const std::string& doc = "");

  
  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present,
   * the store is not changed.
   */
  ReadDecorHandleKey& operator= (const std::string& sgkey);


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
   * @brief Return the class ID for the referenced object.
   *
   * Overridden here to return the CLID for @c T instead of @c topbase_t.
   */
  CLID clid() const;


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
  /// The container handle.
  ReadHandleKey<T> m_contHandleKey;
};


} // namespace SG


#include "StoreGate/ReadDecorHandleKey.icc"


#endif // not STOREGATE_READDECORHANDLEKEY_H
