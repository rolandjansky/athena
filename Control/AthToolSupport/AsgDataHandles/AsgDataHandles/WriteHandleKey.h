// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AsgDataHandles/WriteHandleKey.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original version)
 * @brief Property holding a SG store/key/clid from which a WriteHandle is made.
 */


#ifndef ASG_DATA_HANDLES_WRITE_HANDLE_KEY_H
#define ASG_DATA_HANDLES_WRITE_HANDLE_KEY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/WriteHandleKey.h>
#else

#include "AsgDataHandles/VarHandleKey.h"


namespace SG {


/**
 * @brief Property holding a SG store/key/clid from which a WriteHandle is made.
 *
 * See StoreGate/WriteHandleKey for details.
 *
 * This currently (07 Aug 20) contains a number of commented out
 * members that were part of the original handle implementation, but
 * have not yet been implemented in the standalone version.  The plan
 * is to either implement or remove them, depending on what is needed
 * in AnalysisBase as we add more packages.
 *
 */
template <class T>
class WriteHandleKey
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
  WriteHandleKey (const std::string& key = "");
                  // const std::string& storeName = StoreID::storeName(StoreID::EVENT_STORE));


  /**
   * @brief auto-declaring Property Constructor.
   * @param owner Owning component.
   * @param name name of the Property
   * @param key  default StoreGate key for the object.
   * @param doc Documentation string.
   *
   * will associate the named Property with this WHK via declareProperty
   *
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present
   * the store named by @c storeName is used.
   */
  template <class OWNER, class K>
  inline WriteHandleKey (OWNER* owner,
                         std::string name,
                         const K& key={},
                         std::string doc="");

  
  /**
   * @brief Change the key of the object to which we're referring.
   * @param sgkey The StoreGate key for the object.
   * 
   * The provided key may actually start with the name of the store,
   * separated by a "+":  "MyStore+Obj".  If no "+" is present,
   * the store is not changed.
   */
  WriteHandleKey& operator= (const std::string& sgkey);
};


} // namespace SG


#include "AsgDataHandles/WriteHandleKey.icc"


#endif

#endif
