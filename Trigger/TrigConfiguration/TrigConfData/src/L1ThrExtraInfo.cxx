/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1ThrExtraInfo.h"

#include <boost/lexical_cast.hpp>
#include <stdexcept>

using namespace std;

std::unique_ptr<TrigConf::L1ThrExtraInfoBase>
TrigConf::L1ThrExtraInfo::createExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data) {
   std::unique_ptr<TrigConf::L1ThrExtraInfoBase> extraInfo(nullptr);
   if( thrTypeName == "EM"  ) {
      extraInfo = std::make_unique<L1ThrExtraInfo_EMTAULegacy>(thrTypeName, data);
   } else if( thrTypeName == "JET" ) {
      extraInfo = std::make_unique<L1ThrExtraInfo_JETLegacy>(thrTypeName, data);
   } else if( thrTypeName == "MU" ) {
      extraInfo = std::make_unique<L1ThrExtraInfo_MU>(thrTypeName, data);
   } else if( thrTypeName == "TAU" ) {
      extraInfo = std::make_unique<L1ThrExtraInfo_EMTAULegacy>(thrTypeName, data);      
   } else if( thrTypeName == "eEM" or thrTypeName == "eTAU" ) {
      extraInfo = std::make_unique<L1ThrExtraInfo_eEMTAU>(thrTypeName, data);
   } else if( thrTypeName == "jJ" ) {
      extraInfo = std::make_unique<L1ThrExtraInfo_jJ>(thrTypeName, data);      
   } else {
      L1ThrExtraInfoBase x(thrTypeName, data);
      if( x.hasExtraInfo() ) {
         extraInfo = std::make_unique<L1ThrExtraInfoBase>(std::move(x));
      }
   }
   return extraInfo;
}

std::weak_ptr<TrigConf::L1ThrExtraInfoBase>
TrigConf::L1ThrExtraInfo::addExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data) {
   if( auto extraInfo = L1ThrExtraInfo::createExtraInfo( thrTypeName, data) ) {
      auto success = m_thrExtraInfo.emplace(thrTypeName, std::shared_ptr<TrigConf::L1ThrExtraInfoBase>(std::move(extraInfo)));
      return std::weak_ptr<TrigConf::L1ThrExtraInfoBase>( success.first->second );
   };
   return std::weak_ptr<TrigConf::L1ThrExtraInfoBase>( m_emptyInfo );
}

bool
TrigConf::L1ThrExtraInfo::hasInfo(const std::string & typeName) const
{
   return ( m_thrExtraInfo.find(typeName) != m_thrExtraInfo.end() );
}

const TrigConf::L1ThrExtraInfo_EMTAULegacy &
TrigConf::L1ThrExtraInfo::EM() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_EMTAULegacy&>( * m_thrExtraInfo.at("EM") );
}

const TrigConf::L1ThrExtraInfo_EMTAULegacy &
TrigConf::L1ThrExtraInfo::TAU() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_EMTAULegacy&>( * m_thrExtraInfo.at("TAU") );
}

const TrigConf::L1ThrExtraInfo_JETLegacy &
TrigConf::L1ThrExtraInfo::JET() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_JETLegacy&>( * m_thrExtraInfo.at("JET") );
}

const TrigConf::L1ThrExtraInfo_eEMTAU &
TrigConf::L1ThrExtraInfo::eEM() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_eEMTAU&>( * m_thrExtraInfo.at("eEM") );
}

const TrigConf::L1ThrExtraInfo_eEMTAU &
TrigConf::L1ThrExtraInfo::eTAU() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_eEMTAU&>( * m_thrExtraInfo.at("eTAU") );
}

const TrigConf::L1ThrExtraInfo_jJ &
TrigConf::L1ThrExtraInfo::jJ() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_jJ&>( * m_thrExtraInfo.at("jJ") );
}

const TrigConf::L1ThrExtraInfo_jTAU &
TrigConf::L1ThrExtraInfo::jTAU() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_jTAU&>( * m_thrExtraInfo.at("jTAU") );
}

const TrigConf::L1ThrExtraInfo_gXE &
TrigConf::L1ThrExtraInfo::gXE() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_gXE&>( * m_thrExtraInfo.at("gXE") );
}

