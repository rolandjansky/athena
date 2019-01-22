/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// @file FolderTypes.h
// Header for FolderTypes utilities
// @author Shaun Roe
// @date 05 October 2017
#ifndef FolderTypes_H
#define FolderTypes_H

#include "CoolKernel/IFolder.h"
#include "CoolKernel/pointers.h" //IFolderPtr etc, typdefs
#include <string>

class IClassIDSvc;
class CondAttrListCollection;

namespace cool{
  class IRecordSpecification;
}


namespace IOVDbNamespace{
  //basic folder types
  enum FolderType {
    UNKNOWN=-1,
    AttrList=0,
    AttrListColl,
    PoolRef,
    PoolRefColl,
    CoraCool,
    CoolVector,
    JSON,
    NUM_FOLDER_TYPES
   };
   
   //determine folder type with optional check using clid service to check clid matches typename
   FolderType
   determineFolderType(const cool::IFolderPtr & pFolder, IClassIDSvc* clidsvc=nullptr);
   
   std::string
   folderTypeName(const FolderType f);
   
   FolderType
   determineFolderType(const CondAttrListCollection * pAttrListColl);
   
   FolderType
   determineFolderType(const CondAttrListCollection & pAttrListColl);
   
   FolderType
   determineFolderType(const coral::AttributeSpecification& spec);
   
   bool
   poolCompatible(const cool::IRecordSpecification& rspec);
   
   bool
   poolCompatible(const CondAttrListCollection * pAttrListColl);
   
   bool
   poolCompatible(const CondAttrListCollection & pAttrListColl);
}

#endif

