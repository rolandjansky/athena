/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAMonitoring/MuonDQAEventInfo.h"
#include <inttypes.h>

#include <utility>

namespace MuonDQA {
  
  MuonDQAEventInfo::MuonDQAEventInfo()
    : m_runNumber( 0 ), m_eventNumber( 0 ), m_timeStamp( 0 ), m_ns_Offset( 0 ), m_running_time( 0. ), m_lumiBlock( 0 ), m_tag( "none" )
  {
  }

  MuonDQAEventInfo::MuonDQAEventInfo( unsigned int run_number, unsigned int event_number )
    : m_runNumber( run_number ), m_eventNumber( event_number ), m_timeStamp( 0 ), m_ns_Offset( 0 ), m_running_time( 0. ), m_lumiBlock( 0 )
  {
  }

 
  MuonDQAEventInfo::MuonDQAEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp )
    : m_runNumber( run_number ), m_eventNumber( event_number ), m_timeStamp( time_stamp ), m_ns_Offset( 0 ), m_running_time( 0. ), m_lumiBlock( 0 ), m_tag( "none" )
  {
  }
 
  MuonDQAEventInfo::MuonDQAEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp, unsigned int lumi_block )
    : m_runNumber( run_number ), m_eventNumber( event_number ), m_timeStamp( time_stamp ), m_ns_Offset( 0 ), m_running_time( 0. ), m_lumiBlock( lumi_block ) 
  {
  }

  MuonDQAEventInfo::MuonDQAEventInfo( unsigned int run_number, 
                                      unsigned int event_number, 
				      std::string  event_type,
				      unsigned int time_stamp, 
				      unsigned int ns_Offset, 
				      std::bitset<8> trig_Type, 
				      float running_time,
				      unsigned int lumi_block, 
				      /*
					std::vector<long>* lvl1_Type, 
					unsigned int ext_Lvl1ID,
					std::vector<long>* lvl2_Info,
					std::vector<long>* ef_info ,
				      */  
				      std::string tag )
    : m_runNumber( run_number ), m_eventNumber( event_number ), m_eventtype( std::move(event_type) ) , m_timeStamp( time_stamp ), 
      m_ns_Offset( ns_Offset ), m_trigType ( trig_Type ), m_running_time( running_time ), m_lumiBlock( lumi_block ), 
      /*m_lvl1Type( lvl1_Type ), m_extLvl1ID( ext_Lvl1ID ) , m_lvl2Info( lvl2_Info ) , m_efinfo( ef_info ) , */  m_tag( std::move(tag) )
  {
  }

 
  MuonDQAEventInfo::MuonDQAEventInfo( const MuonDQAEventInfo& eventInfo )
  {
    m_runNumber = eventInfo.runNumber();
    m_eventNumber = eventInfo.eventNumber();
    m_timeStamp = eventInfo.timeStamp();
    m_ns_Offset = eventInfo.ns_Offset();
    m_trigType = eventInfo.trigType();
    m_running_time = eventInfo.runningtime();
    m_lumiBlock = eventInfo.lumiBlock();
    // m_lvl1Type = eventInfo.lvl1Type();
    // m_extLvl1ID = eventInfo.extLvl1ID();
    // m_lvl2Info = eventInfo.lvl2Info();
    // m_efinfo = eventInfo.efinfo();
    m_tag = eventInfo.tag();
    m_trigger_bits = eventInfo.m_trigger_bits;
  }
 
  MuonDQAEventInfo::~MuonDQAEventInfo()
  {
  }
    
  std::ostream& MuonDQAEventInfo::dump( std::ostream& stream ) const {
    stream << "MuonDQAEventInfo :      " << std::endl;
    stream << "         runnumber    = " << runNumber() << std::endl;
    stream << "         eventnumber  = " << eventNumber() << std::endl;
    stream << "         eventType    = " << eventtype() << std::endl;
    stream << "         timestamp    = " << timeStamp() << std::endl;
    stream << "         ns_Offset    = " << ns_Offset() << std::endl;
    stream << "         trigType     = " << trigType() << std::endl;
    stream << "         runningtime  = " << runningtime()*3600 << std::endl;
    stream << "         LumiBlock    = " << lumiBlock() << std::endl;
    // stream << "         L1TrigType   = " << lvl1Type() << std::endl;
    // stream << "         ExtendedL1   = " << extLvl1ID() << std::endl;
    // stream << "         L2TrigInfo   = " << lvl2Info() << std::endl;
    // stream << "         EF Info      = " << efinfo() << std::endl;
    stream << "         tag          = " << tag() << std::endl;
    //stream << "         TrigerBit = " << triggerBits() << std::endl;
    return stream ;
  }
 

 
std::ostream& operator<<( std::ostream& stream, const MuonDQAEventInfo& info ){
  return info.dump(stream);
}
}//namespace MuonDQA