const TrigConf::L1ThrExtraInfo_MU &
TrigConf::L1ThrExtraInfo::MU() const {
   return dynamic_cast<const TrigConf::L1ThrExtraInfo_MU&>( * m_thrExtraInfo.at("MU") );
}

const TrigConf::L1ThrExtraInfoBase &
TrigConf::L1ThrExtraInfo::thrExtraInfo(const std::string & thrTypeName) const
{
   try {
      return * m_thrExtraInfo.at(thrTypeName);
   }
   catch(std::exception & ex) {
      std::cerr << "Threshold type " << thrTypeName << " does not have extra info defined" << endl;
      throw;
   }
}





TrigConf::L1ThrExtraInfoBase::L1ThrExtraInfoBase(const std::string & thrTypeName, const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   m_name = thrTypeName;
    update();
}

TrigConf::L1ThrExtraInfoBase::~L1ThrExtraInfoBase()
{}

std::string
TrigConf::L1ThrExtraInfoBase::className() const {
   return "L1ThrExtraInfoBase";
}

void
TrigConf::L1ThrExtraInfoBase::update()
{
   if(! isInitialized() || empty() ) {
      return;
   }
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
 * EM legacy extra info
 */
TrigConf::L1ThrExtraInfo_EMTAULegacy::L1ThrExtraInfo_EMTAULegacy(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_EMTAULegacy::~L1ThrExtraInfo_EMTAULegacy()
{}

std::string
TrigConf::L1ThrExtraInfo_EMTAULegacy::className() const {
   return "L1ThrExtraInfo_EMTAULegacy";
}

unsigned int
TrigConf::L1ThrExtraInfo_EMTAULegacy::emScale() const {
   return m_emScale;
}

unsigned int
TrigConf::L1ThrExtraInfo_EMTAULegacy::ptMinToTopo() const {
   return m_ptMinToTopo;
}

const TrigConf::IsolationLegacy &
TrigConf::L1ThrExtraInfo_EMTAULegacy::isolation(const std::string & thrType, size_t bit) const
{
   try {
      return m_isolation.at(thrType)[bit-1];
   }
   catch(std::exception & ex) {
      std::cerr << "Threshold type EM does not have isolation parameters for type " << thrType << endl;
      throw;
   }
}

void
TrigConf::L1ThrExtraInfo_EMTAULegacy::update()
{
   for( auto & x : m_extraInfo ) {
      if( x.first == "emscale" ) {
         m_emScale = x.second.getValue<unsigned int>();
      } else if( x.first == "ptMinToTopo" ) {
         m_ptMinToTopo = x.second.getValue<unsigned int>();
      } else if( x.first == "isolation" ) {
         for( auto & y : x.second.data() ) {
            auto & isoV = m_isolation[y.first] = std::vector<IsolationLegacy>();
            isoV.resize(5);
            for(auto & c : y.second.get_child("Parametrization") ) {
               auto iso = IsolationLegacy(c.second);
               isoV[iso.isobit()-1] = iso;
            }
         }
      }
   }
}



/**
 * JET legacy extra info
 */
TrigConf::L1ThrExtraInfo_JETLegacy::L1ThrExtraInfo_JETLegacy(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_JETLegacy::~L1ThrExtraInfo_JETLegacy()
{}

unsigned int
TrigConf::L1ThrExtraInfo_JETLegacy::jetScale() const {
   return m_jetScale;
}

unsigned int
TrigConf::L1ThrExtraInfo_JETLegacy::ptMinToTopoLargeWindow() const {
   return m_ptMinToTopoLargeWindow;
}

unsigned int
TrigConf::L1ThrExtraInfo_JETLegacy::ptMinToTopoSmallWindow() const {
   return m_ptMinToTopoSmallWindow;
}

void
TrigConf::L1ThrExtraInfo_JETLegacy::update()
{
   for( auto & x : m_extraInfo ) {
      if( x.first == "jetscale" ) {
         m_jetScale = x.second.getValue<unsigned int>();
      } else if( x.first == "ptMinToTopoLargeWindow" ) {
         m_ptMinToTopoLargeWindow = x.second.getValue<unsigned int>();
      } else if( x.first == "ptMinToTopoSmallWindow" ) {
         m_ptMinToTopoSmallWindow = x.second.getValue<unsigned int>();
      }
   }
}


/***********************************
 *
 * Extra info for new thresholds
 *
 ***********************************/

/*******
 * eEM
 *******/
TrigConf::L1ThrExtraInfo_eEMTAU::L1ThrExtraInfo_eEMTAU(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_eEMTAU::~L1ThrExtraInfo_eEMTAU()
{}

unsigned int
TrigConf::L1ThrExtraInfo_eEMTAU::ptMinToTopo() const {
   return m_ptMinToTopo;
}

const TrigConf::Isolation &
TrigConf::L1ThrExtraInfo_eEMTAU::isolation(TrigConf::Isolation::WP wp, int eta) const
{
   return m_isolation.at(wp).at(eta);
}

const TrigConf::ValueWithEtaDependence<TrigConf::Isolation> &
TrigConf::L1ThrExtraInfo_eEMTAU::isolation(TrigConf::Isolation::WP wp) const
{
   return m_isolation.at(wp);
}


void
TrigConf::L1ThrExtraInfo_eEMTAU::update()
{
   for( auto & x : m_extraInfo ) {
      if( x.first == "ptMinToTopo" ) {
         m_ptMinToTopo = x.second.getValue<unsigned int>();
      } else if( x.first == "workingPoints" ) {
         for( auto & y : x.second.data() ) {
            auto wp = (y.first == "Loose") ? Isolation::WP::LOOSE : ( (y.first == "Medium") ? Isolation::WP::MEDIUM : Isolation::WP::TIGHT );
            auto & iso = m_isolation.emplace(wp, string("eEM_WP_" + y.first)).first->second;
            for(auto & c : y.second ) {
               auto etamin = c.second.get_child("etamin").get_value<unsigned int>();
               auto etamax = c.second.get_child("etamax").get_value<unsigned int>();
               auto priority = c.second.get_optional<unsigned int>("priority").get_value_or(0);
               iso.addRangeValue(Isolation(c.second), etamin, etamax, priority, /*symmetric=*/ false);
            }
         }
      }
   }
}


/*******
 * jJ
 *******/
TrigConf::L1ThrExtraInfo_jJ::L1ThrExtraInfo_jJ(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_jJ::~L1ThrExtraInfo_jJ()
{}

unsigned int
TrigConf::L1ThrExtraInfo_jJ::ptMinToTopoSmall(int eta) const {
   for(auto & ptmin : m_ptMinToTopoSmall) {
      if( (ptmin.first.first<=eta) && (eta<=ptmin.first.second) ) {
         return ptmin.second;
      }
   }
   throw std::runtime_error("No ptMinToTopoSmall for jJ at eta=" + std::to_string(eta));
}

unsigned int
TrigConf::L1ThrExtraInfo_jJ::ptMinToTopoLarge(int eta) const {
   for(auto & ptmin : m_ptMinToTopoLarge) {
      if( (ptmin.first.first<=eta) && (eta<=ptmin.first.second) ) {
         return ptmin.second;
      }
   }
   throw std::runtime_error("No ptMinToTopoLarge for jJ at eta=" + std::to_string(eta));
}

void
TrigConf::L1ThrExtraInfo_jJ::update()
{
   for( auto & x : m_extraInfo ) {
      if( x.first == "ptMinToTopo" ) {
         for( auto & k : x.second.data() ) {
            unsigned int etamin = k.second.get_child("etamin").get_value<unsigned int>();
            unsigned int etamax = k.second.get_child("etamax").get_value<unsigned int>();
            unsigned int small = k.second.get_child("small").get_value<unsigned int>();
            unsigned int large = k.second.get_child("large").get_value<unsigned int>();
            m_ptMinToTopoSmall.emplace( std::piecewise_construct,
                                        std::forward_as_tuple(etamin,etamax),
                                        std::forward_as_tuple(small));
            m_ptMinToTopoLarge.emplace( std::piecewise_construct,
                                        std::forward_as_tuple(etamin,etamax),
                                        std::forward_as_tuple(large));
         }
      }
   }
}


/*******
 * jTAU
 *******/
TrigConf::L1ThrExtraInfo_jTAU::L1ThrExtraInfo_jTAU(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_jTAU::~L1ThrExtraInfo_jTAU()
{}

unsigned int
TrigConf::L1ThrExtraInfo_jTAU::ptMinToTopo() const {
   return m_ptMinToTopo;
}

void
TrigConf::L1ThrExtraInfo_jTAU::update()
{
   for( auto & x : m_extraInfo ) {
      if( x.first == "ptMinToTopo" ) {
         m_ptMinToTopo = x.second.getValue<unsigned int>();
      }
   }
}

/*******
 * gXE
 *******/
TrigConf::L1ThrExtraInfo_gXE::L1ThrExtraInfo_gXE(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_gXE::~L1ThrExtraInfo_gXE()
{}

void
TrigConf::L1ThrExtraInfo_gXE::update()
{
}

/*******
 * MU
 *******/
TrigConf::L1ThrExtraInfo_MU::L1ThrExtraInfo_MU(const std::string & thrTypeName, const ptree & data) :
   L1ThrExtraInfoBase(thrTypeName, data)
{
   update();
}

TrigConf::L1ThrExtraInfo_MU::~L1ThrExtraInfo_MU()
{}

unsigned int
TrigConf::L1ThrExtraInfo_MU::rpcIdxForPt(unsigned int pt) const
{
   try {
      return m_rpcPtMap.at(pt);
   }
   catch(std::exception & ex) {
      std::cerr << "No RPC index defined for pt " << pt << endl;
      throw;
   }   
}


unsigned int
TrigConf::L1ThrExtraInfo_MU::tgcIdxForPt(unsigned int pt) const
{
   try {
      return m_tgcPtMap.at(pt);
   }
   catch(std::exception & ex) {
      std::cerr << "No TGC index defined for pt " << pt << endl;
      throw;
   }
}

std::vector<unsigned int>
TrigConf::L1ThrExtraInfo_MU::knownRpcPtValues() const
{
   std::vector<unsigned int> ptValues;   
   for( auto & x : m_rpcPtMap ) {
      ptValues.emplace_back(x.first);
   }
   return ptValues;
}

std::vector<unsigned int>
TrigConf::L1ThrExtraInfo_MU::knownTgcPtValues() const
{
   std::vector<unsigned int> ptValues;   
   for( auto & x : m_tgcPtMap ) {
      ptValues.emplace_back(x.first);
   }
   return ptValues;
}


std::vector<std::string>
TrigConf::L1ThrExtraInfo_MU::exclusionListNames() const
{
   std::vector<std::string> listNames;   
   for( auto & x : m_roiExclusionLists ) {
      listNames.emplace_back(x.first);
   }
   return listNames;
}


const std::map<std::string, std::vector<unsigned int> > &
TrigConf::L1ThrExtraInfo_MU::exlusionList(const std::string & listName) const
{
   try {
      return m_roiExclusionLists.at(listName);
   }
   catch(std::exception & ex) {
      std::cerr << "No exclusion list '" << listName << "' defined in MU threshold exlusionLists" << endl;
      throw;
   }
}

void
TrigConf::L1ThrExtraInfo_MU::update()
{
   for( auto & x : m_extraInfo["roads"].getObject("rpc").data() ) {
      m_rpcPtMap.emplace( boost::lexical_cast<unsigned int, std::string>(x.first),
                          boost::lexical_cast<unsigned int, std::string>(x.second.data()));
   }
   for( auto & x : m_extraInfo["roads"].getObject("tgc").data() ) {
      m_tgcPtMap.emplace( boost::lexical_cast<unsigned int, std::string>(x.first),
                          boost::lexical_cast<unsigned int, std::string>(x.second.data()));
   }

   for( auto & x : m_extraInfo["exclusionLists"].data() ) {
      const std::string & listName = x.first;
      std::map<std::string, std::vector<unsigned int>> roisBySector;
      for( auto & list : x.second ) {
         const std::string & sectorName = list.second.get_child("sectorName").get_value<std::string>();
         std::vector<unsigned int> rois;
         for( auto & roi : list.second.get_child("rois") ) {
            rois.push_back( boost::lexical_cast<unsigned int, std::string>( roi.second.data() ) );
         }
         roisBySector.emplace(sectorName, std::move(rois));
      }
      m_roiExclusionLists.emplace(listName, std::move(roisBySector));
   }
}


