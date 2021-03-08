/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// @file CrestFunctions.cxx
// Implementation for CrestFunctions utilities
// @author Shaun Roe
// @date 1 July 2019

#include "CrestFunctions.h"
#include "CrestApi/CrestApi.h"
#include <iostream>
#include <exception>
#include <regex>
#include "IOVDbStringFunctions.h"

namespace IOVDbNamespace{
  const std::string
  urlBase(){
    return  "http://crest-01.cern.ch:8080";
  }

  std::string
  extractHashFromJson(const std::string & jsonReply){
    std::string hash{};
    try{
      std::string signature="payloadHash\":\"";
      auto signaturePosition=jsonReply.rfind(signature);
      if (signaturePosition == std::string::npos) throw std::runtime_error("signature "+signature+" not found");
      auto startOfHash=signaturePosition + signature.size();
      auto endOfHash=jsonReply.rfind("\"");
      auto len=endOfHash-startOfHash;
      if (startOfHash > jsonReply.size()) throw std::runtime_error("Hash start is beyond end of string");
      hash=jsonReply.substr(startOfHash, len);
    } catch (std::exception & e){
      std::cout<<__FILE__<<":"<<__LINE__<< ": "<<e.what()<<" while trying to find the hash in "<<jsonReply<<std::endl;
    }
    return hash;
  }

  std::string 
  getIovsForTag(const std::string & tag, const bool testing){
    std::string reply{R"delim([{"tagName":"Indet_Align-channelList","since":0,"insertionTime":"2019-07-08T15:33:46.124+0000","payloadHash":"551e8b2807dea49dd91933ba963d8eff78b3441ae5836cd17cddad26ec14ecc3"}])delim"};
    if (not testing){
      //...CrestApi returns Iovs as a json object
      auto myCrestClient = Crest::CrestClient(urlBase());
      reply = myCrestClient.findAllIovs(tag);
    }
    return extractHashFromJson(reply);
  }

  std::string 
  getPayloadForHash(const std::string & hash, const bool testing){
    std::string reply{R"delim({"channel_list": ["0", "100", "101", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230"]})delim"};
    if (not testing){
      //CrestApi method:
      auto   myCrestClient = Crest::CrestClient(urlBase());
      reply = myCrestClient.getPayloadAsString(hash);
    }
    return reply;
  }
  
  std::string 
  getPayloadForTag(const std::string & tag, const bool testing){
    return getPayloadForHash(getIovsForTag(tag, testing), testing);
  }
  
  std::string 
  extractDescriptionFromJson(const std::string & jsonReply){
    std::string description{};
    try{
      const std::string signature="node_description\": \"";
      const auto signaturePosition = jsonReply.find(signature);
      if (signaturePosition == std::string::npos) throw std::runtime_error("signature "+signature+" not found");
      const auto startOfDescription= signaturePosition + signature.size();
      const auto endOfDescription=jsonReply.find_last_of("\"");
      const auto len=endOfDescription-startOfDescription;
      description=jsonReply.substr(startOfDescription, len);
    } catch (std::exception & e){
      std::cout<<__FILE__<<":"<<__LINE__<< ": "<<e.what()<<" while trying to find the description in "<<jsonReply<<std::endl;
    }
    
    return unescapeQuotes(description);
  }
  
  std::string 
  extractSpecificationFromJson(const std::string & jsonReply){
    std::string spec{};
    try{
      const std::string signature="folder_payloadspec\": \"";
      const auto signaturePosition = jsonReply.find(signature);
      if (signaturePosition == std::string::npos) throw std::runtime_error("signature "+signature+" not found");
      const auto startOfSpec= signaturePosition + signature.size();
      const auto endOfSpec=jsonReply.find_last_of("\"");
      const auto len=endOfSpec-startOfSpec;
      spec=jsonReply.substr(startOfSpec, len);
    } catch (std::exception & e){
      std::cout<<__FILE__<<":"<<__LINE__<< ": "<<e.what()<<" while trying to find the payload spec in "<<jsonReply<<std::endl;
    }
    return spec;
  }
  
  std::vector<cool::ChannelId> 
  extractChannelListFromJson(const std::string & jsonReply){
    std::vector<cool::ChannelId> list;
    std::string textRep;
    try{
      const std::string signature="channel_list\": ";
      const auto startOfList=jsonReply.find(signature) + signature.size();
      const auto endOfList=jsonReply.find_last_of("}");
      const auto len=endOfList-startOfList;
      textRep=jsonReply.substr(startOfList, len);
    } catch (std::exception & e){
      std::cout<<__FILE__<<":"<<__LINE__<< ": "<<e.what()<<"\n while trying to find the description in "<<jsonReply<<std::endl;
    }
    //channel list is of format ["0", "2", "5"]
    std::regex r("\"([0-9]+)\"");
    std::sregex_iterator it(textRep.begin(), textRep.end(), r);
    std::sregex_iterator end;
    for (;it!=end;++it){
      std::smatch  m= *it;
      if (not m.empty()) list.push_back(std::stoll(m[1].str()));
    }
    return list;
  }
  
  std::string 
  folderDescriptionForTag(const std::string & tag, const bool testing){
    const auto descriptionTag=tag+"-description";
    std::string jsonReply{R"delim({"node_description": "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1170039409\" /></addrHeader><typeName>AlignableTransformContainer</typeName>"})delim"};
    if (not testing){
      jsonReply= getPayloadForHash(getIovsForTag(descriptionTag));
    }
    return extractDescriptionFromJson(jsonReply);
  }
  
  std::string 
  payloadSpecificationForTag(const std::string & tag, const bool testing){
    const auto specTag=tag+"-payloadSpec";
    std::string jsonReply{R"delim({"folder_payloadspec": "PoolRef: String4k"})delim"};
    if (not testing){
      jsonReply= getPayloadForHash(getIovsForTag(specTag));
    }
    return extractSpecificationFromJson(jsonReply);
  }
  
  std::vector<cool::ChannelId> 
  channelListForTag(const std::string & tag, const bool testing){
    const auto channelListTag=tag+"-channelList";
    std::string reply{R"delim({"channel_list": ["0", "100", "101", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230"]})delim"};
    if (not testing){
     reply = getPayloadForHash(getIovsForTag(channelListTag, testing), testing);
    }
    return extractChannelListFromJson(reply);
  }
  
  
}
