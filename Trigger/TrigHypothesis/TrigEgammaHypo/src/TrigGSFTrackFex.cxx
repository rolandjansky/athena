// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigGSFTrackFex.cxx
 **
 **   Description: FEX Algorithm to refit tracks using GSF 
 **                
 **
 **   Author: Ryan Mackenzie White 
 **            
 **
 **   Created:      January 2015 
 **************************************************************************/

#include "TrigEgammaHypo/TrigGSFTrackFex.h"
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
//Refit
#include "egammaInterfaces/IegammaTrkRefitterTool.h"
//extrapolation
#include "egammaInterfaces/IEMExtrapolationTools.h"
//
#include "TrkEventPrimitives/VertexType.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
// #include "CandidateMatchHelpers.h" --> need our own methods since this is in egammaTools
//
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class ISvcLocator;


TrigGSFTrackFex::TrigGSFTrackFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
    declareProperty( "AcceptAll",            m_acceptAll  = false );
    // Tools
    declareProperty("TrackRefitTool",                         m_trkRefitTool);
    declareProperty("TrackParticleCreatorTool",               m_particleCreatorTool);
    declareProperty("TrackSlimmingTool",                      m_slimTool);
    declareProperty("TrackSummaryTool",                       m_summaryTool);
    declareProperty("ExtrapolationTool",                      m_extrapolationTool, "Handle of the extrapolation tool");
    //=================================================================================
    //Minimum silicon hits before doing GSF
    declareProperty("minNoSiHits",                            m_MinNoSiHits = 4,         
            "Minimum number of silicon hits on track before it is allowed to be refitted");
    //=================================================================================
    // Value of broad cut for delta eta
    declareProperty("broadDeltaEta",                           m_broadDeltaEta = 0.1,
            "Value of broad cut for delta eta, it is mult by 2");
    // Value of broad cut for delta phi
    declareProperty("broadDeltaPhi",                           m_broadDeltaPhi = 0.15,
            "Value of broad cut for delta phi, it is mult by 2 ");
    // Value of narrow cuts
    declareProperty("narrowDeltaEta",                          m_narrowDeltaEta = 0.05,
            "Value of narrow cut for delta eta");
    declareProperty("narrowDeltaPhi",                          m_narrowDeltaPhi = 0.05,
            "Value of narrow cut for delta phi");
    declareProperty("narrowDeltaPhiBrem",                      m_narrowDeltaPhiBrem =0.15,
            "Value of the narrow cut for delta phi in the brem direction");
    declareProperty("narrowDeltaPhiRescale",                   m_narrowRescale =0.05,
            "Value of the narrow cut for delta phi Rescale");
    declareProperty("narrowDeltaPhiRescaleBrem",               m_narrowRescaleBrem =0.1,
            "Value of the narrow cut for delta phi Rescale Brem");

    declareMonitoredVariable("nClusters",m_AllClusters);
    declareMonitoredVariable("nTracks",m_AllTracks);
    declareMonitoredVariable("nSelTracks",m_SelectedTracks);
    declareMonitoredVariable("nFailedFitTracks",m_FailedFitTracks);
    declareMonitoredVariable("nFailedSiTracks",m_FailedSiliconRequirFit);
    declareMonitoredVariable("nRefitTracks",m_RefittedTracks);
}


TrigGSFTrackFex::~TrigGSFTrackFex()
{}


