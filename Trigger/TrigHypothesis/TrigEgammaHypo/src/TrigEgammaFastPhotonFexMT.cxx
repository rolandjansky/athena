/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// -*- C++ -*-

/**************************************************************************
 ** 
 **   Original Author:       B.Safarzadeh   

 **************************************************************************/ 

#include "TrigEgammaFastPhotonFexMT.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"


class ISvcLocator;


TrigEgammaFastPhotonFexMT::TrigEgammaFastPhotonFexMT(const std::string & name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}


TrigEgammaFastPhotonFexMT::~TrigEgammaFastPhotonFexMT()
{}


StatusCode TrigEgammaFastPhotonFexMT::initialize()
{

  ATH_CHECK( m_roiCollectionKey.initialize() );
  ATH_CHECK( m_TrigEMClusterContainerKey.initialize() );
  ATH_CHECK( m_outputPhotonsKey.initialize() );
  return  StatusCode::SUCCESS;
}


StatusCode TrigEgammaFastPhotonFexMT::execute()
{
  using namespace xAOD;
  auto ctx = getContext();

  //  xAOD::TrigPhotonAuxContainer trigPhotonAuxContainer;

  auto trigPhotoColl =   SG::makeHandle (m_outputPhotonsKey, ctx);  
  ATH_CHECK( trigPhotoColl.record (std::make_unique<xAOD::TrigPhotonContainer>(),
				   std::make_unique<xAOD::TrigEMClusterAuxContainer>()) );

  ATH_MSG_DEBUG( "Made WriteHandle " << m_outputPhotonsKey );

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
  trigPhotoColl->push_back(p_trigPhoton);
  p_trigPhoton->init( roiDescriptor->roiId(), dPhi, dEta, EClus);
  

  ATH_MSG_DEBUG("REGTEST: TrigPhotonContainer has " << trigPhotoColl->size()<< " element");
  if (!trigPhotoColl->empty()) {
    xAOD::TrigPhoton* p_tp = trigPhotoColl->front();
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