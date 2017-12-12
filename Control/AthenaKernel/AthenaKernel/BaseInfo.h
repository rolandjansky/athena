// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BaseInfo.h,v 1.11 2008-12-15 16:22:45 ssnyder Exp $

/**
 * @file  AthenaKernel/BaseInfo.h
 * @author scott snyder
 * @date Nov 2005
 * @brief Provide an interface for finding inheritance information
 *        at run time.
 *
 * The @a SG::BaseInfo<T> class provides an interface for finding
 * inheritance information about class @a T.  In the absence of compiler
 * support for reflection, the inheritance information must be
 * explicitly declared.  This is done with the @a SG_BASE macro
 * and friends.  To declare that class @a D derives from class @a B, use
 *
 *@code
 *   struct B {};
 *   struct D : public B {};
 *   SG_BASE (D, B);
 @endcode
 *
 * You can also use multiple inheritance with the @a SG_BASES2
 * and @a SG_BASES3 macros:
 *
 *@code
 *   struct B1 {};
 *   struct B2 {};
 *   struct B3 {};
 *   struct D : public B1, public B2, public B3 {};
 *   SG_BASES3 (D, B1, B2, B3);
 @endcode
 *
 * Supporting more than three base classes requires (straightforward)
 * changes to the code here.
 *
 * If any of the derivations are virtual, the corresponding
 * base class should be within a @a SG_VIRTUAL macro:
 *    
 *@code
 *   struct B1 {};
 *   struct B2 {};
 *   struct D : public B1, virtual public B2 {};
 *   SG_BASES2 (D, B1, SG_VIRTUAL (B2));
 @endcode
 *
 * Note that these macros will only work with non-templated types,
 * or with specific instantiations of templated types.  If you want
 * a templated @a SG_BASE, you'll need to declare the specialization
 * yourself.  You should specialize @a SG::Bases<T>, defining types
 * @a Base1, @a Base2, and @a Base3 (which should be @a SG::NoBase
 * if not used).  Example:
 *
 *@code
 *   template <class T> struct B {};
 *   template <class T> struct D : public B<T> {};
 *   namespace SG {
 *     template <class T>
 *     struct Bases<D<T> > {
 *       typedef B<T> Base1;
 *       typedef NoBase Base2;
 *       typedef NoBase Base3;
 *     };
 *   }
 @endcode
 *
 * Once these declarations are in place, what can you do with it?
 * The interface is provided by the @a SG::BaseInfo<T> class.
 * (This is a wrapper around a singleton, so it's not expensive
 * to create instances of this class.)  You can specify the base
 * classes either by the Gaudi class ID or by the C++ @c std::type_info.
 * Of course, you'll only be able to get information by class ID
 * if the classes actually have IDs defined.
 *
 * Here are the available methods of @a SG::BaseInfo<T>:
 *
 *@code
 *   static bool is_base (CLID clid)
 *   static bool is_base (const std::type_info& tinfo)
 @endcode
 *     Test to see if the type given by @a clid or @a tinfo
 *     is a base of @a T.  Note that @a T counts as its own base.
 *
 *@code
 *   static bool is_virtual (CLID clid)
 *   static bool is_virtual (const std::type_info& tinfo)
 @endcode
 *     Test to see if the type given by @a clid or @a tinfo
 *     is a virtual base of @a T.  This should always be @a false
 *     for @a T itself.
 *
 *@code
 *   static void* cast (T* p, CLID clid)
 *   static void* cast (T* p, const std::type_info& tinfo)
 @endcode
 *     Cast a pointer from @a T* to a pointer to the type given
 *     by @a clid or @a tinfo, which must be a base if @a T
 *     known to @a BaseInfo.  The result is returned as a @a void*.
 *     If the conversion cannot be done (because the target is not
 *     known to be a base of @a T), then 0 is returned.
 *
 *@code
 *   static T* castTo (void* p, CLID clid)
 *   static T* castTo (void* p, const std::type_info& tinfo)
 @endcode
 *     Similar, except converts from a pointer to the type given by
 *     @a clid or @a tinfo (which must be a base of @a T) to @a T*.
 *     This involves a @a dynamic_cast.  Returns 0 if the cast fails.
 *
 *@code
 *   static std::vector<CLID> get_bases ()
 @endcode
 *     Return all the known bases of @a T (that have class IDs).
 *     @a T itself will be included in this list.
 *
 *@code
 *   static std::vector<const std::type_info*> get_ti_bases ()
 @endcode
 *     Return all the known bases of @a T.
 *     @a T itself will be included in this list.
 *
 * It is also possible to get a non-templated version of @c SG::BaseInfo<T>.
 * This is called @c SG::BaseInfoBase.  These objects can be found using
 * @c SG::BaseInfoBase::find, by either class ID or @c std::type_info
 * (in order for this to work, the corresponding @c SG::BaseInfo<T> class
 * must have been used somewhere in the program).  The interface
 * of @c SG::BaseInfoBase is the same as @c SG::BaseInfo<T>, except
 * that @c void* replaces @c T* in the @c cast methods.
 *
 * Initialization issues: We don't want to build the @c SG::BaseInfo<T>
 * objects at static initialization time.  But we do need to remember
 * which ones are available.  Thus, @c BaseInfoBase maintains a list
 * of CLIDs and @c std::type_info's for which we may not have done
 * initialization, along with a function to call to do the initialization.
 * The initialization list is filled during static initialization.
 * When we look for an instance, if we don't find one, we look in the
 * initialization list; if there's a match there, we run the initialization
 * and remove it from the list.
 *
 * Copy conversions: The conversion machinery above provides access to the
 * standard C++ base<>derived conversions.  However, sometimes you'd like
 * to allow additional conversions as well, for example between a
 * vector<ElementLink<T> > and DataVector<T>.  For those cases,
 * you can register a conversion function with the _source_ type
 * that can initialize an instance of the _destination_ type
 * from the _source_ type.  Then, when you try to retrieve the object
 * from StoreGate with the _destination_ type, your conversion function
 * will be called.
 *
 * Your conversion function should be the method @a convert of
 * a class deriving from the @a SG::CopyConversion template;
 * for example,
 *
 *@code
 *   class MyCopyConversion
 *     : public SG::CopyConversion<std::vector<ElementLink<MyType> >,
 *                                 DataVector<MyType> >
 *   {
 *   public:
 *     virtual void convert (const std::vector<ElementLink<MyType> >& src,
 *                           DataVector<MyType>& dst) const
 *     {
 *       size_t sz = src.size();
 *       if (dst.size() != sz) {
 *         dst.clear (SG::VIEW_ELEMENTS);
 *         dst.reserve (sz);
 *         for (size_t i = 0; i < sz; i++) {
 *           const MyType* p = *(src[i]).cptr(); 
 *           dst.push_back (const_cast<MyType*> (p));
 *         }
 *       }
 *     }
 *   };
 @endcode
 *
 * You then declare this using the macro
 *
 @code
 *  SG_ADD_COPY_CONVERSION (std::vector<ElementLink<MyType> >,
 *                          MyCopyConversion);
 @endcode
 *
 * Copy conversions are enabled only for objects that have been marked as const.
 */

