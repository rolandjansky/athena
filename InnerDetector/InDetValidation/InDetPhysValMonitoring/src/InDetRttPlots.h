/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETRTTPLOTS
#define INDETPHYSVALMONITORING_INDETRTTPLOTS
/**
 * @file InDetRttPlots.h
 * @author shaun roe
 * @date 29/3/2014
**/


//std includes
#include <string>

//local includes
#include "TrkValHistUtils/PlotBase.h"
#include "InDetPerfPlot_Pt.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "InDetPerfPlot_res.h"
#include "InDetPerfPlot_pull.h"
#include "InDetPerfPlot_fakes.h"

#include "TrkValHistUtils/IDHitPlots.h"
#include "InDetPerfPlot_HitDetailed.h"
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_VertexContainer.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttPlots:public PlotBase {
public:
	InDetRttPlots(PlotBase * pParent, const std::string & dirName);
	///fill for things needing truth and track only
	void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
	///fill for things needing track only
	void fill(const xAOD::TrackParticle& particle);
	///fill for things needing truth only
	void fill(const xAOD::TruthParticle& particle);
	///fill reco-vertex related plots
	void fill(const xAOD::VertexContainer& vertexContainer);
	///fill reco-vertex related plots that need EventInfo 
	void fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei);

	virtual ~InDetRttPlots(){/**nop**/};
	///fill for Counters
	void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory  counter);
	///fill for fakes
	void fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const InDetPerfPlot_fakes::Category c = InDetPerfPlot_fakes::ALL);

	
private:
	InDetPerfPlot_Pt  m_ptPlot;
	Trk::ParamPlots m_PtEtaPlots;
	Trk::ImpactPlots m_IPPlots;
	Trk::RecoInfoPlots m_TrackRecoInfoPlots;
	Trk::TruthInfoPlots m_TrackTruthInfoPlots;
	InDetPerfPlot_nTracks m_nTracks;
	InDetPerfPlot_res m_resPlots;
	InDetPerfPlot_pull m_pullPlots;
	InDetPerfPlot_fakes m_fakePlots; //fakes vs eta etc, as per original RTT code
	Trk::IDHitPlots m_hitsPlots;
	Trk::IDHitPlots m_hitsMatchedTracksPlots;
	Trk::IDHitPlots m_hitsFakeTracksPlots;
	InDetPerfPlot_HitDetailed m_hitsDetailedPlots;

	InDetPerfPlot_VertexContainer m_verticesPlots;
	InDetPerfPlot_Vertex m_vertexPlots;
	InDetPerfPlot_Vertex m_hardScatterVertexPlots;	

	std::string m_trackParticleTruthProbKey;
	float m_truthProbThreshold;
	float m_truthProbLowThreshold;
	
};




#endif
