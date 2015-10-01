/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Exceptions that can be thrown from AthLinks.
 */


#include "AthLinks/exceptions.h"
#include <sstream>
#include <string>


namespace SG {


/// For setting debugger breakpoints
void AthLinks_error() {}


//*************************************************************************


/// Helper: format exception error string.
std::string excPointerNotInSG_format (const void* pointer)
{
  std::ostringstream os;
  os << "SG::ExcPointerNotInSG: "
     << "The object referenced by a DataLink / ElementLink is not registered "
     << "in StoreGate: " << pointer << ".";
  return os.str();
}


/**
 * @brief Constructor.
 * @param pointer Pointer to the object that the link is referencing.
 */
ExcPointerNotInSG::ExcPointerNotInSG (const void* pointer)
  : std::runtime_error (excPointerNotInSG_format (pointer))
{
  AthLinks_error();
}


//*************************************************************************


/// Helper: format exception error string.
std::string excCLIDMismatch_format (CLID obj_clid, CLID link_clid)
{
  std::ostringstream os;
  os << "SG::ExcCLIDMismatch: "
     << "Attempt to set DataLink / ElementLink with CLID " << link_clid
     << " to object with CLID " << obj_clid;
  return os.str();
}


/**
 * @brief Constructor.
 * @param obj_clid The CLID of the object being assigned to the link.
 * @param link_clid The declared CLID of the link.
 */
ExcCLIDMismatch::ExcCLIDMismatch (CLID obj_clid, CLID link_clid)
  : std::runtime_error (excCLIDMismatch_format (obj_clid, link_clid))
{
  AthLinks_error();
}


//*************************************************************************


/// Helper: format exception error string.
std::string
excInvalidLink_format (CLID clid, const std::string& key, SG::sgkey_t sgkey)
{
  std::ostringstream os;
  os << "SG::ExcInvalidLink: "
     << "Attempt to dereference invalid DataLink / ElementLink "
     << "[" << clid << "/" << key << "] (" << sgkey << ")";
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid CLID of the link.
 * @param key String key of the link.
 * @param sgkey Hashed key of the link.
 */
ExcInvalidLink::ExcInvalidLink (CLID clid,
                                const std::string& key,
                                SG::sgkey_t sgkey)
  : std::runtime_error (excInvalidLink_format (clid, key, sgkey))
{
  AthLinks_error();
}


/**
 * @brief Throw a SG::ExcInvalidLink exception.
 * @param clid CLID of the link.
 * @param key String key of the link.
 * @param sgkey Hashed key of the link.
 */
void throwExcInvalidLink (CLID clid, const std::string& key, SG::sgkey_t sgkey)
{
  throw ExcInvalidLink (clid, key, sgkey);
}


//*************************************************************************


/// Helper: format exception error string.
std::string
excBadForwardLink_format (size_t index, size_t size)
{
  std::ostringstream os;
  os << "SG::ExcBadForwardLink: "
     << "ForwardIndexingPolicy: internal link state is invalid";
  if (index != static_cast<size_t>(-1)) {
    os << ": m_index =" << index 
       << " is >=  data container size =" 
       << size << std::ends;
  } 
  return os.str();
}


/**
 * @brief Constructor.
 * @param index Index in the link.
 * @param size Size of the referenced container.
 */
ExcBadForwardLink::ExcBadForwardLink (size_t index, size_t size)
  : std::runtime_error (excBadForwardLink_format (index, size))
{
  AthLinks_error();
}


/**
 * @brief Throw a SG::ExcBadForwardLink exception.
 * @param index Index in the link.
 * @param size Size of the referenced container.
 */
void throwExcBadForwardLink (size_t index, size_t size)
{
  throw ExcBadForwardLink (index, size);
}


//*************************************************************************


/**
 * @brief Constructor.
 * @param where The operation being attempted.
 */
ExcElementNotFound::ExcElementNotFound (const std::string& where)
  : std::runtime_error ("SG::ExcElementNotFound: " + where +
                        ": element not found")
{
  AthLinks_error();
}


/**
 * @brief Throw a SG::ExcElementNotFound exception.
 * @param where The operation being attempted.
 */
void throwExcElementNotFound (const char* where)
{
  throw ExcElementNotFound (where);
}


//*************************************************************************


/**
 * @brief Constructor.
 * @param where The operation being attempted.
 */
ExcInvalidIndex::ExcInvalidIndex (const std::string& where)
  : std::runtime_error ("SG::ExcInvalidIndex: " + where + ": invalid index")
{
  AthLinks_error();
}


/**
 * @brief Throw a SG::ExcInvalidIndex exception.
 * @param where The operation being attempted.
 */
void throwExcInvalidIndex (const char* where)
{
  throw ExcInvalidIndex (where);
}


//*************************************************************************


/**
 * @brief Constructor.
 * @param where The operation being attempted.
 */
ExcIndexNotFound::ExcIndexNotFound (const std::string& where)
  : std::runtime_error ("SG::ExcIndexNotFound: " + where + ": index not found")
{
  AthLinks_error();
}


/**
 * @brief Throw a SG::ExcIndexNotFound exception.
 * @param where The operation being attempted.
 */
void throwExcIndexNotFound (const char* where)
{
  throw ExcIndexNotFound (where);
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcIncomparableEL::ExcIncomparableEL()
  : std::runtime_error ("SG::ExcIncomparableEL: Attempt to compare an ElementLink that does not have a SG key or index.")
{
  AthLinks_error();
}


/**
 * @brief Throw a SG::ExcIncomparableSG exception.
 */
void throwExcIncomparableEL()
{
  throw ExcIncomparableEL();
}


} // namespace SG
