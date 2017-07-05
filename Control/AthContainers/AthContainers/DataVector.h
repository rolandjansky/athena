// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataVector.h 794113 2017-01-26 22:01:47Z ssnyder $

/**
 * @file  AthContainers/DataVector.h
 * @author scott snyder, Paolo Calafiura, etc
 * @date May 2005; rewritten from earlier version.
 * @brief An STL vector of pointers that by default owns its pointed-to
 *        elements.
 *
 * For further information, see
 *    <https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/DataVector>
 *
 * Introduction
 * ============
 *
 * A @c DataVector<T> acts like a @c std::vector<T*>, except that it can
 * optionally manage the memory that it contains.  The constructors take
 * an (optional) extra argument, which can be either @c SG::OWN_ELEMENTS
 * or @c SG::VIEW_ELEMENTS (defaulting to @c SG::OWN_ELEMENTS, except for a
 * copy constructor).  This tells whether the @c DataVector owns
 * its contained elements or not.
 *
 * Ownership issues
 * ================
 *
 * If a @c DataVector owns its elements, then they are deleted
 * when the container itself is.  Further, they are deleted by actions
 * which erase elements from the container (i.e, @c erase(), @c pop_back()).
 * A replacement (such as <code> v[0] = new T </code>) will result in the
 * old element being deleted and the container taking ownership
 * of the new element.  It is an error to assign directly between
 * two owning containers (<code>v1[0] = v2[0]</code>).
 * To remove an element from a container and acquire ownership,
 * use @c swapElement.
 *
 * Beware of ownership issues when you modify a @c DataVector.
 * Obviously you should not delete explicitly a @c DataVector element.
 * A @c DataVector should never have two elements pointing to the same object.
 * This may seem obvious but certain std algorithms (e.g. @c remove_if) may 
 * leave a @c DataVector with two copies of the same element in the 
 * "left-over" range.  To avoid a crash when clearing the vector (e.g. in the
 * destructor we have introduced a @f$n\log n@f$ helper function that searches
 * and removes duplicates in the @c DataVector.  This is used by the destructor
 * by @c clear() and by <code>erase(first, last)</code>.  As this may change
 * in the future to improve performance, do not rely on this functionality
 * and do avoid introducing duplicated elements in a @c DataVector.
 *
 * All these cautions do not apply when a @c DataVector it is created with 
 * the flag @c SG::VIEW_ELEMENTS (see <code>enum OwnershipPolicy</code>)
 * and hence does not own its elements. This is typically used
 * to have @c DataVector elements allocated by @c DataPool.
 * Otherwise consider the cleaner alternative of using a @c vector<T*>.
 *
 * The interface for @c DataVector should be mostly compatible with that
 * of @c std::vector.  There are a few differences which should not make
 * much difference in practice.  For example, methods which would
 * return a reference return a proxy object instead.  Also @c value_type
 * is used instead of @c const_reference; this is justified by the fact
 * that the elements are always pointers.
 *
 * Note that algorithms which modify their range may not work
 * correctly if the container owns its contents.  Specializations
 * that work properly for @c DataVector are available for some algorithms.
 * These include:
 *  - @c std::sort
 *  - @c std::stable_sort
 *  - @c std::partial_sort
 *  - @c std::remove
 *  - @c std::remove_if
 *  - @c std::unique
 *  - @c std::reverse
 *  - @c std::rotate
 *  - @c std::random_shuffle
 *  - @c std::partition
 *  - @c std::stable_partition
 *
 * Alternately, for @c sort(), the @c sort() methods defined in @c DataVector
 * may be used.  Or do the sorting in a view @c DataVector
 * or a plain @c std::vector.
 *
 * There are a few other additions to the standard @c std::vector interface.
 *  - The method @c stdcont may be used to get access to the underlying
 *    @c std::vector representation.
 *  - The type @c PtrVector is the type of the underlying @c std::vector.
 *    @c BaseContainer is a synonym for this.
 *  - The method @c swapElement may be used to get ownership of an element
 *    back from a @c DataVector.
 *  - The method @c sort provides an interface
 *    to @c std::sort that works even if the container owns its contents.
 *  - The method @c ownPolicy returns the ownership policy of the container.
 *  - An additional overload of @c clear() is provided that takes as
 *    an argument a new ownership policy for the container.  This is
 *    the only way to change the ownership policy, save for swapping
 *    or moving the container.
 *
 * Note that since @c DataVector\<T> has an element type of @c T*,
 * it is not possible to directly insert a <code>const T*</code>.
 * If you want to do that, see @c ConstDataVector.  (In some cases,
 * such as if the destination container is not being recorded in StoreGate,
 * it may be more appropriate to simply use a
 * <code>std::vector<const T*></code>.)  Don't just use a @c const_cast!
 *
 * Inheritance
 * ===========
 *
 * @c DataVector's may inherit from one another.
 * If you have class @c D which derives from class @c B, you can set things
 * up so that @c DataVector\<D> derives from @c DataVector\<B>.  This allows
 * you do to the same sort of conversions on the @c DataVector's as on the
 * element pointers themselves.  The key to doing this is to add the
 * declaration
 *
 *@code
 *   DATAVECTOR_BASE (D, B);
 @endcode
 *
 * before using @c DataVector\<D>.  A few caveats about doing this.
 * The pointers are actually stored in the base @c DataVector instance,
 * and the type that @c stdcont returns will reflect this.
 * For example, in the example given above, @c DataVector<D>::stdcont()
 * will return a reference to std::vector\<B*>.  Second, in order
 * to preserve the invariant
 * that a @c DataVector\<D> contains only elements that actually derive
 * from @c D, while at the same time not requiring that the contained
 * objects be polymorphic, there is a restriction that you cannot
 * insert into a @c DataVector if you're not referring to it as the
 * most derived type (even if such an insertion would not actually
 * break the invariant).  This is implemented as a runtime check.
 *
 * Example:
 *
 *@code
 *   DataVector<D> vd;
 *   vd.push_back (new D);  // This is ok.
 *   vd.push_back (new B);  // This will give a compilation error
 *                             (it would break the invariant).
 *   DataVector<B>& vb = vd;
 *   vb.push_back (new B);  // This will give a run-time error
 *                             (it breaks the invariant).
 *   vb.push_back (new D);  // This will also give a run-time error.
 *                             (It's actually ok, but there's no good way
 *                             to distinguish it from the previous case.)
 @endcode
 *
 * Note also this (related to a common atlas idiom).  If we have the above,
 * and also:
 *
 *@code
 *   class B_Vector : public DataVector<B> { ... };
 *   class D_Vector : public DataVector<D> { ... };
 @endcode
 *
 * Then a @c D_Vector will be convertible to a DataVector\<B>, but _not_
 * to a @c B_Vector.
 *
 * Multiple and virtual inheritance are also supported.  In this case,
 * use <code>DATAVECTOR_VIRTBASES</code><em>n</em> (where @e n is 1, 2, or 3)
 * instead of @c DATAVECTOR_BASE.  Example: Given:
 *
 *@code
 *   class M { ... };
 *   class N : virtual public M { ... };
 *   class O : virtual public M { ... };
 *   class P : virtual public N, virtual public O { ... };
 @endcode
 *
 * declare this with
 *
 *@code
 *   DATAVECTOR_VIRTBASES1(N, M);
 *   DATAVECTOR_VIRTBASES1(O, M);
 *   DATAVECTOR_VIRTBASES2(P, N, O);
 @endcode
 *
 * There is a restriction that there must be a unique base class that
 * does not derive from anything else.  For example, the diamond configuration
 * above is ok, but this would not be:
 *
 *@code
 *   class L { ... };
 *   class M { ... };
 *   class N : virtual public M, virtual public L { ... };
 *   class O : virtual public M { ... };
 *   class P : virtual public N, virtual public O { ... };
 *
 *   DATAVECTOR_VIRTBASES2(N, M, L);
 *   DATAVECTOR_VIRTBASES1(O, M);
 *   DATAVECTOR_VIRTBASES2(P, N, O);
 @endcode
 *
 * Note, however, that you don't have to tell @c DataVector about the complete
 * hierarchy; leaving the @c L out of @c DATAVECTOR_VIRTBASES would work
 * (you just wouldn't be able to convert to @c DataVector\<L>).
 *
 * If you use @c DATAVECTOR_VIRTBASES, there is an additional time penalty
 * to retrieve elements from the collection.  This does not apply
 * for @c DATAVECTOR_BASES.
 *
 * All applicable @c DATAVECTOR_* macros must be visible at the point at which
 * a @c DataVector is instantiated.  A confusing compilation error is
 * likely to result otherwise.  Note that this means that if you have
 * the @c DATAVECTOR_* macros within a container header file, then
 * the header for the derived container must include the header
 * for the base container.   Be alert to this when converting existing
 * code to use the inheritance scheme.  For example, if class D2 derives
 * from D which derives from B:
 *
 * BVec.h:
 *@code
 *  #include "B.h"
 *  #include "DataVector.h"
 *  typedef DataVector<B> BVec;
 @endcode
 *
 * DVec.h:
 *@code
 *  #include "D.h"
 *  #include "DataVector.h"
 *  DATAVECTOR_BASE(D,B);
 *  typedef DataVector<D> DVec;
 @endcode
 *
 * D2Vec.h:
 *@code
 *  #include "D2.h"
 *  #include "DataVector.h"
 *  #include "DVec.h" // This is required
 *  DATAVECTOR_BASE(D2,D);
 *  typedef DataVector<D2> DVec;
 @endcode
 *
 * Using @c DATAVECTOR_BASE will also set up the corresponding @c SG::BaseInfo
 * definitions, both for the vectors themselves and for the contained objects.
 *
 * Forward declaration of inheritance
 * ==================================
 *
 * Sometimes it is necessary to reference a specialization @c DataVector<T>
 * where @c T isn't completely defined  (this can happen for example if a class
 * has an @c ElementLink to its own container type).  The @c DATAVECTOR_BASE
 * macro needs to come before the first reference to the specialization,
 * but it also needs to have a complete declaration for @c T.
 *
 * There is a way to get around this, but it requires splitting up
 * the @c DATAVECTOR_BASE declaration.  Before the first reference
 * to the @c DataVector<T> specialization, use
 *
 *@code
 *  DATAVECTOR_BASE_FWD(T, B);
 @endcode
 *
 * where here @c T does not need to be completely defined.  Then, where
 * you would normally have @c DATAVECTOR_BASE, write instead
 * @c DATAVECTOR_BASE_FIN.
 *
 *
 * Auxiliary data
 * ==============
 *
 * A @c DataVector may also have `auxiliary data' associated with it.
 * This is a set of name/value pairs that can be attached to each element
 * of the vector.  These data are stored in vectors that exist parallel
 * to the @c DataVector; the exact way in which this managed is hidden
 * behind an abstract interface.
 *
 * We'll start with an example, and then go into more detail.
 *
 *@code
 *
 * class MyClass
 *   : public SG::AuxElement  // Derive from AuxElement to allow for aux data
 * {
 * public:
 *   // A getter for an aux data item.
 *   float foo() const
 *   { const static ConstAccessor<float> acc ("foo");  return acc(*this); }
 *
 *   // A setter for an aux data item.
 *   void foo(float x)
 *   { const static Accessor<float> acc ("foo");  acc(*this) = x; }
 * };
 *
 * ...
 *
 * // Create the vector and associate a store.
 * DataVector<MyClass>* v = new DataVector<MyClass>;
 * SG::AuxStoreInternal* store = new SG::AuxStoreInternal;
 * v->setStore (store);
 *
 * // Add an item to the vector and set aux data on it.
 * v->push_back (new MyClass);
 * v[0]->foo() = 3;
 *
 * // Add a user-defined aux data item.
 * MyClass::Accessor<int> x ("x");
 * x(*v[0]) = 10;
 *
 * // Add a standalone store to an object.
 * MyClass* c = new MyClass;
 * c->makePrivateStore();
 * c->foo() = 4;
 * c->push_back (c); // Aux data will be transferred to the container.
 @endcode
 *
 * In order to be able store auxiliary data in a container three things
 * must hold.  First, the container's payload type must derive from
 * @c SG::AuxElement.  Second, the container must have index tracking
 * enabled.  Third, the container must have an associated auxiliary
 * data store.  More about these below.
 *
 * SG::AuxElement
 * --------------
 *
 * A type which may have associated auxiliary data must derive from the base
 * class @c SG::AuxElement.  This does several things.
 *
 * First, in order to be able to find auxiliary data from a pointer to
 * a container element, the element must be able to know both the container
 * that it's a member of and its index within that container.
 * @c SG::AuxElement makes this information available with the @c container()
 * and @c index() methods.  This information is maintained as long as the
 * container has index tracking enabled (see below).
 *
 * Second, it provides an interface for getting/setting auxiliary data.
 * The recommended way of doing this is through the nested @c Accessor
 * and @c ConstAccessor classes; these allows caching the translation
 * from attribute names to the internal representation.  The difference
 * between these two is that @c ConstAccessor allows only const access
 * to the element, while @c Accessor, which derives from it,
 * allows non-const access as well.  Create an @c Accessor
 * or @c ConstAccessor object with the data type (which can be anything
 * that can be used within a @c std::vector) and a name.  For @c Accessor,
 * the resulting object can then be used both as an rvalue and a lvalue;
 * for @c ConstAccessor, it can be used only as an rvalue.
 *
 *@code
 *  MyClass* c = ...;
 *  MyClass::Accessor<double> y ("y");
 *  y(*c) = 4;  // assign attribute.
 *  return y(*c);  // read attribute.
 @endcode
 *
 * A given name must always be used with the same type, otherwise an exception
 * will be thrown.  In the case that you want to use the same name for
 * different types in different classes, the name may be qualified with
 * an optional class name:
 *
 *@code
 *  MyClass::ConstAccessor<double> y ("y", "MyClass");
 @endcode
 *
 * If you have some auxiliary data items that are to be regarded as members
 * of a class, it is recommended to define a setter and getter as in the
 * example above.
 *
 * If you need to operate on a particular auxiliary data item for all elements
 * in a container, it will be more efficient to access the auxiliary data
 * directly by index, rather than through the elements.  This can be done like:
 *
 *@code
 *  DataVector<MyClass>* v = ...;
 *  MyClass::ConstAccesor<float> x ("x");
 *  size_t sz = v->size();
 *  for (size_t i = 0; i < sz; i++)
 *    do_something (x(*v, i));
 @endcode
 *
 * @c Accessor and @c ConstAccessor also have @c getDataArray methods
 * to directly return a pointer to the start of the auxiliary data array.
 *
 * It is also possible to use the @c auxdata method to access auxiliary
 * data directly.  However, it is not recommended to do this inside a loop
 * or anywhere where performance is important.
 *
 *@code
 *  c->auxdata<int> ("aux") = 5;  // set
 *  return c->auxdata<int> ("aux");  // retrieve
 @endcode
 *
 * Decorations
 * -----------
 *
 * In addition, sometimes one wants to add additional auxiliary data to
 * an existing, const container; this is called `decorating' it.
 * For example, you might want to retrieve a const container from StoreGate,
 * run some algorithm on it, and attach additional data to the object
 * that can be accessed by downstream algorithms or written to a file.
 * To support this, there is a @c Decorator object analogous to @c Accessor
 * and @c ConstAccessor.  The difference is that @c Decorator can operate
 * on a const object and return a non-const reference.
 *
 * To prevent modifying existing data, the contents of a container
 * may be locked with the @c lock() call; this happens automatically
 * when the container is made const with @c StoreGateSvc::setConst.
 * Once a container is locked, @c Decorator will only succeed
 * if either the variable does not yet exist (in which case it is
 * created and marked as a decoration) or it is marked as a decoration.
 *
 * Calling @c clearDecorations will erase all variables marked as decorations,
 * restoring the state back to where it was when @c lock was called.
 *
 * An @c auxdecor method is also available, analogous to @c auxdata.
 *
 *
 * Index tracking
 * --------------
 *
 * Recall that a @c DataVector either owns its elements or not.
 * By default, a @c DataVector that owns its elements will update
 * the container and index fields of the elements that it contains,
 * while a view @c DataVector will not.
 *
 * This rule does not always suffice, though.  In particular, a @c DataVector
 * that gets filled with elements from a @c DataPool does not own its
 * elements but should track indices.  For this reason, one can specify
 * an index tracking policy separate from the ownership policy.  Where
 * @c DataVector takes an ownership policy, it can also take as the
 * next argument an index tracking policy.  This policy defaults to
 * @c SG::DEFAULT_TRACK_INDICES, which means to set the indexing
 * policy based on the ownership policy.  But it can also be set to
 * @c SG::ALWAYS_TRACK_INDICES or @c SG::NEVER_TRACK_INDICES
 * to override this.
 *
 * The current state of index tracking for a @c DataVector is returned
 * by the @c trackIndices method.  Like the ownership policy, it may
 * be changed with the @c clear method.
 *
 * Auxiliary store
 * ---------------
 *
 * @c DataVector does not itself manage the storage for auxiliary data.
 * Instead, this is done by a separate _auxiliary store_ object.
 * This store object implements either the interface @c SG::IConstAuxStore
 * or @c SG::IAuxStore (which derives from it).  The first of these
 * provides operations needed to read data from the store, while the
 * second adds operations needed to add to and modify the data in the store.
 *
 * When you are create a @c DataVector object that is to have
 * auxiliary data, you will also need to create the store object.
 * A generic store object, which manages dynamic auxiliary data,
 * is available, @c SG::AuxStoreInternal.  There may also be specialized
 * store implementations for particular data classes.  Store implementations
 * also exist that are specialized for use in root; in that case, the data
 * are managed as part of a root tree, and the store object manages
 * access to it.
 *
 *@code
 *  DataVector<MyClass>* v = new DataVector<MyClass>;
 *  CHECK( evtStore->record (v, "mykey") );
 *  SG::AuxStoreInternal* store = new SG::AuxStoreInternal;
 *  CHECK( evtStore->record (store, "mykeyAux.") );
 *  v->setStore (store);
 @endcode
 *
 * You can only set a store if the container has index tracking enabled;
 * otherwise, an exception will be thrown.
 *
 * You should not have to explicitly deal with the auxiliary store
 * for objects that are read from a file.  The I/O system is responsible
 * for getting the store association correct in that case.
 *
 * Standalone objects
 * ------------------
 *
 * Normally, an element can only have associated auxiliary data if it is
 * a member of a container; otherwise, there is no store in which to store
 * the auxiliary data.  However, it is possible to request that a store
 * be created for an individual element not part of a container by calling
 * the method @c makePrivateStore.  You can then add auxiliary data to the
 * object:
 *
 *@code
 *  MyClass* c = new MyClass;
 *  c->makePrivateStore();
 *  MyClass::Accessor<int> x ("x");
 *  x(*c) = 5;
 @endcode
 *
 * If the element is then added to a container, the auxiliary data will be
 * copied to the container's store, and the private store the was associated
 * with the element will be released.  The fact that there was a private store
 * is remembered, however; if the element is later removed from the container
 * (in a way that doesn't delete the element), the private store will be remade
 * and the auxiliary data copied back from the container to the private store.
 *
 * @c makePrivateStore can also take an argument.  If provided, auxiliary data
 * will be copied from it.  This can be used to properly implement copy
 * constructors:
 *
 *@code
 *  class MyStandaloneClass
 *    : public SG::AuxElement
 *  {
 *  public:
 *    MyStandaloneClass() { this->makePrivateStore(); }
 *
 *    MyStandaloneClass (const MyStandaloneClass& other)
 *      SG::AuxElement (other)
 *    {
 *      this->makePrivateStore (other);
 *    }
 @endcode
 *
 * As a shorthand, one can use the wrapper class @c SG::AuxElementComplete.
 * This will add constructors that automatically make a private store:
 *
 *@code
 *  typedef SG::AuxElementComplete<MyClass> MyStandaloneClass;
 @endcode
 *
 * A standalone object also has @c setStore methods that can be used
 * to associate an external store with a since object, in the same manner
 * as for containers.
 */


