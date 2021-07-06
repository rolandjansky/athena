/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
using namespace std;
using namespace robmonitor;
int main() {
  // create list
  ROBDataMonitorStruct req;
  req.lvl1ID = 12345 ;
  req.requestor_name = "MyAlgorithm";
  gettimeofday(&(req.start_time_of_ROB_request), 0);
  gettimeofday(&(req.end_time_of_ROB_request), 0);

  // individual ROB records
  ROBDataStruct u(0x770000);
  u.rob_history = robmonitor::UNCLASSIFIED;
  u.rob_size = 10000; 
  ROBDataStruct c(0x760000);
  c.rob_history = robmonitor::HLT_CACHED;
  c.rob_status_word = 0;
  c.rob_size = 25000; 
  ROBDataStruct c1(0x760001);
  c1.rob_history = robmonitor::HLT_CACHED;
  c1.rob_status_word = 0x08;
  c1.rob_size = 125000; 

  ROBDataStruct i(0x740001);
  i.rob_history = robmonitor::IGNORED;
  ROBDataStruct r(0x750001);
  r.rob_history = robmonitor::RETRIEVED;
  r.rob_status_word = 0x04;
  ROBDataStruct d(0x730001);
  d.rob_history = robmonitor::UNDEFINED;

  // fill ROBDataMonitorStruct with ROB requests
  req.requested_ROBs[u.rob_id]  = u;
  req.requested_ROBs[c.rob_id]  = c;
  req.requested_ROBs[c1.rob_id] = c1;
  req.requested_ROBs[r.rob_id]  = r;

  r.rob_id = 0x750002;
  req.requested_ROBs[r.rob_id]  = r;
  req.requested_ROBs[d.rob_id]  = d;
  c.rob_id = 0x760002;
  req.requested_ROBs[c.rob_id]  = c;

  req.requested_ROBs[i.rob_id]  = i;
  c.rob_id = 0x760003;
  req.requested_ROBs[c.rob_id]  = c;
  i.rob_id = 0x740002;
  req.requested_ROBs[i.rob_id]  = i;

  // print out everything
  std::cout << "\n ROBDataMonitorStruct req = \n\n" << "  " << req << std::endl;
}
