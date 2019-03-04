/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H
#define DERIVATIONFRAMEWORKTRIGGER_TRIGGERMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigNavStructure/TriggerElement.h"
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
    virtual public IAugmentationTool,
    virtual public IIncidentListener
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

      /// Handle an incident
      void handle(const Incident& inc) override;

    private:
      // Properties
      /// The list of chain names to match
      std::vector<std::string> m_chainNames;

      /// The trigger decision tool to use
      ToolHandle<Trig::TrigDecisionTool> m_tdt;

      /// The input containers to use. These are keyed by xAOD object type
      std::map<xAOD::Type::ObjectType, std::string> m_offlineInputs;

      /// The DR threshold to use
      float m_drThreshold;

      /// Whether to match in rerun mode or not.
      bool m_rerun;

      // Internal values
      /// The pointer to the navigation. This is cached from the TDT so is
      /// *logical* const.
      mutable const HLT::TrigNavStructure* m_navigation;

      /// The cache of TE ID -> type lookup
      mutable std::map<HLT::te_id_type, xAOD::Type::ObjectType> m_egammaTEs;

      // Internal functions
      /// Call on every new file
      void beginNewFile();

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
       * @brief Retrieve all combinations of online objects that could have
       * fired the trigger.
       * @param[out] combinations The found combinations.
       * @param cg The chain group to query.
       */
      StatusCode getOnlineCombinations(
          std::vector<particleVec_t>& combinations,
          const Trig::ChainGroup& cg) const;

      /**
       * @brief Retrieve the combination of online objects from the given
       * Trig::Combination object.
       * @param[out] output The found combination will be appended onto this
       * vector
       * @param combination The Trig::Combination to query.
       */
      StatusCode getCombination(
          std::vector<particleVec_t>& output,
          const Trig::Combination& combination) const;

      /**
       * @brief Retrieve all particles that caused this TE to fire.
       * @param[out] particles The found particles will be appended to this
       * vector.
       * @param te The te to query.
       */
      StatusCode processTE(
          particleVec_t& particles,
          const HLT::TriggerElement* te) const;

      /**
       * @brief Retrieve particles from an EGamma TE
       * @param[out] particles The found particle will be appended to this
       * vector.
       * @param te The te to query.
       * By the time we've reached an 'egamma' node we should be on a single leg
       * and should therefore only be finding one particle.
       */
      StatusCode processEGammaTE(
          particleVec_t& particles,
          const HLT::TriggerElement* te) const;

      /**
       * @brief Retrieve a single particle from a feature.
       * @param[out] particle The particle will be filled here.
       * @param feature The feature to read the particle from.
       */
      StatusCode retrieveFeatureParticle(
          const xAOD::IParticle*& particle,
          const HLT::TriggerElement::FeatureAccessHelper& feature,
          const HLT::TriggerElement* te) const;

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
