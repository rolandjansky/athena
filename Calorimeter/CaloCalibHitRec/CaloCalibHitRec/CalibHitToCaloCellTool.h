/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************************************
// CalibHitToCaloCellTool.h
// Ioannis Nomidis <ioannis.nomidis@cern.ch>
// Dec 2016
// Convert energy deposits from calibration hits to CaloCell, xAOD::CaloCluster
// *****************************************************************************
#ifndef CalibHitToCaloCellTool_H
#define CalibHitToCaloCellTool_H

//#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include <string>
#include <vector>

class CaloCell_ID;
class CaloDM_ID;
class TileID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class CaloCell;
class CaloDetDescrManager;
class CaloCellContainer;

static const InterfaceID IID_CalibHitToCaloCellTool("CalibHitToCaloCellTool", 1, 0);

namespace CalibHitUtils {
  enum EnergyType { EnergyEM=0, EnergyVisible, EnergyTotal, nEnergyTypes };  
}

class CalibHitToCaloCellTool: virtual public AthAlgTool {

 public:
  CalibHitToCaloCellTool(const std::string& t, const std::string& n, const IInterface*  p);
  ~CalibHitToCaloCellTool();
  StatusCode initialize();    
  StatusCode processCalibHitsFromParticle(int barcode=-1) const;
  StatusCode finalize();
  
  static const InterfaceID& interfaceID() { return IID_CalibHitToCaloCellTool;}

  //  inline CaloCellContainer* getTruthCellsCont(CalibHitUtils::EnergyType type) {return m_truthCells[(int)type];}
  //  inline xAOD::CaloClusterContainer* getTruthClustersCont(CalibHitUtils::EnergyType type) {return m_truthClusters[(int)type];}
  
 private:
  int m_caloGain;
  std::vector<std::string> m_calibHitContainerNames;
  bool m_writeTruthCellContainers;
  bool m_writeTruthClusterContainers;
  int m_singleParticleBarcode;
  
  std::string m_tileActiveHitCnt;
  std::string m_tileInactiveHitCnt;
  std::string m_tileDMHitCnt;
  std::string m_larInactHitCnt;
  std::string m_larActHitCnt;
  std::string m_larDMHitCnt;

  bool m_doTile;
  
  std::string m_caloCell_Tot;
  std::string m_caloCell_Vis;
  std::string m_caloCell_Em;
  std::string m_caloCell_NonEm;

  const CaloCell_ID*  m_caloCell_ID;
  const CaloDM_ID*    m_caloDM_ID;
  //const TileID*       m_tile_ID;
  //const LArEM_ID*     m_larEm_ID;
  //const LArHEC_ID*    m_larHec_ID;
  //const LArFCAL_ID*   m_larFcal_ID;

  const CaloDetDescrManager*   m_caloDDMgr;

  std::string m_outputCellContainerName;
  std::string m_outputClusterContainerName;
  
  
};

#endif
