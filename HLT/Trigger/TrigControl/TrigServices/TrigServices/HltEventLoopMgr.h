/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

#include "TrigKernel/ITrigEventLoopMgr.h"
#include "EventInfo/EventID.h"  /* number_type */

#include "GaudiKernel/MinimalEventLoopMgr.h"

// Forward declarations
class IIncidentSvc;
class IAlgContextSvc;
class StoreGateSvc;
class IROBDataProviderSvc;
class CondAttrListCollection;
class TrigCOOLUpdateHelper;

class HltEventLoopMgr : public MinimalEventLoopMgr,
                        virtual public ITrigEventLoopMgr
{

public:
  
  /// Standard constructor
  HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
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
  
private:

  /// read DataFlow configuration properties
  void updateDFProps();
  
  /// Helper to do whatever is necessary with RunParams (prepare) ptree
  const CondAttrListCollection* processRunParams(const boost::property_tree::ptree & pt);

  
  // ------------------------- Handles to required services/tools --------------
  typedef ServiceHandle<IIncidentSvc> IIncidentSvc_t;
  IIncidentSvc_t m_incidentSvc;
  
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_evtStore;
  StoreGateSvc_t m_detectorStore;
  StoreGateSvc_t m_inputMetaDataStore;

  
  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  IIROBDataProviderSvc_t m_robDataProviderSvc;
  
  ToolHandle<TrigCOOLUpdateHelper> m_coolHelper;
  
  // ------------------------- Pointers to optional services/tools -------------
  IAlgContextSvc* m_algContextSvc{0};


  // ------------------------- Properties --------------------------------------
  /// Application Name (="None" or "athenaHLT" for simulated data, "HLTMPPU-xx"? in online environment)
  StringProperty m_applicationName;
  /// Partition Name (="None" for offline, real partion name in online environment)
  StringProperty m_partitionName;
  /// JobOptions type (="NONE" or "DB", same as in PSC)
  StringProperty m_jobOptionsType;
  
  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  /// list of all enabled ROBs which can be retrieved
  Uint32ArrayProperty m_enabledROBs;
  /// list of all enabled Sub Detectors which can be retrieved
  Uint32ArrayProperty m_enabledSubDetectors;

  // ------------------------- Other private members ---------------------------
  /// Current run number
  EventID::number_type m_currentRun{0};

};

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H
