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
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//std includes
#include <algorithm>
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
  ATH_CHECK(m_trkRefitTool.retrieve());
  /* Get the particle creation tool */
  ATH_CHECK(m_particleCreatorTool.retrieve());
  /* Get the track slimming tool */
  ATH_CHECK(m_slimTool.retrieve());
  /* Get the track summary tool */
  ATH_CHECK(m_summaryTool.retrieve());
  /* the extrapolation tool*/
  ATH_CHECK(m_extrapolationTool.retrieve());
  
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
  /*
   * Read in the input
   * All TrackParticles and the selected view
   */
  SG::ReadHandle<xAOD::TrackParticleContainer> trackTES(m_trackParticleContainerKey);
  if(!trackTES.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_trackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }
  SG::ReadHandle<xAOD::TrackParticleContainer> selectedTracks(m_selectedTrackParticleContainerKey);
  if(!selectedTracks.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_selectedTrackParticleContainerKey.key());
    return StatusCode::FAILURE;
  }
  /*
   * Create the final containers to be written out
   */
  SG::WriteHandle<xAOD::TrackParticleContainer> finalTrkPartContainer(m_OutputTrkPartContainerKey);
  ATH_CHECK(finalTrkPartContainer.record(std::make_unique<xAOD::TrackParticleContainer>(),
                                         std::make_unique<xAOD::TrackParticleAuxContainer>()));
  xAOD::TrackParticleContainer* cPtrTrkPart=finalTrkPartContainer.ptr();

  SG::WriteHandle<TrackCollection> finalTracks(m_OutputTrackContainerKey);
  ATH_CHECK(finalTracks.record(std::make_unique<TrackCollection>())); 
  TrackCollection* cPtrTracks=finalTracks.ptr();
  /* 
   * Split TRT-alone from silicon ones
   * For the TRT we can get all the info already
   */
  std::vector<const xAOD::TrackParticle*> siliconTrkTracks;
  siliconTrkTracks.reserve(8); 
  std::vector<TrackWithIndex> trtAloneTrkTracks;
  trtAloneTrkTracks.reserve(8); 
  for(const xAOD::TrackParticle* track : *selectedTracks){
    const Trk::Track* trktrack{nullptr};
    if (  track->trackLink().isValid() ){ 
      trktrack =track->track();
    }
    else{
      ATH_MSG_ERROR ("TrackParticle has not Track --  are you running on AOD?");
      return StatusCode::FAILURE;
    }
    int nSiliconHits_trk =0; 
    uint8_t dummy(0); 
    if( track->summaryValue(dummy,xAOD::numberOfSCTHits) ){
      nSiliconHits_trk += dummy;
    }
    if(track->summaryValue(dummy,xAOD::numberOfPixelHits) ){
      nSiliconHits_trk += dummy;
    }
    ATH_MSG_DEBUG("Number of Silicon hits "<<nSiliconHits_trk);    

    if(nSiliconHits_trk >= m_MinNoSiHits){
      siliconTrkTracks.push_back(track); 
    }else{
      //copy Trk::Track and keep also the index of the original
      trtAloneTrkTracks.emplace_back(
        std::make_unique<Trk::Track>(*trktrack),track->index());
    }
  }
  /* 
   * Refitted  and possible failed fit info
   */
  std::vector<TrackWithIndex> refitted; 
  refitted.reserve(siliconTrkTracks.size());    
  std::vector<TrackWithIndex> failedfit; 
  /*
   * Do the refit and get all the info
   */
  ATH_CHECK(refitTracks(Gaudi::Hive::currentContext(),siliconTrkTracks,refitted,failedfit)); 
  /* 
   * update counters
   * */
  m_FailedFitTracks.fetch_add(failedfit.size(), std::memory_order_relaxed); 
  m_RefittedTracks.fetch_add(refitted.size(), std::memory_order_relaxed);
  m_FailedSiliconRequirFit.fetch_add(trtAloneTrkTracks.size(),std::memory_order_relaxed);
  siliconTrkTracks.clear();
  /*
   * Fill the final collections
   */
  ATH_CHECK(createCollections(refitted,failedfit,trtAloneTrkTracks,
                    cPtrTracks,cPtrTrkPart,trackTES.ptr()));
  ATH_MSG_DEBUG ("Final Track container size: "  << cPtrTrkPart->size() );
  return StatusCode::SUCCESS;
}

StatusCode EMBremCollectionBuilder::refitTracks(const EventContext& ctx,
                                                const std::vector<const xAOD::TrackParticle*>& input,
                                                std::vector<TrackWithIndex>& refitted,
                                                std::vector<TrackWithIndex>& failedfit) const {
  for (const xAOD::TrackParticle* in:input){
    const Trk::Track* track =in->track();
    IegammaTrkRefitterTool::Cache cache{};
    StatusCode status = m_trkRefitTool->refitTrack(ctx,                                                   
                                                   track,
                                                   cache);
    if (status == StatusCode::SUCCESS){
      ATH_MSG_DEBUG("FIT SUCCESS ");
      //this is new track
      refitted.emplace_back(
        std::move(cache.refittedTrack),in->index()
        ); 
    }
    else{
      ATH_MSG_DEBUG("FIT FAILED ");
      //This is copy ctor
      failedfit.emplace_back(
        std::make_unique<Trk::Track>(*track),in->index()
        );
    }    
  }
  return StatusCode::SUCCESS;  
}

