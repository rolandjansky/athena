#ifndef VTUNE_PROFILERSERVICE_H
#define VTUNE_PROFILERSERVICE_H

// STD include(s):
#include <atomic>
#include <memory>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaBaseComps/AthService.h"

// Local include(s):
#include "VTuneProfileRunner.h"
#include "PerfMonVTune/IVTuneProfilerSvc.h"


class VTuneProfilerService : public AthService,
                             public virtual IVTuneProfilerSvc,
                             public virtual IIncidentListener {

  public:

      /// Standard Gaudi service constructor
      VTuneProfilerService( const std::string& name, ISvcLocator* svcloc );

      /// Function declaring the interface(s) implemented by the service
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIF ) override;

      /// Standard Gaudi initialization function
      virtual StatusCode initialize() override;

      /// Resume profiling
      virtual StatusCode resumeProfiling() override;

      /// Pause profiling
      virtual StatusCode pauseProfiling() override;

      /// Is the profiling running at the moment?
      virtual bool isProfilingRunning() const override;

      /// Function handling incoming incidents
      virtual void handle( const Incident& inc ) override;

  private:

      /// Handle to the incident service
      ServiceHandle< IIncidentSvc > m_incidentSvc;

      /// Property: Event in which profiling should start
      int m_resumeEvent;

      /// Property: Event in which profiling should pause 
      int m_pauseEvent;

      /// Unique ptr to the VTuneProfileRunner
      std::unique_ptr< VTuneProfileRunner > m_runner;

      /// Number of events processed so far
      std::atomic<int> m_processedEvents;

}; // class IVTuneProfilerSvc

#endif // VTUNE_PROFILERSERVICE.H