HLT::ErrorCode TrigGSFTrackFex::hltInitialize()
{
    ATH_MSG_DEBUG("Initialization:");
    // retrieve the track refitter tool:
    if(m_trkRefitTool.retrieve().isFailure()) {
        ATH_MSG_ERROR ( "Unable to retrieve " << m_trkRefitTool);
        return HLT::BAD_JOB_SETUP;
    }

    /* Get the particle creation tool */
    if ( m_particleCreatorTool.retrieve().isFailure() ) {
        ATH_MSG_ERROR ("Failed to retrieve tool " << m_particleCreatorTool);
        return HLT::BAD_JOB_SETUP;
    }

    /* Get the track slimming tool */
    if ( m_slimTool.retrieve().isFailure() ) {
        ATH_MSG_ERROR ( "Failed to retrieve TrkTrackSlimmingTool tool "<< m_slimTool);
        return HLT::BAD_JOB_SETUP;
    }

    /* Get the track summary tool */
    if ( m_summaryTool.retrieve().isFailure() ) {
        ATH_MSG_ERROR ( "Failed to retrieve TrkTrackSummaryTool tool "<< m_summaryTool);
        return HLT::BAD_JOB_SETUP;
    }

    /* the extrapolation tool*/
    if(m_extrapolationTool.retrieve().isFailure()){
        ATH_MSG_ERROR("initialize: Cannot retrieve extrapolationTool " << m_extrapolationTool);
        return HLT::BAD_JOB_SETUP;
    }
    if (timerSvc()){
        m_totalTimer = addTimer("TrigGSFTrackFexTot");  
        m_toolTimer = addTimer("TrigGSFTrackFexRefit");
    }

    // print out settings
    ATH_MSG_DEBUG("Initialization completed successfully:");
    ATH_MSG_DEBUG("AcceptAll            = "	<< (m_acceptAll==true ? "True" : "False") ); 
    return HLT::OK;
}


