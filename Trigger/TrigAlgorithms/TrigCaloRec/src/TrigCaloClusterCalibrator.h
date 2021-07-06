// Hi Emacs ! this is -* C++ -*-

/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

/********************************************************************
 *
 * NAME:      TrigCaloClusterCalibrator
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    Jon Burr
 * CREATED:   2020/07/10
 *
 * Shallow copy an existing cluster container and apply cluster processors to
 * it. Largely copied from the TrigCaloClusterMaker.
 *********************************************************************/

#ifndef TRIGCALOREC_TRIGCALOCLUSTERCALIBRATOR_H
#define TRIGCALOREC_TRIGCALOCLUSTERCALIBRATOR_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterProcessor.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

class TrigCaloClusterCalibrator : public AthReentrantAlgorithm {
  public:
    TrigCaloClusterCalibrator(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    ToolHandleArray<CaloClusterProcessor> m_clusterCorrections
    {this, "ClusterCorrectionTools", {}, ""};

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClustersKey{
      this, "InputClusters", "",
      "The input calocluster container to be shallow copied"};

    SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputClustersKey{
      this, "OutputClusters", "",
      "The output, calibrated calocluster container"};

    SG::WriteHandleKey<CaloClusterCellLinkContainer> m_outputCellLinksKey{
      this, "OutputCellLinks", "",
      "The output cell links containing the updated weights"};

    ToolHandle<GenericMonitoringTool> m_monTool{
      this, "MonTool", "", "The monitoring tool"};

    Gaudi::Property<bool> m_isSW{
      this, "IsSW", false, 
      "Is this running corrections to 'SW' clusters? If it is, then cluster "
      "processors will be run selectively based on the cluster's eta"};
}; //> end class TrigCaloClusterCalibrator

#endif //> !TRIGCALOREC_TRIGCALOCLUSTERCALIBRATOR_H
