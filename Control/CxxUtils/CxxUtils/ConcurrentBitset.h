// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/ConcurrentBitset.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Variable-sized bitset allowing (mostly) concurrent access.
 */


#ifndef CXXUTILS_CONCURRENTBITSET_H
#define CXXUTILS_CONCURRENTBITSET_H


#include "CxxUtils/atomic_fetch_minmax.h"
#include "CxxUtils/bitscan.h"
#include "CxxUtils/ones.h"
#include <climits>
#include <vector>
#include <algorithm>
#include <iterator>
#include <atomic>
#include <mutex>
#include <memory>
#include <type_traits>
#include <cstddef>


namespace CxxUtils {


/**
 * @brief Variable-sized bitset allowing (mostly) concurrent access.
 *
 * This class represents a set of bits.  One can think of such an object as either
 * like a std::bitset or like a std::set<unsigned>.  This class provides
 * basically the union of the two interfaces.  In a few cases, the same method
 * has different semantics in the two interfaces, most notably @c size().
 * We follow here the semantics that we would get from std::set<unsigned>.
 * (Rationale: The motivation for this class is to replace the existing use
 * of unordered_set<unsigned> for auxid_set_t.  Supplying the same interface
 * makes this easier to drop in as a replacement.  However, in some cases,
 * the set operations as provided by a bitset-like interface can be much more
 * efficient, so we'd want to provide those as well.)
 *
 * The size of the bitset is specified at runtime, to the constructor.  Most operations
 * do not change the size.  However, the @c insert() methods, as well as @c operator=,
 * may be used to grow the size of the set.
 *
 * Most methods can execute completely concurrently.  However, the methods which
 * can change the size of the container, @c insert() and @c operator=, are not
 * compatible with other concurrent writes.  (Concurrent reads are ok, though.)
 *
 * Some methods can operate on the entire set (e.g., @c clear()).  Such methods
 * may run concurrently with others, but they will not necessarily be atomic:
 * other threads may be able to see the operation partially completed.
 *
 * An iterator is provided that iterates over all bits that are set (like iterating
 * over a set<unsigned>).
 *
 * When the container is expanded, the internal representation needs to be reallocated.
 * The old object is not, however, deleted immediately, as other threads may
 * still be referencing it.  If if some point you know that no threads can be
 * referencing old versions any more, you can clean them up by calling @c emptyGarbage
 * (otherwise, all versions will be deleted when the set object is destroyed).
 *
 * Some notes on motivation:
 *
 * The use case that motivated this class was auxid_set_t, which tells which
 * auxiliary variables are available for a given container.  This is logically
 * a relatively sparse set of relatively small integers.  (Maximum value is ~2000,
 * with a set having ~100 entries.)
 *
 * This had been implemented as a std::unordered_set<size_t>, but this was
 * not entirely satisfactory.  First, in some cases, there was a significant overhead
 * in inserting and deleting items from the set.  Second, unordered_set does not
 * allow concurrent reading and writing.  This meant that we ended up maintaining
 * thread-local copies of each set, which adds extra overhead and complexity.
 *
 * The threading issue suggests that we would like to use a container that allows
 * readers to run currently with at least one writer.  The fact that the maximum
 * value to be stored in these sets is not too large suggests that a bitmap
 * might be a good representation, as long as the time required to iterate
 * over the set doesn't blow up due to the sparseness of the map.
 *
 * To study this, a reconstruction job was instrumented to dump out all unique
 * auxid_set_t values.  This corpus was then used to run a set of timing tests
 * for different set implementations.  This test is built into the ConcurrentBitset
 * unit test, and the corpus is available in the CxxUtils package.  Run like:
 *
 *@code
 *   .../ConcurrentBitset_test.exe --perf .../CxxUtils/share/auxids.uniq 
 @endcode
 *
 * Set implementations that have been tested so far include:
 *  - std::set
 *  - std::unordered_set
 *  - concurrent_unordered_set, from TBB
 *  - ck_hs, from ConcurrencyKit
 *  - ck_bitmap, from ConcurrencyKit
 *  - ConcurrentBitset
 *
 * Representative results (times in seconds; lower is better):
 *
 *@code
 *  |--------------------------+------+------+---------+--------|
 *  |                          | fill | copy | iterate | lookup |
 *  |--------------------------+------+------+---------+--------|
 *  | set                      | 0.92 | 0.66 |   10.95 |   0.53 |
 *  | unordered_set            | 1.63 | 0.93 |    6.56 |   0.50 |
 *  | concurrent_unordered_set | 1.79 | 1.70 |    9.55 |   1.20 |
 *  | ck_hs                    | 0.78 | 0.83 |   18.92 |   0.88 |
 *  | ck_bitmap                | 0.13 | 0.21 |    5.52 |   0.05 |
 *  | ConcurrentBitset         | 0.31 | 0.06 |    4.48 |   0.08 |
 *  |--------------------------+------+------+---------+--------|
 @endcode
 *
 * For this workload, the bitmaps are the clear winner.
 *
 * Implementation notes:
 *
 * The implementation here is inspired by ck_bitmap from ConcurrencyKit
 * (http://concurrencykit.org/), though the code is all new.
 * ck_bitmap itself isn't suitable because it doesn't allow for the set to grow,
 * and also because it's a pure C interface, while we want something with
 * a C++ style interface (and in particular something which supports interfaces
 * close to unordered_set<size_t> in order to ease migration).
 *
 * The bitset is stored as an array of std::atomic<Block_t> objects, where Block_t
 * is the largest unsigned type for which atomic is lockless.  This is stored
 * in a separate implementation object in order to allow the container to grow.
 * The implementation object has a fixed-size header followed by the variable-size
 * array of blocks.
 *
 * To speed up iteration for the typical case of a sparse set, we maintain
 * a `high-water' mark, m_hwm, which is the index of the highest block that
 * might have a bit set.  m_hwm can only increase unless the set is cleared.
 */
class ConcurrentBitset
{
private:
  // Forward declaration of implementation class.
  class Impl;

