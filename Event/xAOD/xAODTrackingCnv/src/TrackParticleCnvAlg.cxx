/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// Local include(s):
#include "TrackParticleCnvAlg.h"
#include "xAODTrackingCnv/ITrackCollectionCnvTool.h"
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"

namespace xAODMaker {
  TrackParticleCnvAlg::TrackParticleCnvAlg( const std::string& name,
					    ISvcLocator* svcLoc )
    : AthReentrantAlgorithm( name, svcLoc ),
      m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool"),
      m_truthClassifier("MCTruthClassifier/MCTruthClassifier"),
      m_TrackCollectionCnvTool( "xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool", this ),
      m_RecTrackParticleContainerCnvTool( "xAODMaker::RecTrackParticleContainerCnvTool/RecTrackParticleContainerCnvTool", this ),
      m_aod("TrackParticleCandidate"),
      m_tracks("Tracks"),
      m_xaodout("InDetTrackParticles"),
      m_xaodTrackParticlesout("ConvertedTrackParticleCandidate"),
      m_truthParticleLinkVec("xAODTruthLinks"),
      m_aodTruth(""),
      m_trackTruth("")
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
    declareProperty( "TrackCollectionCnvTool",                 m_TrackCollectionCnvTool );
    declareProperty( "RecTrackParticleContainerCnvTool",       m_RecTrackParticleContainerCnvTool );
    declareProperty( "DoMonitoring",      m_doMonitoring = false  );
    declareProperty( "TrackMonTool",      m_trackMonitoringTool  );

  }

  StatusCode TrackParticleCnvAlg::initialize() {

    ATH_MSG_DEBUG( "Initializing TrackParticleCnvAlg" );
    ATH_MSG_DEBUG( "AODContainerName  = " << m_aod.key() );
    ATH_MSG_DEBUG( "xAODContainerName = " << m_xaodTrackParticlesout.key() );
    ATH_CHECK(m_particleCreator.retrieve());
    if(m_addTruthLink) ATH_CHECK(m_truthClassifier.retrieve());
    else m_truthClassifier.disable();
    ATH_CHECK( m_TrackCollectionCnvTool.retrieve() );
    ATH_CHECK( m_RecTrackParticleContainerCnvTool.retrieve(DisableTool{!m_convertAODTrackParticles}) );
    // to preserve the inisialised parameters of the ParticleCreatorTool:
    ATH_MSG_DEBUG( "Overriding particle creator tool settings." );
    ATH_CHECK( m_TrackCollectionCnvTool->setParticleCreatorTool( &m_particleCreator ) );
    if(m_convertAODTrackParticles) ATH_CHECK( m_RecTrackParticleContainerCnvTool->setParticleCreatorTool( &m_particleCreator ) );

    ATH_CHECK(m_xaodout.initialize(m_convertTracks));
    ATH_CHECK(m_xaodTrackParticlesout.initialize(m_convertAODTrackParticles));
    ATH_CHECK(m_aod.initialize(m_convertAODTrackParticles));
    ATH_CHECK(m_tracks.initialize(m_convertTracks));
    ATH_CHECK(m_truthParticleLinkVec.initialize(m_addTruthLink));
    ATH_CHECK(m_aodTruth.initialize(m_addTruthLink && m_convertAODTrackParticles));
    ATH_CHECK(m_trackTruth.initialize(m_addTruthLink && m_convertTracks));

    //Retrieve monitoring tool if provided
    ATH_CHECK( m_trackMonitoringTool.retrieve(DisableTool{!m_doMonitoring}) );


    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode TrackParticleCnvAlg::execute(const EventContext& ctx) const {

    const Rec::TrackParticleContainer *aod=nullptr;
    const TrackCollection *tracks = nullptr;
    const xAODTruthParticleLinkVector *truthLinks = nullptr;
    const TrackParticleTruthCollection *aodTruth = nullptr;
    const TrackTruthCollection *trackTruth = nullptr;

    // Retrieve the AOD particles:
    if (m_convertAODTrackParticles){
      SG::ReadHandle<Rec::TrackParticleContainer> rh_aod(m_aod, ctx);
      if (!rh_aod.isValid()) {
        ATH_MSG_DEBUG("Error finding " << m_aod.key() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      } else {
	aod = rh_aod.cptr();
        ATH_MSG_VERBOSE("Got TrackParticleContainer with key " << m_aod.key() << " found.");
      }
    }
    // Retrieve the Tracks:
    if (m_convertTracks){
      SG::ReadHandle<TrackCollection> rh_tracks(m_tracks, ctx);
      if (!rh_tracks.isValid()) {
        ATH_MSG_DEBUG("Error finding " << m_tracks.key() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      }  else {
	tracks = rh_tracks.cptr();
        ATH_MSG_VERBOSE("Got TrackCollection with key " << m_tracks.key() << " found.");
      }
    }
    if( m_addTruthLink ){
      if (m_convertAODTrackParticles){
	SG::ReadHandle<TrackParticleTruthCollection> rh_aodTruth(m_aodTruth, ctx);
        if (!rh_aodTruth.isValid()) {
          ATH_MSG_WARNING("No TrackParticleTruthCollection with key " << m_aodTruth.key() << " found. Do nothing.");
          return StatusCode::SUCCESS;
        }
	else aodTruth = rh_aodTruth.cptr();
      }
      if (m_convertTracks){
	SG::ReadHandle<TrackTruthCollection> rh_trackTruth(m_trackTruth, ctx);
        if (!rh_trackTruth.isValid()) {
          ATH_MSG_WARNING("No DetailedTrackTruthCollection with key " << m_trackTruth.key() << " found. Do nothing.");
          return StatusCode::SUCCESS;
        }
	else trackTruth = rh_trackTruth.cptr();
      }

      SG::ReadHandle<xAODTruthParticleLinkVector> rh_truthParticleLinkVec(m_truthParticleLinkVec, ctx);
      if (!rh_truthParticleLinkVec.isValid()) {
        ATH_MSG_WARNING("No xAODTruthParticleLinkVector with key " << m_truthParticleLinkVec.key() << " found. Do nothing.");
        return StatusCode::SUCCESS;
      }
      else truthLinks = rh_truthParticleLinkVec.cptr();
    }
    if (m_convertTracks) {
      SG::WriteHandle<xAOD::TrackParticleContainer> wh_xaodout(m_xaodout, ctx);
      ATH_CHECK(wh_xaodout.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
      convert((*tracks), trackTruth, m_TrackCollectionCnvTool,  wh_xaodout, truthLinks);

      //Monitor track parameters
      if( m_doMonitoring) m_trackMonitoringTool->monitor_tracks( "Track", "Pass", *wh_xaodout );

    }
    if (m_convertAODTrackParticles){
      SG::WriteHandle<xAOD::TrackParticleContainer> wh_xaodTrackParticlesout(m_xaodTrackParticlesout, ctx);
      ATH_CHECK(wh_xaodTrackParticlesout.record(std::make_unique<xAOD::TrackParticleContainer>(), std::make_unique<xAOD::TrackParticleAuxContainer>()));
      convert((*aod), aodTruth, m_RecTrackParticleContainerCnvTool, wh_xaodTrackParticlesout, truthLinks);
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
  int TrackParticleCnvAlg::convert(const CONT& container, const TRUTHCONT& truth, CONVTOOL& conv_tool, SG::WriteHandle<xAOD::TrackParticleContainer> &xaod, const xAODTruthParticleLinkVector *truthLinkVec) const{    
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

      if (!(*itr_xaod)) {
	ATH_MSG_WARNING("WTaF? Empty element in xAOD container!");
	continue;
      }

      xAOD::TrackParticle* particle =  *itr_xaod;

      if(!particle){
	ATH_MSG_WARNING("Failed to get an xAOD::TrackParticle");
	continue;
      }

      //
      // --------- statistics
      //

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
            link = truthLinkVec->find(result->second.particleLink());
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
            else {
               if (result->second.particleLink().barcode()>0) {
                  ATH_MSG_WARNING( "No associated xAOD truth for valid truth link " << result->second.particleLink());
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
    return StatusCode::SUCCESS;  
  }




} // namespace xAODMaker
