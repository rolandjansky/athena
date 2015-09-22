// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaSharedHeapSTLAllocator.h 552460 2013-06-25 17:29:25Z ssnyder $
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


#include "AthAllocators/ArenaSTLAllocator.h"
#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaHeapSTLAllocator.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
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
   * @param name   Value to use as the base for the allocator names.
   */
  ArenaSharedHeapSTLHeader (const void* owner,
                            int nblock,
                            const std::string& name);


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
  std::string get_name();
    

  /**
   * @brief Return the heap allocator for type @c T.
   * @param index Reference to the index for type @c T.
   *              Before the first call, this should be initialized
   *              to std::string::npos.  This should generally
   *              be a static variable.
   */
  template <class T>
  ArenaHeapAllocator* get_pool (size_t& index);


  void report (std::ostream& os) const
  {
    for (size_t i = 0; i < m_allocators.size(); i++)
      if (m_allocators[i])
        m_allocators[i]->report (os);
  }


private:
  /// Address of the allocator that created this header.
  const void* m_owner;

  /// Saved value for nblock parameter.
  size_t m_nblock;

  /// Saved value for base name.
  std::string m_name;

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


  /// Standard STL allocator rebinder.
  template <class U> struct rebind {
    typedef ArenaSharedHeapSTLAllocator<U> other;
  };


  /**
   * @brief Default constructor.
   * @param nblock Value to set in the parameters structure for the
   *               number of elements to allocate per block.
   * @param name   Value to use as the base for the allocator names.
   */
  ArenaSharedHeapSTLAllocator (size_t nblock = 1000,
                               const std::string& name = "");


  /**
   * @brief Constructor from another @c ArenaHeapSTLAllocator.
   *
   * The new STL allocator will reference the same set of underlying
   * Arena allocators as the old one.
   */
  ArenaSharedHeapSTLAllocator (const ArenaSharedHeapSTLAllocator& a);


  /**
   * @brief Constructor from another @c ArenaHeapSTLAllocator.
   *
   * The new STL allocator will reference the same set of underlying
   * Arena allocators as the old one.
   */
  template <class U>
  ArenaSharedHeapSTLAllocator (const ArenaSharedHeapSTLAllocator<U>& a);

  // We don't bother to supply a more general constructor --- shouldn't
  // be needed.

  ~ArenaSharedHeapSTLAllocator();


  /**
   * @brief Assignment.
   *
   * We allow assignment only if the two objects involved represent
   * the same arena, in which case it's a no-op.
   * In other cases, we raise an exception.
   *
   * FIXME: By default, swap() is implemented in terms of this.
   * It might be useful, though, to have a swap() that could
   * handle different arenas.  We would need to be able handle
   * updating the ownership back pointers from the headers, though;
   * but that's much easier for swap than for the case of general
   * assignments.
   */
  ArenaSharedHeapSTLAllocator&
  operator= (const ArenaSharedHeapSTLAllocator& a);


  /// Convert a reference to an address.
  pointer address (reference x) const;
  const_pointer address (const_reference x) const;


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


  /**
   * @brief Return the maximum number of objects we can allocate at once.
   *
   * This always returns 1.
   */
  size_type max_size() const throw();


  /**
   * @brief Call the @c T constructor.
   * @param p Location of the memory.
   * @param val Parameter to pass to the constructor.
   */
  void construct (pointer p, const T& val);


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
  const ArenaAllocatorBase::Stats& stats() const;


  /**
   * @brief Return the statistics blocks summed up over all allocators
   *        using this pool.
   */
  ArenaAllocatorBase::Stats totstats() const;


  /**
   * @brief Return a pointer to the underlying allocator.
   *        This creates the allocator if needed.
   */
  ArenaHeapAllocator* poolptr() const;


  void report (std::ostream& os) const
  {
    m_header->report(os);
  }



  /**
   * @brief Compare two allocators.  Needed by some @c swap implementations.
   *
   *        We consider two allocators to be the same if they're
   *        referencing the same Header.
   */
  bool operator!= (const ArenaSharedHeapSTLAllocator& other) const;
  

private:
  /**
   * @brief Ask the Header for the allocator to use.
   *        This will either return an existing one or create a new one.
   */
  void get_pool() const;

  ArenaSharedHeapSTLHeader* m_header;
  mutable ArenaHeapAllocator* m_pool;

  static size_t s_index;
};


} // namespace SG


#include "AthAllocators/ArenaSharedHeapSTLAllocator.icc"


#endif // not ATLALLOCATORS_ARENASHAREDHEAPSTLALLOCATOR_H