#ifndef ATHENAKERNEL_BASEINFO_H
#define ATHENAKERNEL_BASEINFO_H

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ClassID.h"
#include <vector>
#include <typeinfo>
#include <type_traits>


//===========================================================================
// Macros used to declare base class information.
//

/**
 * @brief Used to mark virtual derivation.
 *        When using the @a SG_BASE macros below, use this in the case
 *        of virtual derivation.  Example:
 *
 *@code
 *   struct B1 {};
 *   struct B2 {};
 *   struct D : public B1, virtual public B2 {};
 *   SG_BASES2 (D, B1, SG_VIRTUAL (B2));
 @endcode
 */
#define SG_VIRTUAL(T) Virtual<T>


/**
 * @brief Declare that class @a D derives from class @a B.  Example:
 *
 *@code
 *   struct B {};
 *   struct D : public B {};
 *   SG_BASE (D, B);
 @endcode
 */
#define SG_BASE(D, B) SG_BASES1(D, B)


/**
 * @brief Declare that class @a D derives from class @a B.
 *        This is the same as @a SG_BASE.  Example:
 *
 *@code
 *   struct B {};
 *   struct D : public B {};
 *   SG_BASES1 (D, B);
 @endcode
 */
#define SG_BASES1(D, B)          \
  namespace SG        {          \
    template<> struct Bases<D >{ \
      typedef B Base1;           \
      typedef NoBase Base2;      \
      typedef NoBase Base3;      \
    };                           \
    template struct RegisterBaseInit<D >; \
    template struct BaseInit<D >; \
} struct sg_dummy // to swallow semicolon


