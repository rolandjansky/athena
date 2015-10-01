// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Exceptions that can be thrown from AthLinks.
 */


#ifndef ATHLINKS_EXCEPTIONS_H
#define ATHLINKS_EXCEPTIONS_H


#include "AthenaKernel/sgkey_t.h"
#include "CxxUtils/noreturn.h"
#include "GaudiKernel/ClassID.h"
#include <stdexcept>


namespace SG {


/**
 * @brief Exception --- The object referenced by a DataLink / ElementLink is
 *                      not registered in SG.
 *
 * You have a DataLink or ElementLink that's referencing an object
 * directly by a pointer.  You attempted an operation on the link
 * that requires finding the object's StoreGate information (such as trying
 * to make it persistent), but the object had not been registered
 * in StoreGate.
 */
class ExcPointerNotInSG
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param pointer Pointer to the object that the link is referencing.
   */
  ExcPointerNotInSG (const void* pointer);
};


/**
 * @brief Exception --- Attempt to set DataLink / ElementLink with CLID <clid>
 *                      to object with CLID <clid>.
 *
 *
 * The object being assigned to a link doesn't match the link's
 * declared type.  It should not be possible to do this through the
 * link's public interfaces, but it could occur reading a corrupt
 * input file.
 */
class ExcCLIDMismatch
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param obj_clid The CLID of the object being assigned to the link.
   * @param link_clid The declared CLID of the link.
   */
  ExcCLIDMismatch (CLID obj_clid, CLID link_clid);
};


/**
 * @brief Exception --- Attempt to dereference invalid DataLink / ElementLink "
 *
 * You tried to dereference a link that either is null or for which the
 * referenced object can not be retrieved from StoreGate.
 */
class ExcInvalidLink
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param clid CLID of the link.
   * @param key String key of the link.
   * @param sgkey Hashed key of the link.
   */
  ExcInvalidLink (CLID clid, const std::string& key, SG::sgkey_t sgkey);
};


/**
 * @brief Throw a SG::ExcInvalidLink exception.
 * @param clid CLID of the link.
 * @param key String key of the link.
 * @param sgkey Hashed key of the link.
 */
ATH_NORETURN(void throwExcInvalidLink (CLID clid,
                                       const std::string& key,
                                       SG::sgkey_t sgkey));


/**
 * @brief Exception --- ForwardIndexingPolicy: internal link state is invalid.
 *
 * You tried to dereference a link to a container described by
 * ForwardIndexingPolicy (a vector or similar), but the index
 * was outside of the range of the container.
 */
class ExcBadForwardLink
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param index Index in the link.
   * @param size Size of the referenced container.
   */
  ExcBadForwardLink (size_t index, size_t size);
};


/**
 * @brief Throw a SG::ExcBadForwardLink exception.
 * @param index Index in the link.
 * @param size Size of the referenced container.
 */
ATH_NORETURN(void throwExcBadForwardLink (size_t index, size_t size));


/**
 * @brief Exception --- element not found
 *
 * You had an ElementLink initialized with a pointer to the element directly.
 * Some operation requires the index of the element within its container,
 * but the element is not present in the container.
 */
class ExcElementNotFound
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param where The operation being attempted.
   */
  ExcElementNotFound (const std::string& where);
};


/**
 * @brief Throw a SG::ExcElementNotFound exception.
 * @param where The operation being attempted.
 */
ATH_NORETURN(void throwExcElementNotFound (const char* where));


/// Backwards compatibility.
typedef ExcElementNotFound maybe_thinning_error;



/**
 * @brief Exception --- invalid index
 *
 * You tried to set the index for an ElementLink to an invalid value.
 */
class ExcInvalidIndex
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param where The operation being attempted.
   */
  ExcInvalidIndex (const std::string& where);
};


/**
 * @brief Throw a SG::ExcInvalidIndex exception.
 * @param where The operation being attempted.
 */
ATH_NORETURN(void throwExcInvalidIndex (const char* where));


/**
 * @brief Exception --- index not found
 *
 * You have an ElementLink referencing an associative container.
 * You tried to dereference the link, but the index stored in the link
 * wasn't present in the container.
 */
class ExcIndexNotFound
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param where The operation being attempted.
   */
  ExcIndexNotFound (const std::string& where);
};


/**
 * @brief Throw a SG::ExcIndexNotFound exception.
 * @param where The operation being attempted.
 */
ATH_NORETURN(void throwExcIndexNotFound (const char* where));


/**
 * @brief Exception --- incomparable ElementLink
 *
 * In order to comare two ElementLink instances, both must have available
 * both the StoreGate key of the container and the index within the
 * container.  If only a pointer to the element is available, then
 * the link cannot be compared.  (This is to avoid the results
 * of comparisons possibly changing.)
 */
class ExcIncomparableEL
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   */
  ExcIncomparableEL();
};


/**
 * @brief Throw a SG::IncomparableEL exception.
 */
ATH_NORETURN(void throwExcIncomparableEL());


} // namespace SG


#endif // not ATHLINKS_EXCEPTIONS_H
