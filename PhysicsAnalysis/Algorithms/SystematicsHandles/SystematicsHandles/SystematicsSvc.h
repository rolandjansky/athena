/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYSTEMATICS_SVC_H
#define SYSTEMATICS_HANDLES__SYSTEMATICS_SVC_H

#include <AsgServices/AsgService.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISystematicsSvc.h>
#include <atomic>
#include <mutex>

namespace CP
{
  /// \brief the canonical implementation of \ref ISystematicsSvc

  class SystematicsSvc final : public asg::AsgService,
                                 virtual public ISystematicsSvc
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
    SystematicsSvc (const std::string& name,
                      ISvcLocator* pSvcLocator);



    //
    // inherited interface
    //

  public:
    virtual ::StatusCode initialize () override;
    virtual ::StatusCode finalize () override;
    virtual const SysListType& systematicsVector () const override;



    //
    // private interface
    //

    /// \brief the names of the systematics to request
  private:
    std::vector<std::string> m_systematicsList;

    /// \brief the regular expression for filterinf systematics
  private:
    std::string m_systematicsRegex {"(.*)"};

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
    mutable std::vector<CP::SystematicSet> m_systematicsVector;

    /// \brief whether \ref m_systematicsVector was initialized
  private:
    mutable std::atomic<bool> m_hasVector {false};

    /// \brief a mutex for filling \ref m_systematicsVector
  private:
    mutable std::mutex m_vectorMutex;
  };
}

#endif
