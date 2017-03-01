/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentTools/JetTrackMomentsTool.h"
#include <sstream>
#include "JetUtils/JetDistances.h"
#include "xAODPFlow/PFO.h"

JetTrackMomentsTool::JetTrackMomentsTool(const std::string& name)
    : JetModifierBase(name)
    , m_vertexContainer("")
    , m_assocTracksName("")
    , m_tva("")
    , m_minTrackPt()
    , m_htsel("")
{
    declareProperty("VertexContainer",m_vertexContainer);
    declareProperty("AssociatedTracks",m_assocTracksName);
    declareProperty("TrackVertexAssociation",m_tva);
    declareProperty("TrackMinPtCuts",m_minTrackPt);
    declareProperty("TrackSelector", m_htsel);
}


//**********************************************************************

StatusCode JetTrackMomentsTool::initialize() {
  ATH_MSG_INFO("Initializing JetTrackMomentsTool " << name());
  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************


int JetTrackMomentsTool::modifyJet(xAOD::Jet& jet) const {

  // Get input vertex collection
  const xAOD::VertexContainer* vertexContainer = nullptr;
  if ( evtStore()->retrieve(vertexContainer,m_vertexContainer).isFailure()
       || vertexContainer == nullptr ) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: "
                  << m_vertexContainer);
    return 1;
  }

  // Get the track-vertex association
  const jet::TrackVertexAssociation* tva = nullptr;
  if ( evtStore()->retrieve(tva,m_tva).isFailure() || tva==nullptr ) {
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: "
                  << m_tva);
    return 2;
  }

#if 0
    // Get the tracks associated to the jet
    // Note that there may be no tracks
    const std::vector<const xAOD::IParticle*> tracksAsParticles;
    bool havetracks = jet.getAssociatedObjects(m_assocTracksName, tracksAsParticles);

    if ( ! havetracks ) {
      ATH_MSG_DEBUG("Associated tracks not found");
    }

    // Do the dynamic_cast once for the particles instead of repeatedly
    std::vector<const xAOD::TrackParticle*> tracks;
    tracks.resize(tracksAsParticles.size());
    for (size_t iTrack = 0; iTrack < tracksAsParticles.size(); ++iTrack)
    {
        const xAOD::TrackParticle* track = dynamic_cast<const xAOD::TrackParticle*>(tracksAsParticles.at(iTrack));
        if (!track)
        {
            ATH_MSG_ERROR("Error converting index " << iTrack << " from IParticle to TrackParticle");
            return 3;
        }
        tracks[iTrack] = track;
    }
#endif

  // Retrieve the associated tracks.
  std::vector<const xAOD::TrackParticle*> tracks;
  bool havetracks = jet.getAssociatedObjects(m_assocTracksName, tracks);
  if ( ! havetracks ) ATH_MSG_WARNING("Associated tracks not found");
  ATH_MSG_DEBUG("Successfully retrieved track particles");
  
  //For PFlow jets we will also calculate the same moments, using charged PFO                                                                                                                                                                                                        
  xAOD::Type::ObjectType ctype = jet.rawConstituent( 0 )->type();
  std::vector<const xAOD::TrackParticle*> pflowTracks;
  bool isPFlowJet = false;
  if (ctype  == xAOD::Type::ParticleFlow) {
    isPFlowJet = true;
    size_t numConstit = jet.numConstituents();
    for ( size_t i=0; i<numConstit; i++ ) {
      const xAOD::PFO* constit = dynamic_cast<const xAOD::PFO*>(jet.rawConstituent(i));
      if (0.0 != constit->charge()){
        const xAOD::TrackParticle *thisTrack = constit->track(0);//by construction xAOD::PFO can only have one track, in eflowRec usage                                                                                                                                             
        pflowTracks.push_back(thisTrack);
      }//we have a charged PFO                                                                                                                                                                                                                                                      
    }//loop on jet constituents         
  }//yes this jet is made form xAOD::PFO, so we do calculate the pflow moments           

  // For each track cut, get the associated moments
  for (size_t iCut = 0; iCut < m_minTrackPt.size(); ++iCut) {
    // Get info
    const float minPt = m_minTrackPt.at(iCut);
    const std::vector<TrackMomentStruct> moments =
      getTrackMoments(jet,vertexContainer,minPt,tracks,tva);
    const std::string baseName = getMomentBaseName(minPt);
    // Collate info
    std::vector<int>   numTrkVec;       numTrkVec.resize(moments.size());
    std::vector<float> sumPtTrkVec;     sumPtTrkVec.resize(moments.size());
    std::vector<float> trackWidthVec;   trackWidthVec.resize(moments.size());
    for ( size_t iVertex = 0; iVertex < moments.size(); ++iVertex ) {
      numTrkVec[iVertex]     = moments.at(iVertex).numTrk;
      sumPtTrkVec[iVertex]   = moments.at(iVertex).sumPtTrk;
      trackWidthVec[iVertex] = moments.at(iVertex).trackWidth;
    }
    // Set info
    jet.setAttribute("NumTrk"+baseName,numTrkVec);
    jet.setAttribute("SumPtTrk"+baseName,sumPtTrkVec);
    jet.setAttribute("TrackWidth"+baseName,trackWidthVec);

    if (true == isPFlowJet){
      const std::vector<TrackMomentStruct> pflowMoments = getTrackMoments(jet,vertexContainer,minPt,pflowTracks,tva);

      std::vector<int>   pflowNumTrkVec;       pflowNumTrkVec.resize(pflowMoments.size());
      std::vector<float> pflowSumPtTrkVec;     pflowSumPtTrkVec.resize(pflowMoments.size());
      std::vector<float> pflowTrackWidthVec;   pflowTrackWidthVec.resize(pflowMoments.size());
      for ( size_t iVertex = 0; iVertex < pflowMoments.size(); ++iVertex ) {
	pflowNumTrkVec[iVertex]     = pflowMoments.at(iVertex).numTrk;
	pflowSumPtTrkVec[iVertex]   = pflowMoments.at(iVertex).sumPtTrk;
	pflowTrackWidthVec[iVertex] = pflowMoments.at(iVertex).trackWidth;
      }
      // Set info                                                                                                                                                                                                                                                                      
      jet.setAttribute("NumChargedPFO"+baseName,pflowNumTrkVec);
      jet.setAttribute("SumPtChargedPFO"+baseName,pflowSumPtTrkVec);
      jet.setAttribute("ChargedPFOWidth"+baseName,pflowTrackWidthVec);

    }//yes this is a pflow jet, and so we calculate the additional pflow specific  moments
  }

  return 0;
}


