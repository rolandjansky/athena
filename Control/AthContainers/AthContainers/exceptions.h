// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: exceptions.h 602555 2014-06-19 03:09:29Z ssnyder $
/**
 * @file AthContainers/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Exceptions that can be thrown from AthContainers.
 */


#ifndef ATHCONTAINERS_EXCEPTIONS_H
#define ATHCONTAINERS_EXCEPTIONS_H


#include "AthContainersInterfaces/AuxTypes.h"
#include "CxxUtils/noreturn.h"
#include <stdexcept>
#include <typeinfo>
#include <string>


namespace SG {


/**
 * @brief Exception --- Aux data requested from object with no store.
 *
 * You attempted to retrieve auxiliary data from an object with no
 * associated auxiliary data store.
 */
class ExcNoAuxStore
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param auxid The ID of the requested aux data item.
   */
  ExcNoAuxStore (SG::auxid_t auxid);


  /**
   * @brief Constructor.
   * @param op The attempted operation.
   */
  ExcNoAuxStore (const char* op);
};


/**
 * @brief Exception --- Attempt to retrieve nonexistent aux data item.
 *
 * You attempted to retrieve auxiliary data from an object, but the
 * requested item does not exist.
 */
class ExcBadAuxVar
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param auxid The ID of the requested aux data item.
   */
  ExcBadAuxVar (SG::auxid_t auxid);
};


/**
 * @brief Exception --- Non-const operation performed on const aux data.
 *
 * You attempted to perform some non-const operation on an object
 * with const auxiliary data.
 */
class ExcConstAuxData
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param op The attempted operation.
   * @param auxid The ID of the requested aux data item, if any.
   */
  ExcConstAuxData (const std::string& op, SG::auxid_t auxid = null_auxid);
};


/**
 * @brief Exception --- Attempt to set aux data store on container
 *                      that doesn't track indices, or disable index tracking
 *                      for a container with aux data.
 */
class ExcUntrackedSetStore
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   */
  ExcUntrackedSetStore();
};



/**
 * @brief Exception --- Bad use of private store.
 *
 * You called @c makePrivateStore when there was already one,
 * or @c releasePrivateStore when there wasn't one.
 */
class ExcBadPrivateStore
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param op The attempted operation.
   */
  ExcBadPrivateStore (const std::string& op);
};



/**
 * @brief Exception --- Type mismatch for aux variable
 *
 * Thrown when there's a mismatch between the type requested for
 * an aux data item and a type previously requested for the same item.
 */
class ExcAuxTypeMismatch
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param auxid ID of the requested aux data item.
   * @param new_type New type requested for the item.
   * @param old_type Previous type associated with the item.
   */
  ExcAuxTypeMismatch (SG::auxid_t auxid,
                      const std::type_info& new_type,
                      const std::type_info& old_type);
};



/**
 * @brief Exception --- Attempted to do OP on a BASE base class of COMPLETE;
 *                      can only be done on the most-derived class.
 *
 * Thrown when there's a mismatch between the type requested for
 * an aux data item and a type previously requested for the same item.
 */
class ExcInsertionInBaseClass
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param op The attempted operation.
   * @param base_type The type on which the operation was attempted.
   * @param complete_type The most-derived type of the object.
   */
  ExcInsertionInBaseClass (const char* op,
                           const std::type_info& base_type,
                           const std::type_info& complete_type);
};


/**
 * @brief Exception --- Attempted to modify auxiliary data in a locked store.
 *
 * Thrown when there was an attempt to access a non-decorator auxiliary
 * item in a locked store, or when some other operation that would
 * change the store is attempted.
 */
class ExcStoreLocked
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param op The attempted operation.
   */
  ExcStoreLocked (SG::auxid_t auxid);


  /**
   * @brief Constructor.
   * @param op The attempted operation.
   */
  ExcStoreLocked (const char* op);
};


/**
 * @brief Exception --- Attempted to insert a unique_ptr to a non-owning container.
 *
 * The @c unique_ptr interfaces can be used only on containers
 * that own their elements.
 */
class ExcNonowningContainer
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   */
  ExcNonowningContainer();
};


/**
 * @brief Throw a SG::ExcNonowningContainer exception.
 */
ATH_NORETURN(void throwExcNonowningContainer());


/**
 * @brief Exception --- Unknown aux data item.
 *
 * This can be thrown by @c TypelessConstAccessor if the requested
 * item is not known to the registry, or (if the @c type_info is supplied)
 * we don't know how to make a vector factory for the given @c type_info.
 */
class ExcUnknownAuxItem
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param name Name of the aux data item.
   * @param clsname Class name of the aux data item, or an empty string.
   * @param typ Type of the item, if provided.
   */
  ExcUnknownAuxItem (const std::string& name,
                     const std::string& clsname = "",
                     const std::type_info* typ = 0);
};


/**
 * @brief Throw a SG::ExcUnknownAuxItem exception.
 * @param name Name of the aux data item.
 * @param clsname Class name of the aux data item, or an empty string.
 * @param typ Type of the item, if provided.
 */
ATH_NORETURN(void throwExcUnknownAuxItem (const std::string& name,
                                          const std::string& clsname = "",
                                          const std::type_info* typ = 0));


} // namespace SG


#endif // not ATHCONTAINERS_EXCEPTIONS_H
