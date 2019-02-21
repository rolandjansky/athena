/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/********************************************************************
NAME:     EMBremCollectionBuilder
PACKAGE:  offline/Reconstruction/egamma/egammaTrackTools/EMBremCollectionBuilder

AUTHORS:  Anastopoulos
CREATED:  

PURPOSE:  Performs Brem refit for silicon tracks, copies over TRT-standalone
UPDATE :
 **********************************************************************/
#include "EMBremCollectionBuilder.h"
//
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrackLink/ITrackLink.h"
#include "AthenaKernel/errorcheck.h"
//
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//std includes
#include <algorithm>
#include <cmath>
#include <memory>

EMBremCollectionBuilder::EMBremCollectionBuilder(const std::string& name, 
                                                 ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode EMBremCollectionBuilder::initialize() {

  ATH_CHECK(m_selectedTrackParticleContainerKey.initialize());
  ATH_CHECK(m_trackParticleContainerKey.initialize());
  ATH_CHECK(m_OutputTrkPartContainerKey.initialize());
  ATH_CHECK(m_OutputTrackContainerKey.initialize());

  /* retrieve the track refitter tool*/
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
  return StatusCode::SUCCESS;
}  

StatusCode EMBremCollectionBuilder::EMBremCollectionBuilder::finalize(){ 


  ATH_MSG_INFO ("Not refitted due to Silicon Requirements " << m_FailedSiliconRequirFit);
  ATH_MSG_INFO ("Failed Fit Tracks " << m_FailedFitTracks);
  ATH_MSG_INFO ("RefittedTracks " << m_RefittedTracks);

  return StatusCode::SUCCESS;
}

StatusCode EMBremCollectionBuilder::execute()
{

  // Record the final Track Particle container in StoreGate
  SG::WriteHandle<xAOD::TrackParticleContainer> finalTrkPartContainer(m_OutputTrkPartContainerKey);
  ATH_CHECK(finalTrkPartContainer.record(std::make_unique<xAOD::TrackParticleContainer>(),
                                         std::make_unique<xAOD::TrackParticleAuxContainer>()));
  xAOD::TrackParticleContainer* cPtrTrkPart=finalTrkPartContainer.ptr();
  //create container for the final slimmed Trk::Tracks
  SG::WriteHandle<TrackCollection> finalTracks(m_OutputTrackContainerKey);
  ATH_CHECK(finalTracks.record(std::make_unique<TrackCollection>())); 
  TrackCollection* cPtrTracks=finalTracks.ptr();

  //Read input
  SG::ReadHandle<xAOD::TrackParticleContainer> trackTES(m_trackParticleContainerKey);
  if(!trackTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_trackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ("Track Particle container  size: "  <<trackTES->size() );

  SG::ReadHandle<xAOD::TrackParticleContainer> selectedTracks(m_selectedTrackParticleContainerKey);
  if(!selectedTracks.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_selectedTrackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ("Selected Track Particle container  size: "  <<selectedTracks->size() );

  /*
   * Struct of unsigned int, local varibale of this method
   * Initialization of counters is 0
   */
  localCounter locCounter; 
  //Loop over the selected input tracks 
  xAOD::TrackParticleContainer::const_iterator track_iter=selectedTracks->begin();
  xAOD::TrackParticleContainer::const_iterator track_iter_end=selectedTracks->end();
  for(; track_iter !=  track_iter_end; ++track_iter){

    const xAOD::TrackParticle* track=(*track_iter); 
    ATH_MSG_DEBUG ("Attempt to Refit Track with Eta "<< track->eta()
                   << " Phi " << track->phi()<<" Pt " <<track->pt());
    //Try to refit, if failded move to next one
    if(refitTrack(track,cPtrTracks,cPtrTrkPart,locCounter).isFailure()) {
      ATH_MSG_WARNING("Problem in EMBreCollection Builder Refit");
      continue;
    }
    xAOD::TrackParticle* gsfTrack = cPtrTrkPart->back();	  

    //Add an element link back to original Track Particle collection  
    static const SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer> >  tP ("originalTrackParticle");
    ElementLink<xAOD::TrackParticleContainer> linkToOriginal(*trackTES,track->index());   	  
    tP(*gsfTrack) = linkToOriginal;	      

    if(m_doTruth){
      //Add Truth decorations. Copy from the original.
      static const SG::AuxElement::Accessor<ElementLink<xAOD::TruthParticleContainer> >  tPL ("truthParticleLink");
      if(tPL.isAvailable(*(track))){
        ElementLink<xAOD::TruthParticleContainer> linkToTruth= tPL(*(track));
        tPL(*gsfTrack) = linkToTruth;	      
      }	 
      static const SG::AuxElement::Accessor<float >  tMP ("truthMatchProbability");
      if(tMP.isAvailable(*(track))){
        float originalProbability = tMP(*(track));
        tMP(*gsfTrack)= originalProbability ;
      }
      static const SG::AuxElement::Accessor<int> tT("truthType") ;
      if(tT.isAvailable(*(track))){
        int truthType = tT(*(track));
        tT(*gsfTrack) = truthType ;
      }
      static const SG::AuxElement::Accessor<int> tO("truthOrigin") ;
      if(tO.isAvailable(*(track))){
        int truthOrigin = tO(*(track));
        tO(*gsfTrack) = truthOrigin ;
      } 
    }//End truth
  }//Loop over tracks
  ATH_MSG_DEBUG ("Final Track container size: "  << cPtrTrkPart->size() );

  /*
   * Increment the global counters
   */
  m_FailedFitTracks.fetch_add(locCounter.failedFitTracks, std::memory_order_relaxed); 
  m_RefittedTracks.fetch_add(locCounter.refittedTracks, std::memory_order_relaxed);
  m_FailedSiliconRequirFit.fetch_add(locCounter.failedSiliconRequirFit,std::memory_order_relaxed);

  return StatusCode::SUCCESS;
}


StatusCode EMBremCollectionBuilder::refitTrack(const xAOD::TrackParticle* tmpTrkPart,
                                               TrackCollection* finalTracks,
                                               xAOD::TrackParticleContainer* finalTrkPartContainer,
                                               localCounter& counter) const{


  //No output SG Collections , should never happen  
  if(!finalTrkPartContainer ||!finalTracks ){
    ATH_MSG_ERROR ("No GSF TrackParticle or Trk::Track Container ");
    return StatusCode::FAILURE;
  }
  //Get the original track that the track particle points to. 
  const Trk::Track* tmpTrk(0);
  if ( tmpTrkPart->trackLink().isValid() ){
    tmpTrk =  tmpTrkPart->track();
  }
  else{
    ATH_MSG_ERROR ("TrackParticle has not Track --  are you running on AOD?");
    return StatusCode::FAILURE;
  }

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

  bool isSilicon=(nSiliconHits_trk >= m_MinNoSiHits);
  //Setup the Trk::Track Refit 
  std::unique_ptr<Trk::Track> trk_refit; 
  if( isSilicon ) {
    IegammaTrkRefitterTool::Cache cache{};
    StatusCode status = m_trkRefitTool->refitTrackParticle(tmpTrkPart,cache);
    if (status == StatusCode::SUCCESS){
      ATH_MSG_DEBUG("FIT SUCCESS ");
      ++(counter.refittedTracks); 
      trk_refit.reset(cache.refittedTrack.release()); 
      m_summaryTool->updateTrack(*trk_refit);   
    }
    else{
      //We end up here due to a failed fit
      ATH_MSG_DEBUG("FIT FAILED ");
      ++(counter.failedFitTracks);
      trk_refit.reset(new Trk::Track(*tmpTrk));
    }
  }
  else{
    //We end up here if not enough silicons hits
    ATH_MSG_DEBUG("NO FIT ATTEMPTED");
    ++(counter.failedSiliconRequirFit);
    trk_refit.reset(new Trk::Track(*tmpTrk));
  }
  //Refit Trk::Track has been created

  // Create TrackParticle from the refitted Trk::Track
  xAOD::TrackParticle* aParticle = m_particleCreatorTool->createParticle( *trk_refit, finalTrkPartContainer, nullptr, xAOD::electron );
  if (!aParticle){
    ATH_MSG_ERROR("Could not create TrackParticle, this should never happen !");
    return StatusCode::FAILURE;
  }
 
  /*
   * Additional info for  
   * internal e/gamma usage via the full Trk::Track
   * Save extrapolated perigee to calo (eta,phi) for later usage in supercluster algorithm.
   * Only for silicon tracks as in only needed in this case
   */
  static const SG::AuxElement::Accessor<float> pgExtrapEta ("perigeeExtrapEta");
  static const SG::AuxElement::Accessor<float> pgExtrapPhi ("perigeeExtrapPhi");  
  float perigeeExtrapEta(-999.), perigeeExtrapPhi(-999.);
  if(isSilicon){
    auto tsos = trk_refit->trackStateOnSurfaces()->begin();
    for (;tsos != trk_refit->trackStateOnSurfaces()->end(); ++tsos) {
      if ((*tsos)->type(Trk::TrackStateOnSurface::Perigee) && (*tsos)->trackParameters()!=0) {
        float extrapEta(-999.), extrapPhi(-999.);
        const Trk::TrackParameters *perigeeTrackParams(0);
        perigeeTrackParams = (*tsos)->trackParameters();

        const Trk::PerigeeSurface pSurface (perigeeTrackParams->position());
        std::unique_ptr<const Trk::TrackParameters> pTrkPar(pSurface.createTrackParameters( perigeeTrackParams->position(), 
                                                                                            perigeeTrackParams->momentum().unit()*1.e9, +1, 0));
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
  }
  pgExtrapEta(*aParticle) = perigeeExtrapEta;    
  pgExtrapPhi(*aParticle) = perigeeExtrapPhi;
 
  /*
   * Add qoverP from the last measurement
   */
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
    finalTracks->push_back(slimmed);
    ElementLink<TrackCollection> trackLink( slimmed, *finalTracks);
    aParticle->setTrackLink( trackLink );     
  }
  return StatusCode::SUCCESS;
}

