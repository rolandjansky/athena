/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1ThrExtraInfo.h"


TrigConf::L1Threshold_EM::L1Threshold_EM( const std::string & name, const std::string & type,
                                          std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
   L1Threshold(name, type, m_extraInfo, data)
{
   update();
}

void
TrigConf::L1Threshold_EM::update()
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

unsigned int
TrigConf::L1Threshold_EM::thrValueCounts(int eta) const {
   auto emInfo = std::dynamic_pointer_cast<L1ThrExtraInfo_EMTAULegacy>(m_extraInfo.lock());
   return thrValue(eta) * emInfo->emScale();
}

void
TrigConf::L1Threshold_EM::print(std::ostream & os) const {
   os << "EM threshold " << name() << " with mapping " << mapping() << std::endl;
   for( int eta = 0; eta<=49; eta++ ) {
      auto vneg = thrValue(-eta);
      auto vpos = thrValue(eta);
      auto counts = thrValueCounts(eta);
      auto iso = isolationMask(eta);
      os << "    eta = " << eta << " : " << vneg << "  " << vpos << (vpos!=vneg ? "  !!!   " : "     ") 
         << counts << ", isoMaks " << iso << std::endl;
   }
}


TrigConf::L1Threshold_TAU::L1Threshold_TAU( const std::string & name, const std::string & type,
                                            std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
   L1Threshold(name, type, m_extraInfo, data)
{
   update();
}

void
TrigConf::L1Threshold_TAU::update()
{
   // read the isolation
   std::string isobits = getAttribute("isobits");
   m_isolationMask = 0;
   for(char c: isobits) {
      m_isolationMask <<= 1;
      if(c=='1') m_isolationMask += 1;
   }
}



TrigConf::L1Threshold_eEM::L1Threshold_eEM( const std::string & name, const std::string & type,
                                            std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
   L1Threshold(name, type, m_extraInfo, data)
{
   update();
}

void
TrigConf::L1Threshold_eEM::update()
{
   auto translate = [](const std::string &wp) { return wp=="Loose" ? Isolation::WP::LOOSE : 
                                                ( wp=="Medium" ? Isolation::WP::MEDIUM : 
                                                  ( wp=="Medium" ? Isolation::WP::TIGHT : Isolation::WP::NONE ) ); };
   // read the isolation requirements
   m_reta = translate(getAttribute("reta"));
   m_rhad = translate(getAttribute("rhad"));
   m_wstot = translate(getAttribute("wstot"));
}




TrigConf::L1Threshold_MU::L1Threshold_MU( const std::string & name, const std::string & type,
                                          std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo, const ptree & data) :
   L1Threshold(name, type, m_extraInfo, data)
{
   update();
}

void
TrigConf::L1Threshold_MU::update()
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




