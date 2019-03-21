/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/TreeCacheModule.h>

#include <EventLoop/Job.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/ModuleData.h>
#include <TTree.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ::StatusCode TreeCacheModule ::
    onNewInputFile (ModuleData& data)
    {
      if (data.m_inputTree)
      {
        double cacheSize = data.m_metaData->castDouble (Job::optCacheSize, 0);
        if (cacheSize > 0)
          data.m_inputTree->SetCacheSize (cacheSize);
        double cacheLearnEntries = data.m_metaData->castDouble (Job::optCacheLearnEntries, 0);
        if (cacheLearnEntries > 0)
          data.m_inputTree->SetCacheLearnEntries (cacheLearnEntries);
      }
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode TreeCacheModule ::
    onCloseInputFile (ModuleData& data)
    {
      using namespace msgEventLoop;
      if (data.m_metaData->castBool (Job::optPrintPerFileStats, false))
      {
        ANA_MSG_INFO ("file stats for: " << data.m_inputFileUrl);
        data.m_inputTree->PrintCacheStats ();
      }
      return ::StatusCode::SUCCESS;
    }
  }
}
