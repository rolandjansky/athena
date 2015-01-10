/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauHypo/HLTTauCaloRoiUpdater.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TLorentzVector.h"
#include "TVector2.h"

HLTTauCaloRoiUpdater::HLTTauCaloRoiUpdater(const std::string & name, ISvcLocator* pSvcLocator) 
  : HLT::FexAlgo(name, pSvcLocator)
{
  declareProperty("dRForCenter", m_dRForCenter = 0.2);
}

HLTTauCaloRoiUpdater::~HLTTauCaloRoiUpdater()
{

}

HLT::ErrorCode HLTTauCaloRoiUpdater::hltInitialize()
{

  msg() << MSG::DEBUG << "in initialize()" << endreq;
  msg() << MSG::DEBUG << " REGTEST: HLTTauCaloRoiUpdater parameters " << endreq;
  msg() << MSG::DEBUG << " REGTEST: dRForCenter = " << m_dRForCenter << endreq;

  return HLT::OK;

}

HLT::ErrorCode HLTTauCaloRoiUpdater::hltFinalize()
{
  msg() << MSG::DEBUG << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode HLTTauCaloRoiUpdater::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
{
  HLT::ErrorCode status = HLT::OK;

  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  status = getFeature(outputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endreq;
    return status;
  }

  // Preserve the dEta and dPhi requirements from the original RoI
  float dEta = fabs(roiDescriptor->etaPlus() - roiDescriptor->eta());
  float dPhi = fabs(TVector2::Phi_mpi_pi(roiDescriptor->phiPlus()-roiDescriptor->phi()));
  
  // Retrieve Calocluster container
  std::vector<const xAOD::CaloClusterContainer*> vectorCaloClusterContainer;
  status = getFeatures(outputTE, vectorCaloClusterContainer);

  if(status!=HLT::OK ) {
    msg() << MSG::ERROR << " No CaloClusterContainers retrieved for the trigger element" << endreq;
    return status;
  }

  if (vectorCaloClusterContainer.size() < 1) {
    msg() << MSG::ERROR
          << "  CaloCluster container is empty"
          << endreq;
    return HLT::ERROR;
  }

  //if( msgLvl() <= MSG::DEBUG )
  //  msg() << MSG::DEBUG << " CaloCluster container size is " << vectorCaloClusterContainer.size() << endreq;

  // Grab the last cluster collection attached
  const xAOD::CaloClusterContainer* RoICaloClusterContainer = vectorCaloClusterContainer.back();

  if(RoICaloClusterContainer != NULL) {
    msg() << MSG::DEBUG
          << "REGTEST: Size of vector CaloCluster container is "
          << RoICaloClusterContainer->size()
          << endreq;
    if(RoICaloClusterContainer->size()==0) {
      msg() << MSG::DEBUG
            << "Cannot proceed, size of vector CaloCluster container is "
            << RoICaloClusterContainer->size()
            << endreq;
      return HLT::OK;
    }
  }
  else {
    msg() << MSG::ERROR << "no CaloCluster container found "<< endreq;
    return HLT::ERROR;
  }
  
  // Make a minimal effort to speed things up ;)
  TLorentzVector myCluster;
  TLorentzVector TauBarycenter(0., 0., 0., 0.);
  
  xAOD::CaloClusterContainer::const_iterator clusterIt;
  for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
    if((*clusterIt)->e() < 0)
      continue;
    
    myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
    TauBarycenter += myCluster;
  }
  
  // Determine the LC tau pT at detector axis
  TLorentzVector TauDetectorAxis(0.,0.,0.,0.);
  
  for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
    
    myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
    
    if(TauBarycenter.DeltaR(myCluster) > m_dRForCenter)
      continue;
    
    TauDetectorAxis += myCluster;
  } // end loop on clusters

  // Prepare the new RoI
  TrigRoiDescriptor *outRoi = new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						    TauDetectorAxis.Eta(), TauDetectorAxis.Eta()-dEta, TauDetectorAxis.Eta()+dEta,
						    TauDetectorAxis.Phi(), TVector2::Phi_mpi_pi(TauDetectorAxis.Phi()-dPhi), TVector2::Phi_mpi_pi(TauDetectorAxis.Phi()+dPhi),
						    roiDescriptor->zed() ,roiDescriptor->zedMinus(), roiDescriptor->zedPlus());
  
  ATH_MSG_DEBUG("Input RoI " << *roiDescriptor);
  ATH_MSG_DEBUG("Output RoI " << *outRoi);
  
  std::string roiName = "forID";
  
  if ( HLT::OK !=  attachFeature(outputTE, outRoi, roiName) ) {
    ATH_MSG_ERROR("Could not attach feature to the TE");
       return HLT::NAV_ERROR;
  }
  else {
    ATH_MSG_DEBUG("REGTEST: attached RoI " << roiName << *outRoi);
  }
  
  return HLT::OK;

}