/**
 * @brief Declare that class @a D derives from classes @a B1 and @a B2.
 *        Example:
 *
 *@code
 *   struct B1 {};
 *   struct B2 {};
 *   struct D : public B1, public B2 {};
 *   SG_BASES2 (D, B1, B2);
 @endcode
 */
#define SG_BASES2(D, B1, B2)     \
  namespace SG        {          \
    template<> struct Bases<D >{ \
      typedef B1 Base1;          \
      typedef B2 Base2;          \
      typedef NoBase Base3;      \
    };                           \
    template struct RegisterBaseInit<D >; \
    template struct BaseInit<D >; \
} struct sg_dummy // to swallow semicolon


/**
 * @brief Declare that class @a D derives from classes @a B1, @a B2, and @a B3.
 *        Example:
 *
 *@code
 *   struct B1 {};
 *   struct B2 {};
 *   struct B3 {};
 *   struct D : public B1, public B2, public B3 {};
 *   SG_BASES3 (D, B1, B2, B3);
 @endcode
 */
#define SG_BASES3(D, B1, B2, B3) \
  namespace SG        {          \
    template<> struct Bases<D >{ \
      typedef B1 Base1;          \
      typedef B2 Base2;          \
      typedef B3 Base3;          \
    };                           \
    template struct RegisterBaseInit<D >; \
    template struct BaseInit<D >; \
} struct sg_dummy // to swallow semicolon



/**
 * @brief Add a new base class @c B to class @c D.
 *
 * Sometimes, the SG_BASE macro for a class isn't present in its header.
 *
 * One can try to put the appropriate SG_BASE macro somewhere else,
 * but this doesn't always work: if the @c BaseInfoBase for @c D has
 * already been created by the time the @c SG_BASE initialization
 * runs, then that @c SG_BASE macro won't do anything.
 *
 * @c SG_ADD_BASE, however, will add a new base to an existing @c BaseInfoBase.
 *
 *@code
 *   struct B {};
 *   struct D : public B {};
 *   SG_ADD_BASE (D, B);
 @endcode
 */
#define SG_ADD_BASE(D, B) \
  namespace SG        {          \
    template struct AddBaseInit<D, B >;            \
} struct sg_dummy // to swallow semicolon


/**
 * @brief Declare a copy conversion from class @c T using converter @c C.
 *
 * See the comments in the header for a detailed description.
 */
#define SG_ADD_COPY_CONVERSION(D, C) \
  namespace SG        {          \
    template struct AddCopyConversionInit<D, C >;            \
} struct sg_dummy // to swallow semicolon



namespace SG {


/**
 * @brief Helper metafunction to get base class types.
 *
 * For a class @c T,
 *@code
 *   SG::BaseType<SG::Bases<T>::Base1>::type
 @endcode
 * gives the type of @c T's first base.  Also,
 *@code
 *   SG::BaseType<SG::Bases<T>::Base1>::is_virtual
 @endcode
 * tells whether the derivation is virtual
 * (either @c true_type or @c false_type).
 *
 * Note that @c SG::Bases\<T>::Base1 is not the actual type
 * of @c T's first base if virtual derivation was used.
 */
template <class T>
struct BaseType;


// Forward declaration.
template <class T>
class BaseInfoImpl;


struct BaseInfoBaseImpl;


//===========================================================================
// Copy conversion declarations.
//


/**
 * @brief Base class for copy conversions.
 */
class CopyConversionBase
{
public:
  /// Destructor.
  virtual ~CopyConversionBase() {}

  /// Create an instance of the destination class.
  virtual void* create() const = 0;

  /// Destroy an instance of the destination class.
  virtual void destroy (void* p) const = 0;

  /// Convert the contents of an instance of the source class SRC
  /// to an instance of the destination class DST.
  virtual void convertUntyped (const void* src, void* dst) const = 0;
};


/**
 * @brief Base class for copy conversions, templated on source
 *        and destination classes.
 */
template <class SRC, class DST>
class CopyConversion
  : public CopyConversionBase
{
public:
  /// The destination class.
  typedef DST target_type;

  /// Create an instance of the destination class.
  virtual void* create() const { return new DST; }

  /// Destroy an instance of the destination class.
  virtual void destroy (void* p) const { delete reinterpret_cast<DST*>(p); }

  /// Convert the contents of an instance of the source class SRC
  /// to an instance of the destination class DST.
  virtual void convertUntyped (const void* src, void* dst) const
  {
    convert (*reinterpret_cast<const SRC*>(src),
             *reinterpret_cast<DST*>(dst));
  }

  /// Convert the contents of an instance of the source class SRC
  /// to an instance of the destination class DST.
  /// (Type-safe version.)
  virtual void convert (const SRC& src, DST& dst) const = 0;
};


//===========================================================================
// Base class declaration.
// This factors out the part of the implementation that doesn't
// depend on the template parameter @a T.
//


template <class D, class B>
struct RegisterAddBaseInit;
template <class D, class B>
struct RegisterAddCopyConversionInit;


/**
 * @brief The non-template portion of the @a BaseInfo implementation.
 */
class BaseInfoBase
{
public:
  template <class D, class B>
  friend struct RegisterAddBaseInit;
  template <class D, class B>
  friend struct RegisterAddCopyConversionInit;


