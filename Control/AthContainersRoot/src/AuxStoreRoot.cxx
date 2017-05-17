/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/src/AuxStoreRoot.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Base class for root-based dynamic auxiliary store.
 */


#include "AthContainersRoot/AuxStoreRoot.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/error.h"
#include "RootUtils/Type.h"
#include "TROOT.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TVirtualCollectionProxy.h"


namespace {


/**
 * @brief Decode a ROOT @c EDataType.
 * @param type ROOT type to decode.
 * @param[out] typeName Name of the type.
 *
 * Returns the @c std::type_info for the type.
 */
const std::type_info* dataTypeToTypeInfo (EDataType type, std::string& typeName)
{
  RootUtils::Type typ (type);
  typeName = typ.getTypeName();
  return typ.getTypeInfo();
}


/**
 * @brief Given a branch, return the aux data type for that branch.
 * @param br The branch for which we want to get the type.
 * @param standalone Is this for a standalone object?
 * @param[out] elementTypeName Name of the type for one aux data element.
 *                             Same as branchTypeName if @c standalone is true.
 * @param[out] branchTypeName Name of the type for this branch.
 *
 * If standalone is true, then we return the type of the branch
 * directly.  Otherwise, the branch should be a STL vector;
 * we return the type of the vector's payload.
 *
 * Returns 0 on failure.
 */
const std::type_info* getElementType (TBranch* br,
                                      bool standalone,
                                      std::string& elementTypeName,
                                      std::string& branchTypeName)
{
  TClass* expectedClass = 0;
  EDataType expectedType = kOther_t;
  if (br->GetExpectedType (expectedClass, expectedType) != 0) {
    return 0;
  }

  if (standalone) {
    if (expectedClass) {
      elementTypeName = expectedClass->GetName();
      branchTypeName = elementTypeName;
      return expectedClass->GetTypeInfo();
    }
    const std::type_info* ret = dataTypeToTypeInfo (expectedType,
                                                    elementTypeName);
    branchTypeName = elementTypeName;
    return ret;
  }

  // Not standalone; branch should be a vector.
  if (!expectedClass) return 0;

  branchTypeName = expectedClass->GetName();
  if (strncmp (expectedClass->GetName(), "vector<", 7) == 0) {
    TVirtualCollectionProxy* prox = expectedClass->GetCollectionProxy();
    if (!prox) return 0;
    if (prox->GetValueClass()) {
      elementTypeName = prox->GetValueClass()->GetName();
      return prox->GetValueClass()->GetTypeInfo();
    }
    return dataTypeToTypeInfo (prox->GetType(), elementTypeName);
  }
  else if (strncmp (expectedClass->GetName(), "SG::PackedContainer<", 20) == 0){
    TClassEdit::TSplitType split (expectedClass->GetName());
    if (split.fElements.size() > 1) {
      elementTypeName = split.fElements[1];
      RootUtils::Type typ (elementTypeName);
      return typ.getTypeInfo();
    }
  }
  return 0;
}


} // Anonymous namespace


namespace SG {


/**
 * @brief Constructor.
 * @param container Representation of the object for which we're reading.
 * @param entry The entry in the root tree to read.
 * @param standalone If true, this is a standalone auxiliary object.
 */
AuxStoreRoot::AuxStoreRoot(IAuxBranches& container,
                           long long entry,
                           bool standalone) :
  SG::AuxStoreInternal (standalone),
  m_entry(entry),
  m_container(container)
{
}


/**
 * @brief Return the data vector for one aux data item
 * @param auxid The identifier of the desired aux data item.
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * This should return 0 if the item doesn't exist.
 */
const void* AuxStoreRoot::getData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  // lock
  const void* ret = SG::AuxStoreInternal::getData (auxid);
  if (!ret) {
    if (const_cast<AuxStoreRoot*>(this)->readData(auxid))
      ret = SG::AuxStoreInternal::getData (auxid);
  }
  return ret;
}


