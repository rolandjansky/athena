/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Exceptions that can be thrown from AthContainers.
 */


#include "AthContainers/exceptions.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/error.h"
#include <sstream>


namespace SG {


/// Helper: format an aux data item name.
std::string excFormatName (SG::auxid_t auxid)
{
  std::ostringstream os;
  if (auxid != null_auxid) {
    SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
    os << "`" << r.getClassName (auxid);
    if (!os.str().empty())
      os << "::";
    os << r.getName (auxid);
    os << "' (" << auxid << ")";
  }
  return os.str();
}
  

//*************************************************************************


/// Helper: format exception error string.
std::string excNoAuxStore_format (SG::auxid_t auxid)
{
  std::ostringstream os;
  os << "SG::ExcNoAuxStore: "
     << "Requested aux data item "
     << excFormatName (auxid)
     << " but there is no associated aux data store.";
  return os.str();
}


/// Helper: format exception error string.
std::string excNoAuxStore_format (const char* op)
{
  std::ostringstream os;
  os << "SG::ExcNoAuxStore: "
     << "Operation attempted on container with no associated aux data store: ";
  os << op;
  return os.str();
}


/**
 * @brief Constructor.
 * @param auxid The ID of the requested aux data item.
 */
ExcNoAuxStore::ExcNoAuxStore (SG::auxid_t auxid)
  : std::runtime_error (excNoAuxStore_format (auxid))
{
}


/**
 * @brief Constructor.
 * @param op The attempted operation.
 */
ExcNoAuxStore::ExcNoAuxStore (const char* op)
  : std::runtime_error (excNoAuxStore_format (op))
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excBadAuxVar_format (SG::auxid_t auxid)
{
  std::ostringstream os;
  os << "SG::ExcBadAuxVar: "
     << "Attempt to retrieve nonexistent aux data item "
     << excFormatName (auxid)
     << ".";
  return os.str();
}


/**
 * @brief Constructor.
 * @param auxid The ID of the requested aux data item.
 */
ExcBadAuxVar::ExcBadAuxVar (SG::auxid_t auxid)
  : std::runtime_error (excBadAuxVar_format (auxid))
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excConstAuxData_format (const std::string& op, SG::auxid_t auxid)
{
  std::ostringstream os;
  os << "SG::ExcConstAuxData: "
     << "Non-const operation `" << op << "' performed on const aux data "
     << excFormatName (auxid)
     << ".";
  return os.str();
}


/**
 * @brief Constructor.
 * @param op The attempted operation.
 * @param auxid The ID of the requested aux data item, if any.
 */
ExcConstAuxData::ExcConstAuxData (const std::string& op, SG::auxid_t auxid)
  : std::runtime_error (excConstAuxData_format (op, auxid))
{
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcUntrackedSetStore::ExcUntrackedSetStore()
  : std::runtime_error
    ("SG::ExcUntrackedSetStore: "
     "Attempt to set aux data store on container that doesn't track indices, "
     "or disable index tracking for a container with aux data.")
{
}


//*************************************************************************


/**
 * @brief Constructor.
 * @param op The attempted operation.
 */
ExcBadPrivateStore::ExcBadPrivateStore (const std::string& op)
  : std::runtime_error ("SG::ExcBadPrivateStore: "
                        "Bad use of private store: " + op)
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excAuxTypeMismatch_format (SG::auxid_t auxid,
                                       const std::type_info& new_type,
                                       const std::type_info& old_type)
{
  std::ostringstream os;
  os << "SG::ExcAuxTypeMismatch: "
     << "Type mismatch for aux variable "
     << excFormatName (auxid)
     << "; old type is " << AthContainers_detail::typeinfoName (old_type)
     << " new type is "  << AthContainers_detail::typeinfoName (new_type);
  return os.str();
}


/**
 * @brief Constructor.
 * @param auxid ID of the requested aux data item.
 * @param new_type New type requested for the item.
 * @param old_type Previous type associated with the item.
 */
ExcAuxTypeMismatch::ExcAuxTypeMismatch (SG::auxid_t auxid,
                                        const std::type_info& new_type,
                                        const std::type_info& old_type)
  : std::runtime_error (excAuxTypeMismatch_format (auxid,
                                                   new_type, old_type))
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excInsertionInBaseClass_format (const char* op,
                                            const std::type_info& base_type,
                                            const std::type_info& complete_type)
{
  std::ostringstream os;
  os << "SG::ExcInsertionInBaseClass: "
     << "Attempted to do " << op
     << " on a " << AthContainers_detail::typeinfoName (base_type)
     << " base class of " << AthContainers_detail::typeinfoName (complete_type)
     << "; can only be done on the most-derived class.";
  return os.str();
}


/**
 * @brief Constructor.
 * @param op The attempted operation.
 * @param base_type The type on which the operation was attempted.
 * @param complete_type The most-derived type of the object.
 */
ExcInsertionInBaseClass::ExcInsertionInBaseClass
   (const char* op,
    const std::type_info& base_type,
    const std::type_info& complete_type)
     : std::runtime_error (excInsertionInBaseClass_format (op,
                                                           base_type,
                                                           complete_type))
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excStoreLocked_format (SG::auxid_t auxid)
{
  std::ostringstream os;
  os << "SG::ExcStoreLocked: "
     << "Attempted to modify auxiliary data in a locked store: ";
  os << excFormatName (auxid);
  return os.str();
}


/// Helper: format exception error string.
std::string excStoreLocked_format (const char* op)
{
  std::ostringstream os;
  os << "SG::ExcStoreLocked: "
     << "Attempted to modify auxiliary data in a locked store: ";
  os << op;
  return os.str();
}


/**
 * @brief Constructor.
 * @param op The attempted operation.
 */
ExcStoreLocked::ExcStoreLocked (SG::auxid_t auxid)
  : std::runtime_error (excStoreLocked_format (auxid))
{
}


/**
 * @brief Constructor.
 * @param op The attempted operation.
 */
ExcStoreLocked::ExcStoreLocked (const char* op)
  : std::runtime_error (excStoreLocked_format (op))
{
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcNonowningContainer::ExcNonowningContainer()
  : std::runtime_error ("SG::ExcNonowningContainer: Attempted to insert a unique_ptr to a non-owning container.")
{
}


/**
 * @brief Throw a SG::ExcNonowningContainer exception.
 */
void throwExcNonowningContainer()
{
  throw ExcNonowningContainer();
}


//*************************************************************************


/// Helper: format exception error string.
std::string excUnknownAuxItem_format (const std::string& name,
                                      const std::string& clsname,
                                      const std::type_info* typ)
{
  std::ostringstream os;
  os << "SG::ExcUnknownAuxItem: "
     << "Unknown aux data item ";
  if (!clsname.empty())
    os << clsname << "::";
  os << name;
  if (typ)
    os << " (of type " << AthContainers_detail::typeinfoName (*typ) << ")";
  return os.str();
}


/**
 * @brief Constructor.
 * @param name Name of the aux data item.
 * @param clsname Class name of the aux data item, or an empty string.
 * @param typ Type of the item, if provided.
 */
ExcUnknownAuxItem::ExcUnknownAuxItem (const std::string& name,
                                      const std::string& clsname /*= ""*/,
                                      const std::type_info* typ /*= 0*/)
  : std::runtime_error (excUnknownAuxItem_format (name, clsname, typ))
{
}


/**
 * @brief Throw a SG::ExcUnknownAuxItem exception.
 * @param name Name of the aux data item.
 * @param clsname Class name of the aux data item, or an empty string.
 * @param typ Type of the item, if provided.
 */
void throwExcUnknownAuxItem (const std::string& name,
                             const std::string& clsname /*= ""*/,
                             const std::type_info* typ /*= 0*/)
{
  throw ExcUnknownAuxItem (name, clsname, typ);
}


//*************************************************************************


/// Helper: format exception error string.
std::string excDVToELV_format (const std::string& detail)
{
  std::ostringstream os;
  os << "SG::ExcDVToELV: "
     << "Can't convert DataVector to vector of ElementLinks: "
     << detail;
  return os.str();
}


/**
 * @brief Constructor.
 * @param detail More information about the error.
 */
ExcDVToELV::ExcDVToELV (const std::string& detail)
  : std::runtime_error (excDVToELV_format (detail))
{
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcViewVectorNotView::ExcViewVectorNotView()
  : std::runtime_error ("ViewVector not in view mode.")
{
}


/**
 * @brief Throw a SG::ExcViewVectorNotView exception.
 */
void throwExcViewVectorNotView()
{
  throw ExcViewVectorNotView();
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcMissingViewVectorCLID::ExcMissingViewVectorCLID (const std::type_info& ti)
  : std::runtime_error ("ViewVector " + AthContainers_detail::typeinfoName(ti) +
                        "was used in a context that requires a CLID, "
                        "but no CLID was available.  Make sure a "
                        "VIEWVECTOR_CLASS_DEF declaration exists for the class "
                        "in a library that has been loaded.")
{
}


/**
 * @brief Throw a SG::ExcViewVectorCLID exception.
 */
void throwExcMissingViewVectorCLID (const std::type_info& ti)
{
  throw ExcMissingViewVectorCLID (ti);
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcMissingBaseInfo::ExcMissingBaseInfo (const std::type_info& ti)
  : std::runtime_error ("Missing BaseInfo for " +
                        AthContainers_detail::typeinfoName(ti))
{
}


/**
 * @brief Throw a SG::ExcMissingBaseInfo exception.
 */
void throwExcMissingBaseInfo (const std::type_info& ti)
{
  throw ExcMissingBaseInfo (ti);
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcInsertMoveOwnershipMismatch::ExcInsertMoveOwnershipMismatch()
  : std::runtime_error ("Ownership mismatch for insertMove.")
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excAtomicMismatch_format (SG::auxid_t auxid,
                                      const std::type_info& type)
{
  std::ostringstream os;
  os << "SG::ExcAtomicMismatch: "
     << "Non-atomic access to atomic aux variable  "
     << excFormatName (auxid)
     << " of type " << AthContainers_detail::typeinfoName (type);
  return os.str();
}


/**
 * @brief Constructor.
 * @param auxid ID of the requested aux data item.
 * @param new_type Type of the item.
 */
ExcAtomicMismatch::ExcAtomicMismatch (SG::auxid_t auxid,
                                      const std::type_info& type)
  : std::runtime_error (excAtomicMismatch_format (auxid, type))
{
}


//*************************************************************************


/// Helper: format exception error string.
std::string excInvalidThinningTarget_format (unsigned int clid,
                                             const std::string& key)
{
  std::ostringstream os;
  os << "SG::ExcInvalidThinningTarget: "
     << "ThinningHandle target does not exist: "
     << clid << "/" << key;
  return os.str();
}


/**
 * @brief Constructor.
 * @param clid The CLID of the requested container.
 * @param key The StoreGate key of the requested container.
 */
ExcInvalidThinningTarget::ExcInvalidThinningTarget (unsigned int clid,
                                                    const std::string& key)
  : std::runtime_error (excInvalidThinningTarget_format (clid, key))
{
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcBadIterSwap::ExcBadIterSwap()
  : std::runtime_error ("SG::ExcBadIterSwap: Attempt to swap between containers with inconsistent ownership.")
{
}


//*************************************************************************


/**
 * @brief Constructor.
 */
ExcAllocOwnership::ExcAllocOwnership()
  : std::runtime_error ("SG::ExcAllocOwnership: Bad allocation ownership.")
{
}


/**
 * @brief Throw a SG::ExcAllocOwnership exception.
 */
void throwExcAllocOwnership()
{
  throw ExcAllocOwnership();
}


} // namespace SG
