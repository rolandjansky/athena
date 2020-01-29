/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1TopoMenu.h"
#include "TrigConfData/L1TopoOutput.h"
using namespace std;


TrigConf::L1TopoMenu::L1TopoMenu()
{}

TrigConf::L1TopoMenu::L1TopoMenu(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::L1TopoMenu::~L1TopoMenu()
{}

const std::string &
TrigConf::L1TopoMenu::name() const
{
   return getAttribute("name");
}

unsigned int 
TrigConf::L1TopoMenu::version() const
{
   return getAttribute<unsigned int>("version");
}

std::size_t
TrigConf::L1TopoMenu::size() const
{
   return data().get_child("outputs").size();
}

vector<TrigConf::L1TopoAlgorithm>
TrigConf::L1TopoMenu::sortingAlgorithms() const
{

   std::vector<TrigConf::L1TopoAlgorithm> algorithmList;
   const auto & sortingAlgorithms = data().get_child("sortingAlgorithms");
   algorithmList.reserve(sortingAlgorithms.size());

   for( auto & alg : sortingAlgorithms )
      algorithmList.emplace_back( alg.second );

   return algorithmList;
}

vector<TrigConf::L1TopoAlgorithm>
TrigConf::L1TopoMenu::decisionAlgorithms() const
{

   std::vector<TrigConf::L1TopoAlgorithm> algorithmList;
   const auto & decisionAlgorithms = data().get_child("decisionAlgorithms");
   algorithmList.reserve(decisionAlgorithms.size());

   for( auto & alg : decisionAlgorithms )
      algorithmList.emplace_back( alg.second );

   return algorithmList;
}

TrigConf::L1TopoMenu::const_iterator
TrigConf::L1TopoMenu::begin() const
{
   return {data().get_child("outputs"), 0,  [](auto & x){return L1TopoOutput(x.second);}};
}

TrigConf::L1TopoMenu::const_iterator
TrigConf::L1TopoMenu::end() const
{
   const auto & outputs = data().get_child("outputs");
   return {outputs, outputs.size(), [](auto & x){return L1TopoOutput(x.second);}};
}
