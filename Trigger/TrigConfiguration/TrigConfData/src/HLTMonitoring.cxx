/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTMonitoring.h"

using TV = boost::property_tree::ptree::value_type;  // tree-value type
using namespace std;

TrigConf::HLTMonitoring::HLTMonitoring()
{}

TrigConf::HLTMonitoring::HLTMonitoring(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   load();
}

TrigConf::HLTMonitoring::~HLTMonitoring()
{}

void
TrigConf::HLTMonitoring::load()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name");
}

void
TrigConf::HLTMonitoring::clear()
{
   m_smk = 0;
   m_name = "";
}

unsigned int
TrigConf::HLTMonitoring::smk() const {
   return m_smk;
}

void
TrigConf::HLTMonitoring::setSMK(unsigned int smk) {
   m_smk = smk;
}
