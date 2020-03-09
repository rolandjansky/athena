/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// @file FolderTypes.h
// Header for FolderTypes utilities
// @author Shaun Roe
// @date 05 October 2017
#ifndef IOVDbSvc_FolderTypes_h
#define IOVDbSvc_FolderTypes_h

#include "CoolKernel/IFolder.h"
#include "CoolKernel/pointers.h" //IFolderPtr etc, typdefs
#include <string>
#include <vector>

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
   
   ///Determine folder type with optional check using clid service to check clid matches typename
   FolderType
   determineFolderType(const std::string & folderDescription, const std::string & spec, const std::vector<cool::ChannelId> & chans);
   
   ///Determine folder type with optional check using clid service to check clid matches typename
   FolderType
   determineFolderType(const cool::IFolderPtr & pFolder, IClassIDSvc* clidsvc=nullptr);
   
   ///Give a string name for the folder type
   std::string
   folderTypeName(const FolderType f);
   
   ///Determine folder type using the CondAttrListCollection pointer
   FolderType
   determineFolderType(const CondAttrListCollection * pAttrListColl);
   
   ///Determine folder typeusing the CondAttrListCollection reference
   FolderType
   determineFolderType(const CondAttrListCollection & pAttrListColl);
   
   ///Determine folder type using the coral::AttributeSpecification
   FolderType
   determineFolderType(const coral::AttributeSpecification& spec);
   
   //@{
   /** Is the folder type compatible with Pool storage type? */
   bool
   poolCompatible(const cool::IRecordSpecification& rspec);
   
   bool
   poolCompatible(const CondAttrListCollection * pAttrListColl);
   
   bool
   poolCompatible(const CondAttrListCollection & pAttrListColl);
   
   bool
   poolCompatible(const std::string & specString);
   //@}
}

#endif

