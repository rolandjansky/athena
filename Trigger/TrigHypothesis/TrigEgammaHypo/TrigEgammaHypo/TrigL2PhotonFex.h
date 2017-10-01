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



class TrigL2PhotonFex : public HLT::FexAlgo {
  
 public:
  
  TrigL2PhotonFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2PhotonFex();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
			    HLT::TriggerElement* outputTE);
  
 private:
  
  xAOD::TrigPhotonContainer* m_trigPhotonContainer; 
  
};

#endif
