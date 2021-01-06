// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file AsgDataHandles/ReadDecorHandleKey.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original)
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        ReadDecorHandle is made.
 */


#ifndef ASG_DATA_HANDLES_READ_DECOR_HANDLE_KEY_H
#define ASG_DATA_HANDLES_READ_DECOR_HANDLE_KEY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/ReadDecorHandleKey.h>
#else

#include "AsgDataHandles/ReadHandleKey.h"

namespace SG {

/*
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        ReadDecorHandle is made.
 *
 * See StoreGate/ReadDecorHandleKey for details.
 *
 * This currently (31 Jul 20) contains a number of commented out
 * members that were part of the original handle implementation, but
 * have not yet been implemented in the standalone version.  The plan
 * is to either implement or remove them, depending on what is needed
 * in AnalysisBase as we add more packages.
 *
 */
template <class T>
class ReadDecorHandleKey
  : public ReadHandleKey<T>
{
public:
  /// Base class.
  typedef ReadHandleKey<T> Base;

//   /// Class for which we set the CLID.  See above.
//   typedef typename SG::TopBase<T>::type topbase_t;


  /**
   * @brief Constructor.
   * @param key The StoreGate key for the object.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  ReadDecorHandleKey (const std::string& key = "");
                      // const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


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
  template <class OWNER, class K>
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


//   /**
//    * @brief Return the class ID for the referenced object.
//    *
//    * Overridden here to return the CLID for @c T instead of @c topbase_t.
//    */
//   CLID clid() const;


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


#include "AsgDataHandles/ReadDecorHandleKey.icc"

#endif

#endif // not STOREGATE_READDECORHANDLEKEY_H
