/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/EventCountModule.h>

#include <EventLoop/ModuleData.h>
#include <TH1.h>
#include <TList.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ::StatusCode EventCountModule ::
    postFinalize (ModuleData& data)
    {
      auto eventCount = std::make_unique<TH1D>
        ("EventLoop_EventCount", "number of events per algorithm",
         data.m_algs.size()+1, 0, data.m_algs.size()+1);
      eventCount->SetDirectory (nullptr);

      for (std::size_t iter = 0; iter != data.m_algs.size(); ++ iter)
        eventCount->Fill (iter, data.m_algs[iter].m_executeCount);

      // count the number of events that passed all algorithms without
      // skipping
      if (data.m_algs.size() > 0)
      {
        eventCount->Fill (data.m_algs.size(), data.m_algs.back().m_executeCount - data.m_algs.back().m_skipCount);
      } else
      {
        eventCount->Fill (data.m_algs.size(), data.m_eventsProcessed);
      }

      data.addOutput (std::move (eventCount));
      return ::StatusCode::SUCCESS;
    }
  }
}
