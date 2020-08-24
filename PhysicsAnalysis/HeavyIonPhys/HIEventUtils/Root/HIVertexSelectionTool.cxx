/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "HIEventUtils/HIVertexSelectionTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AsgTools/Check.h"

using std::string;

HI::HIVertexSelectionTool::HIVertexSelectionTool( const string& name )
  : asg::AsgTool( name )
  , m_accept( "HIVertexSelection" )
#ifndef XAOD_STANDALONE
    // we don't want to give the tool a name in rootcore
  , m_trkSelTool( "InDet::InDetTrackSelectionTool", this )
#endif // XAOD_STANDALONE
{
  declareProperty( "RequirePrimary", m_requirePrimary, "Require the vertex to have type PriVtx");
  declareProperty( "MaxAbsZ", m_maxAbsZ, "Maximum absolute value of the vertex z position");
  declareProperty( "TrackSelectionTool", m_trkSelTool, "Track selection tool" );
  declareProperty( "MinNTrk", m_minNtrk, "Minimum number of associated tracks passing selection" );
  declareProperty( "MinRmsPt", m_minRmsPt, "Minimum RMS pt [MeV] of associated tracks passing selection" );
}

HI::HIVertexSelectionTool::~HIVertexSelectionTool() = default;

StatusCode HI::HIVertexSelectionTool::initialize()
{
  ATH_MSG_INFO( "Initializing vertex selection tool." );
  ATH_CHECK( asg::AsgTool::initialize() );


  if (m_requirePrimary) {
    ATH_MSG_INFO( "Requiring vertex to be a primary vertex" );
    m_accept.addCut( "type", "Whether the vertex satisfies the requirement to be a primary vertex" );
  }
  else {
    ATH_MSG_INFO( "NOT requiring vertex to be a primary vertex" );
  }

  if (m_maxAbsZ > 0.) {
    ATH_MSG_INFO( "Maximum |z| = " << m_maxAbsZ << " mm" );
    m_accept.addCut( "z", "Whether the vertex's |z| is in an allowed range" );
  }

  if (m_minNtrk >= 0) {
    m_checkTracks = true;
    ATH_MSG_INFO( "Minimum Ntrk = " << m_minNtrk );
    m_accept.addCut( "ntrk", "Whether the vertex has the minimum number of tracks" );
  }
  if (m_minRmsPt >= 0.) {
    m_checkTracks = true;
    ATH_MSG_INFO( "Minimum RMS track pt = " << m_minRmsPt << " MeV" );
    m_accept.addCut( "pt", "Whether the vertex's tracks RMS pt is sufficient" );
  }


  return StatusCode::SUCCESS;
}

StatusCode HI::HIVertexSelectionTool::finalize()
{
  ATH_MSG_INFO( "Finalizing vertex selection tool." );
  return StatusCode::SUCCESS;
}

//R.Longo 13-10-2019 - Replacing PATCore/TAccept (inherited from 21.0 HI-equalization)
const asg::AcceptInfo& HI::HIVertexSelectionTool::getAcceptInfo() const
{
  // return the current TAccept object by reference. This allows users to avoid copying it.
  return m_accept;
}

asg::AcceptData HI::HIVertexSelectionTool::accept( const xAOD::IParticle* ) const
{
  asg::AcceptData acceptData (&m_accept);
  ATH_MSG_ERROR( "Vertex selection tool should not be passed an IParticle." );
  throw std::invalid_argument( "Vertex selection tool given an IParticle." );
  return acceptData;
}

asg::AcceptData HI::HIVertexSelectionTool::accept( const xAOD::Vertex& vtx ) const
{
  asg::AcceptData acceptData (&m_accept);

  if (!m_requirePrimary || vtx.vertexType() == xAOD::VxType::PriVtx) {
    acceptData.setCutResult( "type", true );
  }
  if (m_maxAbsZ < 0. || std::fabs( vtx.z() ) < m_maxAbsZ) {
    acceptData.setCutResult( "z", true );
  }
  if (m_checkTracks) {
    bool countTracks = m_minNtrk >= 0;
    bool countPt = m_minRmsPt >= 0.;
    int nPassed = 0; // cumulative total number of tracks passed
    double sumPtSq = 0.; // cumulative total of pt^2 of tracks passed
    for ( const auto& track : vtx.trackParticleLinks() ) {
      if ( !track.isValid() ) continue;
      if ( !m_trkSelTool->accept( **track, &vtx ) ) continue;

      if (countTracks) {
	nPassed++;
	if (nPassed >= m_minNtrk) {
	  acceptData.setCutResult( "ntrk", true );
	  countTracks = false; // stop bothering to count
	}
      }
      if (countPt) {
	auto pt = (*track)->pt();
	sumPtSq += pt*pt;
	if (sumPtSq >= m_minRmsPt*m_minRmsPt) {
	  acceptData.setCutResult( "pt", true);
	  countPt = false; // stop evaluating pt
	}
      }

      if (!countTracks && !countPt) break; // don't run through tracks we don't need to check
    } // end loop over tracks
  }

  return acceptData;
}
