/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexFractionTool.cxx

#include "JetMomentTools/JetVertexFractionTool.h"

//**********************************************************************

JetVertexFractionTool::JetVertexFractionTool(const std::string& name)
: JetModifierBase(name)
, m_assocTracksName("")
, m_tvaName("")
, m_tracksName("")
, m_htsel("") {
  declareProperty("VertexContainer", m_verticesName);
  declareProperty("AssociatedTracks", m_assocTracksName);
  declareProperty("TrackVertexAssociation", m_tvaName);
  declareProperty("TrackParticleContainer",m_tracksName);
  declareProperty("SumPtTrkName",m_sumPtTrkName="SumPtTrkPt500");
  declareProperty("TrackSelector", m_htsel);
  declareProperty("JVFName", m_jvfname ="JVF");
  declareProperty("K_JVFCorrScale",m_kcorrJVF = 0.01);
  declareProperty("PUTrkPtCut",m_PUtrkptcut = 30000.);
  declareProperty("IsTrigger",m_isTrigger =false);
}

//**********************************************************************

StatusCode JetVertexFractionTool::initialize() {
  ATH_MSG_INFO("Initializing JetVertexFractionTool " << name());
  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }
  ATH_MSG_INFO("  Attribute name: " << m_jvfname);
  return StatusCode::SUCCESS;
}

//**********************************************************************
// Legacy version that uses the direct computation of sums over tracks
//**********************************************************************

int JetVertexFractionTool::modifyJet(xAOD::Jet& jet) const {
	
  // Get the vertices container
  const xAOD::VertexContainer* vertices = NULL;
  if ( evtStore()->retrieve(vertices,m_verticesName).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_verticesName);
    return 1;
  }
  ATH_MSG_DEBUG("Successfully retrieved VertexContainer from evtStore: " << m_verticesName);
	   
  // Get the tracks associated to the jet
  // Note that there may be no tracks - this is both normal and an error case
  // In this case, just fill a vector with zero and don't set the highest vtx moment
  std::vector<const xAOD::TrackParticle*> tracks;
  if ( ! jet.getAssociatedObjects(m_assocTracksName, tracks) ) {
    ATH_MSG_WARNING("Associated tracks not found.");
  }
	
  // Get the TVA object
  const jet::TrackVertexAssociation* tva = NULL;
  if (evtStore()->retrieve(tva,m_tvaName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: " << m_tvaName);
    return 3;
  }
  ATH_MSG_DEBUG("Successfully retrieved TrackVertexAssociation from evtStore: " << m_tvaName);
	
  // Get and set the JVF vector
  const std::vector<float> jvf = getJetVertexFraction(vertices,tracks,tva);
  jet.setAttribute(m_jvfname, jvf);
	
  // Get and set the highest JVF vertex
  jet.setAttribute("Highest" + m_jvfname + "Vtx",getMaxJetVertexFraction(vertices,jvf));
	   
  // Done
  return 0;
}

