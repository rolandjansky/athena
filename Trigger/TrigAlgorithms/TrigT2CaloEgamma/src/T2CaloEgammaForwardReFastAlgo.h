/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/




#ifndef TRIGT2CALOEGAMMA_T2CaloEgammaForwardReFastAlgo_H
#define TRIGT2CALOEGAMMA_T2CaloEgammaForwardReFastAlgo_H

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
class T2CaloEgammaForwardReFastAlgo : public AthReentrantAlgorithm {

  public:
    T2CaloEgammaForwardReFastAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;

  private:

   
    Gaudi::Property<double> m_etaWidth{this, "EtaWidth", 0.1, "Eta Width of the Region of Interest"};
    Gaudi::Property<double> m_phiWidth{this, "PhiWidth", 0.1, "Phi Width of the Region of Interest"};
 
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
