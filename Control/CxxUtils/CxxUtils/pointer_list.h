// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/pointer_list.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from earlier code.
 * @brief A fast way to store a variable-sized collection of pointers.
 */


#ifndef CXXUTILS_POINTER_LIST_H
#define CXXUTILS_POINTER_LIST_H


#include "boost/static_assert.hpp"
#include <iterator>


namespace CxxUtils {


/**
 * @brief A fast way to store a variable-sized collection of pointers.
 *
 * See @c pointer_list below for a summary.
 *
 * Some extra detail:
 * The list is a set of fixed-size blocks, each holding (by default)
 * 15 pointers.  At the end of each block is another pointer
 * used to form a a linked list.  We used to mark this last pointer
 * by setting the low bit as a sentinel.  But now that we control our
 * own allocator we can instead just insist that the blocks have
 * a size that's a power of 2 and are fully aligned.  We can then
 * test the low bits of the address to tell if we're looking
 * at the last pointer of a block.
 *
 * We then need to keep pointers to the head block, and to the current
 * insertion position.  To insert, we see if the insertion position
 * is pointing at a block end.  If so, we allocate another block, chain
 * to it, and reset the insertion position to the beginning of the new block.
 * Then store the pointer in the insertion position and bump it.
 * Iterators are just pointers to the stored pointers.  We use the
 * end positions to tell when to chain to a new block.  The insertion
 * point gives the end iterator.
 *
 * pointer_list is templated on the number of pointers stored in each
 * block.  We put it as a template parameter so that it can be accessed
 * from the iterator class without having to have a reference from the
 * iterator back to the containing list.
 */
class pointer_list_base
{
public:
  /**
   * @brief A single block in the list.
   *
   * It contains some number of pointers.
   * The last element of the block contains a link to the next block.
   * Blocks are aligned so that we can tell if we're looking
   * at the last element from the low bits of an address.
   */
  struct list_block
  {
  public:
    typedef unsigned long ulong;

    /// The element type we store.
    typedef void* value_type;

    /// The elements.  This structure is variable-sized;
    /// it will be allocated for the correct number of elements.
    value_type m_data[1];

    /// Size in bytes of a block holding @c nelt elements
    /// (excluding the end-pointer).
    static size_t size (size_t nelt);
  };


  /**
   * @brief Very simple allocator for use with @c pointer_list.
   *
   * We allocate large chunks and divide them up into @c list_block's.
   * We don't support deleting individual elements; instead, everything
   * is deleted when the allocator is.  The chunks are chained together
   * to allow for this.
   *
   * The total size of a block should be a power of two, and blocks
   * must be aligned to this boundary.
   */
  class allocator
  {
  public:
    /**
     * @brief Constructor.
     * @param nelt Number of pointers per block
     *             (excluding the end pointer).
     *             Must be one less than a power of two.
     * @param nblock Number of blocks to allocate per chunk.
     * @param end_mask Mask to use for end-of-block test.
     * @param end_offs Offset to use for end-of-block test.
     */
    allocator (size_t nelt,
               size_t nblock,
               unsigned long end_mask,
               unsigned long end_offs);

    /// Destructor.  Deletes all blocks from this allocator.
    ~allocator();

    /// Allocate a new block.
    list_block* allocate();

    /// Return the number of pointers per block
    /// (excluding the end-pointer).
    size_t nelt() const;

    /// Return the current number of allocated chunks.
    size_t nchunks() const;

    /// Test if P is pointing at the end-pointer of a block.
    bool at_end (const void* p) const;


  private:
    /// Allocate a new chunk of blocks.
    void refill();

    /// One memory allocation chunk.
    struct chunk
    {
      /// Link to the next chunk.
      chunk* m_next;

      /// Pointer to the first block contained within this chunk.
      list_block* m_blocks;
    };

    /// Number of elements per block (excluding the end-pointer).
    size_t m_nelt;

    /// Number of blocks per chunk.
    size_t m_nblock;

    /// Most recent chunk allocated.
    chunk* m_chunks;

    /// Number of blocks allocated so far from that chunk.
    size_t m_nthis;

    /// Current number of allocated chunks.
    size_t m_nchunks;

    /// Mask for testing for an end pointer.
    unsigned long m_end_mask;

    /// Offset for testing for an end pointer.
    unsigned long m_end_offs;
  };


  /// Alias for allocator.
  typedef allocator pool_type;