HLT::ErrorCode TrigGSFTrackFex::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigGSFTrackFex::hltExecute(const HLT::TriggerElement* inputTE,
        HLT::TriggerElement* outputTE) {

    // If this runs as an additional algorithm after tracking but at same step in sequence
    // Need to get TrackCollection Features from outputTE and attach new collection to outputTE
    HLT::ErrorCode stat = HLT::OK;
    m_AllClusters = 0;
    m_AllTracks = 0;
    m_AllTRTTracks = 0;
    m_AllSiTracks = 0;
    m_SelectedTracks = 0;
    m_FailedFitTracks = 0;
    m_FailedSiliconRequirFit = 0;
    m_RefittedTracks = 0;
    if (timerSvc()) m_totalTimer->start();    
    // Retrieve cluster container
    // get pointer to CaloClusterContainer from the trigger element
    std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer;

    stat = getFeatures(inputTE, vectorClusterContainer);
    if ( stat!= HLT::OK ) {
        ATH_MSG_ERROR(" REGTEST: No CaloClusterContainers retrieved for the trigger element");
        return HLT::OK;
    }
    //debug message
    ATH_MSG_DEBUG(" REGTEST: Got " << vectorClusterContainer.size() << " CaloClusterContainers associated to the TE ");
            

    // Check that there is only one ClusterContainer in the RoI
    if (vectorClusterContainer.size() != 1){
        ATH_MSG_ERROR("REGTEST: Size of vectorClusterContainer is not 1, it is: "<< vectorClusterContainer.size());
        return HLT::OK;
    }

    // Get the last ClusterContainer
    const xAOD::CaloClusterContainer* clusContainer = vectorClusterContainer.back();
    if(!clusContainer){
        ATH_MSG_ERROR(" REGTEST: Retrieval of CaloClusterContainer from vector failed");
        return HLT::OK;
    }

    ATH_MSG_DEBUG (clusContainer->size() << " calo clusters in container");
    if(clusContainer->size() < 1) return HLT::OK;

    // Track container from outputTE
    std::vector<const xAOD::TrackParticleContainer*> vectorTrackParticleContainer;
    stat = getFeatures(outputTE, vectorTrackParticleContainer);
    // in case a TrackParticleContainer is retrieved from the TE
    // check that it is not empty
    if (vectorTrackParticleContainer.size() < 1) {
        ATH_MSG_ERROR(" REGTEST: empty TrackParticleContainer from TE");
        return HLT::OK;
    }
    // Get the pointer to last TrackParticleContainer 
    const xAOD::TrackParticleContainer *pTrackParticleContainer = vectorTrackParticleContainer.back();
    if(!pTrackParticleContainer){
        ATH_MSG_ERROR("REGTEST:: Retrieval of TrackParticleContainer failed");
        return HLT::OK;
    }
    ATH_MSG_DEBUG(pTrackParticleContainer->size() << " tracks in container");
    if(pTrackParticleContainer->size() < 1) return HLT::OK;
    // Check the container key

    // Check the keys for the containers
    std::string oldTrkCollKey;
    if ( getStoreGateKey( pTrackParticleContainer, oldTrkCollKey) != HLT::OK) {
        ATH_MSG_ERROR("Failed to get key for TrackContainer");
    }
    else ATH_MSG_DEBUG("Track Collection key in SG: " << oldTrkCollKey);
    std::string clusCollKey;
    if ( getStoreGateKey( clusContainer, clusCollKey) != HLT::OK) {
        ATH_MSG_ERROR("Failed to get key for TrackContainer");
    }
    else ATH_MSG_DEBUG("Cluster Collection key in SG: " << clusCollKey);

    // Record the final Track Particle container in StoreGate
    m_finalTrkPartContainer = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer* aux = new xAOD::TrackParticleAuxContainer();
    m_finalTrkPartContainer->setStore( aux );
    
    std::string trkCollKey = "";
    std::string persKey = "";
    std::string persKeyLink = "TrigGSFTrackFex";
    
    //  msg() << MSG::DEBUG << store()->dump() << endreq;
    stat = getUniqueKey( m_finalTrkPartContainer, trkCollKey, persKey );
    if (stat != HLT::OK) {
        msg() << MSG::DEBUG << "Could not retrieve the cluster collection key" << endreq;
        return stat;
    }
    if (store()->record (m_finalTrkPartContainer, trkCollKey).isFailure()) {
        msg() << MSG::ERROR << "recording TrackParticleContainer with key <" << trkCollKey << "> failed" << endreq;
        delete m_finalTrkPartContainer; // meber of class, need a delete?
        delete aux;
        return HLT::TOOL_FAILURE;
    }
    else  ATH_MSG_DEBUG("recording TrackParticleContainer with key " << trkCollKey);
    if (store()->record (aux, trkCollKey + "Aux.").isFailure()) {
        msg() << MSG::ERROR << "recording TrackParticleAuxContainer with key <" << trkCollKey << "Aux. failed" << endreq;
        delete aux;
        return HLT::TOOL_FAILURE;
    }
    else  ATH_MSG_DEBUG("recording TrackParticleAuxContainer with key " << trkCollKey << "Aux.");
    //
    //create container for slimmed tracks
    //Is this required?
    m_finalTracks = new TrackCollection(0);
    if(store()->record(m_finalTracks, trkCollKey).isFailure()){
        ATH_MSG_ERROR("Unable to create new track container " << trkCollKey);
        return HLT::TOOL_FAILURE;
    }

    ATH_MSG_DEBUG ("Cluster container size: "  << clusContainer->size() );
    ATH_MSG_DEBUG ("Track   container  size: "  << pTrackParticleContainer->size() );

    m_AllTracks = pTrackParticleContainer->size();
    m_AllClusters = clusContainer->size();
    int trackNumber = 0;
    for( const auto *trk : *pTrackParticleContainer){

        int nclus(0);

        uint8_t dummy(0); 
        if( trk->summaryValue(dummy,xAOD::numberOfPixelHits) )
            nclus += dummy;
        if( trk->summaryValue(dummy,xAOD::numberOfSCTHits) )
            nclus += dummy;


        //inner loop on clusters
        for(const auto *clus : *clusContainer){

            if(Select(clus, trk)){
                ATH_MSG_DEBUG ("Track Matched");
                if (timerSvc()) m_toolTimer->start();   // Timer for track refit 
                if(refitTrack(trk).isFailure()) {
                    ATH_MSG_WARNING("Problem in EMBreCollection Builder Refit");
                }
                else {
                    ElementLink<xAOD::TrackParticleContainer> linkToOriginal(*pTrackParticleContainer,trackNumber);
                    xAOD::TrackParticle* gsfTrack = m_finalTrkPartContainer->back();       
                    gsfTrack->auxdata< ElementLink< xAOD::TrackParticleContainer > >( "originalTrackParticle" ) = linkToOriginal;
                    linkToOriginal.toPersistent();
                }
                if (timerSvc()) m_toolTimer->stop();    
                m_SelectedTracks++;
                //if(isTRT) m_SelectedTRTTracks++;
                //else m_SelectedSiTracks++;
                break;
            }
            else{
                ATH_MSG_DEBUG ("Track did not match cluster--not added to collection");
            }
        }//Loop on clusters
        trackNumber++; //increment counter
    }//Loop on tracks
    // Do we need this for trigger?
    /*ATH_MSG_DEBUG ("Final Track container size: "  << m_finalTrkPartContainer->size() );
    // Make readable from INav4mom
    const INavigable4MomentumCollection* theNav4s(0);
    sc = evtStore()->symLink(m_finalTrkPartContainer,theNav4s);
    if (sc.isFailure()){
        ATH_MSG_WARNING("Could not symLink TrackParticleContainer to INavigable4MomentumCollection");
    }  */
    // Build the "uses" relation for the outputTE to the cell container
    std::string aliasKey = "";
    stat = reAttachFeature(outputTE, m_finalTrkPartContainer, aliasKey, persKey );

    if (stat != HLT::OK) {
        msg() << MSG::ERROR
            << "Write of RoI Cluster Container into outputTE failed"
            << endreq;
        return HLT::NAV_ERROR;
    }
    bool pass = false;
    bool result = false;
    // Accept-All mode: temporary patch; should be done with force-accept
    if (m_acceptAll) {
        pass = true;
        if ( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
                << endreq;
        }
    } else {
        pass = false;
        if ( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
                << endreq;
        }
    }
    
    // set output TriggerElement unless acceptAll is set
    if (!m_acceptAll) pass = result;

    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "acceptInput = " << pass << endreq;
    if (timerSvc()) m_totalTimer->stop();    
    return HLT::OK;
}
// ====================================================================
StatusCode TrigGSFTrackFex::refitTrack(const xAOD::TrackParticle* tmpTrkPart){

  int nSiliconHits_trk =0;
  
  uint8_t dummy(0); 
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfSCTHits) )
    nSiliconHits_trk += dummy;
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfPixelHits) )
    nSiliconHits_trk += dummy;
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfSCTOutliers) )
    nSiliconHits_trk += dummy;
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfPixelOutliers) )
    nSiliconHits_trk += dummy;

  ATH_MSG_DEBUG("Number of Silicon hits "<<nSiliconHits_trk);
  //Get the vertex (may be pileup) and the vertex type that this track particle points to

  const xAOD::Vertex* trkVtx(0);
  if (tmpTrkPart->vertexLink().isValid()){ 
    trkVtx = tmpTrkPart->vertex();
  }
    
  //Get the original track that the track particle points to. Clone it in order to assume ownership
  
  const Trk::Track* tmpTrk(0);
  if ( tmpTrkPart->trackLink().isValid() ){
    tmpTrk =  tmpTrkPart->track();
  }
  else
    {
      ATH_MSG_ERROR ("TrackParticle has not Track --  are you running on AOD?");
      return StatusCode::FAILURE;
    }
  
  Trk::Track* trk_refit = 0;
  //
  if( nSiliconHits_trk >= m_MinNoSiHits ) {
    StatusCode status = m_trkRefitTool->refitTrackParticle(tmpTrkPart);
    if (status == StatusCode::SUCCESS){
      ATH_MSG_DEBUG("FIT SUCCESS ");
      m_RefittedTracks++;
      trk_refit = m_trkRefitTool->refittedTrack(); //this is a Trk::Track
      m_summaryTool->updateTrack(*trk_refit);   
    }
    else{
      //We end up here due to a failed fit
      ATH_MSG_DEBUG("FIT FAILED ");
      m_FailedFitTracks++;
      trk_refit = new Trk::Track(*tmpTrk);
    }
  }
  else{
    //We end up here if not enough silicons hits
    ATH_MSG_DEBUG("NO FIT ATTEMPTED");
    m_FailedSiliconRequirFit++;
    trk_refit = new Trk::Track(*tmpTrk);
  }
  //Slim the tracks   
  Trk::Track* slimmed = m_slimTool->slim(*trk_refit);
  if(!slimmed){
    ATH_MSG_ERROR ("TrackSlimming failed, this should never happen !");
    delete trk_refit;
    return StatusCode::FAILURE;
  }
  m_finalTracks->push_back(slimmed);

  // Use the the refitted track and the original vertex to construct a new track particle
  xAOD::TrackParticle* aParticle = m_particleCreatorTool->createParticle( *trk_refit, m_finalTrkPartContainer, trkVtx, xAOD::electron );

  // Try with ElementLink same as HLTID xAODCnv algorithm
  //const ElementLink<TrackCollection> trackLink(*m_tracks, idtr); // Need a counter from the container
  //xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, tpCont);
  //
  delete trk_refit;
  if(aParticle!=0) { //store in containers
    ElementLink<TrackCollection> trackLink( slimmed, *m_finalTracks);
    aParticle->setTrackLink( trackLink );     
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}
// =================================================================
bool TrigGSFTrackFex::Select(const xAOD::CaloCluster*   cluster,
        const xAOD::TrackParticle* track) const
{

    // Removed TRT track from method, but a bool required in extrapolation tool
    bool trkTRT = false; 
    ATH_MSG_DEBUG("EMBremCollectionBuilder::Select()" );
    if (cluster == 0 || track == 0) return false;
    const Trk::Perigee& candidatePerigee = track->perigeeParameters();
    //Get Perigee Parameters
    double  trkPhi = candidatePerigee.parameters()[Trk::phi];
    double  trkEta = candidatePerigee.eta();
    double  r_first=candidatePerigee.position().perp();
    double  z_first=candidatePerigee.position().z();
    //===========================================================//     
    //Get Cluster parameters
    double clusterEta=cluster->etaBE(2);
    bool isEndCap= cluster->inEndcap();
    double Et= cluster->e()/cosh(trkEta);

    //===========================================================//     
    // a few sanity checks
    if (fabs(clusterEta) > 10.0 || fabs(trkEta) > 10.0 || Et <= 0.0) {
        ATH_MSG_DEBUG("FAILS sanity checks :  Track Eta : " << trkEta 
                << ", Cluster Eta " << clusterEta );
        return false;
    }

    //===========================================================//     
    //Calculate corrrected eta and Phi
    double etaclus_corrected = CorrectedEta(clusterEta,z_first,isEndCap);
    double phiRot = PhiROT(Et,trkEta, track->charge(),r_first ,isEndCap)  ;
    double phiRotTrack = PhiROT(track->pt(),trkEta, track->charge(),r_first ,isEndCap);
    //===========================================================//     
    //Calcualate deltaPhis 
    double deltaPhiStd = m_phiHelper.diff(cluster->phiBE(2), trkPhi);
    double trkPhiCorr = m_phiHelper.diff(trkPhi, phiRot);
    double deltaPhi2 = m_phiHelper.diff(cluster->phiBE(2), trkPhiCorr);
    double trkPhiCorrTrack = m_phiHelper.diff(trkPhi, phiRotTrack);
    double deltaPhi2Track = m_phiHelper.diff(cluster->phiBE(2), trkPhiCorrTrack);
    //===========================================================//     

    if (fabs(cluster->etaBE(2) - trkEta) > 2*m_broadDeltaEta && 
            fabs( etaclus_corrected- trkEta) > 2.*m_broadDeltaEta){
        ATH_MSG_DEBUG("FAILS broad window eta match (track eta, cluster eta, cluster eta corrected): ( " 
                << trkEta << ", " << cluster->etaBE(2) <<", "<<etaclus_corrected<<")" );
        return false;
    }
    //if it does not fail the eta cut, does it fail the phi?
    if ( (fabs(deltaPhi2) > 2*m_broadDeltaPhi) && (fabs(deltaPhi2Track) > 2.*m_broadDeltaPhi) 
            && (fabs(deltaPhiStd) > 2*m_broadDeltaPhi)){
        ATH_MSG_DEBUG("FAILS broad window phi match (track phi, phirotCluster , phiRotTrack ,cluster phi): ( " 
                << trkPhi << ", " << phiRot<< ", "<<phiRotTrack<< ", " << cluster->phiBE(2) << ")" );
        return false;
    }

    //Extrapolate from last measurement, since this is before brem fit last measurement is better.
    IEMExtrapolationTools::TrkExtrapDef extrapFrom = IEMExtrapolationTools::fromLastMeasurement;
    std::vector<double>  eta(4, -999.0);
    std::vector<double>  phi(4, -999.0);
    std::vector<double>  deltaEta(4, -999.0);
    std::vector<double>  deltaPhi(4, -999.0);
    if (m_extrapolationTool->getMatchAtCalo (cluster, 
                track, 
                trkTRT,
                Trk::alongMomentum, 
                eta,
                phi,
                deltaEta, 
                deltaPhi, 
                extrapFrom).isFailure()) {return false;}  

    // Selection in narrow eta/phi window
    if(( trkTRT || fabs(deltaEta[2]) < m_narrowDeltaEta ) && 
            deltaPhi[2] < m_narrowDeltaPhi && 
            deltaPhi[2] > -m_narrowDeltaPhiBrem) {
        ATH_MSG_DEBUG("Standard Match success " << deltaPhi[2] );
        return true;
    }
    else if(!trkTRT && fabs(deltaEta[2]) < m_narrowDeltaEta ){ 
        ATH_MSG_DEBUG("Normal matched Failed deltaPhi/deltaEta " 
                << deltaPhi[2] <<" / "<< deltaEta[2]<<", Trying Rescale" );
        //Extrapolate from Perigee Rescaled 
        IEMExtrapolationTools::TrkExtrapDef extrapFrom1 = IEMExtrapolationTools::fromPerigeeRescaled;
        std::vector<double>  eta1(4, -999.0);
        std::vector<double>  phi1(4, -999.0);
        std::vector<double>  deltaEta1(4, -999.0);
        std::vector<double>  deltaPhi1(5, -999.0); // Set size to 5 to store deltaPhiRot
        if (m_extrapolationTool->getMatchAtCalo (cluster, 
                    track, 
                    trkTRT,
                    Trk::alongMomentum, 
                    eta1,
                    phi1,
                    deltaEta1, 
                    deltaPhi1, 
                    extrapFrom1).isFailure()) return false;
        if( fabs(deltaEta1[2]) < m_narrowDeltaEta 
                && deltaPhi1[2] < m_narrowRescale
                && deltaPhi1[2] > -m_narrowRescaleBrem) {
            ATH_MSG_DEBUG("Rescale Match success " << deltaPhi1[2] );
            return true;
        }
        else {
            ATH_MSG_DEBUG("Rescaled matched Failed deltaPhi/deltaEta " 
                    << deltaPhi1[2] <<" / "<< deltaEta1[2] );
            return false;
        }
    }

    ATH_MSG_DEBUG("Matched Failed deltaPhi/deltaEta " << deltaPhi[2] <<" / "<< deltaEta[2]<<",isTRT, "<< trkTRT);
    return false;
}

