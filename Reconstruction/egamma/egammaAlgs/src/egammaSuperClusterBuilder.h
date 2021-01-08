/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMASUPERCLUSTERBUILDER_H
#define EGAMMAALGS_EGAMMASUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

// Fwd declarations
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IegammaMVASvc.h"
#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

#include <memory>

class CaloDetDescrManager;
/** Base class for electronSuperClusterBuilder and photonSuperClusterBuilder.
 *  The inheritance should be private. This class should never be instantiated
 *  by itself, and hence has no interface class
 **/
class egammaSuperClusterBuilder : public AthReentrantAlgorithm
{
public:
  struct CentralPosition
  {
    float etaB = 999;
    float phiB = 999;
    float emaxB = -999 * Gaudi::Units::GeV;
    float etaEC = 999;
    float phiEC = 999;
    float emaxEC = -999 * Gaudi::Units::GeV;
  };

  struct PhiSize
  {
    float plusB = 0;
    float minusB = 0;
    float plusEC = 0;
    float minusEC = 0;
  };

protected:
  /** Protected constructor since this class should not be instantiated by
   * itself */
  egammaSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** should be called by the derived class in the initialize phase */
  virtual StatusCode initialize() override;

  /** Is clus in window center around ref? */
  bool matchesInWindow(const xAOD::CaloCluster* ref,
                       const xAOD::CaloCluster* clus) const;

  /** Creates a new supercluster out of the input cluster */
  // not const because it calls CalibrateCluster
  std::unique_ptr<xAOD::CaloCluster> createNewCluster(
    const EventContext& ctx,
    const std::vector<const xAOD::CaloCluster*>& clusters,
    const CaloDetDescrManager& mgr,
    xAOD::EgammaParameters::EgammaType egType) const;

  // some constants to use
  static constexpr float s_cellEtaSize = 0.025;
  static constexpr float s_cellPhiSize = M_PI / 128.;

  /** @brief Seed selection requirements */
  Gaudi::Property<float> m_EtThresholdCut{
    this,
    "EtThresholdCut",
    1.5 * Gaudi::Units::GeV,
    "The minimum EM Et required of SEED clusters (not applied to secondaries)"
  };

  // these are calculated search window values
  // half of search window size, converted to units of eta,phi
  float m_searchWindowEtaBarrel;
  float m_searchWindowPhiBarrel;
  float m_searchWindowEtaEndcap;
  float m_searchWindowPhiEndcap;

private:
  /** Find the size of the cluster */
  PhiSize findPhiSize(const CentralPosition& cp0,
                      const xAOD::CaloCluster& cluster) const;

  /** fill Super Clusterlimiting its size.*/
  StatusCode fillClusterConstrained(
    xAOD::CaloCluster& tofill,
    const std::vector<const xAOD::CaloCluster*>& clusters,
    const CentralPosition& cp0) const;

  /** functions to add all tile Gap 3 cells in a window*/
  StatusCode addTileGap3CellsinWindow(xAOD::CaloCluster& tofill,
                                      const CaloDetDescrManager& mgr) const;

  /** function to calibrate the new clusters energy */
  StatusCode calibrateCluster(
    const EventContext& ctx,
    xAOD::CaloCluster* newCluster,
    const CaloDetDescrManager& mgr,
    const xAOD::EgammaParameters::EgammaType egType) const;

  /** function to decorate the calo cluster with position variables */
  StatusCode fillPositionsInCalo(xAOD::CaloCluster* cluster,
                                 const CaloDetDescrManager& mgr) const;

  /** functions to refine position in eta1*/
  StatusCode refineEta1Position(xAOD::CaloCluster* cluster,
                                const CaloDetDescrManager& mgr) const;
  StatusCode makeCorrection1(xAOD::CaloCluster* cluster,
                             const CaloDetDescrManager& mgr,
                             const CaloSampling::CaloSample sample) const;

