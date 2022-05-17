/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTPrescalesSet.h"

TrigConf::HLTPrescalesSet::HLTPrescalesSet()
{}

TrigConf::HLTPrescalesSet::HLTPrescalesSet(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   load();
}

void
TrigConf::HLTPrescalesSet::load()
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

      // repeat for express stream prescale
      HLTPrescale ps_express;
      // checks if contains express prescale in chain from prescale file/DB entry
      // does not check against menu definition, check to be done when used elsewhere
      if(p.second.get_optional<double>("prescale_express")){
         double prescaleValue_express = p.second.get_child("prescale_express").get_value<double>();
         ps_express.prescale = prescaleValue_express < 0 ? -prescaleValue_express : prescaleValue_express;
         ps_express.enabled = (prescaleValue_express > 0);
         boost::optional<bool> enabledField_express = p.second.get_optional<bool>("enabled_express");
         if( enabledField_express ) {
            ps_express.enabled = *enabledField_express;
         }
         // store in map by name
         m_prescales_express[p.first] = ps_express;

         // store in maps by hash
         m_prescalesByHash_express[namehash] = ps_express;
      }
   }
}

void
TrigConf::HLTPrescalesSet::clear()
{
   m_psk = 0;
   m_prescales.clear();
   m_prescalesByHash.clear();
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


const TrigConf::HLTPrescalesSet::HLTPrescale &
TrigConf::HLTPrescalesSet::prescale_express(const std::string & chainName) const {
    auto search = m_prescales_express.find(chainName);
    if (search != m_prescales_express.end()) {
        return search->second;
    } else {
        return m_notInExpress;
    }
}

const TrigConf::HLTPrescalesSet::HLTPrescale &
TrigConf::HLTPrescalesSet::prescale_express(uint32_t chainHash) const {
    auto search = m_prescalesByHash_express.find(chainHash);
    if (search != m_prescalesByHash_express.end()) {
        return search->second;
    } else {
        return m_notInExpress;
    }
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

