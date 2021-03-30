/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOPOTOWERALGORITHM_H
#define CALOREC_CALOTOPOTOWERALGORITHM_H
///////////////////////////////////////////////////////////////////////////////
/// \brief CaloTopoTower builder algorithm
///
/// The CaloTopoTowerAlgorithm executes a sequence of CaloTopoTowerBuilderTools 
/// defined in the jobOptions. It creates, records and locks a 
/// CaloTopoTowerContainer into StoreGate. Even though this class is probably
/// sufficient for all tower builder strategies, it has been designed as
/// a concrete base class, thus allowing to overwrite the default behavior with
/// very specific client implementations.
///
/// \author Ian M. Nugent <inugent@triumf.ca>
/// \date March 15, 2010 - first implementation
///
/// Based on CaloTowerAlgorithm
///
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloUtils/CaloTopoTowerBuilderToolBase.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include <string>
#include <vector>
#include <map>

class CaloTopoTowerContainer;
class ICaloTopoTowerBuilderToolBase;

class CaloTopoTowerAlgorithm : public AthReentrantAlgorithm
{
 public:

  typedef CaloTopoTowerBuilderToolBase         tool_type;
  typedef std::vector<tool_type*>              tool_store;
  typedef tool_store::const_iterator           tool_iterator;
  
  /// Algorithm constructor
  CaloTopoTowerAlgorithm(const std::string& name, ISvcLocator* pService);

  virtual ~CaloTopoTowerAlgorithm();

  /// inherited from Algorithm
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 protected:

  ////////////////
  // Properties //
  ////////////////

  // link output container
  bool m_genericLink;

  // tool collection
  std::vector<std::string> m_toolNames;
  ToolHandleArray<ICaloTopoTowerBuilderToolBase> m_ptools;

  /////////////////////////
  // Stores and Services //
  /////////////////////////

  // list of tools
  tool_store m_tools;

  //////////////////////////////////////////
  // CaloTopoTowerContainer variables
  // Container name strings
  SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey;
  SG::ReadHandleKey<CaloClusterContainer> m_clusterKey;
  SG::ReadHandleKey<CaloCell2ClusterMap> m_cellToClusterMapKey;
  SG::ReadHandleKey<CaloTowerContainer> m_towerContainerKey;
  SG::WriteHandleKey<CaloTopoTowerContainer> m_newTowerContainerKey;

  // Selection criteria
  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;
  bool   m_useCellWeights;

  // Noise tool stuff
  float m_noiseSigma;
  float m_cellESignificanceThreshold;

  // Type definitions
  //  typedef Navigable<CaloClusterContainer>           nav_t;

  // List of calorimeters from which to use cells
  std::vector<std::string> m_includedCalos;
  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;
  bool m_caloSelection;

  //END CaloTopoTowerContainer variables
  ////////////////////////////////////////////////
};
#endif
