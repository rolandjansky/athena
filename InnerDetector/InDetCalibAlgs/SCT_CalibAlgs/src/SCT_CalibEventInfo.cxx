/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibEventInfo.h
 * Implementation file for the SCT_CalibEventInfo class
 * @author Shaun Roe
**/
#include <limits>
//STL, boost
#include "SCT_CalibEventInfo.h"
#include "SCT_CalibUtilities.h"
#include "EventInfo/EventInfo.h"

#include "EventInfo/EventID.h"
//coral/cool
#include "CoralBase/TimeStamp.h"

namespace {
const int INTMIN(std::numeric_limits<int>::min());
const int INTMAX(std::numeric_limits<int>::max());
const long long oneBillion(1000000000LL);
}

SCT_CalibEventInfo::SCT_CalibEventInfo(const std::string &name, ISvcLocator * svc):AthService(name,svc),
   m_eventInfoKey(std::string("ByteStreamEventInfo")),
   m_incidentSvc ( "IncidentSvc", name ),
   m_timeStampBegin(INTMAX),
   m_tsBeginString(""),
   m_tsEndString(""),
   m_timeStampEnd(INTMIN),
   m_timeStampMax(INTMIN),
   m_duration(0),
   m_LBBegin(INTMAX),
   m_LBEnd(INTMIN),
   m_numLB(0),
   m_numUOFO(0),
   m_numUOFOth(10),
   m_source("UNKNOWN"),
   m_runNumber(0),
   m_eventNumber(0),
   m_lumiBlock(0),
   m_timeStamp(0),
   m_bunchCrossing(0),
   m_counter(0)
{
}

StatusCode
SCT_CalibEventInfo::initialize() {
   msg( MSG::INFO)<<"Initialize of evtInfo in "<<PACKAGE_VERSION<<endmsg;
   ATH_CHECK(m_eventInfoKey.initialize());
   const int pri(500);
   m_incidentSvc->addListener( this, "BeginRun",         pri, true, true );
   m_incidentSvc->addListener( this, "UnknownOfflineId", pri, true );
   m_incidentSvc->addListener( this, "BeginEvent",       pri, true );
   m_incidentSvc->addListener( this, "EndEvent",         pri, true );
   return StatusCode::SUCCESS;
}

StatusCode
SCT_CalibEventInfo::finalize() {
   return StatusCode::SUCCESS;
}

StatusCode
SCT_CalibEventInfo::queryInterface(const InterfaceID & riid, void** ppvInterface ) {
   if ( ISCT_CalibEvtInfo::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ISCT_CalibEvtInfo*>(this);
   } else {
      return AthService::queryInterface(riid, ppvInterface);
   }
   addRef();
   return StatusCode::SUCCESS;
}

int SCT_CalibEventInfo::lumiBlock() const {
   return m_lumiBlock;
}

