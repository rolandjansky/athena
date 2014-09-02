/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_BEAMSPOTID_H
#define INDET_BEAMSPOTID_H

#include <iostream>
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace BeamSpot {
  /** Class holding meta-data about a beamspot entry.
      Uses run, id, luminosity block start and one-past lumiEnd.
   */
  class ID {
  public:
    /** Default constructor*/
    ID():m_run(0), m_id(0),m_lumiStart(0),m_lumiEnd(0),m_runRange(1)
      ,m_eventFirst(0),m_eventLast(0),m_timeStampBegin(0), m_timeStampLast(0),
      m_timeStampOffset(0)
      ,m_firstAcceptedLB(0), m_lastAcceptedLB(0),m_bcid(0),m_beamSpotTool(0)
      {}
    /** Constructor with run and id (usually id is combined 64bit run+lumi)*/
    ID(unsigned int run ,uint64_t id):
      m_run(run), m_id(id),m_lumiStart(0),m_lumiEnd(0),
      m_runRange(1) ,m_eventFirst(0),m_eventLast(0),m_timeStampBegin(0), m_timeStampLast(0),
	m_timeStampOffset(0),m_firstAcceptedLB(0), m_lastAcceptedLB(0),m_bcid(0),m_beamSpotTool(0)
      {}
    /** Full constructor with run, id, lumiStart and lumirRange */
    ID(unsigned int run ,uint64_t id,
       unsigned int lumiStart, unsigned int lumiRange):
      m_run(run), m_id(id),m_lumiStart(lumiStart),
      m_lumiEnd(lumiStart+lumiRange),m_runRange(1) 
      ,m_eventFirst(0),m_eventLast(0),m_timeStampBegin(0), m_timeStampLast(0),
	m_timeStampOffset(0),m_firstAcceptedLB(0), m_lastAcceptedLB(0),m_bcid(0),m_beamSpotTool(0)
      {}
    //Important: lumiEnd is defined to be 'one-past-the-end' for consitency with DB.
    //E.G. for lumiblock == 1, with lumiRange of 1, lumiEnd would be 2.
    /** Special version of ID for running over multiple run values */
    ID(unsigned int run ,uint64_t id,
       unsigned int lumiStart, unsigned int lumiRange,
       unsigned int runRange): m_run(run), m_id(id),m_lumiStart(lumiStart),
      m_lumiEnd(lumiStart+lumiRange),m_runRange(runRange) 
      ,m_eventFirst(0),m_eventLast(0),m_timeStampBegin(0), m_timeStampLast(0),
      m_timeStampOffset(0),m_firstAcceptedLB(0), m_lastAcceptedLB(0),
	m_bcid(0),m_beamSpotTool(0){}

      // as above, but includes the bcid as well
      ID(unsigned int run ,uint64_t id,
	 unsigned int lumiStart, unsigned int lumiRange,
	 unsigned int runRange, unsigned int bcid): m_run(run), m_id(id),m_lumiStart(lumiStart),
	m_lumiEnd(lumiStart+lumiRange),m_runRange(runRange) 
	,m_eventFirst(0),m_eventLast(0),m_timeStampBegin(0), m_timeStampLast(0),
	m_timeStampOffset(0),m_firstAcceptedLB(0), m_lastAcceptedLB(0),
	m_bcid(bcid),m_beamSpotTool(0){} 

    void setRunRange(unsigned int rr){m_runRange = rr;}
    unsigned int getBSTool() const {return m_beamSpotTool;}
    unsigned int run() const {return m_run;} //!< run value.
    void run(unsigned int run) {m_run = run;}//!< set run value.
    uint64_t id() const {return m_id;}       //!< id (64bit).
    void id(uint64_t id) {m_id = id;}        //!< set id.
    unsigned int lumiStart() const {return m_lumiStart;} //!< Luminosity start value.
    void lumiStart(unsigned int l) { m_lumiStart= l;}    //!< Set luminosity start value
    //lumiEnd IS [begin, end), ie. the last lumivalue + 1
    /** End value of Luminosity. This is defined to be one-past-the-post style.
	For example for the range 1-3 inclusive, the LumiEnd value returns 4.
	Justification is for how the values are stored in the Conditions Database.
     */
    unsigned int lumiEnd() const {return m_lumiEnd;}
    void lumiEnd(unsigned int l) { m_lumiEnd= l;} //!< set (one-past) the lumiEnd value.
    // runEnd gives [begin,end], ie if runRange = 1, runBegin = runEnd.
    /** Final run position. Set as the final value - not one-past-the-end.*/
    unsigned int runEnd() const { return m_run + m_runRange -1; } 
    
    void runEnd(unsigned int end) { m_runRange = end - m_run +1;}//!< Set the runEnd value.
  
    /** Comparison operator compares only the values of the member @c id. and the bcid*/
    bool operator==( const ID & rhs)const { return ((m_id == rhs.m_id) && (m_bcid == rhs.m_bcid) && (m_beamSpotTool == rhs.m_beamSpotTool));}
    bool operator!=( const ID & rhs)const { return !(operator==(rhs));}
    /** Comparison operator compares only the values of the member @c id.and the bcid*/
    /*  bool operator<( const ID & rhs) const { 
      if ( m_id > rhs.m_id) return false;
      else if ( m_id < rhs.m_id) return true;
      else 
	return m_bcid < rhs.m_bcid;
        }
    */

    bool operator<( const ID & rhs) const { 
      if ( m_id < rhs.m_id) return true;
      if ( m_id > rhs.m_id) return false;
      if ( m_bcid < rhs.m_bcid ) return true;
      if ( m_bcid > rhs.m_bcid ) return false;
      if ( m_beamSpotTool < rhs.m_beamSpotTool ) return true;
      if ( m_beamSpotTool > rhs.m_beamSpotTool ) return false;
      return false;
    }
  
    void firstEvent(unsigned int l) { m_eventFirst = l;}
    void lastEvent(unsigned int l) { m_eventLast = l;}

    void firstTimeStamp(unsigned int l) {m_timeStampBegin = l;}
    void lastTimeStamp(unsigned int l) {m_timeStampLast = l;}
    void timeStampOffset(unsigned int l) { m_timeStampOffset = l;}
    
    unsigned int firstEvent() const {  return m_eventFirst;}
    unsigned int lastEvent() const {return m_eventLast;}
    unsigned int firstTimeStamp() const {return m_timeStampBegin;}
    unsigned int lastTimeStamp() const {return m_timeStampLast;}
    unsigned int timeStampOffset() const {return m_timeStampOffset;}

    unsigned int firstAcceptedLB() const { return m_firstAcceptedLB; }
    unsigned int lastAcceptedLB()  const { return m_lastAcceptedLB;  }
    void firstAcceptedLB(unsigned int lb) { m_firstAcceptedLB = lb; }
    void lastAcceptedLB( unsigned int lb) { m_lastAcceptedLB  = lb; }
    
    unsigned int firstLB() const { return m_firstLB; }
    unsigned int lastLB()  const { return m_lastLB;  }
    void firstLB(unsigned int lb) { m_firstLB = lb; }
    void lastLB( unsigned int lb) { m_lastLB  = lb; }

    unsigned int bcid() const { return m_bcid;}
    void bcid(unsigned int bcid) { m_bcid = bcid;}
    //    void setBSTool(ToolHandle<InDet::IInDetBeamSpotTool> tool){ m_beamSpotTool = tool; }
    void setBSTool(unsigned int tool){ m_beamSpotTool = tool; }
  private:
    unsigned int m_run;
    uint64_t m_id;
    unsigned int m_lumiStart, m_lumiEnd;
    unsigned int m_runRange;

    unsigned int m_eventFirst,m_eventLast;
    unsigned int m_timeStampBegin, m_timeStampLast;
    unsigned int m_timeStampOffset;

    //lb events used
    unsigned int m_firstAcceptedLB, m_lastAcceptedLB;
    unsigned int m_firstLB, m_lastLB;

    //if specific bcids are required
    unsigned int m_bcid;
    //ToolHandle<InDet::IInDetBeamSpotTool> m_beamSpotTool;
    unsigned int m_beamSpotTool;
  };
}
std::ostream & operator<<(std::ostream & os,const BeamSpot::ID & rhs) ;

#endif


