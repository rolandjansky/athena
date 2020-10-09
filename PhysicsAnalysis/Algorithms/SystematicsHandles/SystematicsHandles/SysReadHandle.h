/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_READ_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_READ_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgMessaging/AsgMessagingForward.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <string>
#include <type_traits>
#include <unordered_map>

class StatusCode;

namespace CP
{
  class SystematicSet;

  /// \brief a data handle for reading systematics varied input data

  template<typename T> class SysReadHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
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


    /// \brief whether we have a name configured
  public:
    bool empty () const noexcept;

    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;


    /// \brief get the name we retrieve from the event store
  public:
    const std::string& getName (const CP::SystematicSet& sys) const;


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


    /// \brief the type of the event store we use
  private:
    typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type StoreType;

    /// \brief the event store we use
  private:
    mutable StoreType *m_evtStore = nullptr;

    /// \brief the function to retrieve the event store
    ///
    /// This is an std::function to allow the parent to be either a
    /// tool or an algorithm.  Though we are not really supporting
    /// tools as parents when using \ref SysListHandle, so in
    /// principle this could be replaced with a pointer to the
    /// algorithm instead.
  private:
    std::function<StoreType*()> m_evtStoreGetter;
  };
}

#include "SysReadHandle.icc"

#endif
