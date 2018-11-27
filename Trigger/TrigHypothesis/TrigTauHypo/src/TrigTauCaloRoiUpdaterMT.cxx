/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauCaloRoiUpdaterMT.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "TLorentzVector.h"


#include "TrigSteeringEvent/PhiHelper.h"

TrigTauCaloRoiUpdaterMT::TrigTauCaloRoiUpdaterMT(const std::string & name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

StatusCode TrigTauCaloRoiUpdaterMT::initialize() {
  ATH_MSG_INFO( "Initializing " << name() << " ... " );

  ATH_MSG_DEBUG( "declareProperty review:"   );
  ATH_MSG_DEBUG( "    " << m_dRForCenter     );

  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_roIInputKey.initialize()        );
  CHECK( m_clustersKey.initialize() );
  CHECK( m_roIOutputKey.initialize()  );  

  return StatusCode::SUCCESS;
}

TrigTauCaloRoiUpdaterMT::~TrigTauCaloRoiUpdaterMT(){}

StatusCode TrigTauCaloRoiUpdaterMT::execute() {

  ATH_MSG_DEBUG( "Running "<< name() <<" ... " );
  const EventContext& ctx = getContext();

  // Prepare Outputs
  std::unique_ptr< TrigRoiDescriptorCollection > roICollection( new TrigRoiDescriptorCollection() );

  // Retrieve Input CaloClusterContainer
  SG::ReadHandle< xAOD::CaloClusterContainer > CCContainerHandle = SG::makeHandle( m_clustersKey,ctx );
  CHECK( CCContainerHandle.isValid() );
  const xAOD::CaloClusterContainer *RoICaloClusterContainer = CCContainerHandle.get();
  ATH_MSG_DEBUG( "Found " << RoICaloClusterContainer->size() << " caloClusters, updating the corresponding RoI ... " );

  if(RoICaloClusterContainer != NULL) {
    ATH_MSG_DEBUG( "REGTEST: Size of vector CaloCluster container is " << RoICaloClusterContainer->size());
    if(RoICaloClusterContainer->size()==0) {
      ATH_MSG_DEBUG( "Cannot proceed, size of vector CaloCluster container is " << RoICaloClusterContainer->size());
      return StatusCode::SUCCESS;
    }
  }else {
    ATH_MSG_DEBUG( "no CaloCluster container found " );
    return StatusCode::SUCCESS;
  }

  //get RoI descriptor
  SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roIInputKey, ctx );
  ATH_MSG_DEBUG("Size of roisHandle: "<<roisHandle->size());
  const TrigRoiDescriptor *roiDescriptor = roisHandle->at(0);

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
       << ": Eta = " << roiDescriptor->eta()
       << ", Phi = " << roiDescriptor->phi() );

  // fill local variables for RoI reference position
  // Preserve the dEta and dPhi requirements from the original RoI
  float dEta = fabs(roiDescriptor->etaPlus() - roiDescriptor->eta());
  float dPhi = fabs(HLT::wrapPhi(roiDescriptor->phiPlus()-roiDescriptor->phi()));


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
                      TauDetectorAxis.Phi(), HLT::wrapPhi(TauDetectorAxis.Phi()-dPhi), HLT::wrapPhi(TauDetectorAxis.Phi()+dPhi),
                      roiDescriptor->zed() ,roiDescriptor->zedMinus(), roiDescriptor->zedPlus());

  ATH_MSG_DEBUG("Input RoI " << *roiDescriptor);
  ATH_MSG_DEBUG("Output RoI " << *outRoi);

  roICollection->push_back(outRoi);

  // Save Outputs
  ATH_MSG_DEBUG( "Saving RoIs to be used as input to Fast Tracking -- TO BE CHANGED -- ::: " << m_roIOutputKey.key() );
  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoiHandle = SG::makeHandle( m_roIOutputKey,ctx );
  CHECK( outputRoiHandle.record( std::move( roICollection ) ) );

  return StatusCode::SUCCESS;
}

StatusCode TrigTauCaloRoiUpdaterMT::finalize() {
  ATH_MSG_INFO( "Finalizing " << name() << " ... " );
  return StatusCode::SUCCESS;
}

//** ----------------------------------------------------------------------------------------------------------------- **//
/*
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

  msg() << MSG::DEBUG << "in initialize()" << endmsg;
  msg() << MSG::DEBUG << " REGTEST: HLTTauCaloRoiUpdater parameters " << endmsg;
  msg() << MSG::DEBUG << " REGTEST: dRForCenter = " << m_dRForCenter << endmsg;

  return HLT::OK;

}

HLT::ErrorCode HLTTauCaloRoiUpdater::hltFinalize()
{
  msg() << MSG::DEBUG << "in finalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode HLTTauCaloRoiUpdater::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
{
  HLT::ErrorCode status = HLT::OK;

  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  status = getFeature(outputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return status;
  }

  // Preserve the dEta and dPhi requirements from the original RoI
  float dEta = fabs(roiDescriptor->etaPlus() - roiDescriptor->eta());
  float dPhi = fabs(HLT::wrapPhi(roiDescriptor->phiPlus()-roiDescriptor->phi()));
  
  // Retrieve Calocluster container
  std::vector<const xAOD::CaloClusterContainer*> vectorCaloClusterContainer;
  status = getFeatures(outputTE, vectorCaloClusterContainer);

  if(status!=HLT::OK ) {
    msg() << MSG::ERROR << " No CaloClusterContainers retrieved for the trigger element" << endmsg;
    return status;
  }

  if (vectorCaloClusterContainer.size() < 1) {
    msg() << MSG::ERROR
          << "  CaloCluster container is empty"
          << endmsg;
    return HLT::ERROR;
  }

  //if( msgLvl() <= MSG::DEBUG )
  //  msg() << MSG::DEBUG << " CaloCluster container size is " << vectorCaloClusterContainer.size() << endmsg;

  // Grab the last cluster collection attached
  const xAOD::CaloClusterContainer* RoICaloClusterContainer = vectorCaloClusterContainer.back();

  if(RoICaloClusterContainer != NULL) {
    msg() << MSG::DEBUG
          << "REGTEST: Size of vector CaloCluster container is "
          << RoICaloClusterContainer->size()
          << endmsg;
    if(RoICaloClusterContainer->size()==0) {
      msg() << MSG::DEBUG
            << "Cannot proceed, size of vector CaloCluster container is "
            << RoICaloClusterContainer->size()
            << endmsg;
      return HLT::OK;
    }
  }
  else {
    msg() << MSG::ERROR << "no CaloCluster container found "<< endmsg;
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
						    TauDetectorAxis.Phi(), HLT::wrapPhi(TauDetectorAxis.Phi()-dPhi), HLT::wrapPhi(TauDetectorAxis.Phi()+dPhi),
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
*/
