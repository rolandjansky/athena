/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TRTTrackHoleSearchTool.h
// author: Ryan D. Reece <ryan.reece@cern.ch>
// created: Jan 2010


#ifndef TRT_TrackHoleSearch_TRTTrackHoleSearchTool_h
#define TRT_TrackHoleSearch_TRTTrackHoleSearchTool_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkParameters/TrackParameters.h"

#include <string>
#include <vector>
#include <fstream>

namespace Trk
{
	class IExtrapolator;
	class CylinderSurface;
	class Track;
	class Surface;
}

class TRT_ID;
class IInDetConditionsSvc;

class TRTTrackHoleSearchTool : public Trk::ITrackHoleSearchTool, public AthAlgTool
{
 public:
	TRTTrackHoleSearchTool(const std::string& type, const std::string& name, const IInterface* parent);

	StatusCode initialize();
	StatusCode finalize();

	/** Input : track, partHyp
	    Output: Changes in information
	    This method fills the fields relevant to the hole counts in the vector information. These fields should be initialised to 0. 
	    The relevant indices are specified by the enumeration in Tracking/TrkEvent/TrkTrackSummary.
	    If problems occur, the information counters are reset to -1 flagging them as not set.
	    The parthyp argument is relevant for the extrapolation steps in the hole search.
	*/
	void countHoles(const Trk::Track& track, 
	                std::vector<int>& information ,
	                const Trk::ParticleHypothesis partHyp = Trk::pion) const;
    
	/** Input : track, parthyp
	    Return: A DataVector containing pointers to TrackStateOnSurfaces which each represent an identified hole on the track.
	    The parthyp argument is relevant for the extrapolation steps in the hole search.
	    Attention: This is a factory, ownership of the return vector is passed to the calling method.
	*/
	const DataVector<const Trk::TrackStateOnSurface>* getHolesOnTrack(const Trk::Track& track, 
	                                                                  const Trk::ParticleHypothesis partHyp = Trk::pion) const;
    
	/** Input : track, parthyp
	    Return: A pointer to a new Trk::Track which containes the information of the input track plus the tsos of the identified holes
	    The parthyp argument is relevant for the extrapolation steps in the hole search.
	    Attention: This is a factory, ownership of the return track is passed to the calling method.
	*/
	const Trk::Track* getTrackWithHoles(const Trk::Track& track, 
	                                    const Trk::ParticleHypothesis partHyp = Trk::pion) const;
        
    
	/** Input : track, parthyp
	    Return: A pointer to a new Trk::Track which containes the information of the input track plus the tsos of the identified holes or outliers
	    The parthyp argument is relevant for the extrapolation steps in the hole search.
	    Attention: This is a factory, ownership of the return track is passed to the calling method.
	*/
	const Trk::Track* getTrackWithHolesAndOutliers(const Trk::Track& track, 
	                                               const Trk::ParticleHypothesis partHyp = Trk::pion) const;
    
          
 private:
	// configurables
	//----------------------------------
	ToolHandle<Trk::IExtrapolator>  m_extrapolator;
	ServiceHandle<IInDetConditionsSvc> m_conditions_svc;
	bool m_use_conditions_svc;
	float m_outer_radius;
	float m_max_z;
	int m_max_trailing_holes;
	bool m_begin_at_first_trt_hit;
	bool m_end_at_last_trt_hit;
	bool m_bcheck;
	bool m_do_dump_bad_straw_log;
	float m_locR_cut;
	float m_locR_sigma_cut;
	float m_locZ_cut;
       
	// private data
	//----------------------------------
	mutable bool m_has_been_called;
	const TRT_ID* m_TRT_ID;
	Trk::CylinderSurface* m_trt_outer_surf;

	// private methods
	//----------------------------------
	int extrapolateBetweenHits(const Trk::TrackParameters* start_parameters,
	                           const Trk::Surface& end_surf,
	                           DataVector<const Trk::TrackStateOnSurface>* holes,
	                           const Trk::ParticleHypothesis partHyp = Trk::pion) const;

	void dump_bad_straw_log() const;
        
	DataVector<const Trk::TrackStateOnSurface>::const_iterator
		find_first_trt_hit(const DataVector<const Trk::TrackStateOnSurface>& track_states) const;

	DataVector<const Trk::TrackStateOnSurface>::const_iterator
		find_last_hit_before_trt(const DataVector<const Trk::TrackStateOnSurface>& track_states) const;

	const Trk::Track* addHolesToTrack(
	                                  const Trk::Track& track,
	                                  const DataVector<const Trk::TrackStateOnSurface>* holes) const;
};

#endif // TRT_TrackHoleSearch_TRTTrackHoleSearchTool_h
