/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//@file TagFunctions.cxx
//@brief Implementation of helper functions for tag resolution
//@author Shaun Roe

#include "TagFunctions.h"
#include "StoreGate/StoreGateSvc.h"
#include "IOVDbStringFunctions.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include <stdexcept>


namespace IOVDbNamespace{
  std::string 
  getTagInfo(const std::string &tag, const ITagInfoMgr *tagInfoMgr){
    std::string result{};
    enum ResultIndices{WHOLETAG, MAJMIN, PATH1, PATH2, SIZEWITHPREFIX};
    auto results=IOVDbNamespace::parseMagicTag(tag);
    if (results.empty()) return result;
    const auto & [prefix, target] = IOVDbNamespace::tag2PrefixTarget(results);
    std::string tagval = tagInfoMgr->findTag(target);
    if (not tagval.empty()) result=prefix+tagval;
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
  
  std::string
  resolveUsingTagInfo(const std::string & tag, const ITagInfoMgr *tagInfoMgr){
    // tag an input tag of form TagInfo{Major|Minor}/<tag> or 
    // TagInfo(Major|Minor}/<prefix>/<tag>
    // and resolve to value of TagInfo object tag <tag>
    // with optional prefix
    // <prefix>DEFAULT tags are no longer returned
    // return true for successful resolution
    std::string result="";
    enum ResultIndices{WHOLETAG, MAJMIN, PATH1, PATH2, SIZEWITHPREFIX};
    auto results=IOVDbNamespace::parseMagicTag(tag);
    if (results.empty()) {
       return result;
    }  
    const auto & [prefix, target] = IOVDbNamespace::tag2PrefixTarget(results);
    // try to get the Tag value
    if( tagInfoMgr ) {
      result= IOVDbNamespace::getTagInfo(tag, tagInfoMgr);
    }
    // if nothing found, try to get GeoAtlas directly from GeoModelSvc
    if (result.empty() and target=="GeoAtlas") {
      result=prefix+IOVDbNamespace::getGeoAtlasVersion();
    }
    if (not result.empty()) {
      std::string::size_type rstrip{};
      //number of characters to strip from tag end
      if (results[MAJMIN] == "Major") rstrip=6;
      if (results[MAJMIN] == "Minor") rstrip=3;
      // check if characters need to be stripped from end of tag
      if (rstrip>0 && tag.size()>rstrip) result=result.substr(0,result.size()-rstrip);
    } else {
      throw std::runtime_error("Could not resolve TagInfo tag " + target );
    }
    return result;
  }
  
  
  
 
  
}
