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
#include "EventLoopGrid/PandaRootTools.h"

namespace EL
{
  class GridWorker : public Worker
  {

  public:

    void testInvariant() const;

    GridWorker(const SH::MetaObject *sample, 
	       const std::string& location,
	       PandaRootTools& pandaTools);

    void run(JobConfig&& jobConfig, const TList& bigOutputs, const std::string& location);

    static void execute (const std::string& sampleName);


  private:

    const SH::MetaObject *m_meta;

    const std::string m_location;

    PandaRootTools m_pandaTools;

    ClassDef(EL::GridWorker, 1);
  };

}

#endif
