/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAEventInfo.h"
#include <iostream>

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
				      const std::string  & event_type,
				      unsigned int time_stamp, 
				      unsigned int ns_Offset, 
				      const std::bitset<8> & trig_Type, 
				      float running_time,
				      unsigned int lumi_block, 
				    
				      std::string tag )
    : m_runNumber( run_number ), m_eventNumber( event_number ), m_eventtype( std::move(event_type) ) , m_timeStamp( time_stamp ), 
      m_ns_Offset( ns_Offset ), m_trigType ( trig_Type ), m_running_time( running_time ), m_lumiBlock( lumi_block ), 
        m_tag( std::move(tag) )
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
   
    stream << "         tag          = " << tag() << std::endl;
    return stream ;
  }
 

 
std::ostream& operator<<( std::ostream& stream, const MuonDQAEventInfo& info ){
  return info.dump(stream);
}
}//namespace MuonDQA
