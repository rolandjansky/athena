/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
         
         // first create the extraInfo object for this type of thresholds
         auto extraInfo = m_thrExtraInfo.addExtraInfo(thrByType.first,thrByType.second);

         auto & v = m_thresholdsByType[thrType] = std::vector<std::shared_ptr<TrigConf::L1Threshold>>();

         if(thrType == "internal") {
            for( auto & thr : data().get_child( path + ".internal.names") ) {
               const std::string thrName = thr.second.data();
               v.push_back( L1Threshold::createThreshold( thrName, thrType, extraInfo, thr.second ) );
            }
         } else {
            for( auto & thr : data().get_child( path + "." + thrType + ".thresholds") ) {
               const std::string thrName = thr.first;
               v.push_back( L1Threshold::createThreshold( thrName, thrType, extraInfo, thr.second ) );
            }
         }
         for( auto & thr : v ) {
            m_thresholdsByName[ thr->name() ] = thr;
         }

      }
   }

   // boards
   for( auto & board : data().get_child( "boards" ) ) {
      m_boards.emplace( std::piecewise_construct,
                        std::forward_as_tuple(board.first),
                        std::forward_as_tuple(board.first, board.second) );
   }

   // connectors
   for( auto & conn : data().get_child( "connectors" ) ) {
      m_connectors.emplace( std::piecewise_construct,
                            std::forward_as_tuple(conn.first),
                            std::forward_as_tuple(conn.first, conn.second) );
   }

   // algorithms
   for( const std::string & path : { "TOPO", "MULTTOPO", "MUTOPO", "R2TOPO" } ) {
      auto & v = m_algorithmsByType[path] = std::vector<TrigConf::L1TopoAlgorithm>();
      if(path == "MULTTOPO") {
         for( auto & alg : data().get_child( "topoAlgorithms." + path + ".multiplicityAlgorithms" ) ) {
            v.emplace_back( alg.first, L1TopoAlgorithm::AlgorithmType::MULTIPLICITY, alg.second );
         }
      } else {
         for( L1TopoAlgorithm::AlgorithmType algoType : { L1TopoAlgorithm::AlgorithmType::DECISION, L1TopoAlgorithm::AlgorithmType::SORTING } ) {
            std::string subpath = "topoAlgorithms." + path + (algoType==L1TopoAlgorithm::AlgorithmType::DECISION ? ".decisionAlgorithms" : ".sortingAlgorithms" );  
            for( auto & algorithm : data().get_child( subpath ) ) {
               v.emplace_back( algorithm.first, algoType, algorithm.second );
            }
         }
      }
      for( auto & algo : v ) { 
         m_algorithmsByName[ algo.name() ] = & algo;
         for( const std::string & output : algo.outputs() ) {
            m_algorithmsByOutput[output] = & algo;
         }
      }

   }
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

unsigned int
TrigConf::L1Menu::smk() const {
   return m_smk;
}

void
TrigConf::L1Menu::setSMK(unsigned int smk) {
   m_smk = smk;
}

TrigConf::L1Item
TrigConf::L1Menu::item(const std::string & itemName) const
{
   if(itemName=="") {
      throw std::runtime_error("L1Menu::item() was called with empty itemName");
   }
   const ptree * pt;
   try {
      pt = & data().get_child(ptree::path_type("items/"+itemName,'/')); // '/' is not used in any item Name, so we can use it as separator in the path 
   } catch(boost::property_tree::ptree_bad_path & ex) {
      throw std::runtime_error("L1Item " + itemName + " does not exist in the menu");
   }
   return L1Item(*pt);
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
TrigConf::L1Menu::thresholdNames() const
{
   std::vector<std::string> thrNames;
   thrNames.reserve(m_thresholdsByName.size());
   for( auto x : m_thresholdsByName ) {
      thrNames.emplace_back(x.first);
   }
   return thrNames;
}


const std::vector<std::shared_ptr<TrigConf::L1Threshold>> &
TrigConf::L1Menu::thresholds(const std::string & typeName) const
{
   try {
      return m_thresholdsByType.at(typeName);
   }
   catch(std::exception & ex) {
      std::cerr << "No threshold type '" << typeName << "' defined in the thresholds section of the L1 menu" << std::endl;
      throw;
   }
}


std::vector<std::shared_ptr<TrigConf::L1Threshold>> 
TrigConf::L1Menu::thresholds() const
{
   std::vector<std::shared_ptr<TrigConf::L1Threshold>> thrlist;
   for ( const std::string & type : thresholdTypes() ) {
      const auto & thrOfType = thresholds(type);
      std::copy(thrOfType.begin(), thrOfType.end(), std::back_inserter(thrlist));
   }
   return thrlist;
}


const TrigConf::L1Threshold &
TrigConf::L1Menu::threshold(const std::string & thresholdName) const
{
   try {
      return * m_thresholdsByName.at(thresholdName);
   }
   catch(std::exception & ex) {
      std::cerr << "No threshold '" << thresholdName << "' defined in the thresholds section of the L1 menu" << std::endl;
      throw;
   }
}

const TrigConf::L1ThrExtraInfo &
TrigConf::L1Menu::thrExtraInfo() const
{
   return m_thrExtraInfo;
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
      std::cerr << "No connector " << connectorName << " defined in the L1 menu" << std::endl;
      throw;
   }
}

const TrigConf::L1TopoAlgorithm & 
TrigConf::L1Menu::algorithm(const std::string & algoName) const
{
   try {
      return * m_algorithmsByName.at(algoName);
   }
   catch(std::exception & ex) {
      std::cerr << "No algorithm " << algoName << " defined in the L1 menu" << std::endl;
      throw;
   }
}

const TrigConf::L1TopoAlgorithm &
TrigConf::L1Menu::algorithmFromOutput(const std::string & outputName) const
{
   try {
      return * m_algorithmsByOutput.at(outputName);
   }
   catch(std::exception & ex) {
      std::cerr << "No output '" << outputName << "' defined by any algorithm in the L1 menu" << std::endl;
      throw;
   }
}

const TrigConf::L1Board &
TrigConf::L1Menu::board(const std::string & boardName) const
{
   try {
      return m_boards.at(boardName);
   }
   catch(std::exception & ex) {
      std::cerr << "No board " << boardName << " defined in the L1 menu" << std::endl;
      throw;
   }
}

std::vector<std::string>
TrigConf::L1Menu::boardNames() const
{
   std::vector<std::string> boardNames;
   boardNames.reserve(m_boards.size());
   for(auto & board : m_boards) {
      boardNames.emplace_back(board.first);
   }
   return boardNames;
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
            cout << "  " << c++ << ": " << thr->name() << "[" << thr->type() << "]" << endl;
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
}

