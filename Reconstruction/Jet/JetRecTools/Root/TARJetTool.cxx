/* 
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TARJetTool.cxx

#include "JetRecTools/TARJetTool.h"

TARJetTool::TARJetTool(const std::string& myname)
: AsgTool(myname),
  TrackAssistHelper(),
  m_inTrackColl(""),
  m_inSelTrackColl(""),
  m_outTrackColl(""),
  m_assocTracksOutName(""),
  m_dRmatch(0.3)
{
  declareProperty("InputTrackContainer", m_inTrackColl = "InDetTrackParticles");
  declareProperty("InputSelectedTrackContainer", m_inSelTrackColl); // Name of a view container to allow a track preselection
  declareProperty("OutputTrackContainer", m_outTrackColl);
  declareProperty("OutputAssociatedTracks", m_assocTracksOutName = "TARTracks");
  declareProperty("OutputAssociatedObjects", m_assocObjsOutName = "TARObjects");
  declareProperty("MatchDeltaR", m_dRmatch);

  declareProperty("InputAssociatedTracks", m_assocTracksInName = "GhostTrack");
  declareProperty("VertexContainer", m_vertexColl = "PrimaryVertices");
  declareProperty("TrackVertexAssociation", m_trackVtxAssoc);
  declareProperty("DoTrackVertexAssociation", m_doTrackVtxAssoc = true);
  declareProperty("TrackSelTool", m_trackSelTool);
}

StatusCode TARJetTool::initialize() 
{

  ATH_MSG_INFO("Initializing TARJetTool " << name() << ".");

  // If no view container is provided, use the full input track container
  if( m_inSelTrackColl.empty() ) {
    m_inSelTrackColl = m_inTrackColl;
  }
  ATH_CHECK( TrackAssistHelper::initialize() );
  print();

  return StatusCode::SUCCESS;
}

int TARJetTool::modify( xAOD::JetContainer& inJets ) const {

  std::vector<int> matchedTrackIndices; // List of ghost matched track indices
  std::vector<int> unmatchedTrackIndices; // List of non ghost matched track indices
  std::vector<int> allGoodTrackIndices; // List of all good track indices

  std::map<int, std::vector<int>> JetTrackMap; // Map of track indices to RC jet indices
  std::map<int, std::vector<int>> ConstitTrackMap; // Map of track indices to constit jet indices
  std::map<int, int> ConstitJetMap; // Map of all constituent jets to RC jets
  std::map<int, int> RawConstitMap; // Map of all constituent jets to RC jet raw constituents

  // Get input tracks
  const xAOD::TrackParticleContainer *inTracks = nullptr;
  ATH_CHECK( evtStore()->retrieve(inTracks,m_inTrackColl) );

  // Get input selected tracks
  const xAOD::TrackParticleContainer *inSelTracks = nullptr;
  ATH_CHECK( evtStore()->retrieve(inSelTracks,m_inSelTrackColl) );

  // Get vertex objects
  const xAOD::Vertex *pvx = nullptr;
  const jet::TrackVertexAssociation *tva = nullptr;
  // NB: getVertexInfo is defined in TrackAssistHelper.cxx
  ATH_CHECK( getVertexInfo(pvx,tva) );

  // Make container for constituent jets
  auto constitJets = std::make_unique<xAOD::JetContainer>();
  auto constitJetsAux = std::make_unique<xAOD::JetAuxContainer>();
  constitJets->setStore( constitJetsAux.get() ); //< Connect the two

  // Make shallow copy of tracks
  auto trackShallowCopy = xAOD::shallowCopyContainer(*inTracks);
  std::unique_ptr<xAOD::TrackParticleContainer> outTracks (trackShallowCopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> outTracksAux (trackShallowCopy.second);

  // Map of track indices to position in the input collection
  // This is needed in case a view container is used
  std::map<size_t,size_t> m_availableTrackIndices;

  // Counter to keep track of track positions in inSelTracks
  size_t iTrack = 0;

  // Create map of available selected track indices and make list of all good selected tracks
  for( const xAOD::TrackParticle* track : *inSelTracks ) {
    // Fill set of track indices in case inSelTracks is a subset
    // of the tracks used for ghost-association
    if( !(m_availableTrackIndices.insert(std::make_pair(track->index(),iTrack)).second) ) {
      // This shouldn't happen, but this serves as a sanity check
      ANA_MSG_WARNING("Found more than one track with index " << track->index());
    }
    iTrack++;

    // Make sure only tracks that pass selection are used
    // NB: isGoodTrack is defined in TrackAssistHelper.cxx
    if( !isGoodTrack(*track,*pvx,*tva) ) continue;

    allGoodTrackIndices.push_back( track->index() );
  }

  // Loop over RC jets
  for( xAOD::Jet* jet : inJets ) {

    // Loop over constituent jets
    for ( unsigned int iConstit = 0; iConstit < jet->numConstituents(); iConstit++ ){

      // Get constituent jet
      const xAOD::Jet *constit = dynamic_cast< const xAOD::Jet* >( jet->rawConstituent(iConstit) );

      // Copy constituent jet into temporary small-R jet collection
      xAOD::Jet* constitJet = new xAOD::Jet();
      constitJets->push_back (constitJet);
      *constitJet = *constit;

      // Record which RC jet constituent jet comes from
      ConstitJetMap[constitJet->index()] = jet->index();
      
      // Record which RC jet raw constituent it is
      RawConstitMap[constitJet->index()] = iConstit;

      // Get ghost-associated tracks
      std::vector<const xAOD::TrackParticle*> myMatchedTracks;
      constitJet->getAssociatedObjects<xAOD::TrackParticle>(m_assocTracksInName,myMatchedTracks);

      // Loop over ghost-associated tracks
      for( const xAOD::TrackParticle* track : myMatchedTracks ) {

        // Skip if link is broken
        if( !track ) continue;

        // Skip if index does not exist in input track collection
        if( !m_availableTrackIndices.count(track->index()) ) continue;

        // Make sure only tracks that pass selection are used
        // NB: isGoodTrack is defined in TrackAssistHelper.cxx
        if( !isGoodTrack(*track,*pvx,*tva) ) continue;

        // Add to list of matched tracks
        matchedTrackIndices.push_back(track->index());

        // Record which constituent jet and RC jet the track belongs to
        ConstitTrackMap[constitJet->index()].push_back(track->index());
        JetTrackMap[jet->index()].push_back(track->index());
      }

    }
  }

  // Sort track indices so the difference can be taken
  std::sort( allGoodTrackIndices.begin(), allGoodTrackIndices.end() ); // Sort matched track indices
  std::sort( matchedTrackIndices.begin(), matchedTrackIndices.end() ); // Sort all track indices

  // Get list of unmatched track indices
  std::set_difference(allGoodTrackIndices.begin(), allGoodTrackIndices.end(), matchedTrackIndices.begin(), matchedTrackIndices.end(), std::inserter(unmatchedTrackIndices,unmatchedTrackIndices.begin()));

  // Loop over all remaining tracks
  for( int trackIdx : unmatchedTrackIndices ) {

    // Get input track
    const xAOD::TrackParticle* track = inSelTracks->at(m_availableTrackIndices[trackIdx]);

    float dRmin = 9999999;
    int jetIdx = -1;

    for( xAOD::Jet* constitJet : *constitJets ) {
      // Find nearest small-R jet to track
      if( track->p4().DeltaR(constitJet->p4()) < dRmin ) {
        dRmin = track->p4().DeltaR(constitJet->p4());
        jetIdx = constitJet->index();
      }
    }

    // Check if dR matching criteria is met
    if( dRmin < m_dRmatch ) {
      // Record which constituent jet and RC jet track belongs to
      ConstitTrackMap[jetIdx].push_back(track->index());
      JetTrackMap[ConstitJetMap[jetIdx]].push_back(track->index());
    }
  }

  // Vector to hold all trackless constituent jets
  std::vector<int> tracklessConstits;
  
  // Loop over all constituent jets to assign jet associations to tracks
  // Trackless constituent jets are stored separately
  // NB: This is being done in a separate loop to make future modifications easier
  for( xAOD::Jet* constitJet : *constitJets ) {

    if( ConstitTrackMap[constitJet->index()].size() ) {
      for( int trackIdx : ConstitTrackMap[constitJet->index()] ) {
        // Set jet associations for associated tracks
        // Nominally, each track is associated to a single jet, but this construction
        // allows each track to be associated to multiple jets with a weight for each
        // association that is used in the rescaling
        // Since outTracks is a shallow copy of inTracks, the track index can be used directly
        outTracks->at(trackIdx)->auxdata< std::vector< std::pair<int, float> > >("JetAssociations").emplace_back(constitJet->index(),1.0);
      }
    }
    else {
      tracklessConstits.push_back(constitJet->index());
    }

  }

  // Rescale tracks
  ATH_CHECK( rescaleTracks(constitJets.get(),outTracks.get())  );

  // Create view container to store only selected tracks
  auto outSelTracks = std::make_unique< ConstDataVector< xAOD::TrackParticleContainer> >(SG::VIEW_ELEMENTS);

  // Fill view container if track passes selection requirements
  for( int goodIdx : allGoodTrackIndices ) {
    outSelTracks->push_back(outTracks->at(goodIdx));
  }

  ATH_MSG_DEBUG("New TARTrack container size " << outSelTracks->size());

  ATH_CHECK( evtStore()->record(outSelTracks.release(),"Sel"+m_outTrackColl) );

  // Get bare pointer for post-record operations
  xAOD::TrackParticleContainer* pOutTracks = outTracks.get();

  // Record output tracks
  ATH_CHECK( evtStore()->record(outTracks.release(), m_outTrackColl) );
  ATH_CHECK( evtStore()->record(outTracksAux.release(), m_outTrackColl+"Aux.") );

  // Loop over input jets to link tracks
  for( xAOD::Jet* jet : inJets ) {

    std::vector<const xAOD::TrackParticle*> TARTracks;
    std::vector<const xAOD::IParticle*> TARObjs;

    // Sum of TAR tracks for mTARTrk
    TLorentzVector TARTrkJet;

    // Sum of TAR tracks and trackless jets for mTAR
    TLorentzVector TARJet;

    // Collect all tracks associated to RC jet
    for( int trackIdx : JetTrackMap[jet->index()] ) {
      TARTracks.push_back(pOutTracks->at(trackIdx));
      TARObjs.push_back(pOutTracks->at(trackIdx));

      TARJet += pOutTracks->at(trackIdx)->p4();
      TARTrkJet += pOutTracks->at(trackIdx)->p4();
    }

    // Add all of the trackless constituents
    for( int constitIdx : tracklessConstits ) {
      if( ConstitJetMap[constitIdx] != (int)jet->index() ) continue;
      TARObjs.push_back( jet->rawConstituent(RawConstitMap[constitIdx]) );
      TARJet += constitJets->at(constitIdx)->p4();
    }

    // Add associated TAR tracks and trackless jets
    jet->setAssociatedObjects< xAOD::IParticle >(m_assocObjsOutName,TARObjs);

    // Add associated TAR tracks
    jet->setAssociatedObjects< xAOD::TrackParticle >(m_assocTracksOutName,TARTracks);

    // Add mTAR and mTARTrk decorations
    jet->setAttribute("mTAR", TARJet.M());
    jet->setAttribute("mTARTrk", TARTrkJet.M());

    // Add TAR constituents multiplicity decorations
    jet->setAttribute("nTARTrk", TARTracks.size());
    jet->setAttribute("nTARObj", TARObjs.size());

  }

  return 0;
}

StatusCode TARJetTool::getTrackVertexAssociation( const jet::TrackVertexAssociation *&tva ) const {

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

StatusCode TARJetTool::getPrimaryVertex( const xAOD::Vertex *&pvx ) const {

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

void TARJetTool::print() const {
  ATH_MSG_INFO("Properties for TARJetTool " << name());
  ATH_MSG_INFO("        InputTrackContainer: " << m_inTrackColl);
  ATH_MSG_INFO("InputSelectedTrackContainer: " << m_inSelTrackColl);
  ATH_MSG_INFO("       OutputTrackContainer: " << m_outTrackColl);
  ATH_MSG_INFO("     OutputAssociatedTracks: " << m_assocTracksOutName);
  ATH_MSG_INFO("                MatchDeltaR: " << m_dRmatch);
  TrackAssistHelper::print();
  return;
}

