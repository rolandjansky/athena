// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaAllocatorBase.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaAllocatorBase.h
 * @author scott snyder
 * @date May 2007
 * @brief Common base class for arena allocator classes.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAALLOCATORBASE_H
#define ATLALLOCATORS_ARENAALLOCATORBASE_H


#include <cstdlib>
#include <new>
#include <string>
#include <iosfwd>
#include "boost/type_traits.hpp"


namespace SG {


/**
 * @brief Common base class for arena allocator classes.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * This base class provides the interfaces and common behavior for
 * arena allocator classes.  An allocator class is responsible for the
 * actual allocation and deletion of objects.  It is expected that
 * unallocated objects may be maintained in a pool for efficiency.
 * In addition, it should be possible to free at once all the objects
 * that this allocator has allocated.  See below for the details
 * of the interface that classes deriving from this should implement.
 *
 * A matter of terminology.  The objects that we allocate and free are
 * called `elements'.  As mentioned, it is expected that the memory will
 * be allocated from the system for large groups of elements at any one
 * time; these groups are called `blocks'.
 *
 * The allocator classes do not themselves depend on the type of the
 * object being allocated.  Instead, the necessary information is included
 * in a parameters object of class @c Params that is passed to the allocator
 * on construction.  These parameters include:
 *
 *  - @c name: The name of this allocator.  This is used both in printed
 *             reports and to identify this allocator in the registry.
 *  - @c eltSize: The size in bytes of the individual elements
 *             we're allocating.
 *  - @c minSize: The minimum size that this Allocator allows for
 *             an element.
 *  - @c nblock: A hint for the number of elements to allocate in a single
 *             block.  This is only a hint; the allocator may allocate
 *             a different number if that would be more efficient.
 *  - @c linkOffset: Some allocators may require keeping a pointer along
 *             with the element.  This gives the offset (in bytes)
 *             from the start of the element where that pointer
 *             lives.  In some cases, it may be safe to save space
 *             by allowing this pointer to overlap part of the element.
 *             Calling code is responsible for setting this up correctly.
 *  - @c constructor, @c destructor: It may be more efficient to call the
 *             element's constructor and destructor not each time the element
 *             is allocated and freed, but instead just when the element's
 *             block is allocated from or freed to the system.  We thus
 *             cache a free pool of already-initialized objects.  These
 *             parameters are to support this case.  These are pointers
 *             to functions with the signature void (*)(char*).  The
 *             @c constructor function is called when an element is first
 *             allocated from the system, and the @c destructor function
 *             is called when the element is finally released to the system.
 *             Either may be 0 to skip the call.
 *  - @c clear: In addition to the constructor and destructor, it may be
 *             necessary to reset the element state after the application
 *             releases it.  If @c clear is not-null, this function will
 *             be called when an application frees an element.
 *  - @c canReclear: If true (the default), then it is safe to call @c clear
 *             more than once on a given element after it has been released.
 *  - @c mustClear: If true, then @c clear must be called before calling
 *             @c destructor.  Otherwise (the default), @c destructor
 *             may be called directly without calling @c clear.
 *
 * The allocator class should maintain statistics for the memory it has
 * allocated.  These are grouped in the @c Stats structure.  It should
 * report the amount of space in use, the amount of free space (allocated
 * from the system but not allocated by the application), and the total
 * space allocated from the system, broken down into blocks, elements,
 * and bytes.  The derived allocator class may of course extend
 * the provided statistics.
 *
 * The interface provided by derived allocator classes should consist
 * of at least the following:
 *
 *  - A constructor from a @c Params structure.
 *
 *  - Implementations of the virtual methods @c reset, @c erase, @c reserve,
 *    @c name, @c stats.  See documentation below.
 *
 *  - An implementation of the method
 *@code
 *      pointer allocate();
 @endcode
 *    This should be non-virtual, and may be inlined.  It should return
 *    a pointer to an new element.  If @c constructor was provided,
 *    then it should have been called on the element.
 *
 *  - Optionally, an implementation of the non-virtual method
 *@code
 *      void free (pointer);
 @endcode
 *     to free a single element.  @c clear will be called on the element
 *     if it was provided.
 *
 *  - Optionally, an implementation of the non-virtual method
 *@code
 *      void resetTo (pointer);
 @endcode
 *     @c pointer should be a pointer to an element that was allocated
 *     from this allocator.  That element and all elements that were
 *     allocated after it will be freed.
 *
 *  - Optionally, @c iterator and @c const_iterator types and the
 *    corresponding const and non-const @c begin and @c end methods.
 *    These iterators should range over all allocated elements.
 *    The @c value_type should be @c pointer, and they should be at least
 *    forward iterators.
 */
class ArenaAllocatorBase
{
public:
  /// Type for pointers to elements.
  typedef char* pointer;

  /// And a const version of the pointer.
  typedef const char* const_pointer;

  /// Type of functions for @c constructor, etc.
  typedef void func_t (pointer);


  /**
   * @brief Allocator parameters.  See above for more details.
   */
  struct Params
  {
    /// The name of this allocator.
    std::string name;

    /// The size in bytes of the individual elements we're allocating.
    size_t eltSize;

    /// The minimum size that this Allocator allows for an element.
    size_t minSize;

    /// The number of elements we should allocate in a single block
    /// (hint only).
    size_t nblock;

    /// Offset from the start of a free element to a pointer to be used
    /// by the allocator.  Only used if the allocator requires it.
    size_t linkOffset;

    /// Constructor function for elements.

    func_t* constructor;
    /// Destructor function for elements.
    func_t* destructor;

    /// Clear function for elements.
    func_t* clear;

