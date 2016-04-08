/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatRec Algorithm = an athena wrapper to iPatTrackFinder plus initialization
 of the class structure required for track finding, following and fitting.
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATRECALGS_IPATREC_H
# define IPATRECALGS_IPATREC_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatTrack/TrackList.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IDetectorSelection;
class ITRT_DataSelector;
class ITrackFinder;
class ITrackFollower;
class ITrackQuality;
class VertexRegion;
namespace Trk
{
    class ITrackFitter;
    class Track;
}

class iPatRec:public AthAlgorithm
{   

public:

    iPatRec(const std::string& name, ISvcLocator* pSvcLocator);
    ~iPatRec(void);

    StatusCode	initialize(void);    
    StatusCode	execute(void);
    StatusCode	finalize(void);

    Trk::Track*	convertTrack (Track& track);
    
private:
    typedef	TrackList::track_list		track_list;
  
    // helpers, managers, tools
    ToolHandle<IDetectorSelection>		m_detectorSelection;
    ToolHandle<Trk::ITrackFitter>       	m_fitter;
    ToolHandle<ITrackFinder>			m_trackFinder;
    ToolHandle<ITrackFollower>			m_trackFollower;
    ToolHandle<ITrackQuality>			m_trackQuality;
    ToolHandle<ITRT_DataSelector>		m_trtDataSelector;

    // configuration
    double					m_maxSecondaryImpact;
    double					m_minPt;
    std::string				       	m_overlapSpacePointsLocation;
    std::string				        m_pixelClustersLocation;
    std::string				       	m_pixelSpacePointsLocation;
    int 					m_printLevel;
    std::string					m_sctClustersLocation;
    std::string				       	m_sctSpacePointsLocation;
    bool					m_slimOutputTracks;
    std::string					m_spacePointsLocation;
    std::string         			m_tracksLocation;
    std::string			      		m_iPatTracksLocation;
    std::vector<double>				m_vertex;
    const VertexRegion*				m_vertexRegion;
    std::vector<double>				m_vertexWidth;

    // internal communication
    track_list*					m_trackList;
    
    // counters
    unsigned					m_primaryCounter;
    unsigned					m_secondaryCounter;
    unsigned					m_truncatedCounter;
};

#endif // IPATRECALGS_IPATREC_H




