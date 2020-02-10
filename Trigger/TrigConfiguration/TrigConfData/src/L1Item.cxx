/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Item.h"

TrigConf::L1Item::L1Item()
{}

TrigConf::L1Item::L1Item(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   update();
}

TrigConf::L1Item::~L1Item()
{}

void
TrigConf::L1Item::update()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   for(auto & bg : getList("bunchgroups") ) {
      m_bunchgroups.push_back(bg.getValue<std::string>());
   }
}

const std::string &
TrigConf::L1Item::name() const
{
   return getAttribute("name");
}

unsigned int
TrigConf::L1Item::ctpId() const
{
   return getAttribute<unsigned int>("ctpid");
}

const std::string &
TrigConf::L1Item::definition() const
{
   return getAttribute("definition");
}

const std::vector<std::string> &
TrigConf::L1Item::bunchgroups() const {
   return m_bunchgroups;
}

unsigned int
TrigConf::L1Item::complexDeadtime() const
{
   return getAttribute<unsigned int>("complexDeadtime");
}

const std::string &
TrigConf::L1Item::monitor() const
{
   return getAttribute("monitor");
}

unsigned int
TrigConf::L1Item::partition() const
{
   return getAttribute<unsigned int>("partition");
}

const std::string &
TrigConf::L1Item::triggerType() const
{
   return getAttribute("triggerType");
}

unsigned char
TrigConf::L1Item::triggerTypeAsUChar() const
{
   std::string tt ( getAttribute("triggerType") );
   unsigned char ttuc(0);
   for(size_t p = 0; p<tt.size(); p++) {
      ttuc <<= 1;
      if(tt[p]=='1') ttuc += 1;
   }
   return ttuc;
}

TrigConf::DataStructure
TrigConf::L1Item::logic() const
{
   return getObject("AND");
}
