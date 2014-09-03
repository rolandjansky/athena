/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOPOTOWERBUILDERTOOL_H
#define CALOREC_CALOTOPOTOWERBUILDERTOOL_H
///////////////////////////////////////////////////////////////////////////////
/// \brief CaloTopoTowerBuilderTool is a tower builder tool implementation class
///
/// CaloTopoTowerBuilderTool is a tower builder tool implementation class.
///
/// \author Ian M. Nugent
/// \date March 15, 2010 - first implementation
///
/// Based on CaloTowerBuilderTool
///
///////////////////////////////////////////////////////////////////////////////

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloUtils/CaloTopoTowerBuilderToolBase.h"


#include <string>
#include <vector>

class CaloTowerStore;

class CaloTopoTowerContainer;
class CaloClusterContainer;
class CaloCell2ClusterMap;
class StoreGateSvc;


#include "CaloEvent/CaloClusterContainer.h"

class MsgStream;

class CaloTopoTowerBuilderTool : public CaloTopoTowerBuilderToolBase
{
 public:
  
  /// AlgTool constructor
  CaloTopoTowerBuilderTool(const std::string& name, const std::string& type,
		       const IInterface* parent);
  virtual ~CaloTopoTowerBuilderTool();

  /// execute
  virtual StatusCode execute(CaloTopoTowerContainer* theContainer,const CaloCellContainer* theCell=0);
  //virtual StatusCode initialize();

  virtual StatusCode initializeTool();

  virtual void handle(const Incident&);

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

 protected:

  /////////////////////////////
  // Specific Initialization //
  /////////////////////////////

  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;

  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;
  bool   m_useCellWeights;
  
  // Noise tool stuff
  bool m_useNoiseTool;
  bool m_usePileUpNoise;
  float m_noiseSigma;
  float m_cellESignificanceThreshold;
    
  // List of calorimeters from which to use cells
  bool m_caloSelection;

 private:
  const CaloCell2ClusterMap* CreateCaloCell2ClusterMap(const CaloClusterContainer* c);
  const CaloDetDescrManager* m_calo_dd_man; 
  const CaloCell_ID* m_calo_id;

  // Type definitions 
  typedef Navigable<CaloClusterContainer>           nav_t;



};
#endif
