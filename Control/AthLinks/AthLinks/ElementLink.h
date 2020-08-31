// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElementLink.h 714258 2015-12-12 04:18:16Z ssnyder $
/**
 * @file AthLinks/ElementLink.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2013
 * @brief a persistable pointer to an element of a STORABLE (data object)
 */


#ifndef ATHLINKS_ELEMENTLINK_H
#define ATHLINKS_ELEMENTLINK_H


#include "AthLinks/tools/ElementLinkTraits.h"
#include "AthLinks/DataLink.h"
#include "AthenaKernel/IProxyDict.h"
#include <utility>


/**
 * @class ElementLink
 * @brief a persistable pointer to an element of a STORABLE (data object)
 * @param STORABLE        host object type (e,g. vector<Elem>, NOT Elem),
 *
 * An @c ElementLink maintains a reference to a container that can
 * be in StoreGate (a la @c DataLink) and an index within that container.
 * This then identifies an element within the container.
 *
 * The preferred way to initialize an @c ElementLink is with the StoreGate
 * key and index.  However, the link may also be initialized using the
 * container directly instead of the key, or with the element directly.
 * In the latter case, the reference to the container can be specified later.
 * Be aware, though, that if you initialize the link from the element
 * directly, without the index, the container will have to be searched
 * if the index is needed, which could be slow.  Also, if the index
 * and StoreGate key are not available (for example, the container
 * wasn't recorded in StoreGate), then the link will not be persistable.
 *
 * The relationship between the container and the index is encapsulated
 * by an `indexing policy' class.  You normally don't need to worry
 * about this, as the appropriate policy is automatically chosen
 * for STL sequence types as well as for STL set and map types.
 * However, it is possible to customize the policy used, if needed.
 * See the macros in DeclareIndexingPolicy.h; the interface
 * required of an indexing policy is summarized in tools/ElementLinkTraits.h.
 *
 * Forward declarations:
 *
 * Dealing with forward declarations has been a problem.  For example,
 * you have a class @c MyCont which is a vector of pointers to @c MyElt,
 * where @c MyElt contains an @c ElementLink<MyCont>.  Here is a way
 * to get this to work.
 *
 * In your header for @c MyElt, make a forward declaration for @c MyCont.
 * Then include the special macro @c ELEMENTLINK_FWD:
 *
 *@code
 *  ELEMENTLINK_FWD(MyCont, MyElt);
 @endcode
 *
 * where the first argument is the container type and the second argument
 * is the base type of the (pointer) value type of the container.
 * You can then write a data member
 * of type @c ElementLink<MyCont>.  You will need to have the complete
 * definition of @c MyCont available in order to actaully call methods
 * on the link, so you'll likely need to include the header for @c MyCont
 * within the implementation file for @c MyElt.  A more complete example
 * of what the header for @c MyElt might look like:
 *
 *@code
 *  #include "AthLinks/ElementLink.h"
 *
 *  class MyElt;
 *  class MyCont;
 *  ELEMENTLINK_FWD(MyCont, MyElt);
 *
 *  class MyElt { ...
 *    ElementLink<MyCont> m_link;
 *  };
 @endcode
 *
 * Restrictions:
 *  - This only works for containers that are vector-like and which
 *    contain pointers (@c DataVector works.) 
 *  - @c MyCont can't be a typedef.  In most cases, @c MyCont will
 *    have to be a class that derives from the actual container class.
 *  - The @c ELEMENTLINK_FWD macro must be used in the global scope
 *    (outside of any namespace).
 *  - The complete definition of the container must be visible
 *    before calling any methods on the link.  The container header
 *    can be included from the implementation file for the element;
 *    however, this implies that anything touching the link cannot
 *    be written as inline code in the element header.
 *
 * Implementation notes:
 *
 * Internally, we store a hashed SG key, a @c DataProxyHolder instance
 * to encapsulate access to the @c DataProxy, the index, and a cached
 * copy of the element to which we refer.  These last two are in general
 * of arbitrary type.  However, the vast majority of EL instantiations
 * refer to vectors of pointers; in that case, we can get away fixed
 * types for these (an unsigned for the index and a void* for the cached
 * element). Since it makes ROOT I/O easier if our state is in
 * a non-templated class, we move all the state of @c ElementLink
 * into a base class.  The particular type of base class is chosen
 * by @c ElementLinkTraits: the non-templated class @c ElementLinkBase
 * is used for the vector of pointers case, while the templated
 * @c GenericElementLinkBase class is used for the general case.
 */
