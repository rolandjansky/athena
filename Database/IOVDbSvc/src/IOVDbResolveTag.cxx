/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// @file IOVDbResolveTag.cxx
// Implementation for tag resolving function, associating global tag to local folder
// @author Shaun Roe
// @date 10 April 2018

#include "IOVDbResolveTag.h"
#include "CrestApi/CrestApi.h"
#include "nlohmann/json.hpp"
#include <sstream>
#include <iostream>

using json = nlohmann::json;

namespace IOVDbNamespace{
  std::string
  resolveCrestTag(const std::string & globalTagName, const std::string & folderName, const std::string & forceTag){
    std::string result{};
    if (not forceTag.empty()) return forceTag;
    const std::string urlBase{"http://crest-01.cern.ch:8080"};
    auto crestClient = Crest::CrestClient(urlBase);
    auto j = crestClient.findGlobalTagMap(globalTagName);
    for (const auto &i:j){
      if (i["label"] == folderName){
        result=i["tagName"];
        break;
      }
    }
    return result;
  }


}
