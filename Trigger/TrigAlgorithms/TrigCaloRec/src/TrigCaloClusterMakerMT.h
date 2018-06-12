
// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloClusterMaker
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.A. Delsart
 * CREATED:   August 2006
 *
 *          Builds Clusters from CaloCell in a ROI using offline calo tools.
 *          Works currently only for CaloTopoCluster and SWClusters. (hard coded hack...)
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGCALOCLUSTERMAKERMT_H
#define TRIGCALOREC_TRIGCALOCLUSTERMAKERMT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"


class IAlgToolEFCalo;
class CaloClusterCollectionProcessor;
class CaloClusterProcessor;
class CaloClusterContainer;
class CaloCellLinkContainer;

class TrigCaloClusterMakerMT : public AthAlgorithm {

 public:

  TrigCaloClusterMakerMT(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigCaloClusterMakerMT();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:

  std::vector<std::string> m_clusterMakerNames;      // Cluster maker tools name
  std::vector<std::string> m_clusterCorrectionNames; // Cluster correction tools name


  // Following used for testing only :
  //bool        m_useMeaningfullNames;      
  std::string m_clustersOutputName;
 

  //Other members  
  std::vector<CaloClusterCollectionProcessor*> m_clusterMakerPointers;      //pointers to tools
  std::vector<CaloClusterProcessor*>           m_clusterCorrectionPointers; //pointers to tools
  xAOD::CaloClusterContainer* m_pCaloClusterContainer;
  TrigCaloQuality*      m_pTrigCaloQuality;

//   double m_Eta;
//   double m_Phi;
//   double m_Et;
//   double m_rCore;
//   double m_EtaEFvsL2;
//   double m_PhiEFvsL2;
//   double m_IsoFrac;
//   double m_StripWidth;
//   double m_EMFraction;
//   double m_EdRFraction;

  SG::ReadHandleKey<TrigCaloQuality> m_inputCaloQualityKey{ this,
      "CaloQuality",                  // property name
      "caloquality",                                             // default value of StoreGate key
      "input TrigCaloQuality"};

  SG::ReadHandleKey<CaloCellContainer> m_inputCellsKey{ this,
      "Cells",                  // property name
      "cells",                                             // default value of StoreGate key
      "input CaloCellContainer "};

  SG::ReadHandleKey<CaloTowerContainer> m_inputTowersKey{ this,
      "CaloTowers",                  // property name
      "calotowers",                                             // default value of StoreGate key
      "output CaloTowerContainer"};

  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputClustersKey{ this,
      "CaloClusters",                  // property name
      "caloclusters",                                             // default value of StoreGate key
      "output CaloClusterContainer"};


  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  
public:

  inline xAOD::CaloClusterContainer* GetClusterContainer() const {return m_pCaloClusterContainer;}

};
#endif
