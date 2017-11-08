#ifndef SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_LIST_HANDLE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <string>
#include <vector>

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

    /// \brief the value of \ref systematicsVector
  private:
    std::vector<CP::SystematicSet> m_systematicsVector;

    /// \brief the variable for the property containing the
    /// systematics list
  private:
    std::vector<std::string> m_systematicsListProperty;
  };
}

#include "SysListHandle.icc"

#endif
