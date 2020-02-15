/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/ModuleData.h>

#include <EventLoop/OutputStreamData.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    void ModuleData ::
    addOutput (std::unique_ptr<TObject> output)
    {
      RCU_ASSERT (m_histOutput != nullptr);
      m_histOutput->addOutput (std::move (output));
    }
  }
}
