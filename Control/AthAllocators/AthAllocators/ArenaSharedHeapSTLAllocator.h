// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/ArenaSharedHeapSTLAllocator.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief STL-style allocator wrapper for @c ArenaHeapAllocator allowing
 *        the heap to be shared between containers.
 *
 * This class defines a STL-style allocator for types @c T with the
 * following special properties.
 *
 *  - We use an @c ArenaHeapAllocator for allocations.
 *  - Only one object at a time may be allocated.
 *  - The memory from several allocators may be grouped
 *    together in the same memory pool.
 *
 * So, this allocator is suitable for an STL container which allocates
 * lots of fixed-size objects, such as @c std::list, and further,
 * you want to be able to have multiple list instances use
 * the same pool.
 *
 * Note that this allocator will not work for containers that
 * make variable-sized allocations, such as vector and the
 * hash table containers.
 *
 * To use it, you should first explicitly create the allocator class,
 * and then pass it to the constructors of the containers.
 * The memory pool will be deleted when the original allocator
 * instance is deleted.  Example:
 *
 *@code
 *  {
 *    typedef SG::ArenaSharedHeapSTLAllocator<int> alloc_t;
 *    typedef std::list<int> list_t;
 *
 *    alloc_t allocator;
 *    list_t list1 (allocator);
 *    list_t list2 (allocator);
 *    ... Now list1 and list2 will both use the same memory pool.
 *  }
 *  ... The memory pool is freed when the object `allocator' is deleted.
 @endcode
 *
 * Implementation: Each allocator references a Header object,
 * which is common to all allocators in the pool.  When an allocator
 * is copied, the header pointer is copied too.  The Header object
 * remembers the address of the allocator which originally created it,
 * so that we can clean things up when that allocator goes away.
 *
 * A Header contains a list of ArenaHeapAllocator objects, one per
 * payload type.  We use ArenaAllocatorRegistry to assign indices
 * to the different allocator types.
 */


#ifndef ATLALLOCATORS_ARENASHAREDHEAPSTLALLOCATOR_H
#define ATLALLOCATORS_ARENASHAREDHEAPSTLALLOCATOR_H


#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaHeapSTLAllocator.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "CxxUtils/concepts.h"
#include <string>
#include <vector>


namespace SG {

/// Forward declaration.
template <class T>
class ArenaSharedHeapSTLAllocator;


/**
 * @brief Common header class for ArenaSharedHeapSTLAllocator.
 *
 * Each ArenaSharedHeapSTLAllocator has a pointer to a Header class.
 * When a new ASHSTLAllocator object is constructed, we make a new Header;
 * after that, the Header pointer is copied on copy-construction.
 * The Header remembers the address of the object that created it,
 * so that it can be deleted when that allocator is.
 *
 * The Header contains a list of ArenaHeapAllocator objects, one for
 * each type we're allocating.
 */
class ArenaSharedHeapSTLHeader
{
public:
  /**
   * @brief Constructor.
   * @param owner Address of the object that owns this Header.
   * @param nblock Value to set in the parameters structure for the
   *               number of elements to allocate per block.
   */
  ArenaSharedHeapSTLHeader (const void* owner, int nblock);


  /**
   * @brief Destructor.
   *
   * Destroy all the allocators we own.
   */
  ~ArenaSharedHeapSTLHeader();


  /**
   * @brief Call this when an allocator is being deleted.
   * @param a The address of calling allocator.
   *
   * If the address matches the address we were given when we were created,
   * this object will be destroyed.
   */
  void maybe_delete (const void* a);


  /**
   * @brief Return allocator statistics summed over all our owned allocators.
   */
  ArenaAllocatorBase::Stats totstats() const;
      

  /**
   * @brief Return the name to use for an allocator for type @c T.
   */
  template <class T>
  static std::string get_name();
    

  /**
   * @brief Return the heap allocator for type @c T.
   */
  template <class T>
  ArenaHeapAllocator* get_pool();


  /**
   * @brief Update the owner of this object.
   * @param old_owner Object giving up ownership.
   * @param new_owner Object acquiring ownership.
   *
   * If the current owner is @c old_owner then change the owner to @c new_owner.
   */
  void update_owner (const void* old_owner, const void* new_owner);


  /**
   * @brief Generate printable report for all contained allocators.
   * @param os Stream to which to write the report.
   */
  void report (std::ostream& os) const;


  /**
   * @brief Write-protect the memory managed by these allocators.
   *
   * Adjust protection on the memory managed by these allocators
   * to disallow writes.
   */
  void protect();


  /**
   * @brief Write-enable the memory managed by these allocators.
   *
   * Adjust protection on the memory managed by these allocators
   * to allow writes.
   */
  void unprotect();


private:
  /**
   * @brief Return the allocator index to use for type @c T.
   */
  template <class T>
  static size_t get_index();

  /// Address of the allocator that created this header.
  const void* m_owner;

  /// Saved value for nblock parameter.
  size_t m_nblock;

  /// List of allocators.
  std::vector<ArenaHeapAllocator*> m_allocators;
};




/**
 * @brief STL-style allocator wrapper for @c ArenaHeapAllocator allowing
 *        the heap to be shared between containers.
 *
 * See the file-level comments for details.
 */
template <class T>
class ArenaSharedHeapSTLAllocator
{
public:
  /// Make all instantiations of this class friends
  /// (to be able to copy the header pointer).
  template <class U> friend class ArenaSharedHeapSTLAllocator;

  /// Standard STL allocator typedefs.
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef T&        reference;
  typedef const T&  const_reference;
  typedef T         value_type;
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;

  /// When we assign to a container, the target should retain its allocator.
  typedef std::false_type propagate_on_container_copy_assignment;

  /// Move allocators on move/swap.
  typedef std::true_type propagate_on_container_move_assignment;
  typedef std::true_type propagate_on_container_swap;

  
  /// Standard STL allocator rebinder.
  template <class U> struct rebind {
    typedef ArenaSharedHeapSTLAllocator<U> other;
  };


  /**
   * @brief Default constructor.
   * @param nblock Value to set in the parameters structure for the
   *               number of elements to allocate per block.
   */
  ArenaSharedHeapSTLAllocator (size_t nblock = 1000);


  /**
   * @brief Copy constructor.
   *
   * The new STL allocator will reference the same set of underlying
   * Arena allocators as the old one.
   */
  ArenaSharedHeapSTLAllocator (const ArenaSharedHeapSTLAllocator& a);


  /// Move constructor is the same as the copy constructor.
  ArenaSharedHeapSTLAllocator (ArenaSharedHeapSTLAllocator&& a) = default;


  /**
   * @brief Constructor from another @c ArenaSharedHeapSTLAllocator.
   *
   * The new STL allocator will reference the same set of underlying
   * Arena allocators as the old one.
   */
  template <class U>
  ArenaSharedHeapSTLAllocator (const ArenaSharedHeapSTLAllocator<U>& a);


  // We don't bother to supply a more general constructor --- shouldn't
  // be needed.


  /**
   * @brief Destructor.
   */
  ~ArenaSharedHeapSTLAllocator();


  /**
   * @brief Assignment.
   *
   * We allow assignment only if the two objects involved represent
   * the same arena, in which case it's a no-op.
   * In other cases, we raise an exception.
   */
  ArenaSharedHeapSTLAllocator&
  operator= (const ArenaSharedHeapSTLAllocator& a);


  /**
   * @brief Move assignment.
   *
   * This allows assignment between different arenas.
   */
  ArenaSharedHeapSTLAllocator& operator= (ArenaSharedHeapSTLAllocator&& a);


  /**
   * @brief Swap.
   */
  void swap (ArenaSharedHeapSTLAllocator& a);


  /**
   * @brief Equality test.
   *
   * Two allocators should compare equal if objects allocated by one
   * can be deallocated by the other.  We check if they are referencing
   * the same Header.
   */
  bool operator== (const ArenaSharedHeapSTLAllocator& other) const;


  /**
   * @brief Inequality test.
   *
   * Two allocators should compare equal if objects allocated by one
   * can be deallocated by the other.  We check if they are referencing
   * the same Header.
   */
  bool operator!= (const ArenaSharedHeapSTLAllocator& other) const;


  /// Convert a reference to an address.
  pointer address (reference x) const;
  ATH_MEMBER_REQUIRES(!(std::is_same_v<reference,const_reference>),
                      const_pointer)
  address (const_reference x) const { return &x; }


  /**
   * @brief Allocate new objects.
   * @param n Number of objects to allocate.  Must be 1.
   * @param hint Allocation hint.  Not used.
   */
  pointer allocate (size_type n, const void* hint = 0);


  /**
   * @brief Deallocate objects.
   * @param n Number of objects to deallocate.  Must be 1.
   */
  void deallocate (pointer, size_type n);
  ATH_MEMBER_REQUIRES(!(std::is_same_v<pointer,const_pointer>), void)
  deallocate (const_pointer p, size_type n) const
  {
    pointer p_nc ATLAS_THREAD_SAFE = const_cast<pointer>(p);
    deallocate (p_nc, n);
  }


  /**
   * @brief Return the maximum number of objects we can allocate at once.
   *
   * This always returns 1.
   */
  size_type max_size() const throw();


  /**
   * @brief Call the @c T constructor.
   * @param p Location of the memory.
   * @param args Arguments to pass to the constructor.
   */
  template <class... Args>
  void construct (pointer p, Args&&... args);


  /**
   * @brief Call the @c T destructor.
   * @param p Location of the memory.
   */
  void destroy (pointer p);


  /**
   * @brief Return the hinted number of objects allocated per block.
   */
  size_t nblock() const;


  /**
   * @brief Return the name of this allocator.
   */
  const std::string& name() const;


  /**
   * @brief Free all allocated elements.
   *
   * All elements allocated are returned to the free state.
   * @c clear should be called on them if it was provided.
   * The elements may continue to be cached internally, without
   * returning to the system.
   */
  void reset();


  /**
   * @brief Free all allocated elements and release memory back to the system.
   *
   * All elements allocated are freed, and all allocated blocks of memory
   * are released back to the system.
   * @c destructor should be called on them if it was provided
   * (preceded by @c clear if provided and @c mustClear was set).
   */
  void erase();


  /**
   * @brief Set the total number of elements cached by the allocator.
   * @param size The desired pool size.
   *
   * This allows changing the number of elements that are currently free
   * but cached.  Any allocated elements are not affected by this call.
   *
   * If @c size is greater than the total number of elements currently
   * cached, then more will be allocated.  This will preferably done
   * with a single block, but that is not guaranteed; in addition, the
   * allocator may allocate more elements than is requested.
   *
   * If @c size is smaller than the total number of elements currently
   * cached, as many blocks as possible will be released back to the system.
   * It may not be possible to release the number of elements requested;
   * this should be implemented on a best-effort basis.
   */
  void reserve (size_t size);


  /**
   * @brief Return the statistics block for this allocator.
   */
  ArenaAllocatorBase::Stats stats() const;


  /**
   * @brief Return the statistics blocks summed up over all allocators
   *        using this pool.
   */
  ArenaAllocatorBase::Stats totstats() const;


  /**
   * @brief Return a pointer to the underlying allocator.
   */
  ArenaHeapAllocator* poolptr();


  /**
   * @brief Return a pointer to the underlying allocator.
   */
  const ArenaHeapAllocator* poolptr() const;


  /**
   * @brief Generate printable report for all contained allocators.
   * @param os Stream to which to write the report.
   */
  void report (std::ostream& os) const;
  

  /**
   * @brief Write-protect the memory managed by these allocators.
   *
   * Adjust protection on the memory managed by these allocators
   * to disallow writes.
   */
  void protect();


  /**
   * @brief Write-enable the memory managed by these allocators.
   *
   * Adjust protection on the memory managed by these allocators
   * to allow writes.
   */
  void unprotect();


private:
  ArenaSharedHeapSTLHeader* m_header;
  ArenaHeapAllocator* m_pool;
};


template <class T>
void swap (ArenaSharedHeapSTLAllocator<T>& a, ArenaSharedHeapSTLAllocator<T>& b)
{
  a.swap (b);
}


/**
 * @brief Hook for unprotecting an arena.
 *
 * Sometimes we need to ensure that an arena is unprotected before we start
 * destroying an object that contains the arena.  To do that without
 * making assumptions about whether the arena supports an unprotect
 * operation, call this function.
 */
template <class T>
void maybeUnprotect (ArenaSharedHeapSTLAllocator<T>& a);


} // namespace SG


#include "AthAllocators/ArenaSharedHeapSTLAllocator.icc"


#endif // not ATLALLOCATORS_ARENASHAREDHEAPSTLALLOCATOR_H
