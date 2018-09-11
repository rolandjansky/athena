/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"

#include "TrigTimeAlgs/TrigTimeStamp.h"

#include "ITrigCostMTSvc.h"
#include "TrigCostDataStore.h"

// Forward declaration
template <class TYPE> class SvcFactory;

/////////////////////////////////////////////////////////////////////////////
class TrigCostMTSvc : public AthService, virtual public  ITrigCostMTSvc {
  public:
  TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigCostMTSvc();

  /// Required of all Gaudi services: 
  StatusCode initialize() override;
  StatusCode finalize() override;
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  inline static const InterfaceID& interfaceID() { return ITrigCostMTSvc::interfaceID(); }

  // ITrigCostMTSvc
  StatusCode processAlg(const std::string& caller, const EventContext& context, const AuditType type) override; 
  StatusCode endEvent(const EventContext& context) override; 

  protected:
  friend class SvcFactory<TrigCostMTSvc>;

  private: 
  /// Default constructor: 
  TrigCostMTSvc() = delete; 

  Gaudi::Property<bool> m_monitorAll{this, "MonitorAll", true, "Monitor every HLT event, e.g. for offline validation."};
  Gaudi::Property<bool> m_printTimes{this, "PrintTimes", true, "Sends per-algorithm timing to MSG::INFO."};
  Gaudi::Property<int>  m_moitorFrequency{this, "MonitorFrequency", 10, "A value of 10 would monitor every 10th event. > 0."};

  bool isMonitoredEvent(const EventContext& context) const;

  StatusCode getAlgTimeFromStore(const std::string& caller, const EventContext& context, double& algTime) const;


  TrigCostDataStore m_algStartTimes;
  TrigCostDataStore m_algStopTimes;

};

#endif // TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H
