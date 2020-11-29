/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./ThresholdMap.h"

#include <cstdlib>


LVL1CTP::ThresholdMap::ThresholdMap( const TrigConf::L1Menu * l1menu )
{
   //unsigned int start = 0;
   for ( auto & thr : l1menu->thresholds() ) {
      CTPTriggerThreshold * ctpThr = new CTPTriggerThreshold(thr->name());
      /*
        defining the start and stop bit is much more complex now, as
        the CTP has many more inputs and the precise mapping of the
        inputs to the TIP vector has to be worked out
      */
      m_mapByName[ctpThr->name()] = ctpThr;
   }
}


LVL1CTP::ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector)
{
   unsigned int start = 0;
   for ( TrigConf::TriggerThreshold* thr : threshold_vector ) {
      CTPTriggerThreshold * ctpThr = new CTPTriggerThreshold(thr->name());
      unsigned int end = start + abs( thr->cableStart() - thr->cableEnd() );
      ctpThr->setStartBit( start );
      ctpThr->setEndBit( end );
      start = end + 1;
      m_mapByName[thr->name()] = ctpThr;
   }		
}
	
LVL1CTP::ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, 
                                     const std::vector< TrigConf::PIT* >& pit_vector)
{		
   for ( TrigConf::TriggerThreshold* thr : threshold_vector ) {
      CTPTriggerThreshold * ctpThr = new CTPTriggerThreshold(thr->name());
      //default pit values: used if threshold doesn't have a corresponding pit bit
      int pit_start=999;
      int pit_end=-999;
      for( TrigConf::PIT* pit : pit_vector ) {
         if( pit->thresholdName() != thr->name() )
            continue;
         if( pit->pitNumber() < pit_start )
            pit_start = pit->pitNumber();
         if( pit->pitNumber() > pit_end )
            pit_end = pit->pitNumber();
      }			
      ctpThr->setStartBit( pit_start ); 
      ctpThr->setEndBit( pit_end );			
      m_mapByName[thr->name()] = ctpThr;
   }		
}


LVL1CTP::ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, 
                                     const std::vector< TrigConf::TIP* >& tip_vector)
{
   for ( TrigConf::TriggerThreshold* thr : threshold_vector ) {
      CTPTriggerThreshold * ctpThr = new CTPTriggerThreshold(thr->name());
      //default pit values: used if threshold doesn't have a corresponding pit bit
      int tip_start=999;
      int tip_end=-999;
      for( TrigConf::TIP* tip : tip_vector){
         if( tip->thresholdName() != thr->name() )
            continue;
         if( tip->tipNumber() < tip_start)
            tip_start = tip->tipNumber();
         if( tip->tipNumber() > tip_end )
            tip_end = tip->tipNumber();
      }
      ctpThr->setStartBit( tip_start ); 
      ctpThr->setEndBit( tip_end );
      m_mapByName[thr->name()] = ctpThr;
   }
}

LVL1CTP::ThresholdMap::~ThresholdMap() {
   for ( auto & entry : m_mapByName ) {
      delete entry.second;
   }
}

std::vector<std::string>
LVL1CTP::ThresholdMap::getThresholdNames() const {
   std::vector<std::string> thrNames;
   thrNames.reserve(m_mapByName.size());
   for( auto & entry : m_mapByName ) {
      thrNames.emplace_back(entry.first);
   }
   return thrNames;
}

const LVL1CTP::CTPTriggerThreshold &
LVL1CTP::ThresholdMap::getCTPThreshold( const std::string & thrName ) const {
   try {
      return * m_mapByName.at( thrName );
   }
   catch(std::exception&) {
      throw std::runtime_error("Threshold " + thrName + " not present in CTPSimulation's internal threshold map");
   }
}

