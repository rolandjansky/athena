// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenericElementLinkBase.h 785879 2016-11-22 22:17:10Z ssnyder $
/**
 * @file AthLinks/GenericElementLinkBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Generic base class for ElementLinks.
 */


#ifndef ATHLINKS_GENERICELEMENTLINKBASE_H
#define ATHLINKS_GENERICELEMENTLINKBASE_H


#include "AthLinks/tools/DataProxyHolder.h"
#include "AthLinks/tools/CachedValue.h"
#include "AthLinks/tools/selection_ns.h"
#include "CxxUtils/checker_macros.h"


// Forward declaration(s):
ENTER_ROOT_SELECTION_NS
namespace SG {
template< class INDEXING_POLICY >
struct GenericElementLinkBase;
}
EXIT_ROOT_SELECTION_NS


template <class POLICY>
class ElementLinkBaseT_test;


namespace SG {


/**
 * @brief Generic base class for ElementLinks.
 *
 * The part of @c ElementLink that doesn't directly depend on the particular
 * container class is factored out here.  We do, however, need to know
 * both the index and element types.  We get these from the indexing
 * policy (so we do have an indirect dependence on the container type).
 *
 * This class holds all the state of the @c ElementLink.  The persistent
 * part is the hashed SG key of the container and the index of the
 * element within the container.  We also store a transient @c DataProxyHolder,
 * which encapsulates the pointer to the @c DataProxy and factors out
 * code shared with @c DataLink.  Further, we store a (transient and mutable)
 * copy of the element to which this link refers, as well as a flag to tell
 * if the cached element is valid.
 *
 * The main reason for breaking out the base class like this is that
 * for the common case where the container has vector-like indexing
 * and the element is a pointer type we actually use the non-templated
 * class @c ElementLinkBase instead of this one.  @c ElementLinkBase
 * is essentially the same as this, but specialized for the case of
 * the index being an unsigned int and the element being a pointer.
 * This factorization makes direct ROOT persistency easier for this
 * common case.
 *
 * See ElementLinkTraits.h for a summary of the requirements for these
 * base classes.
 */
template <class INDEXING_POLICY>
class GenericElementLinkBase
{
private:
  /// Generic pointer to an element.
  typedef typename INDEXING_POLICY::ElementType ElementType;
  typedef typename INDEXING_POLICY::ElementConstPointer ElementConstPointer;

  /// Generic pointer to the container (storable).
  typedef SG::DataProxyHolder::const_pointer_t const_pointer_t;


public:
  /// The index type presented to and returned from the link.
  typedef typename INDEXING_POLICY::index_type index_type;

  /// The type of the index as it is stored internally.
  typedef typename INDEXING_POLICY::stored_index_type stored_index_type;

  /// Type of hashed keys.
  typedef SG::DataProxyHolder::sgkey_t sgkey_t;

  /// Type of string keys.
  typedef SG::DataProxyHolder::ID_type ID_type;

  /// Function casting from a @c SG::DataProxy to a pointer.
  typedef void* castfn_t (SG::DataProxy*);


public:
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


protected:
  /**
   * @brief Default constructor.  Makes a null link.
   */
  GenericElementLinkBase();


  /**
   * @brief Construct a link from a string key and an index.
   * @param dataID Key of the object.
   * @param link_clid CLID of the link being set.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   */
  GenericElementLinkBase (const ID_type& dataID,
                          CLID link_clid,
                          const index_type& elemID,
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
  GenericElementLinkBase (sgkey_t key,
                          CLID link_clid,
                          const index_type& elemID,
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
  GenericElementLinkBase (const ID_type& dataID,
                          CLID link_clid,
                          const index_type& elemID,
                          const ElementType& elt,
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
  GenericElementLinkBase (sgkey_t key,
                          CLID link_clid,
                          const index_type& elemID,
                          const ElementType& elt,
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
  GenericElementLinkBase (const_pointer_t obj,
                          CLID link_clid,
                          const index_type& elemID,
                          IProxyDict* sg);


  /**
   * @brief Construct a link from another link, changing the index.
   * @param other The source link.
   * @param elemID The index for the new link.
   *
   * The index being constructed will reference the same container
   * as @c other, but it will refer to element @c elemID.
   */
  GenericElementLinkBase (const GenericElementLinkBase& other,
                          const index_type& elemID);


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
  template <class OTHER_INDEXING_POLICY, class FROM_TRAITS, class TO_TRAITS>
  GenericElementLinkBase (const GenericElementLinkBase<OTHER_INDEXING_POLICY>& other,
                          FROM_TRAITS*, TO_TRAITS*);


  // Default assignment --- here just to keep coverity happy.
  GenericElementLinkBase& operator= (const GenericElementLinkBase&) = default;


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
  void* storableBase (castfn_t* castfn, const CLID& clid) const;


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
   * @brief Set the to an element given by index and pointer to container.
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
                         const index_type& elemID,
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
                             const index_type& elemID, 
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
                             const index_type& elemID, 
                             IProxyDict* sg);


  /**
   * @brief Set the index part of the link.
   * @param index New index value.
   */
  void setIndex (const index_type& index);


  /**
   * @brief Return the stored representation of the link index.
   */
  const stored_index_type& storedIndex() const;


  /**
   * @brief Set the cached element stored in the link.
   * @param elt New value for the cached element.
   */
  void setCachedElement (const ElementType& elt) const;


  /**
   * @brief Set the cached element stored in the link.
   * @param elt New value for the cached element.
   */
  void storeCachedElement (const ElementType& elt);


  /**
   * @brief Retrieve the cached element from the link.
   * @param elt[out] The cached element.
   * @returns True if an element was cached; false otherwise.
   *
   * @c elt is left unmodified if there is no cached element.
   */
  bool getCachedElement (ElementConstPointer& elt) const;


  /**
   * @brief Return the internal proxy holder object.
   */
  const SG::DataProxyHolder& proxyHolder() const;


private:
  template <class OTHER_POLICY>
  friend class GenericElementLinkBase;

  /// For regression testing.
  template <class POLICY>
  friend class ::ElementLinkBaseT_test;

  /// The hashed key for this link.
  SG::sgkey_t m_key;

  /// The index of the element within the container for this link.
  stored_index_type m_index;

  /// SG proxy for this link.
  SG::DataProxyHolder m_proxy;  //! Transient

  /// Cached element.
  CachedValue<ElementType> m_element; //! Transient

  
  /// Connect to selection rules.
  typedef typename
  ROOT_SELECTION_NS::SG::GenericElementLinkBase<INDEXING_POLICY>::self DictSel;
};


} // namespace SG


/// Selection rules: declare transient members.
ENTER_ROOT_SELECTION_NS
namespace SG {
template <class INDEXING_POLICY>
struct GenericElementLinkBase : SelectNoInstance {
  typedef GenericElementLinkBase<INDEXING_POLICY> self;
  ROOT_SELECTION_NS::TRANSIENT m_proxy;
  ROOT_SELECTION_NS::TRANSIENT m_cacheValid;
  ROOT_SELECTION_NS::TRANSIENT m_element;
};

}
EXIT_ROOT_SELECTION_NS


#include "AthLinks/GenericElementLinkBase.icc"


#endif // not ATHLINKS_GENERICELEMENTLINKBASE_H
