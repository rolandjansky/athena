// this is c++ file -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTStrawEfficiency.h
// author: Ryan D. Reece <ryan.reece@cern.ch>
// created: Nov 2009

#ifndef TRT_TrackHoleSearch_TRTStrawEfficiency_h
#define TRT_TrackHoleSearch_TRTStrawEfficiency_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"

#include "TrkTrack/TrackCollection.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

#include "StoreGate/ReadHandleKey.h"

#include <string>
#include <vector>

namespace Trk
{
	class ITrackHoleSearchTool;
	class TrackStateOnSurface;
}

class ITHistSvc;
class TRT_ID;
class ITRT_StrawNeighbourSvc;

class TTree;

class TRTStrawEfficiency : public AthAlgorithm
{
 public:
	TRTStrawEfficiency(const std::string& name, ISvcLocator* pSvcLocator);

	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

 private:
	// configurables
	//----------------------------------
	ToolHandle<Trk::ITrackHoleSearchTool>  m_trt_hole_finder;
	float m_max_abs_d0;
	float m_max_abs_z0;
	float m_min_pT;
	float m_min_p;		/* added by dan */
	float m_max_abs_eta;
	int m_min_pixel_hits;
	int m_min_sct_hits;
	int m_min_trt_hits;
	ServiceHandle<ITHistSvc> m_hist_svc;
	ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;
	std::string m_tree_name;
	std::string m_stream_name;
	std::string m_required_trigger;

	// private data
	//----------------------------------
	TTree* m_tree;
	const TRT_ID* m_TRT_ID;
	ToolHandle<Trk::IUpdator> m_updator;
	ToolHandle<Trig::ITrigDecisionTool> m_trigDec;

	// Data handles
	SG::ReadHandleKey<TrackCollection> m_tracksKey{this, "track_collection", "CombinedInDetTracks", "Tracks container key"};
	SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "event_info_key", "EventInfo", "Event info key"};
	SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerKey{this, "VertexContainerName", "PrimaryVertices", "Vertex container key"};

	unsigned int m_num_events;
	unsigned int m_num_tracks;
	unsigned int m_num_preselected_tracks;

	// ntuple branches
	unsigned int m_event_number;
	unsigned int m_run_number;
	unsigned int m_lumi_block;
	float m_track_pt;
	float m_track_eta;
	float m_track_phi;
	float m_track_d0;
	float m_track_z0;
	int m_n_pixel_hits;
	int m_n_sct_hits;
	int m_n_trt_hits;
	std::vector<int> m_hit_bec;
	std::vector<int> m_hit_phi;
	std::vector<int> m_hit_layer;
	std::vector<int> m_hit_strawlayer;
	std::vector<int> m_hit_straw;
	std::vector<int> m_hit_chip;
	std::vector<int> m_hit_pad;
	std::vector<float> m_hit_x;
	std::vector<float> m_hit_y;
	std::vector<float> m_hit_z;
	std::vector<float> m_hit_center_x;
	std::vector<float> m_hit_center_y;
	std::vector<float> m_hit_center_z;
	std::vector<float> m_hit_R;
	std::vector<float> m_hit_locR;
	std::vector<int> m_hit_HL;
	std::vector<int> m_hit_det;
	std::vector<float> m_hit_ub_locR;
	std::vector<float> m_hit_ub_x;
	std::vector<float> m_hit_ub_y;
	std::vector<float> m_hit_ub_z;
	int m_n_pixel_holes;
	int m_n_sct_holes;
	int m_n_trt_holes;
	std::vector<int> m_hole_bec;
	std::vector<int> m_hole_phi;
	std::vector<int> m_hole_layer;
	std::vector<int> m_hole_strawlayer;
	std::vector<int> m_hole_straw;
	std::vector<int> m_hole_chip;
	std::vector<int> m_hole_pad;
	std::vector<float> m_hole_x;
	std::vector<float> m_hole_y;
	std::vector<float> m_hole_z;
	std::vector<float> m_hole_center_x;
	std::vector<float> m_hole_center_y;
	std::vector<float> m_hole_center_z;
	std::vector<float> m_hole_locR;
	std::vector<float> m_hole_locR_error;
	std::vector<int> m_hole_det;

	//---- branches added by dan -------
	std::vector<int> m_hit_tube_hit;
	int m_n_primary_vertex;
	int m_n_tube_hits;

	// private methods
	//----------------------------------
	void make_branches();
	void clear_branches();
	int fill_hit_data(const Trk::TrackStateOnSurface& hit);
	int fill_hole_data(const Trk::TrackStateOnSurface& hole);
};

#endif // TRT_TrackHoleSearch_TRTStrawEfficiency_h
