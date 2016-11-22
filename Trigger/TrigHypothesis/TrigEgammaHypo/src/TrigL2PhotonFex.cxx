/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// -*- C++ -*-

/**************************************************************************
 ** 
 **   Original Author:       R.Goncalo    
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2PhotonHypo.cxx
 **                   
 **   Description: Fex algo for TrigPhotons
 ** 
 **   Modified by: R.Goncalo 
 **                V. Perez-Reale
 **                A.Hamilton
 **                V. Dao (new TrigPhoton constructor used)
 **
 **   Created:      Thu Jun 06 16:01:12 BST 2006
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2PhotonFex.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"


class ISvcLocator;


TrigL2PhotonFex::TrigL2PhotonFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_trigPhotonContainer (0)
{

}


TrigL2PhotonFex::~TrigL2PhotonFex()
{ 
  delete m_trigPhotonContainer;
}


HLT::ErrorCode TrigL2PhotonFex::hltInitialize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initializing"   << endmsg;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2PhotonFex::hltFinalize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Finalizing" << endmsg;
  }

  return HLT::OK;
}


HLT::ErrorCode TrigL2PhotonFex::hltExecute(const HLT::TriggerElement* inputTE,
					   HLT::TriggerElement* outputTE)
{

  HLT::ErrorCode status = HLT::OK;
  xAOD::TrigPhotonAuxContainer trigPhotonAuxContainer;

  // always create a TrigPhotonContainer, even if it will be empty
  if(!m_trigPhotonContainer) {
    m_trigPhotonContainer = new xAOD::TrigPhotonContainer();
    m_trigPhotonContainer->setStore(&trigPhotonAuxContainer);
  }
  else {
    m_trigPhotonContainer->clear();
  }
  
  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(inputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    ATH_MSG_WARNING("No RoI for this Trigger Element! ");
    return HLT::NAV_ERROR;
  }
  
  ATH_MSG_DEBUG("Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
          << "; RoI ID = "   << roiDescriptor->roiId()
          << ": Eta = "      << roiDescriptor->eta()
          << ", Phi = "      << roiDescriptor->phi());

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range (probably not needed anymore)   
  if ( fabs(phiRef) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range


  // retrieve TrigEMCluster ElementLink from the TriggerElement
  ElementLink< xAOD::TrigEMClusterContainer > elink_cluster;
  status = getFeatureLink< xAOD::TrigEMClusterContainer, xAOD::TrigEMCluster >( inputTE, elink_cluster );

  if( status == HLT::OK && elink_cluster.isValid() ){
#ifdef DONTDO
    if( msgLvl() <= MSG::DEBUG ){ 
      (*elink_cluster)->print( msg() ); 
    }
#endif
  }
  else {
    if ( msgLvl() <= MSG::DEBUG){ 
      msg() << MSG::DEBUG << "Failed to get TrigEMCluster" << endmsg; 
    }
    return HLT::MISSING_FEATURE;
  }

  // retrieve the TrigEMCluster from the ElementLink
  const xAOD::TrigEMCluster* pClus = (*elink_cluster);

  if(pClus == 0){
    if ( msgLvl() <= MSG::WARNING ){
      msg() << MSG::WARNING << "Failed to retieve TrigEMCluster from ElementLink" << endmsg;
    }
    return HLT::MISSING_FEATURE;
  }
  
  float dEta =  pClus->eta() - etaRef;
  // Deal with angle diferences greater than Pi
  float dPhi =  fabs(pClus->phi() - phiRef);
  dPhi = (dPhi < M_PI ? dPhi : 2*M_PI - dPhi );
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG  << "TrigPhoton will be built with: dEta=" << dEta
	  << "  and dPhi= " << dPhi << endmsg; 
  } 

  // create TrigPhoton from TrigEMCluster
  xAOD::TrigPhoton* p_trigPhoton = new xAOD::TrigPhoton();
  // push TrigPhoton into TrigPhotonContainer
  m_trigPhotonContainer->push_back(p_trigPhoton);
	p_trigPhoton->init( roiDescriptor->roiId(), dPhi, dEta, elink_cluster);
//					    elink_cluster.getStorableObjectPointer(), 
//					    elink_cluster.index() );
  



  // REGTEST output
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "REGTEST: TrigPhotonContainer has " << m_trigPhotonContainer->size()
	  << " element" << endmsg;
    if (!m_trigPhotonContainer->empty()) {
      xAOD::TrigPhoton* p_tp = m_trigPhotonContainer->front();
      msg() << MSG::DEBUG  << "REGTEST: TrigPhoton: RoI=" << p_tp->roiWord()
//	    << "; valid="  << p_tp->isValid() 
	    << "; eta="    << p_tp->eta() 
	    << "; phi="    << p_tp->phi() 
	    << "; Et="     << p_tp->emCluster()->et() 
	    << "; Had Et=" << p_tp->etHad() 
	    << "; EnergyRatio=" << p_tp->eratio()
	    << "; rCore=" << p_tp->rcore()
	    << endmsg;
    }
  }  
  

  // attach TrigPhotonContainer to the output TriggerElement and 
  // record it in StoreGate with key "L2PhotonFex" 
  status = attachFeature( outputTE, m_trigPhotonContainer, "L2PhotonFex");
  m_trigPhotonContainer = 0;

  // this TrigPhotonContainer will contain 1 TrigPhoton, this is historical and 
  // should be changed to attach just the TrigPhoton, but requires an EDM change
  // so has not been done yet. 
  
  if( status == HLT::OK ){
    if( msgLvl() <= MSG::DEBUG ){ 
      msg() << MSG::DEBUG << "Attached TrigPhotonContainer to outputTE with ID: " << outputTE->getId() << endmsg;    
    }
  }
  else {
    if ( msgLvl() <= MSG::ERROR ){
      msg() << MSG::ERROR << "Failed to attach TrigPhotonContainer!" << endmsg;    
    }
    return status;
  }
  
  return HLT::OK;
}



