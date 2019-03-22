/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H
#define DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMatchingTool/IIParticleRetrievalTool.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include <vector>

namespace DerivationFramework {
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
  class TriggerMatchingTool final :
    public AthAlgTool,
    virtual public IAugmentationTool
  {
    public:
      /// Helper typedefs
      using particleVec_t = std::vector<const xAOD::IParticle*>;
      /// Constructor
      TriggerMatchingTool(
          const std::string& type,
          const std::string& name,
          const IInterface* pSvcLocator);

      /// Initialize the tool
      StatusCode initialize() override;

      /// Calculate the matchings
      StatusCode addBranches() const override;

    private:
      // Properties
      /// The list of chain names to match
      std::vector<std::string> m_chainNames;

      /// The tool to retrieve the online candidates
      ToolHandle<Trig::IIParticleRetrievalTool> m_trigParticleTool
      {"Trig::IParticleRetrievalTool/OnlineParticleTool"};

      /// The input containers to use. These are keyed by xAOD object type
      std::map<xAOD::Type::ObjectType, std::string> m_offlineInputs;

      /// The DR threshold to use
      float m_drThreshold;

      /// Whether to match in rerun mode or not.
      bool m_rerun;

      // Internal functions
      /**
       * @brief Create an output container for the named chain
       * @param[out] container A pointer to the created container
       * @param chain The name of the chain to create the container for
       * The container will be recorded in the StoreGate using the name of the
       * chain as a key.
       */
      StatusCode createOuputContainer(
          xAOD::TrigCompositeContainer*& container,
          const std::string& chain) const;

      /**
       * @brief Get all offline particles that could match a given online one.
       * @param part The online particle to match against.
       * @param offlineParticles The offline particles, key by xAOD type.
       * @param cache Store past matches here to speed up future look ups.
       */
      const particleVec_t& getCandidateMatchesFor(
          const xAOD::IParticle* part,
          std::map<xAOD::Type::ObjectType, particleVec_t>& offlineParticles,
          std::map<const xAOD::IParticle*, particleVec_t>& cache) const;

      /**
       * @brief Check if the dR between two particles is below threshold.
       */
      bool matchParticles(
          const xAOD::IParticle* lhs,
          const xAOD::IParticle* rhs) const;
  }; //> end class TriggerMatchingTool
} //> end namespace DerivationFramework

#endif //> ! DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H
