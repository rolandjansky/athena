/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgMessagingForward.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/SysListType.h>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>

class StatusCode;

namespace CP
{
  class ISysHandleBase;
  class SystematicSet;

  /// \brief a class managing the property to configure the list of
  /// systematics to process

  class SysListHandle : public asg::AsgMessagingForward
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
    void addHandle (ISysHandleBase& handle);


    /// \brief register a set of affecting variables for the current
    /// algorithm (usually obtained from an \ref CP::ISystematicsTool)
    ///
    /// This is currently a no-op, but it could be useful for a
    /// variety of future directions we could take the systematics
    /// handling (or at least as a cross check of those).
    ///
    /// \pre !isInitialized()
  public:
    StatusCode addAffectingSystematics
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
    std::unordered_set<CP::SystematicSet> systematicsVector ();


    /// \brief run the function for each systematic
    ///
    /// This allows to perform some amount of behind-the-scenes
    /// optimizations in the future, which hopefully not creating too
    /// many issues in the present.
    ///
    /// Technically this would be slightly more performant as a
    /// template, but this is likely not to be an issue, and can still
    /// be changed if it ever becomes an issue.
    ///
    /// Ideally this would be const, but the current version is not
    /// thread-safe, so I'd rather not add a const qualifier to it.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   function failures
    /// \pre isInitialized()
  public:
    StatusCode foreach
      (const std::function<StatusCode(const CP::SystematicSet&)>& func);



    //
    // private interface
    //

    /// \brief the name under which the systematics list is stored in
    /// the event store
  private:
    std::string m_systematicsListName {"systematics"};

    /// \brief the regular expression for affecting systematics
  private:
    std::string m_affectingRegex {"^$"};

    /// \brief the full affecting systematics including the inputs
  private:
    std::string m_fullAffecting;

    /// \brief the cache of affecting filtered systematics
  private:
    std::unordered_map<CP::SystematicSet,CP::SystematicSet> m_affectingCache;

    /// \brief the list of systematics handles we have
  private:
    std::vector<ISysHandleBase*> m_sysHandles;

    /// \brief the value of \ref isInitialized
  private:
    bool m_isInitialized = false;


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

#include "SysListHandle.icc"

#endif
