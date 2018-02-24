/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 ** 
 **   Original Author:       R.Goncalo    
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2PhotonFex.h
 **                   
 **   Class Description: Fex algo for TrigPhotons
 ** 
 **   Authors: R. Goncalo 
 **            V. Perez-Reale
 **            N. Berger
 **            A. Hamilton
 ** 
 **   Created:      Tue Nov 28 16:01:12 CET 2006
 **
 **************************************************************************/ 

#ifndef TRIG_PHOTON_FEX_H 
#define TRIG_PHOTON_FEX_H

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
 * \class TrigL2PhotonFex  
 * \brief TrigL2PhotonFex is an HLT Fex Algorithm that creates TrigPhotons 
 * from the the L2 TrigEMClusters that passed the TrigL2CaloHypo.
 * The TrigPhotons will then be retrieved by the hypothesis algorithm TrigL2PhotonHypo
 * that will perform the corresponding L2 photon selection
 *
 */

class TrigL2PhotonFexMT : public AthAlgorithm {
  
 public:
  
  TrigL2PhotonFexMT(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2PhotonFexMT();
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  
 private:
  
  xAOD::TrigPhotonContainer* m_trigPhotonContainer; 

    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey { this, 
      "RoIs",                             // property name
      "rois",                            // default value of StoreGate key
      "input RoI Collection name"};

  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_TrigEMClusterContainerKey{ this,
      "TrigEMClusterName",       // property name
      "clusters",                                                // default value of StoreGate key
      "input TrigEMCluster Container name"};


};

#endif
