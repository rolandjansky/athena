// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxStoreInternal.h 793732 2017-01-24 19:42:30Z ssnyder $
/**
 * @file AthContainers/AuxStoreInternal.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief An auxiliary data store that holds data internally.
 */


#ifndef ATHCONTAINERS_AUXSTOREINTERNAL_H
#define ATHCONTAINERS_AUXSTOREINTERNAL_H


#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/tools/threading.h"
#include "CxxUtils/override.h"
#include <vector>
#include <memory>


namespace SG {


class IAuxTypeVector;


/**
 * @brief An auxiliary data store that holds data internally.
 *
 * When auxiliary data are associated with a container, the data themselves
 * are managed by a separate `store' object, described by the interfaces
 * @c SG::IAuxStore and @c SG::IConstAuxStore.
 *
 * This class is an implementation of those interfaces that stores the
 * data itself.  This is suitable, for example, for transient
 * auxiliary data.
 */
class AuxStoreInternal
   : public IAuxStore, public IAuxStoreIO
{
public:
  /**
   * @brief Constructor.
   * @param standalone If true, then write this in standalone mode.
   */
  AuxStoreInternal (bool standalone = false);


  /**
   * @brief Destructor.
   *
   * All contained data will be deleted.
   */
  virtual ~AuxStoreInternal();


  /**
   * @brief Copy constructor.
   */
  AuxStoreInternal (const AuxStoreInternal& orig);


  /// Don't allow assignment.
  AuxStoreInternal& operator= (const AuxStoreInternal&) = delete;
  

  /**
   * @brief Return the standalone flag.
   */
  bool standalone() const;


  /**
   * @brief Return the data vector for one aux data item
   * @param auxid The identifier of the desired aux data item.
   *
   * Each aux data item is stored as a vector, with one entry
   * per entry in the owning container.  This returns a pointer
   * to the start of the vector.
   *
   * This should return 0 if the item doesn't exist.
   */
  virtual const void* getData (SG::auxid_t auxid) const override;


  /**
   * @brief Return the data vector for one aux data item
   * @param auxid The identifier of the desired aux data item.
   * @param size The current size of the container (in case the data item
   *             does not already exist).
   * @param capacity The current capacity of the container (in case
   *                 the data item does not already exist).
   *
   * Each aux data item is stored as a vector, with one entry
   * per entry in the owning container.  This returns a pointer
   * to the start of the vector.
   *
   * If the data item does not exist, it should be created and initialized
   * to default values.  @c size and @c capacity give the size for the
   * new aux data item vector.
   */
  virtual void* getData (SG::auxid_t auxid, size_t size, size_t capacity)
    override;


  /**
   * @brief Return the data vector for one aux data decoration item.
   * @param auxid The identifier of the desired aux data item.
   * @param size The current size of the container (in case the data item
   *             does not already exist).
   * @param capacity The current capacity of the container (in case
   *                 the data item does not already exist).
   *
   * Each aux data item is stored as a vector, with one entry
   * per entry in the owning container.  This returns a pointer
   * to the start of the vector.
   *
   * If the data item does not exist, it then it will be created and initialized
   * with default values.  If the container is locked, then the new
   * item will be marked as a decoration.  @c size and @c capacity give
   * the size for the new aux data item vector.
   *
   * If the data item already exists, then we return it if either the
   * container is not locked or the item is marked as a decoration.
   * Otherwise we throw an exception.
   */
  virtual void* getDecoration (auxid_t auxid, size_t size, size_t capacity)
    override;


  /**
   * @brief Change the size of all aux data vectors.
   * @param sz The new size.
   *
   * This should be called when the size of the container changes.
   * This should resize the vectors for all aux data items.
   *
   * If the size of the container grows, the new elements should
   * be default-initialized; if it shrinks, destructors should
   * be run as appropriate.
   *
   * Should return @c true if it is known that none of the data pointers
   * changed (and thus the cache does not need to be cleared), false
   * otherwise.
   */
  virtual bool resize (size_t sz) override;


  /**
   * @brief Change the capacity of all aux data vectors.
   * @param sz The new capacity.
   *
   * This should be called when the capacity of the container changes
   * (by @c reserve).  This should change the capacity for the vectors
   * for all aux data items.
   */
  virtual void reserve (size_t sz) override;


  /**
   * @brief Shift the elements of the container.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * This operation shifts the elements in the vectors for all
   * aux data items, to implement an insertion or deletion.
   * @c offs may be either positive or negative.
   *
   * If @c offs is positive, then the container is growing.
   * The container size should be increased by @c offs,
   * the element at @c pos moved to @c pos + @c offs,
   * and similarly for following elements.
   * The elements between @c pos and @c pos + @c offs should
   * be default-initialized.
   *
   * If @c offs is negative, then the container is shrinking.
   * The element at @c pos should be moved to @c pos + @c offs,
   * and similarly for following elements.
   * The container should then be shrunk by @c -offs elements
   * (running destructors as appropriate).
   */
  virtual void shift (size_t pos, ptrdiff_t offs) override;


  /**
   * @brief Move all elements from @c other to this store.
   * @param pos The starting index of the insertion.
   * @param other Store from which to do the move.
   * @param ignore Set of variables that should not be added to the store.
   *
   * Let @c len be the size of @c other.  The store will be increased
   * in size by @c len elements, with the elements at @c pos being
   * copied to @c pos+len.  Then, for each auxiliary variable, the
   * entire contents of that variable for @c other will be moved to
   * this store at index @c pos.  This will be done via move semantics
   * if possible; otherwise, it will be done with a copy.  Variables
   * present in this store but not in @c other will have the corresponding
   * elements default-initialized.  Variables in @c other but not in this
   * store will be added unless they are in @c ignore.
   *
   * Returns true if it is known that none of the vectors' memory moved,
   * false otherwise.
   */
  virtual bool insertMove (size_t pos,
                           IAuxStore& other,
                           const SG::auxid_set_t& ignore = SG::auxid_set_t()) override;


  /**
   * @brief Return a set of identifiers for existing data items
   *        in this store.
   *
   *        This should include identifiers for all items,
   *        const and non-const.
   */
  virtual const SG::auxid_set_t& getAuxIDs() const override;


  /**
   * @brief Return a set of identifiers for writable data items
   *        in this store.
   *
   *        This should include only non-const identifiers.
   */
  virtual const SG::auxid_set_t& getWritableAuxIDs() const override;


  /**
   * @brief Return a pointer to the data to be stored for one aux data item.
   * @param auxid The identifier of the desired aux data item.
   *
   * This will usually be a pointer to a @c std::vector; however, it may
   * be something different for a standalone object.
   *
   * Returns 0 and reports an error if the requested aux data item
   * does not exist.
   */
  virtual const void* getIOData (SG::auxid_t auxid) const override;


  /**
   * @brief Return the type of the data to be stored for one aux data item.
   * @param auxid The identifier of the desired aux data item.
   *
   * For an aux data item of type @c T, this will usually be
   * @c std::vector<T>.  For standalone objects, however, it will
   * usually be @c T; and @c std::vector<char> will be used instead
   * of @c std::vector<bool>.
   *
   * Returns 0 if the requested aux data item does not exist.
   */
  virtual const std::type_info* getIOType (SG::auxid_t auxid) const override;


  /**
   * @brief Get the list of all variables that need to be handled.
   */
  virtual const SG::auxid_set_t& getDynamicAuxIDs() const override;


  /**
   * @brief Lock the container.
   *
   * After this, only decorations can be changed/modified.
   * If the container is already locked, this is a no-op.
   */
  virtual void lock() override;


  /**
   * @brief Clear all decorations.
   *
   * Erase all decorations from the store, restoring the state to when
   * @c lock was called.  Be sure to clear the cache of the referencing
   * container!
   *
   * Returns true if there were any decorations that were cleared,
   * false if the store did not contain any decorations.
   */
  virtual bool clearDecorations() override;


  /**
   * @brief Return the number of elements in the store.
   *
   * May return 0 for a store with no aux data.
   */
  virtual size_t size() const override;


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param id The variable for which we want to set the option.
   * @param option The option setting to make.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  virtual bool setOption (auxid_t id, const AuxDataOption& option) override;


  /**
   * @brief Lock a decoration.
   * @param auxid Identifier of the decoration to lock.
   *
   * A decoration is locked by changing from a decoration to an ordinary
   * variable.  If the container itself is locked, then modifications
   * to be variable are not permitted after this call.
   */
  virtual void lockDecoration (SG::auxid_t auxid) override;


protected:
  /**
   * @brief Return a pointer to the data to be stored for one aux data item.
   * @param auxid The identifier of the desired aux data item.
   * @param quiet If true, then don't print an error on failure.
   *
   * This will usually be a pointer to a @c std::vector; however, it may
   * be something different for a standalone object.
   *
   * Returns 0 and reports an error if the requested aux data item
   * does not exist.
   */
  const void* getIODataInternal (auxid_t auxid, bool quiet) const;


  /**
   * @brief Add a new auxid to the set of those being managed by this store.
   * @param auxid The auxid to add.
   */
  void addAuxID (auxid_t auxid);


  /**
   * @brief Return the data vector for one aux data item
   * @param auxid The identifier of the desired aux data item.
   * @param size The current size of the container (in case the data item
   *             does not already exist).
   * @param capacity The current capacity of the container (in case
   *                 the data item does not already exist).
   * @param no_lock_check If true, then skip the test for a locked container.
   *
   * Each aux data item is stored as a vector, with one entry
   * per entry in the owning container.  This returns a pointer
   * to the start of the vector.
   *
   * If the data item does not exist, it should be created and initialized
   * to default values.  @c size and @c capacity give the size for the
   * new aux data item vector.
   */
  virtual void* getDataInternal (SG::auxid_t auxid,
                                 size_t size,
                                 size_t capacity,
                                 bool no_lock_check);


  /**
   * @brief Explicitly add a vector to the store.
   * @param auxid The identifier of the aux data item being added.
   * @param vec Vector data being added.
   * @param isDecoration Should this variable be marked as a decoration?
   *
   * For internal use.  The @c auxid must not already exist in the store.
   */
  void addVector (SG::auxid_t auxid,
                  std::unique_ptr<IAuxTypeVector> vec,
                  bool isDecoration);


private:
  /// Implementation of getDataInternal; no locking.
  virtual void* getDataInternal_noLock (SG::auxid_t auxid,
                                        size_t size,
                                        size_t capacity,
                                        bool no_lock_check);

  /// Return the number of elements in the store; no locking.
  size_t size_noLock() const;

  /// Are we being written in standalone mode?
  bool m_standalone;

  /// The collection of vectors of aux data that we're managing,
  /// indexed by @c auxid.
  std::vector<IAuxTypeVector*> m_vecs;

  /// Record which variables are decorations.
  std::vector<bool> m_isDecoration;

  /// Set of @c auxid's for which we've created a vector.
  SG::auxid_set_t m_auxids;

  /// Count changes to @c m_auxids.
  size_t m_tick;

  /// Has this container been locked?
  bool m_locked;

  /// Mutex used to synchronize modifications to the cache vector.
  typedef AthContainers_detail::mutex mutex_t;
  typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
  mutable mutex_t m_mutex;

  /// Thread-local versions of the auxid set.
  struct TSAuxidSet
  {
    size_t m_tick;
    auxid_set_t m_set;
    TSAuxidSet (size_t tick, const auxid_set_t& set)
      : m_tick (tick), m_set (set) {}
  };
  mutable AthContainers_detail::thread_specific_ptr<TSAuxidSet> m_tsAuxids;
};


} // namespace SG



#ifndef XAOD_STANDALONE

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
SG_BASE( SG::AuxStoreInternal, SG::IAuxStore );
CLASS_DEF( SG::AuxStoreInternal , 16428258 , 1 )

#endif // not XAOD_STANDALONE
 

#endif // not ATHCONTAINERS_AUXSTOREINTERNAL_H
