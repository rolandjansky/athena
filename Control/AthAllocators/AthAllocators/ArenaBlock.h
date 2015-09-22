// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBlock.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaBlock.h
 * @author scott snyder
 * @date May 2007
 * @brief A large memory block that gets carved into smaller uniform elements.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#ifndef ATLALLOCATORS_ARENABLOCK_H
#define ATLALLOCATORS_ARENABLOCK_H


#include <cstdlib>


namespace SG {


class ArenaAllocatorBase;


/**
 * @brief A large memory block that gets carved into smaller uniform elements.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * The block-based memory allocators allocate memory in large blocks
 * and then carve them up into smaller, uniform elements.  This class
 * is used for those large blocks.  Actually, the contents of this class
 * is a fixed header for the block; the contents of the block itself
 * will immediately follow the class contents.
 *
 * Each block keeps some housekeeping information: the element size in bytes,
 * the number of elements in the block, and a pointer that can be used
 * to chain blocks together in singly-linked lists.  There are a few
 * functions available to help manage such lists.
 */
class ArenaBlock
{
public:
  /// Type for a pointer to an element.
  typedef char* pointer;

  /// Function that operates on an element.
  typedef void func_t (pointer);


  /**
   * @brief Create a new block.
   * @param n The number of elements in the new block.
   * @param elt_size The size in bytes of each element.
   * @param ctor If non-null, call this function on each element
   *             in the new block.
   */
  static ArenaBlock* newBlock (size_t n, size_t elt_size, func_t* ctor);


  /**
   * @brief Destroy a block.
   * @param p The block to destroy.
   * @param dtor If non-null, call this function on each element in the block.
   */
  static void destroy (ArenaBlock* p, func_t* dtor);


  /**
   * @brief Destroy all blocks in a list.
   * @param p The first block to destroy.
   * @param dtor If non-null, call this function on each element in the blocks.
   *
   * Will destroy all blocks in the linked list headed by @c p.
   */
  static void destroyList (ArenaBlock* p, func_t* dtor);


  /**
   * @brief Concatenate two lists of blocks.
   * @param headp Pointer to pointer to the head of the list.
   * @param tail Pointer to list to append to the end.
   *
   * The list @c tail is appended to the end of the list @c *headp.
   * (@c headp is a pointer-to-pointer to be able to handle the case
   * of an empty list.)
   */
  static void appendList (ArenaBlock** headp, ArenaBlock* tail);


  /**
   * @brief Call a function on elements in a list of blocks.
   * @param p Pointer to the head of the list.
   * @param func Function to apply.
   * @param n Number of elements in the first block on which
   *        to call the function.
   *
   * This will loop through the elements in all blocks on the list,
   * calling @c func.  In the first block, we apply the function
   * only to the first @c n elements.  In subsequent blocks, the
   * function is applied to all elements.
   */
  static void applyList (ArenaBlock* p, func_t* func, size_t n);


  /**
   * @brief Return the number of elements in the block.
   */
  size_t size() const;


  /**
   * @brief Return the size of the elements in the block.
   */
  size_t eltSize() const;


  /**
   * @brief Return the link pointer of the block.
   */
  ArenaBlock* & link();


  /**
   * @brief Return a pointer to element @c i in the block.
   * @param i The index of the desired element.
   */
  pointer index (size_t i);


  /**
   * @brief Return a pointer to element @c i in the block.
   * @param i The index of the desired element.
   * @param elt_size The block's element size.
   *
   * This is provided in addition to the previous function as it may
   * allow for better inlined code in when used in a loop, if @c elt_size
   * is saved in a local.
   */
  pointer index (size_t i, size_t elt_size);


  /**
   * @brief Return the per-block memory overhead, in bytes.
   *
   * This tries to include malloc overhead as well, but that may just
   * be an estimate.  Don't rely on this to be exact.
   */
  static size_t overhead();


  /// Return the global number of blocks in use.
  static size_t nactive();


private:
  /// Prohibit calling these.
  ArenaBlock (size_t n, size_t elt_size);
  ~ArenaBlock() {}
  ArenaBlock (const ArenaBlock&);
  ArenaBlock& operator= (const ArenaBlock&);

  // This is not really needed.  It's just to squelch the g++ warning
  // about classes with all private ctors/dtors and no friends.
  friend class ArenaAllocatorBase;

  /// The link for the linked list.
  ArenaBlock* m_link;

  /// Number of elements in this block.
  size_t m_size;

  /// Size, in bytes, of each element in this block.
  size_t m_elt_size;

  /// The start of the block body.
  // Try to make sure it's maximally aligned.
  // __attribute__ ((aligned)) will do that with gcc; on other compilers,
  // try to get at least what a double requires.  That's probably enough.
  double m_dummy
#ifdef __GCC__
    __attribute__ ((aligned))
#endif
    ;


  /// Global count of the number of blocks in use.
  static size_t s_nactive;
};


/// The offset from the start of the block to the first element.
static const int ArenaBlockBodyOffset =
  sizeof (ArenaBlock) - sizeof (double);


} // namespace SG


#include "AthAllocators/ArenaBlock.icc"


#endif // not ATLALLOCATORS_ARENABLOCK_H
