/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTAUDITOR_H
#define PERFMONCOMPS_PERFMONMTAUDITOR_H

// STL includes
#include <string>

// Framework includes
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
//class INamedInterface;
class IPerfMonMTSvc;

class PerfMonMTAuditor : public Auditor
{
  /*
  using Auditor::before;
  using Auditor::after;
  */

  public:

    /// Constructor
    PerfMonMTAuditor(const std::string& name, ISvcLocator* pSvcLocator);

    /// Gaudi hooks
    virtual StatusCode initialize() override;

    /// Implement inherited methods from Auditor
    void before( StandardEventType, INamedInterface* ) override;
    void before( StandardEventType, const std::string& ) override;

    void before( CustomEventTypeRef, INamedInterface* ) override;
    void before( CustomEventTypeRef, const std::string& ) override;

    void after( StandardEventType, INamedInterface*, const StatusCode& ) override;
    void after( StandardEventType, const std::string&, const StatusCode& ) override;

    void after( CustomEventTypeRef, INamedInterface*, const StatusCode& ) override;
    void after( CustomEventTypeRef, const std::string&, const StatusCode& ) override;

  private:

    /// Handle to PerfMonMTSvc
    ServiceHandle< IPerfMonMTSvc > m_perfMonMTSvc;

}; // end PerfMonMTAuditor

#endif // PERFMONCOMPS_PERFMONMTAUDITOR_H
