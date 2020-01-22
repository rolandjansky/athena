/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__MANAGER_H
#define EVENT_LOOP__MANAGER_H

#include <EventLoop/Global.h>

#include <string>
#include <utility>

class StatusCode;

namespace EL
{
  namespace Detail
  {
    /// \brief an interface for classes that handle job management for
    /// the \ref Driver
    ///
    /// The idea of this class is similar to \ref Module, but for work
    /// being done on the submission node, instead of the worker node.
    /// Even though it is essentially a pluggable infrastructure, but
    /// it is meant for expert use only, not as something that the
    /// user would directly instantiate or configure himself.
    ///
    /// The general way this works is that at the beginning of each
    /// task/operation a series of managers get loaded based on the
    /// task at hand.  Then we go through a series of steps (\ref
    /// ManagerStep) and at each step each manager gets the option to
    /// do what it needs to do at that step.  That way the different
    /// managers can operate more or less independently and have their
    /// code interleave while avoiding a fair amount of spaghetti
    /// code.  Any data that needs to be communicated between
    /// different managers or steps gets placed into the \ref
    /// ManagerData structure, meaning individual managers don't need
    /// to know about each other directly.
    ///
    /// So far (16 Aug 19) there are only very few managers defined,
    /// and most code still tries to address to many things in a
    /// single manager/driver.  However, this should (hopefully) serve
    /// as the infrastructure for making such changes a lot easier in
    /// the future.

    class Manager
    {
      /// \brief standard (virtual) destructor
      /// \par Guarantee
      ///   no-fail
    public:
      virtual ~Manager () noexcept = default;


      /// \brief get the order/name of this manager
      ///
      /// This is both used to identify the manager we are looking at,
      /// and to make sure they get executed in the right order.  It
      /// is a mistake to load two managers that report the same order.
      ///
      /// This is a pair of an enum that defines the absolute order of
      /// managers, and a string that identifies the stream that this
      /// manager belongs to.  If this manager is not specific to a
      /// stream this should be the empty stream.
      ///
      /// \par Guarantee
      ///   no-fail
    public:
      virtual std::pair<Detail::ManagerOrder,std::string>
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
