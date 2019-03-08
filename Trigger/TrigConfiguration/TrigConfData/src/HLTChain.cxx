/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTChain.h"
TrigConf::HLTChain::HLTChain()
{}

TrigConf::HLTChain::HLTChain(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::HLTChain::~HLTChain()
{}

const std::string &
TrigConf::HLTChain::name() const
{
   return data().get_child("name").data();
}

unsigned int
TrigConf::HLTChain::counter() const
{
   return data().get_child("counter").get_value<unsigned int>();
}

const std::string &
TrigConf::HLTChain::l1item() const
{
   return data().get_child("l1item").data();
}

std::vector<TrigConf::DataStructure>
TrigConf::HLTChain::streams() const
{
   std::vector<DataStructure> strlist;
   const auto & streams = m_data.get_child("streams");
   strlist.reserve(streams.size());

   for( auto & strData : streams )
      strlist.emplace_back( strData.second );
   
   return strlist;
}

std::vector<std::string>
TrigConf::HLTChain::groups() const
{
   std::vector<std::string> grouplist;
   const auto & groups = m_data.get_child("groups");
   grouplist.reserve(groups.size());

   for( auto & groupData : groups )
      grouplist.emplace_back( groupData.second.get_child("name").data() );

   return grouplist;
}