  /**
   * @brief Return the CLID for this class.
   */
  CLID clid() const;


  /**
   * @brief Return the @c std::type_info for this class.
   */
  const std::type_info& typeinfo() const;
  

  /**
   * @brief Cast to a base pointer.
   * @param p The pointer to cast (a @a T* cast to a @a void*).
   * @param clid ID of the class to which to cast.
   * @return The pointer cast to the requested type, returned
   *         as a @a void*.  @a clid must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  void* cast (void* p, CLID clid) const;

  /**
   * @brief Cast to a base pointer.
   * @param p The pointer to cast (a @a T* cast to a @a void*).
   * @param clid @a type_info of the class to which to cast.
   * @return The pointer cast to the requested type, returned
   *         as a @a void*.  @a tinfo must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  void* cast (void* p, const std::type_info& tinfo) const;


  /**
   * @brief Cast to a derived pointer.
   * @param p The pointer to cast (a @a B* cast to a @a void*).
   * @param clid ID of the class @a B from which to cast.
   * @return The pointer cast to the requested type, returned
   *         as a @a void*.  @a clid must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  void* castTo (void* p, CLID clid) const;

  /**
   * @brief Cast to a derived pointer.
   * @param p The pointer to cast (a @a B* cast to a @a void*).
   * @param clid @a type_info of the class @a B from which to cast.
   * @return The pointer cast to the requested type, returned
   *         as a @a void*.  @a tinfo must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  void* castTo (void* p, const std::type_info& tinfo) const;


  /// Type of a pointer conversion function.
  typedef void* castfn_t (void* p);


  // gcc 4.3 complains about the code genreflex generates for these.
  // They're not useful from python anyway, so just suppress them.
#ifndef __REFLEX__
  /**
   * @brief Return a function for casting to a base pointer.
   * @param clid ID of the class to which to cast.
   * @return A function to convert a pointer to a @c T to a pointer
   *         to the type identified by @a clid.
   *         @a clid must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  castfn_t* castfn (CLID clid) const;


  /**
   * @brief Return a function for casting to a base pointer.
   * @param clid @a type_info of the class to which to cast.
   * @return A function to convert a pointer to a @c T to a pointer
   *         to the type identified by @a tinfo.
   *         @a tinfo must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  castfn_t* castfn (const std::type_info& tinfo) const;

  /**
   * @brief Return a function for casting to a derived pointer.
   * @param clid ID of the class @a B from which to cast.
   * @return A function to convert a pointer to a @a B to a pointer
   *         to a @a T.  @a B must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  castfn_t* castfnTo (CLID clid) const;


  /**
   * @brief Return a function for casting to a derived pointer.
   * @param clid @a type_info of the class @a B from which to cast.
   * @return A function to convert a pointer to a @a B to a pointer
   *         to a @a T.  @a B must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  castfn_t* castfnTo (const std::type_info& tinfo) const;
#endif


  /**
   * @brief Return the class IDs of all known bases of @a T (that
   *        have class IDs).  The list will include @a T itself.
   */
  std::vector<CLID> get_bases () const;

  /**
   * @brief Return the @c type_info's of all known bases of @a T.
   *        The list will include @a T itself.
   */
  std::vector<const std::type_info*> get_ti_bases () const;

  /**
   * @brief Return true if @a clid is the ID of a class that
   *        is known to be a base of @a T.  @a T is considered
   *        to be its own base for this purpose.
   * @param clid The ID of the class to test.
   */
  bool is_base (CLID clid) const;

  /**
   * @brief Return true if @a tinfo is the @a type_info of a class that
   *        is known to be a base of @a T.  @a T is considered
   *        to be its own base for this purpose.
   * @param clid The ID of the class to test.
   */
  bool is_base (const std::type_info& tinfo) const;