StatusCode EMBremCollectionBuilder::createCollections(const std::vector<TrackWithIndex>& refitted,
                                                      const std::vector<TrackWithIndex>& failedfit,
                                                      const std::vector<TrackWithIndex>& trtAlone,
                                                      TrackCollection* finalTracks,                                            
                                                      xAOD::TrackParticleContainer* finalTrkPartContainer,
                                                      const xAOD::TrackParticleContainer* AllTracks) const {
  /*
   * Refitted Treatment
   * The other types are "cloned" this is new
   * so need to update the summary
   */
  for (auto& Info : refitted){
    m_summaryTool->updateTrack(*(Info.track));
    ATH_CHECK(createNew(Info,true,finalTracks,finalTrkPartContainer,AllTracks));
  }
  /*
   * Failed Fit Treatment
   */                                                                 
  for (auto& Info  :  failedfit){
    ATH_CHECK(createNew(Info,true,finalTracks,finalTrkPartContainer,AllTracks));
  }
  /*
   * TRT Treatment
   */                                                                 
  for (auto& Info : trtAlone){
    ATH_CHECK(createNew(Info,false,finalTracks,finalTrkPartContainer,AllTracks));
  }
  return StatusCode::SUCCESS;
} 



StatusCode EMBremCollectionBuilder::createNew(const TrackWithIndex& Info,
                                              bool isSilicon,
                                              TrackCollection* finalTracks,
                                              xAOD::TrackParticleContainer* finalTrkPartContainer,
                                              const xAOD::TrackParticleContainer* AllTracks
                                             ) const{

  Trk::Track track= *(Info.track);
  size_t origIndex = Info.origIndex; 
  const xAOD::TrackParticle* original = AllTracks->at(origIndex);
  /*
   * Create TrackParticle it should be now owned by finalTrkPartContainer
   */
  xAOD::TrackParticle* aParticle=m_particleCreatorTool->createParticle(track,                                   
                                                                       finalTrkPartContainer,                        
                                                                       nullptr,                                      
                                                                       xAOD::electron);
  if (!aParticle){
    ATH_MSG_WARNING("Could not create TrackParticle!!! for Track: " << track);
    return StatusCode::SUCCESS;
  }

  /*
   * Add qoverP from the last measurement
   */
  static const SG::AuxElement::Accessor<float > QoverPLM  ("QoverPLM");
  float QoverPLast(0);
  auto rtsos = track.trackStateOnSurfaces()->rbegin();
  for (;rtsos != track.trackStateOnSurfaces()->rend(); ++rtsos){
    if ((*rtsos)->type(Trk::TrackStateOnSurface::Measurement) 
        && (*rtsos)->trackParameters()!=nullptr
        &&(*rtsos)->measurementOnTrack()!=nullptr
        && !(*rtsos)->measurementOnTrack()->type(Trk::MeasurementBaseType::PseudoMeasurementOnTrack)) {
      QoverPLast  = (*rtsos)->trackParameters()->parameters()[Trk::qOverP];
      break;
    }
  }
  QoverPLM(*aParticle) = QoverPLast;
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
    auto tsos = track.trackStateOnSurfaces()->begin();
    for (;tsos != track.trackStateOnSurfaces()->end(); ++tsos) {
      if ((*tsos)->type(Trk::TrackStateOnSurface::Perigee) && (*tsos)->trackParameters()!=0) {
        float extrapEta(-999.), extrapPhi(-999.);
        const Trk::TrackParameters *perigeeTrackParams(0);
        perigeeTrackParams = (*tsos)->trackParameters();

        const Trk::PerigeeSurface pSurface (perigeeTrackParams->position());
        std::unique_ptr<const Trk::TrackParameters> pTrkPar(
          pSurface.createTrackParameters( perigeeTrackParams->position(), 
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
  //Add an element link back to original Track Particle collection  

  static const SG::AuxElement::Accessor<ElementLink<xAOD::TrackParticleContainer>>tP ("originalTrackParticle");
  ElementLink<xAOD::TrackParticleContainer> linkToOriginal(*AllTracks,origIndex);   	  
  tP(*aParticle) = linkToOriginal;	      

  if(m_doTruth){
    //Add Truth decorations. Copy from the original.
    static const SG::AuxElement::Accessor< ElementLink<xAOD::TruthParticleContainer> >  tPL ("truthParticleLink");
    if(tPL.isAvailable(*original)){
      ElementLink<xAOD::TruthParticleContainer> linkToTruth= tPL(*original);
      tPL(*aParticle) = linkToTruth;	      
    }	 
    static const SG::AuxElement::Accessor<float >  tMP ("truthMatchProbability");
    if(tMP.isAvailable(*original)){
      float originalProbability = tMP(*original);
      tMP(*aParticle)= originalProbability ;
    }
    static const SG::AuxElement::Accessor<int> tT("truthType") ;
    if(tT.isAvailable(*original)){
      int truthType = tT(*original);
      tT(*aParticle) = truthType ;
    }
    static const SG::AuxElement::Accessor<int> tO("truthOrigin") ;
    if(tO.isAvailable(*original)){
      int truthOrigin = tO(*original);
      tO(*aParticle) = truthOrigin ;
    } 
  }//End truth

  //Now  Slim the TrK::Track for writing to disk   
  Trk::Track* slimmed = m_slimTool->slim(track);
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