const std::vector<JetTrackMomentsTool::TrackMomentStruct> JetTrackMomentsTool::getTrackMoments(const xAOD::Jet& jet, const xAOD::VertexContainer* vertices, const float minTrackPt, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
{
    std::vector<TrackMomentStruct> moments;
    moments.resize(vertices->size());

    for (size_t iVertex = 0; iVertex < vertices->size(); ++iVertex)
        moments[iVertex] = getTrackMoments(jet,vertices->at(iVertex),minTrackPt,tracks,tva);

    return moments;
}

JetTrackMomentsTool::TrackMomentStruct JetTrackMomentsTool::getTrackMoments(const xAOD::Jet& jet, const xAOD::Vertex* vertex, const float minTrackPt, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const
{
    // Prepare the moments
    TrackMomentStruct moments;
    moments.numTrk = 0;
    moments.sumPtTrk = 0;
    moments.trackWidth = 0;

    // Prepare const vars for jet eta/phi
    const float jetEta = jet.eta();
    const float jetPhi = jet.phi();

    // Track selection and counters
    bool notsel = m_htsel.empty();
    unsigned int nkeep = 0;
    unsigned int nskip = 0;

    // Loop over the tracks
    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack)
    {
        const xAOD::TrackParticle* track = tracks.at(iTrack);
        const float trackPt = track->pt();
        
        // Skip the track if it doesn't pass the cut
        if (trackPt < minTrackPt)
            continue;

        // Skip the track if it's not from the vertex in question
        if( vertex == nullptr || vertex != tva->associatedVertex(track) ) continue ;

	// Check track passes track selection, otherwise mark as skipped 
	if ( notsel || m_htsel->keep(*track) ) {
	  ++nkeep;
	  
	  // Calculate necessary info for the moments
	  const double deltaR = jet::JetDistances::deltaR(jetEta, jetPhi, track->eta(),  track->phi() );
	  
	  // Adjust values as necessary for this track
	  moments.numTrk     += 1;
	  moments.sumPtTrk   += trackPt;
	  moments.trackWidth += deltaR * trackPt;
	}
	else { ++nskip;}
    }

    // Finish processing the moments
    moments.trackWidth = moments.sumPtTrk > 0 ? moments.trackWidth / moments.sumPtTrk : -1;
 
    ATH_MSG_VERBOSE("JetTrackMomentsTool " << name()
		    << ": nsel=" << nkeep
		    << ", nrej=" << nskip );

    
    return moments;
}

const std::string JetTrackMomentsTool::getMomentBaseName(const float minTrackPt) const
{
    int value = round(minTrackPt);
    if (fabs(value - minTrackPt) > 0.1)
        ATH_MSG_WARNING("Cut float and int disagree: " << minTrackPt << " float vs " << value << " int");

    std::ostringstream sout;
    sout << "Pt" << value;
    return sout.str();
}


