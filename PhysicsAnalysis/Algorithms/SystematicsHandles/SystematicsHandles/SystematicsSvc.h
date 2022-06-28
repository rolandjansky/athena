/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYSTEMATICS_SVC_H
#define SYSTEMATICS_HANDLES__SYSTEMATICS_SVC_H

#include <AsgServices/AsgService.h>
#include <CxxUtils/checker_macros.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISystematicsSvc.h>
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
    virtual StatusCode initialize () override;
    virtual StatusCode finalize () override;
    virtual std::vector<CP::SystematicSet>
    makeSystematicsVector () const override;
    virtual StatusCode addSystematics (const CP::SystematicSet& recommended,
                                       const CP::SystematicSet& affecting) const override;
    virtual CP::SystematicSet
    getObjectSystematics (const std::string& name) const override;
    virtual StatusCode
    setObjectSystematics (const std::string& name,
                          const CP::SystematicSet& systematics) const override;
    virtual CP::SystematicSet
    getDecorSystematics (const std::string& objectName,
                         const std::string& decorName) const override;
    virtual StatusCode
    setDecorSystematics (const std::string& objectName,
                         const std::string& decorName,
                         const CP::SystematicSet& systematics) const override;
    virtual StatusCode
    registerCopy (const std::string& fromName,
                  const std::string& toName) const override;
    virtual StatusCode
    makeSystematicsName (std::string& result,
                         const std::string& name,
                         const CP::SystematicSet& sys) const override;



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

    /// \brief nominal systematics name
  private:
    std::string m_nominalSystematicsName {"NOSYS"};


    /// \brief the list of affecting systematics
  private:
    mutable SystematicSet m_affectingSystematics ATLAS_THREAD_SAFE;

    /// \brief the list of recommended systematics
  private:
    mutable SystematicSet m_recommendedSystematics ATLAS_THREAD_SAFE;

    /// \brief the list of per-object systematics
  private:
    mutable std::unordered_map<std::string,CP::SystematicSet> m_objectSystematics ATLAS_THREAD_SAFE;

    /// \brief the list of per-object-and-decoration systematics
  private:
    mutable std::unordered_map<std::string,CP::SystematicSet> m_decorSystematics ATLAS_THREAD_SAFE;

    /// \brief the map of registered copies
  private:
    mutable std::unordered_map<std::string,std::string> m_copies ATLAS_THREAD_SAFE;

    /// \brief a mutex for accessing the above mutable members
  private:
    mutable std::mutex m_systematicsMutex;
  };
}

#endif
