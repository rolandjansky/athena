// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainersInterfaces/AuxStore_traits.h
 * @author scott snyder <snyder@bnl.gov>, Paolo Calafiura
 * @date Sep, 2013
 * @brief Associate @c AuxStore classes with EDM container classes.
 *
 * A @c DataVector may have auxiliary data associated with it,
 * which are kept in separate AuxStore objects.
 *
 * When an object is retrieved from StoreGate, StoreGate can also
 * look for an auxiliary store object and associate it with the container.
 * This traits class is used to get the types of objects for which
 * to look in StoreGate.
 *
 * There are two types defined, @c type and @c const_type, for non-const
 * and const AuxStores, respectively.
 *
 * By default, these are @c SG::IAuxStore and @c SG::IConstAuxStore
 * for types which have auxiliary data (either DOBJ or DOBJ::base_value_type
 * derives from @c SG::IAuxElement).  Otherwise, both types default
 * to @c SG::NoAuxStore.
 *
 * These defaults may be overridden using the @c CLASS_AUXSTORE macro.
 */


#ifndef ATHCONTAINERSINTERFACES_AUXSTORE_TRAITS_H
#define ATHCONTAINERSINTERFACES_AUXSTORE_TRAITS_H


#include "AthContainersInterfaces/IAuxElement.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IConstAuxStore.h"


# include <type_traits>
#include <string>



namespace SG {


/**
 * @brief Mark that there's no associated AuxStore class.
 */
class NoAuxStore {};


/// Default traits values for aux data case.
struct AuxStore_traits_AuxDefault
{
  typedef std::true_type flag;

  typedef IAuxStore type;
  static const std::string& typeName()
  { 
    static const std::string name ("SG::IAuxStore");
    return name;
  }

  typedef IConstAuxStore const_type;
  static const std::string& const_typeName()
  { 
    static const std::string name ("SG::IConstAuxStore");
    return name;
  }
};


/// Default traits values for no-aux data case.
struct AuxStore_traits_NoAuxDefault
{
  typedef std::false_type flag;

  typedef NoAuxStore type;
  static const std::string& typeName()
  { 
    static const std::string name ("SG::NoAuxStore");
    return name;
  }

  typedef NoAuxStore const_type;
  static const std::string& const_typeName()
  { 
    static const std::string name ("SG::NoAuxStore");
    return name;
  }
};


/// Helper for the case where DOBJ::base_value_type does not derive
/// from SG::IAuxElement.  Test DOBJ itself.
template <class DOBJ, class FLAG_>
struct AuxStore_traits1
{
  typedef typename std::conditional<
    std::is_base_of<IAuxElement, DOBJ>::value,
                                AuxStore_traits_AuxDefault,
                                AuxStore_traits_NoAuxDefault>::type
    type;
};


/// Helper for the case where DOBJ::base_value_type does derive
/// from SG::IAuxElement.
/// @c AuxStore_traits is instantiated with the second template
/// argument set to @c true_type.  The second argument in this
/// specialization is @c true_type if @c DOBJ::base_value_type
/// derives from @c IAuxElement and @c false_type if it does not.
/// Further, if @c DOBJ does not have an attribute @c base_value_type,
/// then this specialization is ignored (SFINAE).
template <class DOBJ>
struct AuxStore_traits1<DOBJ,
                        typename std::is_base_of<
                          IAuxElement, typename DOBJ::base_value_type>::type>
{
  typedef AuxStore_traits_AuxDefault type;
};


/**
 * @brief Associate @c AuxStore classes with EDM container classes
 *       (default implementation).
 *
 * Yields @c SG::IAuxStore and @c SG::IConstAuxStore for types that
 * can have auxiliary data and @c NoAuxStore for others.
 */
template <class DOBJ>
struct AuxStore_traits
  : public AuxStore_traits1<DOBJ, std::true_type>::type
{
  /// Attributes inherited from the base class:
  ///   typedef flag
  ///   typedef type
  ///   typedef const_type
  ///   function typeName
  ///   function const_typeName
};


/**
 * @brief Associate @c AuxStore classes with EDM container classes.
 *        This specialization allows using pointer types directly
 *        as an argument.
 */
template <class DOBJ>
struct AuxStore_traits<DOBJ*> : public AuxStore_traits<DOBJ> {};



} // namespace SG


#endif // not ATHCONTAINERSINTERFACES_AUXSTORE_TRAITS_H
