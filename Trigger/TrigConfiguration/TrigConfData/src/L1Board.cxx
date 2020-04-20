/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Board.h"

#include <stdexcept>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

TrigConf::L1Board::L1Board()
{}

TrigConf::L1Board::L1Board(const std::string & boardName, const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   m_name = boardName;
   update();
}

TrigConf::L1Board::~L1Board()
{}

std::string
TrigConf::L1Board::className() const {
   return "L1Board";
}

void
TrigConf::L1Board::update()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name", true, m_name);
   m_legacy = getAttribute<bool>("legacy", true, false); // if legacy flag doesn't exist, it is considered to be false

   std::string boardType(getAttribute("type"));
   if( boardType == "MUCTPI" ) {
      m_type = BoardType::MUCTPI;      
   } else if( boardType == "CTPIN" ) {
      m_type = BoardType::CTPIN;
   } else if( boardType == "TOPO" ) {
      m_type = BoardType::TOPO;
   } else {
      throw std::runtime_error("Unknown board type " + boardType);
   }

   // connectors
   for( auto & conn : getList("connectors") ) {
      m_connectorNames.push_back(conn.getValue<std::string>());
   }
}


std::size_t
TrigConf::L1Board::size() const
{
   return m_connectorNames.size();
}


bool
TrigConf::L1Board::legacy() const 
{
   return m_legacy;
}

const std::vector<std::string> &
TrigConf::L1Board::connectorNames() const
{
   return m_connectorNames;
}
