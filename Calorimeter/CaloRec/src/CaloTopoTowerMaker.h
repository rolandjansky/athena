/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOREC_CALOTOPOTOWERMAKER_H
#define CALOREC_CALOTOPOTOWERMAKER_H

/// @class  CaloTopoTowerMaker
/// @author Tasnuva Chowdhury
/// @date   16-September-2020
/// @brief Top algorithm to reconstruct @c xAOD::CaloCluster objects representing topo-towers from CaloClusters 
/// 
/// This class is an Algorithm to reconstruct CaloTopoTowers which are represented by
/// objects of type @c xAOD::CaloCluster from cells extracted from existing 
/// topo-clusters (also represented by @c xAOD::CaloCluster type objects).
/// The algorithm first extracts all cells form the topo-clusters, stores the
/// LCW calibration weights for each cell (if configured), fills the towers
/// with the cells and applys a topo-tower calibration using the LCW weights
/// (if configured)>
///

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// -- interfaces
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloTowerCollectionProcessor.h"
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloTowerGeometrySvc.h"

class CaloClusterCellLinkContainer;
class IChronoStatSvc;

class CaloTopoTowerMaker : public AthReentrantAlgorithm
{

 public:

  CaloTopoTowerMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloTopoTowerMaker() override;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

  const std::string& getOutputContainerName() const;

 private:

  /**
   * @brief Method to create a CaloClusterContainer together with it's AuxStore and Link container
   *
   */
  StatusCode makeContainer();

  
  /** @brief the name of the key in StoreGate for the output
      CaloClusterContainer */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_towerOutput;

  /** @brief the name of the key in StoreGate for the output
      CaloClusterCellLinkContainer */
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_towerCellLinkOutput;

  /// @brief Tower makers
  /// 
  /// This tool is the tower maker. It fills 
  /// a @c xAOD::CaloClusterContainer with clusters representing 
  /// towers and also retuns the LCW cell weights if configured. 
  ToolHandle<CaloTowerCollectionProcessor>  m_towerMakerTool;

  /// @brief Tower (cluster) moment makers
  /// 
  /// The tools in this list calculate cluster moments for towers after those
  /// are filled. The tools are the as the ones used for topo-clusters.
  /// Any number of tools can be scheduled. 
  ToolHandleArray<CaloClusterCollectionProcessor> m_towerCorrectionTools; 

  /// @brief Tower calibrator(s)
  ///
  /// This tool provides the (optional) LCW calibration.
  ToolHandle<CaloTowerCollectionProcessor> m_towerCalibratorTool;

  /// @brief Tower geometry service
  ServiceHandle<CaloTowerGeometrySvc> m_towerGeometrySvc { this, "CaloTowerGeometrySvc", "CaloTowerGeometrySvc", "CaloTower geometry provider" };

  /// Handle to the ChronoStatSvc
  ServiceHandle<IChronoStatSvc> m_chrono;

  /** @brief Keep the individual results of each correction.
   *
   * If true, we keep in StoreGate the complete list of clusters
   * before each correction has been performed. */
  //MT-unfriendly:bool m_keep_each_correction;

  /** 
   * @brief controls saving the uncalibrated signal state just before
   * the first @c CaloClusterCorrectionTool is invoked. Is a configurable 
   * property with default value @c true.
   */
  bool m_saveSignalState   = { true  };

  ///Use ChronotStatSvc to monitor each tool
  bool m_chronoTools       = { false };

  /// @brief Use LCW calibration for topo-towers (default is @c true)
  bool m_useLCWCalibration = { true  };

};
#endif // CALOREC_CALOTOPOTOWERMAKER_H



