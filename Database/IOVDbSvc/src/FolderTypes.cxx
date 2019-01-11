/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "FolderTypes.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "IOVDbStringFunctions.h"
#include "CoolKernel/IRecordSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <vector>

namespace IOVDbNamespace{
  //determine folder type from DB folder ptr
  FolderType
  determineFolderType(const cool::IFolderPtr & pFolder, IClassIDSvc* /*clidsvc*/){
    const auto & folderDescription = pFolder->description();
    //If you find a coracool tag, it is unambiguously a coracool folder
    if (folderDescription.find("<coracool>") != std::string::npos) return CoraCool;
    const std::string typeName = parseTypename(folderDescription);
    //if the type is CondAttrListVec, and yet it is not a CoraCool, it must be a CoolVector
    if (typeName=="CondAttrListVec") return CoolVector;
    //check if the payload spec is compatible with a pool ref/pool ref collection
    if (poolCompatible(pFolder->payloadSpecification())){
      const auto & chans=pFolder->listChannels();
      const auto nchans = chans.size();
      const bool onlyOneSpecialChannel = ((nchans == 1) and (chans[0] == 0));
      return onlyOneSpecialChannel ? PoolRef : PoolRefColl;
    }
    if (typeName == "CondAttrListCollection") return AttrListColl;
    //defaults to AttrList
    return AttrList;
   }
   
   //determine folder type from CondAttrListCollection ptr
   FolderType
   determineFolderType(const CondAttrListCollection * pAttrListColl){
     FolderType ftype(AttrList);
     //has a single magic channel?
     if (pAttrListColl->size()==1 && pAttrListColl->begin()->first==0xFFFF) {
       if (poolCompatible (pAttrListColl)) return PoolRef;
       return AttrList;
     } else {
       ftype=AttrListColl;
       if ( pAttrListColl->size()>0) {
         if (poolCompatible(pAttrListColl)) return PoolRefColl;
       }
     }
     return ftype;
   }
   
   //determine folder type from CondAttrListCollection
   FolderType
   determineFolderType(const CondAttrListCollection & attrListColl){
     FolderType ftype(AttrList);
     //has a single magic channel?
     if (attrListColl.size()==1 && attrListColl.begin()->first==0xFFFF) {
       if (poolCompatible (attrListColl)) return PoolRef;
       return AttrList;
     } else {
       ftype=AttrListColl;
       if ( attrListColl.size()>0) {
         if (poolCompatible(attrListColl)) return PoolRefColl;
       }
     }
     return ftype;
   }
   
   FolderType
   determineFolderType(const coral::AttributeSpecification& spec){
     FolderType result(UNKNOWN);
     if (spec.name()=="PoolRef" && spec.typeName()=="string") result = PoolRef;
     return result;
   }
   
   bool
   poolCompatible(const cool::IRecordSpecification& rspec){
     return (rspec.exists("PoolRef") && rspec[0].name()=="PoolRef" &&
      rspec[0].storageType()==cool::StorageType::String4k);
   }
   
   bool
   poolCompatible(const CondAttrListCollection * pAttrListColl){
     const coral::AttributeList& payload1=pAttrListColl->begin()->second;
     const coral::AttributeSpecification& spec=payload1[0].specification();
     return (spec.name()=="PoolRef" && spec.typeName()=="string");
   }
   
   bool
   poolCompatible(const CondAttrListCollection & attrListColl){
     const coral::AttributeList& payload1=attrListColl.begin()->second;
     const coral::AttributeSpecification& spec=payload1[0].specification();
     return (spec.name()=="PoolRef" && spec.typeName()=="string");
   }
   
   std::string
   folderTypeName(const FolderType f){
      static const std::vector<std::string> names{"AttrList", "AttrListColl", "PoolRef",
      "PoolRefColl", "CoraCool", "CoolVector", "JSON"};
      if (f==UNKNOWN) return "Unknown";
      return names[f];
   }

}

