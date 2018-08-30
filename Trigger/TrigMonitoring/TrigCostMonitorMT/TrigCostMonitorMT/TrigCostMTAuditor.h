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
class TrigCostMTAuditor : public Auditor, public AthMessaging {

  using AthMessaging::msg;

  public:

  /** Constructor
      @param name    The algorithm object's name
      @param svcloc  A pointer to a service location service */
  TrigCostMTAuditor( const std::string& name, ISvcLocator* svcloc );

  StatusCode initialize() override;
  StatusCode finalize() override;

  /// StandardEventType = { Initialize, ReInitialize, Execute, BeginRun, EndRun, Finalize, Start, Stop, ReStart };
  /// typedef std::string CustomEventType 
 
  void before( StandardEventType evt, const std::string& caller) override;
  void before( CustomEventTypeRef evt, const std::string& caller) override;
  void after( StandardEventType evt, const std::string&, const StatusCode& sc) override;
  void after( CustomEventTypeRef evt, const std::string&, const StatusCode& sc) override;


  /// Redirect the proper calls using INamedInterface
  inline void before( StandardEventType evt, INamedInterface* caller) override {
    return this->before(evt, caller->name());
  }
  inline void before( CustomEventTypeRef evt, INamedInterface* caller) override {
    return this->before(evt, caller->name());
  }
  inline void after( StandardEventType evt, INamedInterface* caller, const StatusCode& sc) override {
    return this->after(evt, caller->name(), sc);
  }
  inline void after( CustomEventTypeRef evt, INamedInterface* caller, const StatusCode& sc) override {
    return this->after(evt, caller->name(), sc);
  }

private:

  // Todo - interface here
  ServiceHandle<ITrigCostMTSvc> m_trigCostSvcHandle { this, "TrigCostMTSvc", "TrigCostMTSvc", "The trigger cost service to pass audit information to" };
};

#endif // TRIGCOSTMONITORMT_TRIGCOSTMTAUDITOR_H
