/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Menu.h"
using namespace std;

#include <iterator>
#include <utility>
TrigConf::L1Menu::L1Menu()
{}

TrigConf::L1Menu::L1Menu(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   update();
}

TrigConf::L1Menu::~L1Menu()
{}

void
TrigConf::L1Menu::update()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name");

   // thresholds
   for( const std::string & path : {"thresholds", "thresholds.legacyCalo" } ) {
      for( auto & thrByType : data().get_child( path ) ) {
         const std::string & thrType = thrByType.first;
         if (thrType == "legacyCalo")
            continue;
         auto & v = m_thresholdsByType[thrType] = std::vector<TrigConf::L1Threshold>();
         if(thrType == "internal") {
            const auto & thresholds = data().get_child( path + ".internal.names");
            for( auto & thr : thresholds ) {
               v.emplace_back( thr.second.data(), thrType, thr.second );
               m_thresholdsByName[ thr.first ] = & v.back();
            }
         } else {
            const auto & thresholds = data().get_child( path + "." + thrType + ".thresholds");
            for( auto & thr : thresholds ) {
               v.emplace_back( thr.first, thrType, thr.second );
               m_thresholdsByName[ thr.first ] = & v.back();
            }
         }
      }
   }

   // connectors
   for( auto & conn : data().get_child( "connectors" ) ) {
      m_connectors.emplace( std::piecewise_construct,
                            std::forward_as_tuple(conn.first),
                            std::forward_as_tuple(conn.first, conn.second) );
   }

}


std::string
TrigConf::L1Menu::name() const
{
   return m_name;
}

unsigned int 
TrigConf::L1Menu::version() const
{
   return getAttribute<unsigned int>("version");
}

unsigned int 
TrigConf::L1Menu::ctpVersion() const
{
   return getAttribute<unsigned int>("ctpVersion");
}

std::size_t 
TrigConf::L1Menu::size() const
{
   return data().get_child("items").size();
}

TrigConf::L1Item
TrigConf::L1Menu::item(const std::string & itemName) const
{
   if(itemName=="") {
      throw std::runtime_error("L1Menu::item() was called with empty itemName");
   }
   ptree pt;
   try {
      pt = data().get_child(ptree::path_type("items/"+itemName,'/')); // '/' is not used in any item Name, so we can use it as separator in the path 
   } catch(boost::property_tree::ptree_bad_path & ex) {
      throw std::runtime_error("L1Item " + itemName + " does not exist in the menu");
   }
   return L1Item(pt);
}

TrigConf::L1Menu::const_iterator
TrigConf::L1Menu::begin() const
{
   return {data().get_child("items"), 0,  [](auto & x){return L1Item(x.second);}};
}


TrigConf::L1Menu::const_iterator
TrigConf::L1Menu::end() const
{
   const auto & items = data().get_child("items");
   return {items, items.size(), [](auto & x){return L1Item(x.second);}};
}


const std::vector<TrigConf::L1Threshold> &
TrigConf::L1Menu::thresholds(const std::string & typeName) const
{
   return m_thresholdsByType.at(typeName);
}


std::vector<TrigConf::L1Threshold> 
TrigConf::L1Menu::thresholds() const
{
   std::vector<TrigConf::L1Threshold> thrlist;
   for ( const std::string & type : thresholdTypes() ) {
      const auto & thrOfType = thresholds(type);
      std::copy(thrOfType.begin(), thrOfType.end(), std::back_inserter(thrlist));
   }
   return thrlist;
}


std::vector<std::string> 
TrigConf::L1Menu::thresholdTypes() const
{
   std::vector<std::string> thrTypeNames;
   thrTypeNames.reserve(m_thresholdsByType.size());
   for( auto x : m_thresholdsByType ) {
      thrTypeNames.emplace_back(x.first);
   }
   return thrTypeNames;
}


std::vector<std::string>
TrigConf::L1Menu::connectorNames() const {
   std::vector<std::string> connNames;
   connNames.reserve(m_connectors.size());
   for( auto & x : m_connectors ) {
      connNames.emplace_back(x.first);
   }
   return connNames;
}

const TrigConf::L1Connector &
TrigConf::L1Menu::connector(const std::string & connectorName) const {
   try {
      return m_connectors.at(connectorName);
   }
   catch(std::exception & ex) {
      std::cerr << "No connector " << connectorName << " found in the L1 menu" << std::endl;
      throw;
   }
}


void
TrigConf::L1Menu::printMenu(bool full) const
{
   cout << "L1 menu '" << name() << "'" << endl;
   cout << "Items: " << size() << endl;
   cout << "Thresholds: " << thresholds().size() << "(of " << thresholdTypes().size() << " different types)" << endl;
   if(full) {
      int c(0);
      for(const std::string & thrType : thresholdTypes() ) {
         cout << thrType << " :" << endl;
         for(auto & thr : thresholds(thrType)) {
            cout << "  " << c++ << ": " << thr.name() << "[" << thr.type() << "]" << endl;
         }
      }
   }
   cout << "Topo algorithms: " << endl;
   cout << "    new   : " << data().get_child("topoAlgorithms.TOPO.decisionAlgorithms").size() << endl;
   cout << "    muon  : " << data().get_child("topoAlgorithms.MUTOPO.decisionAlgorithms").size() << endl;
   cout << "    mult  : " << data().get_child("topoAlgorithms.MULTTOPO.multiplicityAlgorithms").size() << endl;
   cout << "    legacy: " << data().get_child("topoAlgorithms.R2TOPO.decisionAlgorithms").size() << endl;
   cout << "Boards: " << data().get_child("boards").size() << endl;
   cout << "Connectors: " << data().get_child("connectors").size() << endl;
   unsigned int ctpinputs = data().get_child("ctp.inputs.optical").size();
   ctpinputs += data().get_child("ctp.inputs.electrical").size();
   ctpinputs += data().get_child("ctp.inputs.ctpin.slot7").size();
   ctpinputs += data().get_child("ctp.inputs.ctpin.slot8").size();
   ctpinputs += data().get_child("ctp.inputs.ctpin.slot9").size();
   cout << "CTP connections: " << ctpinputs << endl;
   cout << "The L1Menu " << (ownsData() ? "owns" : "does not own") << " the ptree" << endl;
}

