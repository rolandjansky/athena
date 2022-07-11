/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// @file CrestFunctions.h
// Header for CrestFunctions utilities
// @author Shaun Roe
// @date 1 July 2019

#ifndef IOVDbSvc_CrestFunctions_h
#define IOVDbSvc_CrestFunctions_h

#include <vector>
#include <string>
#include <map>

#include "CoolKernel/ChannelId.h"

namespace IOVDbNamespace{

  const std::string
  urlBase();

  std::string
  extractHashFromJson(const std::string & jsonReply);

  std::string 
  getIovsForTag(const std::string & tag, const bool testing=false);

  std::string 
  getPayloadForHash(const std::string & hash, const bool testing=false);
  
  std::vector<cool::ChannelId> 
  extractChannelListFromJson(const std::string & jsonReply);

  std::vector<cool::ChannelId> 
  channelListForTag(const std::string & tag, const bool testing=false);
  
  std::map<cool::ChannelId, std::string> 
  channelNameMap(const std::string & folderName);
  
  std::string 
  getPayloadForTag(const std::string & tag,const bool testing=false);
  
  std::string 
  folderDescriptionForTag(const std::string & tag, const bool testing=false);
  
  std::string 
  payloadSpecificationForTag(const std::string & tag, const bool testing=false);
  
  std::string 
  extractDescriptionFromJson(const std::string & jsonReply);
  
  std::string
	resolveCrestTag(const std::string & globalTagName, const std::string & folderName, const std::string & forceTag="", const bool testing=false);
	
  
  std::string
  jsonTagName(const std::string &globalTag, const std::string & folderName);
}
#endif