#ifndef ATHCONTAINERS_DATAVECTOR_H
#define ATHCONTAINERS_DATAVECTOR_H


// For feature tests.
#define HAVE_CONSTDATAVECTOR


#include "AthContainers/exceptions.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/IndexTrackingPolicy.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/tools/DVLNoBase.h"
#include "AthContainers/tools/DVLInfo.h"
#include "AthContainers/tools/DVLCast.h"
#include "AthContainers/tools/DVLIterator.h"
#include "AthContainers/tools/DVL_iter_swap.h"
#include "AthContainers/tools/DVL_algorithms.h"
#include "AthContainers/tools/ElementProxy.h"
#include "AthContainers/tools/IsMostDerivedFlag.h"
#include "AthLinks/tools/selection_ns.h"
#include <boost/static_assert.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <type_traits>
#include <vector>
#include <typeinfo>
#include <functional>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <initializer_list>



#ifdef XAOD_STANDALONE
# ifndef SG_BASE
#  define SG_BASE(A, B) class ATHCONTAINERS_DUMMY
# endif // not SG_BASE
# ifndef SG_BASES1
#  define SG_BASES1(A, B) class ATHCONTAINERS_DUMMY
# endif // not SG_BASES1
# ifndef SG_BASES2
#  define SG_BASES2(A, B, C) class ATHCONTAINERS_DUMMY
# endif // not SG_BASES2
# ifndef SG_BASES3
#  define SG_BASES3(A, B, C, D) class ATHCONTAINERS_DUMMY
# endif // not SG_BASES3
# ifndef SG_VIRTUAL
#  define SG_VIRTUAL(X) X
# endif // not SG_VIRTUAL
#else
# include "SGTools/BaseInfo.h"
#endif


