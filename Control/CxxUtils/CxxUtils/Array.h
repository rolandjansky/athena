// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  Array.h
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Simple multidimensional arrays.
 *
 * This file defines several families of templated classes that act like
 * multidimensional arrays.
 *
 * There are three families of classes:
 *  - @c Array<N> provides read-only access to an array.
 *    Indexing with [] returns an @c Array<N-1>.  @c Array<0>
 *    is special; it doesn't support indexing, but does support
 *    implicit conversion to the element type, @c Arrayelt.
 *    The @c Array classes do not own their storage (and thus indexing
 *    does not require a complete copy).  Rather, the complete data
 *    for the array are stored in an instance of @c Arrayrep.
 *    An @c Array holds a pointer to an @c Arrayrep and an offset
 *    into the data.
 *
 *  - @c WritableArray is similar to @c Array, except that it may
 *    be written to as well.  An @c Array may be initialized from
 *    a @c WritableArray, but not the other way around.
 *
 *  - @c WritableArrayData is a @c WritableArray together with
 *    an @c Arrayrep.  This is a convenient way of creating
 *    from scratch an array to which you can write.
 */


#ifndef CXXUTILS_ARRAY_H
#define CXXUTILS_ARRAY_H


#include "CxxUtils/Arrayrep.h"
#include "CxxUtils/concepts.h"
#include <iterator>


namespace CxxUtils {


// Forward declaration.
template <unsigned int N>
class ArrayIterator;


//**********************************************************************


/**
 * @class ArrayIteratorChooser
 * @brief Helper for defining iterators over @c Array's.
 *
 * For @c Array<N> with @c N >= 2, we want to use @c ArrayIterator<N> as the
 * iterator class.  However, for @c Array<1>, we want to use just
 * an @c Arrayelt*.  I'm too lazy to either specialize @c ArrayIterator<1>
 * or to add a new @c Array<1> specialization.  So this is an easier way
 * of achieving the same effect.  @c Array<N> uses as its iterator
 * type @c ArrayIteratorChooser<N>::const_iterator; this small class then gets
 * specialized for @c N == 1.  We also need to provide a way to create
 * the iterators (since it will be done differently in the two cases).
 * This is supplied by the @c make_iterator method.
 */
template <unsigned int N>
class ArrayIteratorChooser
{
public:
  /// Iterator type for an @c Array<N>.
  typedef ArrayIterator<N> const_iterator;


  /**
   * @brief Construct an @c Array<N>::const_iterator.
   * @param rep @c Arrayrep from which to initialize the iterator.
   * @param offs Offset of the first element referenced by the iterator
   *             within @a rep.
   * @return The new iterator.
   */
  static const_iterator make_iterator (const Arrayrep* rep, unsigned int offs);
};


/**
 * @class ArrayIteratorChooser<1>
 * @brief Helper for defining iterators over @c Array's, specialized
 *        for @c N == 1.
 *
 * For @c N == 1, we specialize so that @c Array<N> uses
 * @c Arrayelt* as its iterator type.
 */
template <>
class ArrayIteratorChooser<1>
{
public:
  /// Iterator type for an @c Array<1>.
  typedef const Arrayelt* const_iterator;


  /**
   * @brief Construct an @c Array<1>::const_iterator.
   * @param rep @c Arrayrep from which to initialize the iterator.
   * @param offs Offset of the first element referenced by the iterator
   *             within @a rep.
   * @return The new iterator.
   */
  static const_iterator make_iterator (const Arrayrep* rep, unsigned int offs);
};


//**********************************************************************


/**
 * @class Array
 * @brief Read-only multidimensional array.
 *
 * @c Array<N> provides read-only access to an array.
 * Indexing with [] returns an @c Array<N-1>.  @c Array<0>
 * is special; it doesn't support indexing, but does support
 * implicit conversion to the element type, @c Arrayelt.
 * The @c Array classes do not own their storage (and thus indexing
 * does not require a complete copy).  Rather, the complete data
 * for the array are stored in an instance of @c Arrayrep.
 * An @c Array holds a pointer to an @c Arrayrep and an offset
 * into the data.
 */
template <unsigned int N>
class Array
{
public:
  /// The number of dimensions of the array.
  enum { NDIM = N };

  /// The iterator for this container.
  typedef typename ArrayIteratorChooser<N>::const_iterator const_iterator;


