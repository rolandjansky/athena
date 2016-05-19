/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloUtils/CaloTopoTowerBuilderToolBase.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;
class CaloTopoTowerContainer;
class ICaloTopoTowerBuilderToolBase;

class CaloTopoTowerAlgorithm : public AthAlgorithm
{
 public:

  typedef std::string                          tool_key;
  typedef CaloTopoTowerBuilderToolBase         tool_type;
  typedef std::vector<tool_type*>              tool_store;
  typedef tool_store::const_iterator           tool_iterator;
  typedef std::map<tool_key,unsigned int>      tool_stats;
  typedef tool_stats::const_iterator           tool_stats_iterator;
  
  /// Algorithm constructor
  CaloTopoTowerAlgorithm(const std::string& name, ISvcLocator* pService);

  virtual ~CaloTopoTowerAlgorithm();

  /// inherited from Algorithm
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

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

  // tool invocation stats
  tool_stats m_toolInvoke;
  tool_stats m_toolAccept;
  tool_stats m_toolReject;

  //////////////////////////////////////////
  // CaloTopoTowerContainer variables
  // Container name strings
  std::string m_cellContainerName;
  std::string m_clusterName;
  std::string m_cellToClusterMapName;
  std::string m_towerContainerName;     // input tower name
  std::string m_newTowerContainerName;  // output tower name

  // Selection criteria
  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;
  bool   m_useCellWeights;

  // Noise tool stuff
  bool m_useNoiseTool;
  bool m_usePileUpNoise;
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
