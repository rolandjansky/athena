/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTChain.h"

using TV = boost::property_tree::ptree::value_type;  // tree-value type
using namespace std;

TrigConf::HLTMenu::HLTMenu()
{}

TrigConf::HLTMenu::HLTMenu(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::HLTMenu::~HLTMenu()
{}

std::size_t 
TrigConf::HLTMenu::size() const
{
   return m_data.get_child("chains").size();
}

std::string
TrigConf::HLTMenu::name() const
{
   return m_data.get_child("name").data();
}

TrigConf::HLTMenu::const_iterator
TrigConf::HLTMenu::begin() const
{
   return {m_data.get_child("chains"), 0,  [](auto x){return HLTChain(x.second);}};
}

TrigConf::HLTMenu::const_iterator
TrigConf::HLTMenu::end() const
{
   const auto & chains = m_data.get_child("chains");
   return {chains, chains.size(), [](auto x){return HLTChain(x.second);}};
}
