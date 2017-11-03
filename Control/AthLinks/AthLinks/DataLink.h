// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataLink.h 783590 2016-11-11 00:38:09Z ssnyder $
/**
 * @file AthLinks/DataLink.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Object reference supporting deferred reading from StoreGate.
 */


#ifndef ATHLINKS_DATALINK_H
#define ATHLINKS_DATALINK_H


#include "AthLinks/DataLinkBase.h"
#include "SGTools/ClassID_traits.h"
#include "AthenaKernel/DefaultKey.h"
#include "GaudiKernel/EventContext.h"
class IProxyDict;


/**
 * @brief Object reference supporting deferred reading from StoreGate.
 *
 * This is a kind of smart pointer that can reference objects in StoreGate
 * by key.  This allows deferred-reading semantics, and also allows references
 * to objects that may be in a different physical data file (`back-navigation').
 *
 * For example:
 *
 *@code
 *   DataLink<MyClass> link ("mykey");
 *   ...
 *   // Look up the object `mykey' of class MyClass in StoreGate.
 *   const MyClass* obj = *link;
 @endcode
 *
 * You can also initialize a link directly with a pointer:
 *
 *@code
 *   MyClass* obj = ...;
 *   DataLink<MyClass> link (obj);
 @endcode
 *
 * However, if the object has not been recorded in the event store,
 * the possible operations on the link will be limited.  (In particular,
 * such a link cannot be made persistent.)
 *
 * A @c DataLink can exist in one of three states:
 *
 * - A null reference.
 * - Referring to an object in StoreGate.  In this case, the link refers
 *   to the @c DataProxy for the referred-to object.
 * - Referring to an object directly by pointer.  When you initialize
 *   a link from a pointer, the link will try to find the corresponding
 *   proxy.  However, if one doesn't exist, the link will remember the pointer
 *   directly.  If a subsequent operation is attempted that requires
 *   the proxy (such as @c toPersistent or getting the key) and the object
 *   still isn't in StoreGate, @c SG::ExcPointerNotInSG will be thrown.
 *
 * Internally, a link stores a hashed version of the SG key + CLID;
 * this is the part that's persistent.  (In the case of a link referencing
 * an object not in SG, this field is 0.)  There is also a transient
 * pointer to either the proxy or the object itself (if the object is not
 * in SG).  This latter pointer is factored out as @c DataProxyHolder
 * in order to share implementation with @c ElementLink.  Further, everything
 * which does not depend on the @c STORABLE template parameter, including
 * all the object state, is factored out into the non-templated base class
 * @c DataLinkBase.
 *
 * If the link is being saved directly with ROOT, then @c toPersistent
 * should be called prior to writing.  This will ensure that the
 * hashed StoreGate key is set correctly (and will raise an exception
 * if the link is referencing an object not in SG).  This will also
 * handle any remapping of the link.  When a link is read directly
 * from ROOT, @c toTransient should be called to set the proxy pointer.
 * This would normally be done via a ROOT read rule.
 *
 * A link references objects in a particular event store.
 * When a link is constructed, a global default store is used
 * (see SGTools/CurrentEventStore.h).  If a link is subsequently modified,
 * it stays associated with the same store, unless the link was null
 * or was referencing an object directly by pointer, in which case
 * the global default is again used.  Alternatively, the event
 * store may be passed explicitly using the optional @c sg parameter.
 */
