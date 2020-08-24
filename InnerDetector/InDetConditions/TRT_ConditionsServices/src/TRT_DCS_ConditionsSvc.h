/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DCS_CONDITIONSSVC_H
#define TRT_DCS_CONDITIONSSVC_H

/** @file TRT_DCS_ConditionsSvc.h
 *  @brief Service to provide a simple Athena interface to read
 *  DCS conditions data from COOL
 * author Denver Whittington <Denver.Whittington@cern.ch>
 **/
#include <vector>
#include "AthenaBaseComps/AthService.h"
#include "CxxUtils/checker_macros.h"
#include "TRT_ConditionsServices/ITRT_DCS_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "TRT_ConditionsServices/ITRT_HWMappingSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

class StoreGateSvc;
class TRT_ID;
class Identifier;
class TH1D;

/// Service for accessing DCS conditions information
class TRT_DCS_ConditionsSvc : public AthService,
  virtual public ITRT_ConditionsSvc,
  virtual public ITRT_DCS_ConditionsSvc
  {

 public:

  TRT_DCS_ConditionsSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TRT_DCS_ConditionsSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  /// Returns the DCS conditions StatusFlag for a given identifier.
  InDet::TRT_DCS_StatusFlag getFlag( const Identifier );

  /// Returns the value for a given identifier and data type.
  /** Get the value for a given identifier and DCS conditions data type.
   */
  StatusCode getValue( const Identifier, const InDet::TRT_DCS_DataType,
		       InDet::TRT_DCS_ValueType& );

  /// Returns the value for a given folder/channel name.
  /** Get the value for a given folder/channel name.
   */
  StatusCode getValue( const std::string, const std::string, InDet::TRT_DCS_ValueType& );

  /// Returns the value for a given folder and COOL channel number.
  /** Get the value for a given folder and COOL channel number.
   */
  StatusCode getValue( const std::string, const int, InDet::TRT_DCS_ValueType& );

  const CondAttrListCollection* getCollection( const std::string);

  void monitorBarrel();
  void monitorEndcapA();
  void monitorEndcapC();


  /// @name Functions inherited from ITRT_ConditionsSvc
  //@{

  /// Evaluation for TRT_ConditionsSummarySvc
  /** Function inherited from ITRT_ConditionsSvc.
   *  Returns a InDet::TRT_CondFlag for the Identifier, as determined
   *  by the TRT_DCS_ConditionsSvc.
   */
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& );

  //@}

 private:

  // jobOptions properties
  bool m_VeryVerbose;
  std::string m_Barrel_HV_COOLFolderName;
  std::string m_EndcapA_HV_COOLFolderName;
  std::string m_EndcapC_HV_COOLFolderName;
  float m_HVWarnValLo;
  float m_HVWarnValHi;
  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ITRT_HWMappingSvc> m_mapSvc;
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this,"EventInfoKey","EventInfo","RHK for EventInfo"};
  // Conditions access
  ServiceHandle<ICondSvc> m_condSvc;
  SG::ReadCondHandleKey<CondAttrListCollection> m_barrelReadKey{this,"BarrelKeyName","in","HV Barrel in-key"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_EAReadKey{this,"EAKeyName","in","HV EA in-key"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_ECReadKey{this,"ECKeyName","in","HV EC in-key"};
  mutable std::vector<const CondAttrListCollection*> m_Barrel_HV_COOLCont ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex
  mutable std::vector<const CondAttrListCollection*> m_EndcapA_HV_COOLCont ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex
  mutable std::vector<const CondAttrListCollection*> m_EndcapC_HV_COOLCont ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex
  mutable std::mutex m_cacheMutex;
  mutable std::vector<EventContext::ContextEvt_t> m_evtBA ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex
  mutable std::vector<EventContext::ContextEvt_t> m_evtEA ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex
  mutable std::vector<EventContext::ContextEvt_t> m_evtEC ATLAS_THREAD_SAFE; // Guarded by m_cacheMutex

  int m_IOVmaxLength;
  bool m_doIOVchecking;
  bool m_FallBackOnCOOLChanNames;


  // Straw Helpers
  const TRT_ID* m_TRT_ID_Helper;

  /// Counters
  /** Counts the number of times getFlag returns each flag */
  int m_numFlagRED;
  int m_numFlagNOINFO;

  

  // Other private variables
  int m_currentTimestamp;

  // Monitoring histograms
  bool m_doMonitoring;
  TH1D* m_h_Barrel_nRED;
  TH1D* m_h_EndcapA_nRED;
  TH1D* m_h_EndcapC_nRED;
  TH1D* m_h_Barrel_nNOINFO;
  TH1D* m_h_EndcapA_nNOINFO;
  TH1D* m_h_EndcapC_nNOINFO;
  TH1D* m_h_Barrel_HVvalAvg; // don't forget to divide by nEvts at end!
  TH1D* m_h_EndcapA_HVvalAvg; // don't forget to divide by nEvts at end!
  TH1D* m_h_EndcapC_HVvalAvg; // don't forget to divide by nEvts at end!
  int m_nBAEvts;
  int m_nEAEvts;
  int m_nECEvts;
};

/// Query Interface
inline StatusCode TRT_DCS_ConditionsSvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) {
  if ( ITRT_ConditionsSvc::interfaceID() == riid ) {
    *ppvInterface = dynamic_cast<ITRT_ConditionsSvc*>(this);
    addRef();
  } else if ( ITRT_DCS_ConditionsSvc::interfaceID() == riid ) {
    *ppvInterface = dynamic_cast<ITRT_DCS_ConditionsSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvInterface );
  }
  return StatusCode::SUCCESS;
}

#endif // TRT_DCS_CONDITIONSSVC_H
