#ifndef ASSOCIATIONUTILS_SHAREDTRKOVERLAPTOOL_H
#define ASSOCIATIONUTILS_SHAREDTRKOVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"

namespace ORUtils
{

  /// @class EleMuSharedTrkOverlapTool
  /// @brief Tool for removing overlaps between electrons and muons that
  /// share a track.
  ///
  /// I don't yet know if it's straightforward to generalize to any kind of
  /// particles.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class EleMuSharedTrkOverlapTool : public virtual IOverlapTool,
                                    public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(EleMuSharedTrkOverlapTool, IOverlapTool)

    public:

      /// Standard constructor
      EleMuSharedTrkOverlapTool(const std::string& name);

      /// @brief Identify overlaps via shared ID track.
      /// Removes the electron from cont1
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

      /// Alternate method taking actual container types
      StatusCode
      findOverlaps(const xAOD::ElectronContainer& electrons,
                   const xAOD::MuonContainer& muons) const;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    private:

      //
      // Configurable properties
      //

      /// Flag to remove calo-muons overlapping with electrons
      bool m_removeCaloMuons;

      //
      // Utilities
      //

  }; // class EleMuSharedTrkOverlapTool

} // namespace ORUtils

#endif
