///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleBase.cxx 
// Implementation file for class VarHandleBase
// Author: Nils Krumnack <Nils.Erik.Krumnack@cern.ch>
// Author: Paolo Calafiura (for original)
// Author: S.Binet<binet@cern.ch> (for original)
/////////////////////////////////////////////////////////////////// 

#include "AsgDataHandles/VarHandleBase.h"
#include "AsgDataHandles/VarHandleKey.h"

// #include "AsgDataHandles/exceptions.h"

// #include <algorithm>
#include <sstream>


namespace SG {


  /**
   * @brief Constructor from a VarHandleKey.
   * @param key The key object holding the clid/key/store.
   * @param ctx The event context to use, or nullptr.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  VarHandleBase::VarHandleBase (const VarHandleKey& key,
                                const EventContext* /*ctx*/)
    : m_key (&key)
  {
  }



  //*************************************************************************
  // Accessors
  //


  /**
   * @brief Return the StoreGate ID for the referenced object.
   *
   * This is defined in @c VarHandleKey.  We need to redefine it here because
   * it's also in @c IResetable.  (Otherwise there would be an ambiguity.)
   */
  const std::string& VarHandleBase::key() const
  {
    return m_key->key();
  }


  /**
   * @brief Return the StoreGate ID for the referenced object.
   *
   * A synonym for key().
   */
  const std::string& VarHandleBase::name() const 
  {
    return this->key();
  }


//   /**
//    * @brief Return the name of the store holding the object we are proxying.
//    */
//   std::string VarHandleBase::store() const
//   {
//     if (m_store)
//       return m_store->name();
//     return this->storeHandle().name();
//   }


  //*************************************************************************
  // Protected methods.
  //


//*************************************************************************
  // Free functions.
  //


  /**
   * @brief Output stream.
   * @param out Stream to which to write.
   * @param o Object to write.
   */
  std::ostream& operator<<( std::ostream& out, const VarHandleBase& o )
  {
    out << "VarHandleBase @" << &o
        // << " store=" <<o.store()
        << ", key=" <<o.key();
    return out;
  }


  /**
   * @brief Equality comparison.
   */
  bool operator==(const VarHandleBase& l, const VarHandleBase& r)
  {
    return (l.name() == r.name());//  &&
            // l.store() == r.store());
  }


  /**
   * @brief Inequality comparison.
   */
  bool operator!=(const VarHandleBase& l, const VarHandleBase& r)
  {
    return !(l==r);
  }


} /* namespace SG */
