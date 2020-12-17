/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//@file TagFunctions.h
//@brief Helper functions for tag resolution
//@author Shaun Roe
#ifndef IOVDbSvc_TagFunctions_h
#define IOVDbSvc_TagFunctions_h

#include "CoolKernel/IFolder.h"
#include <string>
#include <optional>
class ITagInfoMgr;

namespace IOVDbNamespace{
  ///Retrieve the TagInfo
  std::string getTagInfo(const std::string &tag, const ITagInfoMgr *tagInfoMgr);
  
  ///Get the GeoAtlas version directly from GeoModelSvc
  std::string getGeoAtlasVersion();
  
  ///Check whether a tag is locked on a folder, if possible
  std::optional<bool>
  checkTagLock(const cool::IFolderPtr fptr, const std::string & tag);
  
  std::string
  resolveUsingTagInfo(const std::string & tag, const ITagInfoMgr *tagInfoMgr);
}
#endif
