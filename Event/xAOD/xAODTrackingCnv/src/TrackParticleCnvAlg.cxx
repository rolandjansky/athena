/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleCnvAlg.cxx 298303 2013-12-05 08:41:30Z emoyse $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):

#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"

#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"

#include "CxxUtils/make_unique.h"
// Local include(s):
#include "TrackParticleCnvAlg.h"
#include "xAODTrackingCnv/ITrackCollectionCnvTool.h"
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"



namespace xAODMaker {

  //Small class for cleaning up counter code
  class SummaryCounter{
      xAOD::TrackParticle* m_ptr;
      public:
      SummaryCounter(xAOD::TrackParticle* inptr) : m_ptr(inptr) { }
      void AddToCounter(xAOD::SummaryType type, unsigned int &counter){
        uint8_t result=0;
        if (m_ptr->summaryValue(result, type))
            counter += result;
      }
  };


  TrackParticleCnvAlg::TrackParticleCnvAlg( const std::string& name,
					    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
      m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool"),
      m_truthClassifier("MCTruthClassifier/MCTruthClassifier"),
      m_TrackCollectionCnvTool( "xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool", this ),
      m_RecTrackParticleContainerCnvTool( "xAODMaker::RecTrackParticleContainerCnvTool/RecTrackParticleContainerCnvTool", this ),
      m_aod("TrackParticleCandidate"),
      m_tracks("Tracks"),
      m_xaodout("InDetTrackParticles"),
      m_xauxout(""),
      m_xaodTrackParticlesout("ConvertedTrackParticleCandidate"),
      m_xauxTrackParticlesout(""),
      m_truthParticleLinkVec("xAODTruthLinks"),
      m_aodTruth(""),
      m_trackTruth(""),
      
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
    declareProperty( "AODContainerName",        m_aod );
    declareProperty( "xAODContainerName",       m_xaodTrackParticlesout );
    declareProperty( "TrackParticleCreator",    m_particleCreator );
    declareProperty( "AddTruthLink",            m_addTruthLink = false );
    declareProperty( "AODTruthContainerName",   m_aodTruth );
    declareProperty( "TrackTruthContainerName", m_trackTruth );
    declareProperty( "xAODTruthLinkVector",     m_truthParticleLinkVec);
    declareProperty( "TrackContainerName",      m_tracks );
    declareProperty( "xAODTrackParticlesFromTracksContainerName",   m_xaodout);   
    declareProperty( "ConvertTrackParticles",   m_convertAODTrackParticles = true );   
    declareProperty( "ConvertTracks",           m_convertTracks = false ); 
    declareProperty( "PrintIDSummaryInfo",      m_IdOutputInfo = false ); 
    declareProperty( "TrackCollectionCnvTool",                 m_TrackCollectionCnvTool );
    declareProperty( "RecTrackParticleContainerCnvTool",       m_RecTrackParticleContainerCnvTool );
  }

