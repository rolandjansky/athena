/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTPrescalesSet.h"

TrigConf::HLTPrescalesSet::HLTPrescalesSet()
{}

TrigConf::HLTPrescalesSet::HLTPrescalesSet(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   update();
}

TrigConf::HLTPrescalesSet::~HLTPrescalesSet() = default;

void
TrigConf::HLTPrescalesSet::update()
{
   m_name = getAttribute("name");
   const auto & prescales = data().get_child("prescales");
   for( auto & p : prescales ) {

      double prescaleValue = p.second.get_child("prescale").get_value<double>();
      HLTPrescale ps;
      ps.prescale = prescaleValue < 0 ? -prescaleValue : prescaleValue;
      ps.enabled = (prescaleValue > 0);
      boost::optional<bool> enabledField = p.second.get_optional<bool>("enabled");
      if( enabledField ) {
         ps.enabled = *enabledField;
      }

      // store in map by name
      m_prescales[p.first] = ps;

      // store in map by hash
      uint32_t namehash = p.second.get_child("hash").get_value<uint32_t>();
      m_prescalesByHash[namehash] = ps;
   }
}


std::size_t 
TrigConf::HLTPrescalesSet::size() const
{
   return m_prescales.size();
}

unsigned int
TrigConf::HLTPrescalesSet::psk() const {
   return m_psk;
}

void
TrigConf::HLTPrescalesSet::setPSK(unsigned int psk ) {
   m_psk = psk;
}

const TrigConf::HLTPrescalesSet::HLTPrescale & 
TrigConf::HLTPrescalesSet::prescale(const std::string & chainName) const {
   return m_prescales.at(chainName);
}

const TrigConf::HLTPrescalesSet::HLTPrescale &
TrigConf::HLTPrescalesSet::prescale(uint32_t chainHash) const {
   return m_prescalesByHash.at(chainHash);
}


void
TrigConf::HLTPrescalesSet::printPrescaleSet(bool full) const
{
   std::cout << "Name '" << name() << "'" << std::endl;
   std::cout << "Prescales: " << size() << std::endl;
   if(full) {
      int c(0);
      for( auto & ps : m_prescales ) {
         std::cout << "  " << c++ << ": " << ps.first << " -> " << ps.second.prescale << (ps.second.enabled ? "" : " (disabled)" ) << std::endl;
      }
   }
}

