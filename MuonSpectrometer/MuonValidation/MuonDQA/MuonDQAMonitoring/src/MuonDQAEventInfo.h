/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// Package : MuonDQAMonitoring                                          // 
// Class to store event information in for MuonDQAMonitoring            //
// based on MuonCalibAlgs                                               //
// Author: N. Benekos - UIUC                                            //
// Date  : March 2008                                                   //
//////////////////////////////////////////////////////////////////////////
 
 
#ifndef MuonDQAEventInfo_h
#define MuonDQAEventInfo_h

#include<string>
#include<iostream>
#include<vector>
#include <bitset>
#include <stdint.h> 
#include <inttypes.h>

namespace MuonDQA {
  /**
     @class MuonDQAEventInfo
     MuonDQA::MuonDQAEventInfo: a class which stores the following information of an event:
     - run number
     - event number
     - event Type
     - time stamp
     - ns Offset
     - Trigger Type
     - Running Time
     - LumiBlock
     - L1 Trigger Type
     - Extended L1 
     - L2 Trigger Info
     - Event filter Info
 
  */
  class MuonDQAEventInfo{
  public:
    MuonDQAEventInfo(); //!< Default constructor
   
    //!< constructor with runnumber/eventnumber (for sim data)  
    MuonDQAEventInfo( unsigned int run_number, unsigned int event_number ); 
    
    //!< constructor run/event number and time stamp arguments
    MuonDQAEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp ); 
    