// Forward declarations.
ENTER_ROOT_SELECTION_NS
template <class T, class BASE> class DataVector;
EXIT_ROOT_SELECTION_NS


namespace DataVector_detail {


// These are the intermediate classes from which @c DataVector derives
// in the case of multiple or virtual derivation.  The actual
// definitions are in the @c icc file.  They have the following
// properties:
//   - They derive from those of @c B1, @c B2, and @c B3 that are not
//     @c NoBase.
//   - They perform (static) checking to ensure that there's a unique
//     base class in the hierarchy.
//   - They make the following types available from the base classes:
//     - @c PtrVector
//     - @c BaseContainer
//     - @c size_type
//     - @c difference_type
//     - @c allocator_type
//   - @c has_virtual is defined as @c true.
//   - An override of @c dvlinfo_v is defined, to prevent ambiguities.
template <class B1,
          class B2=DataModel_detail::NoBase,
          class B3=DataModel_detail::NoBase> struct VirtBases;


} // namespace DataVector_detail



/**
 * @brief Derivation information for @c DataVector.
 *
 * Specializations of this class represent the derivation information
 * that @c DataVector needs.  They have the property that
 * <code>DataVectorBase<T>::Base</code> is either @c NoBase or
 * it is the class from which @c DataVector\<T> should derive.
 */
template <class T>
struct DataVectorBase
{
  typedef DataModel_detail::NoBase Base;
};


/**
 * @brief Declare base class info to @c DataVector.
 *        Single, non-virtual derivation.
 *
 * <code>DATAVECTOR_BASE(D, B)</code> says that @c D derives
 * non-virtually from @c B.
 *
 * This macro creates an appropriate specialization of @c DataVectorBase.
 */
#define DATAVECTOR_BASE(T, BASE)          \
DATAVECTOR_BASE_FWD(T, BASE);             \
template struct DataVector_detail::DVLEltBaseInit<T>


/**
 * @brief Version of @c DATAVECTOR_BASE that can be used
 *        in forward declarations.
 */
#define DATAVECTOR_BASE_FWD(T, BASE)      \
template <> struct DataVectorBase<T>      \
{ typedef DataVector<BASE> Base; };       \
SG_BASE(DataVector<T>, DataVector<BASE>)


/**
 * @brief Declare base class info to @c DataVector.
 *        Single, virtual derivation.
 *
 * <code>DATAVECTOR_VIRTBASES1(D, B1)</code> says that @c D derives
 * virtually from @c B1.
 *
 * This macro creates an appropriate specialization of @c DataVectorBase.
 */
#define DATAVECTOR_VIRTBASES1(T, B1)                  \
DATAVECTOR_VIRTBASES1_FWD(T, B1);                     \
template struct DataVector_detail::DVLEltBaseInit<T>



/**
 * @brief Version of @c DATAVECTOR_VIRTBASES1 that can be used
 *        in forward declarations.
 */
#define DATAVECTOR_VIRTBASES1_FWD(T, B1)              \
template <> struct DataVectorBase<T>                  \
{ typedef DataVector_detail::VirtBases<B1> Base; };   \
SG_BASES1(DataVector<T>, SG_VIRTUAL(DataVector<B1>))



/**
 * @brief Declare base class info to @c DataVector.
 *        Multiple derivation.
 *
 * <code>DATAVECTOR_VIRTBASES2(D, B1, B2)</code> says that @c D derives
 * from both @c B1 and @c B2.
 *
 * This macro creates an appropriate specialization of @c DataVectorBase.
 */
#define DATAVECTOR_VIRTBASES2(T, B1, B2)                \
DATAVECTOR_VIRTBASES2_FWD(T, B1, B2);                   \
template struct DataVector_detail::DVLEltBaseInit<T>


/**
 * @brief Version of @c DATAVECTOR_VIRTBASES2 that can be used
 *        in forward declarations.
 */
#define DATAVECTOR_VIRTBASES2_FWD(T, B1, B2)            \
template <> struct DataVectorBase<T>                    \
{ typedef DataVector_detail::VirtBases<B1, B2> Base; }; \
SG_BASES2(DataVector<T>, SG_VIRTUAL(DataVector<B1>),    \
                         SG_VIRTUAL(DataVector<B2>))


/**
 * @brief Declare base class info to @c DataVector.
 *        Multiple derivation.
 *
 * <code>DATAVECTOR_VIRTBASES3(D, B1, B2, B3)</code> says that @c D derives
 * from all of @c B1, @c B2, and @c B3.
 *
 * This macro creates an appropriate specialization of @c DataVectorBase.
 */
#define DATAVECTOR_VIRTBASES3(T, B1, B2, B3)            \
DATAVECTOR_VIRTBASES3_FWD(T, B1, B2, B3);               \
template struct DataVector_detail::DVLEltBaseInit<T>


/**
 * @brief Version of @c DATAVECTOR_VIRTBASES3 that can be used
 *        in forward declarations.
 */
#define DATAVECTOR_VIRTBASES3_FWD(T, B1, B2, B3)            \
template <> struct DataVectorBase<T>                        \
{ typedef DataVector_detail::VirtBases<B1, B2, B3> Base; }; \
SG_BASES3(DataVector<T>, SG_VIRTUAL(DataVector<B1>),        \
                         SG_VIRTUAL(DataVector<B2>),        \
                         SG_VIRTUAL(DataVector<B3>))



