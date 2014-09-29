/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowTauLCCalibTool.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include "CaloRec/CaloClusterProcessor.h"

eflowTauLCCalibTool::eflowTauLCCalibTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_clusterMomentsMakerPointer("CaloClusterMomentsMaker",this),
  m_clusterLocalCalibTool("CaloClusterLocalCalib",this),
  m_clusterLocalCalibOOCCTool("CaloClusterLocalCalib",this),
  m_clusterLocalCalibOOCCPi0Tool("CaloClusterLocalCalib",this),
  m_clusterLocalCalibDMTool("CaloClusterLocalCalib",this)
{
  declareProperty("CaloClusterMomentsMaker",m_clusterMomentsMakerPointer,"Tool to Calculate Cluster Moments");
  declareProperty("CaloClusterLocalCalib",m_clusterLocalCalibTool,"Cluster Calibration for EFlowRec");
  declareProperty("CaloClusterLocalCalibOOCC",m_clusterLocalCalibOOCCTool,"Cluster CalibrationOOCC for EFlowRec");
  declareProperty("CaloClusterLocalCalibOOCCPi0",m_clusterLocalCalibOOCCPi0Tool,"Cluster CalibrationOOCCPi0 for EFlowRec");
  declareProperty("CaloClusterLocalCalibDM",m_clusterLocalCalibDMTool,"Cluster CalibrationDM for EFlowRec");
}

StatusCode eflowTauLCCalibTool::initialize(){

  //get cluster moments maker

  StatusCode sc = m_clusterMomentsMakerPointer.retrieve();
    
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterMomentsMaker Tool "<< endreq;
    return StatusCode::SUCCESS;
  }

  //get local had calib tool
    
  sc = m_clusterLocalCalibTool.retrieve();
  
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibTool " << endreq;
    return StatusCode::SUCCESS;
  }

  //get local had calib tool for out of cluster corrections
  
  sc = m_clusterLocalCalibOOCCTool.retrieve();
  
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibTool_OOCC " << endreq;
    return StatusCode::SUCCESS;
  }
  
  sc = m_clusterLocalCalibOOCCPi0Tool.retrieve();
  
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibTool_OOCCPi0 " << endreq;
    return StatusCode::SUCCESS;
  }
  
  //get local had tool for DM corrections
  
  sc = m_clusterLocalCalibDMTool.retrieve();
  
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find CaloClusterLocalCalibDMTool " << endreq;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;

}

