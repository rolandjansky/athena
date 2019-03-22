/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerMatchingTool_IParticleRetrievalTool_H
#define TriggerMatchingTool_IParticleRetrievalTool_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TriggerMatchingTool/IIParticleRetrievalTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace Trig {
  class IParticleRetrievalTool final : public virtual IIParticleRetrievalTool,
                                       public asg::AsgTool
  {
    ASG_TOOL_CLASS( IParticleRetrievalTool, IIParticleRetrievalTool )
    public:
      /// Standard constructor
      IParticleRetrievalTool(const std::string& name);

      /// Default destructor
      ~IParticleRetrievalTool() override;

      /// Initialise the tool
      StatusCode initialize() override;
      
      /**
       * @brief Retrieve the particles that caused this trigger to fire
       * @param[out] combinations All combinations of particles that could have
       * caused the trigger to fire.
       * @param chain The name of the chain.
       * @param rerun Whether to look at the chain in 'rerun' mode.
       * The provided vector will be cleared before use!
       * Note that this also only returns electrons, photons, muons, taus or
       * (for egamma etcut triggers) calo clusters. This tool will not return
       * (b-)jets. For combined (b-)jet+X triggers (where X is one of the above)
       * only the X will be returned.
       * This tool may not work if too much trigger information has been
       * removed.
       */
      StatusCode retrieveParticles(
          std::vector<std::vector<const xAOD::IParticle*>>& combinations,
          const std::string& chain,
          bool rerun = false) override;

      /**
       * @brief Retrieve particles that caused this trigger element to pass.
       * @param[out] combination All particles that caused this TE to fire.
       * @param te The trigger element to be interrogated.
       */
      StatusCode retrieveParticles(
          std::vector<const xAOD::IParticle*>& combination,
          const HLT::TriggerElement* te) const;

      /**
       * @brief Get the type of particle that should be retrieved from this TE.
       * @param egammaTE The TE to query
       * @return The type of object that caused this TE to fire.
       *
       * This function should only be called on a TE from a single leg of an
       * EGamma trigger.
       */
      xAOD::Type::ObjectType getEGammaTEType(const HLT::TriggerElement* te) const;

      /**
       * @brief Retrieve an IParticle from a feature
       * @param[out] particle The retrieved particle
       * @param feature The feature containing the particle
       * @param te The trigger element containing this feature
       */
      StatusCode retrieveFeatureParticle(
          const xAOD::IParticle*& particle,
          const HLT::TriggerElement::FeatureAccessHelper& feature,
          const HLT::TriggerElement* te) const;

    private:
      // Properties
      /// The TrigDecisionTool that will be used to get the navigation
      ToolHandle<Trig::TrigDecisionTool> m_tdt {
        "Trig::TrigDecisionTool/TrigDecisionTool"};
  }; //> end class IParticleRetrievalTool
} //> end namespace Trig

#endif //> !TriggerMatchingTool_IParticleRetrievalTool_H
