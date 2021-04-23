/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/src/TypelessWriteHandleKey.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Property holding a write key for which the type can be configured.
 */


#include "StoreGate/TypelessWriteHandleKey.h"
#include "StoreGate/exceptions.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/ServiceHandle.h"


namespace SG {


/**
 * @brief Constructor.
 * @param clid Type of this key.
 * @param key The StoreGate key for the object.
 * @param storeName Name to use for the store, if it's not encoded in sgkey.
 *
 * The provided key may actually start with the name of the store,
 * separated by a "+":  "MyStore+Obj".  If no "+" is present
 * the store named by @c storeName is used.
 */
TypelessWriteHandleKey::TypelessWriteHandleKey (CLID clid,
                                                const std::string& key /*= ""*/,
                                                const std::string& storeName /*= "StoreGateSvc"*/)
  : VarHandleKey (clid, key,
                  Gaudi::DataHandle::Writer,
                  storeName)
{
}


/**
 * @brief Change the key of the object to which we're referring.
 * @param sgkey The StoreGate key for the object.
 * 
 * The key may have the full form STORE+CLASS/NAME, where the STORE+
 * and CLASS/ parts are optional.
 */
TypelessWriteHandleKey&
TypelessWriteHandleKey::operator= (const std::string& sgkey)
{
  if (assign (sgkey).isFailure()) {
    throw SG::ExcBadHandleKey (sgkey);
  }
  return *this;
}


/**
 * @brief Change the key of the object to which we're referring.
 * @param sgkey The StoreGate key for the object.
 * 
 * The key may have the full form STORE+CLASS/NAME, where the STORE+
 * and CLASS/ parts are optional.
 *
 * Returns failure the key string format is bad.
 */
StatusCode TypelessWriteHandleKey::assign (const std::string& key_in)
{
  std::string key = key_in;

  // Handle a possible class name.
  std::string::size_type islash = key.find ('/');
  if (islash != std::string::npos) {
    std::string::size_type istart = key.find ('+');
    if (istart == std::string::npos) {
      istart = 0;
    }
    else {
      ++istart;
    }

    // Excise the class name part from the key.
    std::string clname = key.substr (istart, islash-istart);
    key.erase (istart, islash+1-istart);

    // Look up the class name with the CLID service.
    ServiceHandle<IClassIDSvc> clidsvc ("ClassIDSvc", "TypelessWriteHandleKey");
    CHECK( clidsvc.retrieve() );
    CLID clid = CLID_NULL;
    CHECK( clidsvc->getIDOfTypeName (clname, clid) );

    // Modify the CLID of this key.
    Gaudi::DataHandle::setKey (DataObjID (clid, objKey()));
  }

  // Change the key/store.  Class name has been removed from the string
  // by this point.
  return VarHandleKey::assign (key);
}


} // namespace SG
