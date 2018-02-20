/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EgammaAnalysisHelpers/PhotonHelpers.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "AsgTools/AsgMessaging.h"

// ==================================================================
bool PhotonHelpers::passOQquality(const xAOD::Photon *ph){

  static const asg::AsgMessaging msg("PhotonHelpers");
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
bool PhotonHelpers::passOQqualityDelayed(const xAOD::Photon *ph){

  static const asg::AsgMessaging msg("PhotonHelpers");
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
