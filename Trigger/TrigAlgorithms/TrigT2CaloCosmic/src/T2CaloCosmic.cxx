/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloCosmic.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCosmic
//
// AUTHOR:   D.O. Damazio
//
// - Add variables for job option controlled region limits, set defaults
//   to most likely values.
// - Add function EtaPhiRange to return the maximum and minimum eta or phi
//   values to use when calculating energy sums over a region  - R. Soluk
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"


#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "TrigT2CaloCosmic/T2CaloCosmic.h"
//#include "TrigT2CaloCosmic/T2CaloCosmicMon.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"

class ISvcLocator;

T2CaloCosmic::T2CaloCosmic(const std::string & name, ISvcLocator* pSvcLocator)
  : T2CaloBase(name, pSvcLocator),  m_log(0)
{
    declareProperty("TrigEMClusterKey",m_trigEmClusterKey = "T2CaloTrigEMCluster");
    declareProperty("L1ForceEta",m_l1eta = -10.0);
    declareProperty("L1ForcePhi",m_l1phi = -10.0);
    declareProperty("EtaMin",m_etamin);
    declareProperty("EtaMax",m_etamax);
    declareProperty("PhiMin",m_phimin);
    declareProperty("PhiMax",m_phimax);
}

T2CaloCosmic::~T2CaloCosmic()
{
  delete m_log;
}


HLT::ErrorCode T2CaloCosmic::hltInitialize()
{
  if (!m_log) m_log = new MsgStream(messageService(), name());

  // Support for new monitoring
  declareMonitoredObject("Eta",
			m_monitoredCluster,&TrigEMCluster::eta);
  declareMonitoredObject("Phi",
			m_monitoredCluster,&TrigEMCluster::phi);
  declareMonitoredObject("Et",
			m_monitoredCluster,&TrigEMCluster::et);
  declareMonitoredObject("Had1Et",
			m_monitoredCluster,&TrigEMCluster::ehad1);
  declareMonitoredObject("weta2",
			m_monitoredCluster,&TrigEMCluster::weta2);
  // More complicated variables to be monitored
  declareMonitoredVariable("Rcore", m_rCore );
  declareMonitoredVariable("Eratio",m_eRatio);

  return HLT::OK;
}


HLT::ErrorCode T2CaloCosmic::hltExecute(const HLT::TriggerElement* inputTE,
					HLT::TriggerElement* outputTE)
{
  // Time total T2CaloCosmic execution time.
  if ( m_timersvc ) m_timer[0]->start();
  
  if (msgLvl(MSG::DEBUG))
    ATH_MSG_DEBUG("in execute()");

 
  //bool status = false;
  
  // Some debug output:
  if (msgLvl(MSG::DEBUG)) {
    msg (MSG::DEBUG) << "outputTE->getId(): " << outputTE->getId() << endreq;
    
    msg(MSG::DEBUG) << "inputTE->getId(): " << inputTE->getId() << endreq;
  }
    
  const TrigRoiDescriptor* roiDescriptor = 0;

  HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

  if ( hltStatus == HLT::OK ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << *roiDescriptor << endreq;	
  } else {
    msg(MSG::WARNING) << " Failed to find RoiDescriptor " << endreq;
    return hltStatus;
  }

  // Some debug output:

  msg(MSG::DEBUG)  << "Message to count events. LVL1 phi="
		   << roiDescriptor->phi()
		   << " & LVL1 eta="
		   << roiDescriptor->eta()
		   << endreq;

  // End LVL1 part
  double etamin, etamax, phimin, phimax;
  double _eta, _phi;

  int roiword = roiDescriptor->roiWord();

  //  if ( !m_trustRoiLimits ) { 
  if ( (m_l1eta<-9.9)&&(m_l1phi<-9.9)){
    _eta = roiDescriptor->eta();
    _phi = roiDescriptor->phi();
  }
  else { 
    _eta = m_l1eta;
    _phi = HLT::wrapPhi(m_l1phi);
  }
  
  etamin = std::max(-2.5, _eta - m_etaWidth);
  etamax = std::min( 2.5, _eta + m_etaWidth);
  phimin = HLT::wrapPhi( _phi - m_phiWidth);
  phimax = HLT::wrapPhi( _phi + m_phiWidth);
  
  TrigRoiDescriptor* newroi = new TrigRoiDescriptor( _eta, etamin, etamax, _phi, phimin, phimax );
  attachFeature( outputTE, newroi, "T2CaloCosmic" );
  roiDescriptor = newroi;
  
  //  }
  
  

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG)  << " etamin = "<< *roiDescriptor << endreq;
    //  (*m_log) << MSG::DEBUG  << " etamin = "<< etamin << endreq;
    //  (*m_log) << MSG::DEBUG  << " etamax = "<< etamax << endreq;
    //  (*m_log) << MSG::DEBUG  << " phimin = "<< phimin << endreq;
    //  (*m_log) << MSG::DEBUG  << " phimax = "<< phimax << endreq;
  }


  if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG)  << " Making TrigEMCluster "<< endreq;

  std::vector<xAOD::TrigEMCluster*> m_vec_clus;
  std::cout << "m_emAlgTools.size() = " << m_emAlgTools.size() << std::endl;
  // Ok, ignoring LVL1 and forcing a position
  int counter=0;
  
  ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
  if ( m_timersvc ) m_timer[1]->start();
  for (; it < m_emAlgTools.end(); it++)  {
  xAOD::TrigEMCluster* ptrigEmCluster = new xAOD::TrigEMCluster();
  // It is a good idea to clear the energies
  for(int i=0;i<MAXSIZE;i++)
		ptrigEmCluster->setEnergy((CaloSampling::CaloSample )i,0.);

  // Add RoI word to TrigEMCluster
  (*ptrigEmCluster).setRoIword(roiword);
  (*ptrigEmCluster).setEta1(10+counter);
  etamin=m_etamin[counter];
  etamax=m_etamax[counter];
  phimin=m_phimin[counter];
  phimax=m_phimax[counter];
  //    if ((*it)->execute(*ptrigEmCluster,etamin,etamax,phimin,phimax).isFailure() ) {
  if ((*it)->execute(*ptrigEmCluster,*roiDescriptor).isFailure() ) {
    ATH_MSG_WARNING("T2Calo AlgToolCosmic returned Failure");
    return HLT::TOOL_FAILURE;
  }
  // If you do not have any hot cell in top or bottom, stop algo
  if ( (counter==0 || counter==1) && (*ptrigEmCluster).energy()==0)
	return HLT::OK; 
  counter++;
  m_vec_clus.push_back(ptrigEmCluster);
  }
  // support to new monitoring
  m_rCore=0;
  m_eRatio=0;
