/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Menu.h"
using namespace std;

#include <iterator>

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
   return m_data.get_child("items").size();
}

TrigConf::L1Menu::const_iterator
TrigConf::L1Menu::begin() const
{
   return {m_data.get_child("items"), 0,  [](auto x){return L1Item(x.second);}};
}

TrigConf::L1Menu::const_iterator
TrigConf::L1Menu::end() const
{
   const auto & items = m_data.get_child("items");
   return {items, items.size(), [](auto x){return L1Item(x.second);}};
}


std::vector<TrigConf::L1Threshold> 
TrigConf::L1Menu::thresholds(const std::string & type) const
{

   std::vector<TrigConf::L1Threshold> thrlist;

   if(type == "ALL") {
      auto allTypes = thresholdTypes();
      for ( const std::string & type : allTypes ) {
         const auto & thrOfType = thresholds(type);
         std::move(thrOfType.begin(), thrOfType.end(), std::back_inserter(thrlist));
      }

   } else if ( type == "internal" ) {

      std::string path = "thresholds.internal.names";
      
      const auto & thresholds = m_data.get_child(path);
      thrlist.reserve(thresholds.size());

      for( auto & thr : thresholds ) {
         thrlist.emplace_back( thr.second.data(), thr.second );
      }
      
   } else {

      std::string path = "thresholds.";

      if( hasChild(path+type) ) {
         path += type;
      } else if ( hasChild(path + "legacyCalo." + type) ){
         path += std::string("legacyCalo.") + type;
      } else {
         cerr << "unknown threshold type " << type << endl;
         return thrlist;
      }
      path += ".thresholds";

      const auto & thresholds = m_data.get_child(path);
      thrlist.reserve(thresholds.size());

      for( auto & thr : thresholds ) {
         thrlist.emplace_back( thr.first, thr.second );
      }

   }
   return thrlist;
}


std::vector<std::string> 
TrigConf::L1Menu::thresholdTypes() const
{
   std::vector<std::string> thrTypes;

   const auto & thresholds = m_data.get_child("thresholds");
   const auto & legacyThresholds = m_data.get_child("thresholds.legacyCalo");

   for( auto & thr : thresholds ) {
      if (thr.first == "legacyCalo")
         continue;
      thrTypes.push_back( thr.first );
   }
   for( auto & thr : legacyThresholds ) {
      thrTypes.push_back( thr.first );
   }
   return thrTypes;
}





void
TrigConf::L1Menu::printStats() const
{
   cout << "L1 menu '" << name() << "'" << endl;
   cout << "Items: " << size() << endl;
   cout << "Thresholds: " << thresholds().size() << "(of " << thresholdTypes().size() << " different types)" << endl;
   cout << "Topo algorithms: " << endl;
   cout << "    new   : " << m_data.get_child("topoAlgorithms.TOPO.decisionAlgorithms").size() << endl;
   cout << "    muon  : " << m_data.get_child("topoAlgorithms.MUTOPO.decisionAlgorithms").size() << endl;
   cout << "    mult  : " << m_data.get_child("topoAlgorithms.MULT.multiplicityAlgorithms").size() << endl;
   cout << "    legacy: " << m_data.get_child("topoAlgorithms.R2TOPO.decisionAlgorithms").size() << endl;
   cout << "Boards: " << m_data.get_child("boards").size() << endl;
   cout << "Connectors: " << m_data.get_child("connectors").size() << endl;
   unsigned int ctpinputs = m_data.get_child("ctp.inputs.optical").size();
   ctpinputs += m_data.get_child("ctp.inputs.electrical").size();
   ctpinputs += m_data.get_child("ctp.inputs.ctpin.slot7").size();
   ctpinputs += m_data.get_child("ctp.inputs.ctpin.slot8").size();
   ctpinputs += m_data.get_child("ctp.inputs.ctpin.slot9").size();
   cout << "CTP connections: " << ctpinputs << endl;
}

