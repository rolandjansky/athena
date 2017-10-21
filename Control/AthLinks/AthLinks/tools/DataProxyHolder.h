// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/tools/DataProxyHolder.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2013
 * @brief Manage @c DataProxy reference in ElementLink/DataLink.
 */


#ifndef ATHLINKS_DATAPROXYHOLDER_H
#define ATHLINKS_DATAPROXYHOLDER_H


#include "SGTools/CurrentEventStore.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/IStringPool.h"
#include "AthenaKernel/RCUObject.h"
#include "CxxUtils/checker_macros.h"
#include <string>
#include <unordered_map>


namespace SG {


/**
 * @brief Manage @c DataProxy reference in ElementLink/DataLink.
 *
 * This class factors out operations on @c DataProxy that are common
 * between @c ElementLink and @c DataLink, and holds a pointer to the
 * proxy.  This pointer is transient; the (persistent) SG key field
 * is stored in the class that holds this one, and is passed as a parameter
 * where needed.
 *
 * We can be initialized either with a key or with a pointer to an object.
 * In the former case, we try immediately to look up the proxy for that key.
 * If we find it, then we use it; otherwise, we create a dummy @c DataProxy
 * for that key and add it to our store.  If we're given a pointer, we again
 * immediately try to look up the corresponding proxy.  If we don't find it,
 * then we store the pointer directly in the proxy member, with a flag
 * to identify this case.  Subsequent operations that require a proxy
 * will again try to look up the proxy in the store; if that fails,
 * then an exception will be thrown.
 *
 * When we create a new link, the store by default is taken from the
 * global default given by SG::CurrentEventStore::store().
 * This is a thread-specific value, so fetching it can be expensive.
 * If you are creating links inside a loop, it is best to fetch
 * the default before the loop and then passing the result explicitly
 * when you create the links.
 */
class DataProxyHolder
{
public:
  /// Type of hashed keys.
  typedef IStringPool::sgkey_t sgkey_t;

  /// Type of string keys.
  typedef std::string                ID_type;

  /// Generic pointer type.
  typedef void*                      pointer_t;
  typedef const void*                const_pointer_t;

  /// Function casting from a @c SG::DataProxy to a pointer.
  typedef void* castfn_t (SG::DataProxy*);

  /// Input renaming map.
  typedef std::unordered_map<SG::sgkey_t, SG::sgkey_t> InputRenameMap_t;
  typedef Athena::RCUObject<InputRenameMap_t> InputRenameRCU_t;


  /**
   * @brief Default constructor.
   *
   * Make a null link.
   */
  DataProxyHolder();


  /**
   * @brief Constructor from a holder referencing a different type.
   * @param other The object from which to copy.
   *
   * @c FROM_STORABLE is the storable class to which @c other refers;
   * @c TO_STORABLE is the storable class for this object.
   * The actual pointer values are not used, just the types are used.
   * Default conversions for the storable pointer (i.e., derived->base)
   * are allowed.
   */
  template <class FROM_STORABLE, class TO_STORABLE>
  DataProxyHolder (const DataProxyHolder& other, FROM_STORABLE*, TO_STORABLE*);


  // Try to avoid coverity warning.
  DataProxyHolder& operator= (const DataProxyHolder&) = default;


  /**
   * @brief Reset the link to null.
   */
  void clear();


  /**
   * @brief Set the link to an object given by a pointer.
   * @param obj Pointer to the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   * @returns The SG key for this object.
   *
   * This will try to look up the proxy for @c obj.  If that succeeds,
   * then we store the pointer to the proxy and return the SG key.
   * Otherwise, we store a pointer to the object itself, flagging
   * this case with the low bit, and return 0.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  sgkey_t toStorableObject (const_pointer_t obj,
                            CLID link_clid,
                            IProxyDict* sg);


  /**
   * @brief Set the link to an object given by a string key.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   * @returns The SG key for this object.
   *
   * This will try to look up the proxy for @c dataID.  If that succeeds,
   * then we store the pointer to the proxy.  Otherwise, we create
   * a dummy proxy and add it to the store.  We return the SG key
   * in either case.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  sgkey_t toIdentifiedObject (const ID_type& dataID,
                              CLID link_clid,
                              IProxyDict* sg);


  /**
   * @brief Set the link to an object given by a hashed key.
   * @param key Hashed key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   *
   * This will try to look up the proxy for @c key.  If that succeeds,
   * then we store the pointer to the proxy.  Otherwise, we create
   * a dummy proxy and add it to the store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  void toIdentifiedObject (sgkey_t key,
                           CLID link_clid,
                           IProxyDict* sg);


  /**
   * @brief Test to see if this is a null link.
   */
  bool isDefault() const;


