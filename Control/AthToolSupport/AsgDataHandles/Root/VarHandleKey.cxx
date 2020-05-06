/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AsgDataHandles/src/VarHandleKey.cxx
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author scott snyder <snyder@bnl.gov> (for original)
 * @brief A property holding a SG store/key/clid from which a VarHandle is made.
 */


#include "AsgDataHandles/VarHandleKey.h"
// #include "AsgDataHandles/exceptions.h"

#include <iostream>

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
VarHandleKey::VarHandleKey (const std::string& sgkey)
  : m_sgKey (sgkey)
{
}
// VarHandleKey::VarHandleKey (CLID clid,
//                             const std::string& sgkey,
//                             Gaudi::DataHandle::Mode a,
//                             const std::string& storeName /*= "StoreGateSvc"*/,
//                             bool isCond /*= false*/)
//   : Gaudi::DataHandle (DataObjID (clid, sgkey), isCond, a),
//     m_storeHandle (storeName, "VarHandleKey")
// {
//   parseKey (sgkey, storeName);
//   m_isEventStore =  (m_storeHandle.name() == StoreID::storeName(StoreID::EVENT_STORE) ||
//                      m_storeHandle.name() == StoreID::storeName(StoreID::PILEUP_STORE));
// }


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
  m_sgKey = sgkey;
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
  m_sgKey = sgkey;
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
    m_sgKey = "";
    return StatusCode::SUCCESS;
  }

  if (key() == "") {
    // REPORT_ERROR (StatusCode::FAILURE)
    //   << "Cannot initialize a Read/Write/Update handle with a null key.";
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

} // namespace SG

namespace std {
  ostream& operator<<(ostream& s, const SG::VarHandleKey& m) {
    // s << "'" << m.objKey() << "'";
    s << "'" << m.key() << "'";
    return s;
  }
}