/**
 * @brief Return the data vector for one aux data item
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * If the data item does not exist, it should be created and initialized
 * to default values.  @c size and @c capacity give the size for the
 * new aux data item vector.
 *
 * If the container is locked, throw an exception.
 */
void* AuxStoreRoot::getData(SG::auxid_t auxid,
                            size_t /*size*/, size_t /*capacity*/)
{
  // MN:  how do we add new attributes to this store? A:for now we don't
  return const_cast<void*>(getData(auxid));
}




/**
 * @brief Return the data vector for one aux data decoration item.
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * The base class implementation works except for the case where we have
 * not yet created a vector for an item in the root file.  We need to 
 * detect that case and raise an exception.
 */
void*
AuxStoreRoot::getDecoration (SG::auxid_t auxid, size_t size, size_t capacity)
{
  guard_t guard (m_mutex);
  if (SG::AuxStoreInternal::getIODataInternal (auxid, true) == 0 &&
      SG::AuxStoreInternal::getAuxIDs().count (auxid) > 0)
  {
    throw SG::ExcStoreLocked (auxid);
  }
  return SG::AuxStoreInternal::getDecoration (auxid, size, capacity);
}


/**
 * @brief Return a pointer to the data to be stored for one aux data item.
 * @param auxid The identifier of the desired aux data item.
 *
 * This will usually be a pointer to a @c std::vector; however, it may
 * be something different for a standalone object.
 *
 * Returns 0 and reports an error if the requested aux data item
 * does not exist.
 */
const void* AuxStoreRoot::getIOData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  const void* ret = SG::AuxStoreInternal::getIODataInternal (auxid, true);
  if (!ret) {
    if (const_cast<AuxStoreRoot*>(this)->readData(auxid))
      ret = SG::AuxStoreInternal::getIOData (auxid);
  }
  return ret;
}


/**
 * @brief Derived classes should call this from their constructor
 *        to complete initialization.
 *
 * Creates the auxid set from the list of branches.
 * This cannot be done from the @c AuxStoreRoot constructor as it needs
 * to call the @c resolveAuxID virtual method.
 */  
void AuxStoreRoot::fillAuxIDs()
{
   for( const auto& attr2branch: m_container.auxBranches() ) {
     const std::string& attr = attr2branch.first;
     std::string elemen_type_name;
     std::string branch_type_name;
     const std::type_info* ti = getElementType(attr2branch.second,
                                               standalone(),
                                               elemen_type_name,
                                               branch_type_name);
     SG::auxid_t auxid = resolveAuxID (m_container,
                                       ti, attr,
                                       elemen_type_name,
                                       branch_type_name);

     // add AuxID to the list
     // May still be null if we don't have a dictionary for the branch.
     if (auxid != SG::null_auxid)
       addAuxID (auxid);
   }

   //lock();
}


/**
 * @brief Change the TTree entry being read.
 * @param entry New entry being read.
 */
void AuxStoreRoot::setEntry (long long entry)
{
  m_entry = entry;
}


/**
 * @brief Read data from ROOT and store it in the internal vector.
 *
 * Returns true on success, false on failure.
 */
bool AuxStoreRoot::readData(SG::auxid_t auxid)
{
   TBranch* branch = m_container.findAuxBranch(auxid);
   // check if there is a branch in the file
   if( !branch ) return false; 

   TClass*      cl = 0;
   EDataType    typ;
   if( branch->GetExpectedType(cl, typ) ) {
     ATHCONTAINERS_ERROR("AuxStoreRoot::readData",
                         std::string("Error getting branch type for ") + branch->GetName() );
     return false;
   }

   // Make a 1-element vector.
   SG::AuxStoreInternal::getDataInternal(auxid, 1, 1, true);
   if (!standalone()) {
     if (cl && strncmp (cl->GetName(), "SG::PackedContainer<", 20) == 0)
       setOption (auxid, SG::AuxDataOption ("nbits", 32));
   }

   void* vector = const_cast<void*>(SG::AuxStoreInternal::getIOData (auxid)); // xxx

   return doReadData (m_container, auxid, *branch, cl, vector, m_entry);
}


} // namespace SG
