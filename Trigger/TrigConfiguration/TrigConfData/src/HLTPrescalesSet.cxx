/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTPrescalesSet.h"

TrigConf::HLTPrescalesSet::HLTPrescalesSet()
{}

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
      HLTPrescale ps;
      ps.name = p.first;
      ps.namehash = p.second.get_child("hash").get_value<uint32_t>();
      ps.prescale = p.second.get_child("prescale").get_value<double>();
      m_prescales[p.first] = ps;
      m_prescalesByHash[ps.namehash] = ps;
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
