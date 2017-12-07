// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/ElementLinkBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2013
 * @brief Base class for ElementLinks to vectors of pointers.
 */


#ifndef ATHLINKS_ELEMENTLINKBASE_H
#define ATHLINKS_ELEMENTLINKBASE_H


#include "AthLinks/tools/DataProxyHolder.h"
#include "AthLinks/tools/ForwardIndexingPolicy.h"
#include "CxxUtils/CachedPointer.h"
#include "AthenaKernel/sgkey_t.h"
#include <cstdlib>
#include <stdint.h>


/**
 * @brief Base class for ElementLinks to vectors of pointers.
 *
 * This holds the type-independent portion of an @c ElementLink for the
 * case where the target container holds pointers and has vector-like indexing.
 * This is the most common case, and for this case, we can have
 * a non-templated class; this simplifies persistency.
 *
 * Other types of @c ElementLink will use instead @c GenericElementLinkBase.
 *
 * See ElementLinkTraits.h for a summary of the requirements for these
 * base classes.
 *
 * This class stores the hashed SG key of the target container and
 * the index of the element within the container.  (The type of the
 * index should be the same as in @c ForwardIndexingPolicy; however, we
 * don't depend on the indexing policy since that class is templated.)
 * Those two members make up the persistent state.  We also store
 * a transient @c DataProxyHolder, which encapsulates the pointer
 * to the @c DataProxy and factors out code that's common with @c DataLink.
 * Further, we store a copy of the element to which the link refers; this
 * is also transient (and mutable).  In the case of @c ElementLinkBase,
 * the element will always be a pointer, so we can store it as a generic
 * void*.
 */
class ElementLinkBase
{
private:
  /// Value to mark an invalid index.
  static const uint32_t INVALID = static_cast<uint32_t> (-1);

  /// Generic pointer to an element.
  typedef const void* ElementType;

  /// Generic pointer to the container (storable).
  typedef SG::DataProxyHolder::const_pointer_t const_pointer_t;


public:
  /// The index type presented to and returned from the link.
  typedef size_t index_type;

  /// The type of the index as it is stored internally.
  typedef uint32_t stored_index_type;

  /// Type of hashed keys.
  typedef SG::DataProxyHolder::sgkey_t sgkey_t;

  /// Type of string keys.
  typedef SG::DataProxyHolder::ID_type ID_type;

  /// Function casting from a @c SG::DataProxy to a pointer.
  typedef void* castfn_t (SG::DataProxy*);


  // Try to avoid coverity warning.
  ElementLinkBase& operator= (const ElementLinkBase&) = default;


  /**
   * @brief Test the index validity.
   * @returns True if the index is not valid (in default state).
   */
  bool isDefaultIndex() const;


  /**
   * @brief Test to see if this link has a cached element pointer.
   */
  bool hasCachedElement() const;


  /**
   * @brief Test to see if this link is in the default state.
   */
  bool isDefault() const;


  /**
   * @brief Return the index of the link.
   */
  index_type index() const;


  /**
   * @brief Return the index of the link.
   *        (Deprecated; use @c index() instead.)
   */
  stored_index_type persIndex() const;


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
   * @brief Return the SG key that we reference, as a hash.
   *        (Deprecated; use @c key instead.)
   *
   * Returns 0 on failure.
   */
  sgkey_t persKey() const;


  /**
   * @brief Return the data source for this reference.
   */
  IProxyDict* source() const;


  /**
   * @brief Return the SG proxy for the container holding this element.
   *
   * If this is a null link, we return 0.
   * If we're directly referencing an object that's not in StoreGate,
   * either return 0 or throw @c ExcPointerNotInSG, depending
   * on @c nothrow.  Otherwise, return the proxy for the object
   * we're referencing.
   */
  const SG::DataProxy* proxy() const;


  /**
   * @brief Reset the link to a null state.
   */
  void reset ();


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
   * @brief Prepare this link for writing.
   *
   * This method should be called before trying to write the link with root.
   * It will make sure the persistent fields of the link are valid.
   * It will also perform remapping if needed.
   *
   * Will return true on success.  Will return false if either the container
   * or the element index has not been specified.  Will throw
   * @c ExcPointerNotInSG if the link is referencing a container that
   * is not in SG.
   */
  bool toPersistent();


  /**
   * @brief Adjust for thinning.
   *
   * If this link points to a container that has been thinned,
   * it will be adjusted accordingly.
   *
   * Returns @c true if the link was changed; @c false otherwise.
   */
  bool thin();