/**
 * @brief Used to finish up a forward declaration.
 *
 * The @c B parameter is not actually used, but is retained for
 * consistency and documentation.
 */
#define DATAVECTOR_BASE_FIN(T, B) \
template struct DataVector_detail::DVLEltBaseInit<T>


template <class DV> class ConstDataVector;
template <class DV> void test2_assignelement1();
template <class DV> void test2_assignelement2();


/**
 * @brief Derived @c DataVector\<T>.
 *
 * This is used for the case where @c T derives
 * from other classes.  The vector of pointers is actually held
 * in the (unique) base class of the hierarchy.
 *
 * See the file comments for full details.
 */
template <class T, class BASE = typename DataVectorBase<T>::Base>
class DataVector : public BASE
{
public:
  typedef BASE DataVector_BASE;
  typedef BASE DVL_BASE;

  /// This is true for any @c DataVector class if we need to use virtual
  /// derivation to get to the base @c DataVector class.
  static const bool has_virtual = BASE::has_virtual;

  /// This is the type of the underlying @c std::vector
  /// (what @c stdcont returns).
  typedef typename BASE::PtrVector PtrVector;
  typedef typename BASE::PtrVector BaseContainer;


public:
  // Standard types required for the container interface.
  typedef T*& reference;
  typedef T* const & const_reference;
  typedef typename BASE::size_type size_type;
  typedef typename BASE::difference_type difference_type;
  typedef T* value_type;
  typedef typename BASE::allocator_type allocator_type;
  typedef T** pointer;
  typedef T* const * const_pointer;

  typedef const T* const_value_type;

  /// The @c T value used as the template parameter.
  /// Note that this is different from @c value_type (that's @c T*).
  typedef T base_value_type;

  /// This type is used to proxy lvalue accesses to @c DataVector
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<DataVector> ElementProxy;


  /// Standard @c const_iterator.
  typedef typename DataModel_detail::const_iterator<DataVector>
    const_iterator;

  /// Standard @c iterator.  Note that lvalue references here will yield
  /// an @c ElementProxy, not a @c reference.
  typedef typename DataModel_detail::iterator<DataVector> iterator;


  /// Standard @c const_reverse_iterator.
  typedef typename std::reverse_iterator<const_iterator>
    const_reverse_iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;

  typedef DataVector base_data_vector;


  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Default constructor.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   *
   * By default, a @c DataVector will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataVector(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
                      SG::IndexTrackingPolicy trackIndices =
                        SG::DEFAULT_TRACK_INDICES);


  /**
   * @brief Sized constructor.
   * @param n The size of the container.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   *
   * Note that unlike the standard vector constructor, you can't specify
   * an initial value here.  The container will be initialized with 0's.
   *
   * By default, a @c DataVector will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataVector(size_type n, /* value_type pElem = 0,*/
		      SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
                      SG::IndexTrackingPolicy trackIndices =
                        SG::DEFAULT_TRACK_INDICES);


  /**
   * @brief Copy constructor.
   * @param rhs The container from which to copy.
   *
   * This is a `shallow' copy; the new container will not own its elements.
   */
  // The copy constructor for derived classes is deliberately omitted,
  // as otherwise we get warnings about not calling the copy constructors
  // for base classes, which are problematic when we have virtual
  // inheritance.  Most of what needs doing is done in the base class anyway,
  // except for setting @c m_isMostDerived.  We arrange for these flags
  // to all get set to false; they'll get set correctly when
  // @c testInsert is called.
#if __cplusplus > 201100
  // Need this to get the default copy ctor defined when a move
  // ctor is also present.
  DataVector (const DataVector&) = default;
#endif


#if __cplusplus > 201100
  /**
   * @brief Move constructor.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   */
  DataVector (DataVector&& rhs);
#endif


  /**
   * @brief Constructor from iterators.
   * @param first The start of the range to put in the new container.
   * @param last The end of the range to put in the new container.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   * @param store An associated auxiliary data store.
   *
   * By default, a view container is made, which does not own its elements.
   * To have the container take ownership of the pointers passed
   * to this constructor, pass @c SG::OWN_ELEMENTS for @a ownPolicy.
   */
  template <class InputIterator>
  DataVector(InputIterator first,
             InputIterator last,
	     SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS,
             SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES,
             SG::IAuxStore* store = 0);


#if __cplusplus > 201100
  /**
   * @brief Constructor from an initializer list.
   * @param l An initializer list.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   * @param store An associated auxiliary data store.
   *
   * A @c DataVector constructed this way will *not* own its elements
   * by default.  To change this, pass @c SG::OWN_ELEMENTS for @a ownPolicy.
   */
  DataVector(std::initializer_list<value_type> l,
	     SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS,
             SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES,
             SG::IAuxStore* store = 0);
#endif


  /**
   * @brief Assignment operator.
   * @param rhs The DataVector from which to assign.
   * @return This object.
   *
   * This is a `shallow' copy; after the completion of this, the @c DataVector
   * will not own its elements.  Any elements it owned prior to this call
   * will be released.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  DataVector& operator= (const DataVector& rhs);


#if __cplusplus > 201100
  /**
   * @brief Move assignment.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   */
  DataVector& operator= (DataVector&& rhs);


  /**
   * @brief Assignment operator, from an initializer list.
   * @param l An initializer list.
   * @return This object.
   *
   * This is equivalent to @c assign.
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  DataVector& operator= (std::initializer_list<value_type> l);
#endif


  /**
   * @brief Assign from iterators.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last);


#if __cplusplus > 201100
  /**
   * @brief Assign from an initializer list.
   * @param l An initializer list.
   *
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  void assign(std::initializer_list<value_type> l);
#endif


  // Destructor is inherited.


  //@}
  //========================================================================
  /** @name Size and capacity. */
  //@{


  /**
   * @fn size_type size() const
   * @brief Returns the number of elements in the collection.
   *
   * Could in principle be inherited from the base class,
   * but redeclared in the derived class to avoid root6 bugs.
   */
  size_type size() const;


  /**
   * @fn size_type max_size() const
   * @brief Returns the @c size() of the largest possible collection.
   */
  // This is inherited from the base class.


  /**
   * @fn void resize(size_type sz)
   * @brief Resizes the collection to the specified number of elements.
   * @param sz The new size of the collection.
   *
   * Note that this function differs from the standard in that it does
   * not allow specifying the value of any inserted elements.
   * They will always be 0.
   *
   * If the container is shrunk, elements will be deleted as with @c erase().
   */
  void resize(size_type sz);
  

  /**
   * @fn size_type capacity() const
   * @brief Returns the total number of elements that the collection can hold
   *        before needing to allocate more memory.
   */
  // This is inherited from the base class.


  /**
   * @fn bool empty() const
   * @brief Returns @c true if the collection is empty.
   */
  // This is inherited from the base class.


  /**
   * @fn void reserve (size_type n)
   * @brief Attempt to preallocate enough memory for a specified number
   *        of elements.
   * @param n Number of elements required.
   */
  void reserve (size_type n);


  /**
   * @brief Change the vector capacity to match the current size.
   *
   * Note: this does not affect auxiliary data.
   */
  // This is inherited from the base class.


  //@}
  //========================================================================
  /** @name Element access. */
  //@{


  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* operator[] (size_type n) const;


  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   * 
   * This is a synonym for operator[] const, to be used when calling from root
   * (where we can't readily call just the const version of a method).
   */
  const T* get (size_type n) const;


  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy operator[] (size_type n);


  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* at (size_type n) const;


  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy at (size_type n);


  /**
   * @brief Access the first element in the collection as an rvalue.
   * @return The first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* front() const;


  /**
   * @brief Access the last element in the collection as an rvalue.
   * @return The last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* back() const;


  /**
   * @brief Access the first element in the collection as an lvalue.
   * @return Proxy to the first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy front ();


  /**
   * @brief Access the last element in the collection as an lvalue.
   * @return Proxy to the last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy back ();


  //@}
  //========================================================================
  /** @name Iterator creation. */
  //@{


  /**
   * @brief Return a @c const_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator begin() const;


  /**
   * @brief Return a @c const_iterator pointing past the end
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator end() const;


  /**
   * @brief Return an @c iterator pointing at the beginning
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator begin();


  /**
   * @brief Return an @c iterator pointing past the end
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator end();


  /**
   * @brief Return a @c const_reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rbegin() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rend() const;


  /**
   * @brief Return a @c reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rbegin();


  /**
   * @brief Return a @c reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rend();


  /**
   * @brief Return a @c const_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator cbegin() const;


  /**
   * @brief Return a @c const_iterator pointing past the end
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator cend() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator crbegin() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator crend() const;


  //@}
  //========================================================================
  /** @name Insertion operations. */
  //@{


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void push_back(value_type pElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container must be an owning container.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void push_back(std::unique_ptr<base_value_type> pElem);
#endif
#endif


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c push_back.
   * It's included just for interface compatibility with `std::vector`.
   */
  void emplace_back(value_type pElem);


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, value_type pElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container must be an owning container.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, std::unique_ptr<base_value_type> pElem);
#endif
#endif


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c insert.
   * It's included just for interface compatibility with `std::vector`.
   */
  iterator emplace(iterator position, value_type pElem);


  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);


