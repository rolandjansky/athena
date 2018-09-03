/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTMTAUDITOR_H
#define TRIGCOSTMONITORMT_TRIGCOSTMTAUDITOR_H

#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

#include "ITrigCostMTSvc.h"

/////////////////////////////////////////////////////////////////////////////
class TrigCostMTAuditor : virtual public Auditor, public AthMessaging {

  using AthMessaging::msg;

  public:

  /// Constructor
  /// @param name    The algorithm object's name
  /// @param svcloc  A pointer to a service location service
  TrigCostMTAuditor( const std::string& name, ISvcLocator* svcloc );

  StatusCode initialize() override;
  StatusCode finalize() override;

  /// Note: StandardEventType = { Initialize, ReInitialize, Execute, BeginRun, EndRun, Finalize, Start, Stop, ReStart };
  
  /// Implimentation from Auditor
  void before(StandardEventType evt, const std::string& caller) override;
  void after(StandardEventType evt, const std::string&, const StatusCode& sc) override;

  /// Redirect the proper calls using INamedInterface
  inline void before(StandardEventType evt, INamedInterface* caller) override {
    return this->before(evt, caller->name());
  }
  inline void after(StandardEventType evt, INamedInterface* caller, const StatusCode& sc) override {
    return this->after(evt, caller->name(), sc);
  }

  /// Custom event type callbacks are not currently used
  /// Note: CustomEventType = std::string 
  void before(CustomEventTypeRef, const std::string&) override {}
  void after(CustomEventTypeRef, const std::string&, const StatusCode&) override {}
  void before(CustomEventTypeRef, INamedInterface*) override {}
  void after(CustomEventTypeRef, INamedInterface*, const StatusCode&) override {}

private:

  void callService(const std::string& caller, ITrigCostMTSvc::AuditType type);

  ServiceHandle<ITrigCostMTSvc> m_trigCostSvcHandle { this, "TrigCostMTSvc", "TrigCostMTSvc", "The trigger cost service to pass audit information to" };
};

#endif // TRIGCOSTMONITORMT_TRIGCOSTMTAUDITOR_H
