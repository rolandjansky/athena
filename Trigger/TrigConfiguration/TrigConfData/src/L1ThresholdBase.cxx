/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

#include "TrigConfData/L1Threshold.h"

std::shared_ptr<TrigConf::L1Threshold>
TrigConf::L1Threshold::createThreshold( const std::string & name, const std::string & type, 
                                        std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data )
{
   if( type == "EM" ) {
      return std::make_shared<L1Threshold_EM>( name, type, extraInfo, data );
   } else if( type == "TAU" ) {
      return std::make_shared<L1Threshold_TAU>( name, type, extraInfo, data );
   } else if( type == "eEM" ) {
      return std::make_shared<L1Threshold_eEM>( name, type, extraInfo, data );
   } else if( type == "MU" ) {
      return std::make_shared<L1Threshold_MU>( name, type, extraInfo, data );
   } else {
      return std::make_shared<L1Threshold>( name, type, extraInfo, data );
   }
}

TrigConf::L1Threshold::L1Threshold(const std::string & name, const std::string & type, std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const boost::property_tree::ptree & data) 
   : DataStructure(data),
     m_extraInfo(extraInfo),
     m_etaDepThrValue(name + "#" + type + "_value"),
     m_type(type)
{
   m_name = name;
   update();
}

void
TrigConf::L1Threshold::update()
{
   if( type() == "internal" ) {
      return;
   }
   m_name = getAttribute("name", true, m_name);
   m_type = getAttribute("type", true, m_type);
   m_thrValue = getAttribute<unsigned int>("value", true, 0);
   m_input = getAttribute("input", true, "");
   m_mapping = getAttribute<unsigned int>("mapping");
   if( const auto & thrVs = data().get_child_optional("thrValues") ) {
      for( auto & x : thrVs.get() ) {
         auto value = x.second.get_child("value").get_value<unsigned int>();
         auto etamin = x.second.get_child("etamin").get_value<unsigned int>();
         auto etamax = x.second.get_child("etamax").get_value<unsigned int>();
         auto priority = x.second.get_child("priority").get_value<unsigned int>();
         m_etaDepThrValue.addRangeValue(value, etamin, etamax, priority, /*symmetric=*/ false);
      }
   }
}

unsigned int
TrigConf::L1Threshold::thrValue(int eta) const
{
   if( m_etaDepThrValue.empty() ) {
      return m_thrValue;
   } else {
      return m_etaDepThrValue.at(eta);
   }
}

unsigned int
TrigConf::L1Threshold::thrValueCounts(int eta) const {
   return thrValue(eta);
}



TrigConf::IsolationLegacy::IsolationLegacy( const boost::property_tree::ptree & pt ) {
   m_isDefined = true;
   m_isobit = pt.get_child("isobit").get_value<int>();
   m_offset = pt.get_child("offset").get_value<int>();
   m_slope =  pt.get_child("slope").get_value<int>();
   m_mincut =  pt.get_child("mincut").get_value<int>();
   m_upperlimit = pt.get_child("upperlimit").get_value<int>();
   m_etamin =  pt.get_child("etamin").get_value<int>();
   m_etamax =  pt.get_child("etamax").get_value<int>();
   m_priority =  pt.get_child("priority").get_value<int>();
}

std::ostream &
TrigConf::operator<<(std::ostream & os, const TrigConf::IsolationLegacy & iso) {
   os << "isolation bit " << iso.isobit() << ":  offset=" << iso.offset() << ", slope=" << iso.slope() 
      << ", mincut=" << iso.mincut() << ", upperlimit=" << iso.upperlimit() 
      << ", etamin=" << iso.etamin() << ", etamax=" << iso.etamax() << ", priority=" << iso.priority();
   return os;
}

TrigConf::Isolation::Isolation( const boost::property_tree::ptree & pt ) {
   m_isDefined = true;
   m_reta = pt.get_child("reta").get_value<int>();
   m_wstot = pt.get_child("wstot").get_value<int>();
   m_had = pt.get_child("had").get_value<int>();
}

std::ostream &
TrigConf::operator<<(std::ostream & os, const TrigConf::Isolation & iso) {
   os << "reta=" << iso.reta() << ", wstot=" << iso.wstot() << ", had=" << iso.had();
   return os;
}

