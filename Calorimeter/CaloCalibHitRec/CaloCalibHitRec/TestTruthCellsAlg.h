/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TestTruthCellsAlg.h
//
// Author   : Gia	gia@mail.cern.ch
// Created  : March. 2005
//
//  
//****************************************************************************
#ifndef TestTruthCellsAlg_H
#define TestTruthCellsAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include <string>
#include <vector>
#include "TH1F.h"

class ITHistSvc;
class CaloCalibrationHitContainer;
class CaloCellContainer;

class TestTruthCellsAlg : public AthAlgorithm
{
 public:
  
  TestTruthCellsAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TestTruthCellsAlg();                         
    
  StatusCode initialize();    
  StatusCode execute();
  StatusCode finalize();

 private:

  std::string m_truthCellsContainerName;
  std::string m_truthClustersContainerName;
    
  std::vector<std::string> m_caloCalibHitsContainerName;
  std::vector<const CaloCalibrationHitContainer*>  *calibHitContainers;

  std::vector<TH1F*> hCalibHitEvsTruthE;
  TH1F* hCalibHitEtotvsTruthE;
  TH1F* hCalibHitEnotilevsTruthE;
  
  TH1F* hClusterEvsSumCellE;
  TH1F* hCalibHitEvsClusterE;
  TH1F* hClusterEvsTruthE;

  TH1F* hClusterTrueEvsTruthShowerE;
  TH1F* hClusterEResolution;
     
  ITHistSvc* m_ths;

  //
  float getTrueEnergyInCluster(const xAOD::CaloCluster* cl,const CaloCellContainer* truthCells);
  
};

#endif
