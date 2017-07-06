/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/PhotonSelectorHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/Photon.h"
#include "AsgTools/AsgMessaging.h"

// ==================================================================

bool PhotonSelectorHelpers::passOQquality(const xAOD::Photon *ph){

 // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("PhotonSelectorHelpers");

  if(!ph){
    msg.msg(MSG::WARNING) << "No photon found!" << endmsg;
    return 0;
  }
  
  if( !( ( ph->OQ() & 1073741824 )!=0 ||
	 ( ( ph->OQ() & 134217728 )!=0 &&
	   ( ph->showerShapeValue(xAOD::EgammaParameters::Reta) > 0.98
	    || ph->showerShapeValue(xAOD::EgammaParameters::f1) > 0.4
	    || (ph->OQ() & 67108864) !=0)
	   ) ) ){
    return true;
  } 
  
  return false;
}

// ==================================================================
bool PhotonSelectorHelpers::passOQqualitydelayed(const xAOD::Photon *ph){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("PhotonSelectorHelpers");

  if(!ph){
    msg.msg(MSG::WARNING) << "No photon found!" << endmsg;
    return 0;
  }
  
  if( !( ( ph->OQ() & 1073741824)!=0 ||
	 ( ( ph->OQ() & 134217728)!=0 &&
	   ( ph->showerShapeValue(xAOD::EgammaParameters::Reta) > 0.98
	     || ph->showerShapeValue(xAOD::EgammaParameters::f1) > 0.4
	     ) ) ) ) {
    return true;
  }
   
  return false;
}
