// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxVectorData.h 793052 2017-01-19 15:05:28Z ssnyder $
/**
 * @file AthContainers/AuxVectorData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Manage lookup of vectors of auxiliary data.
 */


#ifndef ATHCONTAINERS_AUXVECTORDATA_H
#define ATHCONTAINERS_AUXVECTORDATA_H


#include "AthContainersInterfaces/AuxTypes.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthContainers/tools/AuxDataTraits.h"
#include "AthLinks/DataLink.h"
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/ILockable.h"
#   include "AthenaKernel/IThinningSvc.h"
#endif // not XAOD_STANDALONE
#include <vector>
#include <utility>
#include <cstdlib>

#include "AthContainers/tools/likely.h"
#include "AthContainers/tools/assume.h"
#include "AthContainers/tools/threading.h"
#include "CxxUtils/checker_macros.h"


namespace SG {


class AuxElement;
class IAuxStore;
class AuxDataOption;


/**
 * @brief Manage lookup of vectors of auxiliary data.
 *
 * An object, usually a @c DataVector, can have vectors of auxiliary
 * data associated with it.  This class manages this association.
 *
 * An auxiliary data item is identified by an integer of type
 * @c SG::auxid_t.  The @c getData methods can be used to get
 * a reference to one auxiliary data element given the @c auxid
 * and the vector index.  However, @c getData does not do type checking,
 * so it should generally not be used.  (Use instead the @c Accessor
 * or @c ConstAccessor classes defined in @c AuxElement.)
 *
 * The auxiliary data is not managed by this class, but rather by
 * a separate `aux store' class, to which we hold a pointer.
 * Actually, there can be two pointers.  We define two interfaces
 * for an aux store, @c IConstAuxStore, which defines operations
 * for accessing data read-only, and @c IAuxStore, which defines
 * operations for modifying data.  If we have a const store,
 * only the pointer to the const interface is set; if we have
 * non-const store, then both pointers are set (to the same object).
 *
 * To speed up access to aux data, we cache pointers to the start
 * of the data for each vector.  There are separate caches
 * for const and non-const pointers.  If you make any changes
 * to the aux store behind the back of this container object,
 * you should call @c clearCache.
 *
 * We also support adding `decorations' to a const container.  These are
 * new auxiliary data items that don't conflict with existing ones.
 * See IConstAuxStore for more information.
 *
 * Notes on thread safety:
 *
 * It's a little tricky to make this class thread-safe without spoiling
 * the optimizations in getDataArray.  This section outlines some of the
 * considerations that went into the chosen solution.
 *
 * First, by `thread-safe', we mean that getDataArray can be called
 * in different threads without problems.  This is necessary to allow
 * simultaneous reads of the container.  We make no attempt to any
 * synchronization on modifications to the container, such as adding elements.
 * Such operations must be synchronized externally.  This is the same
 * sort of thread semantics that the STL containers supply.  So our
 * considerations of thread-safety involve only the management
 * of the cache vector.
 *
 * Second, reads (of the cache vector) are very common (and inlined),
 * while modifications of it are uncommon (and handled by out-of-line code).
 * Thus, we would like reading to be entirely lock-free.  If we need
 * to make modifications, though, we can do whatever locking we need.
 * Making the reader lock-free, though, is complicated by the fact
 * that the cache vector may relocate in memory if is expanded.
 *
 * A way forward is suggested by read-copy-update (RCU) synchronization.
 * The idea there is that when you want to change some structure,
 * you copy it and work on the copy.  When the modifications are done,
 * the new structure is copied to the old one in such a manner that
 * at any instant in time, any reader will see a consistent version
 * of the structure (even though it may not be the most recent one).
 *
 * For this case, we can store the vector as a length and a pointer
 * to the beginning.  When we want to access AUXID, we first
 * compare it to the length.  If that's ok, then we test the
 * pointer at index AUXID.  If that's non-null, we go ahead and use it;
 * if either test fails, we go to the out-of-line code.
 *
 * The out-of-line code can then take out a lock and will in the new
 * pointer in the vector.  If it is necessary to expand the the vector,
 * we allocate a new one and copy the old vector to the new one.
 * Then we update the values: first, the pointer, then the length.
 * This ensures that the inline code will always see something
 * consistent.  Then we must delay freeing the old vector until we're
 * sure that no thread can possibly be using it anymore.  For now,
 * we just avoid deleting the old vectors until the container itself
 * is deleted; the memory wasted by this should be negligible in the
 * context of reconstruction.
 *
 * This allows the inline part of the code to avoid locking.  However,
 * there is an additional critical detail.  We have a test like this:
 *
 *  m_cache_length <= auxid || !m_cache[auxid]
 *
 * As long as the length is read before the cache pointer itself,
 * everything's fine, even if those reads were some time in the past.
 * But if the reads can be in the other order, we could face disaster.
 * While the short-circuit operator should prevent the array indexing
 * from happening before the length is read, there is nothing a priori
 * to prevent a speculative read of m_cache before the length.  For the
 * cognoscenti, this is a `control dependency' (rather than a `data dependency'),
 * which implies no ordering guarantees.
 *
 * Now, we can deal with this by inserting a read barrier between the two loads.
 * That should be correct in all cases.  However, that tends to destroy
 * the optimization below for repeated references to the same aux data item
 * (see the use of @c ATHCONTAINERS_ASSUME in @c getDataArray in the icc file).
 *
 * It turns out that on x86 machines, memory ordering guarantees are relatively
 * strong.  In particular, loads cannot be reordered with other loads,
 * and stores from one CPU are seen in the same order by all other CPUs.
 * So in this case, no barrier is actually needed --- provided that the
 * compiler emits the loads in the correct order.  The supported way
 * to do this with gcc is to use `asm volatile ("":::"memory")' --- however,
 * that explicitly clobbers member, which again spoils our optimization.
 *
 * While it seems unlikely that the compiler would actually find it worthwhile
 * to reorder the loads on an x86 machine, some extra safety would be nice.
 * We try to prevent this reordering by adding an explicit data dependency.
 * Instead of a single m_cache pointer, we have an array of two pointers
 * (which will be identical) and use m_cache[m_cache_len&1].  This
 * provides an explicit data dependency which should prevent reading the
 * pointer before the length; the cost is an added and operation and adding
 * an index register to the dereference operation.
 *
 * Actually, this is not completely watertight; the compiler could in principle
 * decide to speculate the reads of both pointers, or speculate one and
 * then throw it away if it guessed wrong.  This seems sufficiently unlikely
 * to be an issue that we'll live with it for now --- though it might
 * be worth having something to validate the generated code.
 */
class AuxVectorData
#ifndef XAOD_STANDALONE
  : public ILockable
#endif // not XAOD_STANDALONE
{
public:
  /// Constructor.
  AuxVectorData();


  /**
   * @brief Move constructor.
   * @param rhs The container from which to move.
   */
  AuxVectorData (AuxVectorData&& rhs);


  /**
   * @brief Move assignment.
   * @param rhs The container from which to move.
   */
  AuxVectorData& operator= (AuxVectorData&& rhs);


  /// Destructor.
  virtual ~AuxVectorData();


  /**
   * @brief Return the size of the container.
   *
   * This is used when we need to create a new aux data vector.
   */
  virtual size_t size_v() const = 0;


  /**
   * @brief Return the capacity of the container.
   *
   * This is used when we need to create a new aux data vector.
   */
  virtual size_t capacity_v() const = 0;


  //========================================================================
  /** @name Aux store management. */
  //@{


  /**
   * @brief Return the current store, as a const interface.
   *
   * This will be non-zero if either a const or non-const store
   * is associated with this object.
   */
  const SG::IConstAuxStore* getConstStore() const;


  /**
   * @brief Return the data link to the current store, as a const interface.
   *
   * This is set by persistency when reading an object, but it may
   * be overridden by setting the store pointer directly.
   */
  const DataLink<SG::IConstAuxStore> getConstStoreLink() const;


  /**
   * @brief Return the current store, as a non-const interface.
   *
   * This will be non-zero if a non-const store is associated with this object.
   */
  SG::IAuxStore* getStore();


  /**
   * @brief Return the current store, as a non-const interface.
   *
   * This will be non-zero if a non-const store is associated with this object.
   */
  const SG::IAuxStore* getStore() const;


  /**
   * @brief Return true if this object has an associated store.
   */
  bool hasStore() const;


  /**
   * @brief Return true if this object has an associated non-const store.
   */
  bool hasNonConstStore() const;


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param id The variable for which we want to set the option.
   * @param option The option setting to make.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  bool setOption (auxid_t id, const AuxDataOption& option);


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param name The name of the variable.
   * @param option The option setting to make.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  bool setOption (const std::string& name,
                  const AuxDataOption& option);


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param name The name of the variable.
   * @param clsname The name of the associated class.  May be blank.
   * @param option The option setting to make.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  bool setOption (const std::string& name,
                  const std::string& clsname,
                  const AuxDataOption& option);


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param id The variable for which we want to set the option.
   * @param optname The name of the option to set.
   * @param arg The option value to set.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  template <class T>
  bool setOption (auxid_t id, const std::string& optname, T arg);


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param name The name of the variable.
   * @param optname The name of the option to set.
   * @param arg The option value to set.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  bool setOption (const std::string& name,
                  const std::string& optname,
                  int arg);
  bool setOption (const std::string& name,
                  const std::string& optname,
                  float arg);
  bool setOption (const std::string& name,
                  const std::string& optname,
                  double arg);


  /**
   * @brief Set an option for an auxiliary data variable.
   * @param name The name of the variable.
   * @param clsname The name of the associated class.  May be blank.
   * @param optname The name of the option to set.
   * @param arg The option value to set.
   *
   * The interpretation of @c option depends on the associated auxiliary store.
   * See PackedParameters.h for option settings for writing packed data.
   * Returns @c true on success, @c false otherwise.
   */
  template <class T>
  bool setOption (const std::string& name,
                  const std::string& clsname,
                  const std::string& optname,
                  T arg);



protected:
  // These are protected.  They shouldn't be called directly but
  // instead from @c setStore methods in derived classes, which do
  // extra consistency checking.


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * This will clear the non-const store pointer, and also
   * clear the cache.
   */
  void setStore (const SG::IConstAuxStore* store);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * This will set both the const and non-const store pointers, and also
   * clear the cache.
   */
  void setStore (SG::IAuxStore* store);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * This will clear the non-const store pointer, and also
   * clear the cache.
   */
  void setStore (const DataLink< SG::IConstAuxStore >& store);


  //@}
  //========================================================================
  /** @name Data access. */
  //@{
public:


  /**
   * @brief Return a set of identifiers for existing data items
   *        in store associated with this object.
   *
   *        This will include identifiers for all items,
   *        const and non-const.  If no store is associated
   *        with this object, this will return an empty set.
   */
  const SG::auxid_set_t& getAuxIDs() const;


  /**
   * @brief Return a set of identifiers for writable data items
   *        in this store.
   *
   *        This will include only non-const identifiers.
   *        If no store is associated
   *        with this object, this will return an empty set.
   */
  const SG::auxid_set_t& getWritableAuxIDs() const;


  /**
   * @brief Test to see if a variable exists in the store.
   * @param id The variable to test.
   */
  bool isAvailable (auxid_t id) const;


  /**
   * @brief Test to see if a variable exists in the store.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   */
  template <class T>
  bool isAvailable (const std::string& name,
                    const std::string& clsname = "") const;


  /**
   * @brief Test to see if a variable is available for writing.
   * @param id The variable to test.
   */
  bool isAvailableWritable (auxid_t id);


  /**
   * @brief Test to see if a variable is available for writing.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   */
  template <class T>
  bool isAvailableWritable (const std::string& name,
                            const std::string& clsname = "");


  /**
   * @brief Test to see if a variable is available for writing as a decoration.
   * @param id The variable to test.
   */
  bool isAvailableWritableAsDecoration (auxid_t id) const;


  /**
   * @brief Test to see if a variable is available for writing as a decoration.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   */
  template <class T>
  bool isAvailableWritableAsDecoration (const std::string& name,
                                        const std::string& clsname = "") const;


  /**
   * @brief Return reference to an aux data item.
   * @param auxid The desired aux data item.
   * @param ndx Index of the element to return.
   *
   * This will return a reference to element @c ndx of aux data item @c auxid.
   * If the aux data item does not exist, it will be created.
   * Errors are signaled by raising an exception.
   *
   * Warning: no type checking is done.  You should usually access
   * the data via @c AuxElement::Accessor or @c AuxElement::ConstAccessor.
   */
  template <class T>
  typename AuxDataTraits<T>::reference_type
  getData (SG::auxid_t auxid, size_t ndx);


  /**
   * @brief Return const reference to an aux data item.
   * @param auxid The desired aux data item.
   * @param ndx Index of the element to return.
   *
   * This will return a reference to element @c ndx of aux data item @c auxid.
   * Errors are signaled by raising an exception.
   *
   * Warning: no type checking is done.  You should usually access
   * the data via @c AuxElement::Accessor or @c AuxElement::ConstAccessor.
   */
  template <class T>
  typename AuxDataTraits<T>::const_reference_type
  getData (SG::auxid_t auxid, size_t ndx) const;


  /**
   * @brief Return reference to an aux decoration item.
   * @param auxid The desired aux decoration item.
   * @param ndx Index of the element to return.
   *
   * This will return a reference to element @c ndx of aux decoration
   * item @c auxid.
   * If the aux data item does not exist, it will be created.
   * Errors are signaled by raising an exception.
   *
   * Warning: no type checking is done.  You should usually access
   * the data via @c AuxElement::Decorator.
   *
   * The difference between @c getDecoration and @c getData is that
   * @c getDecoration takes a const container as input, but returns
   * a non-const reference.  This will only succeed if either the
   * container is not locked or the item was first accessed
   * as a decoration.
   */
  template <class T>
  typename AuxDataTraits<T>::reference_type
  getDecoration (SG::auxid_t auxid, size_t ndx) const;


  /**
   * @brief Return a const pointer to the start of an aux data vector.
   * @param auxid The desired aux data item.
   *
   * This will return a pointer to the start of the data for aux
   * data item @c auxid.
   * Errors are signaled by raising an exception.
   */
  const void* getDataArray (SG::auxid_t auxid) const;


  /**
   * @brief Return a const pointer to the start of an aux data vector.
   * @param auxid The desired aux data item.
   *
   * This will return a pointer to the start of the data for aux
   * data item @c auxid.  If the item does not exist,
   * this will return nullptr rather than raising an exception.
   */
  const void* getDataArrayAllowMissing (SG::auxid_t auxid) const;


  /**
   * @brief Return a pointer to the start of an aux data vector for a decoration.
   * @param auxid The desired aux data item.
   *
   * This will return a pointer to the start of the data for aux
   * data item @c auxid.  If the item doesn't exist, it will be created.
   * Errors are signaled by raising an exception.
   *
   * The difference between @c getDecorationArray and @c getDataArray is that
   * @c getDecorationArray takes a const container as input, but returns
   * a non-const pointer.  This will only succeed if either the
   * container is not locked or the item was first accessed
   * as a decoration.
   */
  void* getDecorationArray (SG::auxid_t auxid) const;


protected:
  /**
   * @brief Return a pointer to the start of an aux data vector.
   * @param auxid The desired aux data item.
   *
   * This will return a pointer to the start of the data for aux
   * data item @c auxid.  If the item doesn't exist, it will be created.
   * Errors are signaled by raising an exception.
   */
  void* getDataArray (SG::auxid_t auxid);


  /// Minimum length to use for the cache vector.
  /// Only changed by unit tests.
  static size_t s_minCacheLen ATLAS_THREAD_SAFE;


  //@}
  //========================================================================
  /** @name Other operations. */
  //@{
public:


  /**
   * @brief Swap this instance with another.
   * @param other The other instance with which to swap.
   */
  void swap (AuxVectorData& other);


  /**
   * @brief Clear the cached aux data pointers.
   *
   * You should call this any time something changes in the aux store
   * that could invalidate the vector pointers.
   */
  void clearCache();


  /**
   * @brief Lock the container.
   *
   * After this, only decorations can be changed/modified.
   * If the container is already locked, this is a no-op.
   */
#ifndef XAOD_STANDALONE
  virtual
#endif // not XAOD_STANDALONE
          void lock()
#ifndef XAOD_STANDALONE
                      override
#endif // not XAOD_STANDALONE
     ;


  /**
   * @brief Clear all decorations.
   *
   * Erase all decorations from the store, restoring the state to when
   * @c lock was called.
   *
   * Returns true if there were any decorations that were cleared,
   * false if the store did not contain any decorations.
   */
  bool clearDecorations() const;


  //@}



private:
  /**
   * @brief Manage cache of pointers to aux element vectors.
   *
   * See the thread-safety comments at the start of this file
   * for notes on what's going on here.
   */
  class Cache
  {
  public:
    /**
     * @brief Cache manager constructor.
     */
    Cache();


    /**
     * @brief Cache manager move constructor.
     * @param rhs The cache from which to copy.
     */
    Cache (Cache&& rhs);


    /**
     * @brief Cache manager move assignment.
     * @param rhs The cache from which to copy.
     */
    Cache& operator= (Cache&& rhs);


    /**
     * @brief Cache manager destructor.
     */
    ~Cache();


    /**
     * @brief Test to see if @c auxid is valid in the cache.
     * @returns If @c auxid is valid, return the pointer to the vector, else 0.
     */
    void* cachePtr (SG::auxid_t auxid);


    /**
     * @brief Return a pointer to the start of an aux data vector.
     * @param auxid The desired aux data item.
     * @param parent The containing @c AuxVectorData object.
     *
     * This will return a pointer to the start of the data for aux
     * data item @c auxid.  If the item doesn't exist, it will be created.
     * Errors are signaled by raising an exception.
     */
    void* getDataArray (SG::auxid_t auxid, AuxVectorData& parent);


    /**
     * @brief Return a const pointer to the start of an aux data vector.
     * @param auxid The desired aux data item.
     * @param parent The containing @c AuxVectorData object.
     *
     * This will return a pointer to the start of the data for aux
     * data item @c auxid.
     * Errors are signaled by raising an exception.
     */
    const void* getDataArray (SG::auxid_t auxid, const AuxVectorData& parent);

    
    /**
     * @brief Return a const pointer to the start of an aux data vector.
     * @param auxid The desired aux data item.
     * @param parent The containing @c AuxVectorData object.
     *
     * This will return a pointer to the start of the data for aux
     * data item @c auxid.  If the item does not exist,
     * this will return nullptr rather than raising an exception.
     */
    const void* getDataArrayAllowMissing (SG::auxid_t auxid,
                                          const AuxVectorData& parent);

    
    /**
     * @brief Return a pointer to the start of an aux decoration vector.
     * @param auxid The desired aux decoration item.
     * @param parent The containing @c AuxVectorData object.
     *
     * This will return a pointer to the start of the data for aux
     * decoration item @c auxid.  If the item doesn't exist, it will be created.
     * Errors are signaled by raising an exception.
     *
     * The difference between @c getDecorationArray and @c getDataArray is that
     * @c getDecorationArray takes a const container as input, but returns
     * a non-const pointer.  This will only succeed if either the
     * container is not locked or the item was first accessed
     * as a decoration.
     */
    void* getDecorationArray (SG::auxid_t auxid, const AuxVectorData& parent);


    /**
     * @brief Swap this cache object with another.
     * @param other The cache object with which to swap.
     */
    void swap (Cache& other);


    /**
     * @brief Clear the cache (and free any old cache vectors).
     */
    void clear();


    /**
     * @brief Store a pointer for @c auxid in the cache.
     * @param auxid The aux data item being stored.
     * @param ptr Pointer to the start of the aux vector for @c auxid.
     */
    void store (SG::auxid_t auxid, void* ptr);


    /// Pointer to the cache vector.
    /// The two pointers here are the same; see the thread-safety
    /// discussion in the file header above.
    void** m_cache[2];

    /// Length of the cache vector.
    size_t m_cache_len;

    /// All cache vectors that have been allocated.
    // See the thread-safety discussion in the file header above.
    // Could be a vector of unique_ptr, but we want to avoid
    // relying on c++11 for now.
    std::vector<void**> m_allcache;


  private:
    Cache (const Cache&);
    Cache& operator= (const Cache&);
  };

  friend class Cache;
  

  /// Copy not allowed.
  AuxVectorData (const AuxVectorData&);
  AuxVectorData& operator= (const AuxVectorData&);

  // Needed for access to getDataArray
  friend class SG::AuxElement;


  /**
   * @brief Out-of-line portion of isAvailable.
   * @param id The variable to test.
   */
  bool isAvailableOol (auxid_t id) const;


  /**
   * @brief Out-of-line portion of isAvailableWritable.
   * @param id The variable to test.
   */
  bool isAvailableWritableOol (auxid_t id);


  /**
   * @brief Out-of-line portion of isAvailableWritableAsDecoration.
   * @param id The variable to test.
   */
  bool isAvailableWritableAsDecorationOol (auxid_t id) const;


  /**
   * @brief Out-of-line portion of data access.
   * @param auxid aux data item being accessed.
   * @param allowMissing If true, then return nullptr if the variable
   *                     is missing rather than throwing an exception.
   *
   * When this function returns, the cache entry @c m_cache[auxid]
   * will be valid.  That entry is also returned.  If there's an error,
   * the function will throw  an exception rather than returning.
   */
  void* getDataOol (SG::auxid_t auxid, bool allowMissing);


  /**
   * @brief Out-of-line portion of data access (const version).
   * @param auxid aux data item being accessed.
   * @param allowMissing If true, then return nullptr if the variable
   *                     is missing rather than throwing an exception.
   *
   * When this function returns, the cache entry @c m_constCache[auxid]
   * will be valid.  That entry is also returned.  If there's an error,
   * the function will throw  an exception rather than returning.
   */
  const void* getDataOol (SG::auxid_t auxid, bool allowMissing) const;


  /**
   * @brief Out-of-line portion of data access (decorator version).
   * @param auxid aux data item being accessed.
   *
   * When this function returns, the cache entry @c m_cache[auxid]
   * will be valid.  That entry is also returned.  If there's an error,
   * the function will throw  an exception rather than returning.
   *
   * The difference between @c getDecorationOol and @c getDataOol is that
   * @c getDecorationOol takes a const container as input, but returns
   * a non-const pointer.  This will only succeed if either the
   * container is not locked or the item was first accessed
   * as a decoration.
   */
  void* getDecorationOol (SG::auxid_t auxid) const;


  /// Cached pointers to the start of aux data vectors, non-const.
  mutable Cache m_cache ATLAS_THREAD_SAFE;

  /// Cached pointers to the start of aux data vectors, const.
  mutable Cache m_constCache ATLAS_THREAD_SAFE;

  /// Cached pointers to the start of aux data vectors, decorations.
  mutable Cache m_decorCache ATLAS_THREAD_SAFE;


  /// Associated store, non-const.
  SG::IAuxStore* m_store;


  /// Associated store, const.
  const SG::IConstAuxStore* m_constStore;


  /// Associated store link, const.
  DataLink< SG::IConstAuxStore > m_constStoreLink;


  /// Mutex used to synchronize modifications to the cache vector.
  typedef AthContainers_detail::mutex mutex_t;
  typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
  mutable mutex_t m_mutex;

  /// Empty auxid set, used for a return value when we have no associated store.
  static const SG::auxid_set_t s_emptySet;
};


} // namespace SG


#ifndef XAOD_STANDALONE


/**
 * @brief Propagate thinning.  Called after applying thinning to @c in.
 * @param in The object that was thinned.
 * @param svc The thinning service (for convenience).
 * @param filter Lists the elements to be kept.
 * @param op How to merge results: AND or OR.
 *
 * This will be called whenever a request is made to thin a @c DataVector.
 *
 * Otherwise, we pass the thinning request on to the aux store, provided
 * that it exists and is in the event store.
 */
StatusCode thinningHook (const SG::AuxVectorData* in,
                         IThinningSvc* svc,
                         const IThinningSvc::Filter_t& filter,
                         const IThinningSvc::Operator::Type op );
                         

#endif


#include "AthContainers/AuxVectorData.icc"


#endif // not ATHCONTAINERS_AUXVECTORDATA_H
