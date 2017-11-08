#ifndef SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/MsgStream.h>
#include <SystematicsHandles/SysListType.h>
#include <string>
#include <vector>

#ifdef ROOTCORE
#include <AsgTools/SgTEvent.h>
#else
#include <StoreGate/StoreGateSvc.h>
#endif

class StatusCode;

namespace CP
{
  class SystematicSet;
}

namespace EL
{
  /// \brief a class managing the property to configure the list of
  /// systematics to process

  class SysListHandle
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T>
    SysListHandle (T *owner, const std::string& propertyName = "systematics",
                       const std::string& propertyDescription = "list of systematics to evaluate");


    /// \brief register an input handle we are using
    ///
    /// This is currently a no-op, but it could be useful for a
    /// variety of future directions we could take the systematics
    /// handling.
    ///
    /// \pre !isInitialized()
  public:
    template<typename T> void
    addInputHandle (const T& inputHandle);


    /// \brief register a set of affecting variables for the current
    /// algorithm (usually obtained from an \ref CP::ISystematicsTool)
    ///
    /// This is currently a no-op, but it could be useful for a
    /// variety of future directions we could take the systematics
    /// handling (or at least as a cross check of those).
    ///
    /// \pre !isInitialized()
  public:
    void addAffectingSystematics
      (const CP::SystematicSet& affectingSystematics);


    /// \brief intialize this property
    ///
    /// This should be called exactly once during initialize of the
    /// owning algorithm.
  public:
    ::StatusCode initialize ();


    /// \brief whether \ref initialize has been called successfully
  public:
    bool isInitialized () const noexcept;


    /// \brief the list of systematics to loop over
  public:
    const std::vector<CP::SystematicSet>& systematicsVector () const noexcept;



    //
    // private interface
    //

    /// \brief the name under which the systematics list is stored in
    /// the event store
  private:
    std::string m_systematicsListName;

    /// \brief the value of \ref isInitialized
  private:
    bool m_isInitialized = false;

#ifdef ROOTCORE
    /// \brief the event store we use
  private:
    mutable asg::SgTEvent *m_evtStore = nullptr;

    /// \brief the function to retrieve the event store
  private:
    std::function<asg::SgTEvent*()> m_evtStoreGetter;
#else
    /// \brief the event store we use
  private:
    mutable StoreGateSvc *m_evtStore = nullptr;

    /// \brief the function to retrieve the event store
  private:
    std::function<StoreGateSvc*()> m_evtStoreGetter;
#endif

    /// \brief the message object we are using
    ///
    /// Note that this violates the ATLAS naming convention to be
    /// compatible with the ATLAS messaging macros.
  private:
    std::function<MsgStream& (const MSG::Level lvl)> msg;
  };
}

#include "SysListHandle.icc"

#endif
