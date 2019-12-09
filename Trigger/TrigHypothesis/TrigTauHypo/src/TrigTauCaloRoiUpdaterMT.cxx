/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauCaloRoiUpdaterMT.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CxxUtils/phihelper.h"

#include "TLorentzVector.h"

TrigTauCaloRoiUpdaterMT::TrigTauCaloRoiUpdaterMT(const std::string & name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}

StatusCode TrigTauCaloRoiUpdaterMT::initialize() {

  ATH_MSG_DEBUG( "declareProperty review:"   );
  ATH_MSG_DEBUG( "    " << m_dRForCenter     );

  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_roIInputKey.initialize()        );
  CHECK( m_clustersKey.initialize() );
  CHECK( m_roIOutputKey.initialize()  );  

  return StatusCode::SUCCESS;
}


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
  float dPhi = fabs(CxxUtils::wrapToPi(roiDescriptor->phiPlus()-roiDescriptor->phi()));


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
                      TauDetectorAxis.Phi(), CxxUtils::wrapToPi(TauDetectorAxis.Phi()-dPhi), CxxUtils::wrapToPi(TauDetectorAxis.Phi()+dPhi),
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