#if __cplusplus > 201100
  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param l An initializer list.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void insert(iterator position, std::initializer_list<value_type> l);
#endif


  /**
   * @brief Insert the contents of another @c DataVector,
   *        with auxiliary data copied via move semantics.
   * @param position Iterator before which the new elements will be added.
   * @param other The vector to add.
   *
   * The ownership mode of this vector must be the same as @c other;
   * otherwise, an exception will be thrown.
   *
   * If both vectors are view vectors, then this is the same
   * as <code> insert (position, other.begin(), other.end()) </code>.
   *
   * Otherwise, the elements from @c other will be inserted into this vector.
   * This vector will take ownership of the elements, and the ownership
   * mode of @c other will be changed to @c VIEW_ELEMENTS.
   * Auxiliary data for these elements will be transferred,
   * using move semantics if possible.  (Thus, the auxiliary store
   * for @c other may be modified and must not be locked.)
   * Finally, the auxiliary store pointer for @c other will be cleared
   * (but the store itself will not be deleted since it's not owned
   * by the vector).
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void insertMove (iterator position, DataVector& other);


  //@}
  //========================================================================
  /** @name Erasure operations. */
  //@{


  /**
   * @brief Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or @c end()).
   *
   * If the container owns its elements, then the pointed-to element
   * will be deleted.
   */
  iterator erase(iterator position);


  /**
   * @brief Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or @c end()).
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  iterator erase(iterator first, iterator last);


  /**
   * @fn void pop_back()
   * @brief Remove the last element from the collection.
   *
   * If the container owns its elements, then the removed element
   * will be deleted.
   */
  void pop_back();


  /**
   * @fn void clear()
   * @brief Erase all the elements in the collection.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear();


  //@}
  //========================================================================
  /** @name Swap and sort. */
  //@{


  /**
   * @brief Swap this collection with another.
   * @param rhs The collection with which to swap.
   *
   * Ownership is swapped along with the collection content.
   *
   * Note: this method may only be called using the most-derived
   * @c DataVector in the hierarchy.  The @a rhs must also be
   * referenced using the most-derived @c DataVector.
   */
  void swap(DataVector& rhs);


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap/
   */
  static void iter_swap (iterator a, iterator b);


  /**
   * @brief Sort the container.
   *
   * This just sorts by pointer value, so it's probably not very useful.
   */
  void sort();


  /**
   * @brief Sort the container with a user-specified comparison operator.
   * @param comp Functional to compare two values.
   */
  template <class COMPARE>
  void sort(COMPARE comp);



  //@}
  //========================================================================
  /** @name Non-standard operations. */
  //@{
  

  /**
   * @brief Swap one element out of the container.
   * @param index Index of the element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a index of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(size_type index, value_type newElem, reference oldElem);


  /**
   * @brief Swap one element out of the container.
   * @param pos The element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a pos of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void swapElement(iterator pos, value_type newElem, reference oldElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Swap one element out of the container.
   * @param index Index of the element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a index of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  
   *
   * The collection must own its elements to use its interface.
   * The collection will take ownership of @c newElem and will return
   * ownership of @c oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(size_type index,
                   std::unique_ptr<base_value_type> newElem,
                   std::unique_ptr<base_value_type>& oldElem);


  /**
   * @brief Swap one element out of the container.
   * @param pos The element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a pos of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.
   *
   * The collection must own its elements to use its interface.
   * The collection will take ownership of @c newElem and will return
   * ownership of @c oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void swapElement(iterator pos,
                   std::unique_ptr<base_value_type> newElem,
                   std::unique_ptr<base_value_type>& oldElem);
#endif
#endif


public:
  /**
   * @fn const PtrVector& stdcont() const
   * @brief Return the underlying @c std::vector of the container.
   * @return Reference to the @c std::vector actually holding the collection.
   *
   * Note that @c DataVector<T>::stdcont does not necessarily return
   * a @c std::vector<T*> if @c DataVector inheritance is being used.
   */
  // This is inherited from the base class.


  /**
   * @fn SG::OwnershipPolicy ownPolicy() const
   * @brief Return the ownership policy setting for this container.
   */
  // This is inherited from the base class.


  /**
   * @fn void clear (SG::OwnershipPolicy ownPolicy)
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear (SG::OwnershipPolicy ownPolicy);


  /**
   * @fn void clear
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   * @param trackIndices The index tracking policy.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear (SG::OwnershipPolicy ownPolicy,
              SG::IndexTrackingPolicy trackIndices);


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  static const DataModel_detail::DVLInfoBase& dvlinfo();


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  virtual const DataModel_detail::DVLInfoBase& dvlinfo_v() const;


  /**
   * @brief Return the offset of a base @c DataVector class.
   * @param ti @c std::type_info of the desired class.
   *
   * If @c ti represents a @c DataVector base class of this one,
   * then return the offset of that base class.  Otherwise, return -1.
   *
   * This function is here due to limitations of root 6, which can't
   * calculate these offsets correctly from the dictionary if
   * virtual derivation is used.
   */
  static
  int baseOffset (const std::type_info& ti);


  /**
   * @brief Convert to @c AuxVectorBase.
   *
   * Needed to get @c AuxVectorBase from a @c ConstDataVector.
   * Present in @c DataVector as well for consistency.
   * We only really need it in the base class; however, root6 fails
   * constructing a @c TMethodCall for this if there is virtual
   * derivation.  A workaround is to redeclare this in the derived
   * classes too.
   */
  const SG::AuxVectorBase& auxbase() const;


  //@}
  //========================================================================


  // Doxygen trick.  We want DataVector<T> to show the complete interface,
  // including inherited methods.  Normally, doxygen would do this.
  // But it's not smart enough to figure out our inheritance relationships.
  // We can add a function with `fn', but doxygen won't actually
  // generate the documentation unless it also sees the declaration.
  // So here are declarations which should be visible to doxygen
  // but not to C++.
#ifndef __cplusplus
  size_type max_size() const;
  void resize(size_type sz);
  size_type capacity() const;
  bool empty() const;
  void shrink_to_fit();
  void pop_back();
  void clear();
  const PtrVector& stdcont() const;
  SG::OwnershipPolicy ownPolicy() const;
  void clear (SG::OwnershipPolicy ownPolicy,
              SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
#endif // not __cplusplus


  /** @name Internal operations. */
  //@{


  /**
   * @brief Helper for @c baseOffset.
   * @param p Pointer to the start of the top-level object.
   * @param dv Reference to the DataVector object.
   * @param ti @c std::type_info of the desired class.
   *
   * If @c ti represents a @c DataVector base class of this one,
   * then return the offset of that base class.  Otherwise, return -1.
   *
   */
  static
  int baseOffset1 (const char* p, const DataVector& dv,
                   const std::type_info& ti);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * Call this after some operation that has permuted the elements in the
   * container (such as sort).  The index information in the elements
   * will be used to permute all auxiliary data in the same way.
   * Finally, all the indices will be reset in the correct order.
   */
  void resortAux (iterator beg, iterator end);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * In order to maintain type-safety, we can only allow insertions
   * using the most-derived instance of @c DataVector.  This checks
   * this by testing the @c m_isMostDerived, which is set by the constructors
   * to true only for the most-derived instance.
   * If the test fails, we call to potentially out-of-line code to continue.
   */
  void testInsert (const char* op);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * This continues the test of @c testInsert.  There is one case
   * where @c m_isMostDerived may not be set correctly.  If this container
   * was made via copy construction, then all the @c m_isMostDerived flags
   * will be false.  So we call @c setMostDerived to set the flags correctly
   * and test again.  If the test fails again, then we raise an exception.
   */
  void testInsertOol (const char* op);


private:
  friend class DataModel_detail::ElementProxy<DataVector>;
  template <class DV>
  friend class ConstDataVector;
  friend void test2_assignelement1<DataVector>();
  friend void test2_assignelement2<DataVector>();


  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos, value_type newElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The container must own its elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos,
                      std::unique_ptr<base_value_type> newElem);
#endif
#endif


  /**
   * @brief Handle element assignment from a base pointer.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignBaseElement (typename BaseContainer::iterator pos,
                          typename BaseContainer::value_type newElem);


  /**
   * @brief Shift the auxiliary elements of the container.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * The elements in the container should have already been shifted;
   * this operation will then adjust the element indices and also shift
   * the elements in the vectors for all aux data items.
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
  void shift (size_t pos, ptrdiff_t offs);


public:
  // Make this public so we can call it from DVCollectionProxy.
  /**
   * @brief Helper to shorten calls to @c DataModel_detail::DVLCast.
   * @param p The value to convert.
   * @return The value as a @c const @c T*.
   *
   * The conversion will be done with @c static_cast if possible,
   * with @c dynamic_cast otherwise.
   */
  static
  const T* do_cast (const typename PtrVector::value_type p);


