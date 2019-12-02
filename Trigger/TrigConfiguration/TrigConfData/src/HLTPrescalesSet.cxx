/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTPrescalesSet.h"

TrigConf::HLTPrescalesSet::HLTPrescalesSet()
{}

TrigConf::HLTPrescalesSet::HLTPrescalesSet(const TrigConf::HLTPrescalesSet & o)
   : DataStructure(o.m_data)
{
   update();
}

TrigConf::HLTPrescalesSet::HLTPrescalesSet(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   update();
}

TrigConf::HLTPrescalesSet::~HLTPrescalesSet()
{}

void
TrigConf::HLTPrescalesSet::update()
{
   m_name = m_data.get_child("name").get_value<std::string>();
   const auto & prescales = m_data.get_child("prescales");
   for( auto & p : prescales ) {

      double prescaleValue = p.second.get_child("prescale").get_value<double>();
      HLTPrescale ps;
      ps.prescale = prescaleValue < 0 ? -prescaleValue : prescaleValue;
      ps.enabled = (prescaleValue > 0);
      boost::optional<bool> enabledField = p.second.get_optional<bool>("enabled");
      if( ! enabledField ) {
         ps.enabled = *enabledField;
      }

      // store in map by name
      m_prescales[p.first] = ps;

      // store in map by hash
      uint32_t namehash = p.second.get_child("hash").get_value<uint32_t>();
      m_prescalesByHash[namehash] = ps;
   }
}

std::string
TrigConf::HLTPrescalesSet::name() const
{
   return m_name;
}

std::size_t 
TrigConf::HLTPrescalesSet::size() const
{
   return m_prescales.size();
}

const TrigConf::HLTPrescalesSet::HLTPrescale & 
TrigConf::HLTPrescalesSet::prescale(const std::string & chainName) const {
   return m_prescales.at(chainName);
}

const TrigConf::HLTPrescalesSet::HLTPrescale &
TrigConf::HLTPrescalesSet::prescale(uint32_t chainHash) const {
   return m_prescalesByHash.at(chainHash);
}
