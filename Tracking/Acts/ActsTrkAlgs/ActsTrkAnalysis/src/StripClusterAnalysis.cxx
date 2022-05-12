/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "StripClusterAnalysis.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "InDetIdentifier/SCT_ID.h"

namespace ActsTrk {

  StripClusterAnalysis::StripClusterAnalysis(const std::string& name, ISvcLocator *pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) 
  {}
  
  StatusCode StripClusterAnalysis::initialize() {
    ATH_MSG_DEBUG( "Initializing ActsTrk::StripClusterAnalysis" );
    
    ATH_CHECK( m_stripClusterContainerKey.initialize() );

    return AthMonitorAlgorithm::initialize();
  }

  StatusCode StripClusterAnalysis::fillHistograms(const EventContext& ctx) const {
    ATH_MSG_DEBUG(" In ActsTrk::StripClusterAnalysis::fillHistograms()" );
    
    SG::ReadHandle< xAOD::StripClusterContainer > inputStripClusterContainer( m_stripClusterContainerKey, ctx );
    if (!inputStripClusterContainer.isValid()){
        ATH_MSG_FATAL("xAOD::StripClusterContainer with key " << m_stripClusterContainerKey.key() << " is not available...");
        return StatusCode::FAILURE;
    }
    
    const SCT_ID *stripID = nullptr;
    ATH_CHECK( detStore()->retrieve(stripID, "SCT_ID") );

    auto monitor_barrelEndcap = Monitored::Collection("barrelEndcap", *inputStripClusterContainer,
                                                      [&stripID] (const auto* cluster) -> int
                                                      {
                                                        const Identifier& id = stripID->wafer_id(cluster->identifierHash());
                                                        return stripID->barrel_ec(id);
                                                      });
    auto monitor_layerDisk = Monitored::Collection("layerDisk", *inputStripClusterContainer,
						   [&stripID] (const auto* cluster) -> int
                                                   {
                                                     const Identifier& id = stripID->wafer_id(cluster->identifierHash());
                                                     return stripID->layer_disk(id);
                                                   });
    auto monitor_phiModule = Monitored::Collection("phiModule", *inputStripClusterContainer,
						   [&stripID] (const auto* cluster) -> int
						   {
						     const Identifier& id = stripID->wafer_id(cluster->identifierHash());
						     return stripID->phi_module(id);
						   });
    auto monitor_etaModule = Monitored::Collection("etaModule", *inputStripClusterContainer,
						   [&stripID] (const auto* cluster) -> int
						   {
						     const Identifier& id = stripID->wafer_id(cluster->identifierHash());
						     return stripID->eta_module(id);
						   });
    auto monitor_sideModule = Monitored::Collection("sideModule", *inputStripClusterContainer,
						    [&stripID] (const auto* cluster) -> int
						    {
						      const Identifier& id = stripID->wafer_id(cluster->identifierHash());
						      return stripID->side(id);
						    });


    auto monitor_eta = Monitored::Collection("eta", *inputStripClusterContainer,
                                             [] (const auto* cluster) -> double
                                             {
                                               const auto& globalPos = cluster->globalPosition();
					       Amg::Vector3D globalPosition(globalPos(0, 0), globalPos(1, 0), globalPos(2, 0));
                                               return globalPosition.eta();
                                             });
    auto monitor_perp = Monitored::Collection("perp", *inputStripClusterContainer,
					      [] (const auto* cluster) -> float
					      {
						const auto& globalPos = cluster->globalPosition();
						Amg::Vector3D globalPosition(globalPos(0, 0), globalPos(1, 0), globalPos(2, 0));
						return globalPosition.perp();
					      });

    auto monitor_globalX = Monitored::Collection("globalX", *inputStripClusterContainer,
						 [] (const auto* cluster) -> float
						 { return cluster->globalPosition()(0, 0); });
    auto monitor_globalY = Monitored::Collection("globalY", *inputStripClusterContainer,
						 [] (const auto* cluster) -> float
						 { return cluster->globalPosition()(1, 0); });
    auto monitor_globalZ = Monitored::Collection("globalZ", *inputStripClusterContainer,
						 [] (const auto* cluster) -> float
						 { return cluster->globalPosition()(2, 0); });
    
    auto monitor_localX = Monitored::Collection("localX", *inputStripClusterContainer,
						[] (const auto cluster) -> float
						{
						  const auto& localPos = cluster->template localPosition<2>();
						  return localPos(0,0);
						});
    
    auto monitor_localCovXX = Monitored::Collection("localCovXX", *inputStripClusterContainer,
						    [] (const auto* cluster) -> float
						    { return cluster->template localCovariance<2>()(0, 0); });

    auto monitor_sizeX = Monitored::Collection("sizeX", *inputStripClusterContainer,
					       [] (const auto* cluster) -> int
					       { return cluster->channelsInPhi(); });
    
    fill("ActsTrkClusterAnalysis",
	 monitor_barrelEndcap, monitor_layerDisk,
	 monitor_phiModule, monitor_etaModule, monitor_sideModule,
	 monitor_eta, monitor_perp,
	 monitor_globalX, monitor_globalY, monitor_globalZ,
	 monitor_localX, monitor_localCovXX,
	 monitor_sizeX);
    
    return StatusCode::SUCCESS;
  }
  
}
