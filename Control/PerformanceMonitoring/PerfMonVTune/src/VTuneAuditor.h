/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VTUNEAUDITOR_H
#define VTUNEAUDITOR_H

// STL include(s)
#include <vector>
#include <string>

// Framework include(s)
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PerfMonVTune/IVTuneProfilerSvc.h"

class VTuneAuditor : public Auditor
{

  public:

    /// Constructor
    VTuneAuditor(const std::string& name, ISvcLocator* pSvcLocator);

    /// Gaudi hooks
    virtual StatusCode initialize() override;

    /// Implement inherited methods from Auditor
    void before( StandardEventType, INamedInterface* ) override;
    void before( StandardEventType, const std::string& ) override { return; }

    void before( CustomEventTypeRef, INamedInterface* ) override { return; }
    void before( CustomEventTypeRef, const std::string& ) override { return; }

    void after( StandardEventType, INamedInterface*, const StatusCode& ) override; 
    void after( StandardEventType, const std::string&, const StatusCode& ) override { return; }

    void after( CustomEventTypeRef, INamedInterface*, const StatusCode& ) override { return; }
    void after( CustomEventTypeRef, const std::string&, const StatusCode& ) override { return; }

  private:

    /// Handle to VTuneProfilerSvc
    ServiceHandle< IVTuneProfilerSvc > m_vtuneProfilerSvc;

    /// Property: List of algorithms to profile
    std::vector<std::string> m_algs;

}; // end VTuneAuditor


#endif // VTUNEAUDITOR_H
