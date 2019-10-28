/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ASG_ANALYSIS_ALGORITHMS__SYS_LIST_LOADER_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__SYS_LIST_LOADER_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/SysListType.h>

namespace CP
{
  /// \todo add documentation

  class SysListLoaderAlg final : public EL::AnaAlgorithm
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
    std::vector<CP::SystematicSet> m_systematicsVector;

    /// \brief whether the next event will be the first event
  private:
    bool m_firstEvent = true;
  };
}

#endif
