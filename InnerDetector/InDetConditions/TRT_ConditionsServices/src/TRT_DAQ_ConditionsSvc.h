/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DAQ_CONDITIONSSVC_H
#define TRT_DAQ_CONDITIONSSVC_H

/*----------------------------------------------------
 *  @file TRT_DAQ_ConditionsSvc.h
 *  @Service Header for TRT_DAQ_ConditionsSvc
 *  @author Denver Whittington
 *///-------------------------------------------------

// Header Includes
#include "AthenaBaseComps/AthService.h"
#include "TRT_ConditionsServices/ITRT_DAQ_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"

class CondAttrListCollection;

// Class definition for this AlgTool
class TRT_DAQ_ConditionsSvc : public AthService,
  virtual public ITRT_ConditionsSvc,
  virtual public ITRT_DAQ_ConditionsSvc,
  virtual public IIncidentListener
{

 public:

  TRT_DAQ_ConditionsSvc( const std::string&, ISvcLocator* );
  virtual ~TRT_DAQ_ConditionsSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Return ROD identifier
  unsigned int RODid( const Identifier& );

  /// Query Interface
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  /// Handle IncidentSvc callbacks
  void handle( const Incident& );

  /// ConditionsSummaryStatus for a ROD ID
  /** Similar to ITRT_ConditionsSvc::condSummaryStatus,
   *  but specific to this service
   */
  InDet::TRT_CondFlag condSummaryStatus( unsigned int );

  /// @name Functions inherited from ITRT_ConditionsSvc
  //@{

  /// Evaluation for TRT_ConditionsSummarySvc
  /** Function inherited from ITRT_ConditionsSvc.
   *  Returns a InDet::TRT_CondFlag for the Identifier, as determined
   *  by the TRT_DAQ_ConditionSvc.
   */
  InDet::TRT_CondFlag condSummaryStatus( const Identifier& );

 private:

  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<StoreGateSvc> m_detStore;

  // jobOptions properties
  std::string m_FolderName;

  // Straw Helpers
  const TRT_ID* m_TRT_ID_Helper;

  // Pointer to COOL Folder
  const CondAttrListCollection* m_EnabledRods;

};

/// Query Interface
inline StatusCode TRT_DAQ_ConditionsSvc::queryInterface( const InterfaceID& riid, void** ppvInterface ) {
  if ( ITRT_ConditionsSvc::interfaceID() == riid ) {
    *ppvInterface = dynamic_cast<ITRT_ConditionsSvc*>(this);
    addRef();
  } else if ( ITRT_DAQ_ConditionsSvc::interfaceID() == riid ) {
    *ppvInterface = dynamic_cast<ITRT_DAQ_ConditionsSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvInterface );
  }
  return StatusCode::SUCCESS;
}

#endif // TRT_DAQ_CONDITIONSSVC_H
