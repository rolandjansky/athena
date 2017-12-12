/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/AsgDeadHVCellRemovalTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEventInfo/EventInfo.h"

AsgDeadHVCellRemovalTool::AsgDeadHVCellRemovalTool(std::string myname) :
  asg::AsgTool(myname){
}

AsgDeadHVCellRemovalTool::~AsgDeadHVCellRemovalTool(){
}

StatusCode AsgDeadHVCellRemovalTool::initialize(){
  return StatusCode::SUCCESS ;
}

StatusCode AsgDeadHVCellRemovalTool::finalize(){
  return StatusCode::SUCCESS;
}

bool AsgDeadHVCellRemovalTool::accept( const xAOD::Egamma* eg ) const{
  
  if ( !eg ){
    ATH_MSG_ERROR ("Failed, no egamma object.");
    return false;
  }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();  
  if ( !cluster ){
    ATH_MSG_ERROR ("Failed, cluster.");
    return false;
  }  

  const xAOD::EventInfo *eventInfo = evtStore()->retrieve< const xAOD::EventInfo> ("EventInfo");
  bool isSimul= eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  
  unsigned int runnumber (0);
  static const SG::AuxElement::ConstAccessor<unsigned int> randomrunnumber("RandomRunNumber");
  static const SG::AuxElement::ConstAccessor<unsigned int> datarunnumber("runNumber");
  
  if(!isSimul&& datarunnumber.isAvailable(*eventInfo) ){
    runnumber = datarunnumber(*eventInfo);
  }
  else if(isSimul && randomrunnumber.isAvailable(*eventInfo) ){
    runnumber = randomrunnumber(*eventInfo);
  }
  else {
    ATH_MSG_ERROR ("Failed, to retrieve the proper run number from event info");
    return false;
  }
  
  double eta_calo;
  double phi_calo;
  if (! cluster->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,eta_calo) || 
      ! cluster->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,phi_calo)){
    ATH_MSG_ERROR ("Could not get Eta/Phi in calo frame");
    return false;
  }
  
  if ((runnumber>=296939 && runnumber<298967) || 
      (runnumber>=299144 && runnumber<300279)){
    
    if (   eta_calo > - 1.825 
	   && eta_calo < -1.5  
	   && phi_calo > -1.104921 
	   && phi_calo < -0.956748){
      return false;
    }
  } 
  
  if ((runnumber>=298967 && runnumber<299144 ) || 
      (runnumber>=300279 && runnumber< 305291)){
    
    if (eta_calo > - 1.825 
	&& eta_calo < -1.5  
	&& phi_calo > -1.104921 
	&& phi_calo < -0.956748){
      return false;
    }
    
    if (eta_calo> - 1.825 
	&& eta_calo < -1.5  
	&& phi_calo > 1.054922 
	&& phi_calo < 1.203097){
      return false;
    }
  }
  
  return true ;
}

bool AsgDeadHVCellRemovalTool::accept( const xAOD::Egamma& part ) const{
  return accept (&part);
}

