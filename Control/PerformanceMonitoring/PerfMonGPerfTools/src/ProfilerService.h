// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ProfilerService.h 496380 2012-04-18 12:28:09Z ritsch $
#ifndef PERFMONGPERFTOOLS_PROFILERSERVICE_H
#define PERFMONGPERFTOOLS_PROFILERSERVICE_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaBaseComps/AthService.h"

// Local include(s):
#include "PerfMonGPerfTools/IProfilerSvc.h"

namespace GPT {

   /**
    *  @short Implementation of the IProfilerSvc interface
    *
    *         This service should be the only component talking directly with
    *         the GPT functions. Every other component should interact with
    *         GPT through this service.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 496380 $
    * $Date: 2012-04-18 14:28:09 +0200 (Wed, 18 Apr 2012) $
    */
   class ProfilerService : public AthService,
                           public virtual IProfilerSvc,
                           public virtual IIncidentListener {

   public:
      /// Standard Gaudi service constructor
      ProfilerService( const std::string& name, ISvcLocator* svcloc );
      /// Destructor
      virtual ~ProfilerService();

      /// Function declaring the interface(s) implemented by the service
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIF );

      /// Standard Gaudi initialization function
      virtual StatusCode initialize();
      /// Standard Gaudi finalization function
      virtual StatusCode finalize();

      /// Start GPT profiling
      virtual StatusCode startCPUProfiling( const std::string& filename );
      /// Stop the GPT profiling
      virtual StatusCode stopCPUProfiling();
      /// Is the GPT profiling running at the moment?
      virtual bool isCPUProfilingRunning() const;

      /// Function handling incoming incidents
      virtual void handle( const Incident& inc );

   private:
      /// Handle to the incident service
      ServiceHandle< IIncidentSvc > m_incidentSvc;

      /// Property: Is profiling controlled from the outside?
      bool m_controlledProfiling;
      /// Property: Event in which non-controlled profiling should start
      int m_initEvent;
      /// Property: Profile file name in non-controlled profiling
      std::string m_profFileName;

      /// Is the CPU profiling running at the moment?
      bool m_running;
      /// Name of the current profile file
      std::string m_fileName;

      /// Number of events processed so far
      int m_processedEvents;

   }; // class IProfilerSvc

} // namespace GPT

#endif // PERFMONGPERFTOOLS_PROFILERSERVICE_H
