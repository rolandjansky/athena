#ifndef ASSOCIATIONUTILS_OVERLAPTOOLDR_H
#define ASSOCIATIONUTILS_OVERLAPTOOLDR_H

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"
#include "AssociationUtils/DeltaRMatcher.h"

namespace ORUtils
{

  /// @class DeltaROverlapTool
  /// @brief A simple overlap finder that uses a dR match.
  ///
  /// This class will remove _all_ objects that fit the criteria.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class DeltaROverlapTool : public virtual IOverlapTool,
                            public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(DeltaROverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      DeltaROverlapTool(const std::string& name);

      /// @brief Identify overlaps with simple dR check.
      /// Flags all objects in cont1 which are found to overlap
      /// with any object in cont2 within the configured dR window.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    private:

      /// @name Configurable properties
      /// @{

      /// Delta-R cone for flagging objects as overlap.
      float m_dR;
      /// Calculate delta-R using rapidity
      bool m_useRapidity;

      /// In default configuration, the first container is the one that gets
      /// tested for object rejection. If this switch is true, it will instead
      /// be the second container which gets flagged. This is useful for
      /// modifying the behavior of a master tool in which the argument order
      /// is fixed.
      bool m_swapContainerPrecedence;

      /// @}

      /// @name Utilities
      /// @{

      /// Delta-R matcher
      std::unique_ptr<DeltaRMatcher> m_dRMatcher;

      /// @}

  }; // class DeltaROverlapTool

} // namespace ORUtils

#endif