  /**
   * @brief Default constructor.
   *
   * This produces an invalid @c Array that is not associated with
   * an @c Arrayrep.  @c valid() will return @c false for such an array.
   * The only other things that it is legal to do with an invalid array
   * are to assign to it (which may make it valid) or request its
   * size with size() (which will always return 0).
   */
  Array ();

  /**
   * @brief Constructor.
   * @param rep @c Arrayrep from which to initialize the array.
   *
   * Initialize an array from an @c Arrayrep.  The new array will
   * represent the entire @c Arrayrep.  The dimension @c N must
   * match the length of the @c Arrayrep's shape.
   */
  Array (const Arrayrep& rep);

  /**
   * @brief Test for validity.
   * @return True if the @c Array is associated with an @c Arrayrep,
   *         false if not.
   */
  bool valid() const;

  /**
   * @brief Return the array shape.
   * @return The array shape.
   *
   * The array shape is vector with one element for each array dimension,
   * giving the size of the array along that dimension.
   */
  std::vector<unsigned int> shape() const;

  /**
   * @brief Return the size of the array along one dimension.
   * @param dim The dimension of the size to retrieve.
   *            Must be less than the number of dimensions.
   * @return The array size along dimension @c dim.
   */
  unsigned int size (unsigned int dim = 0) const;

  /**
   * @brief Array indexing.
   * @param i The desired index.  Must be less than the array size
   *          along this dimension.
   * @return The @a i'th @c N-1 dimensional subarray in the array.
   *
   * Note that this operation is not available if @c N is 0.
   */
  Array<N-1> operator[] (unsigned int i) const;

  /**
   * @brief Return a direct pointer to array elements.
   * @return A pointer to the first array elements.
   *
   * Subsequent elements follow in standard C indexing order.
   */
  const Arrayelt* ptr () const;


  /**
   * @brief Return an iterator pointing at the beginning of the container.
   * @return An iterator pointing at the beginning of the container.
   */
  const_iterator begin () const;


  /**
   * @brief Return an iterator pointing past the end of the container.
   * @return An iterator pointing past the end of the container.
   */
  const_iterator end   () const;


  /**
   * @brief Creates a text representation of the array content.
   * @param std::ostream where the text should be written
   *
   * Writes the content of the array to a ostream. The sub-arrays are
   * enclosed by square-brackets and separated by commas.
   */
  void write_array (std::ostream& stream) const;


  // Protected, not private, so that they can be accessed by @c WritableArray.
protected:
  /**
   * @brief Private constructor for array indexing.
   * @param rep @c Arrayrep from which to initialize the array.
   * @param offs Offset of the first element of the new array
   *             within @a rep.
   *
   * This is a private constructor used to make the @c Array
   * instances returned from an indexing operation.
   */
  Array (const Arrayrep& rep, unsigned int offs);

  // These classes need to call the above protected constructor.
  friend class Array<N+1>;
  friend class ArrayIterator<N+1>;

  /// Pointer to the representation.
  /// Null if this instance was created using the default constructor.
  const Arrayrep* m_rep;

  /// Offset in the @c Arrayrep's data of the first element of this array.
  unsigned int m_offs;
};


/**
 * @class Array<0>
 * @brief Read-only multidimensional array, specialized for @c N=0.
 *
 * This is a specialization of @c Array for the case @c N=0,
 * i.e., a scalar.  It does not support indexing, but it does support
 * implicit conversions to @c Arrayelt, as well as an explicit conversion
 * to an integer.
 */
template <>
class Array<0>
{
public:
  /// The number of dimensions of the array.
  enum { NDIM = 0 };

  /**
   * @brief Default constructor.
   *
   * This produces an invalid @c Array that is not associated with
   * an @c Arrayrep.  @c valid() will return @c false for such an array.
   * The only other thing that it is legal to do with an invalid array
   * is to assign to it (which may make it valid).
   */
  Array ();

  /**
   * @brief Constructor.
   * @param rep @c Arrayrep from which to initialize the array.
   *
   * Initialize an array from an @c Arrayrep.  The new array will
   * represent the entire @c Arrayrep.  The dimension @c N must
   * match the length of the @c Arrayrep's shape.
   */
  Array (const Arrayrep& rep);

  /**
   * @brief Test for validity.
   * @return True if the @c Array is associated with an @c Arrayrep,
   *         false if not.
   */
  bool valid() const;

  /**
   * @brief Return the array shape.
   * @return The array shape.
   *
   * The array shape is vector with one element for each array dimension,
   * giving the size of the array along that dimension.
   * For @c Array<0>, this will always be an empty array.
   */
  std::vector<unsigned int> shape() const;

