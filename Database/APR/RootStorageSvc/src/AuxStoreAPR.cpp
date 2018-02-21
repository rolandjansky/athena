/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AuxStoreAPR.h"
#include "AthContainersRoot/RootAuxVectorFactory.h"
#include "AthContainers/tools/error.h"

#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "RootUtils/Type.h"

#include "TBranch.h"
#include "TLeaf.h"
#include "TClass.h"
#include "TClassTable.h"
#include "TClassEdit.h"
#include "TVirtualCollectionProxy.h"
#include "TROOT.h"

#include <iostream>

using namespace pool;
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
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
  return TClass::GetClass (cname.c_str());
#else
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
#endif
}


} // Anonymous namespace


AuxStoreAPR::AuxStoreAPR(RootTreeContainer &container, long long entry, bool standalone)
  : SG::AuxStoreInternal (standalone),
    m_entry(entry), m_container(container)
{
   SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
   for( const auto& attr2branch: m_container.m_auxBranches ) {
      const string& attr = attr2branch.first;
      std::string elemen_type_name;
      std::string branch_type_name;
      const std::type_info* ti = getElementType(attr2branch.second,
                                                standalone,
                                                elemen_type_name,
                                                branch_type_name);
      SG::auxid_t auxid = r.findAuxID(attr);
      if( ti ) {
         if (auxid == SG::null_auxid) {
            auxid = r.getAuxID(*ti, attr);
         } else {
            const std::type_info* reg_ti = r.getType(auxid);
            if( ti != reg_ti && strcmp(ti->name(), reg_ti->name()) != 0 ) {
               DbPrint log(container.getName());
               log << DbPrintLvl::Debug << "reading attribute " << attr << " (id=" << auxid <<
                  " typename=" << reg_ti->name() << ") has different type than the branch "
                    << branch_type_name << DbPrint::endmsg;
               m_container.findAuxBranch(auxid);  // call this to init_auxBranchMap[auxid]
               m_container.m_auxBranchMap[auxid].aux_type_different = true;
            }
         }

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
          TClass* vec_class = TClass::GetClass (vec_name.c_str());

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

   lock();
}


const void* AuxStoreAPR::getData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  // lock
  const void* ret = SG::AuxStoreInternal::getData (auxid);
  if (!ret) {
    const_cast<AuxStoreAPR*>(this)->readData(auxid);
    ret = SG::AuxStoreInternal::getData (auxid);
  }
  return ret;
}


const void* AuxStoreAPR::getIOData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  const void* ret = SG::AuxStoreInternal::getIODataInternal (auxid, true);
  if (!ret) {
    const_cast<AuxStoreAPR*>(this)->readData(auxid);
    ret = SG::AuxStoreInternal::getIOData (auxid);
  }
  return ret;
}


bool AuxStoreAPR::readData(SG::auxid_t auxid)
{
   //cout << "AuxStoreAPR::getData() id=" << auxid << ",  name="
   //     << reg.getName(auxid) << ", container=" << m_container.m_branchName << endl;

   TBranch* branch = m_container.findAuxBranch(auxid);
   // check if there is a branch in the file
   if( !branch ) return false; 
   // cout << " Branch: name=" << branch->GetName() << ",  class name=" << branch->GetClassName() << endl;

   TClass*      cl = 0;
   EDataType    typ;
   if( branch->GetExpectedType(cl, typ) ) {
      ATHCONTAINERS_ERROR("AuxStoreAPR::getData", string("Error getting branch type for ") + branch->GetName() );
      return false;
   }

   // Make a 1-element vector.
   SG::AuxStoreInternal::getDataInternal(auxid, 1, 1, true);
   if (!standalone()) {
     if (cl && strncmp (cl->GetName(), "SG::PackedContainer<", 20) == 0)
       setOption (auxid, SG::AuxDataOption ("nbits", 32));
   }

   // get memory location where to write data from the branch entry
   void *       vector = const_cast<void*>(SG::AuxStoreInternal::getIOData (auxid)); // xxx
   void *       data = &vector;
   if (standalone() && !cl) {
      // raeding fundamental type, ROOT does it a bit differently
     data = vector;
   }
   
   // read branch
   if( m_container.m_auxBranchMap[auxid].aux_type_different ) {
      // reading through the TTree - allows for schema evolution
      if( m_container.readAuxBranch(*branch, data, getIOType(auxid), m_entry).isSuccess() )
         return true;
   } else {
      if( m_container.readAuxBranch(*branch, data, m_entry).isSuccess() )
         return true;
   }

   ATHCONTAINERS_ERROR("AuxStoreAPR::getData", string("Error reading branch ") + branch->GetName() );
   return false;
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
AuxStoreAPR::getDecoration (SG::auxid_t auxid, size_t size, size_t capacity)
{
  guard_t guard (m_mutex);
  if (SG::AuxStoreInternal::getIODataInternal (auxid, true) == 0 &&
      SG::AuxStoreInternal::getAuxIDs().count (auxid) > 0)
  {
    throw SG::ExcStoreLocked (auxid);
  }
  return SG::AuxStoreInternal::getDecoration (auxid, size, capacity);
}


