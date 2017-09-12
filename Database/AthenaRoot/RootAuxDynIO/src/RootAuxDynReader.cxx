/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/error.h"
#include "AthContainers/exceptions.h"
#include "RootUtils/Type.h"

#include "RootAuxDynReader.h"
#include "RootAuxDynStore.h"
#include "AthContainersRoot/getDynamicAuxID.h"

#include "TBranch.h"
#include "TClass.h"
#include "TClassTable.h"
#include "TClassEdit.h"
#include "TVirtualCollectionProxy.h"
#include "TROOT.h"
#include "TDictAttributeMap.h"

#include <iostream>
using namespace std;


namespace {


const std::type_info* dataTypeToTypeInfo (EDataType type, std::string& typeName)
{
   RootUtils::Type typ (type);
   typeName = typ.getTypeName();
   return typ.getTypeInfo();
}


/**
 * @brief Given TClass+EDataType (as returned by a branch), return the aux data type
 * @param expectedClass  TClass for the storage type if a class
 * @param expectedType   EDT type for the storage type if a primitive type
 * @param standalone     Is this for a standalone object?
 * @param[out] elementTypeName Name of the type for one aux data element.
 *                             Same as storageTypeName if @c standalone is true.
 * @param[out] storageTypeName Name of the type used for I/O
 *
 * If standalone is true, then we return the extracted type
 * directly.  Otherwise, the type should be an STL vector;
 * we return the type of the vector's payload.
 *
 * Returns 0 on failure.
 */
const std::type_info*
getAuxElementType( TClass *expectedClass, EDataType expectedType, bool standalone,
                   std::string& elementTypeName, std::string& storageTypeName)
{
   if (standalone) {
      if(expectedClass) {
         elementTypeName = expectedClass->GetName();
         storageTypeName = elementTypeName;
         return expectedClass->GetTypeInfo();
      }
      const std::type_info* ret = dataTypeToTypeInfo(expectedType, elementTypeName);
      storageTypeName = elementTypeName;
      return ret;
   }

   // Not standalone; branch should be a vector.
   if (!expectedClass) return 0;

   storageTypeName = expectedClass->GetName();
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



SG::auxid_t
getAuxIdForAttribute(const std::string& attr, TClass *tclass, EDataType edt, bool standalone)
{
   SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

   SG::auxid_t auxid = r.findAuxID(attr);
   if(auxid != SG::null_auxid)
      return auxid;
   
   string elemen_type_name;
   string branch_type_name;
   const type_info* ti = getAuxElementType(tclass, edt, standalone, elemen_type_name, branch_type_name);
   if( !ti )
      return auxid;

   return SG::getDynamicAuxID (*ti, attr, elemen_type_name, branch_type_name, standalone);
}


std::string getKeyFromBranch (TBranch* branch)
{
  TClass *tc = 0;
  EDataType type;
  if( branch->GetExpectedType(tc, type) == 0  && tc != nullptr) {
    const char* brname = branch->GetName();
    const char* clname = tc->GetName();
    size_t namelen = strlen (clname);
    std::string key = brname;
    if (strncmp (brname, clname, namelen) == 0 &&
        brname[namelen] == '_')
    {
      key.erase (0, namelen+1);
    }

    if (key.size() >= 5 && key.substr(key.size()-4, 4) == RootAuxDynIO::AUX_POSTFIX )
      key = key.substr (0, key.size()-4);
    return key;
  }
  return "";
}



} // anonymous namespace



namespace RootAuxDynIO
{

   std::string
   auxBranchName(const std::string& attr_name, const std::string& baseBranchName)
   {
      string branch_name = baseBranchName;
      if( branch_name.back() == '.' )  branch_name.pop_back();
      branch_name += RootAuxDynIO::AUXDYN_POSTFIX + attr_name;
      return branch_name;
   }


