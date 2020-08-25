/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_ROBREQUESTALGO_H
#define TRIGGENERICALGS_ROBREQUESTALGO_H

/**
 * @brief  Algorithm to perform predefined ROB requests
 * @author Frank Winklmeier
 *
 * $Id: $
 */

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/AllTEAlgo.h"

#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class IROBDataProviderSvc;

typedef std::vector<uint32_t> ROBList;

/**
 * Helper class to hold ROB list
 */
struct ROBRequest {
  ROBRequest() : N(-1) {}
  void select(ROBList& roblist);
  ROBList robs;     //!< ROB list
  int N;            //!< select N ROBs from list (-1 = all)
};

/**
 * Algorithm to perform predefined ROB requests.
 *
 * The ROBs to be requested are definined in the 'ROBRequest' property,
 * which is a list of strings of the following format:
 *   SPEC[,SPEC]*    with SPEC = ROBSPEC[#N]
 *
 * For each list entry, a separate ROB request is performed.
 *
 * ROBSPEC: (partial) ROB ID to be requested
 *          If less than 3 bytes, match highest bytes against enabled ROBs
 * N      : Randomly request N ROBs matching ROBSPEC
 *
 * Examples:
 *  '0x11'            request all ROBs starting with 0x11
 *  '0x11, 0x12'      ROBs starting with 0x11 or 0x12 within one ROB request
 *  '0x11a#2'         2 random ROBs out of 0x11a*
 *  '0x234234'        this ROB
 *  'ROS-0x1c36#3'    3 random ROBs from ROS with NodeID 0x1c36
 *  '0x11a#2,0x12#5'  INVALID (cannot use more than one random selection per request)
 */
class ROBRequestAlgo : public HLT::AllTEAlgo {
public:
  ROBRequestAlgo(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize()   { return HLT::OK; }

  /// Run once per event
  HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>&, unsigned int); 

  void updateHandler(Gaudi::Details::PropertyBase& p);
  
private:
  void parseROBRequest();
  bool getROBs(const std::string& robSpec, ROBRequest& req);
  
  std::vector< std::vector<ROBRequest> >  m_requestList;
  SimpleProperty< ROBList >               m_enabledROBs;
  ServiceHandle<IROBDataProviderSvc>      m_robDataProviderSvc;
  std::map< int, ROBList >                m_ros_rob_map;   // ROS->ROB map
 
  // Properties
  StringArrayProperty                  m_robRequest;
  SimpleProperty< std::map<int,int> >  m_rob_ros_map ;
};

#endif
