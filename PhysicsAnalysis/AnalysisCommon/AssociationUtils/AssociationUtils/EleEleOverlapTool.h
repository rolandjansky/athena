#ifndef ASSOCIATIONUTILS_ELEELEOVERLAPTOOL_H
#define ASSOCIATIONUTILS_ELEELEOVERLAPTOOL_H

// EDM includes
#include "xAODEgamma/ElectronContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"

namespace ORUtils
{

  /// @class EleEleOverlapTool
  /// @brief A tool implementing the recommended ele-ele overlap removal.
  ///
  /// This tool flags electrons that match to other electrons under two
  /// possible criteria:
  ///   1. The electrons share a track (on by default)
  ///   2. The electron clusters are overlapping (off by default)
  ///
  /// The rejected electron is decided as follows:
  ///   - author 'Ambiguous' rejected from author 'Electron'
  ///   - otherwise, the softer electron is rejected (in PT)
  ///
  /// The implementation here comes from discussions on JIRA issue ATLASG-438.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class EleEleOverlapTool : public virtual IOverlapTool,
                            public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(EleEleOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      EleEleOverlapTool(const std::string& name);

      /// @brief Identify overlapping electrons.
      /// Note that in this tool, the two containers should be the same.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// @brief Identify overlapping electrons and jets.
      /// The above method calls this one.
      virtual StatusCode
      findOverlaps(const xAOD::ElectronContainer& electrons) const;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    private:

      /// Helper method for matching electrons
      bool electronsMatch(const xAOD::Electron& el1, const xAOD::Electron& el2) const;

      /// Helper method to decide which electron to reject
      bool rejectFirst(const xAOD::Electron& el1, const xAOD::Electron& el2) const;

      /// @name Configurable properties
      /// @{

      /// Match electrons by shared track (on by default)
      bool m_useTrackMatch;

      /// Match electrons by cluster distance (off by default)
      bool m_useClusterMatch;

      /// Cluster-matching dEta
      double m_clusterDeltaEta;

      /// Cluster-matching dPhi
      double m_clusterDeltaPhi;

      /// @}

  }; // class EleEleOverlapTool

} // namespace ORUtils

#endif
