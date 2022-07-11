/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H
#define DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMatchingTool/IIParticleRetrievalTool.h"
#include "TriggerMatchingTool/IMatchScoringTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include <vector>

namespace DerivationFramework
{
  /**
   * @brief Tool to perform trigger matching in the derivation framework.
   *
   * Trigger matching in the derivation framework has to take the reverse
   * approach to matching in analysis code. There, the matching tool is provided
   * with a list of analysis particles to match to a given trigger. The
   * navigation then proceeds to search out all features that match the types of
   * those particles.
   * In the derivation framework however, we need to get the full list of all
   * particles that a user *could* ask for in their analysis code. To do this,
   * we ask the navigation for all the relevant xAOD objects and then use those
   * to query the available offline objects.
   *
   * @author Jon Burr
   */
  class TriggerMatchingTool final : public AthAlgTool,
                                    virtual public IAugmentationTool
  {
  public:
    /// Helper typedefs
    using particleVec_t = std::vector<const xAOD::IParticle *>;
    /// Constructor
    TriggerMatchingTool(
        const std::string &type,
        const std::string &name,
        const IInterface *pSvcLocator);

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// Calculate the matchings
    virtual StatusCode addBranches() const override;

  private:
    // Properties
    /// The list of chain names to match
    mutable std::vector<std::string> m_chainNames ATLAS_THREAD_SAFE;
    // This being mutable isn't exactly the greatest thing but we can't check
    // whether the chains actually exist before the first event. A better
    // solution would be to filter the chains by what is available in the
    // input file but I don't know if that's possible.
    // It is thread-safe because the mutable operation is done as part of a
    // static const initialization on the first call of addBranches().

    /// The tool to retrieve the online candidates
    ToolHandle<Trig::IIParticleRetrievalTool> m_trigParticleTool{"Trig::IParticleRetrievalTool/OnlineParticleTool"};

    /// The input containers to use. These are keyed by xAOD object type
    std::map<xAOD::Type::ObjectType, SG::ReadHandleKey<xAOD::IParticleContainer>> m_offlineInputs;

    /// The DR threshold to use
    float m_drThreshold;

    /// Whether to match in rerun mode or not.
    bool m_rerun;

    /// The prefix to place at the beginning of the output containers
    std::string m_outputPrefix;

    /// If set, discard any triggers with empty chain groups (break the job
    /// otherwise).
    bool m_checkEmptyChainGroups;

    /// If using an input-file-dependent config then we warn when triggers
    /// are removed
    bool m_inputDependentConfig;

    /// The trig decision tool
    ToolHandle<Trig::TrigDecisionTool> m_tdt{"Trig::TrigDecisionTool/TrigDecisionTool"};

    /// The pair scoring tool
    ToolHandle<Trig::IMatchScoringTool> m_scoreTool{
        this, "ScoringTool", "Trig::DRScoringTool", "The pair scoring tool"};

    // Internal functions
    /**
       * @brief Create an output container for the named chain
       * @param[out] container A pointer to the created container
       * @param chain The name of the chain to create the container for
       * The container will be recorded in the StoreGate using the name of the
       * chain as a key.
       */
    StatusCode createOutputContainer(
        xAOD::TrigCompositeContainer *&container,
        const std::string &chain) const;

    /**
       * @brief Get all offline particles that could match a given online one.
       * @param part The online particle to match against.
       * @param offlineParticles The offline particles, key by xAOD type.
       * @param cache Store past matches here to speed up future look ups.
       */
    const particleVec_t &getCandidateMatchesFor(
        const xAOD::IParticle *part,
        std::map<xAOD::Type::ObjectType, particleVec_t> &offlineParticles,
        std::map<const xAOD::IParticle *, particleVec_t> &cache) const;

    /**
       * @brief Check if the dR between two particles is below threshold.
       */
    bool matchParticles(
        const xAOD::IParticle *lhs,
        const xAOD::IParticle *rhs) const;
  }; //> end class TriggerMatchingTool
} // namespace DerivationFramework

#endif //> ! DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H
