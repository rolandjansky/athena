// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/tools/AtomicConstAccessor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2018
 * @brief Access an auxiliary variable atomically.
 *
 * This is like SG::AuxElement::ConstAccessor, except that accesses to the
 * underlying @c T variable are cast to @c std::atomic<T>.
 * This will generally work only for fundamental integer types
 * or for pointers.
 *
 * This is the way to access variables that were declared with Flags::Atomic .
 * Be aware, however, that  it is still possible to have non-atomic accesses
 * to such variables via either the implicit operations provided by
 * AuxTypeRegistry or by explicit access to the value vector.
 * Therefore, this is not recommended for general use.
 * Contact core software before using this for new code.
 */


#ifndef ATHCONTAINERS_ATOMICCONSTACCESSOR_H
#define ATHCONTAINERS_ATOMICCONSTACCESSOR_H


#include "AthContainers/AuxElement.h"
#include "AthContainers/tools/AuxDataTraits.h"
#include <atomic>
#include <type_traits>


namespace SG {


/**
 * @brief Access an auxiliary variable atomically.
 */
template <class T>
class AtomicConstAccessor
  : public AuxElement::ConstAccessor<T>
{
public:
  /// Base class.
  typedef AuxElement::ConstAccessor<T> Base;

  /// Type referencing an item.
  typedef const std::atomic<T>& const_reference_type;

  /// Type the user sees.
  typedef typename AuxDataTraits<T>::element_type element_type;

  /// Pointer into the container holding this item.
  typedef const std::atomic<T>* const_container_pointer_type;

  // Consistency checks.
  static_assert (std::is_same<T, typename Base::element_type>::value,
                 "Bad type for AtomicConstAccessor");
  static_assert (std::is_same<const T&, typename Base::const_reference_type>::value,
                 "Bad type for AtomicConstAccessor");

  static_assert (std::is_same<const T*, typename Base::const_container_pointer_type>::value,
                 "Bad type for AtomicConstAccessor");
  static_assert (sizeof(T) == sizeof(std::atomic<T>),
                 "Bad type for AtomicConstAccessor");
  static_assert (std::atomic<T>::is_always_lock_free,
                 "Bad type for AtomicConstAccessor");

  /**
   * @brief Constructor.
   * @param name Name of this aux variable.
   *
   * The name -> auxid lookup is done here.
   */
  AtomicConstAccessor (const std::string& name);


  /**
   * @brief Constructor.
   * @param name Name of this aux variable.
   * @param clsname The name of its associated class.  May be blank.
   *
   * The name -> auxid lookup is done here.
   */
  AtomicConstAccessor (const std::string& name, const std::string& clsname);


  /**
   * @brief Fetch the variable for one element, as a const reference.
   * @param e The element for which to fetch the variable.
   *
   * As this class can be used only read-only for basic types, return
   * the result by value.  That makes it easier to call from python.
   */
  T operator() (const AuxElement& e) const;


  /**
   * @brief Fetch the variable for one element, as a const reference.
   * @param container The container from which to fetch the variable.
   * @param index The index of the desired element.
   *
   * This allows retrieving aux data by container / index.
   * Looping over the index via this method will be faster then
   * looping over the elements of the container.
   *
   * As this class can be used only read-only for basic types, return
   * the result by value.  That makes it easier to call from python.
   */
  T operator() (const AuxVectorData& container, size_t index) const;


  /**
   * @brief Get a pointer to the start of the auxiliary data array.
   * @param container The container from which to fetch the variable.
   */
  const_container_pointer_type getDataArray (const AuxVectorData& container) const;
};


} // namespace SG


#include "AthContainers/tools/AtomicConstAccessor.icc"


#endif // not ATHCONTAINERS_ATOMICCONSTACCESSOR_H
