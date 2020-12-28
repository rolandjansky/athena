/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

 NAME:     T2CaloEgammaReFastAlgo.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   Denis Oliveira Damazio

 PURPOSE:  Reentrant algorithm for egamma reconstruction

 **/

#ifndef TRIGT2CALOEGAMMA_T2CALOEGAMMAREFASTALGO_H
#define TRIGT2CALOEGAMMA_T2CALOEGAMMAREFASTALGO_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <string>

class IReAlgToolCalo;

/** Main LVL2 Algorithm. Processes LVL1 information, call FEX
    IReAlgToolCalos and produces the TrigEMCluster output. */
class T2CaloEgammaReFastAlgo : public AthReentrantAlgorithm {

  public:
    T2CaloEgammaReFastAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;
    /** calculate zo mass */
    // float calculateZ0(const float etaLayer1, const float etaLayer2);
  
  private:
    // Properties:
    Gaudi::Property<float> m_l1eta{this, "L1ForceEta", -10.0, "Forced LVL1 eta"};
    Gaudi::Property<float> m_l1phi{this, "L1ForcePhi", -10.0, "Forced LVL1 phi"};
    Gaudi::Property<double> m_etaWidth{this, "EtaWidth", 0.1, "Eta Width of the Region of Interest"};
    Gaudi::Property<double> m_phiWidth{this, "PhiWidth", 0.1, "Phi Width of the Region of Interest"};
  
    Gaudi::Property<bool> m_storeCells{this, "StoreCells", false,
                                       "store cells in container attached to RoI"};
  
    ToolHandleArray<IEgammaCalibration> m_calibsBarrel{
        this, "CalibListBarrel", {}, "list of calib tools for the Barrel clusters"};
    ToolHandleArray<IEgammaCalibration> m_calibsEndcap{
        this, "CalibListEndcap", {}, "list of calib tools for the EndCap clusters"};
  
    Gaudi::Property<std::vector<float>> m_rhoEta{
        this, "RhoEta", {}, "Variables to calculate Z0 position"};
    Gaudi::Property<std::vector<float>> m_zEta{
        this, "ZEta", {}, "Variables to calculate Z0 position"};
  
    ToolHandleArray<IReAlgToolCalo> m_emAlgTools{
        this, "IReAlgToolList", {}, "list of ReAlgToolCalos for feature extraction"};
    SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey {
	this, "BCIDAvgKey", "CaloBCIDAverage", "" };
  
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{
      this, "RoIs", "OutputRoIs", "input RoIs"};
    SG::WriteHandleKey<xAOD::TrigEMClusterContainer> m_clusterContainerKey{
        this, "ClustersName", "CaloClusters", "Calo cluster container"};
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};

#endif
