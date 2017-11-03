/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     EMBremCollectionBuilder
PACKAGE:  offline/Reconstruction/egamma/egammaTrackTools/EMBremCollectionBuilder

AUTHORS:  Anastopoulos
CREATED:  

PURPOSE:  Performs Brem refit of all tracks 
UPDATE :
**********************************************************************/
#include "EMBremCollectionBuilder.h"
//
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
//
#include "TrkEventPrimitives/VertexType.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "egammaUtils/CandidateMatchHelpers.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
//
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//std includes
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include "CxxUtils/make_unique.h"

#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor
EMBremCollectionBuilder::EMBremCollectionBuilder(const std::string& name, 
						 ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_finalTracks(nullptr),
  m_finalTrkPartContainer(nullptr)
{
  m_AllClusters=0;
  m_AllTracks=0;
  m_AllTRTTracks=0;
  m_AllSiTracks=0;
  m_SelectedTracks=0;
  m_SelectedTRTTracks=0;
  m_SelectedSiTracks=0;
  m_FailedFitTracks=0;
  m_FailedSiliconRequirFit=0;
  m_RefittedTracks=0;
}

// ==================================================================
StatusCode EMBremCollectionBuilder::initialize() {

  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_trackParticleContainerKey.initialize());
  ATH_CHECK(m_OutputTrkPartContainerKey.initialize());
  ATH_CHECK(m_OutputTrackContainerKey.initialize());

  // retrieve the track refitter tool:
  if(m_trkRefitTool.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Unable to retrieve " << m_trkRefitTool);
    return StatusCode::FAILURE;
  } 

  /* Get the particle creation tool */
  if ( m_particleCreatorTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ("Failed to retrieve tool " << m_particleCreatorTool);
    return StatusCode::FAILURE;
  } 
  
  /* Get the track slimming tool */
  if ( m_slimTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Failed to retrieve TrkTrackSlimmingTool tool "<< m_slimTool);
    return StatusCode::FAILURE;
  } 

  /* Get the track summary tool */
  if ( m_summaryTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Failed to retrieve TrkTrackSummaryTool tool "<< m_summaryTool);
    return StatusCode::FAILURE;
  } 
  
  /* the extrapolation tool*/
  if(m_extrapolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("initialize: Cannot retrieve extrapolationTool " << m_extrapolationTool);
    return StatusCode::FAILURE;
  }

  //counters 
  m_AllClusters=0;
  m_AllTracks=0;
  m_AllTRTTracks=0;
  m_AllSiTracks=0;
  m_SelectedTracks=0;
  m_SelectedTRTTracks=0;
  m_SelectedSiTracks=0;
  m_FailedFitTracks=0;
  m_FailedSiliconRequirFit=0;
  m_RefittedTracks=0;

  return StatusCode::SUCCESS;
}  


//*************************************************************************
// Finalize
StatusCode EMBremCollectionBuilder::EMBremCollectionBuilder::finalize(){ 
  
  ATH_MSG_INFO ("AllClusters " << m_AllClusters);
  ATH_MSG_INFO ("AllTracks " << m_AllTracks);
  ATH_MSG_INFO ("AllSiTracks " << m_AllSiTracks);
  ATH_MSG_INFO ("AllTRTTracks " << m_AllTRTTracks);
  ATH_MSG_INFO ("SelectedTracks " << m_SelectedTracks);
  ATH_MSG_INFO ("SelectedSiTracks " << m_SelectedSiTracks);
  ATH_MSG_INFO ("SelectedTRTTracks " << m_SelectedTRTTracks);
  ATH_MSG_INFO ("Not refitted due to Silicon Requirements " << m_FailedSiliconRequirFit);
  ATH_MSG_INFO ("Failed Fit Tracks " << m_FailedFitTracks);
  ATH_MSG_INFO ("RefittedTracks " << m_RefittedTracks);
  
  return StatusCode::SUCCESS;
}

