/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/src/VarHandleKey.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief A property holding a SG store/key/clid from which a VarHandle is made.
 */


#include "StoreGate/VarHandleKey.h"
#include "StoreGate/exceptions.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/StoreID.h"
#include <boost/tokenizer.hpp>

#include "StoreGate/StoreGateSvc.h"

static const char* const storeSeparator = "+";

namespace SG {


/**
 * @brief Constructor.
 * @param clid The class ID for the referenced object.
 * @param sgkey The StoreGate key for the object.
 * @param a: read/write/update.
 * @param isCond True if this is a CondHandleKey.
 *
 * The provided key may actually start with the name of the store,
 * separated by a "+":  "MyStore+Obj".  If no "+" is present,
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
                            const std::string& storeName /*= "StoreGateSvc"*/,
                            bool isCond /*= false*/)
  : Gaudi::DataHandle (DataObjID (clid, sgkey), isCond, a),
    m_storeHandle (storeName, "VarHandleKey")
{
  parseKey (sgkey, storeName);
  m_isEventStore =  (m_storeHandle.name() == StoreID::storeName(StoreID::EVENT_STORE) ||
                     m_storeHandle.name() == StoreID::storeName(StoreID::PILEUP_STORE));
}


/**
 * @brief Change the key of the object to which we're referring.
 * @param sgkey The StoreGate key for the object.
 * 
 * The provided key may actually start with the name of the store,
 * separated by a "+":  "MyStore+Obj".  If no "+" is present
 * the store is not changed.  A key name that starts with a slash
 * is interpreted as a hierarchical key name, not an empty store name.
 *
 * A SG::ExcBadHandleKey exception will the thrown if the key string
 * format is bad.
 */