    //!< constructor run/event number/time stamp and lumiBlock arguments
    MuonDQAEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp , unsigned int lumi_block  ); 
		      
    //!< constructor initializing all members
    MuonDQAEventInfo( unsigned int run_number,        // Run Number
                      unsigned int event_number,      // Event Number
		      std::string  event_type,         // Event Type		      
		      unsigned int time_stamp,        // Time stamp
                      unsigned int ns_Offset,         // ns Offset		      
		      std::bitset<8> trig_Type,        // Trigger Type
		      float running_time,             // Running Time 
		      unsigned int lumi_block,         // Lumi Block  
		      /*
			std::vector<long>* lvl1_Type ,  
			unsigned int ext_Lvl1ID ,
			std::vector<long>* lvl2_Info,  
			std::vector<long>* ef_info,
		      */
		      std::string tag ); 

    MuonDQAEventInfo( const MuonDQAEventInfo& eventInfo ); //!< copyconstructor
    ~MuonDQAEventInfo(); //!< destructor
    
    std::ostream& dump( std::ostream& stream ) const ;
    
    unsigned int runNumber()   const { return m_runNumber ; }   //!< retrieving runnumber of MuonDQAMonitoring
    unsigned int eventNumber() const { return m_eventNumber ; } //!< retrieving eventnumber of MuonDQAMonitoring
    std::string  eventtype()   const { return m_eventtype ; }    //!< retrieving eventType of MuonDQAMonitoring
    unsigned int timeStamp()   const { return m_timeStamp ; }   //!< retrieving timestamp of MuonDQAMonitoring
    unsigned int ns_Offset()   const { return m_ns_Offset ; }   //!< retrieving ns_Offset of MuonDQAMonitoring
    const std::bitset<8> trigType()  const { return m_trigType ; }    //!< retrieving Trigger Type of MuonDQAMonitoring
    float runningtime() const { return m_running_time; } //!< retrieving runningtime of MuonDQAMonitoring
    unsigned int lumiBlock()   const { return m_lumiBlock ; }    //!< retrieving LumiBlock of MuonDQAMonitoring
    
    //const std::vector<long>* lvl1Type()   const { return m_lvl1Type ; }      //!< retrieving lvl1Type of MuonDQAMonitoring     
    //unsigned int extLvl1ID()   const { return m_extLvl1ID ; }    //!< retrieving extLvl1ID of MuonDQAMonitoring 
    //const std::vector<long>* lvl2Info()   const { return m_lvl2Info ; }      //!< retrieving lvl2Info of MuonDQAMonitoring    
    //const std::vector<long>* efinfo()   const { return m_efinfo ; }          //!< retrieving efinfo of MuonDQAMonitoring
    
    std::string  tag() const { return m_tag ; } //!< retrieving reconstruction tag of MuonDQAMonitoring    
    const std::vector<bool> & triggerBits() const {return m_trigger_bits;}

    void setRunNumber( const unsigned int run_number ) { m_runNumber = run_number ; } //!< sets runnumber
    void setEventNumber( const unsigned int event_number ) { m_eventNumber = event_number ; } //!< sets eventnumber
    void setEventType( const std::string event_type ) { m_eventtype = event_type ; } //!< sets eventType
    void setTimeStamp( const unsigned int time_stamp ) { m_timeStamp = time_stamp ; } //!< sets timestamp
    void setOffset( const unsigned int ns_Offset ) { m_ns_Offset = ns_Offset ; } //!< sets ns_Offset
    void setTrigType( const std::bitset<8> trig_Type ) { m_trigType = trig_Type ; } //!< sets Trigger Type
    void setRunTime( const float running_time ) { m_running_time = running_time ; } //!< sets ns_Offset       
    void setLumiBlock( const unsigned int lumi_block ) { m_lumiBlock = lumi_block ; } //!< sets LumiBlock
    // void setLV1Info( uint32_t lvl1_Type ) { m_lvl1Type = lvl1_Type ; } //!< sets lvl1_Type
    // void setLV1Info( const std::vector<long>* lvl1_Type ) { m_lvl1Type = lvl1_Type ; } //!< sets lvl1_Type
    // void setExtLV1ID( const unsigned int ext_Lvl1ID ) { m_extLvl1ID = ext_Lvl1ID ; } //!< sets Extended lvl1 Info
    // void setLV2Info( const std::vector<long>* lvl2_Info ) { m_lvl2Info = lvl2_Info ; } //!< sets lvl2_Info
    // void setEFInfo( const std::vector<long>* ef_info ) { m_efinfo = ef_info ; } //!< sets Event Filter Info
    void setTag( const std::string tag ) { m_tag = tag ; } //!< sets reconstruction tag
    
    inline void setNumberOfTriggerBits(unsigned int n) { m_trigger_bits.resize(n); }
    inline void setTriggerBit(unsigned int n, bool bit) {m_trigger_bits[n]=bit;}

    MuonDQAEventInfo & operator=(const MuonDQAEventInfo &right) {
      if( this != &right ) { 
        this->setRunNumber(right.runNumber());
        this->setEventNumber(right.eventNumber());
        this->setEventType(right.eventtype());
        this->setTimeStamp(right.timeStamp());
        this->setOffset(right.ns_Offset());
        this->setTrigType(right.trigType());
        this->setRunTime(right.runningtime());
        this->setLumiBlock(right.lumiBlock());
        this->setTag(right.tag());
        std::vector<bool> trigbits = right.triggerBits();
        this->setNumberOfTriggerBits(trigbits.size());
        for(unsigned int ii=0; ii<trigbits.size(); ii++){
           this->setTriggerBit(ii, trigbits[ii] );
        }
      } 
      return *this; 
    };

  private:
    unsigned int m_runNumber;     //!< runnumber member
    unsigned int m_eventNumber;   //!< eventnumber member
    std::string m_eventtype;      //!< eventType member
    unsigned int m_timeStamp;     //!< timestamp member
    unsigned int m_ns_Offset;      // ns Offset member
    std::bitset<8> m_trigType ;         // Trigger Type member
    float m_running_time; // Running Time member
    unsigned int m_lumiBlock;     //!< LumiBlock member 
 
    //std::vector<long>* m_lvl1Type;       // L1 Trigger Type member    
    //unsigned int m_extLvl1ID;            // Extended L1info member  
    //std::vector<long>* m_lvl2Info;       // L2 Trigger Info member 
    //std::vector<long>* m_efinfo;         // Event filter Info member 
    
    std::string  m_tag; //!< reconstruction tag member
    std::vector<bool> m_trigger_bits; //!< Trigger bits
  };
 
std::ostream& operator<<( std::ostream& stream, const MuonDQAEventInfo& info ); 
} //namespace MuonDQA
 


#endif //MuonDQAEventInfo_h
 
