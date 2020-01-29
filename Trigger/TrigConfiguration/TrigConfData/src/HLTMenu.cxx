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
   return data().get_child("chains").size();
}

std::string
TrigConf::HLTMenu::name() const
{
   return getAttribute("name");
}

TrigConf::HLTMenu::const_iterator
TrigConf::HLTMenu::begin() const
{
   return {data().get_child("chains"), 0,  [](auto & x){return Chain(x.second);}};
}

TrigConf::HLTMenu::const_iterator
TrigConf::HLTMenu::end() const
{
   auto & pt = data().get_child("chains");
   return { pt, pt.size(), [](auto & x){return Chain(x.second);} };
}



void
TrigConf::HLTMenu::printMenu(bool full) const
{
   cout << "HLT menu '" << name() << "'" << endl;
   cout << "Chains: " << size() << endl;
   if(full) {
      int c(0);
      for( auto & chain : *this ) {
         cout << "  " << c++ << ": " << chain.name() << endl;
      }
   }
   cout << "The HLTMenu " << (ownsData() ? "owns" : "does not own") << " the ptree" << endl;
}
