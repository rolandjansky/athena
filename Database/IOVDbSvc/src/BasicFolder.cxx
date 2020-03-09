/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "BasicFolder.h"
#include <iostream>

namespace IOVDbNamespace{

static const std::pair<cool::ValidityKey, cool::ValidityKey> infiniteIov(0,cool::ValidityKeyMax);
static const coral::AttributeList invalidPayload{};
static const std::vector<coral::AttributeList> invalidVectorPayload{};


  BasicFolder::BasicFolder():
    m_payload{},
    m_vectorPayload{},
    m_name2Channel{},
    m_channel2Name{},
    m_isVectorPayload{},
    m_channels{},
    m_iov{infiniteIov} {

  }
    bool 
    BasicFolder::empty() const{
     return m_channels.empty();
    }
  
    void 
    BasicFolder::setVectorPayloadFlag(const bool flag){
      m_isVectorPayload=flag;
    }
    
    bool 
    BasicFolder::isVectorPayload() const {
      return m_isVectorPayload;
    }
    //
    void 
    BasicFolder::setIov(const std::pair<cool::ValidityKey, cool::ValidityKey> & iov){
      m_iov=iov;
    }
  
    //add attributeList
    void 
    BasicFolder::addChannelPayload(const cool::ChannelId & channelId, const std::string & name, const coral::AttributeList & payload){
      m_name2Channel[name]=channelId;
      m_channel2Name[channelId]=name;
      addChannelPayload(channelId, payload);
    }
  
    void 
    BasicFolder::addChannelPayload(const cool::ChannelId & channelId, const coral::AttributeList& payload){

      m_channels.push_back(channelId);
      m_payload[channelId]=payload;
    }
  
    //add vector payload
    void 
    BasicFolder::addChannelPayload(const cool::ChannelId & channelId, const std::string & name, const std::vector<coral::AttributeList> & payload){ 
      m_name2Channel[name]=channelId;
      m_channel2Name[channelId]=name;
      addChannelPayload(channelId, payload);
    }
  
    void 
    BasicFolder::addChannelPayload(const cool::ChannelId & channelId, const std::vector<coral::AttributeList> & payload){ 
      m_channels.push_back(channelId);
      m_vectorPayload[channelId]=payload;
    }
  
    //
    coral::AttributeList 
    BasicFolder::getPayload(const cool::ChannelId & channelId){ 
      return m_payload.find(channelId)->second;
    }
  
    coral::AttributeList 
    BasicFolder::getPayload(const std::string & channelName){
      if (m_isVectorPayload) return invalidPayload;
      const auto chanPtr= m_name2Channel.find(channelName);
      bool nameExists = (chanPtr != m_name2Channel.end());
      if (not nameExists) return invalidPayload;
      const cool::ChannelId channel=chanPtr->second;
      return getPayload(channel);
    }
  
    std::vector<coral::AttributeList> 
    BasicFolder::getVectorPayload(const cool::ChannelId & channelId){
      return m_vectorPayload.find(channelId)->second;
    }
  
    std::vector<coral::AttributeList> 
    BasicFolder::getVectorPayload(const std::string & channelName){
      if (not m_isVectorPayload) return invalidVectorPayload;
      const auto chanPtr= m_name2Channel.find(channelName);
      bool nameExists = (chanPtr != m_name2Channel.end());
      if (not nameExists) return invalidVectorPayload;
      const cool::ChannelId channel=chanPtr->second;
      return getVectorPayload(channel);
    }
  
    std::vector<cool::ChannelId> 
    BasicFolder::channelIds(){ 
      return m_channels;
    }
  
    std::pair<cool::ValidityKey, cool::ValidityKey> 
    BasicFolder::iov(){ 
      return m_iov;
    }
 
  
  }//namespace end

