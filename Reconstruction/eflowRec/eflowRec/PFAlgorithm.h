/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PFALGORITHM_H
#define PFALGORITHM_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecTrack.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "eflowRec/IPFClusterSelectorTool.h"
#include "eflowRec/IPFBaseTool.h"
#include "eflowRec/IPFSubtractionTool.h"

class eflowRecClusterContainer;

class PFAlgorithm : public AthAlgorithm {

public:
  PFAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
  ~PFAlgorithm() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  /** ToolHandle for the PFClusterSelectorTool which creates the set of eflowRecCluster to be used */
  ToolHandle<IPFClusterSelectorTool> m_IPFClusterSelectorTool{this,"PFClusterSelectorTool","PFClusterSelectorTool","ToolHandle for the PFClusterSelectorTool which creates the set of eflowRecCluster to be used"};
  
  /** List of IPFSubtractionTool, which will be executed by this algorithm */
  ToolHandleArray<IPFSubtractionTool> m_IPFSubtractionTools;

  /** List of PFBaseAlgTool, which will be executed by this algorithm */
  ToolHandleArray<IPFBaseTool> m_IPFBaseTools;
  
  /** ReadHandleKey for the eflowRecTrackContainer to be read in */
  SG::ReadHandleKey<eflowRecTrackContainer> m_eflowRecTracksReadHandleKey{this,"eflowRecTracksInputName","eflowRecTracks","ReadHandleKey for the eflowRecTrackContainer to be read in"};

  /** WriteHandleKey for the eflowRecClusterContainer to write out */
  SG::WriteHandleKey<eflowRecClusterContainer> m_eflowRecClustersWriteHandleKey{this,"eflowRecClustersOutputName","eflowRecClusters","WriteHandleKey for the eflowRecClusterContainer to write out"};
  
  /** WriteHandleKey for CaloClusterContainer to be written out */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_caloClustersWriteHandleKey{this,"PFCaloClustersOutputName","PFCaloCluster","WriteHandleKey for CaloClusterContainer to be written out"};

  /** WriteHandleKey for eflowCaloObjectContainer to be written out */
  SG::WriteHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectsWriteHandleKey{this,"eflowCaloObjectsOutputName","eflowCaloObjects","WriteHandleKey for eflowCaloObjectContainer to be written out"};

  /** Online monitoring tool for recording histograms of the alg in action */
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};

  /** Funciton to print out list of tools if in VERBOSE mode */
  void printTools();
  
};
#endif
