/* 
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// SATScaleTool.cxx

#include "JetRecTools/SATScaleTool.h"

SATScaleTool::SATScaleTool(const std::string& myname)
: AsgTool(myname),
  TrackAssistHelper(),
  m_inTrackColl(""),
  m_inJetColl(""),
  m_outTrackColl(""),
  m_dRmatch(0.2),
  m_doMerge(true),
  m_dRmerge(0.35),
  m_pTdRmerge(0.3)
{
  declareProperty("InputTrackContainer", m_inTrackColl = "InDetTrackParticles");
  declareProperty("InputJetContainer", m_inJetColl);
  declareProperty("OutputTrackContainer", m_outTrackColl);
  declareProperty("MatchDeltaR", m_dRmatch);
  declareProperty("DoMergeJets", m_doMerge);
  declareProperty("MergeDeltaR", m_dRmerge);
  declareProperty("MergePtOverDeltaR", m_pTdRmerge);

  declareProperty("InputAssociatedTracks", m_assocTracksInName = "GhostTrack");
  declareProperty("VertexContainer", m_vertexColl = "PrimaryVertices");
  declareProperty("TrackVertexAssociation", m_trackVtxAssoc);
  declareProperty("DoTrackVertexAssociation", m_doTrackVtxAssoc = true);
  declareProperty("TrackSelTool", m_trackSelTool);
}

StatusCode SATScaleTool::initialize() 
{

  ATH_MSG_INFO("Initializing SATScaleTool " << name() << ".");
  ATH_CHECK( TrackAssistHelper::initialize() );
  print();

  return StatusCode::SUCCESS;
}

int SATScaleTool::execute() const {

  if( makeSATTracks().isFailure() ) return 1;

  return 0;
}

StatusCode SATScaleTool::makeSATTracks() const {

  std::vector<int> matchedTrackIndices; // List of ghost matched track indices
  std::vector<int> unmatchedTrackIndices; // List of non ghost matched track indices
  std::vector<int> allGoodTrackIndices; // List of all good track indices

  std::map<int, std::vector<int>> JetTrackMap; // Map of track indices to jet indices
  std::map<int, std::vector<int> > JetMergeMap; // Map of jet indices for merging

  // Get input jets
  const xAOD::JetContainer *inJets = nullptr;
  ATH_CHECK( evtStore()->retrieve(inJets,m_inJetColl) );

  // Get input tracks
  const xAOD::TrackParticleContainer *inTracks = nullptr;
  ATH_CHECK( evtStore()->retrieve(inTracks,m_inTrackColl) );

  // Get vertex objects
  const xAOD::Vertex *pvx = nullptr;
  const jet::TrackVertexAssociation *tva = nullptr;
  // NB: getVertexInfo is defined in TrackAssistHelper.cxx
  ATH_CHECK( getVertexInfo(pvx,tva) );

  // Make shallow copy of tracks
  auto trackShallowCopy = xAOD::shallowCopyContainer(*inTracks);
  std::unique_ptr<xAOD::TrackParticleContainer> outTracks (trackShallowCopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> outTracksAux (trackShallowCopy.second);

  // Get list of all tracks
  for( const xAOD::TrackParticle* track : *inTracks ) {
    // Make sure only tracks that pass selection are used
    // NB: isGoodTrack is defined in TrackAssistHelper.cxx
    if( !isGoodTrack(*track,*pvx,*tva) ) continue;
    allGoodTrackIndices.push_back( track->index() );
  }

  // Flag jets for merging
  ATH_CHECK( flagJetsToMerge(inJets) );

  // Loop over input jets
  for( const xAOD::Jet* jet : *inJets ) {

    // Get ghost-associated tracks
    std::vector<const xAOD::TrackParticle*> myMatchedTracks;
    jet->getAssociatedObjects<xAOD::TrackParticle>(m_assocTracksInName,myMatchedTracks);

    // Loop over ghost-associated tracks
    for( const xAOD::TrackParticle* track : myMatchedTracks ) {

      // Skip if link is broken
      if( !track ) continue;

      // Make sure only tracks that pass selection are used
      // NB: isGoodTrack is defined in TrackAssistHelper.cxx
      if( !isGoodTrack(*track,*pvx,*tva) ) continue;

      // Add to list of matched tracks      
      matchedTrackIndices.push_back(track->index());

      // Record which jet the track belongs to
      JetTrackMap[jet->index()].push_back(track->index());
    }     

    // Fill map of jet indices to merge
    int mergeIdx = jet->auxdata< int >("mergeIndex");
    if( mergeIdx >= 0 ) {
      int jetIdx = jet->index();
      // Loop up the chain in case a nearest neighbor is also a jet to merge
      while( inJets->at(mergeIdx)->auxdata< int >("mergeIndex") >= 0 ) {
        mergeIdx = inJets->at(mergeIdx)->auxdata< int >("mergeIndex");
      } 
      JetMergeMap[mergeIdx].push_back(jetIdx);
    }
  }

  // Sort track indices so the difference can be taken
  std::sort( allGoodTrackIndices.begin(), allGoodTrackIndices.end() ); // Sort matched track indices
  std::sort( matchedTrackIndices.begin(), matchedTrackIndices.end() ); // Sort all track indices

  // Get list of unmatched track indices
  std::set_difference(allGoodTrackIndices.begin(), allGoodTrackIndices.end(), matchedTrackIndices.begin(), matchedTrackIndices.end(), std::inserter(unmatchedTrackIndices,unmatchedTrackIndices.begin()));

  // dR-match all remaining tracks
  for( int trackIdx : unmatchedTrackIndices ) {
   
    // Get input track 
    const xAOD::TrackParticle* track = inTracks->at(trackIdx);

    float dRmin = 9999999;
    int jetIdx = -1;
    for( const xAOD::Jet* jet : *inJets ) {
      // Find nearest jet to track
      if( track->p4().DeltaR(jet->p4()) < dRmin ) {
        dRmin = track->p4().DeltaR(jet->p4());
        jetIdx = jet->index();
      }
    }

    // Check if dR matching criteria is met
    if( dRmin < m_dRmatch ) {
      // Record which jet the track belongs to
      JetTrackMap[jetIdx].push_back(track->index());
    }
  }

  // Done matching tracks to small-R jets

  // Container for combined jets (and jets that survive the combination)
  auto combinedJets = std::make_unique<xAOD::JetContainer>();
  auto combinedJetsAux = std::make_unique<xAOD::JetAuxContainer>();
  combinedJets->setStore( combinedJetsAux.get() );

  for( const xAOD::Jet* jet : *inJets ) {

    int mergeIndex = jet->auxdata< int >("mergeIndex");

    // Make sure jet doesn't need to be merged
    if( mergeIndex < 0 ) {

      // Copy jet into combined jet container
      xAOD::Jet* combinedJet = new xAOD::Jet();
      combinedJets->push_back (combinedJet);
      *combinedJet = *jet;

      for( int trackIdx : JetTrackMap[jet->index()] ) {
        // Set jet associations for associated tracks
        // Nominally, each track is associated to a single jet, but this construction
        // allows each track to be associated to multiple jets with a weight for each
        // association that is used in the rescaling
        outTracks->at(trackIdx)->auxdata< std::vector< std::pair<int, float> > >("JetAssociations").emplace_back(combinedJet->index(),1.0);
      }

      // Check if there are any jets to merge
      for( int mergeIdx : JetMergeMap[jet->index()] ) {

        // Add jet 4-vectors together
        combinedJet->setJetP4(combinedJet->jetP4() + inJets->at(mergeIdx)->jetP4());

        for( int trackIdx : JetTrackMap[mergeIdx] ) {
          // Set merged jet track associations to combined jet
          outTracks->at(trackIdx)->auxdata< std::vector< std::pair<int, float> > >("JetAssociations").emplace_back(combinedJet->index(),1.0);
        }

      }

    }
  }

  // Rescale tracks
  ATH_CHECK( rescaleTracks(inJets,outTracks.get())  );

  // Create view container to store only selected tracks
  auto outSelTracks = std::make_unique< ConstDataVector< xAOD::TrackParticleContainer> >(SG::VIEW_ELEMENTS);

  // Fill view container if track passes selection requirements
  for( int goodIdx : allGoodTrackIndices ) {
    outSelTracks->push_back(outTracks->at(goodIdx));
  }
  
  ATH_MSG_DEBUG("New SATTrack container size " << outSelTracks->size());

  ATH_CHECK( evtStore()->record(outSelTracks.release(),"Sel"+m_outTrackColl) );

  // Record output tracks
  ATH_CHECK( evtStore()->record(outTracks.release(),m_outTrackColl) );
  ATH_CHECK( evtStore()->record(outTracksAux.release(),m_outTrackColl+"Aux.") );

  return StatusCode::SUCCESS;
}

StatusCode SATScaleTool::flagJetsToMerge( const xAOD::JetContainer *jets ) const {

  ATH_MSG_DEBUG("In SATScaleTool::flagJetsToMerge");

  for( const xAOD::Jet* jet : *jets ) {
    
    // Don't merge if flag is set to false
    if( !m_doMerge ) {
      jet->auxdecor< int >("mergeIndex") = -1;
      continue;
    }

    float dRmin = 999;
    float ptRatioDR = -1;
    int neighborIndex = -1;

    for ( const xAOD::Jet* jet2 : *jets ) {

      // Don't use jet as its own nearest neighbor
      if( jet == jet2 ) continue;

      float dR = jet->p4().DeltaR(jet2->p4());

      // Find nearest neighbor jet
      if( dR < dRmin ) {
        dRmin = dR;
        // Calculate merging variables
        ptRatioDR = (jet->pt() / jet2->pt()) / dR;
        neighborIndex = jet2->index();
      }

    }

    // Store index of jet to merge into if criteria are met
    if( dRmin < m_dRmerge && ptRatioDR < m_pTdRmerge ) jet->auxdecor< int >("mergeIndex") = neighborIndex;
    else jet->auxdecor< int >("mergeIndex") = -1;

  }

  return StatusCode::SUCCESS;
}

StatusCode SATScaleTool::getTrackVertexAssociation( const jet::TrackVertexAssociation *&tva ) const {

  tva = nullptr;

  // Get TrackVertexAssociation
  ATH_CHECK( evtStore()->retrieve(tva,m_trackVtxAssoc) );

  // Fail if none is found
  if ( !tva ) {
    ANA_MSG_ERROR("Could not retrieve the TrackVertexAssociation");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SATScaleTool::getPrimaryVertex( const xAOD::Vertex *&pvx ) const {

  // Set pvx to nullptr to make sure we are actually retrieving the one we want
  pvx = nullptr;

  // Get vertex container
  const xAOD::VertexContainer *vtxContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(vtxContainer,m_vertexColl) );

  // Fail if no vertex container or an empty vertex container is found
  if ( vtxContainer == 0 || vtxContainer->size() == 0 ) {
    ANA_MSG_ERROR("Failed to retrieve PrimaryVertices collection");
    return StatusCode::FAILURE;
  }

  // Find the first primary vertex in the container
  for ( const xAOD::Vertex* vx : *vtxContainer ) {
    if ( vx->vertexType() == xAOD::VxType::PriVtx ) {
      pvx = vx;
      break;
    }
  }

  // Make sure the primary vertex is found
  if( !pvx ) {
    ANA_MSG_ERROR("No primary vertex assigned" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void SATScaleTool::print() const {
  std::string smerge = m_doMerge ? "true" : "false";
  ATH_MSG_INFO("Properties for SATScaleTool " << name());
  ATH_MSG_INFO("     InputTrackContainer: " << m_inTrackColl);
  ATH_MSG_INFO("       InputJetContainer: " << m_inJetColl);
  ATH_MSG_INFO("    OutputTrackContainer: " << m_outTrackColl);
  ATH_MSG_INFO("             MatchDeltaR: " << m_dRmatch);
  ATH_MSG_INFO("             DoMergeJets: " << smerge);
  ATH_MSG_INFO("             MergeDeltaR: " << m_dRmerge);
  ATH_MSG_INFO("       MergePtOverDeltaR: " << m_pTdRmerge);
  TrackAssistHelper::print();
  return;
}

