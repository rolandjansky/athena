/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_THREADINITTOOL_H
#define ATHEXHIVE_THREADINITTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IThreadInitTool.h"

#include <string>
#include <atomic>

//-------------------------------------------------------------------------------
//
// This is an example of a Tool that will get executed in every thread by the
// ThreadPoolSvc.
//
// At the beginning of the job, just after the ThreadPool has been created,
// the initThread() method of the Tool will be called in each thread.
//
// as a check, m_nInitThreads MUST be incremented at the end of the initThread()
// method.
//
// At the end of the job, the terminateThread() method will be called.
//
//-------------------------------------------------------------------------------


class ThreadInitTool: virtual public extends1<AthAlgTool, IThreadInitTool> {

public:
  ThreadInitTool( const std::string&, const std::string&, const IInterface* );


  virtual void initThread() override;
  virtual void terminateThread() override;

  virtual unsigned int nInit() const override { return m_nInitThreads; }

private:
  // Number of threads that have been initialized
  std::atomic_uint m_nInitThreads;

};

#endif
