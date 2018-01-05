/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCandidateTool
//  AlgTool performing pre-selection on MS tracks, extrapolation and creation
//  of MuonCandidate collection. 
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "MuonCandidateTool.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCandidateTool::MuonCandidateTool (const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_trackBuilder("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
      m_trackExtrapolationTool("ExtrapolateMuonToIPTool/ExtrapolateMuonToIPTool"),
      m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonAmbiProcessor")
  {
    declareInterface<IMuonCandidateTool>(this);
    declareProperty("Printer",m_printer );
    declareProperty("ExtrapolationStrategy", m_extrapolationStrategy = 0 );
    declareProperty("TrackBuilder",m_trackBuilder );
    declareProperty("TrackExtrapolationTool",m_trackExtrapolationTool );
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor );
  }

  MuonCandidateTool::~MuonCandidateTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCandidateTool::initialize() {
    ATH_CHECK(m_printer.retrieve());
    if( !m_trackBuilder.empty() )           ATH_CHECK(m_trackBuilder.retrieve());
    else m_trackBuilder.disable();
    if( !m_trackExtrapolationTool.empty() ) ATH_CHECK(m_trackExtrapolationTool.retrieve());
    else m_trackExtrapolationTool.disable();
    ATH_CHECK(m_ambiguityProcessor.retrieve());
    ATH_CHECK(m_evInfo.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCandidateTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCandidateTool::create( const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection ) {
    ATH_MSG_DEBUG("Producing MuonCandidates for " << tracks.size() );
    unsigned int ntracks = 0;
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_evInfo); 
    float beamSpotX = 0.;
    float beamSpotY = 0.;
    float beamSpotZ = 0.;

    if(eventInfo.isValid()){
      beamSpotX = eventInfo->beamPosX();
      beamSpotY = eventInfo->beamPosY();
      beamSpotZ = eventInfo->beamPosZ();
    } 
    ATH_MSG_DEBUG( " Beamspot position  bs_x " << beamSpotX << " bs_y " << beamSpotY << " bs_z " << beamSpotZ);  

      
    // Temporary collection for extrapolated tracks and links with correspondent MS tracks
    std::map<const Trk::Track*, std::pair<ElementLink<xAOD::TrackParticleContainer>, const Trk::Track*> > trackLinks;
    TrackCollection* extrapTracks = new TrackCollection(SG::VIEW_ELEMENTS);
    
    // keep track of track to be deleted
    std::set<const Trk::Track*> tracksToBeDeleted;
    
    unsigned int index = 0;
    // Loop over MS tracks
    for( auto track : tracks ){
      if( !track->trackLink().isValid() || track->track() == 0 ) {
        ATH_MSG_WARNING("MuonStandalone track particle without Trk::Track");
        continue;
      }
      ElementLink<xAOD::TrackParticleContainer> trackLink(tracks,index++);
      //trackLink.toPersistent();

      const Trk::Track& msTrack = *track->track();

      ATH_MSG_VERBOSE("Re-Fitting track " << std::endl << m_printer->print(msTrack) << std::endl << m_printer->printStations(msTrack));
      Trk::Track* standaloneTrack = 0;
      const Trk::Vertex* vertex = 0;
      if( m_extrapolationStrategy == 0 ) standaloneTrack = m_trackBuilder->standaloneFit(msTrack, vertex, beamSpotX, beamSpotY, beamSpotZ);
      else                               standaloneTrack = m_trackExtrapolationTool->extrapolate(msTrack);
      if (standaloneTrack) {
	standaloneTrack->info().setParticleHypothesis(Trk::muon);
	standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandAlone);
	ATH_MSG_VERBOSE("Extrapolated track " << std::endl << m_printer->print(*standaloneTrack) << std::endl << m_printer->printStations(*standaloneTrack));
	++ntracks;
	if( !standaloneTrack->perigeeParameters() )                    ATH_MSG_WARNING(" Track without perigee " << standaloneTrack);
        else if( !standaloneTrack->perigeeParameters()->covariance() ) ATH_MSG_WARNING(" Track with perigee without covariance " << standaloneTrack);
	trackLinks[ standaloneTrack ] = std::make_pair(trackLink,standaloneTrack);
	extrapTracks->push_back( standaloneTrack );
        tracksToBeDeleted.insert( standaloneTrack ); // insert track for deletion
      }else{
	trackLinks[ &msTrack ] = std::make_pair(trackLink,standaloneTrack);
	extrapTracks->push_back( const_cast<Trk::Track*>(&msTrack) );	
      }      
    }
    ATH_MSG_DEBUG("Finished back-tracking, total number of successfull fits " << ntracks);

    // Resolve ambiguity between extrapolated tracks (where available)
    TrackCollection* resolvedTracks = m_ambiguityProcessor->process(extrapTracks); 
    
    ATH_MSG_DEBUG("Finished ambiguity solving: "<<extrapTracks->size()<<" track(s) in -> "<<resolvedTracks->size()<<" track(s) out");
    

    // Loop over resolved tracks and build MuonCondidate collection
    for( auto track : *resolvedTracks ) {
      auto tLink = trackLinks.find(track);
      if(tLink == trackLinks.end()) {
	ATH_MSG_WARNING("Unable to find internal link between MS and SA tracks!");
	continue;
      }      
      auto tpair = tLink->second;
      
      outputCollection.push_back( new MuonCandidate(tpair.first, tpair.second) );
      // remove track from set so it is not deleted
      tracksToBeDeleted.erase(tpair.second);
    }
    
    if( extrapTracks->size() != resolvedTracks->size() + tracksToBeDeleted.size() )
      ATH_MSG_WARNING(" inconsistent number of tracks: in " << extrapTracks->size() << " resolved " << resolvedTracks->size()
                      << " remaining " << tracksToBeDeleted.size() );

    // delete all remaining tracks in the set
    for( auto it = tracksToBeDeleted.begin();it!=tracksToBeDeleted.end();++it ) delete *it;
    delete extrapTracks;
    delete resolvedTracks;
  }
}	// end of namespace