//**********************************************************************
// Operate on the full jet container with fewer retrieves and 
// add corrected JVF, which needs full-event information
//**********************************************************************
int JetVertexFractionTool::modify(xAOD::JetContainer& jetCont) const {

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
	
  // Get the TVA object
  const jet::TrackVertexAssociation* tva = NULL;
  if (evtStore()->retrieve(tva,m_tvaName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation from evtStore: " << m_tvaName);
    return 3;
  }
  ATH_MSG_DEBUG("Successfully retrieved TrackVertexAssociation from evtStore: " << m_tvaName);
	
  if (vertices->size() == 0 ) { 
    ATH_MSG_WARNING("There are no vertices in the container. Exiting"); 
    return 4;
  }

  const xAOD::Vertex* HSvertex = findHSVertex(vertices);

  // Count pileup tracks - currently done for each collection
  const int n_putracks = getPileupTrackCount(HSvertex, tracksCont, tva);

  for(xAOD::Jet * jet : jetCont) {
    // Get the tracks associated to the jet
    // Note that there may be no tracks - this is both normal and an error case
    std::vector<const xAOD::TrackParticle*> tracks;
    if ( ! jet->getAssociatedObjects(m_assocTracksName, tracks) ) {
      ATH_MSG_DEBUG("Associated tracks not found.");
    }
      
    // Get the track pT sums for all tracks in the jet (first key) and those associated to PU (second key) vertices.
    const std::pair<float,float> tracksums = getJetVertexTrackSums(HSvertex, tracks, tva);
    // Get the track pT sums for each individual vertex
    std::vector<float> vsumpttrk = jet->getAttribute<std::vector<float> >(m_sumPtTrkName);
    float sumpttrk_all = tracksums.first;
    float sumpttrk_nonPV = tracksums.second;
    float sumpttrk_PV = vsumpttrk[HSvertex->index()];

    // Get and set the JVF vector
    std::vector<float> jvf(vertices->size());
    for(size_t vtxi=0; vtxi<vertices->size(); ++vtxi) {
      jvf[vtxi] = sumpttrk_all > 1e-9 ? vsumpttrk[vtxi] / sumpttrk_all : -1;
    }
    jet->setAttribute(m_jvfname, jvf);
  
    // Get and set the highest JVF vertex
    if(!m_isTrigger) {
      jet->setAttribute("Highest" + m_jvfname + "Vtx",getMaxJetVertexFraction(vertices,jvf));
    }
    // Calculate RpT and JVFCorr 
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

//**********************************************************************

ElementLink<xAOD::VertexContainer> JetVertexFractionTool::
getMaxJetVertexFraction(const xAOD::VertexContainer* vertices,
                        const std::vector<float>& jvf) const {
  size_t maxIndex = 0;
  float maxVal = -100;
  for ( size_t iVertex = 0; iVertex < jvf.size(); ++iVertex ) {
    if ( jvf.at(iVertex) > maxVal ) {
      maxIndex = iVertex;
      maxVal = jvf.at(iVertex);
    }
  }
  ElementLink<xAOD::VertexContainer> link =
    ElementLink<xAOD::VertexContainer>(*vertices,vertices->at(maxIndex)->index());
  return link;
}

//**********************************************************************

std::pair<float,float> JetVertexFractionTool::getJetVertexTrackSums(const xAOD::Vertex* vertex, 
								  const std::vector<const xAOD::TrackParticle*>& tracks, 
								  const jet::TrackVertexAssociation* tva) const {
    float sumTrackAll = 0;
    float sumTracknotPV = 0;
    bool notsel = m_htsel.empty();
    unsigned int nkeep = 0;
    unsigned int nskip = 0;    
    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack) {
      const xAOD::TrackParticle* track = tracks.at(iTrack);
      if ( notsel || m_htsel->keep(*track) ) {
	sumTrackAll += track->pt();

	const xAOD::Vertex* ptvtx = tva->associatedVertex(track);
	if( ptvtx != nullptr ) {
	  // Track has vertex, assign to appropriate pT sum
	  if ( ptvtx->index() != vertex->index() ) {sumTracknotPV += track->pt(); }
	}
	++nkeep;
      }
      else { ++nskip; }
    }
    ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
                  << ": nsel=" << nkeep
	 	  << ", nrej=" << nskip           );

    return std::make_pair(sumTrackAll,sumTracknotPV);

}


//**********************************************************************

int JetVertexFractionTool::getPileupTrackCount(const xAOD::Vertex* vertex, 
					     const xAOD::TrackParticleContainer*& tracksCont, 
					     const jet::TrackVertexAssociation* tva) const
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
	  const xAOD::Vertex* ptvtx = tva->associatedVertex(track);
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

    ATH_MSG_VERBOSE("JetVertexFractionTool " << name()
                  << ": nsel=" << nkeep
                  << ", nrej=" << nskip
		 << ", total " << tracksCont->size() );
    ATH_MSG_VERBOSE("JetVertexFractionTool " << name()
		 << ": n_PUtracks=" << n_pileuptracks
		 << ", total=" << tot_count          );

    return n_pileuptracks;
}

//**********************************************************************

const xAOD::Vertex* JetVertexFractionTool::findHSVertex(const xAOD::VertexContainer*& vertices) const
{
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
    if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {
      
      ATH_MSG_VERBOSE("JetVertexFractionTool " << name() << " Found HS vertex at index: "<< iVertex);
      return vertices->at(iVertex);
    }
  }
  ATH_MSG_VERBOSE("There is no vertex of type PriVx. Taking default vertex.");
  return vertices->at(0);
}

//**********************************************************************
// Legacy methods allowing direct computation of JVF
//**********************************************************************


const std::vector<float> JetVertexFractionTool::
getJetVertexFraction(const xAOD::VertexContainer* vertices,
                     const std::vector<const xAOD::TrackParticle*>& tracks,
                     const jet::TrackVertexAssociation* tva) const {
  std::vector<float> jvf = getEmptyJetVertexFraction(vertices);
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex )
    jvf.at(iVertex) = getJetVertexFraction(vertices->at(iVertex),tracks,tva);
  return jvf;
}

//**********************************************************************

std::vector<float>
JetVertexFractionTool::getEmptyJetVertexFraction(const xAOD::VertexContainer* vertices) const {
  std::vector<float> jvf;
  jvf.resize(vertices->size());
  for (size_t iVertex = 0; iVertex < vertices->size(); ++iVertex) jvf.at(iVertex) = 0;
  return jvf;
}

//**********************************************************************

float JetVertexFractionTool::
getJetVertexFraction(const xAOD::Vertex* vertex,
                     const std::vector<const xAOD::TrackParticle*>& tracks,
                     const jet::TrackVertexAssociation* tva) const {
  float sumTrackPV = 0;
  float sumTrackAll = 0;
  bool notsel = m_htsel.empty();
  unsigned int nkeep = 0;
  unsigned int nskip = 0;
  for ( size_t itrk = 0; itrk < tracks.size(); ++itrk ) {
    const xAOD::TrackParticle* ptrk = tracks.at(itrk);
    if ( notsel || m_htsel->keep(*ptrk) ) {
      const xAOD::Vertex* ptvtx = tva->associatedVertex(ptrk);
      if ( ptvtx != nullptr ) {
        if ( ptvtx->index() == vertex->index() ) sumTrackPV += ptrk->pt();
      }
      sumTrackAll += ptrk->pt();
      ++nkeep;
    } else {
      ++nskip;
    }
  }
  double jvf = sumTrackAll>0 ? sumTrackPV/sumTrackAll : -1;
  ATH_MSG_VERBOSE("JetVertexFractionTool " << name()
                  << ": nsel=" << nkeep
                  << ", nrej=" << nskip
                  << ", " << m_jvfname << "=" << jvf);
  return jvf;
}
