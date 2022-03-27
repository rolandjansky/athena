/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMASUPERCLUSTERBUILDERBASE_H
#define EGAMMAALGS_EGAMMASUPERCLUSTERBUILDERBASE_H

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

// Fwd declarations
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "EgammaAnalysisInterfaces/IegammaMVASvc.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

#include <memory>
#include <vector>

class CaloDetDescrManager;
/** Base class for electronSuperClusterBuilder
 *                 photonSuperClusterBuilder
 *                 egammaSuperClusterBuilder
 * This class cannot be instantiated by itself since the execute method is not
 * implemented.
 *
 * This class provides functions which are used to build supercluster and are
 * independet from the nature of the particles, for example a function to decide
 * if two clusters are compatible to be added in a supercluster
 * (egammaSuperClusterBuilderBase::matchesInWindow) and a function
 * (egammaSuperClusterBuilderBase::createNewCluster) to create a new
 * supercluster from a collection of clusters.
 **/
class egammaSuperClusterBuilderBase : public AthReentrantAlgorithm
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
  egammaSuperClusterBuilderBase(const std::string& name,
                                ISvcLocator* pSvcLocator);

  /** should be called by the derived class in the initialize phase */
  virtual StatusCode initialize() override;

  /**
   * @brief Is clus in window center around ref?
   *
   * @param ref: reference cluster
   * @param clus: cluster to be tested
   *
   * The matching is done using delta-eta and delta-phi comparing them
   * with the values of
   * \ref egammaSuperClusterBuilderBase.m_searchWindowEtaCellsBarrel
   *"SearchWindowEtaCellsBarrel", \ref
   *egammaSuperClusterBuilderBase.m_searchWindowPhiCellsBarrel
   *"SearchWindowPhiCellsBarrel", \ref
   *egammaSuperClusterBuilderBase.m_searchWindowEtaCellsEndcap
   *"SearchWindowEtaCellsEndcap" and \ref
   *egammaSuperClusterBuilderBase.m_searchWindowPhiCellsEndcap
   *"SearchWindowPhiCellsEndcap", depending if the seed is barrel or endcap. If
   *it is in the crack, an OR of the conditions (using both seeds) is used.
   **/
  bool matchesInWindow(const xAOD::CaloCluster* ref,
                       const xAOD::CaloCluster* clus) const;

  /** 
   * Add  new supercluster ,created out of the input clusters,
   * to the newClusters collections.
   *
   * It decides which cells of the seed and the satellite clusters
   * to add (boxing / cookie cutter).
   *
   * The reference point is computed with findCentralPosition
   * which returns the hottest cells looping on the cells of all the
   * considered topo-clusters. Two references are computed,
   * one for the barrel, one for the endcap. Computations are done in the
   * calo-frame. Once the reference is computed the decision is made by
   * egammaSuperClusterBuilderBase::fillClusterConstrained which add the cells
   * for the accordeon.
   *
   * Cells from the tile gap are added using
   * egammaSuperClusterBuilderBase::addTileGap3CellsinWindow.
   *
   * Kinematic properties of the cluster are computed from the property
   * of the cells.
   *
   * If the supercluster has a cluster energy less then EtThresholdCut (also
   * used as threshould for the seed) false returned, and the cluster is not added.
   * The supercluster need to pass
   * egammaCheckEnergyDepositTool::checkFractioninSamplingCluster 
   * if not false is returned  and the cluster is not added. 
   *
   * Calibrations on eta1, energy are applied with
   * egammaSuperClusterBuilderBase::calibrateCluster
   *
   */
  bool createNewCluster(
    const EventContext& ctx,
    const std::vector<const xAOD::CaloCluster*>& clusters,
    const CaloDetDescrManager& mgr,
    xAOD::EgammaParameters::EgammaType egType,
    xAOD::CaloClusterContainer* newClusters,
    xAOD::CaloClusterContainer* precorrClusters) const;

  /** check if we pass the basic criteria for
   * a seed cluster
   */
  bool seedClusterSelection(const xAOD::CaloCluster* clus) const;

  // some constants to use
  static constexpr float s_cellEtaSize = 0.025;
  static constexpr float s_cellPhiSize = M_PI / 128.;
  static constexpr float s_TG3Run3E4cellEtaMax = 1.72;
  static constexpr float s_TG3Run3E3cellEtaMin = 1.2;
  static constexpr float s_TG3Run2E4cellEtaMax = 1.6;
  static constexpr float s_TG3Run2E4cellEtaMin = 1.4;
  static constexpr float s_ClEtaMinForTG3cell = s_TG3Run2E4cellEtaMin-0.03; //Yes, Run2 here
  static constexpr float s_ClEtaMaxForTG3cell = s_TG3Run3E4cellEtaMax+0.03;

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
  /** Find the size of the cluster in phi using L2 cells.
   *
   * @param cp0: the reference position in calo-coordinates
   * @param cluster: the cluster filled with L2 and L3 cells
   *
   * The window is computed using only cells in the second layer.
   * Asymmetric sizes are computed for barrel and endcap. The size
   * is the maximum difference in phi between the center of a cell
   * and the refence, considering separately cells in the barrel
   * and in the endcap. The computation is done separately for the
   * cells with phi < reference phi or >=. A cutoff value of 1 is used.
   */
  PhiSize findPhiSize(const CentralPosition& cp0,
                      const xAOD::CaloCluster& cluster) const;

  /**
   * Fill super cluster constraining its size
   * in eta,phi around the overall hottest cell
   * and the its L2 size
   *
   * @param tofill: empty cluster to be filled
   * @param clusters: seed and satellite clusters
   * @param cp0: this specify the eta/phi reference in the calo-frame. In egamma
   *  reconstruction the references are the hottest cells in L2, in barrel and
   * endcap.
   *
   * Cells are added if their raw coordinates are inside a window in eta and/or
   * phi centered around the hottest cells specified by cp0. If there are two
   * hottest cells (one in the barrel and one in the endcap) both are used in
   * the tests and an OR of the two is considered.
   *
   * First L2 and L3 LAR EM cells are considered (excluding inner wheel endcap
   * cells). A cut in eta is used. The range of the eta-window depends if the
   * seed cell is in barrel or endcap. For example for the barrel, cells in L2
   * are added if they are inside +/- (\ref
   * egammaSuperClusterBuilderBase.m_addCellsWindowEtaCellsBarrel
   * "AddCellsWindowEtaCellsBarrel" * s_cellEtaSize / 2)
   * where \ref egammaSuperClusterBuilderBase.s_cellEtaSize "s_cellEtaSize" is
   * the eta size of one cell in L2. For L3 the semi-window is increased by
   * (\ref egammaSuperClusterBuilderBase.m_extraL3EtaSizeCells
   * "ExtraL3EtaSizeCells" * s_cellEtaSize / 2).
   *
   * Then cells in PS and L1 are added if they match a window in eta and phi.
   * The eta requiriment is the same as in L2. The window in phi
   * is computed asymetrically using the cells in layer 2, using the function
   * egammaSuperClusterBuilderBase::findPhiSize and increading the semi-window
   * by
   * (\ref egammaSuperClusterBuilderBase.m_extraL0L1PhiSize "ExtraL0L1PhiSize" *
   * s_cellPhiSize) where \ref egammaSuperClusterBuilderBase.s_cellPhiSize
   * "s_cellPhiSize" is the size in phi of cells in L2.
   */
  StatusCode fillClusterConstrained(
    xAOD::CaloCluster& tofill,
    const std::vector<const xAOD::CaloCluster*>& clusters,
    const CentralPosition& cp0) const;

  /** add all tile Gap 3 cells in a window.
   * All the cells in the TileGap3 layer which are in a eta x phi window 0.2,
   * ~0.15 centered around the raw position of the cluster param tofill with
   * weight 1.
   */
  StatusCode addTileGap3CellsinWindow(xAOD::CaloCluster& tofill,
                                      const CaloDetDescrManager& mgr) const;

  /** function to calibrate the new clusters energy
   *
   * Calibrate with
   * - egammaSuperClusterBuilderBase.refineEta1Position
   * - \ref egammaSuperClusterBuilderBase.m_clusterCorrectionTool
   * "ClusterCorrectionTool" (default=egammaSwTool)
   * - \ref egammaSuperClusterBuilderBase.fillPositionsInCalo
   * "fillPositionsInCalo"
   * - \ref egammaSuperClusterBuilderBase.m_MVACalibSvc "MVACalibSvc"
   * (default=egammaMVASvc).
   */
  StatusCode calibrateCluster(
    const EventContext& ctx,
    xAOD::CaloCluster* newCluster,
    const CaloDetDescrManager& mgr,
    const xAOD::EgammaParameters::EgammaType egType,
    xAOD::CaloClusterContainer* precorrClusters) const;

  // window values for the windows
  // in which cells of topoclusters are added
  // half of addCells window size, converted in units of eta/phi
  float m_addCellsWindowEtaBarrel;
  float m_addCellsWindowEtaEndcap;
  // Extra opening in phi for L0,L1
  float m_extraL0L1PhiSize;
  // Extra opening in eta for L3 cells
  float m_extraL3EtaSize;

  /** @brief Decorate the supercluster with links to the component topoclusters */
  Gaudi::Property<bool> m_linkToConstituents{
    this,
    "LinkToConstituents",
    true,
    "Link sister clusters to new cluster"
  };

  /** @brief Use extended TG3 definition (only after Run 2) */
  Gaudi::Property<bool> m_useExtendedTG3{
    this,
    "UseExtendedTG3",
    false,
    "Extend TG3 definition from |eta| = 1.2 to |eta| = 1.72"
  };

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

  /** @brief Size of windows et eta in which cells of topoclusters are added for
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

  /** @brief "When adding L3 cells, how much wider in eta than
   * the L2 */
  Gaudi::Property<int> m_extraL3EtaSizeCells{
    this,
    "ExtraL3EtaSizeCells",
    0,
    "When adding L3 cells how much wider (+/- 0.5 of the value) "
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
