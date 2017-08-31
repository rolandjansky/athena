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

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkParameters/TrackParameters.h"

#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "InDetCandidateTool.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
  int getCount( const xAOD::TrackParticle& tp, xAOD::SummaryType type )  {
    uint8_t val;
    if( !tp.summaryValue(val,type) ) return 0;
    return static_cast<int>(val);
  }

  InDetCandidateTool::InDetCandidateTool (const std::string& type, const std::string& name, const IInterface* parent) : 
  AthAlgTool(type, name, parent),
  m_trackSelector("InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool")
  {
    declareInterface<IInDetCandidateTool>(this);
    declareProperty("TrackSelector",m_trackSelector );    
    declareProperty("FlagCandidatesAsSiAssociated",m_flagCandidateAsSiAssociated = false );    
  }

  InDetCandidateTool::~InDetCandidateTool()
  {}

    //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

    StatusCode InDetCandidateTool::initialize() {
      ATH_CHECK(m_trackSelector.retrieve());
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
        if( !m_trackSelector->decision(*tp) ) {
          if( msgLvl(MSG::VERBOSE) &&  tp->pt() > 5000. )
            ATH_MSG_DEBUG(" Discarding InDet TrackParticle: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() 
              << " Pixel " << getCount(*tp,xAOD::numberOfInnermostPixelLayerHits ) + getCount(*tp,xAOD::numberOfPixelHits )
              << " SCT "  << getCount(*tp,xAOD::numberOfSCTHits ) << " TRT " << getCount(*tp, xAOD::numberOfTRTHits ) );
          continue;
        }
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
          << " Pixel " << getCount(*tp,xAOD::numberOfInnermostPixelLayerHits ) + getCount(*tp,xAOD::numberOfPixelHits )
          << " SCT "  << getCount(*tp,xAOD::numberOfSCTHits ) << " TRT " << getCount(*tp, xAOD::numberOfTRTHits ) );
        if( msgLvl(MSG::VERBOSE) && tp->isAvailable<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink") ){
          ElementLink< xAOD::TruthParticleContainer > truthLink = tp->auxdata<ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
          if( truthLink.isValid() ){
            ATH_MSG_VERBOSE("  Truth particle: pdgId " << (*truthLink)->pdgId() << " type " << tp->auxdata< int >("truthType") 
              << " origin " << tp->auxdata< int >("truthOrigin") << " pt "  
              << (*truthLink)->pt() << " eta " << (*truthLink)->eta() << " phi " << (*truthLink)->phi() );
          }
        }
        InDetCandidate* candidate = new InDetCandidate(link);
        if (m_flagCandidateAsSiAssociated) candidate->setSiliconAssociated(true);
        ++ntracks;
        outputContainer.push_back(candidate);
      }
      ATH_MSG_DEBUG("InDetCandidates selected " << ntracks );
    }

  }	// end of namespace
 
