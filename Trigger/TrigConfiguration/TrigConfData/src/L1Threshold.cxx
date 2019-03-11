/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Threshold.h"

TrigConf::L1Threshold::L1Threshold()
{}

TrigConf::L1Threshold::L1Threshold(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::L1Threshold::~L1Threshold()
{}

const std::string &
TrigConf::L1Threshold::name() const
{
   return data().get_child("name").data();
}

const std::string &
TrigConf::L1Threshold::type() const
{
   return data().get_child("type").data();
}

const std::string &
TrigConf::L1Threshold::input() const
{
   return data().get_child("input").data();
}

unsigned int
TrigConf::L1Threshold::mapping() const
{
   return data().get_child("mapping").get_value<unsigned int>();
}

unsigned int
TrigConf::L1Threshold::id() const
{
   return data().get_child("id").get_value<unsigned int>();
}

std::vector<TrigConf::DataStructure>
TrigConf::L1Threshold::thresholdValues() const
{
   std::vector<DataStructure> thrVlist;

   if( const auto & thrVs = m_data.get_child_optional("thresholdValues") ) {

      thrVlist.reserve(thrVs.get().size());
      
      for( auto & thrVData : thrVs.get() )
         thrVlist.emplace_back( thrVData.second );
   }
   
   return thrVlist;
}

TrigConf::DataStructure
TrigConf::L1Threshold::cable() const
{
   bool ignoreIfMissing = true; 
   return getObject("cable", ignoreIfMissing);
}