private:
  /**
   * @brief Find the most-derived @c DataVector class in the hierarchy.
   * @return The @c type_info for the class for which this method gets run.
   *
   * This is used to generate a nice error message when the most-derived
   * check for insertions fails.
   * Every @c DataVector defines this virtual method, so when it's
   * called, the one corresponding to the most-derived @c DataVector
   * gets run.
   */
  virtual const std::type_info& dv_typeid() const;


protected:
  /**
   * @brief Clear @c m_isMostDerived for this instance and for all bases.
   *
   * Called from the constructor after setting @c m_isMostDerived.
   */
  void clearMostDerived();


  /**
   * @brief Set @c m_isMostDerived for this instance and clear it for all bases.
   *
   * Called from @c testInsert if the test fails.  The flag may not have
   * been set if this container was made via copy construction, so set
   * it appropriately now so we can test again.
   */
  virtual void setMostDerived();


private:
  //@}


  /// This flag is true if this DV instance is the most-derived one.
  /// We set this to true in the top-level constructor; the constructor
  /// then calls @c clearMostDerived on the base classes.
  SG::IsMostDerivedFlag m_isMostDerived;


  /// The DV/DL info struct for this class.
  static DataModel_detail::DVLInfo<DataVector<T> > s_info;


  typedef typename
    ROOT_SELECTION_NS::DataVector<T, DataVector_BASE>::self self;
};


template <class T, class BASE>
const bool DataVector<T, BASE>::has_virtual;

/**
 * @brief Base specialization for @c DataVector\<T>.
 *
 * This is used for the case where @c T does not derive
 * from other classes.  This is the class which actually holds
 * the vector of pointers.
 *
 * See the file comments for full details.
 */
template <class T>
class DataVector<T, DataModel_detail::NoBase>
  : public SG::AuxVectorBase
{
public:
  typedef DataModel_detail::NoBase DataVector_BASE;
  typedef DataModel_detail::NoBase DVL_BASE;

  /// Mark as a sequence, for DataLink / ElementLink.
  typedef std::true_type isSequence;

  /// This is the type of the underlying @c std::vector
  /// (what @c stdcont returns).
  typedef std::vector<T*> PtrVector;
  typedef std::vector<T*> BaseContainer;

  /// This is true for any @c DataVector class if we need to use virtual
  /// derivation to get to the base @c DataVector class.
  /// Since this @e is the base @c DataVector class, set this
  /// unconditionally to @c false.
  static const bool has_virtual = false;

  // Standard types required for the container interface.
  typedef T*& reference;
  typedef T* const & const_reference;
  typedef typename PtrVector::size_type size_type;
  typedef typename PtrVector::difference_type difference_type;
  typedef T* value_type;
  typedef typename PtrVector::allocator_type allocator_type;
  typedef T** pointer;
  typedef T* const * const_pointer;

  typedef const T* const_value_type;

  /// The @c T value used as the template parameter.
  /// Note that this is different from @c value_type (that's @c T*).
  typedef T base_value_type;

  /// This type is used to proxy lvalue accesses to @c DataVector
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<DataVector> ElementProxy;


  /// Standard @c const_iterator.
  // Here, we can just use the @c std::vector iterator.
  typedef typename PtrVector::const_iterator const_iterator;

  /// Standard @c iterator.  Note that lvalue references here will yield
  /// an @c ElementProxy, not a @c reference.
  typedef typename DataModel_detail::iterator<DataVector> iterator;

  /// Standard @c const_reverse_iterator.
  // Note: we can't just use the underlying PtrVector::const_reverse_iterator
  // here; otherwise, const_reverse_iterator won't be convertable to
  // reverse_iterator.
  typedef typename std::reverse_iterator<const_iterator>
    const_reverse_iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;

  typedef DataVector base_data_vector;

  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Default constructor.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   *
   * By default, a @c DataVector will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataVector(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
                      SG::IndexTrackingPolicy trackIndices =
                        SG::DEFAULT_TRACK_INDICES);


  /**
   * @brief Sized constructor.
   * @param n The size of the container.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   *
   * Note that unlike the standard vector constructor, you can't specify
   * an initial value here.  The container will be initialized with 0's.
   *
   * By default, a @c DataVector will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataVector
    (size_type n, /* value_type pElem = 0,*/
     SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,
     SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);


  /**
   * @brief Copy constructor.
   * @param rhs The container from which to copy.
   *
   * This is a `shallow' copy; the new container will not own its elements.
   */
  DataVector(const DataVector& rhs);


#if __cplusplus > 201100
  /**
   * @brief Move constructor.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   */
  DataVector(DataVector&& rhs);
#endif


  /**
   * @brief Constructor from iterators.
   * @param first The start of the range to put in the new container.
   * @param last The end of the range to put in the new container.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   * @param store An associated auxiliary data store.
   *
   * By default, a view container is made, which does not own its elements.
   * To have the container take ownership of the pointers passed
   * to this constructor, pass @c SG::OWN_ELEMENTS for @a ownPolicy.
   */
  template <class InputIterator>
  DataVector(InputIterator first, InputIterator last,
	     SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS,
             SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES,
             SG::IAuxStore* store = 0);


#if __cplusplus > 201100
  /**
   * @brief Constructor from iterators.
   * @param l An initializer list.
   * @param ownPolicy The ownership mode for the container.
   * @param trackIndices The index tracking policy.
   * @param store An associated auxiliary data store.
   *
   * A @c DataVector constructed this way will *not* own its elements
   * by default.  To change this, pass @c SG::OWN_ELEMENTS for @a ownPolicy.
   */
  DataVector(std::initializer_list<value_type> l,
	     SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS,
             SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES,
             SG::IAuxStore* store = 0);
#endif


  /**
   * @brief Assignment operator.
   * @param rhs The DataVector from which to assign.
   * @return This object.
   *
   * This is a `shallow' copy; after the completion of this, the @c DataVector
   * will not own its elements.  Any elements it owned prior to this call
   * will be released.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  DataVector& operator= (const DataVector& rhs);


#if __cplusplus > 201100
  /**
   * @brief Move assignment.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   */
  DataVector& operator= (DataVector&& rhs);


  /**
   * @brief Assignment operator, from an initializer list.
   * @param l An initializer list.
   * @return This object.
   *
   * This is equivalent to @c assign.
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  DataVector& operator= (std::initializer_list<value_type> l);
#endif


  /**
   * @brief Assign from iterators.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last);


#if __cplusplus > 201100
  /**
   * @brief Assign from an initializer list.
   * @param l An initializer list.
   *
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  void assign(std::initializer_list<value_type> l);
#endif


  /**
   * @brief Destructor.
   *
   * If this container owns its elements, the contained elements will
   * be deleted as well.  Before doing this, the destructor will scan
   * for duplicate pointers (takes @f$n \log n@f$ time); duplicates are only
   * destroyed once.  Duplicates should, however, be considered an error;
   * don't rely on this behavior.
   */
  virtual ~DataVector();


  //@}
  //========================================================================
  /** @name Size and capacity. */
  //@{


  /**
   * @brief Returns the number of elements in the collection.
   */
  size_type size() const;


  /**
   * @brief Returns the number of elements in the collection.
   *
   *        This version is virtual, to be callable from the AuxData
   *        base class.
   */
  virtual size_type size_v() const;


  /**
   * @brief Returns the @c size() of the largest possible collection.
   */
  size_type max_size() const;


  /**
   * @brief Resizes the collection to the specified number of elements.
   * @param sz The new size of the collection.
   *
   * Note that this function differs from the standard in that it does
   * not allow specifying the value of any inserted elements.
   * They will always be 0.
   *
   * If the container is shrunk, elements will be deleted as with @c erase().
   */
  void resize(size_type sz);


  /**
   * @brief Returns the total number of elements that the collection can hold
   *        before needing to allocate more memory.
   */
  size_type capacity() const;


  /**
   * @brief Returns the total number of elements that the collection can hold
   *        before needing to allocate more memory.
   *
   *        This version is virtual, to be callable from the AuxData
   *        base class.
   */
  virtual size_type capacity_v() const;


  /**
   * @brief Returns @c true if the collection is empty.
   */
  bool empty() const;


  /**
   * @brief Attempt to preallocate enough memory for a specified number
   *        of elements.
   * @param n Number of elements required.
   */
  void reserve (size_type n);


#if __cplusplus > 201100
  /**
   * @brief Change the vector capacity to match the current size.
   *
   * Note: this does not affect auxiliary data.
   */
  void shrink_to_fit();
