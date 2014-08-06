/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************

 NAME:     T2CaloSwCluster.cxx
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma

 AUTHOR:   A. Phan
 
 PURPOSE:  Based very closely on T2CaloEgamma.cxx . Given an inputTE
           containing a TrigEmCluster, calls a set of IAlgToolCalos
           to perform Feature EXtraction.
 
 *****************************************************************************/

#include "TrigT2CaloEgamma/T2CaloSwCluster.h"


class ISvcLocator;


T2CaloSwCluster::T2CaloSwCluster(const std::string &name, ISvcLocator* pSvcLocator) :
  T2CaloBase(name, pSvcLocator),  m_log(0), m_calibs(this),
  m_egammaqweta2c("egammaqweta2c/egammaqweta2c",this) {
  
  declareProperty("TrigEMClusterKey",m_trigEmClusterKey = "T2CaloTrigEMCluster");
  declareProperty("L1ForceEta",m_l1eta = -10.0);
  declareProperty("L1ForcePhi",m_l1phi = -10.0);
  declareProperty("CalibList",m_calibs,"list of calib tools");
  declareProperty("egammaqweta2c",m_egammaqweta2c,"Egamma Weta2 correction");
}


T2CaloSwCluster::~T2CaloSwCluster() {
  delete m_log;
}


HLT::ErrorCode T2CaloSwCluster::hltInitialize() {
  if (!m_log) m_log = new MsgStream(messageService(), name());

  if ( m_calibs.retrieve().isFailure() )
  	(*m_log) << MSG::DEBUG << "No Calibration Available" << endreq;

  declareMonitoredVariable("Eta", m_MonEta);
  declareMonitoredVariable("Phi",m_MonPhi);
  declareMonitoredObject("Et",m_monitoredCluster,&xAOD::TrigEMCluster::et);
  declareMonitoredObject("Had1Et",m_monitoredCluster,&xAOD::TrigEMCluster::ehad1);
  declareMonitoredObject("weta2",m_monitoredCluster,&xAOD::TrigEMCluster::weta2);
  declareMonitoredObject("frac73",m_monitoredCluster,&xAOD::TrigEMCluster::fracs1); 
  declareMonitoredVariable("Rcore", m_rCore );
  declareMonitoredVariable("Eratio",m_eRatio);
  declareMonitoredVariable("ConversionErrors", m_conversionError);
  declareMonitoredVariable("AlgorithmErrors", m_algorithmError);

  if ( (m_egammaqweta2c.retrieve()).isFailure() ){
                        (*m_log) << MSG::FATAL
                        << "Could not find egammaqweta2c" << endreq;
                        return HLT::TOOL_FAILURE;
                }

  return HLT::OK;
}


HLT::ErrorCode T2CaloSwCluster::hltFinalize() {
  return HLT::OK;
}


HLT::ErrorCode T2CaloSwCluster::hltExecute
  (const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {
  
  // Time total execution time.
  if ( m_timersvc ) m_timer[0]->start();
  m_conversionError=0;
  m_algorithmError=0;

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::INFO << "in execute()" << endreq;
 
    (*m_log) << MSG::DEBUG
	    << "outputTE->getId(): "
	    << outputTE->getId()
	    << endreq;

    (*m_log) << MSG::DEBUG
	    << "inputTE->getId(): "
	    << inputTE->getId()
	    << endreq;
  }
#endif

  const TrigRoiDescriptor* roiDescriptor = 0;

  HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

  if ( hltStatus == HLT::OK ) {
    if ( (*m_log).level() <= MSG::DEBUG ) 
        (*m_log) << MSG::DEBUG  << *roiDescriptor << endreq;
  } else {
    (*m_log) <<  MSG::WARNING << " Failed to find RoiDescriptor " << endreq;
    return hltStatus;
  }

  float etaL1, phiL1;
  double etamin, etamax, phimin, phimax;
  if ( (m_l1eta < -9.9) && (m_l1phi < -9.9)){
    etamin = std::max(-2.5,roiDescriptor->eta() - m_etaWidth);
    etamax = std::min( 2.5,roiDescriptor->eta() + m_etaWidth);
    phimin = roiDescriptor->phi() - m_phiWidth;
    phimax = roiDescriptor->phi() + m_phiWidth;
    etaL1 = roiDescriptor->eta();
    phiL1 = roiDescriptor->phi();
  } else {
    etamin = std::max(-2.5,m_l1eta - m_etaWidth);
    etamax = std::min( 2.5,m_l1eta + m_etaWidth);
    phimin = m_l1phi - m_phiWidth;
    phimax = m_l1phi + m_phiWidth;
    etaL1 =  m_l1eta;
    phiL1 =  m_l1phi;
  }
  while (phimin < 0) {
    phimin += 2. * M_PI;
  }
  while (phimax > 2 * M_PI) {
    phimax -= 2. * M_PI;
  }
  if (phiL1 + 0.001 >= M_PI) phiL1 -= 0.001;
  if (phiL1 - 0.001 <= -M_PI) phiL1 += 0.001;

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG  << " etamin = "<< etamin << endreq;
    (*m_log) << MSG::DEBUG  << " etamax = "<< etamax << endreq;
    (*m_log) << MSG::DEBUG  << " phimin = "<< phimin << endreq;
    (*m_log) << MSG::DEBUG  << " phimax = "<< phimax << endreq;
  }