template <class STORABLE>
class DataLink
  : public DataLinkBase
{
public:
  /// Names for the referenced type and derivatives.
  typedef STORABLE                      value_type;
  typedef STORABLE*                     pointer;
  typedef const STORABLE*               const_pointer;
  typedef STORABLE&                     reference;
  typedef const STORABLE&               const_reference;

  /// A string SG key.
  typedef DataLinkBase::ID_type ID_type;

  /// A hashed SG key.
  typedef DataLinkBase::sgkey_t sgkey_t;


  /**
   * @brief Return the CLID for the class that we reference.
   */
  static const CLID& classID();


  //========================================================================
  /** @name Constructors. */
  //@{


  /**
   * @brief Default constructor --- gives the equivalent of a NULL pointer.
   */
  DataLink();


  /**
   * @brief Constructor --- link to a STORABLE using a transient ref to it.
   * @param data The object to which to link.
   * @param sg Associated store; if 0, use the global default.
   */
  DataLink(const_reference data, IProxyDict* sg=0);


  /**
   * @brief Constructor --- link to a STORABLE using a transient ref to it.
   * @param data The object to which to link.
   * @param ctx Event context for this link.
   */
  DataLink(const_reference data, const EventContext& ctx);


  /**
   * @brief Constructor --- link to a STORABLE using a transient pointer to it.
   * @param data The object to which to link.
   * @param sg Associated store; if 0, use the global default.
   */
  DataLink(const_pointer pdata, IProxyDict* sg=0);


  /**
   * @brief Constructor --- link to a STORABLE using a transient pointer to it.
   * @param data The object to which to link.
   * @param ctx Event context for this link.
   */
  DataLink(const_pointer pdata, const EventContext& ctx);


  /**
   * @brief Constructor --- link to a STORABLE using a string ID.
   * @param dataID Key of the object.
   * @param sg Associated store; if 0, use the global default.
   */
  DataLink(const ID_type& dataID, IProxyDict* sg=0);


  /**
   * @brief Constructor --- link to a STORABLE using a string ID.
   * @param dataID Key of the object.
   * @param ctx Event context for this link.
   */
  DataLink(const ID_type& dataID, const EventContext& ctx);


  /**
   * @brief Constructor --- link to a STORABLE using a hashed ID.
   * @param key Hashed key of the object.
   * @param sg Associated store; if 0, use the global default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  DataLink(sgkey_t key, IProxyDict* sg=0);


  /**
   * @brief Constructor --- link to a STORABLE using a hashed ID.
   * @param key Hashed key of the object.
   * @param ctx Event context for this link.
   *
   * May throw @c ExcCLIDMismatch.
   */
  DataLink(sgkey_t key, const EventContext& ctx);


  /**
   * @brief Constructor from a hashed key and a proxy holder object.
   *        Used internally for EL -> DL conversion.
   * @param key Hashed key of the object.
   * @param holder Internal holder object for the proxy.
   */
  DataLink(sgkey_t key, const SG::DataProxyHolder& holder);


  //@}
  //========================================================================
  /** @name Setters. */
  //@{


  /**
   * @brief Set the link to an object given by a reference.
   * @param data The object to which to link.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void toStorableObject(const_reference data, IProxyDict* sg = 0);


  /**
   * @brief Set the link to an object given by a reference.
   * @param data The object to which to link.
   * @param ctx Event context for this link.
   */
  void toStorableObject(const_reference data, const EventContext& ctx);


  /**
   * @brief Set the link to an object given by a string key.
   * @param dataID Key of the object.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   */
  void toIdentifiedObject(const ID_type& dataID, IProxyDict* sg = 0);


  /**
   * @brief Set the link to an object given by a string key.
   * @param dataID Key of the object.
   * @param ctx Event context for this link.
   */
  void toIdentifiedObject(const ID_type& dataID, const EventContext& ctx);


  /**
   * @brief Set the link to an object given by a hashed key.
   * @param key Hashed key of the object.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   *
   * May throw @c ExcCLIDMismatch.
   */
  void toIdentifiedObject(sgkey_t key, IProxyDict* sg = 0);


  /**
   * @brief Set the link to an object given by a hashed key.
   * @param key Hashed key of the object.
   * @param ctx Event context for this link.
   *
   * May throw @c ExcCLIDMismatch.
   */
  void toIdentifiedObject(sgkey_t key, const EventContext& ctx);


  /**
   * @brief Set the link to the default object of this type.
   * @param sg Associated store.
   *
   * If @c sg is 0, then we take the store from whatever the link's currently
   * set to.  If the link has no current store, then we take the global
   * default.
   *
   * Note that this is _not_ the same as clearing the link
   * (use @c clear for that).  This produces a link that will resolve
   * to any object in SG of the given type, provided that there is only
   * one of them.  (An attempt to dereference an ambiguous default link
   * will give an error.)
   */
  void toDefaultObject (IProxyDict* sg = 0);


  /**
   * @brief Set the link to the default object of this type.
   * @param ctx Event context for this link.
   *
   * Note that this is _not_ the same as clearing the link
   * (use @c clear for that).  This produces a link that will resolve
   * to any object in SG of the given type, provided that there is only
   * one of them.  (An attempt to dereference an ambiguous default link
   * will give an error.)
   */
  void toDefaultObject (const EventContext& ctx);


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
   * @param sg Associated store.
   *
   * The link should be clear before this is called.
   * Returns true.
   *
   * If @c sg is 0, then we use the global default store.
   */
  bool toTransient (const ID_type& dataID, IProxyDict* sg = 0);
  

  /**
   * @brief Finish initialization like the link as just been read from root,
   *        but with a specified key.
   * @param dataID Key of the object.
   * @param ctx Event context for this link.
   *
   * The link should be clear before this is called.
   * Returns true.
   */
  bool toTransient (const ID_type& dataID, const EventContext& ctx);
  


  //@}
  //========================================================================
  /** @name Accessors. */
  //@{

  /**
   * @brief Return a pointer to the currently-referenced object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  const_pointer getDataPtr() const;


  /**
   * @brief Return a pointer to the currently-referenced object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  pointer getDataNonConstPtr();


  /**
   * @brief Dereference the link.
   */
  const_reference operator* () const;


  /**
   * @brief Dereference the link.
   */
  const_pointer operator->() const;


  /**
   * @brief Dereference the link.
   */
  operator const_pointer() const;


  /**
   * @brief Dereference the link.
   */
  const_pointer cptr() const;


  /**
   * @brief Test to see if the link is dereferencable.
   */
  bool isValid() const; 



  /**
   * @brief True if the link is not dereferencable.
   */
  bool operator!() const;


  // Inherited from base class:
  //   bool isDefault() const;
  //   const ID_type& dataID() const;
  //   sgkey_t key() const;
  //   void clear();
  //   SG::DataProxy* proxy (bool nothrow = false) const;
  //   IProxyDict* source() const;
  //   bool toPersistent();
  //   bool toPersistentNoRemap();
  //   bool operator== (const DataLinkBase& other) const;
  //   bool operator!= (const DataLinkBase& other) const;


private:
  /**
   * @brief Return a (void) pointer to the currently-referenced object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  const void* storable() const;


  /**
   * @brief Return a (void) pointer to the currently-referenced object.
   * @return A pointer to an object of the type given by @a clid,
   *         or null on a failure (or if the reference is null).
   */
  void* storableNonConst();
};


#include "AthLinks/DataLink.icc"


#endif // not ATHLINKS_DATALINK_H
