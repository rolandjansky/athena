///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexTaggerTool.cxx
// Implementation file for class JetVertexTaggerTool
// Author: James Frost <james.frost@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetMomentTools/JetVertexTaggerTool.h"
#include "PathResolver/PathResolver.h"

using std::string;
using xAOD::JetFourMom_t;

//**********************************************************************

JetVertexTaggerTool::JetVertexTaggerTool(const std::string& name)
: asg::AsgTool(name)
, m_verticesName("")
, m_assocTracksName("")
, m_tvaName("")
, m_tracksName("")
, m_jvtlikelihoodHistName("")
, m_jvtfileName("")
, m_htsel("")
{
    declareProperty("VertexContainer",m_verticesName);
    declareProperty("AssociatedTracks",m_assocTracksName);
    declareProperty("TrackVertexAssociation",m_tvaName);
    declareProperty("TrackParticleContainer",m_tracksName);

    declareProperty("JVTFileName",m_jvtfileName = "JVTlikelihood_20140805.root");
    declareProperty("JVTLikelihoodHistName",m_jvtlikelihoodHistName = "JVTRootCore_kNN100trim_pt20to50_Likelihood");
    declareProperty("TrackSelector", m_htsel);
    declareProperty("JVTName", m_jvtName ="Jvt");
    declareProperty("K_JVFCorrScale",m_kcorrJVF = 0.01);
    declareProperty("Z0Cut",m_z0cut = 3.);
    declareProperty("PUTrkPtCut",m_PUtrkptcut = 30000.);
}

//**********************************************************************

StatusCode JetVertexTaggerTool::initialize() {
  ATH_MSG_INFO("Initializing JetVertexTaggerTool " << name());

  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }

  // Use the Path Resolver to find the jvt file and retrieve the likelihood histogram
  m_fn =  PathResolverFindCalibFile(m_jvtfileName);	
  ATH_MSG_INFO("  Reading JVT file from:\n    " << m_jvtfileName << "\n");
  ATH_MSG_INFO("                     resolved in  :\n    " << m_fn << "\n\n");

  m_jvtfile = TFile::Open(m_fn);
  if ( !m_jvtfile ) { ATH_MSG_FATAL( "Cannot open JVTLikelihoodFile: " << m_fn ); return StatusCode::FAILURE; }

 ATH_MSG_VERBOSE("\n Reading JVT likelihood histogram from:\n    " << m_fn << "\n\n");

 m_jvthisto = (TH2F*)m_jvtfile->Get(m_jvtlikelihoodHistName.c_str() );
 if ( !m_jvthisto ) 
   {
     ATH_MSG_FATAL( "\n  Found JVT file, but JVT histogram missing. Aborting..." );
     return StatusCode::FAILURE;
   }

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetVertexTaggerTool::modify(xAOD::JetContainer& jetCont) const {

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

  for(xAOD::Jet * jet : jetCont) 
    {

      // Get the tracks associated to the jet
      // Note that there may be no tracks - this is both normal and an error case
      std::vector<const xAOD::TrackParticle*> tracks;
      if ( ! jet->getAssociatedObjects(m_assocTracksName, tracks) ) {
	ATH_MSG_WARNING("Associated tracks not found.");
      }

      // Retrieve the Vertex associated to the jet.

      /* // For retrieving the origin from the jet
      if( ! jet->getAssociatedObject<xAOD::Vertex>("OriginVertex", jetorigin) )
      	{ ATH_MSG_VERBOSE("Jet has no vertex associated. Using leading vertex" ); 
     	  jetorigin = vertices->at(0); 
      	} 
      else
      	{
	  ATH_MSG_VERBOSE("Jet has associated vertex. Using this vertex, number: " << jetorigin->index() ); 
      	}
      */
      
      // Get the track pTs sums associated to the primary (first key of pair) and PU (second key) vertices.
      const std::pair<float,float> tracksums = getJetVertexTrackSums(HSvertex, tracks, tva);
      
      // Calculate RpT and JVFCorr 
      // Default JVFcorr to -1 when no tracks are associated.
      float jvfcorr = -999.;
      if(tracksums.first + tracksums.second > 0) {
	jvfcorr = tracksums.first / (tracksums.first + ( tracksums.second / (m_kcorrJVF * std::max(n_putracks, 1) ) ) );
      } else {
        jvfcorr = -1;
      }
      
      const float rpt = tracksums.first/jet->pt();
      float jvt = evaluateJvt(rpt, jvfcorr);
  
      jet->setAttribute(m_jvtName+"Jvfcorr",jvfcorr);
      jet->setAttribute(m_jvtName+"Rpt",rpt);
      jet->setAttribute(m_jvtName,jvt);

      ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
		   << ": Primary trk pT=" << tracksums.first
		   << ", Pileup trk pT=" << tracksums.second 
		   << ", Old JVF=" <<  tracksums.first/(tracksums.first+tracksums.second)   );

      ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
		   << ": JVT=" << jvt
		   << ", RpT=" << rpt
		   << ", JVFCorr=" << jvfcorr       );

      // Done
      
    } 

  return 0;
}

