//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ROBDATAMONITOR_H
#define ROBDATAMONITOR_H

#include "eformat/SourceIdentifier.h"
#include <time.h>
#include <chrono>
#include <map>
#include <iostream>
#include <iomanip>

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

namespace robmonitor {

  /**
   * A structure with data about ROB properties 
   */
  enum ROBHistory { 
    UNCLASSIFIED      = 0,  // ROB was requested but never arrived at processor. History unknown. 
    SCHEDULED         = 1,  // ROB was scheduled before retrieveing
    RETRIEVED         = 2,  // ROB was retrieved from ROS by DataCollector
    CACHED            = 4,  // ROB was found already in the internal cache of the ROBDataProviderSvc
    IGNORED           = 8,  // ROB was on the "ignore" list and therefore not retrieved 
    DISABLED          = 16,  // ROB was disabled in OKS and therefore not retrieved
    NUM_ROBHIST_CODES = 6   // number of different history codes
  };

  /**
   * A structure with data about ROB properties 
   */
  class ROBDataStruct {
  public:

    /** @brief default constructor */
    ROBDataStruct();

    /** @brief constructor
        @param  ROB Source ID
     */                                   
    ROBDataStruct(const uint32_t);

    // data variables
    uint32_t rob_id;                           // rob source id
    uint32_t rob_size;                         // size of rob in words
    robmonitor::ROBHistory rob_history;        // History of ROB retrieval
    std::vector<uint32_t> rob_status_words;    // all status words in the ROB header


    // Accessor functions
    /** @brief ROB is unclassified */
    bool isUnclassified() const;
    /** @brief ROB was found in cache */
    bool isCached() const;
    /** @brief ROB was retrieved over network */
    bool isRetrieved() const;
    /** @brief ROB was ignored */
    bool isIgnored() const;
    /** @brief ROB was disabled in OKS */
    bool isDisabled() const;
    /** @brief ROB was scheduled over network */
    bool isScheduled() const;
    /** @brief ROB has no status words set */
    bool isStatusOk() const;

    // Extraction operators
    friend std::ostream& operator<<(std::ostream& os, const robmonitor::ROBDataStruct& rhs);
  };

  /**
   * The structure which is used to monitor the ROB data request in L2
   * It is created for every addROBData call 
   */
  class ROBDataMonitorStruct {
  public:

    /** @brief default constructor */
    ROBDataMonitorStruct();

    /** @brief constructor
        @param  L1 ID
	@param  requestor name 
     */                                   
    ROBDataMonitorStruct(const uint32_t, const std::string);             

    /** @brief constructor
        @param  L1 ID
	@param  vector of ROB Ids
	@param  requestor name 
     */                                   
    ROBDataMonitorStruct(const uint32_t, const std::vector<uint32_t>&, const std::string);

    // data variables
    uint32_t lvl1ID;                                                    //current L1 ID from L1 ROBs
    std::string requestor_name;                                         //name of requesting algorithm
    std::map<const uint32_t,robmonitor::ROBDataStruct> requested_ROBs;  //map of ROBs requested
    struct timeval start_time_of_ROB_request;                           //start time of ROB request 
    struct timeval end_time_of_ROB_request;                             //stop  time of ROB request

    // Accessor functions to ROB history summaries
    /** @brief number of ROBs in structure */
    unsigned allROBs() const;
    /** @brief number of unclassified ROBs in structure */
    unsigned unclassifiedROBs() const;
    /** @brief number of cached ROBs in structure */
    unsigned cachedROBs() const;
    /** @brief number of retrieved ROBs in structure */
    unsigned retrievedROBs() const;
    /** @brief number of ignored ROBs in structure */
    unsigned ignoredROBs() const;
    /** @brief number of disabled ROBs in structure */
    unsigned disabledROBs() const;
    /** @brief number of scheduled ROBs in structure */
    unsigned scheduledROBs() const;
    /** @brief number of ROBs with no status words set in structure */
    unsigned statusOkROBs() const;

    /** @brief elapsed time for ROB request in [ms] */
    float elapsedTime() const;

    // Extraction operators
    friend std::ostream& operator<<(std::ostream& os, robmonitor::ROBDataMonitorStruct& rhs);
  };