  // window values for the windows
  // in which cells of topoclusters are added
  // half of addCells window size, converted in units of eta/phi
  float m_addCellsWindowEtaBarrel;
  float m_addCellsWindowPhiBarrel;
  float m_addCellsWindowEtaEndcap;
  float m_addCellsWindowPhiEndcap;
  // Extra opening in phi for L0,L1
  float m_extraL0L1PhiSize;
  // Extra opening in eta for L3 cells
  float m_extraL3EtaSize;

  /** @brief Position in Calo frame**/
  CaloCellDetPos m_caloCellDetPos;

  /** @brief Size of topocluster search window in eta for the barrel */
  Gaudi::Property<int> m_searchWindowEtaCellsBarrel{
    this,
    "SearchWindowEtaCellsBarrel",
    5,
    "Number of cells in eta of window in which to search for topoclusters"
  };

  /** @brief Size of topocluster search window in phi for the barrel */
  Gaudi::Property<int> m_searchWindowPhiCellsBarrel{
    this,
    "SearchWindowPhiCellsBarrel",
    5,
    "Number of cells in phi of window in which to search for topoclusters"
  };

  /** @brief Size of topocluster search window in eta for the end-cap*/
  Gaudi::Property<int> m_searchWindowEtaCellsEndcap{
    this,
    "SearchWindowEtaCellsEndcap",
    5,
    "Number of cells in eta of window in which to search for topoclusters"
  };

  /** @brief Size of topocluster search window in phi for the end-cap*/
  Gaudi::Property<int> m_searchWindowPhiCellsEndcap{
    this,
    "SearchWindowPhiCellsEndcap",
    5,
    "Number of cells in phi of window in which to search for topoclusters"
  };

  /** @brief Size of windows et eta in which cells of topoclusters are edded for
   * the barrel (units of 2nd layer cells) */
  Gaudi::Property<int> m_addCellsWindowEtaCellsBarrel{
    this,
    "AddCellsWindowEtaCellsBarrel",
    3,
    "Number of cells in eta of window around topocluster center to add cells"
  };

  /** @brief Size of windows et eta in which cells of topoclusters are edded for
   * the endcap (units of 2nd layer cells) */
  Gaudi::Property<int> m_addCellsWindowEtaCellsEndcap{
    this,
    "AddCellsWindowEtaCellsEndcap",
    5,
    "Number of cells in eta of window around topocluster center to add cells"
  };

  /** @brief "When adding L0 (PS) and L1 cells, how much wider than
   * the L2 size of the cluster  is the acceptance in phi
   * (units of 2nd layer cells)*/
  Gaudi::Property<int> m_extraL0L1PhiSizeCells{
    this,
    "ExtraL0L1PhiSize",
    1,
    "When adding L0 (PS) and L1 cells in phi, "
    "how much wider (+/- of the value) than the L2 phi size of the "
    "cluster (in L2 cells units) is the acceptance"
  };

  /** @brief "When adding L3 cells, how much wider than
   * the L2 */
  Gaudi::Property<int> m_extraL3EtaSizeCells{
    this,
    "ExtraL3EtaSizeCells",
    1,
    "When adding L3 cells  how much wider (+/- 0.5 of the value) "
    "than L2 (in L2 cells "
    "units) is the acceptance in eta"
  };

  /** @brief Handle to the MVA calibration service **/
  ServiceHandle<IegammaMVASvc> m_MVACalibSvc{ this,
                                              "MVACalibSvc",
                                              "egammaMVASvc",
                                              "calibration service" };

  /** @brief Tool to handle cluster corrections */
  ToolHandle<IegammaSwTool> m_clusterCorrectionTool{
    this,
    "ClusterCorrectionTool",
    "egammaSwTool/egammaswtool",
    "tool that applies cluster corrections"
  };

  /** @brief Pointer to the egammaCheckEnergyDepositTool*/
  ToolHandle<IegammaCheckEnergyDepositTool> m_egammaCheckEnergyDepositTool{
    this,
    "egammaCheckEnergyDepositTool",
    "",
    "Optional tool that performs basic checks of viability of cluster"
  };
};

#endif
