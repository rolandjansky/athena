// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VarHandleBase.h 797637 2017-02-17 02:32:11Z ssnyder $
/**
 * @file AsgDataHandles/VarHandleBase.h
 * @author Nils Krumnack <Nils.Erik.Krumnack@cern.h>
 * @author S. Binet, P. Calafiura, scott snyder <snyder@bnl.gov> (for original)
 * @brief Base class for VarHandle classes.
 */

#ifndef ASG_DATA_HANDLES_VAR_HANDLE_BASE_H
#define ASG_DATA_HANDLES_VAR_HANDLE_BASE_H

#ifndef XAOD_STANDALONE
#include <StoreGate/VarHandleBase.h>
#else

// STL includes
#include <string>

// #include "AsgDataHandles/VarHandleKey.h"

class EventContext;

namespace SG {
  // static constexpr bool defaultQuiet = false;

  class VarHandleKey;


  /**
   * @class SG::VarHandleBase
   * @brief Base class for VarHandle types.
   *
   * See StoreGate/VarHandleBase for details.
   */
  class VarHandleBase
  {
  public: 

    /**
     * @brief Constructor from a VarHandleKey.
     * @param key The key object holding the clid/key/store.
     * @param ctx The event context to use, or nullptr.
     *
     * This will raise an exception if the StoreGate key is blank,
     * or if the event store cannot be found.
     *
     * If the default event store has been requested, then the thread-specific
     * store from @c ctx will be used.  If @c ctx is null, then the
     * current event context will be read.
     */
    explicit VarHandleBase (const VarHandleKey& key, const EventContext* ctx);


    //*************************************************************************
    // Accessors
    //


    /**
     * @brief Return the StoreGate ID for the referenced object.
     */
    const std::string& key() const;


    /**
     * @brief Return the StoreGate ID for the referenced object.
     *
     * A synonym for key().
     */
    const std::string& name() const ;


  //   /**
  //    * @brief Return the name of the store holding the object we are proxying.
  //    */
  //   std::string store() const;


    //*************************************************************************
    // @c VarHandleKey methods.
    //


  //   /**
  //    * @brief Return the key string of the underlying @c DataObjID.
  //    *
  //    * Compared to @c key(), this will be prefixed with the store name.
  //    */
  //   const std::string& objKey() const;


  //   /**
  //    * @brief Return the key as a @c DataObjID.
  //    */
  //   const DataObjID& fullKey() const;
 

  //   /**
  //    * @brief Return a non-const reference to the HandleKey.
  //    *
  //    * If this handle was initialized from a HandleKey, then this doesn't work
  //    * (since the HandleKey is const). ExcNonConstHandleKey will be thrown
  //    * in that case.
  //    */
  //   SG::VarHandleKey& vhKey();


  //   /**
  //    * @brief Update the underlying key from a string.
  //    *
  //    * If this handle was initialized from a HandleKey, then this doesn't work
  //    * (since the HandleKey is const). ExcNonConstHandleKey will be thrown
  //    * in that case.
  //    *
  //    * See VarHandleKey::assign.
  //    */
  //   StatusCode assign (const std::string& sgkey);



  protected: 
    //*************************************************************************
    // Protected data.
    //


  private:
    /// The associated key object.
    const VarHandleKey* m_key {nullptr};

  }; 


  // //*************************************************************************
  // // Free functions.
  // //


  // /**
  //  * @brief Output stream.
  //  * @param out Stream to which to write.
  //  * @param o Object to write.
  //  */
  // std::ostream& operator<<( std::ostream& out, const VarHandleBase& o );


  // /**
  //  * @brief Equality comparison.
  //  */
  // bool operator==(const VarHandleBase& l, const VarHandleBase& r);


  // /**
  //  * @brief Inequality comparison.
  //  */
  // bool operator!=(const VarHandleBase& l, const VarHandleBase& r);


} /* namespace SG */

#include "AsgDataHandles/VarHandleBase.icc"

#endif

#endif