  /**
   * @brief Return the SG key that we reference, as a string.
   *
   * Returns a null string on failure.
   */
  const ID_type& dataID() const;


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
  void* storableBase (castfn_t* castfn, CLID clid) const;



  /**
   * @brief Return the @c DataProxy for this link.
   * @param nothrow If true, return 0 on failure instead of throwing
   *                an exception.
   *
   * If this is a null link, we return 0.
   * Otherwise, if we're pointing at a @c DataProxy, return it.
   * Otherwise, we're pointing at an object directly.  Try to look up
   * the corresponding @c DataProxy using the default store.  Return it
   * if we find it; otherwise, either throw @c ExcPointerNotInSG or
   * return 0, depending on the @c nothrow parameter.
   */
  SG::DataProxy* proxy (bool nothrow = false) const;


  /**
   * @brief Return the data source for this reference.
   *
   * If the link is null, return 0.
   * If we're pointing at an object directly, then we return the default store
   * if the object is found in SG; otherwise, throw @c ExcPointerNotInSG.
   */
  IProxyDict* source() const;


  /**
   * @brief Finish initialization after link has been read.
   * @param sgkey Hashed SG key.
   * @param sg Associated store.
   *
   * This should be called after a link has been read by root
   * in order to set the proxy pointer.  It calls @c toIdentifiedObject
   * with the provided hashed key.
   *
   * If @c sg is 0, then we use the global default store.
   */
  void toTransient (sgkey_t sgkey, IProxyDict* sg = 0);


  /**
   * @brief Finish initialization after link has been read.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param sg Associated store.
   * @returns The hashed SG key for this object.
   *
   * This should be called after a link has been read by root
   * in order to set the proxy pointer.  It calls @c toIdentifiedObject
   * with the provided hashed key.
   *
   * If @c sg is 0, then we use the global default store.
   */
  sgkey_t toTransient (const ID_type& dataID,
                       CLID link_clid,
                       IProxyDict* sg = 0);


  /**
   * @brief Prepare this link for writing.
   * @param sgkey Reference to the hashed SG key.
   *
   * One of the @c toPersistent methods should be called before
   * trying to write the link with root.
   *
   * This takes a reference to the hashed SG key.  In the case where we're
   * referencing an object directly by pointer, the hashed key will be 0.
   * In that case, we try to look up the object in the default store.
   * If we find it, the hashed key is updated appropriately; otherwise,
   * we throw @c ExcPointerNotInSG.
   *
   * This version does not perform link remapping.
   */
  void toPersistentNoRemap (sgkey_t& sgkey);


  /**
   * @brief Prepare this link for writing.
   * @param sgkey Reference to the hashed SG key.
   * @param index Index of this link.
   *
   * One of the @c toPersistent methods should be called before
   * trying to write the link with root.
   *
   * This takes a reference to the hashed SG key.  In the case where we're
   * referencing an object directly by pointer, the hashed key will be 0.
   * In that case, we try to look up the object in the default store.
   * If we find it, the hashed key is updated appropriately; otherwise,
   * we throw @c ExcPointerNotInSG.
   *
   * If the target of the link has been remapped, then the @c sgkey and @c index
   * parameters will be updated to reflect that, and @c true will be returned.
   * Otherwise, if there was no remapping, then @c false will be returned.
   *
   * This version is for the case where indices are given by @c uint64_t.
   */
  bool toPersistent (sgkey_t& sgkey, uint64_t& index);


  /**
   * @brief Prepare this link for writing.
   * @param sgkey Reference to the hashed SG key.
   * @param index Index of this link.
   *
   * One of the @c toPersistent methods should be called before
   * trying to write the link with root.
   *
   * This takes a reference to the hashed SG key.  In the case where we're
   * referencing an object directly by pointer, the hashed key will be 0.
   * In that case, we try to look up the object in the default store.
   * If we find it, the hashed key is updated appropriately; otherwise,
   * we throw @c ExcPointerNotInSG.
   *
   * If the target of the link has been remapped, then the @c sgkey and @c index
   * parameters will be updated to reflect that, and @c true will be returned.
   * Otherwise, if there was no remapping, then @c false will be returned.
   *
   * This version is for the case where indices are given by @c uint32_t.
   */
  bool toPersistent (sgkey_t& sgkey, uint32_t& index);


