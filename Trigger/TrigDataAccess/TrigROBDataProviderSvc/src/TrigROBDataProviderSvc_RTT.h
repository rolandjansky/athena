/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2SERVICES_TRIGROBDATAPROVIDERSVC_RTT_H
#define TRIGL2SERVICES_TRIGROBDATAPROVIDERSVC_RTT_H

/** ===============================================================
 *     TrigROBDataProviderSvc_RTT.h
 *  ===============================================================
 *    Description:  ROBDataProvider class for simulating the ROB data
 *                  access offline in the merged L2/EF system after LS1.
 *
 *                  This class allows to monitor the behaviour of single algorithms
 *                  creating a map between the Algorithm name and the ROBIds
 *                  that have been declared with addROBData
 *                  If this is done during the pre-fetching phase, the algorithm 
 *                  name is added with the string "_pref".
 *
 *    Created:      Mar 27, 2014
 *         By:     F. Pastore
 */ 
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/HistoProperty.h"
#include "GaudiKernel/Property.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
#include "src/TrigROBDataProviderSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc_RTT.h"


#include <vector>
#include <map>
#include <iostream>

// Forward declarations
class StoreGateSvc;  
class IAlgContextSvc;
class TH1F;   
class TH2F;   

class TrigROBDataProviderSvc_RTT : public TrigROBDataProviderSvc,  
  virtual public ITrigROBDataProviderSvc_RTT,
  virtual public IIncidentListener
{
 public:
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF ; 

  TrigROBDataProviderSvc_RTT(const std::string& name, ISvcLocator* svcloc) ;

  ~TrigROBDataProviderSvc_RTT(void){};

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;

  virtual StatusCode initialize() override;

  // fill histograms per event
  void FillEvent();

  /// set the name of the program which uses the ROBDataProviderSvc
  virtual void setCallerName(const std::string_view) override;

  //declare ROBdata // inherited from base class
  using ROBDataProviderSvc::addROBData;
  virtual void addROBData(const std::vector<uint32_t>& robIds,
                          const std::string_view callerName="UNKNOWN") override;

  /// Retrieve ROBFragments for given ROB ids from cache 
  using ROBDataProviderSvc::getROBData;
  virtual void getROBData(const std::vector<uint32_t>& robIds, 
                          std::vector<const ROBF*>& robFragments,
                          const std::string_view callerName="UNKNOWN") override;
 
  /// Add a given LVL1 ROBFragment to cache 
  using ROBDataProviderSvc::setNextEvent;
  virtual void setNextEvent(const std::vector<ROBF>& result) override;

  /// Add all ROBFragments of a RawEvent to cache 
  virtual void setNextEvent(const RawEvent* re) override;


  virtual void handle(const Incident& incident) override;

  /// Check if complete event data are already in cache
  virtual bool isEventComplete() override { return TrigROBDataProviderSvc::isEventComplete(); }
  virtual bool isEventComplete() const override { return TrigROBDataProviderSvc::isEventComplete(); }
  virtual bool isEventComplete(const EventContext& ctx) const override
  { return TrigROBDataProviderSvc::isEventComplete(ctx); }

  /// Collect all data for an event from the ROS and put them into the cache
  /// Return value: number of ROBs which were retrieved to complete the event
  /// Optinonally the name of the caller of this method can be specified for cost monitoring
  virtual int collectCompleteEventData(const std::string_view callerName="UNKNOWN") override
  { return TrigROBDataProviderSvc::collectCompleteEventData (callerName); }
  virtual int collectCompleteEventData(const EventContext& ctx, const std::string_view callerName="UNKNOWN") override
  { return TrigROBDataProviderSvc::collectCompleteEventData (ctx, callerName); }

  virtual bool isMissingPrefetching() override     { return 0;};// m_missingPrefetchingPerEvent; };

  virtual bool isPrefetchingAtAlgoLevel() override { return m_enablePrefetchingAtAlgoLevel;};

  void print(std::ostream& os) const;

  friend  std::ostream& operator << (std::ostream&, const TrigROBDataProviderSvc_RTT&);

 private:

  // parameter to enable the prefetching at Algorithm level
  bool  m_enablePrefetchingAtAlgoLevel;

  typedef std::multimap<std::string, std::vector <uint32_t> > CALLER_ROBMAP;// map between each algo call and its ROBIds
  CALLER_ROBMAP m_caller_robmap; 


  // histogram prperties
  Histo1DProperty m_histProp_requestedROBsPerAlgo;
  Histo1DProperty m_histProp_requestPerAlgo;
  Histo1DProperty m_histProp_callerPerEvent;
  Histo1DProperty m_histProp_nocachedRequestedROBsPerAlgo;
  Histo1DProperty m_histProp_nocachedRequestPerAlgo;
  Histo1DProperty m_histProp_declaredROBsPerAlgo;
  Histo1DProperty m_histProp_prefetchedROBsPerAlgo;

  Histo1DProperty m_histProp_missingRequestedROBsPerAlgo;
  Histo1DProperty m_histProp_missingRequestPerAlgo;
  Histo1DProperty m_histProp_missingRequestedROBsPerCall;

  Histo1DProperty m_histProp_missingRequestedROBsPerAlgo_pref;
  Histo1DProperty m_histProp_missingRequestPerAlgo_pref;
  Histo1DProperty m_histProp_missingRequestedROBsPerCall_pref;

  Histo1DProperty m_histProp_missingRequestedROBsPerAlgoButCached;
  Histo1DProperty m_histProp_missingRequestPerAlgoButCached;
  Histo1DProperty m_histProp_missingRequestedROBsPerCallButCached;

  Histo1DProperty m_histProp_missingRequestedROBsPerAlgoButCached_pref;
  Histo1DProperty m_histProp_missingRequestPerAlgoButCached_pref;
  Histo1DProperty m_histProp_missingRequestedROBsPerCallButCached_pref;

  //histograms
  TH1F* m_hist_requestedROBsPerAlgo;
  TH2F* m_hist_requestedROBsPerCallPerAlgo;
  TH1F* m_hist_requestPerAlgo;
  TH1F* m_hist_callerPerEvent;

  TH1F* m_hist_nocachedRequestedROBsPerAlgo;
  TH1F* m_hist_nocachedRequestPerAlgo;

  TH1F* m_hist_declaredROBsPerAlgo;
  TH1F* m_hist_prefetchedROBsPerAlgo;

  // histogrmas for missing declarations
  TH1F* m_hist_missingRequestedROBsPerAlgo;
  TH1F* m_hist_missingRequestPerAlgo;
  TH1F* m_hist_missingRequestedROBsPerCall;

  //hitogrmas for missing declarations during prefetching
  TH1F* m_hist_missingRequestedROBsPerAlgo_pref;
  TH1F* m_hist_missingRequestPerAlgo_pref;
  TH1F* m_hist_missingRequestedROBsPerCall_pref;

  // histogrmas for missing declarations, but cached
  TH1F* m_hist_missingRequestedROBsPerAlgoButCached;
  TH1F* m_hist_missingRequestPerAlgoButCached;
  TH1F* m_hist_missingRequestedROBsPerCallButCached;

  //hitogrmas for missing declarations during prefetching, but cached
  TH1F* m_hist_missingRequestedROBsPerAlgoButCached_pref;
  TH1F* m_hist_missingRequestPerAlgoButCached_pref;
  TH1F* m_hist_missingRequestedROBsPerCallButCached_pref;

 
};
#endif
