/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_COPY_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_COPY_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgMessagingForward.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <string>
#include <tuple>
#include <unordered_map>

class StatusCode;

namespace CP
{
  class SystematicSet;

  /// \brief a data handle for copying systematics varied input data

  template<typename T> class SysCopyHandle final
    : public ISysHandleBase, public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysCopyHandle (T2 *owner, const std::string& propertyName,
                   const std::string& propertyValue,
                   const std::string& propertyDescription);


    /// \brief whether we have a name configured
  public:
    bool empty () const noexcept;

    /// \brief !empty()
  public:
    explicit operator bool () const noexcept;


    /// \brief retrieve the object for the given name
  public:
    ::StatusCode getCopy (T*& object,
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

    /// \brief the output name we use
  private:
    std::string m_outputName;

    /// \brief the cache of names we use
  private:
    mutable std::unordered_map<CP::SystematicSet,std::tuple<std::string,std::string,std::string> > m_nameCache;


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

#include "SysCopyHandle.icc"

#endif
