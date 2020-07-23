/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// @file IOVDbResolveTag.cxx
// Implementation for tag resolving function, associating global tag to local folder
// @author Shaun Roe
// @date 10 April 2018

#include "IOVDbResolveTag.h"
#include "IOVDbSvcCurl.h"
#include "nlohmann/json.hpp"
#include <sstream>
#include <iostream>

using json = nlohmann::json;

namespace IOVDbNamespace{
  std::string
  resolveCrestTag(const std::string & globalTagName, const std::string & folderName, const std::string & forceTag){
    std::string result{};
    static std::string curlReply{};//preserves state, not very threadsafe
    if (not forceTag.empty()) return forceTag;
    if (curlReply.empty()){
      const std::string urlBase{"http://crest-undertow.web.cern.ch/crestapi"};
      const std::string getTagMap="/globaltagmaps/"+globalTagName;
      const std::string url=urlBase+getTagMap;
      //std::cout<<"Calling CURL"<<std::endl;
      IOVDbSvcCurl request(url);
      curlReply=request.get();
    }
    std::istringstream ss(curlReply);
    json j;
    ss>>j;
    for (const auto &i:j){
      if (i["label"] == folderName){
        result=i["tagName"];
        break;
      }
    }
    return result;
  }


}
