/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AuxStoreAPR.h"
#include "RootAuxVectorFactory.h"
#include "AthContainers/tools/error.h"

#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"

#include "TBranch.h"
#include "TLeaf.h"
#include "TClass.h"
#include "TVirtualCollectionProxy.h"

#include <iostream>

using namespace pool;
using namespace std;


namespace {


const std::type_info* dataTypeToTypeInfo (EDataType type, std::string& typeName)
{
#define TYPE(CODE, TYP) case CODE: typeName=#TYP; return &typeid(TYP)
  switch (type) {
    TYPE(kChar_t,     char);               // 1 
    TYPE(kShort_t,    short);              // 2 
    TYPE(kInt_t,      int);                // 3 
    TYPE(kLong_t,     long);               // 4 
    TYPE(kFloat_t,    float);              // 5 
    TYPE(kCounter,    int);                // 6 
    TYPE(kCharStar,   char*);              // 7 
    TYPE(kDouble_t,   double);             // 8 
    TYPE(kDouble32_t, Double32_t);         // 9 
    TYPE(kchar,       char);               // 10
    TYPE(kUChar_t,    unsigned char);      // 11
    TYPE(kUShort_t,   unsigned short);     // 12
    TYPE(kUInt_t,     unsigned int);       // 13
    TYPE(kULong_t,    unsigned long);      // 14
    TYPE(kBits,       unsigned int);       // 15
    TYPE(kLong64_t,   long long);          // 16
    TYPE(kULong64_t,  unsigned long long); // 17
    TYPE(kBool_t,     bool);               // 18
    TYPE(kFloat16_t,  Float16_t);          // 19
    TYPE(kVoid_t,     void);               // 20
    TYPE(kDataTypeAliasUnsigned_t, unsigned int); // 21
  default: break;
  }
#undef TYPE
  return 0;
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
  if (strncmp (expectedClass->GetName(), "vector<", 7) != 0) return 0;
  TVirtualCollectionProxy* prox = expectedClass->GetCollectionProxy();
  if (!prox) return 0;
  if (prox->GetValueClass()) {
    elementTypeName = prox->GetValueClass()->GetName();
    return prox->GetValueClass()->GetTypeInfo();
  }
  return dataTypeToTypeInfo (prox->GetType(), elementTypeName);
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
          TClass* fac_class = TClass::GetClass (fac_class_name.c_str());
          if (fac_class) {
            TClass* base_class = TClass::GetClass ("SG::IAuxTypeVectorFactory");
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
          TClass* vec_class = TClass::GetClass (branch_type_name.c_str());

          if (vec_class) {
            SG::IAuxTypeVectorFactory* fac = new RootAuxVectorFactory (vec_class);
            r.addFactory (*ti, fac);
            auxid = r.getAuxID(*ti, attr);
          }
        }

      }
      // add AuxID to the list
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

   if( standalone() ) {
      // get TClass for untyped and standalone storage 
      EDataType    typ;
      if( branch->GetExpectedType(cl, typ) ) {
         ATHCONTAINERS_ERROR("AuxStoreAPR::getData", string("Error getting branch type for ") + branch->GetName() );
         return false;
      }
   }

   auto readAuxBranch = [&](void *p) {
      if( m_container.readAuxBranch(*branch, p, m_entry).isSuccess() )
         return true;
      ATHCONTAINERS_ERROR("AuxStoreAPR::getData", string("Error reading branch ") + branch->GetName() );
      return false;
   };

   void *       vector = 0;
   void *       data = &vector;

   // Make a 1-element vector.
   SG::AuxStoreInternal::getDataInternal(auxid, 1, 1, true);
   vector = const_cast<void*>(SG::AuxStoreInternal::getIOData (auxid)); // xxx

   if (standalone() && !cl)
     data = vector;

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


