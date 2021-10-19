/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
  */

#include "egammaMonitorSuperClusterAlgorithm.h"

egammaMonitorSuperClusterAlgorithm::egammaMonitorSuperClusterAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  egammaMonitorBaseAlgorithm( name, pSvcLocator )

{}

StatusCode egammaMonitorSuperClusterAlgorithm::initialize() 
{
  
  ATH_CHECK(m_inputEgammaRecContainerKey.initialize());
  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());
  ATH_CHECK(egammaMonitorBaseAlgorithm::initialize());
  return StatusCode::SUCCESS;
}

StatusCode egammaMonitorSuperClusterAlgorithm::execute(const EventContext& ctx) const
{
  
  fillSuperClusterQuantities(ctx);

  return StatusCode::SUCCESS;
}


void egammaMonitorSuperClusterAlgorithm::fillSuperClusterQuantities( const EventContext& ctx) const{

    SG::ReadHandle<EgammaRecContainer> egammaRec(m_inputEgammaRecContainerKey, ctx);
    ATH_MSG_DEBUG("Fill cluster quantities: ");
    
    std::vector<float> et_vec, eta_vec, phi_vec, clusterSize_vec, signalState_vec;
    
    auto et_col = Monitored::Collection("et" , et_vec);
    auto eta_col = Monitored::Collection("eta" , eta_vec);
    auto phi_col = Monitored::Collection("phi", phi_vec);
    auto clusterSize_col = Monitored::Collection("clusterSize" ,clusterSize_vec);
    auto signalState_col = Monitored::Collection("signalState" ,signalState_vec);
    
    for (const auto *const eg : *egammaRec) {

        et_vec.push_back(eg->caloCluster()->et()/Gaudi::Units::GeV);
        eta_vec.push_back(eg->caloCluster()->eta());
        phi_vec.push_back(eg->caloCluster()->phi());
        clusterSize_vec.push_back(eg->caloCluster()->clusterSize());
        signalState_vec.push_back(eg->caloCluster()->signalState());
        
    }
    auto mon = Monitored::Group(m_monTool, et_col,eta_col,phi_col, clusterSize_col,signalState_col);
    ATH_MSG_DEBUG("SuperCluster - Online Monitoring in Reconstruction ..."); 
    
}
