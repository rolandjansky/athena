/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__MANAGER_H
#define EVENT_LOOP__MANAGER_H

#include <EventLoop/Global.h>

class StatusCode;

namespace EL
{
  namespace Detail
  {
    class Manager
    {
      /// \brief standard (virtual) destructor
      /// \par Guarantee
      ///   no-fail
    public:
      virtual ~Manager () noexcept = default;


      /// \brief get the order of this manager
      /// \par Guarantee
      ///   no-fail
    public:
      virtual Detail::ManagerOrder
      getManagerOrder () const noexcept = 0;


      /// \brief do whatever needs to be done for the given submission step
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   job configuration errors\n
      ///   driver errors
    public:
      virtual ::StatusCode
      doManagerStep (Detail::ManagerData& data) const = 0;
    };
  }
}

#endif
