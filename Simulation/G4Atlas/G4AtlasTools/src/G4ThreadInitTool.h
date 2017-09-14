/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_G4THREADINITTOOL_H
#define G4ATLASTOOLS_G4THREADINITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IThreadInitTool.h"

#include <string>
#include <atomic>

/// @class G4ThreadInitTool
/// @brief A tool which sets up the worker-thread-local workspace for Geant4.
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///
class G4ThreadInitTool : virtual public extends1<AthAlgTool, IThreadInitTool>
{

  public:

    /// Standard tool constructor
    G4ThreadInitTool(const std::string&, const std::string&, const IInterface*);

    /// Set up the Geant4 workspace for this worker thread
    virtual void initThread() override final;

    /// Tear down the Geant4 workspace for this worker thread
    virtual void terminateThread() override final;

    /// Counter used for barrier mechanism in thread initialization.
    /// This number needs to be correctly reported to properly ensure
    /// scheduling of thread-initialization tasks on every thread.
    virtual unsigned int nInit() const override final {
      return m_nInitThreads;
    }

  private:

    /// Counter of threads that have been initialized
    std::atomic_uint m_nInitThreads;

}; // class G4ThreadInitTool

#endif // G4ATLASTOOLS_G4THREADINITTOOL_H