  /// The stored element type.
  typedef list_block::value_type value_type;

  /// Constructor.  @c pool gives the allocator for this container.
  pointer_list_base (pool_type& pool);

  /// Add a new element to the end of the container.  O(1)
  void push_back (value_type p);

  /// The current size of the container.  O(1).
  size_t size() const;

  /// Erase the container.  O(1).  Note: doesn't free memory.
  /// Memory currently in use will be reused when the container
  /// is filled again.
  void clear();

  /// Test to see if the container is empty.
  bool empty() const;


protected:
  /// The first block in the list.
  list_block* m_head;

  /// The current insertion point in the list.
  value_type* m_insert;

  /// The current list size.
  size_t m_size;

  /// The list allocator.
  allocator& m_pool;

  /// Allocate the first block of the list.
  void firstblock ();

  /// Extend the list with another block.
  /// @c m_insert should be at the end of the last block.
  void nextblock ();

  /// Allocate a new block.
  list_block* getblock();
};


/**
 * @brief A fast way to store a variable-sized collection of pointers.
 *
 * If you're growing a variable-sized collection of things, all the
 * STL containers have some performance issues.  A std::vector
 * needs to reallocate and copy its data as it grows.  The use of
 * variable-sized allocations also means that one cannot use the
 * very efficient fixed-size memory allocators.  A std::list
 * incurs a separate memory allocation for each element, and, if the
 * elements are pointers, has a substantial size overhead.
 *
 * The class here is a compromise, which builds a list consisting
 * of fixed-size chunks.
 *
 * The operations supported are rather limited.
 * We support forward iteration, push_back, and erase
 * (though the latter can have O(n) complexity).
 *
 * For best performance, we use our own allocator, an instance of which
 * gets passed to the @c pointer_list constructor.  Memory is not freed
 * until the allocator is destroyed.
 *
 * This class is templated on the number of elements stored per block.
 * This must be one less than a power of two.
 */
template <size_t NELT = 15>
class pointer_list
  : public pointer_list_base
{
public:
  /// Stored value type.
  typedef pointer_list_base::value_type value_type;


  /**
   * @brief Allocator for @c pointer_list, specialized for @c NELT.
   *
   * The purpose for thsi is to be able to have the static
   * @c at_end_static function, which we can call from an iterator.
   */
  class allocator
    : public pointer_list_base::allocator
  {
  public:
    /// Verify that @c NELT is one less than a power of two.
    BOOST_STATIC_ASSERT (((NELT+1) & NELT) == 0);

    /// Constants to use to test if we're at the end of a block.
    static const unsigned long END_OFFS = NELT * sizeof(value_type);
    static const unsigned long END_MASK = END_OFFS | (sizeof(value_type)-1);

    /**
     * @brief Constructor.
     * @param nblock Number of blocks to allocate per chunk.
     */
    allocator (size_t nblock = 100);
    

    /// Test if P is pointing at the end-pointer of a block.
    static bool at_end_static (const void* p);
  };


  /**
   * @brief Forward iterator over the list.
   */
  class iterator
    : public std::iterator<std::forward_iterator_tag, value_type>
  {
  public:
    typedef typename std::iterator<std::forward_iterator_tag, value_type>
      base;
    typedef typename base::iterator_category iterator_category;
    typedef typename base::difference_type   difference_type;
    typedef typename base::pointer            pointer;
    typedef typename base::reference          reference;

    /// Equality comparison.
    bool operator== (const iterator& other) const;

    /// Inequality comparison.
    bool operator!= (const iterator& other) const;

    /// Dereference.
    reference operator*();

    /// Advance (pre-increment).
    iterator& operator++();

    /// Advance (post-increment).
    iterator operator++(int);


  private:
    /// Constructor, from a pointer into a @c pointer_list.
    iterator (value_type* p);

    /// Current iteration position.
    value_type* m_p;

    friend class pointer_list;
  };

  typedef allocator pool_type;

  /// Constructor.  @c pool gives the allocator for this container.
  pointer_list (pool_type& pool);

  /// Iterator at the beginning of the container.
  iterator begin();

  /// Iterator at the end of the container.
  iterator end();

  /// Erase one element.  O(n)
  void erase (iterator it);
};


} // namespace CxxUtils


#include "CxxUtils/pointer_list.icc"


#endif // not CXXUTILS_POINTER_LIST_H
