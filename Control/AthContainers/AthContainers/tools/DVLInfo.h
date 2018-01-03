// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DVLInfo.h,v 1.2 2008-06-17 00:44:17 ssnyder Exp $

/**
 * @file  AthContainers/tools/DVLInfo.h
 * @author scott snyder
 * @date Mar, 2008
 * @brief Holder to implement conversion copies for @c DataVector/@c DataList.
 */


#ifndef ATHCONTAINERS_TOOLS_DVLINFO_H
#define ATHCONTAINERS_TOOLS_DVLINFO_H


#include "AthContainers/tools/ClassID.h"
#include "AthContainers/AuxVectorBase.h"
#include <typeinfo>
#include <cstddef>
#include <type_traits>



namespace DataModel_detail {


/**
 * @brief Helper to iterate over a DV container.
 */
class DVLIteratorBase
{
public:
  /// Destructor.
  virtual ~DVLIteratorBase() {}


  /**
   * @brief Return the next element from the container.
   *
   * This is the next pointer from the container (it's a pointer to the
   * element itself, not a pointer to the pointer.  It will be properly
   * cast to the type described by elt_tinfo().  Null pointers are skipped;
   * this function signals the end of iteration by returning 0.
   */
  virtual const void* next() = 0;
};


/*
 * @brief Holder to implement conversion copies for @c DataVector/@c DataList.
 *
 * In some cases, we want to convert @c DataVector\<A> to @c DataVector\<B>,
 * where the two @c DataVector classes are not related by inheritance,
 * but where @c A and @c B are.  In such a case, we can make a new view
 * container for @c DataVector\<B> and fill it with the pointers
 * from @c DataVector\<A> (suitably converted).  In order for this
 * to work with the conversion machinery in SGTools, we must be able
 * to do this given only the @c type_info for @c DataVector\<B>.
 * Thus, we build a registry of @c DVLInfo objects, indexed
 * by the @c type_info; the @c DVLInfo objects have virtual methods
 * to build the new container and to fill it.  (Everything said here
 * for @c DataVector also applies to @c DataList.)
 *
 * Note: these objects should only be allocated statically.
 */
class DVLInfoBase
{
public:
  /**
   * @brief Constructor.
   * @param tinfo Type info object for the container being described.
   * @param elt_tinfo Type info object for the element type of the container
   *                  being described (with pointer and const's stripped.)
   *
   * Note: these objects should only be allocated statically.
   */
  DVLInfoBase (const std::type_info& tinfo,
               const std::type_info& elt_tinfo);


  /// Destructor.
  virtual ~DVLInfoBase() {}


  /**
   * @brief Return the @c type_info for the container.
   */
  const std::type_info& tinfo() const;


  /**
   * @brief Return the CLID for the container.
   */
  CLID clid() const;


  /**
   * @brief Return the @c type_info for the container's element.
   *        (Pointer and const stripped.)
   */
  const std::type_info& elt_tinfo() const;


  /**
   * @brief Construct a new container.
   * @param nreserve Number of elements for which to reserve space.
   *                 (Ignored if not appropriate.)
   */
  virtual void* make (size_t nreserve) const = 0;


  /**
   * @brief Push a new pointer into the container.
   * @param cont_p Pointer to the container.
   * @param elt_p Pointer to the element to push.
   *              (Must match the container's declared element type.)
   */
  virtual void push (void* cont_p, void* elt_p) const = 0;


  /**
   * @brief Return the size of the container.
   * @param cont_p Pointer to the container.
   */
  virtual size_t size (void* cont_p) const = 0;


  /**
   * @brief Erase the elements in the container.
   * @param cont_p Pointer to the container.
   */
  virtual void clear (void* cont_p) const = 0;


  /**
   * @brief Delete a container.
   * @param cont_p Pointer to the container.
   */
  virtual void del (void* cont_p) const = 0;


  /**
   * @brief Copy a container.
   * @param cont_p Pointer to the container.
   */
  virtual void* clone (void* cont_p) const = 0;


  /**
   * @brief Return a new iterator object.
   * @param cont_p Pointer to the container.
   */
  virtual DVLIteratorBase* iterator (const void* cont_p) const = 0;


  /**
   * @brief Return a pointer to the container base.
   * @param cont_p Pointer to the container.
   */
  virtual SG::AuxVectorBase* base (void* cont_p) const = 0;


  /**
   * @brief Find the @c DVLInfo for the container @a tinfo.
   * @param tinfo @c type_info of the desired container.
   * @returns Pointer to the @c DVLInfo, or 0 if not found.
   */
  static DVLInfoBase* find (const std::type_info& tinfo);


  /**
   * @brief Find the @c DVLInfo for the container @a clid.
   * @param clid @c CLID of the desired container.
   * @returns Pointer to the @c DVLInfo, or 0 if not found.
   */
  static DVLInfoBase* find (CLID clid);


private:
  /// The @c type_info of the container.
  const std::type_info& m_tinfo;

