/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetCustomVertexFractionTool.cxx

#include "JetMomentTools/JetCustomVertexFractionTool.h"

//**********************************************************************

JetCustomVertexFractionTool::JetCustomVertexFractionTool(const std::string& name) :
JetVertexFractionTool(name) {
  declareProperty("z0cutHighEta",m_z0cutHighEta = 3., "z0sinTheta threshold for tracks with |eta|<z0etaDiv");
  declareProperty("z0cutLowEta", m_z0cutLowEta = 3.,  "z0sinTheta threshold for tracks with |eta|>=z0etaDiv");
  declareProperty("z0etaDiv",    m_z0etaDiv = 1.5,    "|eta| threshold above (below) which z0cutHighEta (z0cutLowEta) is used");
}

std::vector<float> JetCustomVertexFractionTool::getJetVertexTrackSums(const xAOD::Vertex* vertex, 
								  const std::vector<const xAOD::TrackParticle*>& tracks) const {
    std::vector<float> output(3,0);

    bool notsel = m_htsel.empty();
    unsigned int nkeep = 0;
    unsigned int nskip = 0;    
    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack) {
      const xAOD::TrackParticle* track = tracks.at(iTrack);
      if ( notsel || m_htsel->keep(*track) ) {
	output[0] += track->pt();

	const xAOD::Vertex* ptvtx = associatedVertex(track);
	if( ptvtx != nullptr ) {
	  // Track has vertex, assign to appropriate pT sum
	  if ( ptvtx->index() == vertex->index() ) {output[1] += track->pt(); }
	  if ( ptvtx->index() != vertex->index() ) {output[2] += track->pt(); }
	}
	++nkeep;
      }
      else { ++nskip; }
    }
    ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
                  << ": nsel=" << nkeep
	 	  << ", nrej=" << nskip           );

    return output;

}

int JetCustomVertexFractionTool::getPileupTrackCount(const xAOD::Vertex* vertex, 
					     const xAOD::TrackParticleContainer*& tracksCont) const
{
    int n_pileuptrackcount = 0;
    bool notsel = m_htsel.empty();
    unsigned int nkeep = 0;
    unsigned int nskip = 0;
    int tot_count = 0;
    for(size_t iTrack = 0; iTrack < tracksCont->size(); ++iTrack)
      {
	const xAOD::TrackParticle * track = tracksCont->at(iTrack);
	if ( notsel || m_htsel->keep(*track) ) {
	  const xAOD::Vertex* ptvtx = associatedVertex(track);
	  // Count track as PU if associated with non-primary vertex and within pT cut.
	  // N.B. tracks with no vertex associated may be added to PV track sums, but not PU sums, nor the PU vertex counting.
	  if ( ptvtx != nullptr ) {	   
	    if ( (ptvtx->index() != vertex->index() ) && (track->pt() < m_PUtrkptcut) ) ++n_pileuptrackcount;
	  }
	  tot_count++;
	  ++nkeep;
	}
	else { ++nskip; }
      }
    const int n_pileuptracks = n_pileuptrackcount;

    ATH_MSG_VERBOSE("JetCustomVertexFractionTool " << name()
                  << ": nsel=" << nkeep
                  << ", nrej=" << nskip
		 << ", total " << tracksCont->size() );
    ATH_MSG_VERBOSE("JetCustomVertexFractionTool " << name()
		 << ": n_PUtracks=" << n_pileuptracks
		 << ", total=" << tot_count          );

    return n_pileuptracks;
}

const xAOD::Vertex* JetCustomVertexFractionTool::associatedVertex(const xAOD::TrackParticle* track) const {
  // Get the vertices container
  const xAOD::VertexContainer* vertices = NULL;
  if ( evtStore()->retrieve(vertices,m_verticesName).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_verticesName);
    return nullptr;
  }
  ATH_MSG_DEBUG("Successfully retrieved VertexContainer from evtStore: " << m_verticesName);

  const xAOD::Vertex* HSvertex = findHSVertex(vertices);
  if (track->vertex()) return track->vertex();
  if (HSvertex && fabs((track->z0()+track->vz()-HSvertex->z())*sin(track->theta()))<(fabs(track->eta())<m_z0etaDiv?m_z0cutLowEta:m_z0cutHighEta)) return HSvertex;
  return nullptr;
}

//**********************************************************************
// Operate on the full jet container with fewer retrieves and 
// add corrected JVF, which needs full-event information
//**********************************************************************
int JetCustomVertexFractionTool::modify(xAOD::JetContainer& jetCont) const {

  // Get the vertices container
  const xAOD::VertexContainer* vertices = NULL;
  if ( evtStore()->retrieve(vertices,m_verticesName).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_verticesName);
    return 1;
  }
  ATH_MSG_DEBUG("Successfully retrieved VertexContainer from evtStore: " << m_verticesName);
    
  // Get the Tracks container
  const xAOD::TrackParticleContainer* tracksCont = NULL;
  if ( evtStore()->retrieve(tracksCont,m_tracksName).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the TrackParticleContainer from evtStore: " << m_tracksName);
    return 2;
  }
  ATH_MSG_DEBUG("Successfully retrieved TrackParticleContainer from evtStore: " << m_tracksName);
	
  if (vertices->size() == 0 ) { 
    ATH_MSG_WARNING("There are no vertices in the container. Exiting"); 
    return 4;
  }

  const xAOD::Vertex* HSvertex = findHSVertex(vertices);

  // Count pileup tracks - currently done for each collection
  const int n_putracks = getPileupTrackCount(HSvertex, tracksCont);

  for(xAOD::Jet * jet : jetCont) {
    // Get the tracks associated to the jet
    // Note that there may be no tracks - this is both normal and an error case
    std::vector<const xAOD::TrackParticle*> tracks;
    if ( ! jet->getAssociatedObjects(m_assocTracksName, tracks) ) {
      ATH_MSG_DEBUG("Associated tracks not found.");
    }
      
    // Get the track pT sums for all tracks in the jet (first key) and those associated to PU (second key) vertices.
    const std::vector<float> tracksums = getJetVertexTrackSums(HSvertex, tracks);
    // Get the track pT sums for each individual vertex
    std::vector<float> vsumpttrk = jet->getAttribute<std::vector<float> >(m_sumPtTrkName);
    float sumpttrk_nonPV = tracksums[2];
    float sumpttrk_PV = tracksums[1];

    // Default JVFcorr to -1 when no tracks are associated.
    float jvfcorr = -999.;
    if(sumpttrk_PV + sumpttrk_nonPV > 0) {
      jvfcorr = sumpttrk_PV / (sumpttrk_PV + ( sumpttrk_nonPV / (m_kcorrJVF * std::max(n_putracks, 1) ) ) );
    } else {
      jvfcorr = -1;
    }
    jet->setAttribute(m_jvfname+"Corr",jvfcorr);
  }

  return 0;
}