/*
  if ( ptrigEmCluster->e277()!=0 )
	m_rCore =  ptrigEmCluster->e237()/ptrigEmCluster->e277();
  if ( ptrigEmCluster->emaxs1()+ptrigEmCluster->e2tsts1() !=0){
	m_eRatio  =  ptrigEmCluster->emaxs1()-ptrigEmCluster->e2tsts1();
	m_eRatio /=  ptrigEmCluster->emaxs1()+ptrigEmCluster->e2tsts1();
  }
*/
  if ( m_timersvc ) m_timer[1]->stop();
  for(size_t i=0;i<m_vec_clus.size();i++){
  xAOD::TrigEMCluster* ptrigEmCluster=m_vec_clus[i];
  
  // Print out Cluster produced
  msg(MSG::DEBUG)  << " Values of Cluster produced: "<< endreq;
  msg(MSG::DEBUG)  << " REGTEST: emEnergy = "<< (*ptrigEmCluster).energy() << endreq;
  msg(MSG::DEBUG)  << " REGTEST: hadEnergy = "<< (*ptrigEmCluster).ehad1() << endreq;
  msg(MSG::DEBUG)  << " REGTEST: rCore = " << ((*ptrigEmCluster).e237() )/ ((*ptrigEmCluster).e277()) << endreq;
  msg(MSG::DEBUG)  << " REGTEST: energyRatio = "
		   << (((*ptrigEmCluster).emaxs1()-(*ptrigEmCluster).e2tsts1())/
		       ((*ptrigEmCluster).emaxs1()+(*ptrigEmCluster).e2tsts1()))
		   << endreq;
  msg(MSG::DEBUG)  << " REGTEST: clusterWidth = " << (*ptrigEmCluster).weta2() << endreq;
  msg(MSG::DEBUG)  << " REGTEST: frac73 = " << (*ptrigEmCluster).fracs1() << endreq;
  msg(MSG::DEBUG)  << " REGTEST: eta = "<< (*ptrigEmCluster).eta() << endreq;
  msg(MSG::DEBUG)  << " REGTEST: phi = "<< (*ptrigEmCluster).phi() << endreq;
  msg(MSG::DEBUG)  << " REGTEST: roiWord = " << (*ptrigEmCluster).RoIword() << endreq;

  // Fill monitoring hists:
/*
  if (m_mon)
    if ( m_monitoring->fillT2CaloCosmicHists(ptrigEmCluster).isFailure() )
	(*m_log) << MSG::DEBUG << " Failled to Monitor" << endreq;
*/
  
  std::string key = "";

  hltStatus = recordAndAttachFeature(outputTE, ptrigEmCluster, key, "TrigT2CaloCosmic");
  if (hltStatus != HLT::OK){
    if(msgLvl(MSG::DEBUG)) ATH_MSG_ERROR("Write of TrigEMCluster into outputTE failed");
    return hltStatus;
  }
  }

  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them.
/*
  TrigEMCluster* ptrigEmCluster = m_vec_clus[0];
  TrigRoiDescriptor* newRoiDescriptor = 
    new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(),
			  ptrigEmCluster->eta(), ptrigEmCluster->phi());
  
  hltStatus = attachFeature(outputTE,newRoiDescriptor,"TrigT2CaloCosmic");
 
  if ( hltStatus != HLT::OK ) {
     (*m_log) << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	   << endreq;
     return hltStatus;
  }
*/

//#ifndef NDEBUG
 //   (*m_log) << MSG::DEBUG  << "Recorded an RoiDescriptor "
//	  << " phi " <<  newRoiDescriptor->phi0()
//	  << " eta " << newRoiDescriptor->eta0() << endreq;
//#endif

  // Some debug output:
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "We assume success, set TE with Id "
		    << outputTE->getId() << " active to signal positive result."
		    << endreq;
  }
  
  // Time total T2CaloCosmic execution time.
  if ( m_timersvc ) m_timer[0]->stop();
  
  return HLT::OK;
}


HLT::ErrorCode T2CaloCosmic::hltFinalize(){
  //  if ( msgLvl() <= MSG::INFO ) 
  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("in finalize()");
  return HLT::OK;
}