StatusCode EMBremCollectionBuilder::execute()
{
  //======================================================================================================
  StatusCode sc;
  
  // Record the final Track Particle container in StoreGate
  //

  SG::WriteHandle<xAOD::TrackParticleContainer> finalTrkPartContainer(m_OutputTrkPartContainerKey);
  ATH_CHECK(finalTrkPartContainer.record(std::make_unique<xAOD::TrackParticleContainer>(),
					 std::make_unique<xAOD::TrackParticleAuxContainer>()));
  
  m_finalTrkPartContainer = finalTrkPartContainer.ptr();
  //
  //create container for slimmed tracks
  SG::WriteHandle<TrackCollection> finalTracks(m_OutputTrackContainerKey);
  ATH_CHECK(finalTracks.record(std::make_unique<TrackCollection>()));

  m_finalTracks = finalTracks.ptr();
  
  //
  SG::ReadHandle<xAOD::CaloClusterContainer> clusterTES(m_clusterContainerKey);

  // check is only used for serial running; remove when MT scheduler used
  if(!clusterTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve cluster container: "<< m_clusterContainerKey.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::TrackParticleContainer> trackTES(m_trackParticleContainerKey);

  // check is only used for serial running; remove when MT scheduler used
  if(!trackTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_trackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }

  //======================================================================================================
  //Here is the new Logic
  //Loop over tracks and clusters 
  m_AllClusters+=clusterTES->size();
  //
  xAOD::TrackParticleContainer::const_iterator track_iter;
  xAOD::CaloClusterContainer::const_iterator clus_iter;  
  xAOD::TrackParticleContainer::const_iterator track_iter_end=trackTES->end();
  xAOD::CaloClusterContainer::const_iterator clus_iter_end=clusterTES->end();

  ATH_MSG_DEBUG ("Cluster container size: "  << clusterTES->size() );
  ATH_MSG_DEBUG ("Track   container  size: "  <<trackTES->size() );

  track_iter = trackTES->begin();
  for(unsigned int trackNumber = 0; track_iter !=  track_iter_end; ++track_iter,++trackNumber){
    ATH_MSG_DEBUG ("Check Track with Eta "<< (*track_iter)->eta()<< " Phi " << (*track_iter)->phi()<<" Pt " <<(*track_iter)->pt());

    m_AllTracks++;
    bool isTRT=false;
    
    int nclus(0);
    
    uint8_t dummy(0); 
    if( (*track_iter)->summaryValue(dummy,xAOD::numberOfPixelHits) )
      nclus += dummy;
    if( (*track_iter)->summaryValue(dummy,xAOD::numberOfSCTHits) )
      nclus += dummy;
    
    if(nclus<4){
      isTRT = true;
      m_AllTRTTracks++;
    } else{
      isTRT = false;
      m_AllSiTracks++;
    }

    //inner loop on clusters
    clus_iter = clusterTES->begin();
    for( ;clus_iter !=clus_iter_end;  ++clus_iter){
      
      if(Select((*clus_iter), isTRT, (*track_iter))){
        
	ATH_MSG_DEBUG ("Track Matched");
        sc = refitTrack(*track_iter);	    
	if(sc.isFailure()) {
          ATH_MSG_WARNING("Problem in EMBreCollection Builder Refit");
        }	
	else { 	
	  // Add Auxiliary decorations to the GSF Track Particle
	  // Set Element link to original Track Particle	  
          ElementLink<xAOD::TrackParticleContainer> linkToOriginal(*trackTES,trackNumber);
	  xAOD::TrackParticle* gsfTrack = m_finalTrkPartContainer->back();	  
	  static const SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer> >  tP ("originalTrackParticle");
	  tP(*gsfTrack)= linkToOriginal;
		  
	  if(m_doTruth){
	    //Add Truth decorations. Copy from the original.
	    static const SG::AuxElement::Accessor<ElementLink<xAOD::TruthParticleContainer> >  tPL ("truthParticleLink");
	    if(tPL.isAvailable(*(*track_iter))){
	      ElementLink<xAOD::TruthParticleContainer> linkToTruth= tPL(*(*track_iter));
	      tPL(*gsfTrack) = linkToTruth;	      
	      
	      if(!linkToTruth.isValid()){
		ATH_MSG_DEBUG("Cannot create Valid Link to Truth Particle for GSFTrackParticle");
	      }
	    }	 
	    static const SG::AuxElement::Accessor<float >  tMP ("truthMatchProbability");
	    if(tMP.isAvailable(*(*track_iter))){
	      float originalProbability = tMP(*(*track_iter));
	      tMP(*gsfTrack)= originalProbability ;
	    }
	    
	    static const SG::AuxElement::Accessor<int> tT("truthType") ;
	    if(tT.isAvailable(*(*track_iter))){
	      int truthType = tT(*(*track_iter));
	      tT(*gsfTrack) = truthType ;
	    }
	    static const SG::AuxElement::Accessor<int> tO("truthOrigin") ;
	    if(tO.isAvailable(*(*track_iter))){
	      int truthOrigin = tO(*(*track_iter));
	      tO(*gsfTrack) = truthOrigin ;
	    } 
	  }
	}
        m_SelectedTracks++;
        if(isTRT) {m_SelectedTRTTracks++;}
        else {m_SelectedSiTracks++;}
	//The particular track got refitted the moment it matched any cluster break here
        break;
      }
      else{
        ATH_MSG_DEBUG ("Track did not match cluster--not added to collection");
      }
    }//Loop on clusters
  }//Loop on tracks
  
  ATH_MSG_DEBUG ("Final Track container size: "  << m_finalTrkPartContainer->size() );
  // Make readable from INav4mom
  const INavigable4MomentumCollection* theNav4s(0);
  sc = evtStore()->symLink(m_finalTrkPartContainer,theNav4s);
  if (sc.isFailure()){
    ATH_MSG_WARNING("Could not symLink TrackParticleContainer to INavigable4MomentumCollection");
  }  
  //======================================================================================================
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMBremCollectionBuilder::refitTrack(const xAOD::TrackParticle* tmpTrkPart){

  int nSiliconHits_trk =0; 
  uint8_t dummy(0); 
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfSCTHits) ){
    nSiliconHits_trk += dummy;
  }
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfPixelHits) ){
    nSiliconHits_trk += dummy;
  }
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfSCTOutliers) ){
    nSiliconHits_trk += dummy;
  }
  if( tmpTrkPart->summaryValue(dummy,xAOD::numberOfPixelOutliers) ){
    nSiliconHits_trk += dummy;
  }
  ATH_MSG_DEBUG("Number of Silicon hits "<<nSiliconHits_trk);    

  //Get the original track that the track particle points to. 
  const Trk::Track* tmpTrk(0);
  if ( tmpTrkPart->trackLink().isValid() ){
    tmpTrk =  tmpTrkPart->track();
  }
  else{
    ATH_MSG_ERROR ("TrackParticle has not Track --  are you running on AOD?");
    return StatusCode::FAILURE;
  }

  //Setup the Trk::Track Refit 
  std::unique_ptr<Trk::Track> trk_refit; 
  if( nSiliconHits_trk >= m_MinNoSiHits ) {
    StatusCode status = m_trkRefitTool->refitTrackParticle(tmpTrkPart);
    if (status == StatusCode::SUCCESS){
      ATH_MSG_DEBUG("FIT SUCCESS ");
      m_RefittedTracks++;
      trk_refit.reset(m_trkRefitTool->refittedTrack()); //this is a Trk::Track
      m_summaryTool->updateTrack(*trk_refit);   
    }
    else{
      //We end up here due to a failed fit
      ATH_MSG_DEBUG("FIT FAILED ");
      m_FailedFitTracks++;
      trk_refit.reset(new Trk::Track(*tmpTrk));
    }
  }
  else{
    //We end up here if not enough silicons hits
    ATH_MSG_DEBUG("NO FIT ATTEMPTED");
    m_FailedSiliconRequirFit++;
    trk_refit.reset(new Trk::Track(*tmpTrk));
  }
  //Refit Trk::Track has been created

  //Get the vertex (may be pileup) that this track particle points to
  const xAOD::Vertex* trkVtx(0);
  if (tmpTrkPart->vertexLink().isValid()){ 
    trkVtx = tmpTrkPart->vertex();
  }

  // Use the the Refitted Trk::Track and the original vertex to construct a new TrackParticle
  xAOD::TrackParticle* aParticle = m_particleCreatorTool->createParticle( *trk_refit, m_finalTrkPartContainer, trkVtx, xAOD::electron );


  //If no TrackParticle , then ERROR
  if (!aParticle){
    ATH_MSG_ERROR("Could not create TrackParticle, this should never happen !");
    return StatusCode::FAILURE;
  }

  //Set Vertex Link
  aParticle->setVertexLink(tmpTrkPart->vertexLink());         

   
  //Additional info, e/gamma  using the full  Trk::Track

  //Save extrapolated perigee to calo (eta,phi) for later usage in supercluster algorithm.
  static const SG::AuxElement::Accessor<float> pgExtrapEta ("perigeeExtrapEta");
  static const SG::AuxElement::Accessor<float> pgExtrapPhi ("perigeeExtrapPhi");  
  float perigeeExtrapEta(-999.), perigeeExtrapPhi(-999.);

  auto tsos = trk_refit->trackStateOnSurfaces()->begin();
  for (;tsos != trk_refit->trackStateOnSurfaces()->end(); ++tsos) {
    if ((*tsos)->type(Trk::TrackStateOnSurface::Perigee) && (*tsos)->trackParameters()!=0) {
      float extrapEta(-999.), extrapPhi(-999.);
      const Trk::TrackParameters *perigeeTrackParams(0);
      perigeeTrackParams = (*tsos)->trackParameters();
      
      const Trk::PerigeeSurface pSurface (perigeeTrackParams->position());
      std::unique_ptr<const Trk::TrackParameters> pTrkPar(pSurface.createTrackParameters( perigeeTrackParams->position(), perigeeTrackParams->momentum().unit()*1.e9, +1, 0));
      //Do the straight-line extrapolation.	  
      bool hitEM2 = m_extrapolationTool->getEtaPhiAtCalo(pTrkPar.get(), &extrapEta, &extrapPhi);
      if (hitEM2) {
	perigeeExtrapEta = extrapEta;
	perigeeExtrapPhi = extrapPhi;
      } else {
	ATH_MSG_WARNING("Extrapolation to EM2 failed!");
      }
      break;
    }
  }
  pgExtrapEta(*aParticle) = perigeeExtrapEta;    
  pgExtrapPhi(*aParticle) = perigeeExtrapPhi;
  
  //Add qoverP for the last measurement
  static const SG::AuxElement::Accessor<float > QoverPLM  ("QoverPLM");
  float QoverPLast(0);
  auto rtsos = trk_refit->trackStateOnSurfaces()->rbegin();
  for (;rtsos != trk_refit->trackStateOnSurfaces()->rend(); ++rtsos){
    if ((*rtsos)->type(Trk::TrackStateOnSurface::Measurement) 
	&& (*rtsos)->trackParameters()!=0 
	&&(*rtsos)->measurementOnTrack()!=0 
	&& !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*rtsos)->measurementOnTrack())) {
      QoverPLast  = (*rtsos)->trackParameters()->parameters()[Trk::qOverP];
      break;
    }
  }
  QoverPLM(*aParticle) = QoverPLast;

  //Now  Slim the TrK::Track for writing to disk   
  Trk::Track* slimmed = m_slimTool->slim(*trk_refit);
    
  if(!slimmed){
    ATH_MSG_WARNING ("TrackSlimming failed");
    ElementLink<TrackCollection> dummy;
    aParticle->setTrackLink(dummy);     
  }else{
    m_finalTracks->push_back(slimmed);
    ElementLink<TrackCollection> trackLink( slimmed, *m_finalTracks);
    aParticle->setTrackLink( trackLink );     
  }

  return StatusCode::SUCCESS;
}

