/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TRIG_TrigEgammaFastPhotonFexMT_H 
#define TRIG_TrigEgammaFastPhotonFexMT_H

// athena stuff
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthLinks/ElementLink.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/FexAlgo.h"

// trigger EDM
//#include "TrigCaloEvent/TrigEMClusterContainer.h"
//#include "TrigParticle/TrigPhoton.h"
//#include "TrigParticle/TrigPhotonContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"



/**
 * \class TrigEgammaFastPhotonFexMT  
 * \brief TrigEgammaFastPhotonFex is an HLT Fex Algorithm that creates TrigPhotons 
 * from the the L2 TrigEMClusters that passed the TrigL2CaloHypo.
 * The TrigPhotons will then be retrieved by the hypothesis algorithm TrigL2PhotonHypo
 * that will perform the corresponding L2 photon selection
 *
 */

class TrigEgammaFastPhotonFexMT : public AthAlgorithm {
  
 public:
  
  TrigEgammaFastPhotonFexMT(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEgammaFastPhotonFexMT();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  
 private:
  
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey { this, 
      "RoIs",                             // property name
      "rois",                            // default value of StoreGate key
      "input RoI Collection name"};

  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_TrigEMClusterContainerKey{ this,
      "TrigEMClusterName",       // property name
      "clusters",                                                // default value of StoreGate key
      "input TrigEMCluster Container name"};

  SG::WriteHandleKey<xAOD::TrigPhotonContainer> m_outputPhotonsKey{ this,
       "PhotonsName",                  // property name
       "Photons",                                             // default value of StoreGate key
       "output Photon container name "};


};

#endif
