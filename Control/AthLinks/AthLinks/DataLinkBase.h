// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataLinkBase.h 783590 2016-11-11 00:38:09Z ssnyder $
/**
 * @file AthLinks/DataLinkBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Type-independent part of @c DataLink; holds the persistent state.
 */


#ifndef ATHLINKS_DATALINKBASE_H
#define ATHLINKS_DATALINKBASE_H


#include "AthLinks/tools/DataProxyHolder.h"
#include "AthLinks/exceptions.h"
#include "GaudiKernel/EventContext.h"


namespace SG {
class DataProxy;
}
class IProxyDict;


/**
 * @brief Type-independent part of @c DataLink; holds the persistent state.
 *
 * The persistent state for a @c DataLink is the hashed SG key, stored here
 * as @c m_persKey.  The transient member @c m_proxy contains the data proxy
 * pointer.
 */
class DataLinkBase
{
public:
  /// Type of hashed keys.
  typedef SG::DataProxyHolder::sgkey_t sgkey_t;

  /// Type of string keys.
  typedef SG::DataProxyHolder::ID_type ID_type;

  typedef SG::DataProxyHolder::pointer_t       pointer_t;
  typedef SG::DataProxyHolder::const_pointer_t const_pointer_t;

  /// Function casting from a @c SG::DataProxy to a pointer.
  typedef SG::DataProxyHolder::castfn_t        castfn_t;


  /**
   * @brief Test to see if we're in the default state.
   */
  bool isDefault() const;


  /**
   * @brief Return the SG key that we reference, as a string.
   *
   * Returns a null string on failure.
   */
  const ID_type& dataID() const;


  /**
   * @brief Return the SG key that we reference, as a hash.
   *
   * Returns 0 on failure.
   */
  sgkey_t key() const;


  /**
   * @brief Clear the link (make it null).
   */
  void clear();


  /**
   * @brief Return the @c DataProxy for this link.
   * @param nothrow If true, return 0 on failure instead of throwing
   *                an exception.
   *
   * If this is a null link, we return 0.
   * If we're directly referencing an object that's not in StoreGate,
   * either return 0 or throw @c ExcPointerNotInSG, depending
   * on @c nothrow.  Otherwise, return the proxy for the object
   * we're referencing.
   */
  SG::DataProxy* proxy (bool nothrow = false) const;


  /**
   * @brief Return the data source for this reference.
   */
  IProxyDict* source() const;


  
  /**
   * @brief Finish initialization after link has been read.
   * @param sg Associated store.
   *
   * This should be called after a link has been read by root
   * in order to set the proxy pointer.
   * Returns true.
   *
   * If @c sg is 0, then we use the global default store.
   */
  bool toTransient (IProxyDict* sg = 0);


  /**
   * @brief Finish initialization after link has been read.
   * @param ctx Event context for this link.
   *
   * This should be called after a link has been read by root
   * in order to set the proxy pointer.
   * Returns true.
   */
  bool toTransient (const EventContext& ctx);


  /**
   * @brief Finish initialization like the link as just been read from root,
   *        but with a specified key.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   *
   * The link should be clear before this is called.
   * Returns true.
   *
   * If @c sg is 0, then we use the global default store.
   */
  bool toTransient (const ID_type& dataID, CLID link_clid, IProxyDict* sg = 0);


  /**
   * @brief Finish initialization like the link as just been read from root,
   *        but with a specified key.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param ctx Event context for this link.
   *
   * The link should be clear before this is called.
   * Returns true.
   */
  bool toTransient (const ID_type& dataID, CLID link_clid,
                    const EventContext& ctx);


  /**
   * @brief Prepare this link for writing.
   *
   * One of the @c toPersistent methods should be called before
   * trying to write the link with root.
   *
   * Ensures the hashed SG key to be saved is correct.
   * If this link is referencing an object not in SG,
   * we throw @c ExcPointerNotInSG.
   *
   * If the target of the link has been remapped, then the link will be
   * updated with the remapped hash key.
   *
   * Returns true.
   */
  bool toPersistent();


  /**
   * @brief Prepare this link for writing.
   *
   * One of the @c toPersistent methods should be called before
   * trying to write the link with root.
   *
   * Ensures the hashed SG key to be saved is correct.
   * If this link is referencing an object not in SG,
   * we throw @c ExcPointerNotInSG.
   *
   * This version does not perform link remapping.
   *
   * Returns true.
   */
  bool toPersistentNoRemap();


  /**
   * @brief Compare for equality.
   */
  bool operator== (const DataLinkBase& other) const;


  /**
   * @brief Compare for inequality.
   */
  bool operator!= (const DataLinkBase& other) const;



protected:
  /// For component testing.
  friend class DataLinkBase_test;


  /**
   * @brief Default constructor.
   */
  DataLinkBase();


  /**
   * @brief Constructor from pointer to object.
   * @param obj Pointer to the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  DataLinkBase (const_pointer_t obj, CLID link_clid, IProxyDict* sg);


  /**
   * @brief Constructor from a string key.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   */
  DataLinkBase (const ID_type& dataID, CLID link_clid, IProxyDict* sg);


  /**
   * @brief Constructor from a hashed key.
   * @param key Hashed key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  DataLinkBase (sgkey_t key, CLID link_clid, IProxyDict* sg);


  /**
   * @brief Constructor from a hashed key and a proxy holder object.
   *        Used internally for EL -> DL conversion.
   * @param key Hashed key of the object.
   * @param holder Internal holder object for the proxy.
   */
  DataLinkBase (sgkey_t key, const SG::DataProxyHolder& holder);


  /**
   * @brief Set the link to an object given by a pointer.
   * @param obj Pointer to the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   * @returns The SG key for this object.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  void toStorableObject (const_pointer_t obj,
                         CLID clid_in,
                         IProxyDict* sg);


  /**
   * @brief Set the link to an object given by a string key.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   * @returns The SG key for this object.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void toIdentifiedObject (const ID_type& dataID,
                           CLID clid,
                           IProxyDict* sg);


  /**
   * @brief Set the link to an object given by a hashed key.
   * @param key Hashed key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  void toIdentifiedObject (sgkey_t key,
                           CLID clid,
                           IProxyDict* sg);


  /**
   * @brief Return a pointer to the currently-referenced object.
   * @param castfn Function to do the cast from data proxy to object.
   *               If 0, use a dynamic cast.
   * @param clid The CLID of the desired object.
   *             This is used to determine how the returned pointer
   *             is to be converted.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  void* storableBase (castfn_t* castfn, const CLID& clid) const;



  /**
   * @brief Throw a @c ExcInvalidLink exception for this link.
   * @param sgkey The hashed key for this link.
   *
   * This will fill in parameters for the exception message from the proxy.
   */
  void throwInvalidLink() const;



private:
  /// The hashed key for this link.
  sgkey_t m_persKey;

  /// SG proxy for this link.
  SG::DataProxyHolder m_proxy;  //! Transient.
};


#include "AthLinks/DataLinkBase.icc"


#endif // not ATHLINKS_DATALINKBASE_H
