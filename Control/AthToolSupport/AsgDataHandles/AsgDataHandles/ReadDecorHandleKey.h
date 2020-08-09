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


//   /**
//    * @brief Constructor.
//    * @param key The StoreGate key for the object.
//    * @param storeName Name to use for the store, if it's not encoded in sgkey.
//    *
//    * The provided key may actually start with the name of the store,
//    * separated by a "+":  "MyStore+Obj".  If no "+" is present
//    * the store named by @c storeName is used.
//    */
//   ReadDecorHandleKey (const std::string& key = "",
//                       const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


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

  
//   /// Can get this from the base class.
//   using Base::operator=;


//   /**
//    * @brief Return the class ID for the referenced object.
//    *
//    * Overridden here to return the CLID for @c T instead of @c topbase_t.
//    */
//   CLID clid() const;
};


} // namespace SG


#include "AsgDataHandles/ReadDecorHandleKey.icc"

#endif

#endif // not STOREGATE_READDECORHANDLEKEY_H
