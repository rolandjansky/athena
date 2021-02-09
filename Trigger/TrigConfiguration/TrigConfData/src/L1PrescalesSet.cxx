/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1PrescalesSet.h"

TrigConf::L1PrescalesSet::L1PrescalesSet()
{}

TrigConf::L1PrescalesSet::L1PrescalesSet(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   update();
}

TrigConf::L1PrescalesSet::~L1PrescalesSet()
{}

void
TrigConf::L1PrescalesSet::update()
{
   m_name = getAttribute("name");
   const auto & cuts = data().get_child("cutValues");
   for( auto & c : cuts ) {
      L1Prescale ps;
      ps.cut = c.second.get_child("cut").get_value<int32_t>();
      ps.enabled = c.second.get_child("enabled").get_value<bool>();
      ps.prescale = getPrescaleFromCut(ps.cut);
      m_prescales[c.first] = ps;
   }
}

std::size_t 
TrigConf::L1PrescalesSet::size() const
{
   return m_prescales.size();
}

unsigned int
TrigConf::L1PrescalesSet::psk() const {
   return m_psk;
}

void
TrigConf::L1PrescalesSet::setPSK(unsigned int psk ) {
   m_psk = psk;
}

const TrigConf::L1PrescalesSet::L1Prescale & 
TrigConf::L1PrescalesSet::prescale(const std::string & itemName) const {
   return m_prescales.at(itemName);
}
const std::map<std::string, TrigConf::L1PrescalesSet::L1Prescale>&
TrigConf::L1PrescalesSet::prescales() const {
   return m_prescales;
}
/**
   prescale = 2*24/(cut+1.)
   
   cut = 2*24-1  --> prescale =     1
   cut = 2*23-1  --> prescale =     2
   cut = 1677720 --> prescale =    10.000002980233305
   cut = 2*14-1  --> prescale =  1024
   cut = 335543  --> prescale =    50.0000447035
   cut = 33553   --> prescale =   500.006407582 
   cut = 3354    --> prescale =  5000.66020864  
   cut = 334     --> prescale = 50081.238806    
*/
double
TrigConf::L1PrescalesSet::getPrescaleFromCut(uint32_t cut) const {
   return 0xFFFFFF / ( 0x1000000 - cut );
}

