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
      auto iso = isolationMask(eta);
      os << "    eta = " << eta << " : " << value << " GeV " << valueMeV << " MeV " 
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

/******************************************
 *
 *  New L1Calo thresholds
 *
 ******************************************/

/**
 * eEM
 */
void
TrigConf::L1Threshold_eEM::load()
{
   auto translate = [](const std::string &wp) { return wp=="Loose" ? Isolation::WP::LOOSE : 
                                                ( wp=="Medium" ? Isolation::WP::MEDIUM : 
                                                  ( wp=="Tight" ? Isolation::WP::TIGHT : Isolation::WP::NONE ) ); };
   // read the isolation requirements
   m_reta = translate(getAttribute("reta"));
   m_rhad = translate(getAttribute("rhad"));
   m_wstot = translate(getAttribute("wstot"));
}

void
TrigConf::L1Threshold_eTAU::load()
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
   m_tgcFlag = getAttribute("tgcFlags");
   m_region = getAttribute("region");
}