//**********************************************************************

float JetVertexTaggerTool::evaluateJvt(float rpt, float jvfcorr) const {
  // Look up JVT value
  float jvt = -999.;
  if ( jvfcorr == -1.0 ) {
    jvt = -0.1;
  } else {
    float rpt_inputtojvt = std::min(rpt, (float) 1. );
    int bin = m_jvthisto->FindBin(jvfcorr, rpt_inputtojvt);
    jvt = m_jvthisto->GetBinContent(bin);
    jvt = m_jvthisto->Interpolate(jvfcorr, rpt_inputtojvt);
  }
  return jvt;
}  
      
//**********************************************************************

float JetVertexTaggerTool::updateJvt(const xAOD::Jet& jet, std::string sjvt, std::string scale) const {
  string sjvfcorr = sjvt + "Jvfcorr";
  string srpt = sjvt + "Rpt";
  JetFourMom_t p4old = jet.jetP4(scale);
  float ptold = p4old.pt();
  float ptnew = jet.pt();
  float jvfcorr = jet.getAttribute<float>(sjvfcorr);
  float rptold = jet.getAttribute<float>(srpt);
  //float jvtold = jet.getAttribute<float>(sjvt);
  float rptnew = rptold*ptold/ptnew;
  return evaluateJvt(rptnew, jvfcorr);
}

//**********************************************************************

StatusCode JetVertexTaggerTool::finalize() {
  ATH_MSG_INFO("Finalizing JetVertexTaggerTool " << name());
  m_jvtfile->Close();

  return StatusCode::SUCCESS;
}

//**********************************************************************

std::pair<float,float> JetVertexTaggerTool::getJetVertexTrackSums(const xAOD::Vertex* vertex, 
								  const std::vector<const xAOD::TrackParticle*>& tracks, 
								  const jet::TrackVertexAssociation* tva) const {
    float sumTrackPV = 0;
    float sumTracknotPV = 0;
    bool notsel = m_htsel.empty();
    unsigned int nkeep = 0;
    unsigned int nskip = 0;    
    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack) {
      const xAOD::TrackParticle* track = tracks.at(iTrack);
      if ( notsel || m_htsel->keep(*track) ) {
	const xAOD::Vertex* ptvtx = tva->associatedVertex(track);

	// Check track provenance
	//	if ( ptvtx == nullptr ) {

	  // No track associated, check if z0 within cut.
	  // Now done upstream in TVA, comment inactive code.
	  //	  if( (fabs(track->z0()+track->vz()-vertex->z()) < m_z0cut)  ) {sumTrackPV += track->pt(); }  // if pass z0 cuts, assign track without vertex to PV track sum
	  //	}
	  //	else {

	// Previously tracks associated to no vertex were checked against z0 cuts, this is now done in the TVA tool - only consider tracks associated to a vertex.
	if( ptvtx != nullptr ) {
	  // Track has vertex, assign to appropriate pT sum
	  if ( ptvtx->index() == vertex->index() ) { sumTrackPV += track->pt(); }
	  else {sumTracknotPV += track->pt(); }	
	}
	++nkeep;
      }
      else { ++nskip; }
    }
    ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
                  << ": nsel=" << nkeep
	 	  << ", nrej=" << nskip           );

    return std::make_pair(sumTrackPV,sumTracknotPV);

}

//**********************************************************************

int JetVertexTaggerTool::getPileupTrackCount(const xAOD::Vertex* vertex, 
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

    ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
                  << ": nsel=" << nkeep
                  << ", nrej=" << nskip
		 << ", total " << tracksCont->size() );
    ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
		 << ": n_PUtracks=" << n_pileuptracks
		 << ", total=" << tot_count          );

    return n_pileuptracks;
}

//**********************************************************************

const xAOD::Vertex* JetVertexTaggerTool::findHSVertex(const xAOD::VertexContainer*& vertices) const
{
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
    if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {
      
      ATH_MSG_VERBOSE("JetVertexTaggerTool " << name() << " Found HS vertex at index: "<< iVertex);
      return vertices->at(iVertex);
    }
  }
  ATH_MSG_WARNING("There is no vertex of type PriVx. Taking default vertex.");
  return vertices->at(0);
}

//**********************************************************************