#endif


  //@}
  //========================================================================
  /** @name Element access. */
  //@{


  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* operator[] (size_type n) const;


  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   * 
   * This is a synonym for operator[] const, to be used when calling from root
   * (where we can't readily call just the const version of a method).
   */
  const T* get (size_type n) const;


  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy operator[] (size_type n);


  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* at (size_type n) const;


  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy at (size_type n);


  /**
   * @brief Access the first element in the collection as an rvalue.
   * @return The first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* front() const;


  /**
   * @brief Access the last element in the collection as an rvalue.
   * @return The last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* back() const;


  /**
   * @brief Access the first element in the collection as an lvalue.
   * @return Proxy to the first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy front ();


  /**
   * @brief Access the last element in the collection as an lvalue.
   * @return Proxy to the last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy back ();


  //@}
  //========================================================================
  /** @name Iterator creation. */
  //@{


  /**
   * @brief Return a @c const_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator begin() const;


  /**
   * @brief Return a @c const_iterator pointing past the end
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator end() const;


  /**
   * @brief Return an @c iterator pointing at the beginning
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator begin();


  /**
   * @brief Return an @c iterator pointing past the end
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator end();


  /**
   * @brief Return a @c const_reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rbegin() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rend() const;


  /**
   * @brief Return a @c reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rbegin();


  /**
   * @brief Return a @c reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rend();

  /**
   * @brief Return a @c const_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator cbegin() const;


  /**
   * @brief Return a @c const_iterator pointing past the end
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator cend() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator crbegin() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator crend() const;


  //@}
  //========================================================================
  /** @name Insertion operations. */
  //@{


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void push_back(value_type pElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container must be an owning container.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void push_back(std::unique_ptr<base_value_type> pElem);
#endif
#endif


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c push_back.
   * It's included just for interface compatibility with `std::vector`.
   */
  void emplace_back(value_type pElem);


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, value_type pElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container must be an owning container.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, std::unique_ptr<base_value_type> pElem);
#endif
#endif


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c insert.
   * It's included just for interface compatibility with `std::vector`.
   */
  iterator emplace(iterator position, value_type pElem);


  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);


#if __cplusplus > 201100
  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param l An initializer list.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void insert(iterator position, std::initializer_list<value_type> l);
#endif


  /**
   * @brief Insert the contents of another @c DataVector,
   *        with auxiliary data copied via move semantics.
   * @param position Iterator before which the new elements will be added.
   * @param other The vector to add.
   *
   * The ownership mode of this vector must be the same as @c other;
   * otherwise, an exception will be thrown.
   *
   * If both vectors are view vectors, then this is the same
   * as <code> insert (position, other.begin(), other.end()) </code>.
   *
   * Otherwise, the elements from @c other will be inserted into this vector.
   * This vector will take ownership of the elements, and the ownership
   * mode of @c other will be changed to @c VIEW_ELEMENTS.
   * Auxiliary data for these elements will be transferred,
   * using move semantics if possible.  (Thus, the auxiliary store
   * for @c other may be modified and must not be locked.)
   * Finally, the auxiliary store pointer for @c other will be cleared
   * (but the store itself will not be deleted since it's not owned
   * by the vector).
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void insertMove (iterator position, DataVector& other);


  //@}
  //========================================================================
  /** @name Erasure operations. */
  //@{


  /**
   * @brief Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or @c end()).
   *
   * If the container owns its elements, then the pointed-to element
   * will be deleted.
   */
  iterator erase(iterator position);


  /**
   * @brief Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or @c end()).
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  iterator erase(iterator first, iterator last);


  /**
   * @brief Remove the last element from the collection.
   *
   * If the container owns its elements, then the removed element
   * will be deleted.
   */
  void pop_back();


  /**
   * @brief Erase all the elements in the collection.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear();


  //@}
  //========================================================================
  /** @name Swap and sort. */
  //@{


  /**
   * @brief Swap this collection with another.
   * @param rhs The collection with which to swap.
   *
   * Ownership is swapped along with the collection content.
   *
   * Note: this method may only be called using the most-derived
   * @c DataVector in the hierarchy.  The @a rhs must also be
   * referenced using the most-derived @c DataVector.
   */
  void swap(DataVector& rhs);


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap/
   */
  static void iter_swap (iterator a, iterator b);


  /**
   * @brief Sort the container.
   *
   * This just sorts by pointer value, so it's probably not very useful.
   */
  void sort();


  /**
   * @brief Sort the container with a user-specified comparison operator.
   * @param comp Functional to compare two values.
   *
   */
  template <class COMPARE>
  void sort(COMPARE comp);


  //@}
  //========================================================================
  /** @name Non-standard operations. */
  //@{


  /**
   * @brief Swap one element out of the container.
   * @param index Index of the element in the container to swap.
   * @param newElem New element to put in the container.
   *                   May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a index of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(size_type index, value_type newElem, reference oldElem);


  /**
   * @brief Swap one element out of the container.
   * @param pos The element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a pos of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(iterator pos, value_type newElem, reference oldElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Swap one element out of the container.
   * @param index Index of the element in the container to swap.
   * @param newElem New element to put in the container.
   *                   May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a index of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.
   *
   * The collection must own its elements to use its interface.
   * The collection will take ownership of @c newElem and will return
   * ownership of @c oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(size_type index,
                   std::unique_ptr<base_value_type> newElem,
                   std::unique_ptr<base_value_type>& oldElem);


  /**
   * @brief Swap one element out of the container.
   * @param pos The element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a pos of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.
   * The collection must own its elements to use its interface.
   * The collection will take ownership of @c newElem and will return
   * ownership of @c oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(iterator pos,
                   std::unique_ptr<base_value_type> newElem,
                   std::unique_ptr<base_value_type>& oldElem);
#endif
#endif


public:
  /**
   * @brief Return the underlying @c std::vector of the container.
   * @return Reference to the @c std::vector actually holding the collection.
   *
   * Note that @c DataVector<T>::stdcont does not necessarily return
   * a @c std::vector<T*> if @c DataVector inheritance is being used.
   */
  const PtrVector& stdcont() const;


  /**
   * @brief Return the ownership policy setting for this container.
   */
  SG::OwnershipPolicy ownPolicy() const;


  /**
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear (SG::OwnershipPolicy ownPolicy);


  /**
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   * @param trackIndices The index tracking policy.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear (SG::OwnershipPolicy ownPolicy,
              SG::IndexTrackingPolicy trackIndices);


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  static const DataModel_detail::DVLInfoBase& dvlinfo();


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  virtual const DataModel_detail::DVLInfoBase& dvlinfo_v() const;


  /**
   * @brief Return the offset of a base @c DataVector class.
   * @param ti @c std::type_info of the desired class.
   *
   * If @c ti represents a @c DataVector base class of this one,
   * then return the offset of that base class.  Otherwise, return -1.
   *
   * This function is here due to limitations of root 6, which can't
   * calculate these offsets correctly from the dictionary if
   * virtual derivation is used.
   */
  static
  int baseOffset (const std::type_info& ti);


  /**
   * @brief Convert to @c AuxVectorBase.
   *
   * Needed to get @x AuxVectorBase from a @c ConstDataVector.
   * Present in @c DataVector as well for consistency.
   */
  const SG::AuxVectorBase& auxbase() const;


  //@}
  //========================================================================
  /** @name Internal operations. */
  //@{


  /**
   * @brief Helper for @c baseOffset.
   * @param p Pointer to the start of the top-level object.
   * @param dv Reference to the DataVector object.
   * @param ti @c std::type_info of the desired class.
   *
   * If @c ti represents a @c DataVector base class of this one,
   * then return the offset of that base class.  Otherwise, return -1.
   *
   */
  static
  int baseOffset1 (const char* p, const DataVector& dv,
                   const std::type_info& ti);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * Call this after some operation that has permuted the elements in the
   * container (such as sort).  The index information in the elements
   * will be used to permute all auxiliary data in the same way.
   * Finally, all the indices will be reset in the correct order.
   */
  void resortAux (iterator beg, iterator end);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * In order to maintain type-safety, we can only allow insertions
   * using the most-derived instance of @c DataVector.  This checks
   * this by testing the @c m_isMostDerived, which is set by the constructors
   * to true only for the most-derived instance.
   * If the test fails, we call to potentially out-of-line code to continue.
   */
  void testInsert (const char* op);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * This continues the test of @c testInsert.  There is one case
   * where @c m_isMostDerived may not be set correctly.  If this container
   * was made via copy construction, then all the @c m_isMostDerived flags
   * will be false.  So we call @c setMostDerived to set the flags correctly
   * and test again.  If the test fails again, then we raise an exception.
   */
  void testInsertOol (const char* op);


private:
  friend class DataModel_detail::ElementProxy<DataVector>;
  template <class DV>
  friend class ConstDataVector;
  friend void test2_assignelement1<DataVector>();
  friend void test2_assignelement2<DataVector>();


  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos, value_type newElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The container must own its elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos,
                      std::unique_ptr<base_value_type> newElem);