double TrigGSFTrackFex::CorrectedEta(double clusterEta,double z_first,bool isEndCap) const {
    //===========================================================//
    //These are at the face of the calorimeter
    const  double Rcalo = 1500;
    const  double Zcalo = 3700;
    double Rclus=Rcalo;
    double Zclus=Zcalo;    
    if(clusterEta!=0){
        //eta !=0
        double tanthetaclus=(1/cosh(clusterEta))/tanh(clusterEta); 
        if (isEndCap) {
            Rclus=fabs(Zcalo*(tanthetaclus));
            //Negative Eta ---> negative Z
            if(clusterEta<0){
                Zclus = -Zclus;
            }
        } 	  
        else{
            if(tanthetaclus!=0){
                Zclus=Rcalo/(tanthetaclus);
            }
        }
    }
    else{
        //eta 0
        Zclus=0;
    }
    //===========================================================//    
    //correct for a possible shift in the track perigee wrt to (0,0)
    double thetaclus_corrected=atan2(Rclus,Zclus-z_first);
    double etaclus_corrected = 99999; //if theta =0 or M_PI the eta +-inf this happens when Rclus =0 
    if(Rclus!=0){
        etaclus_corrected = -log(tan(thetaclus_corrected/2));
    }
    //===========================================================//
    return etaclus_corrected;
}

double TrigGSFTrackFex::PhiROT(double Et,double Eta, int charge, double r_first ,bool isEndCap) const{
    //===========================================================//
    //Used in order to derive the formula below
    const  double Rcalo = 1200;
    //===========================================================//
    // correct phi for extrapolation to calo
    double phiRot = 0.0;
    double ecCorr = 1.0;
    if (isEndCap) {
        double ecFactor = 1.0/(0.8*atan(15.0/34.0));
        double sinTheta0 = 2.0*atan(exp(-fabs(Eta)));
        ecCorr = sinTheta0*sqrt(sinTheta0)*ecFactor;
    }
    ////
    double Rscaled =(Rcalo-r_first)/Rcalo;
    phiRot = Rscaled*ecCorr*charge*430./(Et);    
    return  phiRot;
}
