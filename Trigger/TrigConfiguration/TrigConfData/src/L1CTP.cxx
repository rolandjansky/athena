/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1CTP.h"

TrigConf::L1CTP::L1CTP(const std::string & name, const boost::property_tree::ptree & data) 
   : DataStructure(name,data)
{
   load();
}

void
TrigConf::L1CTP::load()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   auto inputs = data().get_child("inputs");
   for(size_t slot = 7; slot<=9; ++slot) {
      for(size_t conn=0; conn<4; ++conn) {
         m_ctpin[slot-7][conn] = inputs.get_optional<std::string>("ctpin.slot" + std::to_string(slot) + ".connector" + std::to_string(conn)).get_value_or("");
      }
   }
   auto electrical = inputs.get_child("electrical");
   for(size_t i=0; i<3; ++i) {
      m_electrical[i] = electrical.get_optional<std::string>("connector" + std::to_string(i)).get_value_or("");
   }
   if(auto optical = inputs.get_child_optional("optical")) {
      for(size_t i=0; i<12; ++i) {
         m_optical[i] = optical->get_optional<std::string>("connector" + std::to_string(i)).get_value_or("");
      }
   }
   for( auto & mon : data().get_child("monitoring.ctpmon") ) {
      std::string monName = mon.first;
      size_t multiplicity = mon.second.get_child("multiplicity").get_value<size_t>();
      std::string thr = mon.second.get_child("thr").get_value<std::string>();
      m_ctpmon.emplace( std::piecewise_construct,
                        std::forward_as_tuple(monName),
                        std::forward_as_tuple(multiplicity, thr)
                        );
   }
}

void
TrigConf::L1CTP::clear()
{
   m_ctpmon.clear();
}

const std::string &
TrigConf::L1CTP::ctpin(size_t slot, size_t conn) const
{
   if(slot<7 or slot>9) {
      throw std::runtime_error("CTPIN slot must be between 7 and 9, but " + std::to_string(slot) + "was specified");
   }
   if(conn>3) {
      throw std::runtime_error("CTPIN connector must be between 0 and 3, but " + std::to_string(conn) + "was specified");
   }
   return m_ctpin[slot-7][conn];
}


const std::string &
TrigConf::L1CTP::electrical(size_t conn) const {
   if(conn>2) {
      throw std::runtime_error("Electrical connector must be between 0 and 2, but " + std::to_string(conn) + "was specified");
   }
   return m_electrical[conn];
}

const std::string &
TrigConf::L1CTP::optical(size_t conn) const {
   if(conn>11) {
      throw std::runtime_error("Optical connector must be between 0 and 11, but " + std::to_string(conn) + "was specified");
   }
   return m_optical[conn];
}
