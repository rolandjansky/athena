/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 the iPatTrackFinder InnerDetector pattern recognition algorithm
 - finds all tracks satisfying quality specifications above ptMin
 ----------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKFINDER_TRACKFINDER_H
# define IPATTRACKFINDER_TRACKFINDER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ITrackFinder.h"
#include "iPatTrack/TrackList.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk	{ class RecVertex; }
class IAssignedHits;
class IBeamCondSvc;
class ICombinationMaker;
class ICandidateBuilder;
class IFinderConfiguration;
class ITrackBuilder;
class ITrackFollower;
class ITrackManager;

class TrackFinder: public AthAlgTool,
		   virtual public ITrackFinder
{
    
public:
    TrackFinder			(const std::string& type,
				 const std::string& name,
				 const IInterface* parent);
    ~TrackFinder 		(void);

    StatusCode			initialize();
    StatusCode			finalize();

    void			setProperties (double			maxSecondaryImpact,
					       double			minPt,
					       int			printLevel,
					       const VertexRegion&	vertexRegion);
    
    TrackList::track_list*	trackSearch (const std::vector<Trk::SpacePoint*>& spacePoints);

private:
    typedef	TrackList::track_list		track_list;
    typedef	TrackList::track_citerator 	track_citerator;
    typedef	TrackList::track_iterator      	track_iterator;
    typedef	TrackList::track_riterator 	track_riterator;
   
    void		doublePixelSearch (void);
    void		fastSegmentSearch (void);
    void                searchForPrimaries (void);
    void                searchForSecondaries (void);
    void                searchForVertexRegion (void);
    void		singlePixelSearch (void);
    void		threePointSearch (const Track&);
    void		vertexDefinitionSearch (void);
    void		vertexRegionDefinition (void);

    // helpers, managers, tools
    ToolHandle<IAssignedHits>		m_assignedHits;
    ServiceHandle<IBeamCondSvc>		m_beamCondSvc;
    ToolHandle<ICandidateBuilder>	m_candidateBuilder;
    ToolHandle<ICombinationMaker>	m_combination;
    ToolHandle<IFinderConfiguration>	m_finderTolerances;
    ToolHandle<ITrackBuilder>		m_trackBuilder;
    ToolHandle<ITrackFollower>		m_trackFollower;
    ToolHandle<ITrackManager>		m_trackManager;

    // configuration
    double				m_minPt;
    Amg::Vector3D			m_perigeeVertex;
    int					m_printLevel;
    bool				m_searchForPrimaries;
    bool				m_searchForSecondaries;
    bool				m_searchForVertexRegion;
    Amg::Vector3D			m_vertexRegionCentre;
    Amg::Vector3D			m_vertexRegionWidth;

    // state
    double				m_maxPt;
    Trk::RecVertex*			m_vertex;
    std::vector<double>			m_zVertices;    
};

#endif // IPATTRACKFINDER_TRACKFINDER_H
