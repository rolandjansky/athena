/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_GRID_WORKER_H
#define EVENT_LOOP_GRID_WORKER_H

#include <string>

#include <TList.h>

#include "SampleHandler/MetaObject.h"
#include "EventLoop/Worker.h"
#include "EventLoop/Global.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>

namespace EL
{
  class GridWorker : public Worker
  {

  public:

    void testInvariant() const;

    GridWorker();

    void run(JobConfig&& jobConfig, const TList& bigOutputs, const std::string& location);

    static void execute (const std::string& sampleName);


  private:

    void NotifyJobFinished(uint64_t eventsProcessed,
                           const std::vector<std::string>& fileList);

    void Fail(uint64_t eventsProcessed, std::size_t currentFile,
              const std::string& fileName);
    void Abort();

  private:    

    enum ErrorCodes {
      EC_FAIL = 220,
      EC_ABORT = 221,
      EC_NOTFINISHED = 222,
      EC_BADINPUT = 223
    };

    void createJobSummary(uint64_t eventsProcessed);

    ClassDef(EL::GridWorker, 1);
  };

}

#endif
