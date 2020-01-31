/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Connector.h"
using namespace std;

#include <stdexcept>

TrigConf::L1Connector::L1Connector()
{}

TrigConf::L1Connector::L1Connector(const std::string & connName, const boost::property_tree::ptree & data) 
   : DataStructure(data),
     m_name(connName)
{
   update();
}

TrigConf::L1Connector::~L1Connector()
{}

void
TrigConf::L1Connector::update()
{
   m_name = getAttribute("name", true, m_name);

   std::string connType(getAttribute("type"));
   if( connType == "electrical" ) {
      m_type = ConnectorType::ELECTRICAL;      
   } else if( connType == "optical" ) {
      m_type = ConnectorType::OPTICAL;
   } else if( connType == "ctpin" ) {
      m_type = ConnectorType::CTPIN;
   } else {
      throw std::runtime_error("Unknown connector type " + connType);
   }

   // triggerlines
   size_t loopsize = (m_type == ConnectorType::ELECTRICAL) ? 2 : 1;

   for( size_t fpga = 0; fpga < loopsize; ++fpga ) {
      for( size_t clock = 0; clock < loopsize; ++clock ) {
         std::string path = "triggerlines";
         if( m_type == ConnectorType::ELECTRICAL ) {
            path += ".fpga";
            path += std::to_string(fpga);
            path += ".clock";
            path += std::to_string(clock);            
         }
         const auto & triggerlines = data().get_child(path);
         m_triggerLines[fpga][clock].reserve(triggerlines.size());
         for( auto & tl : triggerlines ) {
            const std::string & name = tl.second.get_child("name").data();
            m_triggerLines[fpga][clock].emplace_back( name,
                                                      tl.second.get_child("startbit").get_value<unsigned int>(),
                                                      tl.second.get_child("nbits").get_value<unsigned int>(),
                                                      fpga, clock);
            m_lineByName[name] = & m_triggerLines[fpga][clock].back();
         }
      }
   }   
}

const std::string &
TrigConf::L1Connector::name() const
{
   return m_name;
}

TrigConf::L1Connector::ConnectorType
TrigConf::L1Connector::type() const 
{
   return m_type;
}

std::size_t
TrigConf::L1Connector::size() const
{
   return m_triggerLines[0][0].size() + m_triggerLines[0][1].size() + m_triggerLines[1][0].size() + m_triggerLines[1][1].size();
}

const std::vector<TrigConf::TriggerLine> &
TrigConf::L1Connector::triggerLines(unsigned int fpga, unsigned int clock) const
{
   return m_triggerLines[fpga][clock];
}

bool
TrigConf::L1Connector::hasLine( const std::string & lineName ) const
{
   return m_lineByName.count(lineName);
}

const TrigConf::TriggerLine & 
TrigConf::L1Connector::triggerLine( const std::string & lineName ) const
{
   return *m_lineByName.at(lineName);
}
