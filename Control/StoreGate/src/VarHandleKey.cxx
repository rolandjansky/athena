/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/src/VarHandleKey.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief 
 */


// WARNING! WARNING! WARNING!
//
// This version of the file has been modified for use in 21.2 and
// should not be swept into master.



#include "StoreGate/VarHandleKey.h"
#include "StoreGate/exceptions.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/errorcheck.h"
#include <boost/tokenizer.hpp>


namespace SG {


/**
 * @brief Constructor.
 * @param clid The class ID for the referenced object.
 * @param sgkey The StoreGate key for the object.
 * @param a Mode: read/write/update.
 *
 * The provided key may actually start with the name of the store,
 * separated by a slash:  "MyStore/Obj".  If no slash is present,
 * the store named by @c storeName is used.  However, if the key name
 * starts with a slash, it is interpreted as a hierarchical key name,
 * not an empty store name.
 *
 * A SG::ExcBadHandleKey exception will the thrown if the key string
 * format is bad.
 */
VarHandleKey::VarHandleKey (CLID clid,
                            const std::string& sgkey,
                            Gaudi::DataHandle::Mode a,
                            const std::string& storeName /*= "StoreGateSvc"*/)
  : Gaudi::DataHandle (DataObjID (clid, sgkey), a),
    m_storeHandle (storeName, "VarHandleKey")
{
  if (parseKey (sgkey, storeName).isFailure())
    throw SG::ExcBadHandleKey (sgkey);
}


/**
 * @brief Change the key of the object to which we're referring.
 * @param sgkey The StoreGate key for the object.
 * 
 * The provided key may actually start with the name of the store,
 * separated by a slash:  "MyStore/Obj".  If no slash is present
 * the store is not changed.  A key name that starts with a slash
 * is interpreted as a hierarchical key name, not an empty store name.
 *
 * A SG::ExcBadHandleKey exception will the thrown if the key string
 * format is bad.
 */
VarHandleKey& VarHandleKey::operator= (const std::string& sgkey)
{
  if (parseKey (sgkey, m_storeHandle.name()).isFailure())
    throw SG::ExcBadHandleKey (sgkey);
  return *this;
}


/**
 * @brief Change the key of the object to which we're referring.
 * @param sgkey The StoreGate key for the object.
 * 
 * The provided key may actually start with the name of the store,
 * separated by a slash:  "MyStore/Obj".  If no slash is present
 * the store is not changed.  A key name that starts with a slash
 * is interpreted as a hierarchical key name, not an empty store name.
 *
 * Returns failure the key string format is bad.
 */
StatusCode VarHandleKey::assign (const std::string& sgkey)
{
  return parseKey (sgkey, m_storeHandle.name());
}


/**
 * @brief If this object is used as a property, then this should be called
 * during the initialize phase.  It will fail if the requested StoreGate
 * service cannot be found or if the key is blank.
 */
StatusCode VarHandleKey::initialize()
{
  if (Gaudi::DataHandle::objKey() == "") {
    REPORT_ERROR (StatusCode::FAILURE)
      << "Cannot initialize a Read/Write/Update handle with a null key.";
    return StatusCode::FAILURE;
  }
  CHECK( m_storeHandle.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the class ID for the referenced object.
 */
CLID VarHandleKey::clid() const
{
  return Gaudi::DataHandle::fullKey().clid();
}


/**
 * @brief Return the StoreGate ID for the referenced object.
 */
const std::string& VarHandleKey::key() const
{
  return Gaudi::DataHandle::objKey();
}


/**
 * @brief Test if the key is blank.
 */
bool VarHandleKey::empty() const
{
  return Gaudi::DataHandle::objKey().empty();
}


/**
 * @brief Return handle to the referenced store.
 */
ServiceHandle<IProxyDict> VarHandleKey::storeHandle() const
{
  return m_storeHandle;
}


/**
 * @brief Prevent this method from being called.
 */
void VarHandleKey::setKey(const DataObjID& /*key*/)
{
  throw SG::ExcForbiddenMethod ("VarHandleKey::setKey");
}


/**
 * @brief Prevent this method from being called.
 */
void VarHandleKey::updateKey(const std::string& /*key*/)
{
  throw SG::ExcForbiddenMethod ("VarHandleKey::updateKey");
}


/**
 * @brief Handle assignment/construction from a string key.
 * @param sgkey The StoreGate key for the referenced object.
 *
 * The provided key may actually start with the name of the store,
 * separated by a slash:  "MyStore/Obj".  If no slash is present
 * the store named by @c storeName is used.  A key name that starts
 * with a slash is interpreted as a hierarchical key name,
 * not an empty store name.
 */
StatusCode VarHandleKey::parseKey (const std::string& sgkey,
                                   const std::string& storeName)
{
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("/");
  tokenizer tokens(sgkey, sep);
  int nToks = distance(tokens.begin(), tokens.end());
  if (nToks == 0) {
    this->updateHandle (storeName);
    Gaudi::DataHandle::updateKey ("");
  } else {
    if (sgkey.find("/") == 0) {
      this->updateHandle( storeName );
      Gaudi::DataHandle::updateKey( sgkey );
    } else {
      if (nToks == 1) {
        this->updateHandle (storeName);
        Gaudi::DataHandle::updateKey (*(tokens.begin()));
      } else if (nToks == 2) {
        auto tok = tokens.begin();
        this->updateHandle (*tok);
        Gaudi::DataHandle::updateKey (*(++tok));
      } else {
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Update the name of the store to which we're referring.
 * @param name The new store name.
 */
void VarHandleKey::updateHandle (const std::string& name)
{
  // Don't invalidate a stored pointer if the handle is already pointing
  // at the desired service.
  if (m_storeHandle.name() != name)
    m_storeHandle = ServiceHandle<IProxyDict>(name, "VarHandleKey");
}


} // namespace SG
