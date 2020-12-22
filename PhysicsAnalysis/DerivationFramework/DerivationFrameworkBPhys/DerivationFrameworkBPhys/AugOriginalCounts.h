/* 
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AugOriginalCounts.h
 *
 * @brief  Augmentation with primary vertex counts (before thinning)
 */
 
#ifndef DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
#define DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
 
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"


#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteDecorHandleKey.h"
 
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include <string>

 
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <string>
#include "xAODEventInfo/EventInfo.h"
#include <StoreGate/WriteDecorHandleKey.h>

namespace DerivationFramework {
  ///
  /// @class  AugOriginalCounts
  ///
  /// @brief  Augmentation with primary vertex counts (before thinning)
  ///
  /// This tool adds primary vertex counts and track counts
  /// to the EventInfo container in order to preserve them in 
  /// case the primary vertex or track collections are thinned.
  ///
  /// ### Job options
  /// <table border="0">
  /// <tr><th align="left">Name</td>
  ///     <th align="left">Description</th></tr>
  /// <tr><td>TrackContainer</td>
  ///     <td>name of the TrackParticle container to be used</td>
  /// </tr>
  /// <tr><td>VertexContainer</td>
  ///     <td>name of the Vertex container to be used</td>
  /// </tr>
  /// <tr><td>AddPVCountsByType</td>
  ///     <td>add PV counts by PV type (default: false)</td>
  /// </td>
  /// </table>
  ///
  class AugOriginalCounts : public AthAlgTool, public IAugmentationTool {
  public:
    /// @brief Main constructor
    AugOriginalCounts(const std::string& t, const std::string& n,
		      const IInterface* p);
    /// @brief Main method called for each event
    virtual StatusCode addBranches() const override;
    virtual StatusCode initialize() override;
    private:
    ///
    /// @name job options
    /// @{
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigPVNTracks{this, "DO_NOT_SET1", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigNTracksKeys{this, "DO_NOT_SET2", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigNtype0{this, "DO_NOT_SET3", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigNtype1{this, "DO_NOT_SET4", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigNtype2{this, "DO_NOT_SET5", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigNtype3{this, "DO_NOT_SET6", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_OrigNtypeUnknown{this, "DO_NOT_SET7", "", "internal property"};

    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_OrigSqrtPt2Sum{this, "DO_NOT_SET8", "", "internal property"};
    SG::WriteDecorHandleKey<xAOD::VertexContainer> m_d_nPVTracks{this, "DO_NOT_SET9", "", "internal property"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackContainername;
    SG::ReadHandleKey<xAOD::VertexContainer> m_PVContainername;
    bool        m_addPVCountsByType;
    bool        m_addNTracksToPVs;
    bool        m_addSqrtPt2SumToPVs;
    /// @}
  };
}
 
#endif // DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
