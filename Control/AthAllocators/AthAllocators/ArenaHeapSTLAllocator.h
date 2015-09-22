// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeapSTLAllocator.h,v 1.2 2008-08-26 02:12:26 ssnyder Exp $

/**
 * @file  AthAllocators/ArenaHeapSTLAllocator.h
 * @author scott snyder
 * @date Nov 2011
 * @brief STL-style allocator wrapper for @c ArenaHeapAllocator.
 *
 * This class defines a STL-style allocator for types @c T with the
 * following special properties.
 *
 *  - We use an @c ArenaHeapAllocator for allocations.
 *  - The memory is owned directly by the allocator object.
 *  - Only one object at a time may be allocated.
 *
 * So, this allocator is suitable for an STL container which allocates
 * lots of fixed-size objects, such as @c std::list.
 *
 * Much of the complexity here is due to the fact that the allocator
 * type that gets passed to the container is an allocator for the
 * container's value_type, but that allocator is not actually
 * used to allocate memory.  Instead, an allocator for the internal
 * node type is used.  This makes it awkward if you want to have
 * allocators that store state.
 *
 * Further, to avoid creating a pool for the allocator for the container's
 * value_type (when the container doesn't actually use that for allocation),
 * this template has a second argument.  This defaults to the first argument,
 * but is passed through by rebind.  If the first and second argument
 * are the same, then we don't create a pool.
 *
 * The effect of all this is that you can give an allocator type like
 *   ArenaHeapSTLAllocator<Mytype>
 * to a STL container.  Allocators for Mytype won't use
 * a pool, but an allocator for node<Mytype> will use the pool.
 */


#ifndef ATLALLOCATORS_ARENAHEAPSTLALLOCATOR
#define ATLALLOCATORS_ARENAHEAPSTLALLOCATOR


#include "AthAllocators/ArenaHeapAllocator.h"
#include <string>


namespace SG {


/**
 * @brief Initializer for pool allocator parameters.
 *
 *        We override the defaults to disable calling the payload ctor/dtor.
 */
template <class T>
class ArenaHeapSTLAllocator_initParams
  : public ArenaHeapAllocator::initParams<T, false, true, true>
{
public:
  /// We take defaults from this.
  typedef ArenaHeapAllocator::initParams<T, false, true, true> Base;

    /**
     * @brief Constructor.
     * @param nblock Value to set in the parameters structure for the
     *               number of elements to allocate per block.
     * @param name   Value to set in the parameters structure for the
     *               allocator name.
     */
    ArenaHeapSTLAllocator_initParams (size_t nblock = 1000,
                                      const std::string& name = "");

    /// Return an initialized parameters structure.
    ArenaAllocatorBase::Params params() const;

    /// Return an initialized parameters structure.
    // Note: gcc 3.2.3 doesn't allow defining this out-of-line.
    operator ArenaAllocatorBase::Params() const { return params(); }
};


/**
 * @brief STL-style allocator wrapper for @c ArenaHeapAllocator.
 *        This is the generic specialization, which uses the heap allocator.
 *
 * See the file-level comments for details.
 */
template <class T, class VETO=T>
class ArenaHeapSTLAllocator
{
public:
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
    typedef ArenaHeapSTLAllocator<U, VETO> other;
  };


  /**
   * @brief Default constructor.
   * @param nblock Value to set in the parameters structure for the
   *               number of elements to allocate per block.
   * @param name   Value to set in the parameters structure for the
   *               allocator name.
   */
  ArenaHeapSTLAllocator (size_t nblock = 1000, const std::string& name = "");


  /**
   * @brief Constructor from another @c ArenaHeapSTLAllocator.
   *
   * The @c name and @c nblock parameters are copied, but the data are not.
   */
  template <class U, class V>
  ArenaHeapSTLAllocator (const ArenaHeapSTLAllocator<U, V>& a);

  // We don't bother to supply a more general constructor --- shouldn't
  // be needed.


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
   * @brief Return a pointer to the underlying allocator (may be 0).
   */
  ArenaAllocatorBase* poolptr() const;


private:
  /// The underlying allocator.
  ArenaHeapAllocator m_pool;
};



/**
 * @brief STL-style allocator wrapper for @c ArenaHeapAllocator.
 *        This is the specialization for the case of the vetoed type.
 *
 * See the file-level comments for details.
 */
template <class T>
class ArenaHeapSTLAllocator<T, T>
  : public std::allocator<T>
{
public:
  typedef std::allocator<T> base;

  /// Standard STL allocator typedefs.
  typedef typename base::pointer         pointer;
  typedef typename base::const_pointer   const_pointer;
  typedef typename base::reference       reference;
  typedef typename base::const_reference const_reference;
  typedef typename base::value_type      value_type;
  typedef typename base::size_type       size_type;
  typedef typename base::difference_type difference_type;


  /// Standard STL allocator rebinder.
  template <class U> struct rebind {
    typedef ArenaHeapSTLAllocator<U, T> other;
  };


  /**
   * @brief Default constructor.
   * @param nblock Value to set in the parameters structure for the
   *               number of elements to allocate per block.
   * @param name   Value to set in the parameters structure for the
   *               allocator name.
   */
  ArenaHeapSTLAllocator (size_t nblock = 1000, const std::string& name = "");


  /**
   * @brief Constructor from another @c ArenaHeapSTLAllocator.
   *
   * The @c name and @c nblock parameters are copied, but the data are not.
   */
  template <class U, class V>
  ArenaHeapSTLAllocator (const ArenaHeapSTLAllocator<U, V>& a);


  // We don't bother to supply a more general constructor --- shouldn't
  // be needed.


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
   * @brief Return a pointer to the underlying allocator (may be 0).
   */
  ArenaAllocatorBase* poolptr() const;


private:
  /// Saved hinted number of objects per block.
  size_t m_nblock;

  /// Saved allocator name.
  std::string m_name;

  /// Point at an underlying allocator from a different specialization.
  ArenaAllocatorBase* m_poolptr;
};


} // namespace SG


#include "AthAllocators/ArenaHeapSTLAllocator.icc"


#endif // ATLALLOCATORS_ARENAHEAPSTLALLOCATOR
