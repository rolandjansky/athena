/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// InDetCandidateTool
//  AlgTool performing pre-selection on ID tracks, extrapolation and creation
//  of InDetCandidate collection. 
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "RecoToolInterfaces/ITrackThroughDetectorBoundaries.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "TrkParameters/TrackParameters.h"

#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "InDetCandidateTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
  int getCount( const xAOD::TrackParticle& tp, xAOD::SummaryType type )  {
    uint8_t val;
    if( !tp.summaryValue(val,type) ) return 0;
    return static_cast<int>(val);
  }

  InDetCandidateTool::InDetCandidateTool (const std::string& type, const std::string& name, const IInterface* parent) : 
    AthAlgTool(type, name, parent),
    m_vectorExtrapolator("Rec::TrackThroughDetectorBoundaries/TrackThroughDetectorBoundaries"),
    m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
    m_caloCellAssociationTool("Trk::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
    //m_caloExtensionTool(""),
    //m_caloCellAssociationTool(""),
    m_trackSelector("InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool")
  {
    declareInterface<IInDetCandidateTool>(this);
    declareProperty("VectorExtrapolator",m_vectorExtrapolator );    
    declareProperty("ParticleCaloExtensionTool",m_caloExtensionTool );    
    declareProperty("ParticleCaloCellAssociationTool",m_caloCellAssociationTool );    
  }

  InDetCandidateTool::~InDetCandidateTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode InDetCandidateTool::initialize() {
    ATH_CHECK(m_vectorExtrapolator.retrieve());
    ATH_CHECK(m_trackSelector.retrieve());
    if( !m_caloExtensionTool.empty() )       ATH_CHECK(m_caloExtensionTool.retrieve());
    if( !m_caloCellAssociationTool.empty() ) ATH_CHECK(m_caloCellAssociationTool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode InDetCandidateTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void InDetCandidateTool::create( const xAOD::TrackParticleContainer& indetTrackParticles, InDetCandidateCollection& outputContainer ) const {
    ATH_MSG_DEBUG("Producing InDetCandidates for " << indetTrackParticles.size() );
    unsigned int ntracks = 0;
    int trackIndex = -1;
    for( auto tp : indetTrackParticles ){
      ++trackIndex;
      if( !tp->perigeeParameters().covariance() ) {
	ATH_MSG_WARNING("InDet TrackParticle without perigee! ");
	continue;
      }
      if( !m_trackSelector->decision(*tp) ) continue;
      ElementLink<xAOD::TrackParticleContainer> link(indetTrackParticles,trackIndex);
      if( !link.isValid() ){
	ATH_MSG_WARNING("Bad element link ");
      }else{
	// testing link
	if(*link!=tp) {
	  ATH_MSG_WARNING("Dereferenced Link not equal to TrackParticle, skipping track ");
	  continue;
	}
      }
      link.toPersistent();
 
      ATH_MSG_DEBUG(" Creating InDetCandidate: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() 
                    << " Pixel " << getCount(*tp,xAOD::numberOfBLayerHits ) + getCount(*tp,xAOD::numberOfPixelHits )
                    << " SCT "  << getCount(*tp,xAOD::numberOfSCTHits ) << " TRT " << getCount(*tp, xAOD::numberOfTRTHits ) );
      InDetCandidate* candidate = new InDetCandidate(link);
      const Trk::Track* track = tp->track();
      if( track ){
	const Rec::ParticleExtrapolationVector* extrapolation = m_vectorExtrapolator->getParametersAcrossAtlas(*track,Trk::muon,Trk::alongMomentum);
	if( extrapolation ){
	  ATH_MSG_VERBOSE("Extrapolation successfull " << extrapolation->size() );
	  candidate->particleExtrapolationVector(extrapolation);
	}
        if( !m_caloExtensionTool.empty() ){
          const xAOD::ParticleCaloExtension* extension = m_caloExtensionTool->caloExtension(const_cast<xAOD::TrackParticle&>(*tp)); 
          if( msgLvl(MSG::DEBUG) && extension && extension->numberOfParameters() != 0 ){
            ATH_MSG_DEBUG("Got calo extension " << extension->numberOfParameters() << " cells " << extension->caloCells().size() );
            for( unsigned int i=0;i<extension->numberOfParameters();++i){
              
              Trk::CurvilinearParameters pars = extension->curvilinearParameters(i);
              ATH_MSG_DEBUG( " id " << extension->parameterIdentifier(i) << " pos: r " << pars.position().perp() 
                             << " z " << pars.position().z()
                             << " momentum " << pars.momentum().perp() << " cov " << pars.covariance() );
            }
          }
        }
        if( !m_caloCellAssociationTool.empty() ){
          const xAOD::ParticleCaloExtension* extension = m_caloCellAssociationTool->caloAssociation(const_cast<xAOD::TrackParticle&>(*tp)); 
          if( msgLvl(MSG::DEBUG) && extension && extension->numberOfParameters() != 0 ){
            ATH_MSG_DEBUG("Got calo cell extension " << extension->numberOfParameters() << " cells " << extension->caloCells().size() );
          }
        }
      }
      ++ntracks;
      outputContainer.push_back(candidate);
    }
    ATH_MSG_DEBUG("InDetCandidates selected " << ntracks );
  }

}	// end of namespace
