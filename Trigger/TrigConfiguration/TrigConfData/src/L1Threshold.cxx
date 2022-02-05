/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1ThrExtraInfo.h"


/******************************************
 *
 *  Legacy L1Calo thresholds
 *
 ******************************************/

/**
 * EM
 */
void
TrigConf::L1Threshold_EM::load()
{
   // read the isolation
   if( const auto & thrVs = data().get_child_optional("thrValues") ) {
      for( auto & x : thrVs.get() ) {
         auto isobits = x.second.get_child("isobits").get_value<std::string>();
         auto etamin = x.second.get_child("etamin").get_value<unsigned int>();
         auto etamax = x.second.get_child("etamax").get_value<unsigned int>();
         auto priority = x.second.get_child("priority").get_value<unsigned int>();
         // turn bin string into uint, e.g. "01001" -> 9 
         uint16_t isomask(0);
         for(char c: isobits) {
            isomask <<= 1;
            if(c=='1') isomask += 1;
         }
         m_isolationMask.addRangeValue(isomask, etamin, etamax, priority, /*symmetric=*/ false);
      }
   }
}

void
TrigConf::L1Threshold_EM::print(std::ostream & os) const {
   os << "EM threshold " << name() << " with mapping " << mapping() << std::endl;
   for( int eta = -49; eta<49; eta++ ) {
      auto value = thrValue(eta);
      auto valueMeV = thrValueMeV(eta);
      auto counts = thrValueCounts(eta);
      auto value100MeV = thrValue100MeV(eta);
      auto iso = isolationMask(eta);
      os << "    eta = " << eta << " : " << value << " GeV " << valueMeV << " MeV " << value100MeV << " 100MeV " 
         << counts << " counts , isoMaks " << iso << std::endl;
   }
}

/**
 * TAU
 */
void
TrigConf::L1Threshold_TAU::load()
{
   // read the isolation
   std::string isobits = getAttribute("isobits");
   m_isolationMask = 0;
   for(char c: isobits) {
      m_isolationMask <<= 1;
      if(c=='1') m_isolationMask += 1;
   }
}

/**
 * JET
 */
void
TrigConf::L1Threshold_JET::load()
{
   // allowed values for eta-range boundaries are 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 1.7, 1.9, 2.1, 2.3, 2.5, 2.8, 3.1, 4.9 and their negatives

   static const std::vector<int> allowedBoundaries{0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 28, 31, 49};
   if( const auto & thrVs = data().get_child_optional("thrValues") ) {
      for( auto & x : thrVs.get() ) {
         auto etamin = x.second.get_child("etamin").get_value<int>();
         // will be re-enabled once we have the faulty configurations in the TriggerDB removed
         // if(std::find(begin(allowedBoundaries), end(allowedBoundaries), abs(etamin)) == allowedBoundaries.end()) {
         //    throw std::runtime_error(name() + ": etamin value " + std::to_string(etamin) + " not an allowed value for legacy JETs");
         // }
         auto etamax = x.second.get_child("etamax").get_value<int>();
         // will be re-enabled once we have the faulty configurations in the TriggerDB removed
         // if(std::find(begin(allowedBoundaries), end(allowedBoundaries), abs(etamax)) == allowedBoundaries.end()) {
         //    throw std::runtime_error(name() + ": etamax value " + std::to_string(etamax) + " not an allowed value for legacy JETs");
         // }
         auto priority = x.second.get_child("priority").get_value<unsigned int>();
         auto window = x.second.get_child("window").get_value<unsigned int>();
         m_etaDepWindow.addRangeValue(window, etamin, etamax, priority, /*symmetric=*/ false);
      }
   }
}
unsigned int
TrigConf::L1Threshold_JET::window(int eta) const {
   return m_etaDepWindow.at(eta);
}

/**
 * ZB
 */
void
TrigConf::L1Threshold_ZB::load()
{
   m_seed = getAttribute("seed");
   m_seedBcdelay = getAttribute<unsigned int>("seedBcdelay");
   m_seedMultiplicity = getAttribute<unsigned int>("seedMultiplicity");
}