template <class STORABLE>
class ElementLink
  : public SG::ElementLinkTraits<STORABLE>::Base
{
private:
  /// Traits class, from which to derive types.
  typedef SG::ElementLinkTraits<STORABLE> Traits;

  /// Base class (factoring out code that doesn't directly depend on STORABLE).
  typedef typename Traits::Base Base;

  /// Function casting from a @c SG::DataProxy to a pointer.
  typedef typename Base::castfn_t castfn_t;


public:
  /// The indexing policy.
  typedef typename Traits::IndexingPolicy IndexingPolicy;

  /// The index type presented to and returned from the link.
  typedef typename IndexingPolicy::index_type index_type; 

  /// Type of hashed keys.
  typedef typename Base::sgkey_t sgkey_t;

  /// Type of string keys.
  typedef typename Base::ID_type ID_type;

  /// Type of container (storable) and derived types.
  typedef STORABLE                      value_type;
  typedef STORABLE*                     pointer;
  typedef const STORABLE*               const_pointer;
  typedef const STORABLE& BaseConstReference;
  typedef const STORABLE* BaseConstPointer;


  /// Container element and derived types.
  typedef typename IndexingPolicy::ElementType ElementType;
  typedef typename IndexingPolicy::ElementConstReference ElementConstReference;
  typedef typename IndexingPolicy::ElementConstPointer ElementConstPointer;


  /**
   * @brief Return the CLID for the class that we reference.
   */
  static const CLID& classID();


  //========================================================================
  /** @name Constructors. */
  //@{


  /**
   * @brief Default constructor.  Makes a null link.
   */
  ElementLink();


  /**
   * @brief Construct a link from a string storable key and an index.  O(1)
   * @param dataID Key of the object.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLink(const ID_type& dataID, index_type elemID,
              IProxyDict* sg = 0);


  /**
   * @brief Construct a link from a hashed storable key and an index.  O(1)
   * @param key Hashed key of the object.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLink(sgkey_t key, index_type elemID, IProxyDict* sg = 0);


  /**
   * @brief Construct from a string storable key, index, AND pointer to element.  O(1)
   * @param dataID Key of the object.
   * @param elemID The index of the element within the container.
   * @param pEl Pointer to the element.
   * @param sg Associated store.
   * 
   * USE CAREFULLY: no coherency checks, we just trust you!
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLink(const ID_type& dataID,
              index_type elemID,
              ElementType pEl,
              IProxyDict* sg = 0);


  /**
   * @brief Construct from a hashed storable key, index, AND pointer to element.  O(1)
   * @param key Hashed key of the object.
   * @param elemID The index of the element within the container.
   * @param pEl Pointer to the element.
   * @param sg Associated store.
   * 
   * USE CAREFULLY: no coherency checks, we just trust you!
   *
   * If @c sg is 0, we take the global default.
   */
  ElementLink (sgkey_t key,
               index_type elemID,
               ElementType pEl,
               IProxyDict* sg = 0);


  /**
   * @brief Construct a link from an index and reference to the container. O(1)
   * @param data Reference to the container (storable).
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   * 
   * If @c sg is 0, we take the global default.
   */
  ElementLink (BaseConstReference data,
               index_type elemID, 
               IProxyDict* sg = 0);


  /**
   * @brief Construct from an element and reference to the container. O(N)
   * @param element The element to reference.
   * @param data Reference to the container (storable).
   * @param sg Associated store.
   *
   * Does the same thing as the default ctor followed by @c toContainedElement.
   * Note the reversed parameter order compared to the previous
   * constructor.  This is to prevent ambiguities in the case that
   * the contained type is convertable to an int.
   *
   * Will throw @c SG::ExcElementNotFound if the element is not
   * in the container.
   * 
   * If @c sg is 0, we take the global default.
   */
  ElementLink (const ElementType& element, 
               BaseConstReference data,
               IProxyDict* sg = 0);


  // Use default copy ctor.


  /**
   * @brief Constructor allowing a derived -> base conversion.
   */
  template <class U>
  ElementLink (const ElementLink<U>& other);


  //@)
  //========================================================================
  /** @name Dereferencing the link. */
  //@{


  /**
   * @brief Return a pointer to the currently-referenced container object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  const_pointer getDataPtr() const;


  /**
   * @brief Return a pointer to the currently-referenced container object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  pointer getDataNonConstPtr();


  /**
   * @brief Return a link to the currently-referenced container object.
   */
  DataLink<STORABLE> getDataLink();


  /**
   * @brief Return a link to the currently-referenced container object.
   */
  const DataLink<STORABLE> getDataLink() const;


  /**
   * @brief Return a pointer to the currently-referenced container object.
   */
  BaseConstPointer getStorableObjectPointer() const;


  /**
   * @brief Return a reference to the currently-referenced container object.
   */
  BaseConstReference getStorableObjectRef() const;


  /**
   * @brief Return a pointer to the referenced element.
   *
   * Be aware: if the element is a pointer, then this will yield
   * a pointer to a pointer.
   */
  ElementConstPointer cptr() const;


  /**
   * @brief Return a reference to the referenced element.
   *
   * Will throw an exception if the link is not valid.
   */
  ElementConstReference operator* () const;


  /**
   * @brief Return a pointer to the referenced element.
   */
  ElementConstPointer operator->() const;


  /**
   * @brief Convert to a pointer to the referenced element.
   */
  operator ElementConstPointer () const;


  /**
   * @brief Test to see if the link can be dereferenced.
   */
  bool isValid() const;


  /**
   * @brief Test to see if the link can not be dereferenced.
   */
  bool operator!() const;


  /**
   * @brief Return the cached element, if any.
   */
  ElementType cachedElement() const;


   /**
    * @brief Is this link invalid because its target was removed (e.g. through
    * slimming or thinning in the derivation framework)
    */
   bool wasTargetRemoved() const;


  //@)
  //========================================================================
  /** @name Modifying the link. */
  //@{


  /**
   * @brief Set the link to an element given by index and pointer to container.
   * @param data Reference to the container (storable).
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
  bool toIndexedElement(BaseConstReference data, index_type elemID,
                        IProxyDict* sg = 0);


  /**
   * @brief Set from element pointer and a reference to the container (storable)
   * @param data Reference to the container (storable).
   * @param element The element.
   * @param sg Associated store.
   * @returns True if the link was changed.
   *
   * O(N) for sequences!
   *
   * If the link is already set, this will return false and leave the
   * link unchanged.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  bool toContainedElement(BaseConstReference data,
                          ElementType element,
                          IProxyDict* sg = 0);


  /**
   * @brief Set to point to an element.
   * @param element The element.
   * @returns True if the link was changed.
   *
   * The collection and the element can be specified independently
   * using @c setElement and @c setStorableObject.
   *
   * If the link is already set, this will return false and leave the
   * link unchanged.
   */
  bool setElement(ElementType element);


  /**
   * @brief Set link to point to a new container (storable).
   * @param data Reference to the container (storable).
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
  bool setStorableObject(BaseConstReference data,
                         bool replace=false,
                         IProxyDict* sg = 0);


  /**
   * @brief Set the link to an element given by string key and index.
   * @param dataID Key of the object.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void resetWithKeyAndIndex(const ID_type& dataID, index_type elemID, 
                            IProxyDict* sg=0);


  /**
   * @brief Set the link to an element given by string key and index.
   * @param key Hashed key of the object.
   * @param elemID The index of the element within the container.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void resetWithKeyAndIndex(sgkey_t key, index_type elemID, 
                            IProxyDict* sg=0);


  //@}


  // Inherited from base class:
  //  bool isDefaultIndex() const
  //   Test the index validity.
  //  bool hasCachedElement() const
  //   Test to see if this link has a cached element pointer.
  //  bool isDefault() const
  //   Test to see if this link is in the default state.
  //  index_type index() const
  //   Return the index of the link.
  //  ID_type dataID() const
  //   Return the SG key that we reference, as a string.
  //  sgkey_t key() const
  //   Return the SG key that we reference, as a hash.
  //  IProxyDict* source() const
  //   Return the data source for the reference.
  //  void reset()
  //   Reset the link to a null state.
  //  bool toTransient (IProxyDict*)
  //   Finish initialization after link has been read.
  //  bool doPersistent()
  //   Prepare this link for writing.


private:
  /**
   * @brief Return a (void) pointer to the currently-referenced
   *        container object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  const void* storable() const;


  /**
   * @brief Return a (void) pointer to the currently-referenced
   *        container object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  void* storableNonConst();
};


/**
 * @brief Ordering relation for @c ElementLink (less-than)
 * @param lhs Left-hand-side EL.
 * @param rhs Right-hand-side EL.
 */
template <typename STORABLE>
bool operator < (const ElementLink<STORABLE>& lhs,
                 const ElementLink<STORABLE>& rhs);


/**
 * @brief Ordering relation for @c ElementLink (greater-than)
 * @param lhs Left-hand-side EL.
 * @param rhs Right-hand-side EL.
 */
template <typename STORABLE>
bool operator > (const ElementLink<STORABLE>& lhs,
		 const ElementLink<STORABLE>& rhs);


/**
 * @brief Equality relation for @c ElementLink.
 * @param lhs Left-hand-side EL.
 * @param rhs Right-hand-side EL.
 */
template <typename STORABLE>
bool operator == (const ElementLink<STORABLE>& lhs,
		  const ElementLink<STORABLE>& rhs);


/**
 * @brief Inequality relation for @c ElementLink.
 * @param lhs Left-hand-side EL.
 * @param rhs Right-hand-side EL.
 */
template <typename STORABLE>
bool operator != (const ElementLink<STORABLE>& lhs,
		  const ElementLink<STORABLE>& rhs);


namespace SG_detail {


/**
 * @brief See if an EL is being remapped.
 * @param sgkey_in Original hashed key of the EL.
 * @param index_in Original index of the EL.
 * @param sgkey_out[out] New hashed key for the EL.
 * @param index_out[out] New index for the EL.
 * @return True if there is a remapping; false otherwise.
 *
 * This version is for the case where the EL index is a @c size_t.
 * For other index types, the the templated version below is used
 * (which doesn't allow remapping indices).
 */
inline
bool checkForRemap (IProxyDict* sg,
                    SG::sgkey_t sgkey_in,
                    size_t index_in,
                    SG::sgkey_t& sgkey_out,
                    size_t& index_out);


/**
 * @brief See if an EL is being remapped.
 * @param sgkey_in Original hashed key of the EL.
 * @param dum_in Ignored.
 * @param sgkey_out[out] New hashed key for the EL.
 * @param dum_out[out] Ignored.
 * @return True if there is a remapping; false otherwise.
 *
 * This version catches the cases where the container index type
 * isn't a @c size_t.  We don't support changing the index in this case.
 */
template <class T>
inline
bool checkForRemap (IProxyDict* sg,
                    SG::sgkey_t sgkey_in,
                    const T& /*dum_in*/,
                    SG::sgkey_t& sgkey_out,
                    T& /*dum_out*/);


} // namespace SG_detail


namespace std {


/**
 * @brief Specialization for a @c pair containing an @c ElementLink,
 *        allowing for faster initialization.
 *
 * [c++11 has another way of doing this --- `piecewise construction'.
 *  The way you use that is not the same, however; so we'll keep
 *  this in place for now.]
 *
 * When @c ElementLink is used with the @c Navigable class, we make
 * lists of @c std::pair<ElementLink<T1>, T2>.  To create one
 * of these pairs, one would generally do something like:
 *
 *@code
 *  pairtype (ElementLink<T1> (container, index), par)
 @endcode
 *
 * This, however, requires superfluous calls to the @c ElementLink
 * constructor and destructor: first we create a temporary @c ElementLink,
 * then we copy-construct the @c ElementLink within the @c pair from it,
 * and finally we destroy the temporary.  In certain cases, this
 * extra ctor/dtor pair can have a significant effect on performance
 * (this was observed for building cells into towers).  It would be
 * faster if we could pass the container and index args directly
 * into the @c pair's constructor; then we wouldn't need to create
 * the temporary:
 *
 *@code
 *  pairtype (container, index, par)
 @endcode
 *
 * The class below is a specialization of @c std::pair for the case
 * where the first type is an @c ElementLink which adds such a constructor.
 * Everything else is a copy of the standard contents of @c std::pair.
 *
 * (For those nervous about the appearance here of `namespace std',
 * note that specializing a library class within the @c std namespace
 * is explicitly allowed by the C++ standard, provided that it's specialized
 * on a user-defined type.)
 */
template <typename CONT, typename T2>
struct pair<ElementLink<CONT>, T2>
{
  // Standard pair types.
  typedef ElementLink<CONT> first_type;
  typedef T2 second_type;

  // Standard pair members.
  first_type  first;
  second_type second;

  // Standard pair constructors.
  pair() : first(), second() {}
  pair (const first_type& x, const second_type& y) : first (x), second (y) {}
  template <class U, class V>
  pair (const pair<U, V>& p) : first (p.first), second (p.second) {}

  // Helper type.
  typedef typename SG::GenerateIndexingPolicy<CONT>::type::index_type
    external_index_type;

  // Extra constructor to allow avoiding a temporary for the
  // @c ElementLink initialization.
  pair (const CONT& child_container,
        const external_index_type& index,
        const T2& y)
    : first (child_container, index), second (y)
  {}

  // Extra constructor to allow avoiding a temporary for the
  // @c ElementLink initialization.
  pair (const CONT& child_container,
        const external_index_type& index,
        IProxyDict* sg,
        const T2& y)
    : first (child_container, index, sg), second (y)
  {}

  void swap (pair& p)
  {
    std::swap (first, p.first);
    std::swap (second, p.second);
  }
};


} // namespace std


#include "AthLinks/ElementLink.icc"


#endif // not ATHLINKS_ELEMENTLINK_H
