
// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloClusterProcessor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"


class IAlgToolEFCalo;
class CaloClusterCollectionProcessor;
class CaloClusterProcessor;
class CaloClusterContainer;
class CaloCellLinkContainer;

class TrigCaloClusterMakerMT : public AthReentrantAlgorithm {

 public:

  TrigCaloClusterMakerMT(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigCaloClusterMakerMT();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

 private:

  ToolHandleArray<CaloClusterCollectionProcessor> m_clusterMakers
  { this, "ClusterMakerTools", {}, "" };
  ToolHandleArray<CaloClusterProcessor> m_clusterCorrections
  { this, "ClusterCorrectionTools", {}, "" };

  // Following used for testing only :
  //bool        m_useMeaningfullNames;      
  std::string m_clustersOutputName;

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

  SG::ReadHandleKey<CaloCellContainer> m_inputCellsKey{ this,
      "Cells",                  // property name
      "cells",                                             // default value of StoreGate key
      "input CaloCellContainer "};

#if 0
  SG::ReadHandleKey<TrigCaloQuality> m_inputCaloQualityKey{ this,
      "CaloQuality",                  // property name
      "caloquality",                                             // default value of StoreGate key
      "input TrigCaloQuality"};

  SG::ReadHandleKey<CaloTowerContainer> m_inputTowersKey{ this,
      "CaloTowers",                  // property name
      "calotowers",                                             // default value of StoreGate key
      "output CaloTowerContainer"};
#endif

  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputClustersKey{ this,
      "CaloClusters",                  // property name
      "caloclusters",                                             // default value of StoreGate key
      "output CaloClusterContainer"};

  // This is to write out the cells container so it is available at later steps
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_clusterCellLinkOutput{ this,
      "CellLinks",
      "celllinks",
      "Output cell links"};


  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::CaloClusterContainer> > m_mDecor_ncells { this,
      "Decor_ncells",                // decorator name
      "nCells",                      // default value
      "Decorator containing the number of cells associated to a cluster"};

  bool m_isSW{false};
};
#endif