  /**
   * @brief Default constructor.  Makes a null link.
   *
   * (Must be public for root i/o.)
   */
  ElementLinkBase();


protected:
  /**
   * @brief Construct a link from a string key and an index.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLinkBase (const ID_type& dataID,
                   CLID link_clid,
                   index_type elemID,
                   IProxyDict* sg);


  /**
   * @brief Construct a link from a hashed key and an index.
   * @param key Hashed key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLinkBase (sgkey_t key,
                   CLID link_clid,
                   index_type elemID,
                   IProxyDict* sg);



  /**
   * @brief Construct a link from a string key, index, AND pointer to element.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param elt Pointer to the element.
   * @param sg Associated store.
   * 
   * USE CAREFULLY: no coherency checks, we just trust you!
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLinkBase (const ID_type& dataID,
                   CLID link_clid,
                   index_type elemID,
                   const void* elt,
                   IProxyDict* sg);


  /**
   * @brief Construct a link from a hashed key, index, AND pointer to element.
   * @param key Hashed key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param elt Pointer to the element.
   * @param sg Associated store.
   * 
   * USE CAREFULLY: no coherency checks, we just trust you!
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLinkBase (sgkey_t key,
                   CLID link_clid,
                   index_type elemID,
                   const void* elt,
                   IProxyDict* sg);


  /**
   * @brief Construct a link from an index and pointer to the container.
   * @param obj Pointer to the container (storable).
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   * 
   * If @c sg is 0, we take the global default.
   */
  ElementLinkBase (const_pointer_t obj,
                   CLID link_clid,
                   index_type elemID,
                   IProxyDict* sg);


  /**
   * @brief Construct a link from another link, changing the index.
   * @param other The source link.
   * @param elemID The index for the new link.
   *
   * The index being constructed will reference the same container
   * as @c other, but it will refer to element @c elemID.
   */
  ElementLinkBase (const ElementLinkBase& other, index_type elemID);


  /**
   * @brief Constructor from a link referencing a different type.
   * @param other The object from which to copy.
   *
   * @c FROM_TRAITS is the @c ElementLinkTraits class for @c other;
   * @c TO_TRAITS is the traits class for this object.
   * The actual pointer values are not used, just the types are used.
   * Default conversions for the storable pointer (i.e., derived->base)
   * are allowed.
   */
  template <class FROM_TRAITS, class TO_TRAITS>
  ElementLinkBase (const ElementLinkBase& other,
                   FROM_TRAITS*, TO_TRAITS*);


  /**
   * @brief Return a pointer to the currently-referenced container object.
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
   * @brief Set the container referenced by the link to @c data.
   * @param data Pointer to the new container.
   * @param link_clid CLID of the link being set.
   * @param replace True if we can change an existing link.
   * @param sg Associated store.
   * @returns True if the link was changed.
   *
   * If the link is already set, this will return false and leave the
   * link unchanged unless @c replace is set.  The @c replace argument
   * should be set if the element is now in a new storable container;
   * e.g. element ptr has been put in a new view container.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  bool setStorableObject (const_pointer_t data,
                          CLID link_clid,
                          bool replace,
                          IProxyDict* sg);


  /**
   * @brief Set the link to an element given by index and pointer to container.
   * @param obj Pointer to the container (storable).
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   * @returns True if the link was changed.
   *
   * If the link is already set, this will return false and leave the
   * link unchanged.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  bool toIndexedElement (const_pointer_t obj,
                         CLID link_clid,
                         index_type elemID,
                         IProxyDict* sg);


  /**
   * @brief Set the link to an element given by string key and index.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void resetWithKeyAndIndex (const ID_type& dataID,
                             CLID link_clid,
                             index_type elemID, 
                             IProxyDict* sg);


  /**
   * @brief Set the link to an element given by string key and index.
   * @param key Hashed key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void resetWithKeyAndIndex (sgkey_t key,
                             CLID link_clid,
                             index_type elemID, 
                             IProxyDict* sg);


  /**
   * @brief Set the index part of the link.
   * @param index New index value.
   */
  void setIndex (index_type index);


  /**
   * @brief Return the stored representation of the link index.
   */
  const stored_index_type& storedIndex() const;


  /**
   * @brief Set the cached element stored in the link,
   *        assuming the cached element is null.
   * @param elt New value for the cached element.
   *
   * This method will set the current cached element if it is
   * currently unset.  If it has previously been assigned, then this
   * method will do nothing.
   */
  void setCachedElement (ElementType elt) const;


  /**
   * @brief Set the cached element stored in the link.
   * @param elt New value for the cached element.
   */
  void storeCachedElement (ElementType elt);


  /**
   * @brief Retrieve the cached element from the link.
   * @param elt[out] The cached element.
   * @returns True if an element was cached; false otherwise.
   *
   * @c elt is left unmodified if there is no cached element.
   */
  template <class T>
  bool getCachedElement (const T* & elt) const;


  /**
   * @brief Return the internal proxy holder object.
   */
  const SG::DataProxyHolder& proxyHolder() const;


private:
  /// For regression testing.
  friend class ElementLinkBase_test;

  /// Clear the currently-cached element.
  void clearCachedElement();

  /// The hashed key for this link.
  SG::sgkey_t m_persKey;

  /// The index of the element within the container for this link.
  stored_index_type m_persIndex;

  /// SG proxy for this link.
  SG::DataProxyHolder m_proxy;  //! Transient

  CxxUtils::CachedPointer  m_element;    //! Transient
};


#include "AthLinks/ElementLinkBase.icc"


#endif // not ATHLINKS_ELEMENTLINKBASE_H