  StatusCode TrackParticleCnvAlg::initialize() {

    ATH_MSG_DEBUG( "Initializing TrackParticleCnvAlg" );
    ATH_MSG_DEBUG( "AODContainerName  = " << m_aod.name() );
    ATH_MSG_DEBUG( "xAODContainerName = " << m_xaodTrackParticlesout.name() );
    ATH_CHECK(m_particleCreator.retrieve());
    ATH_CHECK(m_truthClassifier.retrieve());
    ATH_CHECK( m_TrackCollectionCnvTool.retrieve() );
    ATH_CHECK( m_RecTrackParticleContainerCnvTool.retrieve() );
    // to preserve the inisialised parameters of the ParticleCreatorTool:
    ATH_MSG_DEBUG( "Overriding particle creator tool settings." );
    ATH_CHECK( m_TrackCollectionCnvTool->setParticleCreatorTool( &m_particleCreator ) );
    ATH_CHECK( m_RecTrackParticleContainerCnvTool->setParticleCreatorTool( &m_particleCreator ) );

    if (!m_convertTracks) {
      ATH_CHECK( m_xaodout.assign ("") );
    }
    if (!m_convertAODTrackParticles) {
      ATH_CHECK( m_xaodTrackParticlesout.assign ("") );
    }

    //Automatically set the Aux names
    if(!m_xaodout.name().empty() && m_xauxout.name().empty()){
        m_xauxout = SG::WriteHandle<xAOD::TrackParticleAuxContainer>(m_xaodout.name() + "Aux.");
    }
    if(!m_xaodTrackParticlesout.name().empty() && m_xauxTrackParticlesout.name().empty()){
        m_xauxTrackParticlesout = SG::WriteHandle<xAOD::TrackParticleAuxContainer>( m_xaodTrackParticlesout.name() + "Aux." );
    }
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrackParticleCnvAlg::execute() {

    m_numEvents++; 
    // Retrieve the AOD particles:
    if (m_convertAODTrackParticles){
      if (!m_aod.isValid()) {
        ATH_MSG_DEBUG("Error finding " << m_aod.name() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      } else {
        ATH_MSG_VERBOSE("Got TrackParticleContainer with key " << m_aod.name() << " found.");
      }
    }
    // Retrieve the Tracks:
    if (m_convertTracks){
      if (!m_tracks.isValid()) {
        ATH_MSG_DEBUG("Error finding " << m_tracks.name() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      }  else {
        ATH_MSG_VERBOSE("Got TrackCollection with key " << m_tracks.name() << " found.");
      }
    }
    if( m_addTruthLink ){
      if (m_convertAODTrackParticles){
        if (!m_aodTruth.isValid()) {
          ATH_MSG_WARNING("No TrackParticleTruthCollection with key " << m_aodTruth.name() << " found. Do nothing.");
          return StatusCode::SUCCESS;
        }
      }
      if (m_convertTracks){
        if (!m_trackTruth.isValid()) {
          ATH_MSG_WARNING("No DetailedTrackTruthCollection with key " << m_trackTruth.name() << " found. Do nothing.");
          return StatusCode::SUCCESS;
        }
      }

//      m_truthParticleLinkVec = evtStore()->tryConstRetrieve<xAODTruthParticleLinkVector>(m_truthLinkVecName);
      if (!m_truthParticleLinkVec.isValid()) {
        ATH_MSG_WARNING("No xAODTruthParticleLinkVector with key " << m_truthParticleLinkVec.name() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      }
    }
    if (m_convertTracks) {
       m_xaodout = CxxUtils::make_unique<xAOD::TrackParticleContainer>();
       m_xauxout = CxxUtils::make_unique<xAOD::TrackParticleAuxContainer>();
       if(!m_xaodout.isValid() || !m_xauxout.isValid()){
          ATH_MSG_ERROR("Error creating key " << m_xaodout.name());
          return StatusCode::FAILURE;
       }
       m_xaodout->setStore(m_xauxout.ptr());
       const TrackTruthCollection* truthPtr = nullptr;
       if (m_addTruthLink)
         truthPtr = m_trackTruth.cptr();
       convert(*(m_tracks.cptr()), truthPtr, m_TrackCollectionCnvTool,  m_xaodout);
    }
    if (m_convertAODTrackParticles){
       m_xaodTrackParticlesout = CxxUtils::make_unique<xAOD::TrackParticleContainer>();
       m_xauxTrackParticlesout = CxxUtils::make_unique<xAOD::TrackParticleAuxContainer>();
       if(!m_xaodTrackParticlesout.isValid() || !m_xauxTrackParticlesout.isValid()){
          ATH_MSG_ERROR("Error creating key " << m_xaodTrackParticlesout.name());
          return StatusCode::FAILURE;
       }
       m_xaodTrackParticlesout->setStore(m_xauxTrackParticlesout.ptr());
       convert(*(m_aod.cptr()), m_aodTruth.cptr(), m_RecTrackParticleContainerCnvTool, m_xaodTrackParticlesout);
    }

    return StatusCode::SUCCESS;

  }

  template <typename CONT> 
  class AssociationHelper;

  template<>
  class AssociationHelper<TrackCollection> {
  public:
    AssociationHelper(const TrackCollection &, xAOD::TrackParticleContainer *) {}
    const Trk::Track *operator()(xAOD::TrackParticle *track_particle, unsigned int) const {
      return track_particle->track();
    }
  };

  template<>
  class AssociationHelper<Rec::TrackParticleContainer> {
  public:
    AssociationHelper(const Rec::TrackParticleContainer &cont_src, xAOD::TrackParticleContainer *cont_dest)
      : m_contSrc(&cont_src)
    {
      if (cont_src.size() != cont_dest->size()) {
        std::stringstream message;
        message << __FILE__ << ":" << __LINE__
                << " Expected one-to-one conversion from AOD to xAOD TrackParticles but sizes differ: "
                << cont_src.size() << " != " << cont_dest->size();
        throw std::runtime_error(  message.str() );
      }
    }

    const Rec::TrackParticle *operator()(xAOD::TrackParticle *, unsigned int idx) const {
      return m_contSrc->at(idx);
    }
  private:
    const Rec::TrackParticleContainer *m_contSrc;
  };

  template<typename CONT, typename TRUTHCONT, typename CONVTOOL>
  int TrackParticleCnvAlg::convert(const CONT& container, const TRUTHCONT& truth, CONVTOOL& conv_tool, SG::WriteHandle<xAOD::TrackParticleContainer> &xaod){    
    // Create the xAOD container and its auxiliary store:
   

    // convert the track containers separately with the converting tools that are also used by TrigHLTtoxAODTool
    ATH_MSG_DEBUG( "calling the converting tool for " << xaod.name() );
    if( conv_tool->convert( &container, xaod.ptr() ).isFailure() ) {
      ATH_MSG_ERROR("Couldn't convert aod to xaod (" << xaod.name() << ") with the converting tool");
      return -1;
    }
    // Create the xAOD objects:
    xAOD::TrackParticleContainer::iterator itr_xaod = xaod->begin();
    xAOD::TrackParticleContainer::iterator end_xaod = xaod->end();

    AssociationHelper<CONT> association_to_src(container,xaod.ptr());
    // loop over AOD and converted xAOD for summary info and truth links
    for( ;itr_xaod!=end_xaod;++itr_xaod ) {
      //protect if something went wrong and there is no converted xaod equivalent

      m_nTracksProcessed++;

      if (!(*itr_xaod)) {
	ATH_MSG_WARNING("WTaF? Empty element in xAOD container!");
	continue;
      }

      xAOD::TrackParticle* particle =  *itr_xaod;

      if(!particle){
	ATH_MSG_WARNING("Failed to get an xAOD::TrackParticle");
	continue;
      }

      m_nTrackParticlesCreated++;
      //
      // --------- statistics
      //
      if (m_IdOutputInfo) {
        SummaryCounter count(particle);
	// integer entries in the summary can be -1 or -999, strange I know but that is the design ...
        
        count.AddToCounter(xAOD::numberOfBLayerHits, m_numberOfBLayerHits);
        
        count.AddToCounter(xAOD::numberOfBLayerSharedHits, m_numberOfBLayerSharedHits);
        
        count.AddToCounter(xAOD::numberOfBLayerOutliers, m_numberOfBLayerOutliers);
        
        count.AddToCounter(xAOD::numberOfContribPixelLayers, m_numberOfContribPixelLayers);
        
        count.AddToCounter(xAOD::numberOfPixelHits, m_numberOfPixelHits);

        count.AddToCounter(xAOD::numberOfPixelSharedHits, m_numberOfPixelSharedHits);
        
        count.AddToCounter(xAOD::numberOfPixelHoles, m_numberOfPixelHoles);
        
        count.AddToCounter(xAOD::numberOfGangedPixels, m_numberOfGangedPixels);
        
        count.AddToCounter(xAOD::numberOfGangedFlaggedFakes, m_numberOfGangedFlaggedFakes);
        
        count.AddToCounter(xAOD::numberOfSCTHits, m_numberOfSCTHits);
        
        count.AddToCounter(xAOD::numberOfSCTSharedHits, m_numberOfSCTSharedHits);
                
        count.AddToCounter(xAOD::numberOfSCTHoles, m_numberOfSCTHoles);
        
        count.AddToCounter(xAOD::numberOfSCTDoubleHoles, m_numberOfSCTDoubleHoles);
        
        count.AddToCounter(xAOD::numberOfTRTHits, m_numberOfTRTHits);
                 
        count.AddToCounter(xAOD::numberOfTRTXenonHits, m_numberOfTRTXenonHits);
        
        count.AddToCounter(xAOD::numberOfTRTHighThresholdHits, m_numberOfTRTHighThresholdHits);
       
        count.AddToCounter(xAOD::numberOfTRTOutliers, m_numberOfTRTOutliers);
           
        count.AddToCounter(xAOD::numberOfTRTHighThresholdOutliers, m_numberOfTRTHighThresholdOutliers);
        
        count.AddToCounter(xAOD::numberOfOutliersOnTrack, m_numberOfOutliersOnTrack);
        
        count.AddToCounter(xAOD::numberOfPixelOutliers, m_numberOfPixelOutliers);
        
        count.AddToCounter(xAOD::numberOfPixelDeadSensors, m_numberOfPixelDeadSensors);
        
        count.AddToCounter(xAOD::numberOfPixelSpoiltHits, m_numberOfPixelSpoiltHits);
        
        {
        uint8_t expectBLayerHit = 0;
        uint8_t numberOfBLayerHits = 0;
        uint8_t numberOfBLayerOutliers = 0;
        uint8_t numberOfContribPixelLayers=0;
        if (particle->summaryValue(expectBLayerHit,xAOD::expectBLayerHit) &&
            expectBLayerHit>0 && 
            (particle->summaryValue(numberOfBLayerHits,xAOD::numberOfBLayerHits)) && 
            (particle->summaryValue(numberOfBLayerOutliers,xAOD::numberOfBLayerOutliers)) && 
            (particle->summaryValue(numberOfContribPixelLayers,xAOD::numberOfContribPixelLayers))){
          if((numberOfBLayerHits + numberOfBLayerOutliers) == 0 && numberOfContribPixelLayers ==2) m_numberOfBlayersMissed++;
        }
        }
        
        count.AddToCounter(xAOD::numberOfSCTOutliers, m_numberOfSCTOutliers);
        
        count.AddToCounter(xAOD::numberOfSCTDeadSensors, m_numberOfSCTDeadSensors);
        
        count.AddToCounter(xAOD::numberOfSCTSpoiltHits, m_numberOfSCTSpoiltHits);
        
        count.AddToCounter(xAOD::numberOfTRTHoles, m_numberOfTRTHoles);
        
        count.AddToCounter(xAOD::numberOfTRTDeadStraws, m_numberOfTRTDeadStraws);
        
        count.AddToCounter(xAOD::numberOfTRTTubeHits, m_numberOfTRTTubeHits);
      }


      if( m_addTruthLink ){
        MCTruthPartClassifier::ParticleType type = MCTruthPartClassifier::Unknown;
        MCTruthPartClassifier::ParticleOrigin origin = MCTruthPartClassifier::NonDefined;
        float probability = -1.0;
        ElementLink<xAOD::TruthParticleContainer> link;

        ElementLink<CONT> tpLink(association_to_src(*itr_xaod,itr_xaod-xaod->begin()), container);
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
        float& theProbability =   (particle)->auxdata<float>("truthMatchProbability");
        theProbability = probability;
        if( !m_truthClassifier.empty() ){
          int& theType   =  (particle)->auxdata<int>("truthType");
          int& theOrigin =  (particle)->auxdata<int>("truthOrigin");
          theType = static_cast<int>(type);
          theOrigin = static_cast<int>(origin);
        }
      }
    }// loop over aod tracks

    ATH_MSG_DEBUG( "Converted [" << container.size() << " -> " << xaod->size() << "] TrackParticles and stored in " << xaod.name() );
    if(container.size() != xaod->size()) {
      ATH_MSG_WARNING( "number of items in the AOD container: " 
                       << container.size() 
                       << " is not equal to the number of items in its converted xAOD equivalent: " 
                       << xaod->size() );
    }
    
    return 1; 
  }

  xAOD::TrackParticle* TrackParticleCnvAlg::createParticle( xAOD::TrackParticleContainer& xaod, 
							    const Rec::TrackParticleContainer& /**container*/, 
							    const Rec::TrackParticle& tp)
  {
    // create the xAOD::TrackParticle, the pointer is added to the container in the function
    xAOD::TrackParticle* xp = m_particleCreator->createParticle(tp,&xaod);
    const ElementLink<VxContainer>& vx = tp.reconstructedVertexLink();
    if (!vx.isDefault())
      xp->setVertexLink (ElementLink<xAOD::VertexContainer> (vx.dataID(), vx.index()));
    return xp;
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
      msg(MSG::INFO) << " --- InDetParticleCreation Summary: " << endmsg;
    
      msg(MSG::INFO) << " --- Input  TrackCollection        : \"" <<  m_tracks.name()              << "\" with " << m_nTracksProcessed << " Tracks (";
      msg(MSG::INFO) << ( m_numEvents ? (double)m_nTracksProcessed/(double)m_numEvents : 0 ) << " per event)." << endmsg;
    
    
      msg(MSG::INFO) << " --- Output TrackParticleContainer : \"" << m_xaodout.name() << "\" with " << nTP << " TrackParticles (";
      msg(MSG::INFO) << (m_numEvents ? nTP/(double)m_numEvents : 0 ) << " per event)." << endmsg;
    
      if (m_nTrackParticlesCreated>0){
	msg(MSG::INFO) << " ---  /Track in   -----> "
		       << " BLayer " 
		       << " Pixel " 
		       << "  SCT  "
		       << "  TRT  " 
		       << "  All (P+S+T)" << endmsg;
	msg(MSG::INFO) << " ---  Hits                "
		       << (double)m_numberOfBLayerHits/nTP << "  "
		       << (double)m_numberOfPixelHits/nTP  << "  "
		       << (double)m_numberOfSCTHits/nTP    << "  "
		       << (double)m_numberOfTRTHits/nTP    << "  "
		       << (double)(m_numberOfPixelHits+m_numberOfSCTHits+m_numberOfTRTHits)/nTP << endmsg;
	msg(MSG::INFO) << " ---  Shared Hits         "
		       << (double)m_numberOfBLayerSharedHits/nTP << "  " 
		       << (double)m_numberOfPixelSharedHits/nTP  << "  " 
		       << (double)m_numberOfSCTSharedHits/nTP    << "  " 
		       << " " << "       "
		       << " " << (double)(m_numberOfPixelSharedHits+m_numberOfSCTSharedHits)/nTP << endmsg;
	msg(MSG::INFO) << " ---  Spoiled/Tube Hits    "
		       << "      "
		       << (double)m_numberOfPixelSpoiltHits/nTP << "  "
		       << (double)m_numberOfSCTSpoiltHits/nTP   << "  "
		       << " " << (double)m_numberOfTRTTubeHits/nTP     << "  "
		       << " " << (double)(m_numberOfPixelSpoiltHits+m_numberOfSCTSpoiltHits+m_numberOfTRTTubeHits)/nTP << endmsg;
	msg(MSG::INFO) << " ---  Outliers            "
		       << (double)m_numberOfBLayerOutliers/nTP << "  "
		       << (double)m_numberOfPixelOutliers/nTP  << "  "
		       << (double)m_numberOfSCTOutliers/nTP    << "  "
		       << " " << (double)m_numberOfTRTOutliers/nTP    << "  "
		       << " " << (double)m_numberOfOutliersOnTrack/nTP << endmsg;
	msg(MSG::INFO) << " ---  Holes               "
		       << "       "
		       << (double)m_numberOfPixelHoles/nTP   << "  " 
		       << (double)m_numberOfSCTHoles/nTP     << "  "
		       << " " << (double)m_numberOfTRTHoles/nTP     << "  "
		       << " " << (double)(m_numberOfPixelHoles+m_numberOfSCTHoles+m_numberOfTRTHoles)/nTP << endmsg;
	msg(MSG::INFO) << "      missed (exp) blayer " << (double) m_numberOfBlayersMissed/nTP << endmsg;         
	msg(MSG::INFO) << "      SCT Double Holes    "
		       << "       "
		       << "       "
		       << (double)m_numberOfSCTDoubleHoles/nTP
		       << endmsg;
	msg(MSG::INFO) << " ---  Dead Sensors/Straws "
		       << "       "
		       << (double)m_numberOfPixelDeadSensors/nTP << "  "
		       << (double)m_numberOfSCTDeadSensors/nTP   << "  "
		       << " " << (double)m_numberOfTRTDeadStraws/nTP    << "  "
		       << " " << (double)(m_numberOfPixelDeadSensors+m_numberOfSCTDeadSensors+m_numberOfTRTDeadStraws)/nTP
		       << endmsg;
	msg(MSG::INFO) << " ---  Add info Pixels " << endmsg;
	msg(MSG::INFO) << "      contributing layers        " << (double)m_numberOfContribPixelLayers/nTP << endmsg;
	msg(MSG::INFO) << "      ganged pixels              " << (double)m_numberOfGangedPixels/nTP << endmsg;
	msg(MSG::INFO) << "      ganged flagged as fake     " << (double)m_numberOfGangedFlaggedFakes/nTP << endmsg;
	msg(MSG::INFO) << " ---  Add info TRT " << endmsg;
	msg(MSG::INFO) << "      High Threshold Hits "
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTHighThresholdHits/nTP
		       << endmsg;
	msg(MSG::INFO) << "      High thre. outliers "
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTHighThresholdOutliers/nTP
		       << endmsg;
	msg(MSG::INFO) << "      Xenon hits"
		       << "       "
		       << "       "
		       << "       "
		       << " " << (double)m_numberOfTRTXenonHits/nTP
		       << endmsg;
      
      
      } else 
	msg(MSG::INFO) << " No TrackParticles have been created ... skipping output." << endmsg;
    }    
    msg(MSG::DEBUG) << "finalize() success" << endmsg;
    return StatusCode::SUCCESS;  
  }




} // namespace xAODMaker