  /// Internal type used to hold the bitset data.
  /// The bitset is an array of std::atomic<Block_t>.
  /// This type should generally be the largest unsigned type for which
  /// std::atomic is lockless.
  typedef unsigned long Block_t;

  /// Size, in bits, of @c Block_t.
  static const size_t BLOCKSIZE = sizeof(Block_t) * CHAR_BIT;

  /// Mask to select out the bit offset within one @Block_t.
  static const size_t MASK = BLOCKSIZE-1;


#if __cplusplus >= 201700
  static_assert (std::atomic<Block_t>::is_always_lock_free);
#else
# if ATOMIC_LONG_LOCK_FREE != 2
#  error Code assumes std::atomic<unsigned long> is lock free.
# endif
#endif  


public:
  /// A bit number.
  typedef size_t bit_t;


  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Constructor.
   * @param nbits Initial number of bits to allocate for the map.
   */
  ConcurrentBitset (bit_t nbits = 0);


  /**
   * @brief Copy constructor.
   * @param other Container to copy.
   *
   * The copy is not atomic.  If a non-atomic update is simultaneously made
   * to @c other, then the copy may have this update only partially completed.
   */
  ConcurrentBitset (const ConcurrentBitset& other);


  /**
   * @brief Constructor from an initializer list.
   * @param List of values to set.
   * @param nbits Number of bits to allocate for the map.
   *              If 0, then set the size based on the maximum value in the list.
   *
   * This allows setting specific bits in the set, like
   *@code
   *  ConcurrentBitset bs { 1, 5, 10};
   @endcode
  */
  ConcurrentBitset (std::initializer_list<bit_t> l, bit_t nbits = 0);


  /**
   * @brief Destructor.
   */
  ~ConcurrentBitset();


  /**
   * @brief Assignment.
   * @param other Bitset from which to assign.
   *
   * The copy is not atomic.  If a non-atomic update is simultaneously made
   * to @c other, then the copy may have this update only partially completed.
   */
  ConcurrentBitset& operator= (const ConcurrentBitset& other);


  /**
   * @brief Clean up old versions of the set.
   *
   * The insert and assignment operations may need to grow the set.
   * The original version of the set is not deleted immediately, since other threads
   * may still be accessing it.  Call this when no other threads can be accessing
   * old versions in order to clean them up.
   */
  void emptyGarbage();


  //@}
  //========================================================================
  /** @name Size, bit testing */
  //@{


  /**
   * @brief The number of bits that this container can hold.
   */
  bit_t capacity() const;


  /**
   * @brief Count the number of 1 bits in the set.
   */
  bit_t count() const;