/******************************************
 *
 *  New L1Calo thresholds
 *
 ******************************************/

void
TrigConf::L1Threshold_eEM::load()
{
   // read the isolation requirements
   m_reta  = Selection::stringToWP(getAttribute_optional<std::string>("reta").value_or("None"));
   m_rhad  = Selection::stringToWP(getAttribute_optional<std::string>("rhad").value_or("None"));
   m_wstot = Selection::stringToWP(getAttribute_optional<std::string>("wstot").value_or("None"));
}

void
TrigConf::L1Threshold_jEM::load()
{
   // read the isolation requirements
   m_iso  = Selection::stringToWP(getAttribute_optional<std::string>("iso").value_or("None"));
   m_frac  = Selection::stringToWP(getAttribute_optional<std::string>("frac").value_or("None"));
   m_frac2 = Selection::stringToWP(getAttribute_optional<std::string>("frac2").value_or("None"));
}

void
TrigConf::L1Threshold_eTAU::load()
{
   // read the isolation requirements
   m_rCore = Selection::stringToWP(getAttribute_optional<std::string>("rCore").value_or("None"));
   m_rHad  = Selection::stringToWP(getAttribute_optional<std::string>("rHad").value_or("None"));
}

void
TrigConf::L1Threshold_jTAU::load()
{
   // read the isolation requirements
   m_isolation  = Selection::stringToWP(getAttribute_optional<std::string>("isolation").value_or("None"));
}

void
TrigConf::L1Threshold_cTAU::load()
{
   // read the isolation requirements
   m_isolation  = Selection::stringToWP(getAttribute_optional<std::string>("isolation").value_or("None"));
}

void
TrigConf::L1Threshold_jJ::load()
{
   m_etaDepThrValue.setOutsideRangeValue(getAttribute("maxValue", true, 14000000));
}

void
TrigConf::L1Threshold_jLJ::load()
{
   m_etaDepThrValue.setOutsideRangeValue(getAttribute("maxValue", true, 14000000));
}

void
TrigConf::L1Threshold_gJ::load()
{
   m_etaDepThrValue.setOutsideRangeValue(getAttribute("maxValue", true, 14000000));
}

void
TrigConf::L1Threshold_gLJ::load()
{
   m_etaDepThrValue.setOutsideRangeValue(getAttribute("maxValue", true, 14000000));
}

void
TrigConf::L1Threshold_jXE::load()
{}

void
TrigConf::L1Threshold_jTE::load()
{}

void
TrigConf::L1Threshold_gXE::load()
{}

void
TrigConf::L1Threshold_gTE::load()
{}

/******************************************
 *
 * Muon threshold
 *
 ******************************************/

/**
 * this function is only to satisfy the base class interface. It should not be used
 */
float
TrigConf::L1Threshold_MU::thrValue(int) const
{
   return static_cast<float>(m_ptBarrel);
}

void
TrigConf::L1Threshold_MU::load()
{
   auto muInfo = std::dynamic_pointer_cast<L1ThrExtraInfo_MU>(m_extraInfo.lock());

   m_ptBarrel   = getAttribute<unsigned int>("baThr");
   m_ptEndcap   = getAttribute<unsigned int>("ecThr");
   m_ptForward  = getAttribute<unsigned int>("fwThr");
   m_idxBarrel  = muInfo->rpcIdxForPt(m_ptBarrel);
   m_idxEndcap  = muInfo->tgcIdxForPt(m_ptEndcap);
   m_idxForward = muInfo->tgcIdxForPt(m_ptForward);

   m_rpcExclROIList = getAttribute("rpcExclROIList", true, "");
   m_tgcFlags = getAttribute("tgcFlags");
   m_rpcFlags = getAttribute_optional<std::string>("rpcFlags").value_or(""); 
   m_region = getAttribute("region");
}