  /**
   * @brief Prepare this link for writing.
   * @param sgkey Reference to the hashed SG key.
   * @param index Index of this link.
   *
   * One of the @c toPersistent methods should be called before
   * trying to write the link with root.
   *
   * This takes a reference to the hashed SG key.  In the case where we're
   * referencing an object directly by pointer, the hashed key will be 0.
   * In that case, we try to look up the object in the default store.
   * If we find it, the hashed key is updated appropriately; otherwise,
   * we throw @c ExcPointerNotInSG.
   *
   * This version is for the case where indices are not given by @c size_t.
   * No remapping will be performed for this case; this function will
   * always return false.
   */
  template <class T>
  bool toPersistent (sgkey_t& sgkey, const T& /*index*/);


  /**
   * @brief Adjust for thinning.
   * @param sgkey Reference to the hashed SG key.
   * @param index Index of this link.
   *
   * If this link points to a container that has been thinned,
   * @c sgkey and @c index will be adjusted accordingly.
   *
   * Returns @c true if the index was changed; @c false otherwise.
   */
  bool thin (sgkey_t& sgkey, size_t& index);


  /**
   * @brief Compare for equality.
   */
  bool operator== (const DataProxyHolder& other) const;


  /**
   * @brief Throw a @c ExcInvalidLink exception for this link.
   * @param sgkey The hashed key for this link.
   *
   * This will fill in parameters for the exception message from the proxy.
   */
  void throwInvalidLink (sgkey_t sgkey) const;


  /**
   * @brief Set map used for performing input renaming in toTransient.
   * @param map The new map, or nullptr for no renmaing.
   */
  static void setInputRenameMap ATLAS_NOT_THREAD_SAFE (const InputRenameRCU_t* map);


private:
  /**
   * @brief Test to see if we're pointing directly at an object.
   * @returns True if we're pointing directly at an object, false
   *          if we're pointing at a @c DataProxy (or the link is null).
   */
  bool isObjpointer() const;


  /**
   * @brief Return a pointer to the object we're pointing at directly.
   *
   * Should be used only if @c isObjpointer() is true.
   */
  pointer_t objpointer() const;


  /**
   * @brief Store a direct pointer to an object.
   * @param p Pointer to the object that we're referencing.
   *
   * This will overwrite @c m_proxy with the reference to the object.
   */
  void storeObjpointer (const_pointer_t p);


  /**
   * @brief Helper for @c proxy(), for the case of a direct object pointer.
   * @param nothrow If true, return 0 on failure instead of throwing
   *                an exception.
   *
   * This is the out-of-line portion of @c proxy(), called if this link
   * is directly pointing at an object.  Try to look up
   * the corresponding @c DataProxy using the default store.  Return it
   * if we find it; otherwise, either throw @c ExcPointerNotInSG or
   * return 0, depending on the @c nothrow parameter.
   */
  SG::DataProxy* proxy1 (bool nothrow) const;


  /**
   * @brief Return the data source for this reference.
   *
   * If we're holding a pointer directly, rather than a proxy,
   * then return 0 rather than raising an exception.
   */
  IProxyDict* source1() const;


  /**
   * @brief Test to see if the link has been remapped.
   * @param sgkey Reference to the hashed SG key.
   * @param index Index of this link.
   *
   * If this link has been remapped, @c sgkey and @c index will be
   * adjusted accordingly.
   *
   * Returns @c true if the link was remapped, @c false otherwise.
   */
  bool tryRemap (sgkey_t& sgkey, size_t& index);


  /// The @c DataProxy referring to our object, if the LSB is clear;
  /// pointer to the object which we're referencing directly if the LSB is set.
  SG::DataProxy* m_proxy; //! Transient
};


} // namespace SG


#include "AthLinks/tools/DataProxyHolder.icc"


#endif // not ATHLINKS_DATAPROXYHOLDER_H