  /**
   * @brief Count the number of 1 bits in the set.
   *
   * Note: If you regard this like a std::bitset, you would expect this to return
   * the number of bits that the set can hold, while if you regard this like
   * a set<bit_t>, then you would expect this to return the number of 1 bits.
   * We follow the latter here.
   */
  bit_t size() const;


  /**
   * @brief Test to see if a bit is set.
   * @param bit Number of the bit to test.
   * @return true if the bit is set; false otherwise.
   *
   * Returns false if @c bit is beyond the end of the set.
   */
  bool test (bit_t bit) const;


  /**
   * @brief Test to see if a bit is set.
   * @param bit Number of the bit to test.
   * @return 1 if the bit is set; 0 otherwise.
   *
   * Returns 0 if @c bit is beyond the end of the set.
   */
  size_t count (bit_t bit) const;


  /**
   * @brief Return true if there are no 1 bits in the set.
   */
  bool empty() const;


  /**
   * @brief Return true if there are no 1 bits in the set.
   */
  bool none() const;


  /**
   * @brief Return true if all bits in the set are 1.
   */
  bool all() const;


  /**
   * @brief Return true if there are any 1 bits in the set.
   */
  bool any() const;


  //@}
  //========================================================================
  /** @name Single-bit manipulation. */
  //@{


  /**
   * @brief Turn on one bit.
   * @param bit The bit to turn on.
   *
   * Does nothing if @c bit beyond the end of the set.
   */
  ConcurrentBitset& set (bit_t bit);


  /**
   * @brief Turn off one bit.
   * @param bit The bit to turn off.
   *
   * Does nothing if @c bit beyond the end of the set.
   */
  ConcurrentBitset& reset (bit_t bit);


  /**
   * @brief Turn off one bit.
   * @param bit The bit to turn off.
   *
   * Does nothing if @c bit beyond the end of the set.
   */
  ConcurrentBitset& erase (bit_t bit);


  /**
   * @brief Flip the value of one bit.
   * @param bit The bit to turn flip.
   *
   * Does nothing if @c bit beyond the end of the set.
   */
  ConcurrentBitset& flip (bit_t bit);


  /**
   * @brief Set the value of one bit.
   * @param bit The bit to turn set.
   * @param val The value to which to set it.
   *
   * Does nothing if @c bit beyond the end of the set.
   */
  ConcurrentBitset& set (bit_t bit, bool val);


  //@}
  //========================================================================
  /** @name Set operations. */
  //@{


  /**
   * @brief Clear all bits in the set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& clear();


  /**
   * @brief Clear all bits in the set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& reset();


  /**
   * @brief Turn on all bits in the set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& set();


  /**
   * @brief Flip the state of all bits in the set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& flip();


  /**
   * @brief AND this set with another set.
   * @param other The other set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& operator&= (const ConcurrentBitset& other);


  /**
   * @brief OR this set with another set.
   * @param other The other set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& operator|= (const ConcurrentBitset& other);

  
  /**
   * @brief XOR this set with another set.
   * @param other The other set.
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& operator^= (const ConcurrentBitset& other);


  /**
   * @brief Subtract another set from this set.
   * @param other The other set.
   *
   * This is the same as (*this) &= ~other;
   *
   * This operation is not necessarily atomic; a simultaneous read may be able
   * to see the operation partially done.
   */
  ConcurrentBitset& operator-= (const ConcurrentBitset& other);


  /**
   * @brief Return a new set that is the complement of this set.
   */
  ConcurrentBitset operator~() const;


  //@}
  //========================================================================
  /** @name Comparison. */
  //@{


  /**
   * @brief Test two sets for equality.
   * @param other The other set to test.
   */
  bool operator== (const ConcurrentBitset& other) const;


  /**
   * @brief Test two sets for inequality.
   * @param other The other set to test.
   */
  bool operator!= (const ConcurrentBitset& other) const;


  //@}
  //========================================================================
  /** @name Insert. */
  //@{


  /**
   * @brief Set a bit to 1.  Expand the set if needed.
   * @param bit Number of the bit to set.
   * @param new_nbits Hint for new size of set, if it needs to be expanded.
   *
   * If @c bit is past the end of the container, then the container will be
   * expanded as needed.
   *
   * This operation is incompatible with any other simultaneous writes
   * to the same set (reads are ok).
   */
  ConcurrentBitset& insert (bit_t bit, bit_t new_nbits = 0);