void
SCT_CalibEventInfo::handle(const Incident &inc) {

   if ( m_source == "BS" ) {
      SG::ReadHandle<EventInfo> evt(m_eventInfoKey);
      if (not evt.isValid()) {
         msg(MSG:: ERROR) << "Unable to get the EventInfo" << endmsg;
         return;
      }

      //listening for the Unknown offlineId for OfflineId..." error.
      //count number of instances/event
      if (inc.type() == "UnknownOfflineId") {
         incrementUOFO();
      }

      //at the beginning of each run do a print put
      if (inc.type() == "BeginRun") {
         msg(MSG:: INFO) << "BeginRun is HERE" << endmsg;
      }

      //at the beginning of each event set the counter to 0
      if (inc.type() == "BeginEvent") {
         resetUOFO();
      }

      //at the end end of each event, if there are more than m_numUOFOth instances
      //(default is 10) of the error, skip the event
      if (inc.type() == "EndEvent") {
         int nUOFO = UOFO();
         if (nUOFO > m_numUOFOth) {
            msg ( MSG::DEBUG ) << " More than " << m_numUOFOth <<" Id ROD failures, skipping event" << endmsg;
            m_incidentSvc->fireIncident(Incident(name(), "SkipEvent"));
         }
      }

      const EventInfo* evt_ptr = &(*evt);
      msg( MSG::VERBOSE ) << SCT_CalibAlgs::eventInfoAsString(evt_ptr) << endmsg;
      //--- TimeStamp/LB range analyzed
      const int timeStamp = evt->event_ID()->time_stamp();
      const int lumiBlock = evt->event_ID()->lumi_block();
      setTimeStamp(std::min(timeStamp, m_timeStampBegin), std::max(timeStamp, m_timeStampEnd));
      setLumiBlock(std::min(lumiBlock, m_LBBegin), std::max(lumiBlock, m_LBEnd));
      m_lumiBlock=lumiBlock;
      m_timeStamp=timeStamp;

   } else if ( m_source == "HIST" ) {

      msg( MSG::INFO ) << "RunNumber, TimeStamp (Begin, End): " << m_runNumber << ", " << m_timeStampBegin << ", " << m_timeStampEnd<< endmsg;

   } else msg( MSG::FATAL ) << "SCT_CalibEventInfo: Unknown source!" << endmsg;

}


void
SCT_CalibEventInfo::setTimeStamp(const int begin, const int end) {
   m_timeStampBegin=begin;
   m_timeStampEnd=end;
   m_duration=m_timeStampEnd-m_timeStampBegin;
   m_tsBeginString=toUtc(begin);
   m_tsEndString=toUtc(end);
}

int SCT_CalibEventInfo::duration() const {
   return m_duration;
}

void
SCT_CalibEventInfo::setTimeStamp(const std::string & begin, const std::string & end) {
   int ibegin=std::stoi(begin);
   int iend=std::stoi(end);
   setTimeStamp(ibegin,iend);
}

void
SCT_CalibEventInfo::setTimeStamp(const int ts) {
   m_timeStamp=ts;
}
void
SCT_CalibEventInfo::setLumiBlock(const int begin, const int end) {
   m_LBBegin=begin;
   m_LBEnd=end;
   m_numLB=end-begin+1;
}
void
SCT_CalibEventInfo::setLumiBlock(const int lb) {
   m_lumiBlock=lb;
}
void
SCT_CalibEventInfo::setSource(const std::string source) {
   m_source=source;
}
void
SCT_CalibEventInfo::setRunNumber(const int rn) {
   m_runNumber=rn;
}
void
SCT_CalibEventInfo::setBunchCrossing(const int bc) {
   m_bunchCrossing=bc;
}

void
SCT_CalibEventInfo::getTimeStamps(std::string & begin, std::string & end) const {
   begin=m_tsBeginString;
   end=m_tsEndString;
}

void
SCT_CalibEventInfo::getTimeStamps(int & begin, int & end) const {
   begin=m_timeStampBegin;
   end=m_timeStampEnd;
}

int
SCT_CalibEventInfo::timeStamp() const {
   return m_timeStamp;
}

int
SCT_CalibEventInfo::runNumber() const {
   return m_runNumber;
}

int
SCT_CalibEventInfo::counter() const {
   return m_counter;
}

void
SCT_CalibEventInfo::incrementCounter() {
   ++m_counter;
}

int
SCT_CalibEventInfo::UOFO() const {
   return m_numUOFO;
}

void
SCT_CalibEventInfo::incrementUOFO() {
   ++m_numUOFO;
}

void
SCT_CalibEventInfo::resetUOFO() {
   m_numUOFO=0;
}


void
SCT_CalibEventInfo::setCounter(const int counterVal) {
   m_counter=counterVal;
}

int
SCT_CalibEventInfo::numLumiBlocks() const {
   return m_numLB;
}

std::string
SCT_CalibEventInfo::toUtc(const int timestamp) const {
   coral::TimeStamp::ValueType nsTime = timestamp*oneBillion;
   coral::TimeStamp utc( nsTime );
   return utc.toString();
}
