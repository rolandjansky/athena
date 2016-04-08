/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleCnvAlg.cxx 298303 2013-12-05 08:41:30Z emoyse $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"


// Local include(s):
#include "TrackParticleCnvAlg.h"
#include "xAODTrackingCnv/ITrackCollectionCnvTool.h"
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"

namespace xAODMaker {

  TrackParticleCnvAlg::TrackParticleCnvAlg( const std::string& name,
					    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
      m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool"),
      m_truthClassifier("MCTruthClassifier/MCTruthClassifier"),
      m_TrackCollectionCnvTool( "xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool", this ),
      m_RecTrackParticleContainerCnvTool( "xAODMaker::RecTrackParticleContainerCnvTool/RecTrackParticleContainerCnvTool", this ),
      m_truthParticleLinkVec(0),
      m_numEvents(0),
      m_nTracksProcessed(0),
      m_nTrackParticlesCreated(0),
      
      m_numberOfBLayerHits(0),             
      m_numberOfBLayerSharedHits(0),       
      m_numberOfBLayerOutliers(0),
      
      m_numberOfContribPixelLayers(0),
      m_numberOfPixelHits(0),              
      m_numberOfPixelSharedHits(0),        
      m_numberOfPixelHoles(0),             
      m_numberOfGangedPixels(0),
      m_numberOfGangedFlaggedFakes(0),      
      
      m_numberOfSCTHits(0),                
      m_numberOfSCTSharedHits(0),          
      m_numberOfSCTHoles(0),               
      m_numberOfSCTDoubleHoles(0),         
      m_numberOfTRTHits(0),                
      m_numberOfTRTXenonHits(0),                
      m_numberOfTRTHighThresholdHits(0),   
      m_numberOfTRTOutliers(0),            
      m_numberOfTRTHighThresholdOutliers(0),
      m_numberOfOutliersOnTrack(0),
      
      m_numberOfPixelOutliers(0),
      m_numberOfPixelDeadSensors(0),
      m_numberOfPixelSpoiltHits(0), 
      m_numberOfBlayersMissed(0),
      
      m_numberOfSCTOutliers(0),
      m_numberOfSCTDeadSensors(0),
      m_numberOfSCTSpoiltHits(0),   
      m_numberOfTRTHoles(0),        
      m_numberOfTRTDeadStraws(0),   
      m_numberOfTRTTubeHits(0)
  {
    declareProperty( "MCTruthClassifier",       m_truthClassifier);
    declareProperty( "AODContainerName",        m_aodContainerName = "TrackParticleCandidate" );
    declareProperty( "xAODContainerName",       m_xaodContainerName = "ConvertedTrackParticleCandidate" );
    declareProperty( "TrackParticleCreator",    m_particleCreator );
    declareProperty( "AddTruthLink",            m_addTruthLink = false );
    declareProperty( "AODTruthContainerName",   m_aodTruthContainerName = "" );
    declareProperty( "TrackTruthContainerName", m_trackTruthContainerName = "" );
    declareProperty( "xAODTruthLinkVector",     m_truthLinkVecName="xAODTruthLinks");
    declareProperty( "TrackContainerName",      m_aodTrackContainerName = "Tracks" );
    declareProperty( "xAODTrackParticlesFromTracksContainerName",      m_xaodTracksContainerName = "InDetTrackParticles" );   
    declareProperty( "ConvertTrackParticles",   m_convertAODTrackParticles = true );   
    declareProperty( "ConvertTracks",           m_convertTracks = false ); 
    declareProperty( "PrintIDSummaryInfo",      m_IdOutputInfo = false ); 
    declareProperty( "TrackCollectionCnvTool",                 m_TrackCollectionCnvTool );
    declareProperty( "RecTrackParticleContainerCnvTool",       m_RecTrackParticleContainerCnvTool );
  }

  StatusCode TrackParticleCnvAlg::initialize() {

    ATH_MSG_DEBUG( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_DEBUG( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_DEBUG( "xAODContainerName = " << m_xaodContainerName );
    ATH_CHECK(m_particleCreator.retrieve());
    ATH_CHECK(m_truthClassifier.retrieve());
    ATH_CHECK( m_TrackCollectionCnvTool.retrieve() );
    ATH_CHECK( m_RecTrackParticleContainerCnvTool.retrieve() );
    // to preserve the inisialised parameters of the ParticleCreatorTool:
    ATH_CHECK( m_TrackCollectionCnvTool->setParticleCreatorTool( &m_particleCreator ) );
    ATH_CHECK( m_RecTrackParticleContainerCnvTool->setParticleCreatorTool( &m_particleCreator ) );
  

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrackParticleCnvAlg::execute() {

    m_numEvents++; 

    const Rec::TrackParticleContainer* aod=0;
    // Retrieve the AOD particles:
    if (m_convertAODTrackParticles){
      aod = evtStore()->tryConstRetrieve<Rec::TrackParticleContainer>(m_aodContainerName);
      if (!aod) {
        ATH_MSG_DEBUG("No TrackParticleContainer with key " << m_aodContainerName << " found. Do nothing.");
        return StatusCode::SUCCESS;
      } else {
        ATH_MSG_VERBOSE("Got TrackParticleContainer with key " << m_aodContainerName << " found.");
      }
    }

    const TrackCollection* tracks=0;
    // Retrieve the Tracks:
    if (m_convertTracks){
      tracks = evtStore()->tryConstRetrieve<TrackCollection>(m_aodTrackContainerName);
      if (!tracks) {
        ATH_MSG_DEBUG("No TrackCollection with key " << m_aodTrackContainerName << " found. Do nothing.");
        return StatusCode::SUCCESS;
      }  else {
        ATH_MSG_VERBOSE("Got TrackCollection with key " << m_aodTrackContainerName << " found.");
      }
    }

    const TrackParticleTruthCollection* aodTruth = 0;
    const TrackTruthCollection* trackTruth = 0;
    if( m_addTruthLink ){
      if (m_convertAODTrackParticles){
        aodTruth = evtStore()->tryConstRetrieve<TrackParticleTruthCollection>(m_aodTruthContainerName);
        if (!aodTruth) {
          ATH_MSG_WARNING("No TrackParticleTruthCollection with key " << m_aodTruthContainerName << " found. Do nothing.");
          return StatusCode::SUCCESS;
        }
      }
      if (m_convertTracks){
        trackTruth = evtStore()->tryConstRetrieve<TrackTruthCollection>(m_trackTruthContainerName);
        if (!trackTruth) {
          ATH_MSG_WARNING("No DetailedTrackTruthCollection with key " << m_trackTruthContainerName << " found. Do nothing.");
          return StatusCode::SUCCESS;
        }
      }

      m_truthParticleLinkVec = evtStore()->tryConstRetrieve<xAODTruthParticleLinkVector>(m_truthLinkVecName);
      if (!m_truthParticleLinkVec) {
        ATH_MSG_WARNING("No xAODTruthParticleLinkVector with key " << m_truthLinkVecName << " found. Do nothing.");
        return StatusCode::SUCCESS;
      }
    }

    if (m_convertTracks)              convert(*tracks, trackTruth, m_xaodTracksContainerName, m_TrackCollectionCnvTool);
    if (m_convertAODTrackParticles)   convert(*aod,    aodTruth,   m_xaodContainerName, m_RecTrackParticleContainerCnvTool); 

    return StatusCode::SUCCESS;

  }

  template<typename CONT, typename TRUTHCONT, typename CONVTOOL>
  int TrackParticleCnvAlg::convert(const CONT& container, const TRUTHCONT& truth, const std::string& xaodName, CONVTOOL& conv_tool){    
    // Create the xAOD container and its auxiliary store:
   
    xAOD::TrackParticleContainer* xaod = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer* aux = new xAOD::TrackParticleAuxContainer();
    
    if (evtStore()->contains<xAOD::TrackParticleContainer> (xaodName)) {  
      CHECK(evtStore()->overwrite(aux, xaodName+"Aux.", true, false));
      xaod->setStore( aux );
      CHECK(evtStore()->overwrite(xaod,xaodName, true, false));
      ATH_MSG_DEBUG( "Overwrote TrackParticles with key: " << xaodName );
    }    
    else{
      CHECK(evtStore()->record(aux, xaodName+"Aux."));
      xaod->setStore(aux);
      CHECK(evtStore()->record(xaod, xaodName)); 
      ATH_MSG_DEBUG( "Recorded TrackParticles with key: " << xaodName );
    }

    // convert the track containers separately with the converting tools that are also used by TrigHLTtoxAODTool
    ATH_MSG_DEBUG( "calling the converting tool for " << xaodName );
    if( conv_tool->convert( &container, xaod ).isFailure() ) {
      ATH_MSG_ERROR("Couldn't convert aod to xaod (" << xaodName << ") with the converting tool");
      return -1;
    }

    // Create the xAOD objects:
    typename CONT::const_iterator itr = container.begin();
    typename CONT::const_iterator end = container.end();
    xAOD::TrackParticleContainer::iterator itr_xaod = xaod->begin();
    xAOD::TrackParticleContainer::iterator end_xaod = xaod->end();

    // loop over AOD and converted xAOD for summary info and truth links
    for( ;itr!=end;++itr ) {
      //protect if something went wrong and there is no converted xaod equivalent
      if( itr_xaod == end_xaod ) {
      	ATH_MSG_ERROR("No xAOD equivalent of the AOD track was found");
      	return -1;
      }

      m_nTracksProcessed++;

      // Create the xAOD object:
      if (!(*itr)) {
        ATH_MSG_WARNING("WTaF? Empty element in AOD container!");
        continue;
      }
      if (!(*itr_xaod)) {
	ATH_MSG_WARNING("WTaF? Empty element in xAOD container!");
	continue;
      }

      //      xAOD::TrackParticle* particle = createParticle(*xaod, container, **itr);
      xAOD::TrackParticle* particle = 0;
      particle = *itr_xaod;

      if(!particle){
	ATH_MSG_WARNING("Failed to get an xAOD::TrackParticle");
	continue;
      }

      m_nTrackParticlesCreated++;
      //
      // --------- statistics
      //
      if (m_IdOutputInfo) {

	// integer entries in the summary can be -1 or -999, strange I know but that is the design ...
        uint8_t numberOfBLayerHits=0;
        if (particle->summaryValue(numberOfBLayerHits,xAOD::numberOfBLayerHits))               m_numberOfBLayerHits += numberOfBLayerHits;                           
        uint8_t numberOfBLayerSharedHits=0;
        if (particle->summaryValue(numberOfBLayerSharedHits,xAOD::numberOfBLayerSharedHits))         m_numberOfBLayerSharedHits        += numberOfBLayerSharedHits;               
	uint8_t numberOfBLayerOutliers=0;  
        if (particle->summaryValue(numberOfBLayerOutliers,xAOD::numberOfBLayerOutliers))           m_numberOfBLayerOutliers          += numberOfBLayerOutliers;          
	uint8_t numberOfContribPixelLayers = 0;
        if (particle->summaryValue(numberOfContribPixelLayers,xAOD::numberOfContribPixelLayers))       m_numberOfContribPixelLayers      += numberOfContribPixelLayers;      
	uint8_t numberOfPixelHits = 0; 
        if (particle->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits))                m_numberOfPixelHits               +=  numberOfPixelHits;
        uint8_t numberOfPixelSharedHits = 0;
        if (particle->summaryValue(numberOfPixelSharedHits,xAOD::numberOfPixelSharedHits))          m_numberOfPixelSharedHits         += numberOfPixelSharedHits;                 
	uint8_t  numberOfPixelHoles = 0;
        if (particle->summaryValue(numberOfPixelHoles,xAOD::numberOfPixelHoles))               m_numberOfPixelHoles              += numberOfPixelHoles;                           
	uint8_t numberOfGangedPixels = 0;
        if (particle->summaryValue(numberOfGangedPixels,xAOD::numberOfGangedPixels))             m_numberOfGangedPixels            += numberOfGangedPixels;            
	uint8_t numberOfGangedFlaggedFakes = 0;  
        if (particle->summaryValue(numberOfGangedFlaggedFakes,xAOD::numberOfGangedFlaggedFakes))       m_numberOfGangedFlaggedFakes      += numberOfGangedFlaggedFakes;            
	uint8_t numberOfSCTHits = 0;                                                                                                                                                              
        if (particle->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits))                  m_numberOfSCTHits                 += numberOfSCTHits;                                 
	uint8_t numberOfSCTSharedHits = 0; 
        if (particle->summaryValue(numberOfSCTSharedHits,xAOD::numberOfSCTSharedHits))            m_numberOfSCTSharedHits           += numberOfSCTSharedHits;                     
        uint8_t numberOfSCTHoles = 0;
        if (particle->summaryValue(numberOfSCTHoles,xAOD::numberOfSCTHoles))                 m_numberOfSCTHoles                += numberOfSCTHoles;
        uint8_t numberOfSCTDoubleHoles = 0;
        if (particle->summaryValue(numberOfSCTDoubleHoles,xAOD::numberOfSCTDoubleHoles))           m_numberOfSCTDoubleHoles          += numberOfSCTDoubleHoles;                   
	uint8_t numberOfTRTHits = 0;
        if (particle->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits))                  m_numberOfTRTHits                 += numberOfTRTHits;                                 
	uint8_t numberOfTRTXenonHits = 0;
        if (particle->summaryValue(numberOfTRTXenonHits,xAOD::numberOfTRTXenonHits))             m_numberOfTRTXenonHits            +=  numberOfTRTXenonHits;                                 
        uint8_t numberOfTRTHighThresholdHits = 0;
        if (particle->summaryValue(numberOfTRTHighThresholdHits,xAOD::numberOfTRTHighThresholdHits))     m_numberOfTRTHighThresholdHits    += numberOfTRTHighThresholdHits;       
	uint8_t numberOfTRTOutliers = 0;
        if (particle->summaryValue(numberOfTRTOutliers,xAOD::numberOfTRTOutliers))              m_numberOfTRTOutliers             += numberOfTRTOutliers;                         
	uint8_t numberOfTRTHighThresholdOutliers = 0; 
        if (particle->summaryValue(numberOfTRTHighThresholdOutliers,xAOD::numberOfTRTHighThresholdOutliers)) m_numberOfTRTHighThresholdOutliers+= numberOfTRTHighThresholdOutliers;
	uint8_t numberOfOutliersOnTrack = 0;
        if (particle->summaryValue(numberOfOutliersOnTrack,xAOD::numberOfOutliersOnTrack))          m_numberOfOutliersOnTrack         += numberOfOutliersOnTrack;                 
	uint8_t numberOfPixelOutliers = 0;
        if (particle->summaryValue(numberOfPixelOutliers,xAOD::numberOfPixelOutliers))            m_numberOfPixelOutliers    += numberOfPixelOutliers;
	uint8_t numberOfPixelDeadSensors = 0;
        if (particle->summaryValue(numberOfPixelDeadSensors,xAOD::numberOfPixelDeadSensors))         m_numberOfPixelDeadSensors +=numberOfPixelDeadSensors ;
	uint8_t numberOfPixelSpoiltHits = 0;
        if (particle->summaryValue( numberOfPixelSpoiltHits,xAOD::numberOfPixelSpoiltHits))          m_numberOfPixelSpoiltHits  +=  numberOfPixelSpoiltHits; 

        uint8_t expectBLayerHit = 0;
        if (particle->summaryValue(expectBLayerHit,xAOD::expectBLayerHit) &&
            expectBLayerHit>0 && 
            (particle->summaryValue(numberOfBLayerHits,xAOD::numberOfBLayerHits)) && 
            (particle->summaryValue(numberOfBLayerOutliers,xAOD::numberOfBLayerOutliers)) && 
            (particle->summaryValue(numberOfContribPixelLayers,xAOD::numberOfContribPixelLayers))){
          if((numberOfBLayerHits + numberOfBLayerOutliers) == 0 && numberOfContribPixelLayers ==2) m_numberOfBlayersMissed++;
        }  

        uint8_t numberOfSCTOutliers = 0;
        if (particle->summaryValue(numberOfSCTOutliers,xAOD::numberOfSCTOutliers))              m_numberOfSCTOutliers      += numberOfSCTOutliers;
        uint8_t numberOfSCTDeadSensors = 0;  
        if (particle->summaryValue(numberOfSCTDeadSensors,xAOD::numberOfSCTDeadSensors))           m_numberOfSCTDeadSensors   += numberOfSCTDeadSensors;
        uint8_t numberOfSCTSpoiltHits = 0;
        if (particle->summaryValue(numberOfSCTSpoiltHits,xAOD::numberOfSCTSpoiltHits))            m_numberOfSCTSpoiltHits    += numberOfSCTSpoiltHits;   
        uint8_t numberOfTRTHoles = 0;
        if (particle->summaryValue(numberOfTRTHoles,xAOD::numberOfTRTHoles))                 m_numberOfTRTHoles         +=  numberOfTRTHoles;
        uint8_t numberOfTRTDeadStraws = 0;
        if (particle->summaryValue(numberOfTRTDeadStraws,xAOD::numberOfTRTDeadStraws))            m_numberOfTRTDeadStraws    += numberOfTRTDeadStraws;
        uint8_t numberOfTRTTubeHits = 0;
        if (particle->summaryValue( numberOfTRTTubeHits,xAOD:: numberOfTRTTubeHits))              m_numberOfTRTTubeHits      +=  numberOfTRTTubeHits;     
      }


      if( m_addTruthLink ){
        MCTruthPartClassifier::ParticleType type = MCTruthPartClassifier::Unknown;
        MCTruthPartClassifier::ParticleOrigin origin = MCTruthPartClassifier::NonDefined;
        float probability = -1.0;
        ElementLink<xAOD::TruthParticleContainer> link;
        ElementLink<CONT> tpLink(*itr,container);
        if( !tpLink.isValid() ){
          ATH_MSG_WARNING("Failed to create ElementLink to Track/TrackParticle");
        }else{
          auto result = truth->find(tpLink);
          if( result == truth->end() ){
            ATH_MSG_WARNING("Failed find truth associated with Track/TrackParticle");
          }else{
	    // setTruthLink(link,result->second, type, origin);
            ATH_MSG_VERBOSE("Found track Truth: barcode  " << result->second.particleLink().barcode() 
			    << " evt " << result->second.particleLink().eventIndex());
            probability = result->second.probability();
            link = m_truthParticleLinkVec->find(result->second.particleLink());
            if( link.isValid() ){
              ATH_MSG_DEBUG("Found matching xAOD Truth: barcode " << (*link)->barcode() << " pt " << (*link)->pt() 
			    << " eta " << (*link)->eta() << " phi " << (*link)->phi());
	      // if configured also get truth classification
              if( result->second.particleLink().cptr() && !m_truthClassifier.empty() ){
                auto truthClass = m_truthClassifier->particleTruthClassifier(result->second.particleLink().cptr());
                type = truthClass.first;
                origin = truthClass.second;
                ATH_MSG_VERBOSE("Got truth type  " << static_cast<int>(type) << "  origin " << static_cast<int>(origin));
              }
            }
          }
        }

        ElementLink<xAOD::TruthParticleContainer>& theLink = particle->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
	// set element link 
        theLink = link;
        float& theProbability =  const_cast<xAOD::TrackParticle*>(particle)->auxdata<float>("truthMatchProbability");
        theProbability = probability;
        if( !m_truthClassifier.empty() ){
          int& theType   = const_cast<xAOD::TrackParticle*>(particle)->auxdata<int>("truthType");
          int& theOrigin = const_cast<xAOD::TrackParticle*>(particle)->auxdata<int>("truthOrigin");
          theType = static_cast<int>(type);
          theOrigin = static_cast<int>(origin);
        }
      }
      ++itr_xaod;
    }// loop over aod tracks

    ATH_MSG_DEBUG( "Converted [" << container.size() << " -> " << xaod->size() << "] TrackParticles and stored in " << xaodName );
    if(container.size() != xaod->size()) {
      ATH_MSG_ERROR( "number of items in the AOD container: " 
		     << container.size() 
		     << " is not equal to the number of items in its converted xAOD equivalent: " 
		     << xaod->size() );
      return -1;
    }
    
    return 1; 
  }

  xAOD::TrackParticle* TrackParticleCnvAlg::createParticle( xAOD::TrackParticleContainer& xaod, 
							    const Rec::TrackParticleContainer& /**container*/, 
							    const Rec::TrackParticle& tp)
  {
    // create the xAOD::TrackParticle, the pointer is added to the container in the function
    return m_particleCreator->createParticle(tp,&xaod);
  } // createParticleAndTruth

  xAOD::TrackParticle* TrackParticleCnvAlg::createParticle( xAOD::TrackParticleContainer& xaod, 
							    const TrackCollection& container, 
							    const Trk::Track& tp)
  {
    // create the xAOD::TrackParticle, the pointer is added to the container in the function
    ElementLink<TrackCollection> trackLink(&tp,container);
    return m_particleCreator->createParticle(trackLink,&xaod);
  }

  StatusCode TrackParticleCnvAlg::finalize() {

    if (m_IdOutputInfo){

      double nTP = (double)m_nTrackParticlesCreated;
  
      msg(MSG::INFO) << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setw(9) << std::setprecision(3);
      msg(MSG::INFO) << " --- InDetParticleCreation Summary: " << endreq;
    
      msg(MSG::INFO) << " --- Input  TrackCollection        : \"" <<  m_aodTrackContainerName              << "\" with " << m_nTracksProcessed << " Tracks (";
      msg(MSG::INFO) << ( m_numEvents ? (double)m_nTracksProcessed/(double)m_numEvents : 0 ) << " per event)." << endreq;
    
    
      msg(MSG::INFO) << " --- Output TrackParticleContainer : \"" << m_xaodTracksContainerName << "\" with " << nTP << " TrackParticles (";
      msg(MSG::INFO) << (m_numEvents ? nTP/(double)m_numEvents : 0 ) << " per event)." << endreq;
    
      if (m_nTrackParticlesCreated>0){
	msg(MSG::INFO) << " ---  /Track in   -----> "
		       << " BLayer " 
		       << " Pixel " 
		       << "  SCT  "
		       << "  TRT  " 
		       << "  All (P+S+T)" << endreq;
	msg(MSG::INFO) << " ---  Hits                "
		       << (double)m_numberOfBLayerHits/nTP << "  "
		       << (double)m_numberOfPixelHits/nTP  << "  "
		       << (double)m_numberOfSCTHits/nTP    << "  "
		       << (double)m_numberOfTRTHits/nTP    << "  "
		       << (double)(m_numberOfPixelHits+m_numberOfSCTHits+m_numberOfTRTHits)/nTP << endreq;
	msg(MSG::INFO) << " ---  Shared Hits         "
		       << (double)m_numberOfBLayerSharedHits/nTP << "  " 
		       << (double)m_numberOfPixelSharedHits/nTP  << "  " 
		       << (double)m_numberOfSCTSharedHits/nTP    << "  " 
		       << " " << "       "
		       << " " << (double)(m_numberOfPixelSharedHits+m_numberOfSCTSharedHits)/nTP << endreq;
	msg(MSG::INFO) << " ---  Spoiled/Tube Hits    "
		       << "      "
		       << (double)m_numberOfPixelSpoiltHits/nTP << "  "
		       << (double)m_numberOfSCTSpoiltHits/nTP   << "  "
		       << " " << (double)m_numberOfTRTTubeHits/nTP     << "  "
		       << " " << (double)(m_numberOfPixelSpoiltHits+m_numberOfSCTSpoiltHits+m_numberOfTRTTubeHits)/nTP << endreq;
	msg(MSG::INFO) << " ---  Outliers            "
		       << (double)m_numberOfBLayerOutliers/nTP << "  "
		       << (double)m_numberOfPixelOutliers/nTP  << "  "
		       << (double)m_numberOfSCTOutliers/nTP    << "  "
		       << " " << (double)m_numberOfTRTOutliers/nTP    << "  "
		       << " " << (double)m_numberOfOutliersOnTrack/nTP << endreq;
	msg(MSG::INFO) << " ---  Holes               "
		       << "       "
		       << (double)m_numberOfPixelHoles/nTP   << "  " 
		       << (double)m_numberOfSCTHoles/nTP     << "  "
		       << " " << (double)m_numberOfTRTHoles/nTP     << "  "
		       << " " << (double)(m_numberOfPixelHoles+m_numberOfSCTHoles+m_numberOfTRTHoles)/nTP << endreq;
	msg(MSG::INFO) << "      missed (exp) blayer " << (double) m_numberOfBlayersMissed/nTP << endreq;         
	msg(MSG::INFO) << "      SCT Double Holes    "
		       << "       "
		       << "       "
		       << (double)m_numberOfSCTDoubleHoles/nTP
		       << endreq;
	msg(MSG::INFO) << " ---  Dead Sensors/Straws "
		       << "       "
		       << (double)m_numberOfPixelDeadSensors/nTP << "  "
		       << (double)m_numberOfSCTDeadSensors/nTP   << "  "
		       << " " << (double)m_numberOfTRTDeadStraws/nTP    << "  "
		       << " " << (double)(m_numberOfPixelDeadSensors+m_numberOfSCTDeadSensors+m_numberOfTRTDeadStraws)/nTP
		       << endreq;
	msg(MSG::INFO) << " ---  Add info Pixels " << endreq;
	msg(MSG::INFO) << "      contributing layers        " << (double)m_numberOfContribPixelLayers/nTP << endreq;
	msg(MSG::INFO) << "      ganged pixels              " << (double)m_numberOfGangedPixels/nTP << endreq;
	msg(MSG::INFO) << "      ganged flagged as fake     " << (double)m_numberOfGangedFlaggedFakes/nTP << endreq;
	msg(MSG::INFO) << " ---  Add info TRT " << endreq;
	msg(MSG::INFO) << "      High Threshold Hits "
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTHighThresholdHits/nTP
		       << endreq;
	msg(MSG::INFO) << "      High thre. outliers "
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTHighThresholdOutliers/nTP
		       << endreq;
	msg(MSG::INFO) << "      Xenon hits"
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTXenonHits/nTP
		       << endreq;
      
      
      } else 
	msg(MSG::INFO) << " No TrackParticles have been created ... skipping output." << endreq;
    }    
    msg(MSG::DEBUG) << "finalize() success" << endreq;
    return StatusCode::SUCCESS;  
  }




} // namespace xAODMaker