  /**
   * @brief Return the size of the array along one dimension.
   * @param dim The dimension of the size to retrieve.
   *            Must be less than the number of dimensions.
   * @return The array size along dimension @c dim.
   *
   * For @c Array<0>, @a dim must be 0, and the function
   * will always return 0.
   */
  unsigned int size (unsigned int dim=0) const;

  /**
   * @brief Convert to a number.
   * @return The @c Array<0> contents as a number.
   */
  operator Arrayelt() const;

  /**
   * @brief Convert to an integer.
   * @return The @c Array<0> contents as an integer.
   */
  int asint () const;


  /**
   * @brief Creates a text representation of the array content.
   * @param std::ostream where the text should be written
   *
   * Writes the content of the array to a ostream. The sub-arrays are
   * enclosed by square-brackets and separated by commas.
   */
  void write_array (std::ostream& stream) const;


  // Protected, not private, so that they can be accessed by @c WritableArray.
protected:
  /**
   * @brief Private constructor for array indexing.
   * @param rep @c Arrayrep from which to initialize the array.
   * @param offs Offset of the first element of the new array
   *             within @a rep.
   *
   * This is a private constructor used to make the @c Array
   * instances returned from an indexing operation.
   */
  Array (const Arrayrep& rep, unsigned int offs);

  // This class needs to call the above protected constructor.
  friend class Array<1>;

