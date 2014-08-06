/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloEgammaSamp1.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
//
// AUTHOR:   M.P. Casado
//
// ********************************************************************

//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"

//#include "TrigSteeringEvent/TriggerElement.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include "TrigT2CaloEgamma/T2CaloEgammaSamp1.h"
//#include "TrigT2CaloCommon/T2CaloBase.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"

#include "TrigT2CaloCommon/phiutils.h"

class ISvcLocator;

T2CaloEgammaSamp1::T2CaloEgammaSamp1(const std::string & name, ISvcLocator* pSvcLocator):
  T2CaloNewBase(name, pSvcLocator){
}

T2CaloEgammaSamp1::~T2CaloEgammaSamp1(){
}



HLT::ErrorCode T2CaloEgammaSamp1::hltExecute(const HLT::TriggerElement* inputTE,
				   HLT::TriggerElement* outputTE)
{

  // Time total T2CaloEgammaSamp1 execution time.
  if (m_timersvc) m_timer[0]->start();

  // Retrieve store.
  // Very important!
  //  m_storeGate = getStore(); // not needed anymore in!



#ifndef NDEBUG
  msg() << MSG::INFO << "in execute()" << endreq;
#endif
  //  bool status = false;

  // Some debug output:
#ifndef NDEBUG
  msg() << MSG::DEBUG
	<< "outputTE->label() not available "
	<< endreq;
#endif


  // Some debug output:
#ifndef NDEBUG
  msg() << MSG::DEBUG
	<< "inputTE->getId(): "
	<< inputTE->getId()
	<< endreq;
#endif

  const TrigRoiDescriptor* roiDescriptor = 0;

  HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

  if ( hltStatus == HLT::OK ) {
#ifndef NDEBUG
    msg() << MSG::DEBUG  << *roiDescriptor << endreq;
#endif
  } else {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endreq;
    return hltStatus;
  }

  // Some debug output:

  msg() << MSG::DEBUG  << "Message to count events. LVL1 phi=" << *roiDescriptor
	<< endreq;


  //Commenting this out to be compatible with RecExTB use of FakeLvl1Conversion
  // SRA 6.Aug.2004
  // Get vector of of pointers to all RecEmTauRoIs with label uses:
  //  std::vector<LVL1::RecEmTauRoI*> vectorOfRecEmTauRoI;
  //status = getVectorOfObjects(vectorOfRecEmTauRoI, uses, inputTE);
  //if(!status) return false;
  //
  // Check that there is only one RecEmTauROI.
  // We only expect ONE input RoI.
  //  if (vectorOfRecEmTauRoI.size() != 1){
  //#ifndef NDEBUG
  //    msg() << MSG::ERROR
  //        << "Size of vector is not 1"
  //        << endreq;
  //#endif
  //    return StatusCode::FAILURE;
  //  }
  //
  // Get first (and only) RoI:
  //  LVL1::RecEmTauRoI* pEmTauRoI = vectorOfRecEmTauRoI.front();
  //  if(!pEmTauRoI){
  //#ifndef NDEBUG
  //    msg() << MSG::ERROR
  //        << "Retrieval of RoI from vector failed"
  //        << endreq;
  //#endif
  //    return false;
  //  }

  /// This should all be changed - an RoI should be fetched at the beginning, 
  /// then if the paremeters need to be changed, a new RoI should be created, 
  /// and then that RoI propogated through the system, not all these repeated 
  /// calculations of limts everywhere   
  double etamin = std::max(-2.5,roiDescriptor->eta() - m_etaWidth);
  double etamax = std::min( 2.5,roiDescriptor->eta() + m_etaWidth);
  double phimin = HLT::wrap_phi(roiDescriptor->phi() - m_phiWidth);
  double phimax = HLT::wrap_phi(roiDescriptor->phi() + m_phiWidth);
  while (phimin < -M_PI) phimin += 2. * M_PI;
  while (phimax >  M_PI) phimax -= 2. * M_PI;

  TrigRoiDescriptor newroi( roiDescriptor->eta(), etamin, etamax, 
			    roiDescriptor->phi(), phimin, phimax ); 

#ifndef NDEBUG
  msg() << MSG::DEBUG << " etamin = "<< etamin << endreq;
  msg() << MSG::DEBUG << " etamax = "<< etamax << endreq;
  msg() << MSG::DEBUG << " phimin = "<< phimin << endreq;
  msg() << MSG::DEBUG << " phimax = "<< phimax << endreq;
#endif

#ifndef NDEBUG
  msg() << MSG::DEBUG << " Making TrigEMCluster "<< endreq;
#endif

  // Get vector of of pointers to all TrigEMClusters with label uses:
  // Get vector of of pointers to all EMShowerMinimals with label uses:
  std::vector<const xAOD::TrigEMCluster*> vectorOfClusters;

  HLT::ErrorCode status = getFeatures(inputTE, vectorOfClusters);

  if( status != HLT::OK ) return HLT::NAV_ERROR;

  // Check that there is only one RecEmTauROI.
  // We only expect ONE input RoI.
  if (vectorOfClusters.size() != 1){
#ifndef NDEBUG
    msg() << MSG::ERROR
        << "Size of vector is not 1"
        << endreq;
#endif
    return HLT::NAV_ERROR;
  }

  // Get first (and only) RoI:
  const xAOD::TrigEMCluster* ptrigEMCluster_const = vectorOfClusters.front();
  if(!ptrigEMCluster_const){
#ifndef NDEBUG
    msg() << MSG::ERROR << "Retrieval of RoI from vector failed"
	  << endreq;
#endif
    return HLT::NAV_ERROR;
  }

  xAOD::TrigEMCluster* ptrigEMCluster = const_cast<xAOD::TrigEMCluster*>(ptrigEMCluster_const);


  std::vector<IAlgToolCalo*>::iterator it = m_emAlgTools.begin();
  for (; it < m_emAlgTools.end(); it++)  {
    if ((*it)->execute(*ptrigEMCluster,newroi).isFailure() ) {
      msg() << MSG::WARNING << "T2CaloSamp1En AlgTool returned Failure" << endreq;
      return HLT::ERROR;
    }
  }

  // Print out Cluster produced
  msg() << MSG::DEBUG << " Values of Cluster produced: "<< endreq;
  msg() << MSG::DEBUG << " emEnergy = "<< (*ptrigEMCluster).energy() << endreq;
  msg() << MSG::DEBUG << " hadEnergy = "<< (*ptrigEMCluster).ehad1() << endreq;
  msg() << MSG::DEBUG << " rCore = "<< ((*ptrigEMCluster).e237() )/ ((*ptrigEMCluster).e277()) << endreq;
  msg() << MSG::DEBUG << " energyRatio = "<< (((*ptrigEMCluster).emaxs1()-(*ptrigEMCluster).e2tsts1())/
                                           ((*ptrigEMCluster).emaxs1()+(*ptrigEMCluster).e2tsts1())) << endreq;
  msg() << MSG::DEBUG << " clusterWidth = "<< (*ptrigEMCluster).weta2() << endreq;
  msg() << MSG::DEBUG << " frac73 = "<< (*ptrigEMCluster).fracs1() << endreq;
  msg() << MSG::DEBUG << " eta = "<< (*ptrigEMCluster).eta() << endreq;
  msg() << MSG::DEBUG << " phi = "<< (*ptrigEMCluster).phi() << endreq;


  // Create a new RoiDescriptor with updated eta and phi
  TrigRoiDescriptor* newRoiDescriptor =
    new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(), 
			  ptrigEMCluster->eta(), ptrigEMCluster->eta()-m_etaWidth, ptrigEMCluster->eta()+m_etaWidth,
			  ptrigEMCluster->phi(), HLT::wrap_phi(ptrigEMCluster->phi()-m_phiWidth), HLT::wrap_phi(ptrigEMCluster->eta()+m_phiWidth) );  
  
  //  msg() << MSG::WARNING << "T2CaloSamp1En AlgTool returned Failure" << endreq;


  hltStatus = attachFeature(outputTE,newRoiDescriptor,"TrigT2CaloEgamma");

  if ( hltStatus != HLT::OK ) {
    msg() << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	  << endreq;
     return hltStatus;
  }

#ifndef NDEBUG
  msg() << MSG::DEBUG  << "Recorded an RoiDescriptor " << *newRoiDescriptor << endreq;
#endif

  // Some debug output:
#ifndef NDEBUG
    msg() << MSG::DEBUG << "We assume success, set TE with Id "
	  << outputTE->getId() << " active to signal positive result."
	  << endreq;
#endif


  // Time total T2CaloEgammaSamp1 execution time.
  if (m_timersvc) m_timer[0]->stop();

  return HLT::OK;
}