  /**
   * @brief Return true if @a clid is the ID of a class that
   *        is known to be a virtual base of @a T.  (This will always
   *        be false for @a T itself.)
   * @param clid The ID of the class to test.
   */
  bool is_virtual (CLID clid) const;

  /**
   * @brief Return true if @a tinfo is the @a std::type_info of a class that
   *        is known to be a virtual base of @a T.  (This will always
   *        be false for @a T itself.)
   * @param tinfo The @a std::type_info of the class to test.
   */
  bool is_virtual (const std::type_info& tinfo) const;


  /**
   * @brief Search for a copy conversion to @c tinfo.
   * @param tinfo The class to which we want to convert.
   *
   * Returns the conversion instance or 0.
   */
  const CopyConversionBase* copy_conversion (const std::type_info& tinfo) const;


  /**
   * @brief Search for a copy conversion to @c clid.
   * @param clid The class to which we want to convert.
   *
   * Returns the conversion instance or 0.
   */
  const CopyConversionBase* copy_conversion (CLID clid) const;


  /**
   * @brief Add a new copy conversion.
   * @param tinfo The @c std::type_info of the target class.
   * @param cnv A @c CopyConversionBase instance describing the conversion.
   *
   * The @c BaseInfoBase takes ownership of the @c cnv object.
   */
  void add_copy_conversion (const std::type_info& tinfo,
                            const CopyConversionBase* cnv);


  /**
   * @brief Return known copy conversions.
   *
   * Returns the CLIDs of all target classes that have been registered
   * with this one for copy conversion.
   */
  std::vector<CLID> get_copy_conversions() const;


#ifndef __REFLEX__
  /**
   * @brief Add information about one base class.
   * @param tinfo The @a std::type_info of the base.
   * @param converter Converter function.  This should be able to
   *                  convert a @a T* to a pointer to this base.
   * @param converterTo Converter function.  This should be able to
   *                    convert a pointer to this base to a @a T*.
   * @param is_virtual True if the derivation from this base to @a T
   *                   is via virtual derivation.
   */
  void add_info (const std::type_info& tinfo,
                 castfn_t* converter,
                 castfn_t* converterTo,
                 bool is_virtual);
#endif


  /**
   * @brief Find the @c BaseInfoBase instance for @c clid.
   * @param clid The class ID of the class for which we want information.
   *
   * Returns 0 if no @c BaseInfoBase instance is available.
   */
  static const BaseInfoBase* find (CLID clid);


  /**
   * @brief Find the @c BaseInfoBase instance for @c tinfo.
   * @param tinfo The @c std::type_info of the class
   *              for which we want information.
   *
   * Returns 0 if no @c BaseInfoBase instance is available.
   */
  static const BaseInfoBase* find (const std::type_info& tinfo);


  /// Type for an initialization function.
  typedef void init_func_t (BaseInfoBase* bib);


#ifndef __REFLEX__
  /**
   * @brief Register an initialization function.
   * @param tinfo The @c std::type_info for the class being registered.
   * @param init_func Function to initialize @c BaseInfo for the class.
   */
  static void addInit (const std::type_info* tinfo,
                       init_func_t* init_func);
#endif


  /**
   * @brief Run initializations for this class, if needed.
   */
  void maybeInit();


protected:
  /**
   * @brief Constructor.
   * @param tinfo The @c std::type_info for this class.
   */
  BaseInfoBase (const std::type_info& tinfo);


  /**
   * @brief Destructor.
   */
  ~BaseInfoBase();


private:
  /**
   * @brief Helper for @c find.
   * @param tinfo The @c std::type_info of the class
   *              for which we want information.
   *
   * Returns 0 if no @c BaseInfoBase instance is available.
   */
  static BaseInfoBase* find1 (const std::type_info& tinfo);


  /// Pointer to internal state.
  BaseInfoBaseImpl* m_impl;

  BaseInfoBase (const BaseInfoBase&);
  BaseInfoBase& operator= (const BaseInfoBase&);
};


//===========================================================================
// The templated @c BaseInfo class.
//

/**
 * @brief Provide an interface for finding inheritance information
 *        at run time.  See the file comments for full details.
 */
template <class T>
class BaseInfo
{
public:
  template <class D, class B>
  friend struct RegisterAddCopyConversionInit;
  template <class D, class B>
  friend struct RegisterAddBaseInit;