  /// Pointer to this array's single element.
  /// Null if this instance was created using the default constructor.
  const Arrayelt* m_elt;
};


template <unsigned int N>
std::ostream& operator<< (std::ostream& s, const Array<N>& a);


//**********************************************************************


/**
 * @class ArrayIterator
 * @brief Iterator class for @c Array<N>.
 *
 * This serves as an iterator class for @c Array<N>, where
 * @c N >= 2.  It gives a @c Array<N-1> as @c value_type.
 * Only @c const access is supported.
 *
 * This class is almost, but not quite, a random access iterator.
 * @c operator*  and @c operator[] returns a @c value_type instead
 * of a @c reference.  And @c operator-> returns a proxy.
 * (The issues here are similar to those encountered with
 * @c std::vector<bool>.)  But it should mostly work as you expect.
 */
template <unsigned int N>
class ArrayIterator
  : public std::iterator<std::random_access_iterator_tag, const Array<N-1> >
{
public:
  // Typedefs.
  /// Shorthand for the base class.
  typedef std::iterator<std::random_access_iterator_tag, const Array<N-1> >
             base_iterator;

  /// Standard @c iterator_category typedef, from the base class.
  typedef typename base_iterator::iterator_category iterator_category;

  /// Standard @c value_type typedef, from the base class.
  typedef typename base_iterator::value_type        value_type;

  /// Standard @c difference typedef, from the base class.
  typedef typename base_iterator::difference_type   difference_type;

  /// Standard @c reference typedef, from the base class.
  typedef typename base_iterator::reference         reference;


  /**
   * @brief Proxy to return from @c operator>.
   *
   * @c operator-> is required to either return an actual C++
   * pointer or another object that has a valid @c operator->.
   * This makes things a bit tricky if you're using proxies.
   * We have the @c ArrayIterator @c operator-> return
   * an instance of this proxy class, which contains an
   * instance of the @c Array proxy.  The @c operator->
   * of this proxy then returns a pointer to the contained
   * @c Array.  This will work for most common usages.
   * However, the pointer one gets from the @c operator->
   * here will only be valid until the proxy is destroyed.
   */
  class pointer
  {
  public:
    /**
     * @brief Proxy constructor.
     * @param i The iterator that is being dereferenced.
     */
    pointer (const ArrayIterator& i);


    /**
     * @brief Dereference the proxy.
     * @return A copy of the @c Array proxy.
     */
    value_type operator* () const;


    /**
     * @brief Dereference the proxy.
     * @return A pointer to the @c Array proxy.
     *         This proxy is only until the @c pointer instance
     *         is destroyed.
     */
    const value_type* operator-> () const;


  private:
    /// The contained @c Array proxy instance.
    value_type m_a;
  };


  /**
   * @brief Default constructor.
   *        Makes an invalid iterator.
   */
  ArrayIterator ();


  /**
   * @brief Constructor from @c Arrayrep and offset.
   * @param rep The underlying array representation.
   * @param offs The offset in the representation of the
   *             first element referenced by this iterator.
   */
  ArrayIterator (const Arrayrep* rep, unsigned int offs);

  // Use default copy ctor and assignment.

  /**
   * @brief Equality comparison.
   * @param other The other object with which to compare.
   * @return True if the iterators are equal.
   */
  bool operator== (const ArrayIterator& other) const;


  /**
   * @brief Inequality comparison.
   * @param other The other object with which to compare.
   * @return True if the iterators are not equal.
   */
  bool operator!= (const ArrayIterator& other) const;


  /**
   * @brief Less-than comparison.
   * @param other The other object with which to compare.
   * @return True if this iterator is less than @a other.
   *         This will always return false for iterators
   *         over different arrays.
   */
  bool operator<  (const ArrayIterator& other) const;


  /**
   * @brief Greater-than comparison.
   * @param other The other object with which to compare.
   * @return True if this iterator is greater than @a other.
   *         This will always return false for iterators
   *         over different arrays.
   */
  bool operator>  (const ArrayIterator& other) const;


  /**
   * @brief Less-than-or-equal comparison.
   * @param other The other object with which to compare.
   * @return True if this iterator is less than or equal to @a other.
   *         This will always return false for iterators
   *         over different arrays.
   */
  bool operator<= (const ArrayIterator& other) const;


  /**
   * @brief Greater-than-or-equal comparison.
   * @param other The other object with which to compare.
   * @return True if this iterator is less than or equal to @a other.
   *         This will always return false for iterators
   *         over different arrays.
   */
  bool operator>= (const ArrayIterator& other) const;


  /**
   * @brief Dereference the iterator.
   * @return The value that the iterator points to.
   *         Note that this method returns a @c value_type, not
   *         a @c reference.  (Thus, this class does not quite
   *         conform to the iterator requirements.)
   */
  value_type operator* () const;


  /**
   * @brief Dereference the iterator.
   * @return A proxy for the iterator element.
   *
   * This method will return a proxy for the array, which you
   * can then dereference.  Note that if you get a C++ pointer
   * from this, then it will be valid only until the proxy
   * object gets destroyed.
   */
  pointer operator-> () const;


  /**
   * @brief Advance the iterator.
   * @returns This iterator.
   */
  ArrayIterator<N>& operator++ ();


  /**
   * @brief Advance the iterator.
   * @returns The iterator before being advanced.
   */
  ArrayIterator<N>  operator++ (int);


  /**
   * @brief Back up the iterator.
   * @returns This iterator.
   */
  ArrayIterator<N>& operator-- ();


  /**
   * @brief Back up the iterator.
   * @returns The iterator before being backed up.
   */
  ArrayIterator<N>  operator-- (int);


  /**
   * @brief Array indexing relative to the iterator.
   * @param n The array index.
   * @return The array item at an offset of @a n from the
   *         current iterator position.
   *         Note that this method returns a @c value_type, not
   *         a @c reference.  (Thus, this class does not quite
   *         conform to the iterator requirements.)
   */
  value_type operator[] (difference_type n) const;


  /**
   * @brief Advance the iterator.
   * @param n Number of steps by which to advance the iterator.
   * @return This iterator.
   */
  ArrayIterator<N>& operator+= (difference_type n);


  /**
   * @brief Return a new iterator pointing @a n steps ahead.
   * @param n Number of steps by which to advance.
   * @return The new iterator.
   */
  ArrayIterator<N>  operator+  (difference_type n) const;


  /**
   * @brief Back up the iterator.
   * @param n Number of steps by which to advance the iterator.
   * @return This iterator.
   */
  ArrayIterator<N>& operator-= (difference_type n);


  /**
   * @brief Return a new iterator pointing @a n steps behind.
   * @param n Number of steps by which to back up.
   * @return The new iterator.
   */
  ArrayIterator<N>  operator-  (difference_type n) const;


  /**
   * @brief Return the difference between two iterators.
   * @param other The other iterator for the comparison.
   * @return The number of elements difference between
   *         this iterator and @a other.
   *         Undefined if the two iterators do not point
   *         into the same array.
   */
  difference_type    operator- (const ArrayIterator& other) const;


private:
  /// The underlying array representation.
  const Arrayrep* m_rep;

  /// Offset into the representation's data array of the first element
  /// referred to by this iterator.
  unsigned int m_offs;
};


//**********************************************************************


/**
 * @class WritableArray
 * @brief Read-write multidimensional array.
 *
 * This class derives from @c Array<N>.  It allows writing into
 * the array, but otherwise works in the same way.
 */
template <unsigned int N>
class WritableArray
  : public Array<N>
{
public:
  /**
   * @brief Constructor.
   * @param rep @c Arrayrep from which to initialize the array.
   *
   * Initialize an array from an @c Arrayrep.  The new array will
   * represent the entire @c Arrayrep.  The dimension @c N must
   * match the length of the @c Arrayrep's shape.
   */
  WritableArray (Arrayrep& rep);

  /**
   * @brief Array indexing.
   * @param i The desired index.  Must be less than the array size
   *          along this dimension.
   * @return The @a i'th @c N-1 dimensional subarray in the array.
   *
   * Note that this operation is not available if @c N is 0.
   */
  WritableArray<N-1> operator[] (unsigned int i) const;

  /**
   * @brief Return a direct pointer to array elements.
   * @return A pointer to the first array elements.
   *
   * Subsequent elements follow in standard C indexing order.
   */
  Arrayelt* ptr ();


private:
  /**
   * @brief Private constructor for array indexing.
   * @param rep @c Arrayrep from which to initialize the array.
   * @param offs Offset of the first element of the new array
   *             within @a rep.
   *
   * This is a private constructor used to make the @c Array
   * instances returned from an indexing operation.
   */
  WritableArray (Arrayrep& rep, unsigned int offs);

  // This class needs to call the above protected constructor.
  friend class WritableArray<N+1>;

  /// The underlying array representation, non-const copy.
  Arrayrep* m_rep_nc;
};


/**
 * @class WritableArray<N>
 * @brief Read-write multidimensional array, specialized for @c N=0.
 *
 * This class derives from Array<0>.  It allows writing into
 * the (single) element, but otherwise works in the same way.
 */
template <>
class WritableArray<0>
  : public Array<0>
{
public:
  /**
   * @brief Constructor.
   * @param rep @c Arrayrep from which to initialize the array.
   *
   * Initialize an array from an @c Arrayrep.  The new array will
   * represent the entire @c Arrayrep.  The dimension @c N must
   * match the length of the @c Arrayrep's shape.
   */
  WritableArray (Arrayrep& rep);

  /**
   * @brief Assignment.
   * @param elt The RHS of the assignment.
   * @return This object.
   *
   * Assign into the array.
   */
  WritableArray<0>& operator= (Arrayelt elt);


private:
  /**
   * @brief Private constructor for array indexing.
   * @param rep @c Arrayrep from which to initialize the array.
   * @param offs Offset of the first element of the new array
   *             within @a rep.
   *
   * This is a private constructor used to make the @c Array
   * instances returned from an indexing operation.
   */
  WritableArray (Arrayrep& rep, unsigned int offs);

  // This class needs to call the above protected constructor.
  friend class WritableArray<1>;

  /// Pointer to this array's single element, non-const copy.
  /// Null if this instance was created using the default constructor.
  Arrayelt* m_elt_nc;
};


//**********************************************************************


/**
 * @class WritableArrayData<N>
 * @brief A @c WriteableArray together with an @c Arrayrep.
 *
 * This is a convenient way of creating from scratch an array
 * to which you can write.
 */
template <unsigned int N>
class WritableArrayData
  : private Arrayrep,
    public WritableArray<N>
{
public:
  /**
   * @brief Constructor.
   * @param shape The shape of the array, as a C array.
   *              Should be @c N elements long.
   *
   * The shape is the size of the array along each dimension.
   */
  WritableArrayData (const unsigned int shape[]);

  /**
   * @brief Constructor.
   * @param shape The shape of the array, as a std::vector.
   *              Should be @c N elements long.
   *
   * The shape is the size of the array along each dimension.
   */
  WritableArrayData (const std::vector<unsigned int>& shape);
};


/**
 * @brief Helper to convert from an @x Arrayrep to a scalar type.
 * @param rep Representation object to convert.
 * @param x[out] Result of the conversion.
 */
template <class T>
  ATH_REQUIRES(std::assignable_from<T&, float>)
void fromArrayrep (const CaloRec::Arrayrep& rep, T& x);


/**
 * @brief Helper to convert from an @x Arrayrep to an @c Array.
 * @param rep Representation object to convert.
 * @param x[out] Result of the conversion.
 */
template <unsigned int N>
void fromArrayrep (const CaloRec::Arrayrep& rep, CxxUtils::Array<N>& x);


} // namespace CxxUtils


#include "CxxUtils/Array.icc"


// Backwards compatibility.
namespace CaloRec {
  using CxxUtils::Array;
  using CxxUtils::WritableArray;
  using CxxUtils::WritableArrayData;
}


#endif // not CXXUTILS_ARRAY_H

