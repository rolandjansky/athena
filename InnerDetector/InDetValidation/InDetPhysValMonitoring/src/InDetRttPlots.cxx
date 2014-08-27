/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRttPlots.cxx
 * @author shaun roe
**/

#include "InDetRttPlots.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h" 


InDetRttPlots::InDetRttPlots(PlotBase* pParent, const std::string & sDir):PlotBase(pParent, sDir),
    m_ptPlot(this,"Tracks/SelectedGoodTracks"),
    m_PtEtaPlots(this,"Tracks/SelectedGoodTracks","TrackParticle"),
    m_IPPlots(this,"Tracks/SelectedGoodTracks"),
    m_TrackRecoInfoPlots(this,"Tracks/SelectedGoodTracks"),
    m_TrackTruthInfoPlots(this,"Truth"),
    m_nTracks(this,"Tracks/SelectedGoodTracks"), 
    m_resPlots(this,"Tracks/SelectedGoodTracks"),
    m_pullPlots(this, "Tracks/SelectedGoodTracks"),
    m_fakePlots(this,"Tracks/SelectedFakeTracks"),
    m_hitsPlots(this,"Tracks/SelectedGoodTracks"),
    m_hitsMatchedTracksPlots(this,"Tracks/SelectedMatchedTracks"),
    m_hitsFakeTracksPlots(this,"Tracks/SelectedFakeTracks"),
    m_hitsDetailedPlots(this,"Tracks/SelectedGoodTracks"),                   
    m_verticesPlots(this, "Vertices/AllPrimaryVertices"),
    m_vertexPlots(this,"Vertices/AllPrimaryVertices"),
    m_hardScatterVertexPlots(this,"Vertices/HardScatteringVertex")
{
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbThreshold = 0.8;
  m_truthProbLowThreshold = 0.5;
  //nop
}


void 
InDetRttPlots::fill(const xAOD::TrackParticle& particle,const xAOD::TruthParticle& truthParticle){
  //fill resolution plots
	m_resPlots.fill(particle, truthParticle); 
	//fill pull plots
	m_pullPlots.fill(particle, truthParticle);
	if (particle.isAvailable<float>(m_trackParticleTruthProbKey)) {
		const float prob = particle.auxdata<float>(m_trackParticleTruthProbKey);
		float barcode = truthParticle.barcode();
		if (prob < m_truthProbLowThreshold) {
			m_hitsFakeTracksPlots.fill(particle);
		} else if (barcode < 100000 && barcode != 0) {
			m_hitsMatchedTracksPlots.fill(particle);
		}
	}
}

void 
InDetRttPlots::fill(const xAOD::TrackParticle& particle){
    //fill pt plots
    m_ptPlot.fill(particle);
    m_PtEtaPlots.fill(particle);
    m_IPPlots.fill(particle);
    m_TrackRecoInfoPlots.fill(particle);
    m_hitsPlots.fill(particle);
    m_hitsDetailedPlots.fill(particle);    
}

void 
InDetRttPlots::fill(const xAOD::TruthParticle& truthParticle){
  //fill truth plots
  m_TrackTruthInfoPlots.fill(truthParticle);
}

void 
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer){
  //fill vertex container general properties
  //m_verticesPlots.fill(vertexContainer); //if ever needed
  //fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()){
    if (vtx->vertexType() == xAOD::VxType::NoVtx) continue; //skip dummy vertex
    m_vertexPlots.fill(*vtx);
    if (vtx->vertexType() == xAOD::VxType::PriVtx) m_hardScatterVertexPlots.fill(*vtx);
  }
}

void 
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei){  
  m_verticesPlots.fill(vertexContainer, ei);
}

void 
InDetRttPlots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory  counter){
    m_nTracks.fill(freq,counter);
}

void 
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const InDetPerfPlot_fakes::Category c){
	m_fakePlots.fill(particle,match, c);
}