VarHandleKey& VarHandleKey::operator= (const std::string& sgkey)
{
  parseKey (sgkey, m_storeHandle.name());
  return *this;
}


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
StatusCode VarHandleKey::assign (const std::string& sgkey)
{
  try {
    parseKey (sgkey, m_storeHandle.name());
  } catch (SG::ExcBadHandleKey &e) {
    std::cerr << "VarHandleKey::assign failure: " << e.what() << std::endl;
    return StatusCode::FAILURE;
  } catch (...) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief If this object is used as a property, then this should be called
 *        during the initialize phase.  It will fail if the requested
 *        StoreGate service cannot be found or if the key is blank.
 *
 * @param used If false, then this handle is not to be used.
 *             Instead of normal initialization, the key will be cleared.
 */
StatusCode VarHandleKey::initialize (bool used /*= true*/)
{
  if (!used) {
    Gaudi::DataHandle::updateKey ( "" );
    m_sgKey = "";
    return StatusCode::SUCCESS;
  }

  //  if (Gaudi::DataHandle::objKey() == "") {
  if (key() == "") {
    REPORT_ERROR (StatusCode::FAILURE)
      << "Cannot initialize a Read/Write/Update handle with a null key.";
    return StatusCode::FAILURE;
  }
  // Don't do retrieve() here.  That unconditionally fetches the pointer
  // from the service manager, even if it's still valid, which it might
  // be if this is a handle that was just created from a key.
  if (!m_storeHandle.isValid()) {
    REPORT_ERROR (StatusCode::FAILURE)
      << "Cannot locate store: " << m_storeHandle.typeAndName();
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief If this object is used as a property, then this should be called
 *        during the initialize phase.  This variant will allow the key
 *        to be blank.
 * @param Flag to select this variant.  Call like
 *@code
 *  ATH_CHECK( key.initialize (SG::AllowEmpty) );
 @endcode
*/
StatusCode VarHandleKey::initialize (AllowEmptyEnum)
{
  if (key().empty()) {
    return StatusCode::SUCCESS;
  }
  return initialize (true);
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
  return m_sgKey;
}


/**
 * @brief Test if the key is blank.
 */
bool VarHandleKey::empty() const
{
  return m_sgKey.empty();
}


/**
 * @brief Prevent this method from being called.
 */
void VarHandleKey::setKey(const DataObjID& /*key*/) const
{
  throw SG::ExcForbiddenMethod ("VarHandleKey::setKey");
}


/**
 * @brief Prevent this method from being called.
 */
void VarHandleKey::updateKey(const std::string& /*key*/) const
{
  throw SG::ExcForbiddenMethod ("VarHandleKey::updateKey");
}


/**
 * @brief Handle assignment/construction from a string key.
 * @param sgkey The StoreGate key for the referenced object.
 *
 * The provided key may actually start with the name of the store,
 * separated by a "+":  "MyStore+Obj".  If no "+" is present
 * the store named by @c storeName is used.  A key name that starts
 * with a slash is interpreted as a hierarchical key name,
 * not an empty store name.
 *
 * we also have to check that if the key contains the store name, 
 * it matches the store name that they Handle was constructed with
 *
 * blank keys result in blank DataObjIDs
 *
 */
void VarHandleKey::parseKey (const std::string& key,
                             const std::string& storeName)
{
  std::string sn;
  // test if storeName has classname
  std::string::size_type sp = storeName.find("/");
  if (sp == std::string::npos) {
    sn = storeName;
  } else {
    sn = storeName.substr(sp+1,storeName.length()-sp+1);
  }

  if (key.length() == 0) {
    this->updateHandle(sn);
    Gaudi::DataHandle::updateKey("");
    m_sgKey = "";
    return;
  }

  // StoreName separator is "+"
  sp = key.find(storeSeparator);
  if(sp == std::string::npos) {
    m_sgKey = key;
  } else {
    sn = key.substr(0,sp);
    m_sgKey = key.substr(sp+1,key.length()-sp-1);
  }


  this->updateHandle(sn);

  StoreID::type st;
  // would be nice if we could get the storeID from the storeHandle, but
  // we can't be sure that the store has been created when the VHK is
  // constructed.
  //
  // StoreGateSvc *sgs = dynamic_cast<StoreGateSvc*>( &(*m_storeHandle) );
  // if (sgs != 0) {
  //   st = sgs->storeID();
  // } else {
    st = StoreID::findStoreID(sn);
  // }

    if (st != StoreID::CONDITION_STORE && st != StoreID::METADATA_STORE) {
    if (m_sgKey.find("/") != std::string::npos) {
      throw SG::ExcBadHandleKey("key \"" + key 
                                + "\": keys with \"/\" only allowed for "
                                + StoreID::storeName(StoreID::CONDITION_STORE) 
                                + " - store is \""
                                + sn + "\"");
    }
  } else {
    sp = m_sgKey.rfind("/");
    if (sp != std::string::npos) {
      if (sp == 0) {
        // blank key
        m_sgKey = "";
      } else if ( sp == m_sgKey.length()-1) {
        throw SG::ExcBadHandleKey("key \"" + key 
                                  + "\": must not end with a \"/\"");
      }
    }
  }
  
  if (m_sgKey.length() == 0) {
    Gaudi::DataHandle::updateKey("");
  } else {
    Gaudi::DataHandle::updateKey(sn + storeSeparator + m_sgKey);
  }


}


/**
 * @brief Update the name of the store to which we're referring.
 * @param name The new store name.
 */
void VarHandleKey::updateHandle (const std::string& name)
{
  // Don't invalidate a stored pointer if the handle is already pointing
  // at the desired service.
  if (m_storeHandle.name() != name) {
    m_storeHandle = ServiceHandle<IProxyDict>(name, "VarHandleKey");
    m_isEventStore =  (name == StoreID::storeName(StoreID::EVENT_STORE) ||
                       name == StoreID::storeName(StoreID::PILEUP_STORE));
  }
}

} // namespace SG

namespace std {
  ostream& operator<<(ostream& s, const SG::VarHandleKey& m) {
    s << "'" << m.objKey() << "'";
    return s;
  }
}