  /**
   * @brief Set several bits to 1.  Expand the set if needed.
   * @param beg Start of range of bits to set.
   * @param end End of range of bits to set.
   * @param new_nbits Hint for new size of set, if it needs to be expanded.
   *
   * The iteration range should be over something convertible to @c bit_t.
   * If any bit is past the end of the container, then the container will be
   * expanded as needed.
   *
   * This operation is incompatible with any other simultaneous writes
   * to the same set (reads are ok).
   *
   * Example:
   *@code
   *  std::vector<bit_t> bits { 4, 10, 12};
   *  CxxUtils::ConcurrentBitset bs = ...;
   *  bs.insert (bits.begin(), bits.end());
   @endcode
   */
  // The enable_if is needed to keep this something like
  //    bs.insert (1, 2)
  // from matching this overload.
  template <class ITERATOR,
            typename = typename std::enable_if<
              std::is_base_of<
                typename std::forward_iterator_tag,
                typename std::iterator_traits<ITERATOR>::iterator_category
                >::value> >
  ConcurrentBitset& insert (ITERATOR beg, ITERATOR end, bit_t new_nbits = 0);


  /**
   * @brief Set several bits to 1.  Expand the set if needed.
   * @param l List of bits to set.
   * @param new_nbits Hint for new size of set, if it needs to be expanded.
   *
   * If any bit is past the end of the container, then the container will be
   * expanded as needed.
   *
   * This operation is incompatible with any other simultaneous writes
   * to the same set (reads are ok).
   *
   * Example:
   *@code
   *  std::vector<bit_t> bits { 4, 10, 12};
   *  bs.insert ({4, 10, 12});
   @endcode
   */
  ConcurrentBitset& insert (std::initializer_list<bit_t> l, bit_t new_nbits = 0);


  /**
   * @brief Turn on bits listed in another set.
   * @param other Set of bits to turn on.
   *
   * This is the same as @c operator|=, except that if the size of @c other is
   * larger than this set, then this set will be expanded to match @c other.
   *
   * This operation is incompatible with any other simultaneous writes
   * to the same set (reads are ok).
   */
  ConcurrentBitset& insert (const ConcurrentBitset& other);

  
  //@}
  //========================================================================
  /** @name Array-like element access. */
  //@{


  /**
   * @brief A reference to one bit in a set.
   *
   * These references are invalidated by calls to insert() or operator=.
   */
  class reference
  {
  private:
    friend class ConcurrentBitset;


    /**
     * @brief Constructor.
     * @param impl ConcurrentBitset implementation object.
     * @param bit Bit number to which this reference refers.
     */
    reference (Impl& impl, bit_t bit);


  public:
    /**
     * @brief Set the referenced bit to a given value.
     * @param val Value to which to set the referenced bit.
     */
    reference& operator= (bool val) noexcept;


    /**
     * @brief Copy the value of another referenced bit.
     * @param r The other reference.
     *
     * To allow:
     *@code
     * ConcurrentBitset& bs1 = ...;
     * ConcurrentBitset& bs2 = ...;
     * bs1[1] = bs2[1];
     @endcode
    */
    reference& operator= (const reference& r) noexcept;


    /**
     * @brief Return the value of the referenced bit.
     */
    operator bool() const noexcept;


    /**
     * @brief Return the complement of the value of the referenced bit.
     */
    bool operator~() const noexcept;


    /**
     * @brief Invert the referenced bit.
     */
    reference& flip() noexcept;


  private:
    /// Pointer to the block containing the referenced bit.
    std::atomic<Block_t>* m_block;

    /// Mask of the referenced bit within the block.
    Block_t m_mask;
  };


  /**
   * @brief Return the value of one bit.
   * @param bit The number of the bit to test.
   */
  bool operator[] (bit_t bit) const;
  

  /**
   * @brief Return a reference to one bit.
   * @param bit The number of the bit to reference.
   *
   * The reference will be invalidated by calls to @c insert() or @c operator=.
   * Effects are undefined if @c bit is past the end of the set.
   */
  reference operator[] (bit_t bit);

  
  //@}
  //========================================================================
  /** @name Iterator operations. */
  //@{


