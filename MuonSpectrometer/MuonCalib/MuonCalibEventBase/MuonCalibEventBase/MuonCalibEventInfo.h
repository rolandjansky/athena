/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store event information in for MuonCalibEvent  //
// Author: Zdenko van Kesteren                             //
// Date  : 18 October 2005                                 //
//*********************************************************//


#ifndef MuonCalibEventInfo_h
#define MuonCalibEventInfo_h

#include<string>
#include<iostream>
#include<vector>

namespace MuonCalib {
/**
   @class MuonCalibEventInfo
   MuonCalib::MuonCalibEventInfo: a class which stores the following information of an event:
   - run number
   - event number
   - time stamp
   - a tag, used to distinguish which reconstruction package is used to reconstruct the 
MuonCalibSegment. At the moment, three tags are used:

Moore : Muon reconstruction with the Moore package. One segment per pattern is be stored.
	
Muonboy : Muon reconstruction with the Muonboy package. One segment per pattern is stored.
       
MoMu : Modular muon reconstruction is applied. A pattern may contain multiple segments.
*/
  class MuonCalibEventInfo{
    public:
    MuonCalibEventInfo(); //!< Default constructor
    MuonCalibEventInfo( unsigned int run_number, unsigned int event_number ); //!< constructor with runnumber/eventnumber (for sim data) 
    MuonCalibEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp ); //!< constructor run/event number and time stamp arguments
    MuonCalibEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp, unsigned int lumi_block, unsigned int bc_id );
    MuonCalibEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp, std::string tag ); //!< constructor from older times
    MuonCalibEventInfo( unsigned int run_number, unsigned int event_number, unsigned int time_stamp, unsigned int lumi_block, unsigned int bc_id, std::string tag ); //!< constructor initializing all members
    MuonCalibEventInfo( const MuonCalibEventInfo& eventInfo ); //!< copyconstructor
    MuonCalibEventInfo& operator=(const MuonCalibEventInfo& eventInfo ); // !< assignment operator
    ~MuonCalibEventInfo(); //!< destructor
    
    std::ostream& dump( std::ostream &stream ) const;
    
    unsigned int runNumber()   const { return m_runNumber; }   //!< retrieving runnumber of MuonCalibEvent
    unsigned int eventNumber() const { return m_eventNumber; } //!< retrieving eventnumber of MuonCalibevent
    unsigned int timeStamp()   const { return m_timeStamp; }   //!< retrieving timestamp of MuonCalibEvent
    unsigned int lumiBlock()   const { return m_lumiBlock; }   //!< retrieving lumiblock of MuonCalibEvent
    unsigned int bcId()        const { return m_bcId; }        //!< retrieving bcid of MuonCalibEvent
    std::string  tag()         const { return m_tag; }         //!< retrieving reconstruction tag of MuonCalibEvent
    const std::vector<bool> & triggerBits() const {return m_trigger_bits;}

    void setRunNumber( const unsigned int run_number ) { m_runNumber = run_number; } //!< sets runnumber
    void setEventNumber( const unsigned int event_number ) { m_eventNumber = event_number; } //!< sets eventnumber
    void setTimeStamp( const unsigned int time_stamp ) { m_timeStamp = time_stamp; } //!< sets timestamp
    void setLumiBlock( const unsigned int lumi_block ) { m_lumiBlock = lumi_block; } //!< sets lumiblock
    void setBcId( const unsigned int bc_id ) { m_bcId = bc_id; } //!< sets bcid
    void setTag( const std::string tag ) { m_tag = tag; } //!< sets reconstruction tag
    inline void setNumberOfTriggerBits(unsigned int n) { m_trigger_bits.resize(n); }
    inline void setTriggerBit(unsigned int n, bool bit) {m_trigger_bits[n]=bit;}

    private:
    unsigned int m_runNumber;   //!< runnumber member
    unsigned int m_eventNumber; //!< eventnumber member
    unsigned int m_timeStamp;   //!< timestamp member
    unsigned int m_lumiBlock;   //!< lumiBlock member
    unsigned int m_bcId;        //!< bcId member
    std::string  m_tag;         //!< reconstruction tag member
    std::vector<bool> m_trigger_bits; //!< Trigger bits
  };

} //namespace MuonCalib

std::ostream& operator<<( std::ostream &stream, const MuonCalib::MuonCalibEventInfo &info );

#endif //MuonCalibEventInfo_h
