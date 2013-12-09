/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CandidateBuilder.h, (c) ATLAS iPatRec
//
// build a Track out of the SpacePoint's forming a TrackCandidate
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFINDER_CANDIDATEBUILDER_H
#define IPATTRACKFINDER_CANDIDATEBUILDER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ICandidateBuilder.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ISiClusterProperties;
class ISiDetectorLocator;
class ITrackQuality;
class IiPatFitter;
class TrackEnvelope;

class CandidateBuilder: public AthAlgTool,
			virtual public ICandidateBuilder
{

public:
    CandidateBuilder		(const std::string& type,
				 const std::string& name,
				 const IInterface*  parent);
    ~CandidateBuilder 		(void);
    
    StatusCode			initialize();
    StatusCode			finalize();

    // build track from perigee and 2 or 3 space points
    const Track&     	twoPointTrack (PerigeeParameters*	perigee,
				       const Point&		point1,
				       const Point&		point2,
				       bool			pixelInterchange,
				       const Trk::RecVertex&	vertex);
    const Track&	threePointTrack (const Point&  	point1,
					 const Point&  	point2,
					 const Point&  	point3);
    const Track&	threePointTrack (PerigeeParameters*	perigee,
					 const Point&		point1,
					 const Point&		point2,
					 const Point&		point3);
    const Track&	threePointTrack (const PerigeeParameters& 	perigee,
					 const Point&			point1,
					 const Point&			point2,
					 const Point&		  	point3,
					 bool				pixelInterchange);

private:    
    void 		hitsFromPixel (const Point&);
    void 		hitsFromPoint (const Point&);

    // helpers, managers, tools
    ToolHandle<ISiClusterProperties>	m_clusterProperties;
    ToolHandle<ISiDetectorLocator>	m_detectorLocator;
    ToolHandle<IiPatFitter>		m_trackFitter;
    ToolHandle<ITrackQuality>		m_trackQuality;

    // state
    TrackEnvelope*			m_envelopePP;
    TrackEnvelope*			m_envelopePPP;
    TrackEnvelope*			m_envelopePPS;
    TrackEnvelope*			m_envelopePS;
    TrackEnvelope*			m_envelopePSI;
    TrackEnvelope*			m_envelopePSS;
    TrackEnvelope*			m_envelopeSS;
    TrackEnvelope*			m_envelopeSSS;
    hit_iterator			m_hitIterator;
    hit_list*				m_hits;
    hit_list*				m_hitsPP;
    hit_list*				m_hitsPPP;
    hit_list*				m_hitsPPS;
    hit_list*				m_hitsPS;
    hit_list*				m_hitsPSI;
    hit_list*				m_hitsPSS;
    hit_list*				m_hitsSS;
    hit_list*				m_hitsSSS;
};

#endif // IPATTRACKFINDER_CANDIDATEBUILDER_H
