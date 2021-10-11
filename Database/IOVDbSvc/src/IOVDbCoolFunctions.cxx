/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbCoolFunctions.cxx
//@author Shaun Roe

#include "IOVDbCoolFunctions.h"
#include "IOVDbConn.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
//
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "AthenaKernel/IOVRange.h"
//
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <stdexcept>

namespace{
  const std::unordered_map<std::type_index, std::function<int(const coral::Attribute &)>>
  sizeFunctions{
    {std::type_index(typeid(bool)), [](const coral::Attribute & /*attr*/)->int { return 1; } },
    {std::type_index(typeid(unsigned char)), [](const coral::Attribute & /*attr*/)->int { return 1; } },
    {std::type_index(typeid(char)), [](const coral::Attribute & /*attr*/)->int { return 1; } },
    //
    {std::type_index(typeid(short)), [](const coral::Attribute & /*attr*/)->int { return 2; } },
    {std::type_index(typeid(unsigned short)), [](const coral::Attribute & /*attr*/)->int { return 2; } },
    {std::type_index(typeid(char)), [](const coral::Attribute & /*attr*/)->int { return 2; } },
    //
    {std::type_index(typeid(int)), [](const coral::Attribute & /*attr*/)->int { return 4; } },
    {std::type_index(typeid(unsigned int)), [](const coral::Attribute & /*attr*/)->int { return 4; } },
    {std::type_index(typeid(float)), [](const coral::Attribute & /*attr*/)->int { return 4; } },
    //
    {std::type_index(typeid(long long)), [](const coral::Attribute & /*attr*/)->int { return 8; } },
    {std::type_index(typeid(unsigned long long)), [](const coral::Attribute & /*attr*/)->int { return 8; } },
    {std::type_index(typeid(double)), [](const coral::Attribute & /*attr*/)->int { return 8; } },
    //
    {std::type_index(typeid(std::string)), [](const coral::Attribute & attr)->int { return attr.data<std::string>().size(); } },
    {std::type_index(typeid(coral::Blob)), [](const coral::Attribute & attr)->int { return attr.data<coral::Blob>().size(); } }
  };
}


namespace IOVDbNamespace{

  const coral::AttributeListSpecification &
  attrList2Spec(const coral::AttributeList& atrlist){
    return atrlist.specification();
  }
  
  unsigned int
  attributeSize(const coral::Attribute & attribute){
    const auto & spec{attribute.specification()};
    try{
      return sizeFunctions.at(std::type_index(spec.type()))(attribute);
    }catch (const std::out_of_range& oor) {
      return 0;
    }
  }
    
  bool
  typeSizeIsKnown(const coral::Attribute & attribute){
    return (sizeFunctions.find(std::type_index(attribute.specification().type())) != sizeFunctions.end());
  }
  
  unsigned int
  attributeListSize(const coral::AttributeList& atrlist){
    unsigned int total{};
    for (const auto & attribute:atrlist){
      total+=IOVDbNamespace::attributeSize(attribute);
    }
    return total;  
  }
  
  int
  countSelectedChannels(const std::vector<cool::ChannelId> & channels, const cool::ChannelSelection & selected){
    auto isSelected = [& selected](cool::ChannelId id){return selected.inSelection(id);};
    return std::count_if(channels.begin(), channels.end(),isSelected);//return type of count_if is signed
  }
  
  IOVTime
  makeEpochOrRunLumi(const cool::ValidityKey key, const bool timeIsEpoch){
    IOVTime time;
    if(timeIsEpoch){
      time.setTimestamp(key);
    } else {
      time.setRETime(key);
    }
    return time;
  }
  
  unsigned long long 
  iovTimeFromSeconds(const unsigned long long seconds){
    return seconds*1000000000LL;
  }
  
  unsigned long long
  iovTimeFromRunLumi(const unsigned long long run, const unsigned long long lumi){
    return (run<<32) + lumi;
  }
  
  std::pair<unsigned long long, unsigned long long>
  runLumiFromIovTime(const unsigned long long iovTime){
    return std::pair<unsigned long long, unsigned long long>{iovTime>>32, iovTime&0xFFFFFFFF};
  }
  
  IOVRange 
  makeRange(const cool::ValidityKey since,const cool::ValidityKey until,const bool timeIsEpoch) {
    // make an IOVRange object corresponding to given interval
    // dealing with timestamp/RunLB differences
    IOVTime itsince,ituntil;
    if (timeIsEpoch) {
      itsince.setTimestamp(since);
      ituntil.setTimestamp(until);
    } else {
      itsince.setRETime(since);
      ituntil.setRETime(until);
    }
    return IOVRange(itsince,ituntil);
  }

  cool::ChannelId 
  makeChannel(const std::string& strval, const cool::ChannelId defchan) {
    // construct a cool channelId from the string value (numeric)
    // if empty, use the default value
    if (not strval.empty()) return std::stol(strval);
    return defchan;
  }
  
  std::pair<bool, std::string>
  folderMetadata(IOVDbConn * pConnection, const std::string & folderName){
    bool multiversion{};
    std::string folderdesc{};
    // folder being read from COOL
    // get COOL database - will wake up connection on first access
    cool::IDatabasePtr dbPtr=pConnection->getCoolDb();
    if (not dbPtr.get()) {
      throw std::runtime_error( "Conditions database connection " + pConnection->name() + " cannot be opened - STOP" );
    }
    // get folder and read information
    if (!dbPtr->existsFolder(folderName)) {
      throw std::runtime_error( "Folder " + folderName + " does not exist" );
    }
    cool::IFolderPtr fldPtr=dbPtr->getFolder(folderName);
    // get versiontype of folder
    multiversion=(fldPtr->versioningMode()==cool::FolderVersioning::MULTI_VERSION);
    // read and process description string
    folderdesc=fldPtr->description();
    return std::make_pair(multiversion, std::move(folderdesc));
  }
  
  
  
  std::pair<std::vector<cool::ChannelId>, std::vector<std::string>>
  channelList(IOVDbConn * pConnection, const std::string & folderName, const bool named){
    std::vector<cool::ChannelId> channelNumbers;
    std::vector<std::string> channelNames;
    //
    cool::IDatabasePtr dbPtr=pConnection->getCoolDb();
    // get folder and read information
    if (!dbPtr->existsFolder(folderName)) {
      throw std::runtime_error( "Folder " + folderName + " does not exist" );
    }
    cool::IFolderPtr fldPtr=dbPtr->getFolder(folderName);
    if (named) {
      typedef std::map<cool::ChannelId,std::string> ChanMap_t;
      const ChanMap_t & chanmap=fldPtr->listChannelsWithNames();
      channelNumbers.reserve(chanmap.size());
      channelNames.reserve(chanmap.size());
      auto unzipElement = [&](const ChanMap_t::value_type & element){
        channelNumbers.push_back(element.first);
        channelNames.push_back(element.second);
      };
      std::for_each(chanmap.begin(), chanmap.end(),unzipElement);
    } else {
      channelNumbers=fldPtr->listChannels();
    }
    return std::make_pair(std::move(channelNumbers), std::move(channelNames));
  }
  
}