   bool
   isAuxDynBranch(TBranch *branch)
   {
      const string bname = branch->GetName();
      if( bname.size() >= 5 && bname.substr(bname.size()-4, 4) == RootAuxDynIO::AUX_POSTFIX )
         return true;

      TClass *tc = 0;
      EDataType type;
      if( branch->GetExpectedType(tc, type) ) {
         // error - not expecting this to happen ever, but better report
         errorcheck::ReportMessage msg (MSG::WARNING, ERRORCHECK_ARGS, "RootAuxDynIO::isAuxDynBranch");
         msg << "GetExpectedType() failed for branch: " << branch->GetName();
         return false;
      }
      if( tc && tc->GetAttributeMap() && tc->GetAttributeMap()->HasKey("IAuxStore") )
         return true;

      return false;
   }

   
   IRootAuxDynReader*
   getReaderForBranch(TBranch *branch)
   {
      if( isAuxDynBranch(branch) ) {
         TClass *tc = 0;
         EDataType type;
         if( branch->GetExpectedType(tc, type) ) {
            return nullptr;
         }
         // cout << "---  getReaderForClass: " << tc->GetName() << "  branch " << branch->GetName() << endl;

         TClass *storeTC = tc->GetBaseClass("SG::IAuxStoreHolder");
         if( storeTC ) {  
            int store_holder_offset = tc->GetBaseClassOffset( storeTC );
            return new RootAuxDynReader(branch, store_holder_offset);
         }
      }
      return nullptr;
   }
              
}




void RootAuxDynReader::BranchInfo::setAddress(void* data) const
{
   if( needsSE ) {
      // reading through the TTree - allows for schema evolution
      if( branch->GetTree()->SetBranchAddress( branch->GetName(), data,
                                               SE_tclass, SE_edt, true) < 0 ) {
         throw string("SetBranchAddress() failed for ") + branch->GetName();
      }
   } else {
      branch->SetAddress(data);
   }
}
    

// Fix Reader for a specific tree and branch base name.
// Find all dynamic attribute branches that share the base name
RootAuxDynReader::RootAuxDynReader(TBranch *branch, int store_holder_offset)
      : m_tree( branch->GetTree() ),
        m_baseBranchName( branch->GetName() ),
        m_storeHolderOffset( store_holder_offset ),
        m_key (getKeyFromBranch (branch))
{
   const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
   string branch_prefix = RootAuxDynIO::auxBranchName("", m_baseBranchName);
   // cout << "RootAuxDynReader: scanning for branches with prefix: " << branch_prefix << endl;
   TObjArray *all_branches = m_tree->GetListOfBranches();
   for( int i=0; i<all_branches->GetEntriesFast(); i++ ) {
      const char *bname =  (*all_branches)[i]->GetName();
      if( strncmp(bname, branch_prefix.c_str(), branch_prefix.size()) == 0 ) {
         const string  attr_inFile  = bname+branch_prefix.size();
         const string attr = r.inputRename (m_key, attr_inFile);
         m_branchMap[attr] = (TBranch*)(*all_branches)[i];
         // m_attrNames.insert(attr); // may be useful
         // cout <<  "  >>>  Branch " << bname << ", attr=" << attr << endl;
      }
   }
}


// Has to be a separate method because 'standalone' status is not know at construction time
// Prepare all branch infos for dynamic attributes (auxids and types)
void RootAuxDynReader::init(bool standalone)
{
   if( m_initialized )
      return;
   for( const auto& attr2branch: m_branchMap ) {
      const string& attr = attr2branch.first;
      TBranch*      branch  = attr2branch.second;
      TClass*       expectedClass = 0;
      EDataType     expectedType = kOther_t;
      if( branch->GetExpectedType(expectedClass, expectedType) != 0) {
         // raise hell
      }
      SG::auxid_t auxid = getAuxIdForAttribute(attr, expectedClass, expectedType, standalone);
      // add AuxID to the list
      // May still be null if we don't have a dictionary for the branch.
      if (auxid != SG::null_auxid) {
         m_auxids.insert(auxid);
      } else {
         errorcheck::ReportMessage msg (MSG::WARNING, ERRORCHECK_ARGS, "RootAuxDynReader::init");
         msg << "Could not find auxid for " << branch->GetName()
              << " type: " << expectedClass->GetName();

      }
      m_initialized = true;
   }
}
  

// Called by the AuxStore when it is reading new attribute data from the file
// All information is cached in a BranchInfo object for better performance
const RootAuxDynReader::BranchInfo&
RootAuxDynReader::getBranchInfo(const SG::auxid_t& auxid, const SG::AuxStoreInternal& store)
{
   BranchInfo& brInfo = m_branchInfos[auxid];
   if( brInfo.status == BranchInfo::NotInitialized )
   {
      SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
      brInfo.auxid = auxid;
      brInfo.attribName = r.getName(auxid);
      
      // Don't match this attribute if it's been renamed.
      // For example, suppose we've renamed attribute `foo' to `foo_old',
      // and someone then comes and asks for `foo'.
      // `foo' will not be found in the m_branchMap test below
      // (`foo_old' will be in this map).  However, in the following
      // else clause, we'll recreate the branch name from `foo'.
      // This branch exists (renaming is only in the transient store),
      // so if we didn't have the condition here, then we'd then
      // make a `foo' attribute from that branch.
      if (r.inputRename (m_key, brInfo.attribName) != brInfo.attribName) {
        brInfo.status = BranchInfo::NotFound;
        return brInfo;
      }

      auto it = m_branchMap.find (brInfo.attribName);
      if (it != m_branchMap.end()) {
        brInfo.branch = it->second;
      }
      else {
        const string aux_branch_name = RootAuxDynIO::auxBranchName(brInfo.attribName, m_baseBranchName);
        brInfo.branch = m_tree->GetBranch( aux_branch_name.c_str() );
      }
      
      // mark initialized here so it remembers this branch was not found
      if( !brInfo.branch ) {
         brInfo.status = BranchInfo::NotFound;
         return brInfo;
      }
      EDataType    typ;
      if( brInfo.branch->GetExpectedType( brInfo.tclass, typ) ) {
         brInfo.status = BranchInfo::TypeError;
         throw string("Error getting branch type for ") + brInfo.branch->GetName();
      }
   
      if( !store.standalone() )
         if( brInfo.tclass && strncmp( brInfo.tclass->GetName(), "SG::PackedContainer<", 20) == 0)
            brInfo.isPackedContainer = true;

      string elem_tname, branch_tname;
      const type_info* ti = getAuxElementType( brInfo.tclass, typ, store.standalone(),
                                               elem_tname, branch_tname );
      const type_info* reg_ti = r.getType(auxid);
      if( ti && ti != reg_ti && strcmp(ti->name(), reg_ti->name()) != 0 )
      {
         // type in registry is different than type in the file.
         // will need to use ROOT auto schema evolution
         brInfo.needsSE = true;
         errorcheck::ReportMessage msg (MSG::INFO, ERRORCHECK_ARGS, "RootAuxDynReader");
         msg << "attribute " << brInfo.attribName << " (id=" << auxid <<
            " typename=" << SG::AuxTypeRegistry::instance().getType(auxid)->name()
             << ") has different type than the branch " << branch_tname;

         const std::type_info *tinf =  store.getIOType(auxid);
         brInfo.SE_tclass  = TClass::GetClass(*tinf);
         brInfo.SE_edt = kOther_t;
         if( !brInfo.SE_tclass  ) {
            brInfo.SE_edt = TDataType::GetType(*tinf);
            if( brInfo.SE_edt <=0 ) {
               brInfo.status = BranchInfo::TypeError;
               throw string("Error getting ROOT type for AUX branch ") + brInfo.branch->GetName()
                  + " typeinfo=" + tinf->name();
            }
         }
      }       
      brInfo.status = BranchInfo::Initialized;
   }
   return brInfo;
}


void RootAuxDynReader::addReaderToObject(void* object, size_t ttree_row, std::mutex* iomtx)
{
   auto store_holder = reinterpret_cast<SG::IAuxStoreHolder*>((char*)object + m_storeHolderOffset);
   bool standalone { store_holder->getStoreType()==SG::IAuxStoreHolder::AST_ObjectStore };
   if( !m_initialized )
      init(standalone);
   store_holder->setStore( new RootAuxDynStore(*this, ttree_row, standalone, iomtx) );
}              


void  RootAuxDynReader::addBytes(size_t bytes)
{
   m_bytesRead += bytes;
}

size_t RootAuxDynReader::getBytesRead() {
   return m_bytesRead;
}

void RootAuxDynReader::resetBytesRead() {
   m_bytesRead = 0;
}

const SG::auxid_set_t& RootAuxDynReader::auxIDs() const
{
   return m_auxids;
}
     
  


