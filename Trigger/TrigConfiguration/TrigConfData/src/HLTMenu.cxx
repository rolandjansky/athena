/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTMenu.h"

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
   auto & pt = m_data.get_child("chains");
   auto ci = const_iterator(pt, 0,  [](auto x){return Chain(x.second);});
   return ci;
}

TrigConf::HLTMenu::const_iterator
TrigConf::HLTMenu::end() const
{
   auto & pt = m_data.get_child("chains");
   TrigConf::HLTMenu::const_iterator x = { pt, pt.size(), [](auto x){return Chain(x.second);} };
   return x;
}


