/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__SUBMIT_DIR_MANAGER_H
#define EVENT_LOOP__SUBMIT_DIR_MANAGER_H

#include <EventLoop/Global.h>

#include <EventLoop/Manager.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Manager to handle the submission directory itself

    class SubmitDirManager final : public Manager
    {
    public:
      virtual std::pair<Detail::ManagerOrder,std::string>
      getManagerOrder () const noexcept override;

    public:
      virtual ::StatusCode
      doManagerStep (Detail::ManagerData& data) const override;
    };
  }
}

#endif
