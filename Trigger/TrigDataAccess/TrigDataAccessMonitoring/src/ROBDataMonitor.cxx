/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <functional>
#include <algorithm>
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

using namespace robmonitor;

//
//--- ROBDataStruct 
//    -------------
ROBDataStruct::ROBDataStruct()
  : rob_id(0),
    rob_size(0),
    rob_history(robmonitor::UNCLASSIFIED),
    rob_status_words()
{}

ROBDataStruct::ROBDataStruct(const uint32_t srcId)
  : rob_id(srcId),
    rob_size(0),
    rob_history(robmonitor::UNCLASSIFIED),
    rob_status_words()
{}

bool ROBDataStruct::isUnclassified() const {
  return ((rob_history == robmonitor::UNCLASSIFIED) ? true : false);
}

bool ROBDataStruct::isCached() const {
  return ((rob_history == robmonitor::CACHED) ? true : false);
}

bool ROBDataStruct::isRetrieved() const {
  return ((rob_history == robmonitor::RETRIEVED) ? true : false);
}

bool ROBDataStruct::isIgnored() const {
  return ((rob_history == robmonitor::IGNORED) ? true : false);
}

bool ROBDataStruct::isDisabled() const {
  return ((rob_history == robmonitor::DISABLED) ? true : false);
}

bool ROBDataStruct::isScheduled() const {
  return ((rob_history == robmonitor::SCHEDULED) ? true : false);
}


bool ROBDataStruct::isStatusOk() const {
  if (rob_status_words.size() == 0) return true;
  if ((rob_status_words.size() > 0) && (rob_status_words[0] == 0)) return true;
  return false;
}


//
//--- ROBDataMonitorStruct
//    --------------------
ROBDataMonitorStruct::ROBDataMonitorStruct()
  :lvl1ID(0),
   requestor_name("UNKNOWN"),
   requested_ROBs(),
   start_time_of_ROB_request(),
   end_time_of_ROB_request()
{}

ROBDataMonitorStruct::ROBDataMonitorStruct(const uint32_t l1_id, const std::string req_nam="UNKNOWN")
  :lvl1ID(l1_id),
   requestor_name(req_nam),
   requested_ROBs(),
   start_time_of_ROB_request(),
   end_time_of_ROB_request()
{}

ROBDataMonitorStruct::ROBDataMonitorStruct(const uint32_t l1_id, 
					   const std::vector<uint32_t>& req_robs,
					   const std::string req_nam="UNKNOWN")
  :lvl1ID(l1_id),
   requestor_name(req_nam),
   start_time_of_ROB_request(),
   end_time_of_ROB_request()
{
  for ( std::vector<uint32_t>::const_iterator it=req_robs.begin(); it != req_robs.end(); it++) {
    requested_ROBs[ (*it) ] = robmonitor::ROBDataStruct( (*it) ) ;
  }
}

unsigned ROBDataMonitorStruct::allROBs() const {
  return requested_ROBs.size(); 
}

unsigned ROBDataMonitorStruct::unclassifiedROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isUnclassified()) ++ret;
  }     
  return ret;
} 

unsigned ROBDataMonitorStruct::cachedROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isCached()) ++ret;
  }     
  return ret;
}

unsigned ROBDataMonitorStruct::retrievedROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isRetrieved()) ++ret;
  }     
  return ret;
}

unsigned ROBDataMonitorStruct::ignoredROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isIgnored()) ++ret;
  }     
  return ret;
}

unsigned ROBDataMonitorStruct::disabledROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isDisabled()) ++ret;
  }     
  return ret;
}

unsigned ROBDataMonitorStruct::scheduledROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isScheduled()) ++ret;
  }     
  return ret;
}

unsigned ROBDataMonitorStruct::statusOkROBs() const {
  ptrdiff_t ret=0;
  for ( std::map<const uint32_t,robmonitor::ROBDataStruct>::const_iterator it = requested_ROBs.begin();
        it != requested_ROBs.end(); it++ ) {
    if ((*it).second.isStatusOk()) ++ret;
  }     
  return ret;
}

float ROBDataMonitorStruct::elapsedTime() const {
  int secs = 0 ;
  if (end_time_of_ROB_request.tv_sec >= start_time_of_ROB_request.tv_sec)
    secs = end_time_of_ROB_request.tv_sec - start_time_of_ROB_request.tv_sec;

  int usecs = end_time_of_ROB_request.tv_usec - start_time_of_ROB_request.tv_usec;
  return static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
}
