/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_L1CORRELATIONALGO_H
#define TRIGGENERICALGS_L1CORRELATIONALGO_H

/**
 * @brief  Algorithm to correlate L1 items across neighbouring BCs
 * @author Nils Ruthmann
 *
 * $Id: $
 */

//#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/AllTEAlgo.h"

#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

/// for muon tests
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigMuonToolInterfaces/ITrigMuonRoITool.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

class IROBDataProviderSvc;

typedef std::vector<uint32_t> ROBList;


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
class L1CorrelationAlgo : public HLT::AllTEAlgo {
public:
  L1CorrelationAlgo(const std::string& name, ISvcLocator* pSvcLocator);

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize()   { return HLT::OK; }

  /// Run once per event
  HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>&, unsigned int output); 

  void updateHandler(Gaudi::Details::PropertyBase& p);
  
private:
  std::vector<uint32_t> m_bitmasks;

  std::vector<uint32_t> m_bitmasks_ele;
  std::vector<uint32_t> m_bitmasks_mu;
  std::vector<uint32_t> m_bitmasks_jets;

  // monitoring:
  int m_beforeafterflag;
  int m_l1a_type;
  int m_other_type;

  std::vector<std::string> m_l1itemlist;

  // Ideally would like to set the following variables in the init phase
  // but need a first ctpfragement to detect all these.. can I do it on first execute? Probably not ? 
  /* int32_t m_fragment_size ; */
  /* int32_t m_header_size ; */
  /* int32_t m_trailer_size ; */
  /* int32_t m_status_size ; */
  /* int32_t m_payload_size ; */
  /* int32_t m_timestampwords_size; */
  /* int32_t m_extrawords_size; */
  /* uint32_t m_ctpFormatVersion; */
  /* int m_nitems; */
  
  bool m_nomuon;
  bool m_currentBCincl;
  std::vector<float> m_etaOutOfTimeMuon;
  std::vector<float> m_phiOutOfTimeMuon;
  std::vector<float> m_ptThrOutOfTimeMuon;
  /* void parseROBRequest(); */
  /* bool getROBs(const std::string& robSpec, ROBRequest& req); */
  
  /* std::vector< std::vector<ROBRequest> >  m_requestList; */
  /* SimpleProperty< ROBList >               m_enabledROBs; */
   ServiceHandle<IROBDataProviderSvc>      m_robDataProviderSvc; 
  /* std::map< int, ROBList >                m_ros_rob_map;   // ROS->ROB map */
  /* // Properties */
  /* StringArrayProperty                  m_robRequest; */
  /* SimpleProperty< std::map<int,int> >  m_rob_ros_map ; */

   ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool;
   ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
   ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;
   ServiceHandle<TrigConf::ITrigConfigSvc > m_configSvc;

   xAOD::TrigCompositeContainer * m_passBitContainer ;    //!
};

#endif

