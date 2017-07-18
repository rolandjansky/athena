/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAcccess/src/AuxStoreARA.cxx
 * @author marcin, scott
 * @date May, 2014
 * @brief Aux store implementation to manage dynamic aux variables.
 *
 * This is basically the same as the corresponding class in RootStorageSvc.
 * Duplicated here due to the lack of any suitable common packages
 * with the correct dependencies.
 */


#include "AuxStoreARA.h"
#include "AthContainersRoot/RootAuxVectorFactory.h"
#include "AthenaROOTAccess/branchSeek.h"
#include "AthContainers/tools/error.h"

#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"

#include "TROOT.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TVirtualCollectionProxy.h"

#include <iostream>

using namespace AthenaROOTAccess;
using namespace std;


namespace {


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


// Convert from class name to TClass; try to avoid autoparsing.
TClass* getClassIfDictionaryExists (const std::string& cname)
{
  if (TClass* cl = (TClass*)gROOT->GetListOfClasses()->FindObject(cname.c_str())) {
    if (cl->IsLoaded() && cl->HasDictionary()) return cl;
    return nullptr;
  }

  if (gInterpreter->GetClassSharedLibs (cname.c_str())) {
    TClass* cl = TClass::GetClass (cname.c_str());
    if (cl->HasDictionary())
      return cl;
  }
  return nullptr;
}


} // Anonymous namespace


AuxStoreARA::AuxStoreARA(IAuxBranches &container, long long entry, bool standalone)
  : SG::AuxStoreInternal (standalone),
    m_entry(entry), m_container(container)
{
   SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
   for( const auto& attr2branch: m_container.auxBranches() ) {
      const string& attr = attr2branch.first;
      std::string elemen_type_name;
      std::string branch_type_name;
      const std::type_info* ti = getElementType(attr2branch.second,
                                                standalone,
                                                elemen_type_name,
                                                branch_type_name);
      SG::auxid_t auxid;
      if (!ti)
        auxid = r.findAuxID(attr);
      else {
        auxid = r.getAuxID(*ti, attr);

        if (auxid == SG::null_auxid) {
          std::string fac_class_name = "SG::AuxTypeVectorFactory<" +
            elemen_type_name;
          if (fac_class_name[fac_class_name.size()-1] == '>')
            fac_class_name += ' ';
          fac_class_name += '>';
          TClass* fac_class = getClassIfDictionaryExists (fac_class_name);
          if (fac_class)
          {
            TClass* base_class = getClassIfDictionaryExists ("SG::IAuxTypeVectorFactory");
            if (base_class) {
              int offs = fac_class->GetBaseClassOffset (base_class);
              if (offs >= 0) {
                void* fac_vp = fac_class->New();
                if (fac_vp) {
                  SG::IAuxTypeVectorFactory* fac = reinterpret_cast<SG::IAuxTypeVectorFactory*> (reinterpret_cast<unsigned long>(fac_vp) + offs);
                  r.addFactory (*ti, fac);
                  auxid = r.getAuxID(*ti, attr);
                }
              }
            }
          }
        }

        if (auxid == SG::null_auxid) {
          std::string vec_name = branch_type_name;
          if (standalone) {
            vec_name = "std::vector<" + branch_type_name;
            if (vec_name[vec_name.size()-1] == '>')
              vec_name += " ";
            vec_name += ">";
          }
          TClass* vec_class = TClass::GetClass (branch_type_name.c_str());

          if (vec_class) {
            SG::IAuxTypeVectorFactory* fac = new SG::RootAuxVectorFactory (vec_class);
            r.addFactory (*ti, fac);
            auxid = r.getAuxID(*ti, attr);
          }
        }

      }
      // add AuxID to the list
      // May still be null if we don't have a dictionary for the branch.
      if (auxid != SG::null_auxid)
        addAuxID (auxid);
   }

   //lock();
}


AuxStoreARA::~AuxStoreARA()
{
  for (TBranch* br : m_branches)
    br->SetAddress(nullptr);
}


void AuxStoreARA::GetEntry (long long entry)
{
  m_entry = entry;
  for (TBranch* br : m_branches)
    branchSeek (br, entry);
}


const void* AuxStoreARA::getData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  // lock
  const void* ret = SG::AuxStoreInternal::getData (auxid);
  if (!ret) {
    const_cast<AuxStoreARA*>(this)->readData(auxid);
    ret = SG::AuxStoreInternal::getData (auxid);
  }
  return ret;
}


const void* AuxStoreARA::getIOData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  const void* ret = SG::AuxStoreInternal::getIODataInternal (auxid, true);
  if (!ret) {
    const_cast<AuxStoreARA*>(this)->readData(auxid);
    ret = SG::AuxStoreInternal::getIOData (auxid);
  }
  return ret;
}


bool AuxStoreARA::readData(SG::auxid_t auxid)
{
   //cout << "AuxStoreAPR::getData() id=" << auxid << ",  name="
   //     << reg.getName(auxid) << ", container=" << m_container.m_branchName << endl;

   TBranch* branch = m_container.findAuxBranch(auxid);
   // check if there is a branch in the file
   if( !branch ) return false; 
   // cout << " Branch: name=" << branch->GetName() << ",  class name=" << branch->GetClassName() << endl;

   TClass*      cl = 0;

   if( standalone() ) {
      // get TClass for untyped and standalone storage 
      EDataType    typ;
      if( branch->GetExpectedType(cl, typ) ) {
         ATHCONTAINERS_ERROR("AuxStoreAPR::getData", string("Error getting branch type for ") + branch->GetName() );
         return false;
      }
   }

   auto readAuxBranch = [&](void *p) {
     if( m_container.readAuxBranch(*branch, p, m_entry).isSuccess() ) {
       m_branches.push_back (branch);
       return true;
     }
     ATHCONTAINERS_ERROR("AuxStoreAPR::getData", string("Error reading branch ") + branch->GetName() );
     return false;
   };

   void *       vector = 0;

   // Make a 1-element vector.
   SG::AuxStoreInternal::getDataInternal(auxid, 1, 1, true);
   if (!standalone()) {
     EDataType typ;
     branch->GetExpectedType(cl, typ);
     if (cl && strncmp (cl->GetName(), "SG::PackedContainer<", 20) == 0)
       setOption (auxid, SG::AuxDataOption ("nbits", 32));
   }
   vector = const_cast<void*>(SG::AuxStoreInternal::getIOData (auxid)); // xxx

   void* data;

   if (standalone() && !cl)
     data = vector;
   else {
     m_ptrs.push_back (vector);
     data = &m_ptrs.back();
   }

   // read branch
   return readAuxBranch(data);
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
AuxStoreARA::getDecoration (SG::auxid_t auxid, size_t size, size_t capacity)
{
  guard_t guard (m_mutex);
  if (SG::AuxStoreInternal::getIODataInternal (auxid, true) == 0 &&
      SG::AuxStoreInternal::getAuxIDs().count (auxid) > 0)
  {
    throw SG::ExcStoreLocked (auxid);
  }
  return SG::AuxStoreInternal::getDecoration (auxid, size, capacity);
}


