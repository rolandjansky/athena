/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/ManagerData.h>

#include <EventLoop/BatchJob.h>
#include <EventLoop/Manager.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>

using namespace EL::msgEventLoop;

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ManagerData ::
    ManagerData () noexcept
    {}



    ManagerData ::
    ~ManagerData () noexcept
    {}



    void ManagerData ::
    addManager (std::unique_ptr<Manager> manager)
    {
      if (managers.find (manager->getManagerOrder()) != managers.end())
        throw std::logic_error ("duplicate manager order " + std::to_string (unsigned (manager->getManagerOrder().first)) + " " + manager->getManagerOrder().second);
      managers.insert (std::make_pair (manager->getManagerOrder(), std::move (manager)));
    }



    ::StatusCode ManagerData ::
    run ()
    {
      if (step != Detail::ManagerStep::initial)
        throw std::logic_error ("ManagerData::run() called twice");

      do
      {
        step = nextStep;
        nextStep = Detail::ManagerStep (unsigned (step) + 1);
        ANA_MSG_DEBUG ("running step " << unsigned (step) << " / " << unsigned (Detail::ManagerStep::final));
        for (const auto& manager : managers)
        {
          if (manager.second->doManagerStep (*this).isFailure())
          {
            ANA_MSG_ERROR ("while performing manager step " << unsigned (step));
            ANA_MSG_ERROR ("on submission directory " << submitDir);
            return ::StatusCode::FAILURE;
          }
        }
      } while (step != Detail::ManagerStep::final);
      return ::StatusCode::SUCCESS;
    }
  }
}
