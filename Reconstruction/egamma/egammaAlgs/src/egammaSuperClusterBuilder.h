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
  float m_searchWindowEtaBarrel; //!< half of search window size, converted to
                                 //!< units of eta
  float m_searchWindowPhiBarrel; //!< half of search window size, converted to
                                 //!< units of phi
  float m_searchWindowEtaEndcap; //!< half of search window size, converted to
                                 //!< units of eta
  float m_searchWindowPhiEndcap; //!< half of search window size, converted to
                                 //!< units of phi
  //
private:
  struct CentralPosition
  {
    float etaB = 999;
    float phiB = 999;
    float emaxB = -999 * Gaudi::Units::GeV;
    float etaEC = 999;
    float phiEC = 999;
    float emaxEC = -999 * Gaudi::Units::GeV;
  };

  /** Find the reference position (eta, phi) relative to which cells are
     restricted. The return value is whether it succeeded in finding a positive
     energy max value. (If rv = false, the output variables are passed as
     arguments are not updated.)
  */
  CentralPosition findCentralPosition(
    const std::vector<const xAOD::CaloCluster*>& clusters) const;

  struct PhiSize
  {
    float plusB = 0;
    float minusB = 0;
    float plusEC = 0;
    float minusEC = 0;
  };

  /** Find the size of the cluster */
  PhiSize findPhiSize(const CentralPosition& cp0,
                      const xAOD::CaloCluster* cluster) const;

  /** Add the EM cells from reference cluster to self; eta and phi are the ones
     to use for limiting size. This excludes L1 (which is done as a separate
     step). note, use raw eta and phi! */
  StatusCode addEMCellsToCluster(xAOD::CaloCluster* newCluster,
                                 const xAOD::CaloCluster* ref,
                                 const CentralPosition& cp0) const;

  /** Add the preshower and L1 EM cells from reference cluster to self; note,
   * use raw eta and phi! */
  StatusCode addL0L1EMCellsToCluster(xAOD::CaloCluster* newCluster,
                                     const xAOD::CaloCluster* ref,
                                     const CentralPosition& cp0,
                                     const PhiSize& phiSize) const;

  /** functions to add all tile Gap 3 cells in a window*/
  StatusCode addTileGap3CellsinWindow(xAOD::CaloCluster* myCluster,
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
  // above can't be const because m_caloCellDetPos acceses are not const

  /** functions to refine position in eta1*/
  StatusCode refineEta1Position(xAOD::CaloCluster* cluster,
                                const CaloDetDescrManager& mgr) const;
  StatusCode makeCorrection1(xAOD::CaloCluster* cluster,
                             const CaloDetDescrManager& mgr,
                             const CaloSampling::CaloSample sample) const;

  // these are calculated window values for the windows in which cells of
  // topoclusters are edded
  float m_addCellsWindowEtaBarrel; //!< half of addCells window size, converted
                                   //!< to units of eta
  float m_addCellsWindowPhiBarrel; //!< half of addCells window size, converted
                                   //!< to units of phi
  float m_addCellsWindowEtaEndcap; //!< half of addCells window size, converted
                                   //!< to units of eta
  float m_addCellsWindowPhiEndcap; //!< half of addCells window size, converted
                                   //!< to units of phi
  float
    m_extraL0L1PhiSize; //!< calculated value of cells to add in units of phi

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
   * the barrel */
  Gaudi::Property<int> m_addCellsWindowEtaCellsBarrel{
    this,
    "AddCellsWindowEtaCellsBarrel",
    3,
    "Number of cells in eta of window around topocluster center to add cells"
  };

  /** @brief Size of windows et phi in which cells of topoclusters are edded for
   * the barrel */
  Gaudi::Property<int> m_addCellsWindowPhiCellsBarrel{
    this,
    "AddCellsWindowPhiCellsBarrel",
    999,
    "Number of cells in phi of window around topocluster center to add cells"
  };

  /** @brief Size of windows et eta in which cells of topoclusters are edded for
   * the endcap */
  Gaudi::Property<int> m_addCellsWindowEtaCellsEndcap{
    this,
    "AddCellsWindowEtaCellsEndcap",
    5,
    "Number of cells in eta of window around topocluster center to add cells"
  };

  /** @brief Size of windows et phi in which cells of topoclusters are edded for
   * the endcap */
  Gaudi::Property<int> m_addCellsWindowPhiCellsEndcap{
    this,
    "AddCellsWindowPhiCellsEndcap",
    999,
    "Number of cells in phi of window around topocluster center to add cells"
  };

  /** @brief "When adding L0 (PS) and L1 cells, how much wider than L2 is the
   * acceptance */
  Gaudi::Property<int> m_extraL0L1PhiSizeCells{
    this,
    "ExtraL0L1PhiSize",
    1,
    "When adding L0 (PS) and L1 cells, how much wider than L2 (in L2 cells "
    "units) is the acceptance"
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

  /** @brief Position in Calo frame**/
  CaloCellDetPos m_caloCellDetPos;
};

#endif
