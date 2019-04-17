/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
/// CaloTopoTowerAlg.cxx, (c) ATLAS Detector software
/// Calorimeter/CaloRec/CaloTopoTowerAlg
///
/// Build towers from raw cells that have been taken
/// out of topo clusters. In this way, we can create
/// noise suppressed raw towers to be saved in the AOD
/// and thus allows re-calibration and re-do MET.
///
/// Authors : 
/// \author Peter Loch <loch@physics.arizona.edu>
/// \author Ariel Schwartzman <sch@slac.stanford.edu>
/// \author David W. Miller <David.Miller@slac.stanford.edu>
///
/// Date : 25 February 2009
///============================================================

#ifndef CALOREC_CALOTOPOTOWERALG
#define CALOREC_CALOTOPOTOWERALG

/**
@class CaloTopoTowerAlg
@brief Concrete algorithm for building noise suppressed CaloTowers from standard towers using cells from topo-clusters

  CaloTopoTowerAlg fills CaloCells which are used in topological
  clusters into CaloTowers for the LAr and Tile Calorimeters. 

\author David W. Miller <David.W.Miller@cern.ch>
\date   25 February 2009 - port from JetRecTool

*/

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"

#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"


class CaloTopoTowerAlg : public AthReentrantAlgorithm
{
 public:

  /// AlgTool constructor
  CaloTopoTowerAlg(const std::string& name, ISvcLocator* pSvcLocator);
 
  /// Destructor                                          
  virtual ~CaloTopoTowerAlg();

  /// initialize
  virtual  StatusCode  initialize() override;

  /// finalize   
  virtual  StatusCode  finalize() override;
  
  /// Execute
  virtual StatusCode execute (const EventContext& ctx) const override;

private:
  // Container name strings
  SG::ReadHandleKey<CaloCell2ClusterMap> m_cellToClusterMapKey;
  SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey;
  SG::ReadHandleKey<CaloTowerContainer> m_towerContainerKey;
  SG::WriteHandleKey<CaloTowerContainer> m_newTowerContainerKey;

  // Selection criteria
  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;
  bool   m_useCellWeights;
   
  /** @brief Key of the CaloNoise Conditions data object. Typical values 
      are '"electronicNoise', 'pileupNoise', or '"totalNoise' (default) */
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};

  float m_cellESignificanceThreshold; 
 
  // Type definitions 
  typedef Navigable<CaloClusterContainer>           nav_t;
    
  // List of calorimeters from which to use cells
  std::vector<std::string> m_includedCalos;
  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;
  bool m_caloSelection;
  
};
#endif
