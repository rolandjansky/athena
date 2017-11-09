///////////////////////////////////////////////////////////////////
// BJetRegressionVariables.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Alexander Melzer (alexander.melzer@cern.ch)
//
 
#include "DerivationFrameworkExotics/BJetRegressionVariables.h"
#include "TLorentzVector.h"

#include <AsgTools/MessageCheck.h>

 
namespace DerivationFramework {
 
  BJetRegressionVariables::BJetRegressionVariables(const std::string& t,
						   const std::string& n,
						   const IInterface* p) :
    AthAlgTool(t,n,p),
    m_containerName(""),
    m_assocTracksName(""),
    m_minTrackPt(),
    m_vertexContainer("PrimaryVertices"),
    m_requireTrackPV(true),
    m_tva( "JetTrackVtxAssoc" )
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ContainerName", m_containerName);
    declareProperty("AssociatedTracks", m_assocTracksName);
    declareProperty("MinTrackPtCuts", m_minTrackPt);
    declareProperty("VertexContainer", m_vertexContainer);
    declareProperty("RequireTrackPV", m_requireTrackPV);
    declareProperty("TrackVertexAssociation",m_tva);
  }

  BJetRegressionVariables::~BJetRegressionVariables(){
 }
 
  StatusCode BJetRegressionVariables::initialize()
  {
    ANA_CHECK_SET_TYPE (StatusCode);

    ATH_MSG_INFO("initializing BJetRegressionVariables...");
    if (m_containerName=="") {
      ATH_MSG_ERROR("No Jet collection provided for BJetRegressionVariables!");
      return StatusCode::FAILURE;
    }
    if (m_assocTracksName=="") {
      ATH_MSG_ERROR("No TrackParticle type provided for BJetRegressionVariables!");
      return StatusCode::FAILURE;
    }
    if(m_minTrackPt.size() == 0){
      ATH_MSG_ERROR("No track pt cut provided for BJetRegressionVariables!");
      return StatusCode::FAILURE;
    }
    m_TrackSelTool = new InDet::InDetTrackSelectionTool("TrackSelector");
    ANA_CHECK(m_TrackSelTool->setProperty("CutLevel", "Loose"));
    ANA_CHECK(m_TrackSelTool->initialize());

    return StatusCode::SUCCESS;
  }
 
  StatusCode BJetRegressionVariables::finalize()
  {
    if(m_TrackSelTool){
      delete m_TrackSelTool;
      m_TrackSelTool = 0;
    }
    return StatusCode::SUCCESS;
  }
 
  StatusCode BJetRegressionVariables::addBranches() const
  {
    const xAOD::JetContainer* jets = evtStore()->retrieve< const xAOD::JetContainer >( m_containerName );
    if( ! jets ) {
      ATH_MSG_ERROR ("Couldn't retrieve Jets with key: " << m_containerName );
      return StatusCode::FAILURE;
    }

    const xAOD::VertexContainer* vertices = evtStore()->retrieve<const xAOD::VertexContainer>( m_vertexContainer );
    if( ! vertices ) {
      ATH_MSG_ERROR ("Couldn't retrieve vertex container: " << m_vertexContainer);
      return StatusCode::FAILURE;
    }

    const auto it_pv = std::find_if(vertices->cbegin(), vertices->cend(),[](const xAOD::Vertex* vtx)
				    {return vtx->vertexType() == xAOD::VxType::PriVtx;});
    const xAOD::Vertex* HSvertex = (it_pv == vertices->cend()) ? nullptr : *it_pv;

    const jet::TrackVertexAssociation* tva = evtStore()->retrieve<const jet::TrackVertexAssociation>( m_tva );
    if( ! tva ) {
      ATH_MSG_ERROR ("Couldn't retrieve TrackVertexAssociation: " << m_tva);
      return StatusCode::FAILURE;
    }
    

    for(xAOD::JetContainer::const_iterator jetItr = jets->begin(); jetItr!=jets->end(); ++jetItr){
      std::vector<const xAOD::TrackParticle*> tracks;
      bool hastracks = (*jetItr)->getAssociatedObjects(m_assocTracksName, tracks);
      if ( ! hastracks ) ATH_MSG_WARNING("Associated tracks not found");
      ATH_MSG_DEBUG("Successfully retrieved track particles.");

      for (size_t iCut = 0; iCut < m_minTrackPt.size(); ++iCut) {
	const float minPt = m_minTrackPt[iCut];
	BJetRegressionVariables::TrackMomentStruct moments = getSumTrackPt(minPt, tracks, HSvertex, tva);
	const std::string baseName = getMomentBaseName(minPt, false);
	const std::string baseNameClean = getMomentBaseName(minPt, m_requireTrackPV);
	SG::AuxElement::Decorator< float > scalSumPtTrk("ScalSumPtTrk"+baseName);
	SG::AuxElement::Decorator< float > vecSumPtTrk("VecSumPtTrk"+baseName);
	SG::AuxElement::Decorator< float > scalSumPtTrkClean("ScalSumPtTrkClean"+baseNameClean);
	SG::AuxElement::Decorator< float > vecSumPtTrkClean("VecSumPtTrkClean"+baseNameClean);
	scalSumPtTrk( **jetItr )        = moments.vecSumPtTrk;
	vecSumPtTrk( **jetItr )         = moments.scalSumPtTrk;
	scalSumPtTrkClean( **jetItr )   = moments.vecSumPtTrkClean;
        vecSumPtTrkClean( **jetItr )    = moments.scalSumPtTrkClean;
      }
      
    }
    return StatusCode::SUCCESS;
  }//addBranches
  
  BJetRegressionVariables::TrackMomentStruct BJetRegressionVariables::getSumTrackPt(const float minTrackPt, 
										    const std::vector<const xAOD::TrackParticle*>& tracks, 
										    const xAOD::Vertex* vertex, 
										    const jet::TrackVertexAssociation* tva) const
  {
    BJetRegressionVariables::TrackMomentStruct moments;
    TLorentzVector trackP4Sum;
    double trackPtSum = 0;
    TLorentzVector trackP4SumClean;
    double trackPtSumClean = 0;
    for (size_t iTrack = 0; iTrack < tracks.size(); ++iTrack)
      {
	const xAOD::TrackParticle* track = tracks[iTrack];
	
	const double trackPt = track->pt();
	const TLorentzVector trackP4 = track->p4();
	
	if (trackPt < minTrackPt) {continue;}
	trackPtSum         += trackPt;
	trackP4Sum         += trackP4;
	
	if(m_TrackSelTool->accept(*track)){
	  const xAOD::Vertex* ptvtx = tva->associatedVertex(track);
	  if (!m_requireTrackPV || (ptvtx != nullptr && vertex != nullptr) ){
	    if (!m_requireTrackPV || (ptvtx->index() == vertex->index()) ) {
	      trackPtSumClean         += trackPt;
	      trackP4SumClean         += trackP4;
	    }
	  }
	}
      }
    moments.scalSumPtTrk = trackPtSum;
    moments.vecSumPtTrk  = trackP4Sum.Pt();
    moments.scalSumPtTrkClean = trackPtSumClean;
    moments.vecSumPtTrkClean  = trackP4SumClean.Pt();
    
    return moments;
    
  }//getSumTrackPt

  const std::string BJetRegressionVariables::getMomentBaseName( const float minTrackPt, const bool pv0 ) const
  {
    int value = round(minTrackPt);
    if (fabs(value - minTrackPt) > 0.1){
      ATH_MSG_WARNING("Cut float and int disagree: " << minTrackPt << " float vs " << value << " int");
    }
    
    std::ostringstream sout;
    sout << "Pt" << value;
    if(pv0){
      sout << "PV0";
    }
    return sout.str();
  }
 
}//namespace
