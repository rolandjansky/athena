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

#include "TrigEgammaHypo/TrigL2PhotonFexMT.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"


class ISvcLocator;


TrigL2PhotonFexMT::TrigL2PhotonFexMT(const std::string & name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  m_trigPhotonContainer =0;
}


TrigL2PhotonFexMT::~TrigL2PhotonFexMT()
{}


StatusCode TrigL2PhotonFexMT::initialize()
{

ATH_CHECK( m_roiCollectionKey.initialize() );
ATH_CHECK( m_TrigEMClusterContainerKey.initialize() );
  ATH_MSG_DEBUG("Initialization:");
    return  StatusCode::SUCCESS;
}


StatusCode TrigL2PhotonFexMT::finalize()
{
    ATH_MSG_INFO("in finalize()");
     return StatusCode::SUCCESS;
}


StatusCode TrigL2PhotonFexMT::execute() 
{
 using namespace xAOD;
 auto ctx = getContext();

  xAOD::TrigPhotonAuxContainer trigPhotonAuxContainer;

  // always create a TrigPhotonContainer, even if it will be empty
  if(!m_trigPhotonContainer) {
    m_trigPhotonContainer = new xAOD::TrigPhotonContainer();
    m_trigPhotonContainer->setStore(&trigPhotonAuxContainer);
  }
  else {
    m_trigPhotonContainer->clear();
  }
  

 auto roiCollection = SG::makeHandle(m_roiCollectionKey, ctx);
  if (roiCollection->size()==0) {
    ATH_MSG_DEBUG(" RoI collection size = 0");
    return StatusCode::SUCCESS;
  }

  const TrigRoiDescriptor* roiDescriptor = *(roiCollection->begin());

  ATH_MSG_DEBUG(" RoI ID = "   << (roiDescriptor)->roiId()
		<< ": Eta = "      << (roiDescriptor)->eta()
		<< ", Phi = "      << (roiDescriptor)->phi());
  

  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range (probably not needed anymore)   
  if ( fabs(phiRef) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range


  // retrieve TrigEMCluster ElementLink from the TriggerElement

  auto clusContainer = SG::makeHandle (m_TrigEMClusterContainerKey, ctx);
  
  
  //JTB Should only be 1 cluster in each RoI 

  const xAOD::TrigEMCluster *elink_cluster=(*clusContainer->begin());
  ElementLink<xAOD::TrigEMClusterContainer> EClus=ElementLink<xAOD::TrigEMClusterContainer> (*clusContainer,0);


  // retrieve the TrigEMCluster from the ElementLink
    const xAOD::TrigEMCluster* pClus = elink_cluster;

  if(pClus == 0){
      ATH_MSG_ERROR("Failed to retieve TrigEMCluster from the ElementLink");
      return StatusCode::SUCCESS; //HLT::MISSING_FEATURE;
  }
  
  float dEta =  pClus->eta() - etaRef;
  // Deal with angle diferences greater than Pi
  float dPhi =  fabs(pClus->phi() - phiRef);
  dPhi = (dPhi < M_PI ? dPhi : 2*M_PI - dPhi );
   ATH_MSG_DEBUG("TrigPhoton will be built with: dEta=" << dEta
		 << "  and dPhi= " << dPhi);



  // create TrigPhoton from TrigEMCluster
  xAOD::TrigPhoton* p_trigPhoton = new xAOD::TrigPhoton();
  // push TrigPhoton into TrigPhotonContainer
  m_trigPhotonContainer->push_back(p_trigPhoton);
	p_trigPhoton->init( roiDescriptor->roiId(), dPhi, dEta, EClus);
//					    elink_cluster.getStorableObjectPointer(), 
//					    elink_cluster.index() );
  


ATH_MSG_DEBUG("REGTEST: TrigPhotonContainer has " << m_trigPhotonContainer->size()
		      << " element");
   if (!m_trigPhotonContainer->empty()) {
      xAOD::TrigPhoton* p_tp = m_trigPhotonContainer->front();
      ATH_MSG_DEBUG("REGTEST: TrigPhoton: RoI=" << p_tp->roiWord()       
	    << "; eta="    << p_tp->eta() 
	    << "; phi="    << p_tp->phi() 
	    << "; Et="     << p_tp->emCluster()->et() 
	    << "; Had Et=" << p_tp->etHad() 
	    << "; EnergyRatio=" << p_tp->eratio()
	    << "; rCore=" << p_tp->rcore());
      }  
   return StatusCode::SUCCESS;

}



