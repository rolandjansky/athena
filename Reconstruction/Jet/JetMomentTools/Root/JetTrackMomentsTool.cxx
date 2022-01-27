/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentTools/JetTrackMomentsTool.h"
#include <sstream>
#include "JetUtils/JetDistances.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "AsgDataHandles/WriteDecorHandle.h"

JetTrackMomentsTool::JetTrackMomentsTool(const std::string& name)
    : asg::AsgTool(name)
{
}


//**********************************************************************

StatusCode JetTrackMomentsTool::initialize() {

  ATH_MSG_INFO("Initializing JetTrackMomentsTool " << name());
  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetTrackMomentsTool needs to have its input container name configured!");
    return StatusCode::FAILURE;
  }

  // Set up the decoration names for each track pt cut
  for (size_t iCut = 0; iCut < m_minTrackPt.size(); ++iCut) {
    const float minPt = m_minTrackPt[iCut];
    const std::string baseName = getMomentBaseName(minPt);
    m_keysNumTrk.emplace_back(  m_jetContainerName + ".NumTrk"     + baseName + m_suffix);
    m_keysSumPtTrk.emplace_back(m_jetContainerName + ".SumPtTrk"   + baseName + m_suffix);
    m_keysTrkWidth.emplace_back(m_jetContainerName + ".TrackWidth" + baseName + m_suffix);
    if(m_doPFlowMoments){
      m_keysNumCPFO.emplace_back(  m_jetContainerName + ".NumChargedPFO"   + baseName + m_suffix);
      m_keysSumPtCPFO.emplace_back(m_jetContainerName + ".SumPtChargedPFO" + baseName + m_suffix);
      m_keysCPFOWidth.emplace_back(m_jetContainerName + ".ChargedPFOWidth" + baseName + m_suffix);
    }
  }

  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_tva_key.initialize());
  ATH_CHECK(m_keysNumTrk.initialize());
  ATH_CHECK(m_keysSumPtTrk.initialize());
  ATH_CHECK(m_keysTrkWidth.initialize());
  ATH_CHECK(m_keysNumCPFO.initialize(m_doPFlowMoments));
  ATH_CHECK(m_keysSumPtCPFO.initialize(m_doPFlowMoments));
  ATH_CHECK(m_keysCPFOWidth.initialize(m_doPFlowMoments));

  return StatusCode::SUCCESS;
}

//**********************************************************************


