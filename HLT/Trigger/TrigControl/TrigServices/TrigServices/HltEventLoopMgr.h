/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

#include "TrigKernel/ITrigEventLoopMgr.h"

#include "GaudiKernel/MinimalEventLoopMgr.h"

class HltEventLoopMgr : public MinimalEventLoopMgr,
                        virtual public ITrigEventLoopMgr
{

public:
  
  /// Standard Constructor
  HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~HltEventLoopMgr();
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
  /// @name State transitions
  ///@{
  virtual StatusCode initialize();

  virtual StatusCode prepareForRun(const boost::property_tree::ptree & pt);
  
  virtual StatusCode hltUpdateAfterFork(const boost::property_tree::ptree & pt);
  ///@}
  
  virtual StatusCode
  processRoIs(const std::vector<eformat::ROBFragment<const uint32_t*> >& l1_result,
              hltinterface::HLTResult& hlt_result,
              const hltinterface::EventId& evId);
  
  virtual StatusCode timeOutReached(const boost::property_tree::ptree& pt);
  
  // ------------------------- Properties --------------------------------------
  
  // /// Source ID for CTP ROB fragment
  // IntegerProperty       m_lvl1CTPROBid;
  // /// Monitoring
  // BooleanProperty       m_doMonitoring;
  // 
  // /// Read magnet currents from ptree
  // BooleanProperty       m_setMagFieldFromPtree; 
  // /// Application Name (="None" or "athenaHLT" for simulated data, "HLTMPPU-xx"? in online environment)
  // StringProperty        m_applicationName;      
  // /// Partition Name (="None" for offline, real partion name in online environment)
  // StringProperty        m_partitionName;        
  // /// Force reject of all events 
  // BooleanProperty       m_forceHltReject;      
  // /// Force accept of all events
  // BooleanProperty       m_forceHltAccept;      
  // /// Name of HLT result in StoreGate
  // StringProperty        m_HltResultName;       
  // /// Stream name for Debug events 
  // StringProperty        m_HltDebugStreamName;  
  // /// Stream name for forced accept events
  // StringProperty        m_HltForcedStreamName; 
  // IntegerProperty       m_predefinedLumiBlock;
  // /// Max random sleep during prepareForRun in sec
  // IntegerProperty       m_prepareForRunSleep;    
// 
  // typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  // /// List of all enabled ROBs which can be retrieved
  // Uint32ArrayProperty   m_enabledROBs;         
  // /// List of all enabled Sub Detectors which can be retrieved
  // Uint32ArrayProperty   m_enabledSubDetectors; 
  // /// List of mandatory ROB IDs coming from the RoIB (must come in L1R seed)
  // Uint32ArrayProperty   m_mandatoryL1ROBs;     
// 
  // /// Names of all HLT EDM collections for histogram label 
  // StringArrayProperty   m_hltEdmCollectionNames; 
  // 
  // /// JobOptions type (="NONE" or "DB", same as in PSC)
  // StringProperty        m_jobOptionsType;        
/*
  Histo1DProperty       m_histProp_Hlt_result_size;
  Histo1DProperty       m_histProp_Hlt_result_size_physics;
  Histo1DProperty       m_histProp_Hlt_result_size_express;
  Histo1DProperty       m_histProp_Hlt_result_size_DataScouting;
  Histo1DProperty       m_histProp_numStreamTags;              
  Histo1DProperty       m_histProp_streamTagNames;  
  Histo1DProperty       m_histProp_num_partial_eb_robs;
  /// HLT EDM sizes profile plots 
  Histo1DProperty       m_histProp_Hlt_Edm_Sizes;       
*/
  // /// Check integrity of CTP ROB and put event to debug stream when it fails
  // BooleanProperty       m_lvl1CTPROBcheck ;
// 
  // /// Flag to write events with truncated HLT result to a special debug stream
  // BooleanProperty       m_writeHltTruncationToDebug ;
  // /// Debug stream name for events with a truncated HLT result 
  // StringProperty        m_HltTruncationDebugStreamName; 
  // /// Stream names for which events should be not send to the debug stream even when the HLT result is truncated
  // StringArrayProperty   m_excludeFromHltTruncationDebugStream; 

  
private:

};

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H
