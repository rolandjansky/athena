/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITOR_TRIGCOSTAUDITOR_H
#define TRIGCOSTMONITOR_TRIGCOSTAUDITOR_H 1

#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

#include "ITrigCostSvc.h"

/**
 * @class TrigCostAuditor
 * @brief Gaudi Auditor implementation to hook algorithm executions and notify the Trigger Cost Service
 *
 * Only monitors the Execute event type from the available StandardEventTypes: 
 * {Initialize, ReInitialize, Execute, BeginRun, EndRun, Finalize, Start, Stop, ReStart}
 */
class TrigCostAuditor : virtual public Auditor, public AthMessaging {

  using AthMessaging::msg;
  using Auditor::after; //!< Inherit CustomEventType as a noop
  using Auditor::before;  //!< Inherit CustomEventType as a noop

  public:

  /**
   * @brief Standard Gaudi Auditor constructor
   * @param[in] name The algorithm object's name
   * @param[in] svcloc A pointer to a service location service
   */
  TrigCostAuditor( const std::string& name, ISvcLocator* svcloc );

  /**
   * @brief Initialise auditor. Return handle to Trigger Cost Service
   * @return Success if service handle obtained
   */
  virtual StatusCode initialize() override;

  /**
   * @brief Does nothing
   * @return Success
   */
  virtual StatusCode finalize() override;


  /**
   * @brief Audit before an algorithm standard event type is called
   * @param[in] evt The event type. Only Execute is monitored
   * @param[in] caller The name of the calling algorithm
   */
  virtual void before(StandardEventType evt, const std::string& caller) override;

  /**
   * @brief Audit after an algorithm standard event type is called
   * @param[in] evt The event type. Only Execute is monitored
   * @param[in] caller The name of the calling algorithm
   */
  virtual void after(StandardEventType evt, const std::string&, const StatusCode& sc) override;

  /**
   * @brief Redirects to the const std::string& interface
   */
  virtual void before(StandardEventType evt, INamedInterface* caller) override;

  /**
   * @brief Redirects to the const std::string& interface
   */
  virtual void after(StandardEventType evt, INamedInterface* caller, const StatusCode& sc) override;

private:

  /**
   * @brief Performs internal call to the trigger cost service
   * @param[in] caller Name of algorithm being audited
   * @param[in] type AuditType::Before or AuditType::After depending on if the start or stop of execution
   */
  void callService(const std::string& caller, ITrigCostSvc::AuditType type);

  ServiceHandle<ITrigCostSvc> m_trigCostSvcHandle { this, "TrigCostSvc", "TrigCostSvc", 
    "The trigger cost service to pass audit information to" };

};

inline void TrigCostAuditor::before(StandardEventType evt, INamedInterface* caller) {
  return this->before(evt, caller->name());
}

inline void TrigCostAuditor::after(StandardEventType evt, INamedInterface* caller, const StatusCode& sc) {
  return this->after(evt, caller->name(), sc);
}

#endif // TRIGCOSTMONITOR_TRIGCOSTAUDITOR_H