StatusCode JetTrackMomentsTool::decorate(const xAOD::JetContainer& jets) const {

  // Get input vertex collection
  auto handle_v = SG::makeHandle (m_vertexContainer_key);
  if (!handle_v.isValid()){
    ATH_MSG_ERROR("Could not retrieve the VertexContainer: "
                  << m_vertexContainer_key.key());
    return StatusCode::FAILURE;
  }

  auto vertexContainer = handle_v.cptr();

  // Get the track-vertex association
  auto handle_tva = SG::makeHandle (m_tva_key);
  if (!handle_tva.isValid()){
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation: "
                  << m_tva_key.key());
    return StatusCode::FAILURE;
  }

  auto tva = handle_tva.cptr();

  for(const xAOD::Jet* jet : jets){
    ATH_MSG_VERBOSE("On jet " << jet->index());

    // Retrieve the associated tracks.
    std::vector<const xAOD::TrackParticle*> tracks;
    bool havetracks = jet->getAssociatedObjects(m_assocTracksName, tracks);
    if(!havetracks) ATH_MSG_WARNING("Associated tracks not found");
    ATH_MSG_VERBOSE("Successfully retrieved track particles for jet " << jet->index());

    std::vector<const xAOD::TrackParticle*> pflowTracks;
    bool isPFlowJet = false;
    if(m_doPFlowMoments) {
      // Check if this is a PFlow jet. If so, get the PFO tracks.
      xAOD::Type::ObjectType ctype = jet->rawConstituent(0)->type();
      if (ctype  == xAOD::Type::ParticleFlow) {
        isPFlowJet = true;
        size_t numConstit = jet->numConstituents();
        for ( size_t i=0; i<numConstit; i++ ) {
          const xAOD::PFO* constit = dynamic_cast<const xAOD::PFO*>(jet->rawConstituent(i));
          if (constit->isCharged()){
            const xAOD::TrackParticle *thisTrack = constit->track(0);//by construction xAOD::PFO can only have one track, in eflowRec usage
            pflowTracks.push_back(thisTrack);
          }// We have a charged PFO
        }// Loop on jet constituents
      }// This jet is made from xAOD::PFO, so we do calculate the pflow moments
      else if (ctype  == xAOD::Type::FlowElement) {
        size_t numConstit = jet->numConstituents();
        for ( size_t i=0; i<numConstit; i++ ) {
          const xAOD::FlowElement* constit = dynamic_cast<const xAOD::FlowElement*>(jet->rawConstituent(i));
          if(constit != nullptr && (constit->signalType() & xAOD::FlowElement::PFlow)){
            isPFlowJet = true;
            if (constit->isCharged()){
              const xAOD::TrackParticle *thisTrack = dynamic_cast<const xAOD::TrackParticle*>(constit->chargedObject(0));//PFO should have only 1 track
              if(thisTrack != nullptr) pflowTracks.push_back(thisTrack);
              else ATH_MSG_WARNING("Charged PFO had no associated TrackParticle");
            }// We have a charged PFO
          }// The FlowElement is a PFO
        }// Loop on jet constituents
      }// This jet is made from xAOD::FlowElement, so we calculate the pflow moments if they're PFOs
    }// We actually want to calculate the PFlow moments

    // For each track cut, compute and set the associated moments
    for (size_t iCut = 0; iCut < m_minTrackPt.size(); ++iCut) {
      SG::WriteDecorHandle<xAOD::JetContainer, std::vector<int> > numTrkHandle(m_keysNumTrk.at(iCut));
      SG::WriteDecorHandle<xAOD::JetContainer, std::vector<float> > sumPtTrkHandle(m_keysSumPtTrk.at(iCut));
      SG::WriteDecorHandle<xAOD::JetContainer, std::vector<float> > trkWidthHandle(m_keysTrkWidth.at(iCut));
      // Get info
      const float minPt = m_minTrackPt[iCut];
      const std::vector<TrackMomentStruct> moments = getTrackMoments(*jet,vertexContainer,minPt,tracks,tva);
      // Collate info
      std::vector<int>   numTrkVec;       numTrkVec.resize(moments.size());
      std::vector<float> sumPtTrkVec;     sumPtTrkVec.resize(moments.size());
      std::vector<float> trackWidthVec;   trackWidthVec.resize(moments.size());
      for ( size_t iVertex = 0; iVertex < moments.size(); ++iVertex ) {
        numTrkVec[iVertex]     = moments.at(iVertex).numTrk;
        sumPtTrkVec[iVertex]   = moments.at(iVertex).sumPtTrk;
        trackWidthVec[iVertex] = moments.at(iVertex).trackWidth;
      }
      // Set moment decorations
      numTrkHandle(*jet) = numTrkVec;
      sumPtTrkHandle(*jet) = sumPtTrkVec;
      trkWidthHandle(*jet) = trackWidthVec;

      if(m_doPFlowMoments) {
        SG::WriteDecorHandle<xAOD::JetContainer, std::vector<int> > numCPFOHandle(m_keysNumCPFO.at(iCut));
        SG::WriteDecorHandle<xAOD::JetContainer, std::vector<float> > sumPtCPFOHandle(m_keysSumPtCPFO.at(iCut));
        SG::WriteDecorHandle<xAOD::JetContainer, std::vector<float> > cPFOWidthHandle(m_keysCPFOWidth.at(iCut));
        if(isPFlowJet){

          const std::vector<TrackMomentStruct> pflowMoments = getTrackMoments(*jet,vertexContainer,minPt,pflowTracks,tva);

          std::vector<int>   pflowNumTrkVec;       pflowNumTrkVec.resize(pflowMoments.size());
          std::vector<float> pflowSumPtTrkVec;     pflowSumPtTrkVec.resize(pflowMoments.size());
          std::vector<float> pflowTrackWidthVec;   pflowTrackWidthVec.resize(pflowMoments.size());
          for ( size_t iVertex = 0; iVertex < pflowMoments.size(); ++iVertex ) {
            pflowNumTrkVec[iVertex]     = pflowMoments.at(iVertex).numTrk;
            pflowSumPtTrkVec[iVertex]   = pflowMoments.at(iVertex).sumPtTrk;
            pflowTrackWidthVec[iVertex] = pflowMoments.at(iVertex).trackWidth;
          }
          // Set moment decorations
          numCPFOHandle(*jet) = pflowNumTrkVec;
          sumPtCPFOHandle(*jet) = pflowSumPtTrkVec;
          cPFOWidthHandle(*jet) = pflowTrackWidthVec;
        }
        else{
          // User configured for PFO track moments but this isn't a PFlow jet. Set them to empty vectors.
          numCPFOHandle(*jet) = std::vector<int>();
          sumPtCPFOHandle(*jet) = std::vector<float>();
          cPFOWidthHandle(*jet) = std::vector<float>();
        } // Should find a more graceful way to complain?
      }// do PF moments
    }
  }

  return StatusCode::SUCCESS;
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
    TrackMomentStruct moments{};
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