// =================================================================
bool EMBremCollectionBuilder::Select(const xAOD::CaloCluster*   cluster,
                                     bool                       trkTRT,
                                     const xAOD::TrackParticle* track) const
{

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
  if(trkTRT){
    Et = cluster->et();
  }

  //===========================================================//     
  // a few sanity checks
  if (fabs(clusterEta) > 10.0 || fabs(trkEta) > 10.0 || Et <= 0.0) {
    ATH_MSG_DEBUG("FAILS sanity checks :  Track Eta : " << trkEta 
		  << ", Cluster Eta " << clusterEta );
    return false;
  }
  //===========================================================//     
  //Calculate corrrected eta and Phi
  double etaclus_corrected = CandidateMatchHelpers::CorrectedEta(clusterEta,z_first,isEndCap);
  double phiRot = CandidateMatchHelpers::PhiROT(Et,trkEta, track->charge(),r_first ,isEndCap)  ;
  double phiRotTrack = CandidateMatchHelpers::PhiROT(track->pt(),trkEta, track->charge(),r_first ,isEndCap)  ;
  //===========================================================//     
  //Calcualate deltaPhis 
  double deltaPhiStd = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhi);
  double trkPhiCorr = P4Helpers::deltaPhi(trkPhi, phiRot);
  double deltaPhi2 = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorr);
  double trkPhiCorrTrack = P4Helpers::deltaPhi(trkPhi, phiRotTrack);
  double deltaPhi2Track = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorrTrack);
  //===========================================================//     
 
  if ((!trkTRT)&& fabs(cluster->etaBE(2) - trkEta) > 2*m_broadDeltaEta && 
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


