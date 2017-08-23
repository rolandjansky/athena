/* 
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AugOriginalCounts.h
 *
 * @brief  Augmentation with primary vertex counts (before thinning)
 */
 
#ifndef DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
#define DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
 
#include <string>
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
 
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
    virtual StatusCode addBranches() const;

    private:
    ///
    /// @name job options
    /// @{
    std::string m_TrackContainername;
    std::string m_PVContainername;
    bool        m_addPVCountsByType;
    /// @}
  };
}
 
#endif // DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
