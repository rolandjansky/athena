#ifndef VTUNE_PROFILERSERVICE_H
#define VTUNE_PROFILERSERVICE_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaBaseComps/AthService.h"

// Local include(s):
#include "PerfMonVTune/IVTuneProfilerSvc.h"


class VTuneProfilerService : public AthService,
                             public virtual IVTuneProfilerSvc,
                             public virtual IIncidentListener {

  public:

      /// Standard Gaudi service constructor
      VTuneProfilerService( const std::string& name, ISvcLocator* svcloc );

      /// Destructor
      virtual ~VTuneProfilerService();

      /// Function declaring the interface(s) implemented by the service
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIF );

      /// Standard Gaudi initialization function
      virtual StatusCode initialize();

      /// Standard Gaudi finalization function
      virtual StatusCode finalize();

      /// Resume profiling
      virtual StatusCode resumeProfiling();

      /// Pause profiling
      virtual StatusCode pauseProfiling();

      /// Is the profiling running at the moment?
      virtual bool isProfilingRunning() const;

      /// Function handling incoming incidents
      virtual void handle( const Incident& inc );

  private:

      /// Handle to the incident service
      ServiceHandle< IIncidentSvc > m_incidentSvc;

      /// Property: Event in which profiling should start
      int m_resumeEvent;

      /// Property: Event in which profiling should pause 
      int m_pauseEvent;

      /// Is the profiling running at the moment?
      bool m_running;

      /// Number of events processed so far
      int m_processedEvents;

}; // class IVTuneProfilerSvc

#endif // VTUNE_PROFILERSERVICE.H