StatusCode eflowTauLCCalibTool::execute(const eflowTauObject& eflowTauObject){

  int debug = 0;

  xAOD::PFOContainer* thePFOContainer = eflowTauObject.getPFOContainer();

  xAOD::PFOContainer::iterator firstPFO = thePFOContainer->begin();
  xAOD::PFOContainer::iterator lastPFO = thePFOContainer->end();

  xAOD::CaloClusterContainer* clusterContainer = new xAOD::CaloClusterContainer(SG::VIEW_ELEMENTS);

  //First of all we need to put all the CaloCluster from all neutral EFO into a container to be used to calculate the Cluster Moments

  for (; firstPFO != lastPFO; ++firstPFO){

    if (0 == (*firstPFO)->charge()){
      const xAOD::CaloCluster* theCluster = (*firstPFO)->cluster(0);
      if (theCluster) {
	clusterContainer->push_back(const_cast<xAOD::CaloCluster*>(theCluster));//container will only accept non-const pointer
	if (1 == debug) std::cout << "Adding cluster with E, eta and phi to moments maker " << theCluster->e() << ", " << theCluster->eta() << " and " << theCluster->phi() << std::endl;
      }
      else msg(MSG::WARNING) << "Invalid pointer to neutral EFO CaloCluster " << endreq;
    }//neutral EFO

  }//eflow loop
  //reset iterator for future use
  firstPFO = thePFOContainer->begin();

  StatusCode clusMomentSC;
    
  
  clusMomentSC = m_clusterMomentsMakerPointer->execute(clusterContainer);
  
  if (clusMomentSC.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not execute ClusterMomentsMaker " << endreq;
  }
  

  for (; firstPFO != lastPFO; ++firstPFO){

    if (0 == (*firstPFO)->charge()){

      const xAOD::CaloCluster* theConstCluster = (*firstPFO)->cluster(0);

      //We want to calibrate this cluster so we can cast away the constness so the cluster can be modified
      xAOD::CaloCluster* theCluster = const_cast<xAOD::CaloCluster*>(theConstCluster);

      if (theCluster) {

	StatusCode clusToolSC;

	if (1 == debug) std::cout << " about to calibrate cluster with e, eta and phi :  " << theCluster->e() << ", " << theCluster->eta() << " and " << theCluster->phi() << std::endl;

	/*
	CaloCluster::moment_iterator myMoment=theCluster->beginMoment(true);
	CaloCluster::moment_iterator myEnd = theCluster->endMoment(true);
      
	for (; myMoment != myEnd; myMoment++) {
	  if (1 == debug) std::cout << "Moment type " << theCluster->getMomentType(myMoment) << " has value " << myMoment.getMoment().getValue() << std::endl;
	}      
	if (1 == debug) std::cout << " A cluster with e, eta and phi :  " << theCluster->e() << ", " << theCluster->eta() << " and " << theCluster->phi() << std::endl;
	CaloRecoStatus theStatusA = theCluster->getRecoStatus();
	if (1 == debug) std::cout << " Reco status is " << theStatusA.getStatusWord() << std::endl;
	*/

	CaloClusterCollectionProcessor* myCollectionProcessor_calibTool = &(*m_clusterLocalCalibTool);
	CaloClusterProcessor* myCalibProcessor_calibTool = dynamic_cast<CaloClusterProcessor*>(myCollectionProcessor_calibTool);

	clusToolSC = myCalibProcessor_calibTool->execute(theCluster);
        if (clusToolSC.isFailure()) {
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not execute ClusterLocalCalibTool " << endreq;
        }

	CaloClusterCollectionProcessor* myCollectionProcessor_calibOOCCTool = &(*m_clusterLocalCalibOOCCTool);
	CaloClusterProcessor* myCalibProcessor_calibOOCCTool = dynamic_cast<CaloClusterProcessor*>(myCollectionProcessor_calibOOCCTool);

	clusToolSC = myCalibProcessor_calibOOCCTool->execute(theCluster);
	if (clusToolSC.isFailure()) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not execute ClusterLocalCalibOOCCTool " << endreq;
	}

	if (1 == debug) std::cout << "B about to calibrate cluster with e, eta and phi :  " << theCluster->e() << ", " << theCluster->eta() << " and " << theCluster->phi() << std::endl;

	CaloClusterCollectionProcessor* myCollectionProcessor_calibOOCCPi0Tool = &(*m_clusterLocalCalibOOCCPi0Tool);
	CaloClusterProcessor* myCalibProcessor_calibOOCCPi0Tool = dynamic_cast<CaloClusterProcessor*>(myCollectionProcessor_calibOOCCPi0Tool);

	clusToolSC = myCalibProcessor_calibOOCCPi0Tool->execute(theCluster);
	if (clusToolSC.isFailure()) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not execute ClusterLocalCalibOOCCPi0Tool " << endreq;
	}
	
	if (1 == debug) std::cout << " C about to calibrate cluster with e, eta and phi :  " << theCluster->e() << ", " << theCluster->eta() << " and " << theCluster->phi() << std::endl;

	CaloClusterCollectionProcessor* myCollectionProcessor_calibDMTool = &(*m_clusterLocalCalibDMTool);
	CaloClusterProcessor* myCalibProcessor_calibDMTool = dynamic_cast<CaloClusterProcessor*>(myCollectionProcessor_calibDMTool);

	clusToolSC = myCalibProcessor_calibDMTool->execute(theCluster);
	if (clusToolSC.isFailure()) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not execute ClusterLocalCalibDMTool " << endreq;
	}

	if (1 == debug) std::cout << " D about to calibrate cluster with e, eta and phi :  " << theCluster->e() << ", " << theCluster->eta() << " and " << theCluster->phi() << std::endl;

	xAOD::PFO* neutralPFO = *firstPFO;
	
	//now update EFO kinematics with calibrated cluster kinematics
	neutralPFO->setP4(theCluster->pt(),theCluster->eta(),theCluster->phi(),theCluster->m());

	if (1 == debug) std::cout << "now neutral EFO has E, eta and phi of " << neutralPFO->e() << ", " << neutralPFO->eta() << " and " << neutralPFO->phi() << std::endl;

      }
      else msg(MSG::WARNING) << "Invalid pointer to neutral PFO CaloCluster " << endreq;
    }//neutral EFO
  
  }//EFO loop
  
  delete  clusterContainer;

  return StatusCode::SUCCESS;
}

StatusCode eflowTauLCCalibTool::finalize(){
   return StatusCode::SUCCESS;
}
