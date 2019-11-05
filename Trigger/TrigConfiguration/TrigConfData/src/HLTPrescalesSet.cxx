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
   const auto & prescales = m_data.get_child("prescales");
   for( auto & p : prescales ) {
      HLTPrescale ps;
      ps.name = p.first;
      ps.namehash = p.second.get_child("hash").get_value<uint32_t>();
      ps.prescale = p.second.get_child("prescale").get_value<double>();
      m_prescales[p.first] = ps;
      m_prescalesByHash[ps.namehash] = ps;
   }
   for( auto & entry : m_prescales ) {
      std::cout << entry.first << "  =>  " << entry.second.prescale << std::endl;
   }
}

std::size_t 
TrigConf::HLTPrescalesSet::size() const
{
   return m_prescales.size();
}

const std::map<std::string, TrigConf::HLTPrescalesSet::HLTPrescale> & 
TrigConf::HLTPrescalesSet::prescales() const {
   return m_prescales;
}


const std::map<uint32_t, TrigConf::HLTPrescalesSet::HLTPrescale> &
TrigConf::HLTPrescalesSet::prescalesByHash() const {
   return m_prescalesByHash;
}