  /// The @c type_info of the container's element.
  const std::type_info& m_elt_tinfo;
};


/*
 * @brief Per-container @c DVLInfo.
 *
 * One instance of this class is instantiated for each
 * @c DataVector/@c DataList class.
 */
template <class T>
class DVLInfo
  : public DVLInfoBase
{
public:
  /// Container type.
  typedef T Container;

  /// The container's element type (with pointer and any const removed).
  typedef typename std::remove_const<typename Container::base_value_type>::type Elt;


  /**
   * @brief Constructor.
   *
   * Note: these objects should only be allocated statically.
   */
  DVLInfo();


  /**
   * @brief Construct a new container.
   * @param nreserve Number of elements for which to reserve space.
   *                 (Ignored if not appropriate.)
   */
  virtual void* make (size_t nreserve) const;


  /**
   * @brief Push a new pointer into the container.
   * @param cont_p Pointer to the container.
   * @param elt_p Pointer to the element to push.
   *              (Must match the container's declared element type.)
   */
  virtual void push (void* cont_p, void* elt_p) const;


  /**
   * @brief Return the size of the container.
   * @param cont_p Pointer to the container.
   */
  virtual size_t size (void* cont_p) const;


  /**
   * @brief Erase the elements in the container.
   * @param cont_p Pointer to the container.
   */
  virtual void clear (void* cont_p) const;


  /**
   * @brief Delete a container.
   * @param cont_p Pointer to the container.
   */
  virtual void del (void* cont_p) const;


  /**
   * @brief Copy a container.
   * @param cont_p Pointer to the container.
   */
  virtual void* clone (void* cont_p) const;


  /**
   * @brief Return a new iterator object.
   * @param cont_p Pointer to the container.
   */
  virtual DVLIteratorBase* iterator (const void* cont_p) const;


  /**
   * @brief Return a pointer to the container base.
   * @param cont_p Pointer to the container.
   */
  virtual SG::AuxVectorBase* base (void* cont_p) const;


  /**
   * @brief Helper to create the @c DVLInfo static instance.
   *
   * Calling this will create the @c DVLInfo static instance.
   * This can be called from, for example, an initializer for
   * a class static variable.
   */
  static const std::type_info* initHelper();
};


template <class T>
class DVLIterator
  : public DVLIteratorBase
{
public:
  typedef typename T::const_iterator base_iterator;


  /**
   * @brief Constructor.
   * @param beg Start of the container.
   * @param end End of the container.
   */
  DVLIterator (const base_iterator& beg, const base_iterator& end);


  /**
   * @brief Return the next element from the container.
   *
   * This is the next pointer from the container (it's a pointer to the
   * element itself, not a pointer to the pointer).  It will be properly
   * cast to the type described by elt_tinfo().  Null pointers are skipped;
   * this function signals the end of iteration by returning 0.
   */
   virtual const void* next();


private:
  typename T::const_iterator m_it;
  typename T::const_iterator m_end;
};


/**
 * @brief Perform @c DataVector/@c DataList conversion copying.
 * @param src The source container.
 * @param targ_info The @c DVLInfo for the target container type.
 * @return The new container, or 0.
 *
 * If the elements of @a src can be converted to elements
 * of a @a targ_tinfo container, then we make a new (view)
 * container of that type and populate it with elements copied from @a src.
 */
template <class T>
void* dvl_convert (const T& src,
                   const DVLInfoBase& targ_info);


/**
 * @brief Perform @c DataVector/@c DataList conversion copying.
 * @param src The source container.
 * @param targ_tinfo @c type_info for the desired container type.
 * @param targ_info[out] The @c DVLInfo for the target container type.
 * @return The new container, or 0.
 *
 * If the elements of @a src can be converted to elements
 * of a @a targ_tinfo container, then we make a new (view)
 * container of that type and populate it with elements copied from @a src.
 */
template <class T>
void* dvl_convert (const T& src,
                   const std::type_info& targ_tinfo,
                   DVLInfoBase*& targ_info);


/**
 * @brief Perform @c DataVector/@c DataList conversion copying.
 * @param src The source container.
 * @param clid CLID for the desired container type.
 * @param targ_info[out] The @c DVLInfo for the target container type.
 * @return The new container, or 0.
 *
 * If the elements of @a src can be converted to elements
 * of a @a targ_tinfo container, then we make a new (view)
 * container of that type and populate it with elements copied from @a src.
 */
template <class T>
void* dvl_convert (const T& src,
                   CLID clid,
                   DVLInfoBase*& targ_info);


/**
 * @brief Update the elements in the target container from the source.
 * @param src The source container.
 * @param target The target container.
 * @param targ_info The @c DVLInfo for the target container type.
 *
 * The target container is cleared and then repopulated from the elements
 * of the source container.
 */
template <class T>
void dvl_update (const T& src,
                 void* target,
                 const DVLInfoBase* targ_info);



} // namespace DataModel_detail


#include "AthContainers/tools/DVLInfo.icc"


#endif // not ATHCONTAINERS_TOOLS_DVLINFO_H
