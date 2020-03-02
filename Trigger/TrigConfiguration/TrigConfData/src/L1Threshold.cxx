/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Threshold.h"

TrigConf::L1Threshold::L1Threshold()
{}

TrigConf::L1Threshold::L1Threshold(const std::string & name, const std::string & type, const boost::property_tree::ptree & data) 
   : DataStructure(data),
     m_name(name),
     m_type(type)
{
   update();
}

TrigConf::L1Threshold::~L1Threshold()
{}

void
TrigConf::L1Threshold::update()
{
   m_name = getAttribute("name", true, m_name);
   m_type = getAttribute("type", true, m_type);
   m_mapping = getAttribute("mapping", true, m_mapping);
}

const std::string &
TrigConf::L1Threshold::name() const
{
   return m_name;
}

const std::string &
TrigConf::L1Threshold::type() const
{
   return m_type;
}

const std::string &
TrigConf::L1Threshold::input() const
{
   return getAttribute("input");
}

unsigned int
TrigConf::L1Threshold::mapping() const
{
   return m_mapping;
}

unsigned int
TrigConf::L1Threshold::id() const
{
   return getAttribute<unsigned int>("id");
}

std::vector<TrigConf::DataStructure>
TrigConf::L1Threshold::thresholdValues() const
{
   std::vector<DataStructure> thrVlist;
   if( const auto & thrVs = data().get_child_optional("thresholdValues") ) {
      thrVlist.reserve(thrVs.get().size());
      for( auto & thrVData : thrVs.get() )
         thrVlist.emplace_back( thrVData.second );
   }
   return thrVlist;
}


TrigConf::DataStructure
TrigConf::L1Threshold::thresholdValue(int eta) const
{
   int current_prio = -1;
   DataStructure thrV;
   if( const auto & thrVs = data().get_child_optional("thresholdValues") ) {

      for( auto & thrVData : thrVs.get() ) {
         auto prio = thrVData.second.get_child("priority").get_value<int>();
         auto etamin = thrVData.second.get_child("etamin").get_value<int>();
         auto etamax = thrVData.second.get_child("etamax").get_value<int>();
         if(eta>etamax or eta<etamin) // outside the window
            continue;
         if(prio < current_prio)
            continue;
         if(prio == current_prio) {
            throw std::runtime_error("Found two threshold values of the same priority for eta = " + std::to_string(eta));
         }
         thrV.setData(thrVData.second);
      }
   }
   return thrV;
}


TrigConf::DataStructure
TrigConf::L1Threshold::cable() const
{
   bool ignoreIfMissing = true; 
   return getObject("cable", ignoreIfMissing);
}