  /**
   * @brief Iterator over all 1 bits in the set.
   *
   * This iterator will visit all 1 bits in the set (that is, it works
   * like the iterator for a set<bit_t>).  The state of an iterator consists of:
   *
   *   - m_bit -- The number of the current bit we're referencing.
   *   - m_data -- Pointer to the current block we're referencing.
   *   - m_cache -- Cached value of the current block.  This is shifted such that
   *                the referenced bit has just fallen off the right.
   *   - m_end -- Pointer to one past the last block we should consider
   *              in the iteration.  (This may be less than the full size
   *              of the set if we know that the end of the set is all 0's.
   *              This is tracked by Impl::m_hwm.)
   *
   * The end iterator is marked by setting m_bit to -1; none of the other fields
   * matter in that case.  Thus, we can compare iterators by simply comparing m_bit.
   *
   * To initialize an iterator in begin(), we set the iterator to point to the
   * block preceding the first block of the set, with the cache set to 0.
   * We can then use the increment operator to advance the iterator to the
   * first 1 bit.
   */
  struct const_iterator
  {
    typedef std::forward_iterator_tag iterator_category;
    typedef size_t value_type;
    typedef ptrdiff_t difference_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;


  private:
    friend ConcurrentBitset;


    /**
     * @brief Constructor.
     * @param cache Cached block at the current iteration point, shifted such
     *              that bit number @c bit has just been shifted off the right.
     * @param bit Bit number the at which the iterator is currently pointing.
     * @param data Pointer to the block at which the iterator is currently pointing.
     * @param end One past the last block of the iteration range.
     */
    const_iterator (Block_t cache,
                    bit_t bit,
                    const std::atomic<Block_t>* data,
                    const std::atomic<Block_t>* end);


  public:
    /**
     * @brief Return the bit number which the iterator is currently referencing.
     */
    bit_t operator*() const;


    /**
     * @brief Advance the iterator to the next set bit (preincrement).
     */
    const_iterator& operator++();


    /**
     * @brief Advance the iterator to the next set bit (postincrement).
     */
    const_iterator operator++(int);


    /**
     * @brief Compare two iterators.
     * @param other The other iterator to compare.
     */
    bool operator== (const const_iterator& other) const;


    /**
     * @brief Compare two iterators.
     * @param other The other iterator to compare.
     */
    bool operator!= (const const_iterator& other) const;


  private:
    /// Cache of the block to which we're currently pointing.
    /// Should be shifted such that the bit we're referencing has just
    /// been shifted off the right.
    Block_t m_cache;

    /// Bit number which we're currently referencing.
    bit_t m_bit;

    /// Pointer to the block containing the bit which we're currently referencing.
    const std::atomic<Block_t>* m_data;


    /// Pointer to one past the last block in the set.
    const std::atomic<Block_t>* m_end;
  };


  /**
   * @brief Return a begin iterator.
   */
  const_iterator begin() const;


  /**
   * @brief Return an end iterator.
   */
  const_iterator end() const;


  /**
   * @brief If bit @c bit is set, return an iterator pointing to it.
   *        Otherwise, return an end iterator.
   * @param bit Bit number to test.
   */
  const_iterator find (bit_t bit) const;



  //@}
private:
  //========================================================================
  /** @name Implementation. */
  //@{


  /**
   * @brief Find number of blocks needed to hold a given number of bits.
   * @param nbits The number of bits.
   */
  static bit_t nBlocks (bit_t nbits);


  /**
   * @brief Create a new, uninitialized implementation object.
   * @param nbits Number of bits to allocate.
   *
   * This will allocate memory for the Impl object,
   * but will does not run the constructor.
   */
  Impl* newImpl (bit_t nbits);


  /**
   * @brief Expand the container.
   * @param new_nbits The desired new size of the container.
   */
  void expand (bit_t new_nbits);


  /**
   * @brief Expand the container: out-of-line portion.
   * @param new_nbits The desired new size of the container.
   */
  void expand_ool (bit_t new_nbits);


  /**
   * @brief Implementation object.
   *
   * An instance of this holds the set data for a fixed size.
   * If the set needs to be expanded, a new implementation object must be allocated
   * and the data copied.
   *
   * This object consists of a fixed header, followed by a variable-sized array
   * containing the actual set data.  In this class, the array is declared with
   * a size of 1; however, we allocate these objects (using the @c newImpl function)
   * with enough space to hold the entire set.
   *
   * This class also contains the basic methods for operating on the set.
   */
  class Impl
  {
  public:
    /**
     * @brief Constructor.
     * @param nbits Number of bits in the set.
     */
    Impl (bit_t nbits);


