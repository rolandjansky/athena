/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUSHOTFINDER_H
#define TAURECTOOLS_TAUSHOTFINDER_H

#include "tauRecTools/TauRecToolBase.h"

#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloInterface/IHadronicCalibrationTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

/**
 * @brief Construct the shot candidates
 *        1. select seed cells used to construct the shot candidates 
 *        2. create the shot PFOs by merging the neighbour seed cells in phi direction
 *        3. the cluster of the shot PFO contains cells in a window of 2 x NCellsInEta
 *
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */

class CaloCell_ID;

class TauShotFinder : public TauRecToolBase {

public:
  
  ASG_TOOL_CLASS2(TauShotFinder, TauRecToolBase, ITauToolBase);

  TauShotFinder(const std::string& name);
  virtual ~TauShotFinder() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& tauShotCaloClusContainer, xAOD::PFOContainer& tauShotPFOContainer) const override;

private:

  struct ptSort
  { 
    ptSort( const TauShotFinder& info );
    const TauShotFinder& m_info;
    bool operator()( const CaloCell* c1, const CaloCell* c2 );
  };

  /** @brief Apply preselection of the cells 
   *         Cells within dR < 0.4, in EM1, and pt > 100 MeV are selected
   */
  std::vector<const CaloCell*> selectCells(const xAOD::TauJet& tau, const CaloCellContainer& cellContainer,
                                           const CaloDetDescrManager* detMgr) const;

  /** @brief Select the seed cells used to construct the shot 
   *         Cells must sastisfy:
   *         1. pre-selction: dR < 0.4, in EM1, and pt > 100 MeV
   *         2. have largest pt among the neighbours in the eta direction 
   *         3. no other seed cells as neighbors in the eta direction
   */
  std::vector<const CaloCell*> selectSeedCells(const xAOD::TauJet& tau, const CaloCellContainer& cellContainer,
                                               const CaloDetDescrManager* detMgr) const;

  /** @brief Check whether two cells are neighbours in the phi direction */
  bool isPhiNeighbour(IdentifierHash cell1Hash, IdentifierHash cell2Hash) const;

  /** @brief Get the hottest neighbour cell in the phi direction */ 
  const CaloCell* getPhiNeighbour(const CaloCell& seedCell, const std::vector<const CaloCell*>& seedCells) const;
  
  /** @brief Get neighbour cells in the eta direction */
  std::vector<const CaloCell*> getEtaNeighbours(const CaloCell& cell, const CaloCellContainer& cellContainer, int maxDepth) const;

  /** @brief Get neighbour cells in the eta direction */
  void addEtaNeighbours(const CaloCell& cell,
                        const CaloCellContainer& cellContainer,
                        std::vector<const CaloCell*>& cells,
                        int depth,
                        int maxDepth,
                        bool next) const;

  /** @brief Create the shot cluster 
   *         Shot cluster contains 5x1 cells from the seed cell and hottestneighbour
   *         cell in the phi direction
   */
  xAOD::CaloCluster* createShotCluster(const CaloCell* cell,
                                       const CaloCell* phiNeighCell,
                                       const CaloCellContainer& cellContainer) const;

  /** @brief Get eta bin */
  int getEtaBin(float eta) const;

  /** @brief Get NPhotons in shot */
  int getNPhotons(float eta, float energy) const;

  Gaudi::Property<int> m_nCellsInEta {this, "NCellsInEta"};
  Gaudi::Property<std::vector<float>> m_minPtCut {this, "MinPtCut"};
  Gaudi::Property<std::vector<float>> m_doubleShotCut {this, "AutoDoubleShotCut"};

  SG::ReadHandleKey<CaloCellContainer> m_caloCellInputContainer{this,"Key_caloCellInputContainer", "AllCalo", "input vertex container key"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};
  ToolHandle<IHadronicCalibrationTool> m_caloWeightTool {this, "CaloWeightTool", "H1WeightToolCSC12Generic"};
  
  /// calo cell navigation
  const CaloCell_ID* m_calo_id = nullptr;

};

#endif  // TAURECTOOLS_TAUSHOTFINDER_H