  /**
   * @brief Cast to a base pointer.
   * @param p The pointer to cast.
   * @param clid ID of the class to which to cast.
   * @return The pointer cast to the requested type, returned
   *         as a @a void*.  @a clid must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  static void* cast (T* p, CLID clid);

  /**
   * @brief Cast to a base pointer.
   * @param p The pointer to cast.
   * @param clid @a type_info of the class to which to cast.
   * @return The pointer cast to the requested type, returned
   *         as a @a void*.  @a tinfo must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  static void* cast (T* p, const std::type_info& tinfo);


  /**
   * @brief Cast to a derived pointer.
   * @param p The pointer to cast.
   * @param clid ID of the class @a B from which to cast.
   * @return The pointer cast to a @a T*.
   *         @a B must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  static T* castTo (void* p, CLID clid);

  /**
   * @brief Cast to a derived pointer.
   * @param p The pointer to cast.
   * @param clid @a type_info of the class @a B from which to cast.
   * @return The pointer cast to a @a T*.
   *         @a B must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  static T* castTo (void* p, const std::type_info& tinfo);


  /**
   * @brief Return a function for casting to a base pointer.
   * @param clid ID of the class to which to cast.
   * @return A function to convert a pointer to a @c T to a pointer
   *         to the type identified by @a clid.
   *         @a clid must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  static BaseInfoBase::castfn_t* castfn (CLID clid);


  /**
   * @brief Return a function for casting to a base pointer.
   * @param clid @a type_info of the class to which to cast.
   * @return A function to convert a pointer to a @c T to a pointer
   *         to the type identified by @a tinfo.
   *         @a tinfo must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   */
  static BaseInfoBase::castfn_t* castfn (const std::type_info& tinfo);


  /**
   * @brief Return a function for casting to a derived pointer.
   * @param clid ID of the class @a B from which to cast.
   * @return A function to convert a pointer to a @c T to a pointer
   *         to a @a T.  @a clid must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  static BaseInfoBase::castfn_t* castfnTo (CLID clid);


  /**
   * @brief Return a function for casting to a derived pointer.
   * @param clid @a type_info of the class @a B from which to cast.
   * @return A function to convert a pointer to a @c B to a pointer
   *         to a @a T.  @a tinfo must be known to be a base
   *         of @a T; otherwise, 0 will be returned.
   *         0 will also be returned if the @a dynamic_cast fails.
   */
  static BaseInfoBase::castfn_t* castfnTo (const std::type_info& tinfo);


  /**
   * @brief Return the class IDs of all known bases of @a T (that
   *        have class IDs).  The list will include @a T itself.
   */
  static std::vector<CLID> get_bases ();


  /**
   * @brief Return the @c type_info's of all known bases of @a T.
   *        The list will include @a T itself.
   */
  static std::vector<const std::type_info*> get_ti_bases ();


  /**
   * @brief Return true if @a clid is the ID of a class that
   *        is known to be a base of @a T.  @a T is considered
   *        to be its own base for this purpose.
   * @param clid The ID of the class to test.
   */
  static bool is_base (CLID clid);

  /**
   * @brief Return true if @a tinfo is the @a std::type_info of a class that
   *        is known to be a base of @a T.  @a T is considered
   *        to be its own base for this purpose.
   * @param tinfo The @a std::type_info of the class to test.
   */
  static bool is_base (const std::type_info& tinfo);


  /**
   * @brief Return true if @a clid is the ID of a class that
   *        is known to be a virtual base of @a T.  (This will always
   *        be false for @a T itself.)
   * @param clid The ID of the class to test.
   */
  static bool is_virtual (CLID clid);

  /**
   * @brief Return true if @a tinfo is the @a std::type_info of a class that
   *        is known to be a virtual base of @a T.  (This will always
   *        be false for @a T itself.)
   * @param tinfo The @a std::type_info of the class to test.
   */
  static bool is_virtual (const std::type_info& tinfo);


  /**
   * @brief Return the non-templated @c BaseInfoBase object for this type.
   */
  static const BaseInfoBase& baseinfo ();


  static void maybeInit ();


private:
  /// Return a reference to the (singleton) implementation object
  /// for this class.
  static BaseInfoImpl<T>& instance();

  static BaseInfoImpl<T> s_instance ATLAS_THREAD_SAFE;
};


} // namespace SG


#include "AthenaKernel/BaseInfo.icc"


#endif // not ATHENAKERNEL_BASEINFO_H