    /**
     * @brief Copy constructor.
     * @brief Other object to copy.
     * @brief Number of bits to use for this container.
     *
     * If @c nbits is smaller than the size of @c other, then the size of @c other
     * will be used instead.
     */
    Impl (const Impl& other, bit_t nbits = 0);


    /**
     * @brief Copy from another instance.
     * @param other Object from which to copy.
     *
     * This does not change the size of the container.
     * If This container is larger than @c other, then the remainder will be
     * filled with zeros.  If @c other is larger than this container, then the
     * remainder will be ignored.
     */
    void assign (const Impl& other);


    /** 
     * @brief Return the number of bits in the set.
     */
    bit_t nbits() const;


    /**
     * @brief Test to see if a bit is set.
     * @param bit Number of the bit to test.
     * @return true if the bit is set; false otherwise.
     *
     * Returns false if @c bit is beyond the end of the set.
     */
    bool test (bit_t bit) const;


    /**
     * @brief Count the number of 1 bits in the set.
     */
    bit_t count() const;


    /**
     * @brief Return true if there are no 1 bits in the set.
     */
    bool none() const;

    
    /**
     * @brief Return true if all bits in the set are 1.
     */
    bool all() const;


    /**
     * @brief Return a pointer to the block containing @c bit.
     * @param bit Desired bit number.
     *
     * Returns nullptr if @c bit is past the end of the set.
     */
    std::atomic<Block_t>* block (bit_t bit);


    /**
     * @brief Turn on one bit.
     * @param bit The bit to turn on.
     *
     * Does nothing if @c bit beyond the end of the set.
     */
    void set (bit_t bit);

    
    /**
     * @brief Turn off one bit.
     * @param bit The bit to turn off.
     *
     * Does nothing if @c bit beyond the end of the set.
     */
    void reset (bit_t bit);


    /**
     * @brief Flip the value of one bit.
     * @param bit The bit to turn flip.
     *
     * Does nothing if @c bit beyond the end of the set.
     */
    void flip (bit_t bit);


    /**
     * @brief Clear all bits in the set.
     *
     * This operation is not necessarily atomic; a simultaneous read may be able
     * to see the operation partially done.
     */
    void clear();


    /**
     * @brief Turn on all bits in the set.
     *
     * This operation is not necessarily atomic; a simultaneous read may be able
     * to see the operation partially done.
     */
    void set();


    /**
     * @brief Flip the state of all bits in the set.
     *
     * This operation is not necessarily atomic; a simultaneous read may be able
     * to see the operation partially done.
     */
    void flip();


    typedef void operator_t (std::atomic<Block_t>& a, Block_t v);
    /**
     * @brief Apply a binary operation.
     * @param op Operation to apply.
     * @param other Second set for the operation.
     *
     * Each block B in this set is replaced by B OP OTHER, where OTHER is
     * the corresponding block in the other container.  (If this set
     * is larger than @c other, then the trailing blocks will be 0.)
     */
    void operate (operator_t op, const Impl& other);


    /**
     * @brief Compare with another set.
     * @param other Other set with which to compare.
     */
    bool operator== (const Impl& other) const;



    /**
     * @brief Return an iterator referencing the first 1 bit.
     */
    const_iterator begin() const;


    /**
     * @brief Return the end iterator.
     */
    const_iterator end() const;


    /**
     * @brief If bit @c bit is set, return an iterator pointing to it.
     *        Otherwise, return an end iterator.
     * @param bit Bit number to test.
     */
    const_iterator find (bit_t bit) const;


  private:
    /// Number of bits in the container.
    size_t m_nbits;

    /// Number of blocks in the container.
    size_t m_nblocks;

    /// High-water mark: index of last block with a 1 bit.
    std::atomic<size_t> m_hwm;

    /// The set data.
    /// The implementation objects are allocated such that there are actually
    /// m_nblocks entries available in this array.
    std::atomic<Block_t> m_data[1];
  };


  //@}


private:
  /// The current implementation object.
  std::atomic<Impl*> m_impl;

  /// Old implementation objects, pending deletion.
  std::vector<Impl*> m_garbage;

  /// Mutex used for synchronization when switching to a new implementation object.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutex_t m_mutex;
};


} // namespace CxxUtils


#include "CxxUtils/ConcurrentBitset.icc"

  
#endif // not CXXUTILS_CONCURRENTBITSET_H
