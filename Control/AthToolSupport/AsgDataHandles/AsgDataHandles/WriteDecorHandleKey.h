// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file StoreGate/WriteDecorHandleKey.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original version)
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        WriteDecorHandle is made.
 */


#ifndef ASG_DATA_HANDLES_WRITE_DECOR_HANDLE_KEY_H
#define ASG_DATA_HANDLES_WRITE_DECOR_HANDLE_KEY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/WriteDecorHandleKey.h>
#else


#include "AsgDataHandles/DecorKeyHelpers.h"
#include "AsgDataHandles/WriteHandleKey.h"
// #include "StoreGate/ReadHandleKey.h"


// class AthAlgorithm;
// class AthReentrantAlgorithm;
// class AthAlgTool;


namespace SG {


/*
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        WriteDecorHandle is made.
 *
 * See StoreGate/WriteDecorHandleKey for details.
 *
 * This currently (24 Aug 20) contains a number of commented out
 * members that were part of the original handle implementation, but
 * have not yet been implemented in the standalone version.  The plan
 * is to either implement or remove them, depending on what is needed
 * in AnalysisBase as we add more packages.
 *
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
  explicit WriteDecorHandleKey (const std::string& key = ""/*,
                                                    const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE)*/);


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
  WriteDecorHandleKey( OWNER* owner,
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
  WriteDecorHandleKey& operator= (const std::string& sgkey);


//   /**
//    * @brief Change the key of the object to which we're referring.
//    * @param sgkey The StoreGate key for the object.
//    * 
//    * The provided key may actually start with the name of the store,
//    * separated by a "+":  "MyStore+Obj".  If no "+" is present
//    * the store is not changed.  A key name that starts with a slash
//    * is interpreted as a hierarchical key name, not an empty store name.
//    *
//    * Returns failure the key string format is bad.
//    */
//   virtual StatusCode assign (const std::string& sgkey) override;

  
//   /**
//    * @brief If this object is used as a property, then this should be called
//    *        during the initialize phase.  It will fail if the requested
//    *        StoreGate service cannot be found or if the key is blank.
//    * @param used If false, then this handle is not to be used.
//    *             Instead of normal initialization, the key will be cleared.
//    */
//   StatusCode initialize (bool used = true);


//   /**
//    * @brief Return the handle key for the container.
//    */
//   const ReadHandleKey<T>& contHandleKey() const;


// private:
//   friend class ::AthAlgorithm;
//   friend class ::AthReentrantAlgorithm;
//   friend class ::AthAlgTool;

//   /**
//    * @brief Return the handle key for the container.
//    *
//    * Should be used only by AthAlgorithm/AthAlgTool.
//    */
//   ReadHandleKey<T>& contHandleKey_nc();


//   /// The container handle.
//   ReadHandleKey<T> m_contHandleKey;
};


} // namespace SG


#include "AsgDataHandles/WriteDecorHandleKey.icc"

#endif

#endif