#endif
#endif


  /**
   * @brief Handle element assignment from a base pointer.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignBaseElement (typename BaseContainer::iterator pos,
                          typename BaseContainer::value_type newElem);


  /**
   * @brief Shift the auxiliary elements of the container.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * The elements in the container should have already been shifted;
   * this operation will then adjust the element indices and also shift
   * the elements in the vectors for all aux data items.
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
  void shift (size_t pos, ptrdiff_t offs);


public:
  // Make this public so we can call it from DVCollectionProxy.
  /**
   * @brief Helper to shorten calls to @c DataModel_detail::DVLCast.
   * @param p The value to convert.
   * @return The value as a @c const @c T*.
   *
   * This is a no-op for the base class.
   */
  static
  const T* do_cast (const typename PtrVector::value_type p);


private:
  /**
   * @brief Find the most-derived @c DataVector class in the hierarchy.
   * @return The @c type_info for the class for which this method gets run.
   *
   * This is used to generate a nice error message when the most-derived
   * check for insertions fails.
   * Every @c DataVector defines this virtual method, so when it's
   * called, the one corresponding to the most-derived @c DataVector
   * gets run.
   */
  virtual const std::type_info& dv_typeid() const;


protected:
  /**
   * @brief Helper for @c erase().  Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or @c end()).
   *
   * This function factors out common code between @c erase() in the
   * base and derived @c DataVector classes.  It deals with the
   * @c std::vector iterators directly.
   */
  typename PtrVector::iterator
  erase_base(typename PtrVector::iterator position);


  /**
   * @brief Helper for @c erase().  Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or @c end()).
   *
   * This function factors out common code between @c erase() in the
   * base and derived @c DataVector classes.  It deals with the
   * @c std::vector iterators directly.
   */
  typename PtrVector::iterator
  erase_base(typename PtrVector::iterator first,
             typename PtrVector::iterator last);


protected:
  /// The ownership policy of this container ---
  /// either SG::OWNS_ELEMENTS or SG::VIEW_ELEMENTS.
  SG::OwnershipPolicy m_ownPolicy;

  /// This actually holds the elements.
  PtrVector m_pCont;


  /**
   * @brief Clear @c m_isMostDerived for this instance and for all bases.
   *
   * Called from the constructor after setting @c m_isMostDerived.
   */
  void clearMostDerived();


  /**
   * @brief Set @c m_isMostDerived for this instance and clear it for all bases.
   *
   * Called from @c testInsert if the test fails.  The flag may not have
   * been set if this container was made via copy construction, so set
   * it appropriately now so we can test again.
   */
  virtual void setMostDerived();


private:
  //@}


  /// This flag is true if this DV instance is the most-derived one.
  /// We set this to true in the top-level constructor; the constructor
  /// then calls clearMostDerived on the base classes.
  SG::IsMostDerivedFlag m_isMostDerived;


  /// The DV/DL info struct for this class.
  static DataModel_detail::DVLInfo<DataVector<T> > s_info;


  typedef typename
    ROOT_SELECTION_NS::DataVector<T, DataVector_BASE>::self self;
};

template <class T>
const bool DataVector<T, DataModel_detail::NoBase>::has_virtual;


/**
 * @brief  Vector equality comparison.
 * @param  a  A @c DataVector.
 * @param  b  A @c DataVector of the same type as @a x.
 * @return  True iff the size and elements of the vectors are equal.
 *
 * This is an equivalence relation.  It is linear in the size of the
 * vectors.  Vectors are considered equivalent if their sizes are equal,
 * and if corresponding elements compare equal.
 */
template <class T>
bool operator== (const DataVector<T>& a, const DataVector<T>& b);


/// Based on operator==
template <class T>
bool operator!= (const DataVector<T>& a, const DataVector<T>& b);


/**
 * @brief  Vector ordering relation.
 * @param  a  A @c DataVector.
 * @param  b  A @c DataVector of the same type as @a x.
 * @return  True iff @a x is lexicographically less than @a y.
 *
 * This is a total ordering relation.  It is linear in the size of the
 * vectors.  Comparisons are done on the pointer values of the elements.
 *
 * See @c std::lexicographical_compare() for how the determination is made.
 */
template <class T>
bool operator< (const DataVector<T>& a, const DataVector<T>& b);


/// Based on operator<
template <class T>
bool operator> (const DataVector<T>& a, const DataVector<T>& b);


/// Based on operator<
template <class T>
bool operator<= (const DataVector<T>& a, const DataVector<T>& b);


/// Based on operator<
template <class T>
bool operator>= (const DataVector<T>& a, const DataVector<T>& b);


/// See @c DataVector<T, BASE>::swap().
template <class T>
void swap (DataVector<T>& a, DataVector<T>& b);


ENTER_ROOT_SELECTION_NS

#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 )

template <class T>
struct SelectVirtBases
{
  typedef int type;
};

template <class B1, class B2, class B3>
struct SelectVirtBases<DataVector_detail::VirtBases<B1, B2, B3> >
{
  typedef ROOT_SELECTION_NS::AUTOSELECT type;
};

template <class T, class BASE>
class DataVector
{
public:
  typedef DataVector<T, BASE> self;

  ROOT_SELECTION_NS::TEMPLATE_DEFAULTS<
   ROOT_SELECTION_NS::NODEFAULT,
    typename ::DataVector<T>::DataVector_BASE> dum1;
  ROOT_SELECTION_NS::NO_SELF_AUTOSELECT dum2;
  ROOT_SELECTION_NS::AUTOSELECT m_pCont;
  ROOT_SELECTION_NS::TRANSIENT m_isMostDerived;
  typename SelectVirtBases<BASE>::type __base1;
};

#else

template< class T, class BASE >
class DataVector : KeepFirstTemplateArguments< 1 >
// The SelectNoInstance type was added after 6.00/02...
#if ROOT_VERSION_CODE > ROOT_VERSION( 6, 0, 2 )
   , SelectNoInstance
#endif // > v6.00/02
{

public:
   /// A helper typedef
   typedef DataVector< T, BASE > self;
#ifndef XAOD_STANDALONE
   /// Automatically generate dictionary for contained vector
    //MN: this causes massive dictionary duplication.  Disabling for now.
   // ROOT_SELECTION_NS::MemberAttributes< kAutoSelected > m_pCont;
#endif  
   /// Declare the automatically created variable transient
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_isMostDerived;

};

#endif // ROOT_VERSION

EXIT_ROOT_SELECTION_NS


#include "AthContainers/ClassName.h"


/**
 * @brief Specialization of @c ClassName for @c DataVector.
 *
 * This overrides the default implementation of @c ClassName
 * to hide @c DataVector's second template parameter.
 */
template <class T>
class ClassName<DataVector<T> >
{
public:
  static std::string name();
};


#ifndef XAOD_STANDALONE


#include "AthContainers/tools/DVLDataBucket.h"
#include "SGTools/DataBucketTraitFwd.h"


namespace SG {


/**
 * @brief Metafunction to find the proper @c DataBucket class for @c T.
 *
 * Specialize this for @c DataVector.
 * See SGTools/StorableConversions.h for an explanation.
 */
template <class T, class U>
struct DataBucketTrait<DataVector<T>, U>
{
  typedef SG::DVLDataBucket<U> type;
  static void init() { DataVector<T>::dvlinfo(); }
};


} // namespace SG


#endif


#include "AthContainers/DataVector.icc"


#if 0
//===================================================================
// Code to try to make the DATAVECTOR_BASES definitions automatically
// based on SG_BASES.  Still have to use one macro, DATAVECTOR_BASES_FROM_SG.
// Currently missing the part that would declare the DataVector relations
// to SG.  Not sure how useful this will be; don't bother trying to
// finish it now.
#include "boost/mpl/if.hpp"
#include "boost/mpl/and.hpp"
#include "boost/mpl/equal_to.hpp"
#define if_  boost::mpl::if_
#define and_ boost::mpl::and_
#define equal_to boost::mpl::equal_to
#define true_  boost::mpl::true_
#define false_ boost::mpl::false_
template <class U> struct not_virtual { typedef true_ type; };
template <class U> struct not_virtual<SG::Virtual<U> > { typedef false_ type; };
template <class U> struct clean_type { typedef U type; };
template <> struct clean_type<SG::NoBase>
{ typedef DataModel_detail::NoBase type; };
template <class U> struct clean_type<SG::Virtual<U> >
{ typedef typename clean_type<U>::type type; };
template <class T>
struct bases_from_sg
{
  typedef typename
          if_<equal_to<typename SG::Bases<T>::Base1, SG::NoBase>,
              DataModel_detail::NoBase,
              if_<and_<equal_to<typename SG::Bases<T>::Base1, SG::NoBase>,
                       not_virtual<typename SG::Bases<T>::Base2> >,
                  typename SG::Bases<T>::Base1,
                  DataVector_detail::VirtBases
                    <clean_type<typename SG::Bases<T>::Base1>,
                     clean_type<typename SG::Bases<T>::Base2>,
                     clean_type<typename SG::Bases<T>::Base3> >
                 > >::type type;
};
#undef if_
#undef and_
#undef equal_to
#undef true_
#undef false_

#define DATAVECTOR_BASES_FROM_SG(T) \
template <> struct DataVectorBase<T>                    \
{ typedef bases_from_sg<T>::type Base; };
//===================================================================
#endif


#endif // not ATHCONTAINERS_DATAVECTOR_H
