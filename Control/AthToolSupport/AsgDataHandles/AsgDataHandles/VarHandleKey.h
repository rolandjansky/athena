// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AsgDataHandles/VarHandleKey.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original)
 * @brief A property holding a SG store/key/clid from which a VarHandle is made.
 */


#ifndef ASG_DATA_HANDLES_VAR_HANDLE_KEY_H
#define ASG_DATA_HANDLES_VAR_HANDLE_KEY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/VarHandleKey.h>
#else

#include "AsgDataHandles/common.h"
#include <AsgMessaging/StatusCode.h>
#include <string>

namespace SG {


enum AllowEmptyEnum {
  AllowEmpty = 1
};


class VarHandleBase;


/**
 * @brief A property holding a SG store/key/clid from which a VarHandle is made.
 *
 * See StoreGate/VarHandleKey for details.
 */
class VarHandleKey
{
public:
  /**
   * @brief Constructor.
   * @param clid The class ID for the referenced object.
   * @param sgkey The StoreGate key for the object.
   * @param a Mode: read/write/update.
   * @param storeName Name to use for the store, if it's not encoded in sgkey.
   * @param isCond True if this is a CondHandleKey.
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.  However, if the key name
   * starts with a slash, it is interpreted as a hierarchical key name,
   * not an empty store name.
   *
   * A SG::ExcBadHandleKey exception will the thrown if the key string
   * format is bad.
   */
  explicit VarHandleKey (const std::string& sgkey);
  // VarHandleKey (CLID clid,
  //               const std::string& sgkey,
  //               Gaudi::DataHandle::Mode a,
  //               const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE),
  //               bool isCond = false);


  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present,
   * the store is not changed.  A key name that starts with a slash
   * is interpreted as a hierarchical key name, not an empty store name.
   *
   * A SG::ExcBadHandleKey exception will the thrown if the key string
   * format is bad.
   */
  VarHandleKey& operator= (const std::string& sgkey);


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
  virtual StatusCode assign (const std::string& sgkey);

  
  /**
   * @brief If this object is used as a property, then this should be called
   *        during the initialize phase.  It will fail if the requested
   *        StoreGate service cannot be found or if the key is blank.
   *
   * @param used If false, then this handle is not to be used.
   *             Instead of normal initialization, the key will be cleared.
   */
  StatusCode initialize (bool used = true);


  /**
   * @brief If this object is used as a property, then this should be called
   *        during the initialize phase.  This variant will allow the key
   *        to be blank.
   * @param Flag to select this variant.  Call like
   *@code
   *  ATH_CHECK( key.initialize (SG::AllowEmpty) );
   @endcode
   */
  StatusCode initialize (AllowEmptyEnum);


  /**
   * @brief Return the StoreGate ID for the referenced object.
   */
  const std::string& key() const;


  /**
   * @brief Test if the key is blank.
   */
  bool empty() const;


  /**
   * @brief Does this key reference the primary event store?
   */
  bool isEventStore() const;

private:


//   /**
//    * @brief Handle assignment/construction from a string key.
//    * @param sgkey The StoreGate key for the referenced object.
//    *
//    * The provided key may actually start with the name of the store,
//    * separated by a "+":  "MyStore+Obj".  If no "+" is present,
//    * the store named by @c storeName is used.  A key name that starts
//    * with a slash is interpreted as a hierarchical key name,
//    * not an empty store name.
//    *
//    * see implementation comments for further details
//    */
//   void parseKey (const std::string& sgkey, const std::string& storeName);


//   /**
//    * @brief Update the name of the store to which we're referring.
//    * @param name The new store name.
//    */
//   void updateHandle (const std::string& name);

  /// StoreGate key, that doesn't include the storename
  std::string m_sgKey;

  /// Cache test for whether we're referencing the event store.
  bool m_isEventStore = true;
};


} // namespace SG

namespace std {
  ostream& operator<<(ostream& s, const SG::VarHandleKey& m);
}


#include "AsgDataHandles/VarHandleKey.icc"

#endif

#endif
