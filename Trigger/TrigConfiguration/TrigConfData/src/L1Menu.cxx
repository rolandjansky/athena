/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Menu.h"
using namespace std;


TrigConf::L1Menu::L1Menu()
{}

TrigConf::L1Menu::L1Menu(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::L1Menu::~L1Menu()
{}

std::string
TrigConf::L1Menu::name() const
{
   return m_data.get_child("name").data();
}

unsigned int 
TrigConf::L1Menu::version() const
{
   return m_data.get_child("version").get_value<unsigned int>();
}

unsigned int 
TrigConf::L1Menu::ctpVersion() const
{
   return m_data.get_child("ctpVersion").get_value<unsigned int>();
}

std::size_t 
TrigConf::L1Menu::size() const
{
   return m_data.get_child("menu.items").size();
}

TrigConf::L1Menu::const_iterator
TrigConf::L1Menu::begin() const
{
   return {m_data.get_child("menu.items"), 0,  [](auto x){return L1Item(x.second);}};
}

TrigConf::L1Menu::const_iterator
TrigConf::L1Menu::end() const
{
   const auto & items = m_data.get_child("menu.items");
   return {items, items.size(), [](auto x){return L1Item(x.second);}};
}


std::vector<TrigConf::L1Threshold> 
TrigConf::L1Menu::thresholds() const
{
   std::vector<TrigConf::L1Threshold> thrlist;
   const auto & thresholds = m_data.get_child("thresholds");
   thrlist.reserve(thresholds.size());

   for( auto & thr : thresholds )
      thrlist.emplace_back( thr.second );

   return thrlist;
}

