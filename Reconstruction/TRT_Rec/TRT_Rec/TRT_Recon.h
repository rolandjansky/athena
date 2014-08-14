/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool for TRT data access and pattern recognition for a single Track.
 adapted from code by Jim Loken
 ***************************************************************************/

#ifndef TRTREC_TRTRECON_H
#define TRTREC_TRTRECON_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ITRT_Recon.h"

class HitOnTrack;
class IDetectorSelection;
class ITRT_DataSelector;
class ITRT_Histogrammer;
class ITRT_Predictor;
class PerigeeParameters;
class TRT_RoadData;
class TrackParameters;
namespace Trk
{
    class IIntersector;
    class IPropagator;
    class IRIO_OnTrackCreator;
}

class TRT_Recon: public AthAlgTool,
		 virtual public ITRT_Recon
{
public:
    TRT_Recon			(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~TRT_Recon			(void);
    
    StatusCode			initialize();
    StatusCode			finalize();

    // track is brem-like (from rPhi deflection)
    bool				bremLike(void) const;
    
    // reconstruction of one track
    const std::vector<HitOnTrack*>*	reconstructSegment(const PerigeeParameters&	perigee, 
							   const TrackParameters&	params,
							   const Trk::TrackParameters&	trkParameters);

private:
    void				makeHitList(const std::vector<TRT_RoadData*>& roadData,
						    const Trk::TrackParameters& trkParameters);

    // helpers, managers, tools
    ToolHandle<ITRT_DataSelector>		m_dataSelector;
    ToolHandle<IDetectorSelection>		m_detectorSelection;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_driftCircleOnTrackCreator;
    ToolHandle<ITRT_Histogrammer>		m_histogrammer;
    ToolHandle<Trk::IIntersector>		m_intersector;
    ToolHandle<ITRT_Predictor>			m_predictor;
    ToolHandle<Trk::IPropagator>		m_propagator;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_strawOnTrackCreator;

    // configuration
    double					m_minFractionExpected;
    int						m_minStraws;
    double					m_muonTrThreshold;
    double					m_sigmaDriftHit;
    double					m_sigmaStrawHit;
    double					m_sigmaWireHit;

    // counters
    int						m_trackBremLike;
    int						m_trackCount;
    int						m_trackTrRich;
    int						m_trackVeto;
    
    // state
    bool					m_bremLike;
    std::vector<HitOnTrack*>			m_hitList;
    double					m_rMax;
    double					m_rPhiMax;

};

#endif // TRTREC_TRTRECON_H
