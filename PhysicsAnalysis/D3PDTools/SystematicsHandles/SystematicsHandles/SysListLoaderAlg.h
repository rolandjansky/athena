#ifndef SYSTEMATICS_HANDLES__SYS_LIST_LOADER_ALG_H
#define SYSTEMATICS_HANDLES__SYS_LIST_LOADER_ALG_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AnaAlgorithm/AnaAlgorithm.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/SysListType.h>

namespace EL
{
  /// \todo add documentation

  class SysListLoaderAlg final : public AnaAlgorithm
  {
    //
    // public interface
    //

    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    SysListLoaderAlg (const std::string& name, 
                      ISvcLocator* pSvcLocator);



    //
    // inherited interface
    //

  public:
    virtual ::StatusCode initialize () override;

  public:
    virtual ::StatusCode execute () override;



    //
    // private interface
    //

    /// \brief the name under which to store the systematics in the
    /// event store
  private:
    std::string m_systematicsName {sysListDefaultName()};

    /// \brief the names of the systematics to request
  private:
    std::vector<std::string> m_systematicsList;

    /// \brief load all recommended systematics at the given number of
    /// sigmas
    ///
    /// The idea here is that this allows to run a simple analysis by
    /// itself without having to generate the list of systematics
    /// manually.
  private:
    float m_sigmaRecommended = 0;

    /// \brief the list of actual systematics
  private:
    std::vector<CP::SystematicSet> m_systematicsVector;

    /// \brief whether the next event will be the first event
  private:
    bool m_firstEvent = true;
  };
}

#endif
