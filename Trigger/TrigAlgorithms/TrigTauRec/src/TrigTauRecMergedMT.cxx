/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/SystemOfUnits.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloEvent/CaloCellContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrack.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "TrigTauRecMergedMT.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include <iterator>
#include <algorithm>


// Invokes base class constructor.
TrigTauRecMergedMT::TrigTauRecMergedMT(const std::string& name,ISvcLocator* pSvcLocator)
  :AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrigTauRecMergedMT::~TrigTauRecMergedMT()
{ 
}

StatusCode TrigTauRecMergedMT::initialize()
{
  ATH_MSG_DEBUG("TrigTauRecMergedMT::initialize()");

  if ( m_tools.begin() == m_tools.end() ) {
    ATH_MSG_DEBUG(" no tools given for this algorithm.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("List of tools in execution sequence:");

  for(const auto& tool : m_tools) {
    // make sure the key of the container in tauRecTool are the same
    // need to set the property before the initialization of tools
    if (tool.name().find("VertexFinder") != std::string::npos) {
      ATH_CHECK( AAH::setProperty(tool, "Key_trackPartInputContainer",m_tracksKey.key()) );
      ATH_CHECK( AAH::setProperty(tool, "Key_vertexInputContainer",m_vertexKey.key()) );
    }
    else if (tool.name().find("TrackFinder") != std::string::npos) {
      ATH_CHECK( AAH::setProperty(tool, "Key_trackPartInputContainer",m_tracksKey.key()) );
    }

    StatusCode p_sc = tool.retrieve();
    if( p_sc.isFailure() ) {
      ATH_MSG_DEBUG("Cannot find tool named <" << tool << ">");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Add timer for tool "<< tool.type() <<" "<< tool.name());
    }
  }

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }

  ATH_MSG_DEBUG("Initialising HandleKeys");
  ATH_CHECK(m_roIInputKey.initialize());
  ATH_CHECK(m_L1RoIKey.initialize());
  ATH_CHECK(m_clustersKey.initialize(!m_clustersKey.key().empty()));
  ATH_CHECK(m_tracksKey.initialize(!m_tracksKey.key().empty()));
  ATH_CHECK(m_vertexKey.initialize(!m_vertexKey.key().empty()));
  ATH_CHECK(m_trigTauJetKey.initialize(!m_trigTauJetKey.key().empty()));
  ATH_CHECK(m_trigTauTrackInKey.initialize(!m_trigTauTrackInKey.key().empty()));
  ATH_CHECK(m_trigtauSeedOutKey.initialize());
  ATH_CHECK(m_trigtauRecOutKey.initialize());
  ATH_CHECK(m_trigtauTrkOutKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode TrigTauRecMergedMT::finalize()
{
  return StatusCode::SUCCESS;
}
StatusCode TrigTauRecMergedMT::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Execution");

  // variables to initialize and keep values for monitoring variables
  std::vector<unsigned char> calo_errors(0);
  std::vector<unsigned char> track_errors(0);
  //std::vector<const xAOD::CaloCluster *> clusters;
  std::vector<float> cluster_et_log(0);
  std::vector<float> cluster_dEta(0); 
  std::vector<float> cluster_dPhi(0);
  std::vector<float> cluster_log_SECOND_R(0);
  std::vector<float> cluster_SECOND_LAMBDA(0);
  std::vector<float> cluster_CENTER_LAMBDA(0);
  std::vector<float> track_pt_log(0);
  std::vector<float> track_dEta(0);
  std::vector<float> track_dPhi(0);
  std::vector<float> track_z0sinThetaTJVA_abs_log(0);
  std::vector<float> track_d0_abs_log(0);
  std::vector<float> track_nIBLHitsAndExp(0);
  std::vector<float> track_nPixelHitsPlusDeadSensors(0);
  std::vector<float> track_nSCTHitsPlusDeadSensors(0);

  auto nCells             = Monitored::Scalar<int>("nRoI_EFTauCells",    0);
  auto nTracks            = Monitored::Scalar<int>("nRoI_EFTauTracks", -10);
  auto dEta               = Monitored::Scalar<float>("dEtaEFTau_RoI",    -10.);
  auto dPhi               = Monitored::Scalar<float>("dPhiEFTau_RoI",    -10.);
  auto emRadius           = Monitored::Scalar<float>("EMRadius", -0.099);
  auto hadRadius          = Monitored::Scalar<float>("HadRadius", -0.099);
  auto EtFinal            = Monitored::Scalar<float>("EtFinal", 0.);
  auto Et                 = Monitored::Scalar<float>("Et", 0.);
  auto EtHad              = Monitored::Scalar<float>("EtHad",-10.);
  auto EtEm               = Monitored::Scalar<float>("EtEm",-10.);
  auto EMFrac             = Monitored::Scalar<float>("EMFrac",-10.);
  auto IsoFrac            = Monitored::Scalar<float>("IsoFrac",-1.);
  auto centFrac           = Monitored::Scalar<float>("centFrac",-10.);
  auto nWideTrk           = Monitored::Scalar<int>("nWideTrk",-10);
  auto ipSigLeadTrk       = Monitored::Scalar<float>("ipSigLeadTrk",-1000.);
  auto trFlightPathSig    = Monitored::Scalar<float>("trFlightPathSig",-10.);
  auto massTrkSys         = Monitored::Scalar<float>("massTrkSys",-10.);
  auto dRmax              = Monitored::Scalar<float>("dRmax",-10.);
  auto numTrack           = Monitored::Scalar<int>("NTrk", -10);
  auto trkAvgDist         = Monitored::Scalar<float>("TrkAvgDist",-1.0);
  auto etovPtLead         = Monitored::Scalar<float>("EtovPtLead",-10.);
  auto PSSFraction        = Monitored::Scalar<float>("PSSFraction",-999.9);
  auto EMPOverTrkSysP     = Monitored::Scalar<float>("EMPOverTrkSysP",-999.9);
  auto ChPiEMEOverCaloEME = Monitored::Scalar<float>("ChPiEMEOverCaloEME",-999.9);
  auto SumPtTrkFrac       = Monitored::Scalar<float>("SumPtTrkFrac",-999.9);
  auto innerTrkAvgDist    = Monitored::Scalar<float>("innerTrkAvgDist",-1.0);
  auto Ncand              = Monitored::Scalar<int>("nCand",0);
  auto EtaL1              = Monitored::Scalar<float>("EtaL1",-99.9);
  auto PhiL1              = Monitored::Scalar<float>("PhiL1",-99.9);
  auto EtaEF              = Monitored::Scalar<float>("EtaEF",-99.9);
  auto PhiEF              = Monitored::Scalar<float>("PhiEF",-99.9);
  auto mEflowApprox       = Monitored::Scalar<float>("mEflowApprox", -99.9);
  auto ptRatioEflowApprox = Monitored::Scalar<float>("ptRatioEflowApprox", -99.9);
  auto pt_jetseed_log     = Monitored::Scalar<float>("pt_jetseed_log",-99.9);
  auto ptDetectorAxis     = Monitored::Scalar<float>("ptDetectorAxis",-99.9);
  auto RNN_tracknumber    = Monitored::Scalar<int>("RNN_tracknumber",0);
  auto RNN_clusternumber  = Monitored::Scalar<int>("RNN_clusternumber",0); 
   
  auto EF_calo_errors     = Monitored::Collection("calo_errors",calo_errors);
  auto EF_track_errors    = Monitored::Collection("track_errors",track_errors);

  auto Cluster_et_log     = Monitored::Collection("cluster_et_log", cluster_et_log);
  auto Cluster_dEta       = Monitored::Collection("cluster_dEta", cluster_dEta);
  auto Cluster_dPhi       = Monitored::Collection("cluster_dPhi", cluster_dPhi);
  auto Cluster_log_SECOND_R = Monitored::Collection("cluster_log_SECOND_R", cluster_log_SECOND_R);
  auto Cluster_SECOND_LAMBDA = Monitored::Collection("cluster_SECOND_LAMBDA", cluster_SECOND_LAMBDA);
  auto Cluster_CENTER_LAMBDA = Monitored::Collection("cluster_CENTER_LAMBDA", cluster_CENTER_LAMBDA);

  auto Track_pt_log = Monitored::Collection("track_pt_log", track_pt_log);
  auto Track_dEta = Monitored::Collection("track_dEta", track_dEta);
  auto Track_dPhi = Monitored::Collection("track_dPhi", track_dPhi);
  auto Track_z0sinThetaTJVA_abs_log = Monitored::Collection("track_z0sinThetaTJVA_abs_log", track_z0sinThetaTJVA_abs_log); 
  auto Track_d0_abs_log = Monitored::Collection("track_d0_abs_log", track_d0_abs_log);
  auto Track_nIBLHitsAndExp = Monitored::Collection("track_nIBLHitsAndExp", track_nIBLHitsAndExp);
  auto Track_nPixelHitsPlusDeadSensors = Monitored::Collection("track_nPixelHitsPlusDeadSensors", track_nPixelHitsPlusDeadSensors);
  auto Track_nSCTHitsPlusDeadSensors = Monitored::Collection("track_nSCTHitsPlusDeadSensors", track_nSCTHitsPlusDeadSensors);

  auto monitorIt = Monitored::Group( m_monTool, nCells, nTracks, dEta, dPhi, emRadius, hadRadius,
                   EtFinal, Et, EtHad, EtEm, EMFrac, IsoFrac, centFrac, nWideTrk, ipSigLeadTrk, trFlightPathSig, massTrkSys,
                   dRmax, numTrack, trkAvgDist, etovPtLead, PSSFraction, EMPOverTrkSysP, ChPiEMEOverCaloEME, SumPtTrkFrac,
                   innerTrkAvgDist, Ncand, EtaL1, PhiL1, EtaEF, PhiEF, mEflowApprox, ptRatioEflowApprox, pt_jetseed_log, ptDetectorAxis, RNN_clusternumber, Cluster_et_log, Cluster_dEta, Cluster_dPhi, Cluster_log_SECOND_R,
                   Cluster_SECOND_LAMBDA, Cluster_CENTER_LAMBDA, RNN_tracknumber, Track_pt_log, Track_dEta, Track_dPhi, Track_z0sinThetaTJVA_abs_log, Track_d0_abs_log, Track_nIBLHitsAndExp,
                   Track_nPixelHitsPlusDeadSensors, Track_nSCTHitsPlusDeadSensors); 


  // Retrieve store.
  ATH_MSG_DEBUG("Executing TrigTauRecMergedMT");

  // Get RoiDescriptor
  SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roIInputKey, ctx );
  if ( not roisHandle.isValid() ) {
    ATH_MSG_ERROR("No roisHandle found");
    return StatusCode::FAILURE;
  }

  if(roisHandle->size() == 0){
    ATH_MSG_ERROR("RoIHandle size = Zero");
    return StatusCode::FAILURE;
  }
  const TrigRoiDescriptor *roiDescriptor = roisHandle->at(0);

  if ( roiDescriptor != nullptr ) {
    ATH_MSG_DEBUG(" RoI " << *roiDescriptor);
  }
  else {
    ATH_MSG_ERROR("Failed to find RoiDescriptor ");
    return StatusCode::FAILURE;
  }

  // get TauJetContainer from SG
  const xAOD::TauJetContainer *pTauContainer = nullptr;
  const xAOD::TauTrackContainer *pTauTrackContainer = nullptr;

  if (!m_trigTauJetKey.key().empty() && m_clustersKey.key().empty()) {
    SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_trigTauJetKey, ctx);
    pTauContainer = tauInputHandle.cptr();
    ATH_MSG_DEBUG("Input TauJet Container size: " << pTauContainer->size());
  }

  if (!m_trigTauTrackInKey.key().empty() && m_clustersKey.key().empty()) {
    SG::ReadHandle<xAOD::TauTrackContainer> tauTrackInputHandle(m_trigTauTrackInKey, ctx);
    pTauTrackContainer = tauTrackInputHandle.cptr();
    ATH_MSG_DEBUG("Tau Track Container Size " << pTauTrackContainer->size());
  }

  // Make new container which is deep copy of that
  std::unique_ptr<xAOD::TauJetContainer>    pContainer    = std::make_unique<xAOD::TauJetContainer>();
  std::unique_ptr<xAOD::TauJetAuxContainer> pAuxContainer = std::make_unique<xAOD::TauJetAuxContainer>();
  pContainer->setStore(pAuxContainer.get());

  // Write final taujets container
  SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_trigtauRecOutKey, ctx);
  ATH_CHECK(outputTauHandle.record(std::move(pContainer),std::move(pAuxContainer)));

  xAOD::TauJet* tau(0);
  xAOD::TauJet* p_tau(0);

  ATH_CHECK(deepCopy(outputTauHandle, tau, pTauContainer));
  if(outputTauHandle->size()>0) p_tau = outputTauHandle->back();

  std::unique_ptr<xAOD::TauTrackContainer>    pTrackContainer    = std::make_unique<xAOD::TauTrackContainer>();
  std::unique_ptr<xAOD::TauTrackAuxContainer> pTrackAuxContainer = std::make_unique<xAOD::TauTrackAuxContainer>();
  pTrackContainer->setStore(pTrackAuxContainer.get());

  SG::WriteHandle<xAOD::TauTrackContainer> tauTrackHandle(m_trigtauTrkOutKey, ctx);
  ATH_MSG_DEBUG("  write: " << tauTrackHandle.key() << " = " << "..." );
  ATH_CHECK(tauTrackHandle.record(std::move(pTrackContainer), std::move(pTrackAuxContainer)));

  xAOD::TauTrack* tautrack(0);

  ATH_CHECK(deepCopy(tauTrackHandle, tautrack, pTauTrackContainer));

  if(!m_trigTauTrackInKey.key().empty() && m_clustersKey.key().empty()){
    p_tau->clearTauTrackLinks();
    if(tauTrackHandle.isValid() && tauTrackHandle->size()>0){
      ATH_MSG_DEBUG("TauTrackContainer size: " << tauTrackHandle->size());
      for( xAOD::TauTrack* track : *tauTrackHandle){
        ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
        linkToTauTrack.toContainedElement(*tauTrackHandle, track);
        p_tau->addTauTrackLink(linkToTauTrack);

        RNN_tracknumber += 1;
        track_pt_log.push_back(TMath::Log10( track->pt()));
        track_dEta.push_back(track->eta()- p_tau->eta()); 
        track_dPhi.push_back(track->p4().DeltaPhi(p_tau->p4()));
        track_z0sinThetaTJVA_abs_log.push_back(track->z0sinThetaTJVA(*p_tau));
        track_d0_abs_log.push_back(TMath::Log10( TMath::Abs(track->track()->d0()) + 1e-6));

        uint8_t inner_pixel_hits, inner_pixel_exp;                    
        const auto success1_innerPixel_hits = track->track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);                        
        const auto success2_innerPixel_exp = track->track()->summaryValue(inner_pixel_exp, xAOD::expectInnermostPixelLayerHit);                                       
        float nIBLHitsAndExp = -999;                                              
        if (success1_innerPixel_hits && success2_innerPixel_exp) {nIBLHitsAndExp=inner_pixel_exp ? inner_pixel_hits : 1.;};        
        track_nIBLHitsAndExp.push_back(nIBLHitsAndExp);

        uint8_t pixel_hits, pixel_dead;                                    
        const auto success1_pixel_hits = track->track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);          
        const auto success2_pixel_dead = track->track()->summaryValue(pixel_dead, xAOD::numberOfPixelDeadSensors);                           
        float nPixelHitsPlusDeadSensor = -999;                                         
        if (success1_pixel_hits && success2_pixel_dead) {nPixelHitsPlusDeadSensor=pixel_hits + pixel_dead;};                      
        track_nPixelHitsPlusDeadSensors.push_back(nPixelHitsPlusDeadSensor);

        uint8_t sct_hits, sct_dead;                                       
        const auto success1_sct_hits = track->track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);                   
        const auto success2_sct_dead = track->track()->summaryValue(sct_dead, xAOD::numberOfSCTDeadSensors);                           
        float nSCTHitsPlusDeadSensors = -999;     
        if (success1_sct_hits && success2_sct_dead) {nSCTHitsPlusDeadSensors=sct_hits + sct_dead;};                               
        track_nSCTHitsPlusDeadSensors.push_back(nSCTHitsPlusDeadSensors);

      }
    }
  }

  const xAOD::CaloClusterContainer *RoICaloClusterContainer = nullptr;

  if(!m_clustersKey.key().empty()){
    // Retrieve Calocluster container
    SG::ReadHandle< xAOD::CaloClusterContainer > CCContainerHandle = SG::makeHandle( m_clustersKey,ctx );
    CHECK( CCContainerHandle.isValid() );

    RoICaloClusterContainer = CCContainerHandle.get();

    if(RoICaloClusterContainer != nullptr) {
      ATH_MSG_DEBUG( "CaloCluster container found of size: " << RoICaloClusterContainer->size());
      //If size is zero, don't stop just continue to produce empty TauJetCollection
    }
    else {
      ATH_MSG_ERROR( "no CaloCluster container found " );
      return StatusCode::FAILURE;
    }

    if(p_tau==nullptr){
      p_tau = new xAOD::TauJet();
      outputTauHandle->push_back(p_tau);
      p_tau->setROIWord(roiDescriptor->roiWord());
    }

    std::unique_ptr<xAOD::JetContainer> theJetContainer{std::make_unique<xAOD::JetContainer>()};
    std::unique_ptr<xAOD::JetTrigAuxContainer> theTrigJetAuxContainer{std::make_unique<xAOD::JetTrigAuxContainer>()};

    theJetContainer->setStore(theTrigJetAuxContainer.get());

    xAOD::Jet *aJet = new xAOD::Jet;

    theJetContainer->push_back(aJet);
	
    // Build the jet, also keep track of the kinematics by hand
    // We are using calibrated clusters, we need to keep track of this
    aJet->setConstituentsSignalState(xAOD::JetConstitScale::CalibratedJetConstituent);
	
    // Make a minimal effort to speed things up ;)
    // Eventually, want to use FastJet here?
    TLorentzVector myCluster;
    TLorentzVector TauBarycenter(0., 0., 0., 0.);

    xAOD::CaloClusterContainer::const_iterator clusterIt;
    for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
      ATH_MSG_DEBUG(" Cluster (e, eta, phi) : ("<< (*clusterIt)->e() << " , " <<(*clusterIt)->eta()<<" , "<<(*clusterIt)->phi()<< " )");
    
      if((*clusterIt)->e() < 0)
        {
	  ATH_MSG_DEBUG(" Negative energy cluster is rejected");
	  continue;
        }
    
      myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
      aJet->addConstituent(*clusterIt);

      TauBarycenter += myCluster;
      RNN_clusternumber += 1;

      cluster_et_log.push_back(TMath::Log10( (*clusterIt)->et()));
      cluster_dEta.push_back((*clusterIt)->eta()- p_tau->eta());
      cluster_dPhi.push_back((*clusterIt)->p4().DeltaPhi(p_tau->p4()));
     
      double log_second_R = -999.;
      const auto success_SECOND_R = (*clusterIt)->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_R,log_second_R);
      if (success_SECOND_R) log_second_R = TMath::Log10(log_second_R + 0.1);
      cluster_log_SECOND_R.push_back(log_second_R);
 
      double second_lambda = -999.;
      const auto success_SECOND_LAMBDA = (*clusterIt)->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA, second_lambda);
      if (success_SECOND_LAMBDA) second_lambda = TMath::Log10(second_lambda + 0.1);
      cluster_SECOND_LAMBDA.push_back(second_lambda);

      double center_lambda = -999.;
      const auto success_CENTER_LAMBDA = (*clusterIt)->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA, center_lambda);
      if (success_CENTER_LAMBDA) center_lambda = TMath::Log10(center_lambda + 1e-6);
      cluster_CENTER_LAMBDA.push_back(center_lambda);      

    }
	 
    aJet->setJetP4(xAOD::JetFourMom_t(TauBarycenter.Pt(), TauBarycenter.Eta(), TauBarycenter.Phi(), TauBarycenter.M() ) ); 

    ATH_MSG_DEBUG("jet formed"<< aJet->eta() <<" , " << aJet->phi() <<" , " << aJet->pt() << " , "<< aJet->e() );

    //-------------------------------------------------------------------------
    // using Jet collection
    // setup tau candidate structure
    //-------------------------------------------------------------------------

    p_tau->setJet(theJetContainer.get(), aJet);

    if(aJet->e()<=0) {
      ATH_MSG_DEBUG( "Roi: changing eta due to energy " << aJet->e() );
      p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());		
      ATH_MSG_DEBUG( "Roi: " << roiDescriptor->roiId() << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi() );
    }

    // Save Outputs
    SG::WriteHandle< xAOD::JetContainer > outTauSeedHandle = SG::makeHandle( m_trigtauSeedOutKey,ctx );
    CHECK( outTauSeedHandle.record( std::move( theJetContainer ), std::move( theTrigJetAuxContainer ) ) );
  }

  //Check if jetLink is valid for all taus
  CHECK(p_tau->jetLink().isValid());

  // get TrackContainer
  if(!m_tracksKey.key().empty()){
    SG::ReadHandle< xAOD::TrackParticleContainer > TPContainerHandle = SG::makeHandle( m_tracksKey,ctx );

    const xAOD::TrackParticleContainer *RoITrackParticleContainer = nullptr;

    if ( !TPContainerHandle.isValid() ) {
      ATH_MSG_DEBUG(" No Track container found.");
      track_errors.push_back(NoTrkCont);
    }
    else {
      RoITrackParticleContainer = TPContainerHandle.get();
      ATH_MSG_DEBUG(" Size of vector Track container is " << RoITrackParticleContainer->size());
      if(RoITrackParticleContainer != nullptr) nTracks = RoITrackParticleContainer->size();
    }
  }

  // get Vertex Container
  if(!m_vertexKey.key().empty()){
    SG::ReadHandle< xAOD::VertexContainer > VertexContainerHandle = SG::makeHandle( m_vertexKey,ctx );

    const xAOD::VertexContainer* RoIVxContainer = nullptr;

    if( !VertexContainerHandle.isValid() ) {
      ATH_MSG_DEBUG(" No VxContainers retrieved for the trigger element");
      track_errors.push_back(NoVtxCont);
    }
    else {
      RoIVxContainer = VertexContainerHandle.get();
      ATH_MSG_DEBUG(" Size of vector Vertex  container " << RoIVxContainer->size());
    }
  }

  ATH_MSG_DEBUG(" roidescriptor roiword " << roiDescriptor->roiWord() << " saved " << p_tau->ROIWord() );

  StatusCode processStatus = StatusCode::SUCCESS;

  ATH_MSG_DEBUG(" initialize all good ");

  //-------------------------------------------------------------------------
  // loop over booked tau tools
  //-------------------------------------------------------------------------
  int toolnum = 0;
  processStatus = StatusCode::SUCCESS;
  
  // dummy container passed to TauVertexVariables, not used in trigger though
  xAOD::VertexContainer dummyVxCont;
  
  ATH_MSG_DEBUG("Starting tool loop with seed jet");

  for (const auto& tool : m_tools) {
    // loop stops only when Failure indicated by one of the tools
    ATH_MSG_DEBUG("Starting Tool: " <<  tool->name() );
    // time in the various tools
    ++toolnum;

    if (tool->type() == "TauVertexFinder" ) {
      processStatus = tool->executeVertexFinder(*p_tau);
    }
    else if (tool->type() == "TauTrackFinder") {
      processStatus = tool->executeTrackFinder(*p_tau, *tauTrackHandle);
    }
    else if (tool->type() == "TauVertexVariables" ) {
      processStatus = tool->executeVertexVariables(*p_tau, dummyVxCont);
    }
    else {
      processStatus = tool->execute( *p_tau );
    }
    
    if ( !processStatus.isFailure() ) {
      ATH_MSG_DEBUG(" "<< tool->name() << " executed successfully ");
      ATH_MSG_DEBUG(" Roi: " << roiDescriptor->roiId()
		    << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi()
		    << " Tau pT : "<< p_tau->pt());
    }
    else {
      ATH_MSG_DEBUG(" "<< tool->name() << " execution failed ");
      break;
    }
  }

  ATH_MSG_DEBUG("This tau has " << p_tau->allTracks() << " tracks linked");

  //check status
  if ( !processStatus.isSuccess() )  {   // some problem
    ATH_MSG_DEBUG("The tau object has NOT been registered in the tau container");

    xAOD::TauJet* bad_tau = outputTauHandle->back();
    ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << " tracks associated with tau");
    tauTrackHandle->erase(tauTrackHandle->end()-bad_tau->nAllTracks(), tauTrackHandle->end());

    outputTauHandle->pop_back();

    ATH_MSG_DEBUG("Clean up done after jet seed");
  
  }
  else {

    float fJetEnergy = (*p_tau->jetLink())->e();
    ATH_MSG_DEBUG("Roi: jet e "<< fJetEnergy);
	  
    if( fJetEnergy < 0.00001 ) {
      ATH_MSG_DEBUG("Roi: changing eta phi to L1 ones due to energy negative (PxPyPzE flips eta and phi)");
      ATH_MSG_DEBUG("Roi: this is probably not needed anymore, method PxPyPzE has been corrected");
	    
      p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());
	    
      ATH_MSG_DEBUG("Roi: " << roiDescriptor->roiId()
		    << " Tau eta: " << p_tau->eta()
		    << " Tau phi: " << p_tau->phi()
		    << " Tau pT : "<< p_tau->pt());
    }
	  
    // Get L1 RoiDescriptor
    SG::ReadHandle< TrigRoiDescriptorCollection > L1roisHandle = SG::makeHandle( m_L1RoIKey, ctx );

    if ( L1roisHandle.isValid() && (L1roisHandle->size() != 0) ) {
      const TrigRoiDescriptor *roiL1Descriptor = L1roisHandle->at(0);      
      EtaL1 = roiL1Descriptor->eta();
      PhiL1 = roiL1Descriptor->phi();
    }
    else {
      ATH_MSG_WARNING("Failed to retrieve L1 RoI descriptor!");
    }

    // get tau detail variables for Monitoring

    numTrack = p_tau->nTracks();
    nWideTrk = p_tau->nTracksIsolation();
    p_tau->detail(xAOD::TauJetParameters::numCells, nCells);
    p_tau->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
    p_tau->detail(xAOD::TauJetParameters::trkAvgDist, trkAvgDist);
    p_tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etovPtLead);
    p_tau->detail(xAOD::TauJetParameters::EMRadius, emRadius);
    p_tau->detail(xAOD::TauJetParameters::hadRadius, hadRadius);
    p_tau->detail(xAOD::TauJetParameters::isolFrac, IsoFrac);
    p_tau->detail(xAOD::TauJetParameters::centFrac, centFrac);
    p_tau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk);
    p_tau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig);
    p_tau->detail(xAOD::TauJetParameters::dRmax, dRmax);
    p_tau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys);
	  
    p_tau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction);
    p_tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP);
    p_tau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME);
    p_tau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist);	 
    p_tau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac);

    massTrkSys /= Gaudi::Units::GeV;
    p_tau->detail(xAOD::TauJetParameters::etEMAtEMScale, EtEm);
    EtEm /= Gaudi::Units::GeV;
    p_tau->detail(xAOD::TauJetParameters::etHadAtEMScale, EtHad);
    EtHad /= Gaudi::Units::GeV;
    Et            = EtEm + EtHad;
    EtFinal       = p_tau->pt()/Gaudi::Units::GeV;
    
    EtaEF = p_tau->eta();
    PhiEF = p_tau->phi();
	  
    if( Et !=0) EMFrac =  EtEm/ Et ;
	  
    dEta =  EtaEF - roiDescriptor->eta();
    dPhi =  PhiEF - roiDescriptor->phi();
    if(dPhi<-M_PI) dPhi += 2.0*M_PI;
    if(dPhi>M_PI)  dPhi -= 2.0*M_PI;

   
    float pre_mEflowApprox;
    p_tau->detail(xAOD::TauJetParameters::mEflowApprox, pre_mEflowApprox);  
    mEflowApprox = TMath::Log10(std::max(pre_mEflowApprox, 140.0f));

    float pre_ptRatioEflowApprox;
    p_tau->detail(xAOD::TauJetParameters::ptRatioEflowApprox, pre_ptRatioEflowApprox);
    ptRatioEflowApprox = std::min(pre_ptRatioEflowApprox, 4.0f);
    
    pt_jetseed_log  = TMath::Log10(p_tau->ptJetSeed());
    ptDetectorAxis  =  TMath::Log10(std::min(p_tau->ptDetectorAxis() / 1000.0, 100.0));


    ATH_MSG_DEBUG(" Roi: " << roiDescriptor->roiId()
		  << " Tau being saved eta: " << EtaEF << " Tau phi: " << PhiEF
		  << " wrt L1 dEta "<< dEta<<" dPhi "<<dPhi
		  << " Tau Et (GeV): "<< EtFinal);
	  
    ++Ncand;
  }

  //-------------------------------------------------------------------------
  // all done, register the tau Container in TDS.
  //-------------------------------------------------------------------------
  
  ATH_MSG_DEBUG("Output TauJetContainer size:"<< outputTauHandle->size());
  ATH_MSG_DEBUG("Output TauJetTrackContainer size:"<< tauTrackHandle->size());
  
  
  ATH_MSG_DEBUG("Recorded a tau container: HLT_TrigTauRecMergedMT");
  ATH_MSG_DEBUG("the tau object has been registered in the tau container");
  
  return StatusCode::SUCCESS;
}

