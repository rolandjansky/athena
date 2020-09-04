/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

#include <algorithm>
#include <cmath>


#include "TrigConfData/L1ThresholdBase.h"
#include "TrigConfData/L1Threshold.h"

/**
 *
 *  L1Threshold base class
 *
 **/

unsigned int 
TrigConf::energyInCounts(unsigned int energyMeV, unsigned int energyResolutionMeV)
{
   if ( energyMeV % energyResolutionMeV != 0 ) {
      throw std::runtime_error("Energy " + std::to_string(energyMeV) + " MeV is not a multiple of the energy resolution " + std::to_string(energyResolutionMeV));
   }
   return energyMeV / energyResolutionMeV;
}


std::shared_ptr<TrigConf::L1Threshold>
TrigConf::L1Threshold::createThreshold( const std::string & name, const std::string & type, 
                                        std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data )
{
  if( type == "EM" )
      return std::make_shared<L1Threshold_EM>( name, type, extraInfo, data );
    
   if( type == "TAU" )
      return std::make_shared<L1Threshold_TAU>( name, type, extraInfo, data );

   if( type == "XE" )
      return std::make_shared<L1Threshold_XE>( name, type, extraInfo, data );

   if( type == "JET" )
      return std::make_shared<L1Threshold_JET>( name, type, extraInfo, data );
   
   if( type == "XS" )
      return std::make_shared<L1Threshold_XS>( name, type, extraInfo, data );
   
   if( type == "TE" )
      return std::make_shared<L1Threshold_TE>( name, type, extraInfo, data );

   if( type == "eEM" )
      return std::make_shared<L1Threshold_eEM>( name, type, extraInfo, data );

   if( type == "eTAU" )
      return std::make_shared<L1Threshold_eTAU>( name, type, extraInfo, data );

   if( type == "MU" )
      return std::make_shared<L1Threshold_MU>( name, type, extraInfo, data );

   static const std::string NIMtypes[] = { "BCM", "BCMCMB", "LUCID", "ZDC", "BPTX", "CALREQ", "MBTS", "MBTSSI", "NIM" };
   bool isNIMtype = std::find(std::begin(NIMtypes), std::end(NIMtypes), type) !=
     std::end(NIMtypes);

   if( isNIMtype )
      return std::make_shared<L1Threshold_NIM>( name, type, extraInfo, data );

   if( type == "internal" )
      return std::make_shared<L1Threshold_internal>( name, type, extraInfo, data );

   static const std::string noSpecialImp[] = { "JET", "XS", "jJ", "gXE", "jXE", "TOPO", "MULTTOPO", "MUTOPO", "R2TOPO", "ZB", "ALFA" };
   bool useBaseClass = std::find(std::begin(noSpecialImp), std::end(noSpecialImp),type) !=
     std::end(noSpecialImp);

   if( useBaseClass )
      return std::make_shared<L1Threshold>( name, type, extraInfo, data );
  
   throw std::runtime_error("Threshold " + name + " is not of a valid L1 threshold type: " + type);
  
}

TrigConf::L1Threshold::L1Threshold( const std::string & name, const std::string & type,
                                    std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const boost::property_tree::ptree & data) 
   : DataStructure(data),
     m_extraInfo(extraInfo),
     m_type(type)
{
   m_name = name;
   load();
}

float
TrigConf::L1Threshold::thrValue(int) const {
   return 0;
}

void
TrigConf::L1Threshold::update()
{
   load();
}

void
TrigConf::L1Threshold::load()
{
   if( type() == "internal" ) {
      return;
   }
   m_name = getAttribute("name", true, m_name);
   m_type = getAttribute("type", true, m_type);
   m_mapping = getAttribute<unsigned int>("mapping");
}


TrigConf::L1ThrExtraInfoBase::L1ThrExtraInfoBase(const std::string & thrTypeName, const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   m_name = thrTypeName;
   load();
}

