#ifndef SYSTEMATICS_HANDLES__SYS_READ_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_READ_HANDLE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/MsgStream.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <string>
#include <unordered_map>

#ifdef ROOTCORE
#include <AsgTools/SgTEvent.h>
#else
#include <StoreGate/StoreGateSvc.h>
#endif

class StatusCode;

namespace CP
{
  class SystematicSet;

  /// \brief a data handle for reading systematics varied input data

  template<typename T> class SysReadHandle final : public ISysHandleBase
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysReadHandle (T2 *owner, const std::string& propertyName,
                   const std::string& propertyValue,
                   const std::string& propertyDescription);


    /// \brief retrieve the object for the given name
  public:
    ::StatusCode retrieve (const T*& object,
                           const CP::SystematicSet& sys) const;



    //
    // inherited interface
    //

  public:
    virtual std::string getInputAffecting () const override;



    //
    // private interface
    //

    /// \brief the input name we use
  private:
    std::string m_inputName;

    /// \brief the regular expression for affecting systematics
  private:
    std::string m_affectingRegex {".*"};

    /// \brief the cache of names we use
  private:
    mutable std::unordered_map<CP::SystematicSet,std::string> m_inputNameCache;

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


    /// \brief the message stream we use
  private:
    MsgStream *m_msg {nullptr};

    /// \brief helper for message macros
  private:
    MsgStream& msg( ) const;

    /// \brief helper for message macros
  private:
    MsgStream& msg( const MSG::Level lvl ) const;
  };
}

#include "SysReadHandle.icc"

#endif
