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
      auto endOfHash=jsonReply.find("\"",startOfHash);
      auto len=endOfHash-startOfHash;
      if (startOfHash > jsonReply.size()) throw std::runtime_error("Hash start is beyond end of string");
      hash=jsonReply.substr(startOfHash, len);
    } catch (std::exception & e){
      std::cout<<__FILE__<<":"<<__LINE__<< ": "<<e.what()<<" while trying to find the hash in "<<jsonReply<<std::endl;
    }
    return hash;
  }
  //N.B. Returns ONE hash for now, the last one.
  std::string 
  getIovsForTag(const std::string & tag, const bool testing){
    std::string reply{R"delim([{"insertionTime":"2022-05-26T12:10:58+0000","payloadHash":"99331506eefbe6783a8d5d5bc8b9a44828a325adfcaac32f62af212e9642db71","since":0,"tagName":"LARIdentifierFebRodMap-RUN2-000"}])delim"};
    if (not testing){
      //...CrestApi returns Iovs as a json object
      auto myCrestClient = Crest::CrestClient(urlBase());
      reply = myCrestClient.findAllIovs(tag).dump();
    }
    return extractHashFromJson(reply);
  }

  std::string 
  getPayloadForHash(const std::string & hash, const bool testing){
    std::string reply{R"delim({"data":{"0":["[DB=B2E3B2B6-B76C-DF11-A505-000423D5ADDA][CNT=CollectionTree(LArTTCell_P/LArTTCellMapAtlas)][CLID=DF8C509C-A91A-40B5-B76C-5B57EEE21EC3][TECH=00000202][OID=00000003-00000000]"]}})delim"};
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
      const std::string signature="node_description\\\":\\\"";
      const auto signaturePosition = jsonReply.find(signature);
      if (signaturePosition == std::string::npos) throw std::runtime_error("signature "+signature+" not found");
      const auto startOfDescription= signaturePosition + signature.size();
      const std::string endSignature = "\\\",\\\"payload_spec";
      const auto endOfDescription=jsonReply.find(endSignature);
      if (endOfDescription == std::string::npos) throw std::runtime_error("end signature "+endSignature+" not found");
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
      const std::string signature="payload_spec\\\":\\\"";
      const auto signaturePosition = jsonReply.find(signature);
      if (signaturePosition == std::string::npos) throw std::runtime_error("signature "+signature+" not found");
      const auto startOfSpec= signaturePosition + signature.size();
      const auto endOfSpec=jsonReply.find("\\\"}\"",startOfSpec);
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
      const std::string signature="channel_list\\\":[";
      const auto startOfList=jsonReply.find(signature) + signature.size();
      const auto endOfList=jsonReply.find("]", startOfList);
      const auto len=endOfList-startOfList;
      textRep=jsonReply.substr(startOfList, len);
    } catch (std::exception & e){
      std::cout<<__FILE__<<":"<<__LINE__<< ": "<<e.what()<<"\n while trying to find the description in "<<jsonReply<<std::endl;
    }
    //channel list is of format ["0", "2", "5"]
    std::string s=R"d(\{\\\"([0-9]+)\\\":\\\"([^"])\"},?)d";
    std::regex r(s);
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
    //std::string jsonReply{R"delim({"node_description": "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1170039409\" /></addrHeader><typeName>AlignableTransformContainer</typeName>"})delim"};
    std::string jsonReply{R"delim({"format":"TagMetaSetDto","resources":[{"tagName":"LARAlign-RUN2-UPD4-03","description":"{\"dbname\":\"CONDBR2\",\"nodeFullpath\":\"/LAR/Align\",\"schemaName\":\"COOLONL_LAR\"}","chansize":1,"colsize":1,"tagInfo":"{\"channel_list\":[{\"0\":\"\"}],\"node_description\":\"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\\"71\\\" clid=\\\"254546453\\\" /></addrHeader><typeName>DetCondKeyTrans</typeName>\",\"payload_spec\":\"PoolRef:String4k\"}","insertionTime":"2022-05-26T12:10:38+0000"}],"size":1,"datatype":"tagmetas","format":null,"page":null,"filter":null})delim"};
    if (not testing){
      auto myCrestClient = Crest::CrestClient(urlBase());
      jsonReply= myCrestClient.getTagMetaInfo(tag).dump();
    }
    return extractDescriptionFromJson(jsonReply);
  }
  
  std::string 
  payloadSpecificationForTag(const std::string & specTag, const bool testing){
    std::string jsonReply{R"delim({"folder_payloadspec": "PoolRef: String4k"})delim"};
    if (not testing){
      auto myCrestClient = Crest::CrestClient(urlBase());
      jsonReply= myCrestClient.getTagMetaInfo(specTag).dump();
    }
    return extractSpecificationFromJson(jsonReply);
  }
  
  std::vector<cool::ChannelId> 
  channelListForTag(const std::string & tag, const bool testing){
    const auto channelListTag=tag;
    std::string reply{R"delim({"format":"TagMetaSetDto","resources":[{"tagName":"LARBadChannelsOflBadChannels-RUN2-UPD4-21","description":"{\"dbname\":\"CONDBR2\",\"nodeFullpath\":\"/LAR/BadChannelsOfl/BadChannels\",\"schemaName\":\"COOLOFL_LAR\"}","chansize":8,"colsize":5,"tagInfo":"{\"channel_list\":[{\"0\":\"\"},{\"1\":\"\"},{\"2\":\"\"},{\"3\":\"\"},{\"4\":\"\"},{\"5\":\"\"},{\"6\":\"\"},{\"7\":\"\"}],\"node_description\":\"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\\"71\\\" clid=\\\"1238547719\\\" /></addrHeader><typeName>CondAttrListCollection</typeName>\",\"payload_spec\":\"ChannelSize:UInt32,StatusWordSize:UInt32,Endianness:UInt32,Version:UInt32,Blob:Blob64k\"}","insertionTime":"2022-05-26T16:40:32+0000"}],"size":1,"datatype":"tagmetas","format":null,"page":null,"filter":null})delim"};
    if (not testing){
     auto myCrestClient = Crest::CrestClient(urlBase());
     reply= myCrestClient.getTagMetaInfo(tag).dump();
    }
    return extractChannelListFromJson(reply);
  }
  
  std::string
  resolveCrestTag(const std::string & globalTagName, const std::string & folderName, const std::string & forceTag, const bool testing){
    std::string result{};
    if (not forceTag.empty()) return forceTag;
    if (testing) return "LARAlign-RUN2-UPD4-03";
    auto crestClient = Crest::CrestClient(urlBase());
    auto j = crestClient.findGlobalTagMap(globalTagName);
    for (const auto &i:j){
      if (i["label"] == folderName){
        result=static_cast<std::string>(i["tagName"]);
        break;
      }
    }
    return result;
  }
  
  std::string
  jsonTagName(const std::string &globalTag, const std::string & folderName){
    std::cout<<"resolving tag "<<globalTag<<", "<<folderName<<std::endl; 
    return resolveCrestTag(globalTag,folderName);
  }
  
}
