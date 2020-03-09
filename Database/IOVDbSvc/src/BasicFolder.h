/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// @file BasicFolder.h
// Header for FolderTypes utilities
// @author Shaun Roe
// @date May 2019
#ifndef IOVDbSvc_BasicFolder_h
#define IOVDbSvc_BasicFolder_h

#include <utility>
#include "CoolKernel/ValidityKey.h" //a typedef
#include "CoolKernel/ChannelId.h" 
#include <string>
#include <vector>
#include "CoralBase/AttributeList.h"
#include <map>

namespace IOVDbNamespace{
//@Basic data structure to hold the folder data retrieved from CREST or file
class BasicFolder{
  public:
  BasicFolder();
  void setVectorPayloadFlag(const bool flag);
  bool isVectorPayload() const;
  //
  void setIov(const std::pair<cool::ValidityKey, cool::ValidityKey> & iov);
  //add attributeList
  void addChannelPayload(const cool::ChannelId & channelId, const std::string & name, const coral::AttributeList & payload);
  void addChannelPayload(const cool::ChannelId & channelId, const coral::AttributeList & payload);
  //add vector payload
  void addChannelPayload(const cool::ChannelId & channelId, const std::string & name, const std::vector<coral::AttributeList> & payload);
  void addChannelPayload(const cool::ChannelId & channelId, const std::vector<coral::AttributeList> & payload);
  //
  coral::AttributeList getPayload(const cool::ChannelId & channelId);
  coral::AttributeList getPayload(const std::string & channelName);
  std::vector<coral::AttributeList> getVectorPayload(const cool::ChannelId & channelId);
  std::vector<coral::AttributeList> getVectorPayload(const std::string & channelName);
  std::vector<cool::ChannelId> channelIds();
  std::pair<cool::ValidityKey, cool::ValidityKey> iov();
  //
  bool empty() const;
  
  private:
  //ugh, should be templated, and consider using hashmaps
  std::map<int, coral::AttributeList> m_payload;
  std::map<int, std::vector<coral::AttributeList>> m_vectorPayload;
  //could use single bidirectional map
  std::map<std::string, int> m_name2Channel;
  std::map<int, std::string> m_channel2Name;
  //ugliness
  bool m_isVectorPayload;
  std::vector<cool::ChannelId> m_channels;
  std::pair<cool::ValidityKey, cool::ValidityKey> m_iov;
};

}
#endif