  // Extraction operator for ROBDataStruct 
  inline std::ostream& operator<<(std::ostream& os, const robmonitor::ROBDataStruct& rhs) {
    os << "[SourceID,Size(words),History,(Status words)]=[" 
       << std::hex <<  std::setfill( '0' ) << "0x" << std::setw(6) << rhs.rob_id  
       << std::dec << std::setfill(' ')
       << "," << std::setw(8) << rhs.rob_size;
    os << "," << std::setw(12);
    if (rhs.rob_history == robmonitor::UNCLASSIFIED) {
      os << "UNCLASSIFIED";
    } else if (rhs.rob_history == robmonitor::RETRIEVED) {
      os << "RETRIEVED";
    } else if (rhs.rob_history == robmonitor::CACHED) {
      os << "CACHED";
    } else if (rhs.rob_history == robmonitor::IGNORED) {
      os << "IGNORED";
    } else if (rhs.rob_history == robmonitor::DISABLED) {
      os << "DISABLED";
   } else if (rhs.rob_history == robmonitor::SCHEDULED) {
      os << "SCHEDULED";

    } else {
      os << "invalid code";
    }
    os << ",(";
    for (uint32_t i(0); i < rhs.rob_status_words.size(); ++i) {
      if (i > 0) os << ",";
      os << std::hex <<  std::setfill( '0' ) << "0x" << std::setw(8) << rhs.rob_status_words[i];
    }
    os << ")]";
    return os;
  }

  // Extraction operator for ROBDataMonitorStruct
  inline std::ostream& operator<<(std::ostream& os, robmonitor::ROBDataMonitorStruct& rhs) {
    std::string prefix("   ");
    std::string prefix2("-> ");
    os << "ROB Request for L1 ID = " << std::dec << rhs.lvl1ID << " (decimal), L1 ID = 0x" 
       << std::hex << rhs.lvl1ID << " (hex)" << std::dec;  
    os << "\n" << prefix << "Requestor name = " << rhs.requestor_name;

    const std::time_t s_time(rhs.start_time_of_ROB_request.tv_sec);
    struct tm buf;
    localtime_r(&s_time, &buf);
    os << "\n" << prefix << "Start time of ROB request         = " 
       << std::put_time(&buf, "%c")
       << " + " << static_cast<float>(rhs.start_time_of_ROB_request.tv_usec)/1000 << " [ms]";

    const std::time_t e_time(rhs.end_time_of_ROB_request.tv_sec);
    localtime_r(&e_time, &buf);
    os << "\n" << prefix << "Stop  time of ROB request         = "
       << std::put_time(&buf, "%c")
       << " + " << static_cast<float>(rhs.end_time_of_ROB_request.tv_usec)/1000 << " [ms]";
    os << "\n" << prefix << "Elapsed time for ROB request [ms] = " << rhs.elapsedTime();
    os << "\n" << prefix << "Requested ROBs:";
    os << "\n" << prefix << prefix2 << "All          " << rhs.allROBs()          ;
    os << "\n" << prefix << prefix2 << "Unclassified " << rhs.unclassifiedROBs() ;
    os << "\n" << prefix << prefix2 << "Cached       " << rhs.cachedROBs()       ;
    os << "\n" << prefix << prefix2 << "Retrieved    " << rhs.retrievedROBs()    ;
    os << "\n" << prefix << prefix2 << "Ignored      " << rhs.ignoredROBs()      ;
    os << "\n" << prefix << prefix2 << "Disabled     " << rhs.disabledROBs()     ;
    os << "\n" << prefix << prefix2 << "Scheduled     " << rhs.scheduledROBs()     ;
    os << "\n" << prefix << prefix2 << "Status OK    " << rhs.statusOkROBs()     ;
    for (std::map<const uint32_t,robmonitor::ROBDataStruct>::iterator it=rhs.requested_ROBs.begin();
	 it != rhs.requested_ROBs.end(); ++it) {
      os << "\n" << prefix << prefix2 << (*it).second;
    }
    return os;
  }
} // end namespace robmonitor

typedef DataVector<robmonitor::ROBDataMonitorStruct> ROBDataMonitorCollection;

CLASS_DEF( ROBDataMonitorCollection , 1303465505 , 1 )


#endif /* ROBDATAMONITOR_H */
