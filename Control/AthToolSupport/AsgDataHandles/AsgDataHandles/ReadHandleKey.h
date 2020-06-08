// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AsgDataHandles/ReadHandleKey.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original version)
 * @brief Property holding a SG store/key/clid from which a ReadHandle is made.
 */


#ifndef ASG_DATA_HANDLES_READ_HANDLE_KEY_H
#define ASG_DATA_HANDLES_READ_HANDLE_KEY_H

#ifndef XAOD_STANDALONE
#include <StoreGate/ReadHandleKey.h>
#else

#include "AsgDataHandles/VarHandleKey.h"


namespace SG {


/**
 * @brief Property holding a SG store/key/clid from which a ReadHandle is made.
 *
 * See StoreGate/ReadHandleKey for details.
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
};


} // namespace SG


#include "AsgDataHandles/ReadHandleKey.icc"

#endif

#endif
