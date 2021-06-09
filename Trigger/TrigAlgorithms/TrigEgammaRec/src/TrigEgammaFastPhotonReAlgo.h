/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TrigEgammaFastPhotonReAlgo_H 
#define TrigEgammaFastPhotonReAlgo_H


#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


/**
 * Convert the TrigEMCluster to TrigPhoton object
 */

class TrigEgammaFastPhotonReAlgo : public AthReentrantAlgorithm {
  
  public:
  
    TrigEgammaFastPhotonReAlgo(const std::string & name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
  
  private:
  
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey { this, 
      "RoIs",                             // property name
      "rois",                            // default value of StoreGate key
      "input RoI Collection name"};

    SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_TrigEMClusterContainerKey{ this,
      "TrigEMClusterName",       // property name
      "clusters",                // default value of StoreGate key
      "input TrigEMCluster Container name"};

    SG::WriteHandleKey<xAOD::TrigPhotonContainer> m_outputPhotonsKey{ this,
       "PhotonsName",                  // property name
       "Photons",                      // default value of StoreGate key
       "output Photon container name "};
};

#endif