#endif

  // retrieve TrigEMCluster from the TE
  std::vector< const xAOD::TrigEMCluster* > vectorOfClusters;  
  hltStatus = getFeatures( outputTE, vectorOfClusters );

  if ( hltStatus != HLT::OK ) {
    if ( (*m_log).level()  <= MSG::WARNING)
      (*m_log) << MSG::WARNING << "Failed to get TrigEMClusters" << endreq;
    return HLT::OK;
  }

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) 
    (*m_log) << MSG::DEBUG << "Found vector with " << vectorOfClusters.size() << " TrigEMClusters" << endreq;
#endif

  // Should be only 1 cluster, normally!
  if (vectorOfClusters.size() != 1) {
    if ( (*m_log).level() <= MSG::DEBUG )
      (*m_log) << MSG::WARNING << "Size of vector of TrigEMClusters is not 1!" << endreq;
    return HLT::OK;
  }

  // Get cluster
  const xAOD::TrigEMCluster* inputCluster = vectorOfClusters.front();
  xAOD::TrigEMCluster* ptrigEmCluster = new xAOD::TrigEMCluster();
  *ptrigEmCluster = *inputCluster;
  const CaloDetDescrElement * caloDDE = 0;

  if ( !ptrigEmCluster ) {
    (*m_log) << MSG::WARNING << "No TrigEMCluster in feature" << endreq;
    return HLT::OK;
  }
  
  ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
  if ( m_timersvc ) m_timer[1]->start();
  uint32_t error = 0;
  for (; it < m_emAlgTools.end(); it++)  {
    (*it)->setCaloDetDescrElement(caloDDE);
    if ((*it)->execute(*ptrigEmCluster,etamin,etamax,phimin,phimax).isFailure() ) {
      (*m_log) << MSG::WARNING << "T2Calo AlgToolEgamma returned Failure" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
    }
    uint32_t in_error = (*it)->report_error();
    if ( 0x0FFFFFFF & in_error ) m_conversionError++;
    if ( 0xF0000000 & in_error ) m_algorithmError++;
    if ( (*it)->getCaloDetDescrElement() != 0 )
        caloDDE = (*it)->getCaloDetDescrElement();

    error|=in_error;
  }

  // Monitoring
  m_rCore=0;
  m_eRatio=0;
  m_MonEta=ptrigEmCluster->eta();
  m_MonPhi=ptrigEmCluster->phi();
  if ( ptrigEmCluster->e277()!=0 )
	  m_rCore =  ptrigEmCluster->e237()/ptrigEmCluster->e277();
  if ( ptrigEmCluster->emaxs1()+ptrigEmCluster->e2tsts1() !=0){
	  m_eRatio  =  ptrigEmCluster->emaxs1()-ptrigEmCluster->e2tsts1();
	  m_eRatio /=  ptrigEmCluster->emaxs1()+ptrigEmCluster->e2tsts1();
  }
  ptrigEmCluster->setClusterQuality(((uint32_t)ptrigEmCluster->clusterQuality())|error);
  
  if ( m_timersvc ) m_timer[1]->stop();

  // Check Cluster phi - if not within range, clustering failed
  if ( (error & 0xC0000000) || 
    ptrigEmCluster->phi() < -M_PI || 
    ptrigEmCluster->phi() > +M_PI ||
    fabsf ( ptrigEmCluster->eta() ) > 10.0 ) {
    ptrigEmCluster->setEta(etaL1);
    ptrigEmCluster->setPhi(phiL1);
    ptrigEmCluster->setEnergy(0.0);
    ptrigEmCluster->setEt(0.0);    
  }

  // Run Calibrations
  if ( caloDDE )
  for( ToolHandleArray<IEgammaCalibration>::const_iterator
		ical=m_calibs.begin();
		ical != m_calibs.end(); ++ical )
  	(*ical)->makeCorrection(ptrigEmCluster,caloDDE);

  // Final correction to weta only
  if ( caloDDE != 0 )
  ptrigEmCluster->setWeta2(
    m_egammaqweta2c->Correct(ptrigEmCluster->eta()
        ,caloDDE->eta(),ptrigEmCluster->weta2()) );
  