    /// If true, @c clear can be called more than once on a given element.
    bool canReclear;

    /// If true, the @c clear call cannot be skipped before @c destructor.
    bool mustClear;
  };


  /**
   * @brief Statistics for an allocator.  See above for more details.
   */
  struct Stats {
    // Strictly unneeded, but without it we get a cppcheck false positive.
    Stats() = default;

    /// A single statistic.
    struct Stat {
      /// Default constructor.
      Stat();
      /// Reset to zero.
      void clear();
      /// Accumulate.
      Stat& operator+= (const Stat& other);

      /// Number of items currently allocated by the application.
      size_t inuse;
      /// Number of items currently not allocated by the application
      /// but cached by the allocator.
      size_t free;
      /// Total number of items held by the allocator.
      size_t total;
    };
    /// Reset to zero.
    void clear();
    /// Accumulate.
    Stats& operator+= (const Stats& other);
    /// Print report header.
    static void header (std::ostream& os);

    /// Counts of blocks.
    Stat blocks;
    /// Counts of elements.
    Stat elts;
    /// Counts of bytes.
    Stat bytes;
  };


  /**
   * @brief Destructor.
   */
  virtual ~ArenaAllocatorBase() {}


  /**
   * @brief Free all allocated elements.
   *
   * All elements allocated are returned to the free state.
   * @c clear should be called on them if it was provided.
   * The elements may continue to be cached internally, without
   * returning to the system.
   */
  virtual void reset() = 0;


  /**
   * @brief Free all allocated elements and release memory back to the system.
   *
   * All elements allocated are freed, and all allocated blocks of memory
   * are released back to the system.
   * @c destructor should be called on them if it was provided
   * (preceded by @c clear if provided and @c mustClear was set).
   */
  virtual void erase() = 0;


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
  virtual void reserve (size_t size) = 0;


  /**
   * @brief Return the statistics block for this allocator.
   */
  virtual Stats stats() const = 0;


  /**
   * @brief Return the name of this allocator.
   */
  virtual const std::string& name() const = 0;


  /**
   * @brief Generate a report on the memory usage of this allocator.
   * @param os Stream to which the report should be written.
   */
  virtual void report (std::ostream& os) const;


  //=========================================================================
  /**
   * @brief Helper to initialize a parameters structure.
   *
   * This creates a @c Params class appropriately initialized for class @c T.
   * Assumptions made:
   *  - The constructor and destructor calls will be filled in
   *    if non-trivial, unless no_ctor or no_dtor is set to true.
   *    These two arguments are useful if the ctor/dtor are to
   *    be run elsewhere, or if they are trivial and can be skipped,
   *    but the compiler cannot detect that by itself.
   *  - The clear call will be filled in if the optional template parameter
   *    @c clear is true.
   *  - No space will be reserved for an extra link.
   *  - @c canReclear is @c true and @c mustClear is @c false.
   *
   * If these are not appropriate, you can derive from this class
   * and make the appropriate changes.
   */
  template <typename T,
            bool clear = false,
            bool no_ctor = false,
            bool no_dtor = false>
  struct initParams
  {
    /**
     * @brief Constructor.
     * @param nblock Value to set in the parameters structure for the
     *               number of elements to allocate per block.
     * @param name   Value to set in the parameters structure for the
     *               allocator name.
     */
    initParams (size_t nblock = 1000, const std::string& name = "");

    /// Return an initialized parameters structure.
    Params params() const;

    /// Return an initialized parameters structure.
    // Note: gcc 3.2.3 doesn't allow defining this out-of-line.
    operator Params() const { return params(); }


  private:
    /// Saved value of the number of elements to allocate per block.
    size_t m_nblock;

    /// Saved value of the allocator name.
    std::string m_name;
  };


  /// Make a constructor function pointer for a non-trivial constructor.
  template <class T>
  static func_t* makeConstructor (const boost::false_type&);

  /// Make a constructor function pointer for a trivial constructor.
  template <class T>
  static func_t* makeConstructor (const boost::true_type&);

  /// Make a constructor function pointer for a non-trivial destructor.
  template <class T>
  static func_t* makeDestructor (const boost::false_type&);

  /// Make a constructor function pointer for a trivial destructor.
  template <class T>
  static func_t* makeDestructor (const boost::true_type&);

  /// Make a function pointer for a @c clear function.
  template <class T>
  static func_t* makeClear (const boost::false_type&);

  /// Make a dummy @c clear function pointer.
  template <class T>
  static func_t* makeClear (const boost::true_type&);

private:
  /**
   * @brief Call @c T's default constructor on the object at @c p.
   * @param p The object on which to run the constructor.
   */
  template <typename T>
  static void construct_fcn (pointer p);


  /**
   * @brief Call @c T's destructor on the object at @c p.
   * @param p The object on which to run the destructor.
   */
  template <typename T>
  static void destroy_fcn (pointer p);


  /**
   * @brief Call @c T::clear on the object at @c p.
   * @param p The object on which to run the @c clear.
   */
  template <typename T>
  static void clear_fcn (pointer p);
};


/**
 * @brief Format a statistic structure.
 * @param os The stream to which to write.
 * @param stat The statistic structure to write.
 */
std::ostream& operator<< (std::ostream& os,
                          const ArenaAllocatorBase::Stats::Stat& stat);


/**
 * @brief Format a complete statistics structure.
 * @param os The stream to which to write.
 * @param stats The statistics structure to write.
 */
std::ostream& operator<< (std::ostream& os,
                          const ArenaAllocatorBase::Stats& stats);

} // namespace SG


#include "AthAllocators/ArenaAllocatorBase.icc"


#endif // not ATLALLOCATORS_ARENAALLOCATORBASE_H
