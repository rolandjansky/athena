/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/Module.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ::StatusCode Module ::
    preInitialize (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    onNewInputFile (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    onCloseInputFile (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    postInitialize (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    void Module ::
    reportInputFailure (ModuleData& /*data*/)
    {}

    ::StatusCode Module ::
    onFileExecute (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    onInitialize (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    onFinalize (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    postFinalize (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }

    ::StatusCode Module ::
    onWorkerEnd (ModuleData& /*data*/)
    {
      return ::StatusCode::SUCCESS;
    }
  }
}