#ifndef NDEBUG
  // Print out Cluster produced
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG  << " Values of Cluster produced: "<< endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: emEnergy = "<< (*ptrigEmCluster).energy()
	           << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: hadEnergy = "<< (*ptrigEmCluster).ehad1()
	           << endreq;
    if ( ptrigEmCluster->e277()!=0. )
      (*m_log) << MSG::DEBUG  << " REGTEST: rCore = "
	             << ((*ptrigEmCluster).e237() )/ ((*ptrigEmCluster).e277()) << endreq;
    else 
      (*m_log) << MSG::DEBUG  << " REGTEST: e277 equals to 0" << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: energyRatio = "
	           << (((*ptrigEmCluster).emaxs1()-(*ptrigEmCluster).e2tsts1())/
	                ((*ptrigEmCluster).emaxs1()+(*ptrigEmCluster).e2tsts1()))
	           << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: clusterWidth = "
	           << (*ptrigEmCluster).weta2() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: frac73 = "
	      << (*ptrigEmCluster).fracs1() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: eta = "<< (*ptrigEmCluster).eta() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: phi = "<< (*ptrigEmCluster).phi() << endreq;
    (*m_log) << MSG::DEBUG  << " REGTEST: roiWord = "
	           << (*ptrigEmCluster).RoIword() << endreq;
  }
#endif

  std::string key = "";

  hltStatus = recordAndAttachFeature(outputTE, ptrigEmCluster, key, "T2CaloSwCluster");
  if (hltStatus != HLT::OK){
    if ( (*m_log).level() <= MSG::DEBUG ) 
      (*m_log) << MSG::ERROR << "Write of TrigEMCluster into outputTE failed"
	    << endreq;
    return hltStatus;
  }

  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them. (will it??)
  // how big should this RoI be??? Should also include the z position
  // and the z size!!

  TrigRoiDescriptor* newRoiDescriptor = 
    new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(),
			  ptrigEmCluster->eta(), ptrigEmCluster->eta()-m_etaWidth, ptrigEmCluster->eta()+m_etaWidth,
			  ptrigEmCluster->phi(), ptrigEmCluster->phi()-m_phiWidth, ptrigEmCluster->eta()+m_phiWidth );
  
  hltStatus = attachFeature(outputTE,newRoiDescriptor,"TrigT2CaloEgamma");
 

  if ( hltStatus != HLT::OK ) {
     (*m_log) << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	   << endreq;
     return hltStatus;
  }

#ifndef NDEBUG
  if ( (*m_log).level() <= MSG::DEBUG ) {
    (*m_log) << MSG::DEBUG  << "Recorded an RoiDescriptor "
	  << newRoiDescriptor->phi() << endreq;
    (*m_log) << MSG::DEBUG << "We assume success, set TE with Id "
	  << outputTE->getId() << " active to signal positive result."
	  << endreq;
  }
#endif
  
  if ( error )
    return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::USERDEF_2);

  // Time total T2CaloEgamma execution time.
  if ( m_timersvc ) m_timer[0]->stop();

  return HLT::OK;
}
