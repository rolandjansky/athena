/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @file   CalibHitToCaloCellTool.h
 * @author Ioannis Nomidis <ioannis.nomidis@cern.ch>
 * @date   Dec 2016
 * @brief  Convert energy deposits from calibration hits to CaloCell, xAOD::CaloCluster
 */

#ifndef CALOCALIBHITREC_CALIBHITTOCALOCELLTOOL_H
#define CALOCALIBHITREC_CALIBHITTOCALOCELLTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>
#include <vector>

class CaloCell_ID;
class CaloDM_ID;

class CaloCell;
class CaloCellContainer;

static const InterfaceID IID_CalibHitToCaloCellTool("CalibHitToCaloCellTool", 1, 0);

namespace CalibHitUtils {
  enum EnergyType { EnergyEM=0, EnergyVisible, EnergyTotal, nEnergyTypes };  
}

class CalibHitToCaloCellTool: virtual public AthAlgTool {

 public:
  CalibHitToCaloCellTool(const std::string& t, const std::string& n, const IInterface*  p);
  ~CalibHitToCaloCellTool();
  StatusCode initialize() override;
  StatusCode processCalibHitsFromParticle(int barcode=-1) const;
  StatusCode finalize() override;
  
  static const InterfaceID& interfaceID() { return IID_CalibHitToCaloCellTool;}

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

  const CaloCell_ID*  m_caloCell_ID{nullptr};
  const CaloDM_ID*    m_caloDM_ID{nullptr};

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  std::string m_outputCellContainerName;
  std::string m_outputClusterContainerName;
  
  
};

#endif