void
TrigConf::L1ThrExtraInfoBase::load()
{
   if(! isInitialized() || empty() )
      return;

   if( m_name == "internal" ) { // internal trigger have no extra info
      return;
   }
   for( auto & content : data() ) {
      if( content.first == "type" ||
          content.first == "thresholds" ) {
         continue;
      }
      // if there is anything else in the tree we consider it extra info fro this threshold type
      m_extraInfo.emplace( std::piecewise_construct,
                           std::forward_as_tuple(content.first),
                           std::forward_as_tuple(content.second));
   }

   m_resolutionMeV = getAttribute<unsigned int>("resolutionMeV", true, 1000);
   if( hasAttribute("emscale") ) { // for previous definition of the EM resolution
      m_resolutionMeV = 1000 / getAttribute<unsigned int>("emscale");
   }
}


const std::string &
TrigConf::L1ThrExtraInfoBase::thresholdTypeName() const
{
   return m_name;
}

bool
TrigConf::L1ThrExtraInfoBase::hasExtraInfo() const
{
   return m_extraInfo.size()>0;
}




/**
 *
 *  L1Threshold_Calo base class
 *
 **/

TrigConf::L1Threshold_Calo::L1Threshold_Calo( const std::string & name, const std::string & type,
                                              std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const boost::property_tree::ptree & data) 
   : L1Threshold(name, type, extraInfo, data),
     m_etaDepThrValue(name + "#" + type + "_value")
{
   load();
}

void
TrigConf::L1Threshold_Calo::update()
{
   L1Threshold::update();
   load();
}

namespace {
   unsigned int gev2MeVThrVal(float gevVal) {
      unsigned int mev_i = std::lround( 1000 * gevVal );
      if( gevVal != (mev_i / 1000.) ) {
         std::runtime_error("Value conversion failed");
      }
      return mev_i;
   }
   
}


void
TrigConf::L1Threshold_Calo::load()
{
   if( type() == "internal" ) {
      return;
   }
   m_thrValue = gev2MeVThrVal( getAttribute<float>("value", true, 0) );
   m_input = getAttribute("input", true, "");
   if( const auto & thrVs = data().get_child_optional("thrValues") ) {
      for( auto & x : thrVs.get() ) {
         auto value = gev2MeVThrVal( x.second.get_child("value").get_value<float>() );
         auto etamin = x.second.get_child("etamin").get_value<unsigned int>();
         auto etamax = x.second.get_child("etamax").get_value<unsigned int>();
         auto priority = x.second.get_child("priority").get_value<unsigned int>();
         m_etaDepThrValue.addRangeValue(value, etamin, etamax, priority, /*symmetric=*/ false);
      }
   }
}

/*
  various access methods to the threshold values
*/

float
TrigConf::L1Threshold_Calo::thrValue(int eta) const
{
   return thrValueMeV(eta) / 1000.0f;
}

unsigned int
TrigConf::L1Threshold_Calo::thrValueCounts(int eta) const {
   auto extraInfo = m_extraInfo.lock();
   return energyInCounts( thrValueMeV(eta), extraInfo->resolutionMeV() );
}

unsigned int
TrigConf::L1Threshold_Calo::thrValueMeV(int eta) const {
   return m_etaDepThrValue.empty() ? m_thrValue : m_etaDepThrValue.at(eta);
}

TrigConf::ValueWithEtaDependence<float>
TrigConf::L1Threshold_Calo::thrValues() const {
   auto thresholdValuesGeV = ValueWithEtaDependence<float>{ m_etaDepThrValue.name()+"Counts" };
   for( auto & r : m_etaDepThrValue ) {
      thresholdValuesGeV.addRangeValue(r.value() / 1000.0f, r.etaMin(), r.etaMax(), r.priority(), r.symmetric());
   }
   return thresholdValuesGeV;
}

const TrigConf::ValueWithEtaDependence<unsigned int> &
TrigConf::L1Threshold_Calo::thrValuesMeV() const {
   return m_etaDepThrValue;
}

TrigConf::ValueWithEtaDependence<unsigned int>
TrigConf::L1Threshold_Calo::thrValuesCounts() const {
   auto extraInfo = m_extraInfo.lock();
   auto thrValuesCounts = ValueWithEtaDependence<unsigned int>{ m_etaDepThrValue.name()+"Counts" };
   for( auto & r : m_etaDepThrValue ) {
      thrValuesCounts.addRangeValue( energyInCounts( r.value(), extraInfo->resolutionMeV() ), r.etaMin(), r.etaMax(), r.priority(), r.symmetric());
   }
   return thrValuesCounts;
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

