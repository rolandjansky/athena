/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//@file TagFunctions.cxx
//@brief Implementation of helper functions for tag resolution
//@author Shaun Roe

#include "TagFunctions.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TagInfo.h"
#include "IOVDbStringFunctions.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <stdexcept>


namespace IOVDbNamespace{
  std::string 
  getTagInfo(const std::string &tag, const StoreGateSvc* detStore){
    std::string result{};
    enum ResultIndices{WHOLETAG, MAJMIN, PATH1, PATH2, SIZEWITHPREFIX};
    auto results=IOVDbNamespace::parseMagicTag(tag);
    if (results.empty()) return result;
    const auto & [prefix, target] = IOVDbNamespace::tag2PrefixTarget(results);
    const TagInfo* tagInfo{};
    if (StatusCode::SUCCESS==detStore->retrieve(tagInfo)) {
      std::string tagval{};
      tagInfo->findTag(target,tagval);
      if (not tagval.empty()) result=prefix+tagval;
    }
    return result;
  }
  
  std::string
  getGeoAtlasVersion(){
    std::string version{};
    IGeoModelSvc* geomodel=0;
    ISvcLocator* svcLocator=Gaudi::svcLocator();
    if (not svcLocator or StatusCode::SUCCESS!=svcLocator->service("GeoModelSvc",geomodel)) {
      throw std::runtime_error("The GeoModelSvc could not be retrieved in TagFunctions.cxx getGeoAtlasVersion()");
    } else { 
      version=geomodel->atlasVersion();
    }
    return version;
  }
  
  std::optional<bool>
  checkTagLock(const cool::IFolderPtr fptr, const std::string & tag){
    try {
      return (fptr->tagLockStatus(tag)==cool::HvsTagLock::LOCKED); 
    } catch (cool::Exception& e) {
      //dont do anything, but...
    }
    //...return a null object
    return std::nullopt;
  }
  
  
  
 
  
}
