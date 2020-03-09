/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VTUNE_IPROFILERSERVICE_H
#define VTUNE_IPROFILERSERVICE_H

/// Interface definition
static const InterfaceID IID_IVTuneProfilerService( "IVTuneProfilerService", 0, 1 );

class IVTuneProfilerSvc : public virtual IService {

  public:
  
    /// Interface ID definition
    static const InterfaceID& interfaceID() { return IID_IVTuneProfilerService; }

    /// Resume profiling
    virtual StatusCode resumeProfiling() = 0;

    /// Pause profiling
    virtual StatusCode pauseProfiling() = 0;

    /// Is the profiling running at the moment?
    virtual bool isProfilingRunning() const = 0;

}; // class IVTuneProfilerSvc

#endif // VTUNE_IPROFILERSERVICE.H
