/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//@file IOVDbCoolFunctions.cxx
//@author Shaun Roe

#include "IOVDbCoolFunctions.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
//
#include "CoolKernel/ChannelSelection.h"

//
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <stdexcept>

namespace{
  std::unordered_map<std::type_index, std::function<int(const coral::Attribute &)>> 
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
}