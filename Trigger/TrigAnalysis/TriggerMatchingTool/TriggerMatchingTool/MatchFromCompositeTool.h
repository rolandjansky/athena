/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerMatchingTool_MatchFromCompositeTool_H
#define TriggerMatchingTool_MatchFromCompositeTool_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Package includes
#include "TriggerMatchingTool/IMatchingTool.h"


namespace Trig {
  /**
   * @brief Tool to perform matching to selected offline particles using the
   * list of candidates created by the DerivationFramework::TriggerMatchingTool.
   * @author Jon Burr
   */
  class MatchFromCompositeTool final : public virtual IMatchingTool,
                                       public asg::AsgTool
  {
    ASG_TOOL_CLASS( MatchFromCompositeTool, IMatchingTool )
    public:
      /// Standard constructor
      MatchFromCompositeTool(const std::string& name);

      /// Default destructor
      ~MatchFromCompositeTool() override;

      /// Initialise the tool
      StatusCode initialize() override;

      /**
       * @brief Match a single object
       * @param recoObject The object to match.
       * @param chain The chain to match to.
       * The final two parameters for this function are inherited from the
       * interface but have no effect!
       * To follow the implementation of the other tool, the matching returns
       * true if all provided offline objects have a corresponding online
       * object, not if all online objects match a provided offline object.
       */
      bool match(
          const xAOD::IParticle& recoObject,
          const std::string& chain,
          double=0.1, bool=false) override;

      /**
       * @brief Match a multiple objects
       * @param recoObjects The objects to match.
       * @param chain The chain to match to.
       * The final two parameters for this function are inherited from the
       * interface but have no effect!
       * To follow the implementation of the other tool, the matching returns
       * true if all provided offline objects have a corresponding online
       * object, not if all online objects match a provided offline object.
       */
      bool match(
          const std::vector<const xAOD::IParticle*>& recoObject,
          const std::string& chain,
          double=0.1, bool=false) override;

    protected:
      /// Helper typedefs for accessors/decorators, vectors of ele links
      template <typename T>
        using constAcc_t = SG::AuxElement::ConstAccessor<T>;
      template <typename T>
        using dec_t = SG::AuxElement::Decorator<T>;
      template <typename T>
        using vecLink_t = std::vector<ElementLink<T>>;

      // Properties
      /// Allow matching shallow copy to shallow copy
      bool m_matchShallow = true;

      /// If greater than 0 then will skip the above check and just check that
      /// DR between the two particles is less than this value. 
      float m_drThreshold = -1;

      // Internal functions
      /// Inherited from the interface but does nothing
      virtual MatchingImplementation* impl() override { return nullptr; }

      /**
       * @brief Test a combination of offline objects against a combination of
       * online objects
       * @param onlineLinks The online combination
       * @param offline The offline combination
       */
      bool testCombination(
          const vecLink_t<xAOD::IParticleContainer>& onlineLinks,
          const std::vector<const xAOD::IParticle*>& offline) const;

      /// Check if two particles are the same.
      bool areTheSame(
          const xAOD::IParticle& lhs,
          const xAOD::IParticle& rhs) const;
  }; //> end class MatchFromCompositeTool
} //> end namespace Trig

#endif //> TriggerMatchingTool_MatchFromCompositeTool_H
