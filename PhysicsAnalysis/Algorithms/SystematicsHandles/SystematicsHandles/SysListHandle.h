/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H

#include <AsgMessaging/AsgMessagingForward.h>
#include <AsgServices/ServiceHandle.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISystematicsSvc.h>
#include <SystematicsHandles/SysListType.h>
#include <functional>
#include <string>
#include <vector>

class StatusCode;

namespace CP
{
  class IReentrantSystematicsTool;
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
    SysListHandle (T *owner, const std::string& propertyName = "systematicsService",
                   const std::string& propertyDescription = "systematics to evaluate");


    /// \brief register an input handle we are using
    ///
    /// This is currently a no-op, but it could be useful for a
    /// variety of future directions we could take the systematics
    /// handling.
    ///
    /// \pre !isInitialized()
  public:
    StatusCode addHandle (ISysHandleBase& handle);


    /// \brief register a set of affecting variables for the current
    /// algorithm (usually obtained from an \ref CP::ISystematicsTool)
    ///
    /// This is currently a no-op, but it could be useful for a
    /// variety of future directions we could take the systematics
    /// handling (or at least as a cross check of those).
    ///
    /// \pre !isInitialized()
    /// \{
  public:
    StatusCode addSystematics (const CP::SystematicSet& recommended,
                               const CP::SystematicSet& affecting);
    StatusCode addSystematics (const IReentrantSystematicsTool& tool);
    /// \}


    /// \brief intialize this property
    ///
    /// This should be called exactly once during initialize of the
    /// owning algorithm.
  public:
    ::StatusCode initialize ();


    /// \brief whether \ref initialize has been called successfully
  public:
    bool isInitialized () const noexcept;


    /// \brief the service we use
  public:
    const ISystematicsSvc& service () const;


    /// \brief the list of systematics to loop over
    ///
    /// The way used should use this is as:
    /// ```
    ///   for (const auto& sys : m_systematicsList.systematicsVector())
    ///   {
    ///     ...
    ///   }
    /// ```
    ///
    /// The important part is to use `const auto&` instead of `const
    /// CP::SystematicSet&` here, as in the future this may be updated
    /// to be a vector of something other than a `CP::SystematicSet`
    /// (still convertible to `const CP::SystematicSet&` though).
  public:
    const std::vector<CP::SystematicSet>& systematicsVector () const;


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
    ///
    /// \warn This is deprecated in favor of just calling \ref
    /// systematicsVector directly (mostly to make code easier to
    /// understand for new users).
  public:
    [[deprecated("please use systematicsVector() instead")]]
    StatusCode foreach
      (const std::function<StatusCode(const CP::SystematicSet&)>& func) const;



    //
    // private interface
    //

    /// \brief the handle for the systematics service
  private:
    ServiceHandle<ISystematicsSvc> m_systematicsService {"SystematicsSvc", ""};

    /// \brief the list of systematics handles we have
  private:
    std::vector<ISysHandleBase*> m_sysHandles;

    /// \brief this set of affecting systematics
  private:
    CP::SystematicSet m_affecting;

    /// \brief the value of \ref systematicsVector
  private:
    std::vector<CP::SystematicSet> m_systematicsVector;

    /// \brief the value of \ref isInitialized
  private:
    bool m_isInitialized = false;

  private:
    StatusCode fillSystematicsVector ();
  };
}

#include "SysListHandle.icc"

#endif
